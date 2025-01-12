;;; Perry Cook's physical model of the vocal tract as described in:
;;;
;;; Cook, Perry R. "Synthesis of the Singing Voice Using a Physically Parameterized Model of the Human Vocal Tract"
;;;     Published in the Proceedings of the International Computer Music Conference, Ohio 1989 
;;;     and as Stanford University Department of Music Technical Report Stan-M-57, August 1989.
;;; 
;;; ---- "Identification of Control Parameters in an Articulatory Vocal Tract Model, with Applications 
;;;    to the Synthesis of Singing," Ph.D. Thesis, Stanford University Department of Music Technical Report 
;;;    Stan-M-68, December 1990.
;;;
;;; ----  "SPASM, a Real-time Vocal Tract Physical Model Controller; and Singer, the Companion Software 
;;;    Synthesis System", Computer Music Journal, vol 17 no 1 Spring 1993.
;;;
;;; This code is a translation of Perry Cook's singer implementation originally in C.
;;; Apparently all Perry's data is aimed at srate=22050.
;;;
;;; translated from CLM singer.ins

(provide 'snd-singer.scm)
(if (provided? 'snd)
    (if (not (provided? 'snd-ws.scm)) (load "ws.scm"))
    (if (not (provided? 'sndlib-ws.scm)) (load "sndlib-ws.scm")))

(definstrument (singer beg amp data)
  ;; data is a list of lists very similar to the sequence of synthesize calls in Perry's original implementation.
  ;;    Each imbedded list has the form: dur shape glot pitch glotamp noiseamps vibramt.
  ;;    See below for examples.

  (let* ((setup (car data))
	 (durs (map car data))
	 (dur (apply + durs))
	 (begs (let ((bg beg))
		 (append (list beg)
			 (map (lambda (x)
				(set! bg (+ bg x))
				bg)
			      durs))))
	 (beg-samps (map seconds->samples begs)))

    (let ((change-times (let* ((len (length beg-samps))
			       (nbegs (append beg-samps (list (beg-samps (- len 1))))))
			  (list->vector nbegs)))
	  
	  (shps (map cadr data))
	  (glts (map caddr data))
	  
	  (pfun (let ((initCombFilter (list 0.0 (* .8 (setup 3)))))
		  (for-each (lambda (b dat)
			      (set! initCombFilter (append initCombFilter (list (- b beg))))
			      (set! initCombFilter (append initCombFilter (list (exact->inexact (dat 3))))))
			    (cdr begs)
			    data)
		  initCombFilter))
	  (gfun (let ((initCombFilter (list 0.0 0.0)))
		  (for-each (lambda (b dat)
			      (set! initCombFilter (append initCombFilter (list (- b beg))))
			      (set! initCombFilter (append initCombFilter (list (exact->inexact (dat 4))))))
			    (cdr begs)
			    data)
		  initCombFilter))
	  (nfun (let ((initCombFilter (list 0.0 (exact->inexact (setup 5)))))
		  (for-each (lambda (b dat)
			      (set! initCombFilter (append initCombFilter (list (- b beg))))
			      (set! initCombFilter (append initCombFilter (list (exact->inexact (dat 5))))))
			    (cdr begs)
			    data)
		  initCombFilter))
	  (vfun (let ((initCombFilter (list 0.0 (exact->inexact (setup 6)))))
		  (for-each (lambda (b dat)
			      (set! initCombFilter (append initCombFilter (list (- b beg))))
			      (set! initCombFilter (append initCombFilter (list (exact->inexact (dat 6))))))
			    (cdr begs)
			    data)
		  initCombFilter))
	  (noiseamps (let* ((len (length data))
			    (v (make-vct len 0.0)))
		       (do ((i 0 (+ i 1)))
			   ((= i len))
			 (set! (v i) (exact->inexact ((data i) 5))))
		       v))
	  (tractlength 9))		;length of vocal tract

      (let ((frq-env (make-env pfun :duration dur))
	    (vib-env (make-env vfun :duration dur))
	    (vib-osc (make-oscil 6.0))
	    (glot-env (make-env gfun :duration dur))
	    (noise-env (make-env nfun :duration dur))
	    (ran-vib (make-rand-interp :frequency 10 :amplitude .02))
	    
	    (glot-datai (make-vct (* 2 (length glts)) 0.0))
	    (glot-datar (make-vct (* 2 (length glts)) 0.0))
	    
	    (tractlength+8 (+ tractlength 8))
	    (tractlength+1 (+ tractlength 1))
	    (tractlength-1 (- tractlength 1))
	    (tractlength-2 (- tractlength 2))
	    
	    (noselength 6)
	    (table-size 1000)		; size of glottis wave-table
	    (dpole 0.998)
	    (two-pi (* 2 pi))
	    (bg (seconds->samples beg))
	    (tong-hump-pole 0.998)
	    (tong-tip-pole 0.998))

	(let ((shape-data (make-vct (* (length shps) tractlength+8) 0.0))
	      
	      (noselength-1 (- noselength 1))
	      (noselength-2 (- noselength 2))
	      (nose-ring-time 1000)	; naso pharynx response decay time
	      (table-size-over-sampling-rate (/ table-size (mus-srate)))
	      (dgain (- 1.0 dpole))
	      (tong-hump-gain (- 1.0 tong-hump-pole))
	      (tong-tip-gain (- 1.0 tong-tip-pole))
	      
	      (last-sfd -1)
	      (last-gfd -1)

	      (glot-table (make-vct (+ 1 table-size) 0.0))
	      (glot-table2 (make-vct (+ 1 table-size) 0.0))
	      ;; (gn-table (make-vct (+ 1 table-size) 0.0)) ;(gn-gain 0.0) ;(gn-out 0.0) ;(gn-del (make-vct 4 0.0))
	      ;; (gn-coeffs (make-vct 4 0.0)) ; in Perry's C code, these were set in setGlotNoiseFilter but it was never called!
	      (table-increment 0.0)
	      (glot-refl-gain 0.7)
	      (pitch 400.0)
	      (last-lip-in 0.0)		;for lip reflection/transmission filter
	      (last-lip-out 0.0)
	      (last-lip-refl 0.0)
	      (lip-refl-gain -0.45)
	      (noise-gain 0.0)		;for vocal tract noise generator
	      (noise-input 0.0)
	      (noise-output 0.0)
	      (noisef (make-fir-filter 4 :xcoeffs (make-vct 4)))
	      (noisev #f)
	      (noise-pos 0)
	      (fnoiseamp 0.0)
	      (inz1 0.0)
	      (inz2 0.0)
	      ;; nasal tract acoustic tube structure
	      (nose-coeffs (make-vct noselength 0.0))
	      (nose1 (make-vct noselength 0.0))
	      (nose2 (make-vct noselength 0.0))
	      (velum-pos 0.0)
	      (nose-last-minus-refl 0.0)
	      (nose-last-plus-refl 0.0)
	      (nose-last-output 0.0)
	      (nose-filt 0.0)
	      (nose-filt1 0.0)
	      (time-nose-closed 1000)	; this is a hack used to determine if we need to calculate the nasal acoustics
	      ;; vocal tract acoustic tube structure
	      
	      ;; throat radiation low-pass filter
	      (lt1 0.0)
	      (lp (make-one-pole 0.05 (* -0.05 .9995)))

	      (lip-radius   0.0)
	      (s-glot-mix 0.0)
	      (s-noise 0.0)
	      (initial-noise-position 0.0)
	      (formant-shift 1.0)
	      (change-radii #f) 
	      (delta 0.0) 
	      (new-tract #t)
	      (first-tract #t)
	      (offset -1)
	      (nd (floor (change-times (- (length change-times) 1))))
	      (next-offset bg)

	      (table-location 0.0)
	      (glotsamp 0.0)
	      (last-tract-plus 0.0)
	      (alpha1 0.0)
	      (alpha2 0.0)
	      (alpha3 0.0)
	      (noseposition 3)

	      (target-radii (make-vct tractlength+8 0.0))
	      (radii-poles (make-vct tractlength+8 0.0))
	      (radii-pole-gains (make-vct tractlength+8 0.0))
	      (radii (make-vct tractlength+8 0.0))
					; the radii array contains the vocal tract section radii
					; (tractlength-1 of them), then glottal reflection gain
					; then lip reflection gain, then noise position, then noise gain,
					; then noise pole angle, then noise pole radius, 
					; then noise pole angle2, then noise pole radius2, then velum opening radius
	      (coeffs (make-vct tractlength 0.0))
	      (dline1 (make-vct tractlength 0.0))
	      (dline2 (make-vct tractlength 0.0)))

	  (set! noisev (mus-xcoeffs noisef))

	  (do ((k 0 (+ k 1))
	       (i 0 (+ i tractlength+8)))
	      ((= k (length shps)))
	    (let ((shp (cdr (shps k))))
	      (do ((j i (+ j 1))
		   (m 0 (+ 1 m)))
		  ((= m (length shp)))
		(vct-set! shape-data j (shp m)))))
	  
	  (do ((k 0 (+ k 1))
	       (i 0 (+ i 2)))
	      ((= k (length glts)))
	    (let ((glt (glts k)))
	      (set! (glot-datai i) 0.0)
	      (set! (glot-datai (+ i 1)) (car glt))
	      (set! (glot-datar i) (cadr glt))
	      (set! (glot-datar (+ i 1)) (caddr glt))))
	  
	  (set! (nose-coeffs 0) 0.0)
	  (set! (nose-coeffs 1) -0.29)
	  (set! (nose-coeffs 2) -0.22)
	  (set! (nose-coeffs 3) 0.0)
	  (set! (nose-coeffs 4) 0.24)
	  (set! (nose-coeffs 5) 0.3571)
	  
	  (fill! radii 1.0) ;(do ((i 0 (+ i 1))) ((= i 8)) (set! (radii i) 1.0))
	  (set! (radii 8) 0.7)
	  (set! (radii 9) -0.5)
	  (fill! target-radii 1.0) ;(do ((i 0 (+ i 1))) ((= i 8)) (set! (target-radii i) 1.0))
	  (set! (target-radii 8) 0.7)
	  (set! (target-radii 9) -0.5)
	  
	  (fill! radii-poles dpole) ;(do ((i 0 (+ i 1))) ((= i tractlength+8)) (set! (radii-poles i) dpole))
	  (set! (radii-poles 2) tong-hump-pole)
	  (set! (radii-poles 3) tong-hump-pole)
	  (set! (radii-poles 4) tong-hump-pole)
	  (set! (radii-poles 5) tong-tip-pole)
	  
	  (fill! radii-pole-gains dgain) ;(do ((i 0 (+ i 1))) ((= i tractlength+8)) (set! (radii-pole-gains i) dgain))
	  (set! (radii-pole-gains 2) tong-hump-gain)
	  (set! (radii-pole-gains 3) tong-hump-gain)
	  (set! (radii-pole-gains 4) tong-hump-gain)
	  (set! (radii-pole-gains 5) tong-tip-gain)

	  ;; ---------------- make glot ----------------
	  (let ((harms (floor (glot-datai 1)))
		(temp1 0.0)
		(temp 0.0)
		(sines (make-vct 200 0.0))
		(cosines (make-vct 200 0.0))
		(one-over-two-pi  0.159154943)
		(two-pi-over-table-size (/ two-pi table-size))
		(a (glot-datar 0))
		(b (glot-datar 1)))
	    (let ((a2 (* two-pi a))
		  (b2 (* two-pi b))
		  (b-a (- b a)))
	      (let ((sa2 (sin a2))
		    (ca2 (cos a2)))
		(vct-fill! sines 0.0)
		(vct-fill! cosines 0.0)
		(if (not (= b a))
		    (begin
		      (set! temp (/ one-over-two-pi b-a))
		      (set! temp1 (- 1.0 ca2))
		      (set! (sines 1) (* (+ ca2 (* (- sa2 (sin b2)) temp)) temp1 one-over-two-pi))
		      (set! (cosines 1) (* (+ (- sa2) (* (- ca2 (cos b2)) temp)) temp1 one-over-two-pi))))
		(set! (sines 1) (+ (sines 1) (* (+ 0.75 (- ca2) (* (cos (* 2 a2)) 0.25)) one-over-two-pi)))
		(set! (cosines 1) (+ (cosines 1) (- (* (- sa2 (* (sin (* 2 a2)) 0.25)) one-over-two-pi) (* a 0.5))))
		(do ((k 2 (+ k 1))
		     (ka2 (* 2 a2) (+ ka2 a2))
		     (ka1 a2 (+ ka1 a2))
		     (ka3 (* 3 a2) (+ ka3 a2)))
		    ((> k harms))
		  (if (not (= b a))
		      (begin
			(set! temp (/ one-over-two-pi (* b-a k)))
			(set! (sines k) (* (+ (cos ka2) (* (- (sin ka2) (sin (* k b2))) temp)) (/ temp1 k)))
			(set! (cosines k) (* (+ (- (sin ka2)) (* (- (cos ka2) (cos (* k b2))) temp)) (/ temp1 k)))))
		  (set! (sines k) (+ (sines k) 
				     (/ (- 1.0 (cos ka2)) k)
				     (/ (* (- (cos ka1) 1.0) 0.5) (- k 1))
				     (/ (* (- (cos ka3) 1.0) 0.5) (+ k 1))))
		  (set! (sines k) (* (sines k) one-over-two-pi))
		  (set! (cosines k) (+ (cosines k) (- (/ (sin ka2) k) (/ (* (sin ka1) 0.5) (- k 1)) (/ (* (sin ka3) 0.5) (+ k 1)))))
		  (set! (cosines k) (* (cosines k) one-over-two-pi)))
		(vct-fill! glot-table 0.0)
		(do ((j 0 (+ j 1))
		     (x 0.0 (+ x two-pi-over-table-size)))
		    ((> j table-size))
		  (do ((k 1 (+ k 1))
		       (kx x (+ kx x)))
		      ((> k harms))
		    (vct-set! glot-table j (+ (vct-ref glot-table j) 
					    (* (vct-ref cosines k) (cos kx))
					    (* (vct-ref sines k) (sin kx)))))))))
	  (set! s-glot-mix 1.0)
	  (copy glot-table glot-table2)
	  ;; ---------------- end make glot ----------------

	  
	  (do ((i bg (+ i 1)))
	      ((= i nd))
	    (if (= i next-offset)
		(begin
		  ;; time to check for new tract shapes, glottal pulse shapes etc.
		  (set! offset (+ offset 1))
		  (set! fnoiseamp (noiseamps offset))
		  (if (= last-sfd -1)
		      (set! last-sfd 0)
		      (let ((new-sfd (+ last-sfd 8 tractlength)))
			(do ((j last-sfd (+ j 1))
			     (k new-sfd (+ k 1)))
			    ((= j new-sfd))
			  (if (> (abs (- (shape-data j) (shape-data k))) .001)
			      (set! new-tract #t)))
			(set! last-sfd new-sfd)))
		  (if (= last-gfd -1)
		      (set! last-gfd 0)
		      (let ((new-gfd (+ last-gfd 2)))
			(set! last-gfd new-gfd)))
		  (set! next-offset (floor (change-times (+ offset 1))))
		  (set! delta (/ 1.0 (- next-offset i)))))
	    
	    (if new-tract
		(begin
		  (copy shape-data target-radii last-sfd)

		  (if first-tract
		      (copy target-radii radii))
		  (set! change-radii #f)
		  (set! initial-noise-position (radii tractlength+1))
		  (do ((j 0 (+ j 1)))
		      ((or (= j tractlength+8)
			   change-radii))
		    (if (> (abs (- (target-radii j) (radii j))) 0.001)
			(set! change-radii #t)))))
	    
	    (if (or first-tract change-radii)
		(begin
		  (if (not new-tract)
		      (begin
			(do ((j 0 (+ j 1)))
			    ((= j tractlength+8))
			  (vct-set! radii j (+ (* (vct-ref radii j) (vct-ref radii-poles j))
					     (* (vct-ref target-radii j) (vct-ref radii-pole-gains j)))))))
		  ;; set tract shape
		  (let ((tj 1.0)
			(tk 0.0))
		    (do ((k 0 (+ k 1))
			 (j 1 (+ j 1)))
			((= j tractlength))
		      (set! tk tj)
		      (if (zero? (radii j))
			  (set! tj 1e-10)
			  (set! tj (* (radii k) (radii k))))
		      (set! (coeffs j) (/ (- tk tj) (+ tk tj)))))

		  (set! glot-refl-gain (radii tractlength-1))
		  (set! lip-refl-gain (radii tractlength))
		  (set! noise-pos (floor (radii tractlength+1)))
		  (set! noise-gain (radii (+ tractlength 2)))

		  (let ((temp1 (radii (+ tractlength 3)))
			(r (radii (+ tractlength 4)))
			(t2 (radii (+ tractlength 5)))
			(r2 (radii (+ tractlength 6))))
		    (let (;; fricative noise generator (set noise angle and radius)
			  (noise-angle (hz->radians temp1))
			  (noise-angle2 (hz->radians t2))
			  (noise-radius r)
			  (noise-radius2 r2))
		      (let ((noise-a (* -2.0 (cos (/ noise-angle formant-shift)) noise-radius))
			    (noise-b (* noise-radius noise-radius))
			    (noise-a2 (* -2.0 (cos (/ noise-angle2 formant-shift)) noise-radius2))
			    (noise-b2 (* noise-radius2 noise-radius2)))
			(set! (noisev 0) (+ noise-a noise-a2))
			(set! (noisev 1) (+ noise-b noise-b2 (* noise-a noise-a2)))
			(set! (noisev 2) (+ (* noise-a2 noise-b) (* noise-b2 noise-a)))
			(set! (noisev 3) (* noise-b2 noise-b)))))
		  
		  (set! lip-radius (radii tractlength-2))
		  (set! velum-pos (radii (+ tractlength 7)))
		  (let ((leftradius (radii (- noseposition 2)))
			(velumradius velum-pos)
			(rightradius (radii (- noseposition 1))))
		    (let ((temp1 0.0) 
			  (temp 0.0))
		      ;; nasal tract (set nasal shape)
		      (set! temp (- rightradius velumradius))
		      (if (< temp 0.0) (set! temp 0.0))
		      (set! alpha1 (* leftradius leftradius))
		      (set! alpha2 (* temp temp))
		      (set! alpha3 (* velumradius velumradius))
		      (set! temp1 (/ 2.0 (+ alpha1 alpha2 alpha3)))
		      (set! alpha1 (* alpha1 temp1))
		      (set! alpha2 (* alpha2 temp1))
		      (set! alpha3 (* alpha3 temp1))))))
	    
	    (if new-tract
		(begin
		  (set! new-tract #f)
		  (set! first-tract #f)
		  (if (or (< s-noise 1.0) (< fnoiseamp 0.0001))
		      (set! (target-radii tractlength+1) initial-noise-position))))

	    (set! s-glot-mix (- s-glot-mix delta))
	    (set! s-noise (env noise-env))
	    (set! pitch (env frq-env))
	    (set! table-increment (* pitch (+ 1.0 (* (env vib-env) (oscil vib-osc)) (rand-interp ran-vib)) table-size-over-sampling-rate))
	    (set! last-lip-out (+ last-lip-in last-tract-plus))
	    (set! last-lip-refl (* (+ last-lip-in last-tract-plus) lip-refl-gain))
	    (set! last-lip-in last-tract-plus)
	    ;; next glot tick
	    (set! glotsamp (* (dline2 1) glot-refl-gain))
	    (if (not (= table-increment 0.0))
		(begin
		  (set! table-location (+ table-location table-increment))
		  (if (>= table-location table-size)
		      (set! table-location (- table-location table-size)))
		  (let ((int-loc (floor table-location)))
		    (let ((table1 (glot-table int-loc)))
		      (set! glotsamp (+ glotsamp (* (env glot-env) (+ table1 (* s-glot-mix (- (glot-table2 int-loc) table1))))))))))
	    
	    ;; next tract tick
	    (let ((j 0)
		  (temp1 0.0)
		  (temp (dline2 2)))
	      (set! lt1 (one-pole lp (+ (dline1 2) temp)))

	      (set! (dline2 1) (+ temp (* (coeffs 1) (- glotsamp temp))))
	      (set! temp (+ glotsamp (- (dline2 1) temp)))
	      (do ((j 2 (+ j 1))
		   (k 1 (+ k 1)))
		  ((= j noseposition))
		(let ((x (dline2 (+ j 1))))
		  (set! (dline2 j) (+ x (* (coeffs j) (- (dline1 k) x))))
		  (set! temp1 temp)
		  (set! temp (+ (dline1 k) (- (dline2 j) x)))
		  (set! (dline1 k) temp1)))
	      (set! j noseposition)	;added
	      ;;next nasal tick
	      (let ((plussamp (dline1 (- j 1)))
		    (minussamp (dline2 (+ j 1)))
		    (nose-reftemp 0.0))
		(if (and (= velum-pos 0.0)
			 (>= time-nose-closed nose-ring-time))
		    (let ((nose2-1 (vct-ref nose2 1)))
		      (set! nose-reftemp (+ (* alpha1 plussamp) (* alpha2 minussamp) (* alpha3 nose2-1)))
		      (set! nose-last-minus-refl (- nose-reftemp plussamp))
		      (set! nose-last-plus-refl (- nose-reftemp minussamp)))
		    (begin
		      (if (not (= velum-pos 0.0))
			  (set! time-nose-closed 0)
			  (set! time-nose-closed (+ time-nose-closed 1))) ; added 1 bil 17-Apr-11 but didn't test it
		      ;; nasal tick
		      (let ((nose-reftemp (+ (* alpha1 plussamp) (* alpha2 minussamp) (* alpha3 (nose2 1)))))
			(let ((nose-t1 0.0)
			      (nose-temp 0.0)
			      (plus-in (* velum-pos (- nose-reftemp (nose2 1)))))
			  (set! nose-last-minus-refl (- nose-reftemp plussamp))
			  (set! nose-last-plus-refl (- nose-reftemp minussamp))
			  (set! nose-reftemp (* (nose-coeffs 1) (- plus-in (nose2 2))))
			  (set! (nose2 1) (+ (nose2 2) nose-reftemp))
			  (set! nose-temp (+ plus-in nose-reftemp))
			  (do ((j 2 (+ j 1))
			       (k 1 (+ k 1)))
			      ((= j noselength-1))
			    (set! nose-reftemp (* (nose-coeffs j) (- (nose1 k) (nose2 (+ j 1)))))
			    (set! (nose2 j) (+ (nose2 (+ j 1)) nose-reftemp))
			    (set! nose-t1 nose-temp)
			    (set! nose-temp (+ (nose1 k) nose-reftemp))
			    (set! (nose1 k) nose-t1))
			  (set! nose-reftemp (* (nose-coeffs noselength-1) (- (nose1 noselength-2) (* nose-last-output 0.25))))
			  (set! (nose2 noselength-1) (+ (* nose-last-output 0.25) nose-reftemp))
			  (set! (nose1 noselength-1) (+ (nose1 noselength-2) nose-reftemp))
			  (set! (nose1 noselength-2) nose-temp)
			  (set! nose-filt1 nose-filt)
			  (set! nose-filt (nose1 noselength-1))
			  (set! nose-last-output (* (+ nose-filt nose-filt1) 0.5))))))
		(set! (dline2 j) nose-last-minus-refl))
		
	      (set! temp1 temp)
	      (set! temp nose-last-plus-refl)
	      (set! (dline1 (- j 1)) temp1)
	      (do ((j (+ noseposition 1) (+ j 1))
		   (k noseposition (+ k 1)))
		  ((= j tractlength-1))
		(let ((x (dline2 (+ j 1))))
		  (set! (dline2 j) (+ x (* (coeffs j) (- (dline1 k) x))))
		  (set! temp1 temp)
		  (set! temp (+ (dline1 k) (- (dline2 j) x)))
		  (set! (dline1 k) temp1)))
	      (set! (dline2 tractlength-1) (+ last-lip-refl (* (coeffs tractlength-1) (- (dline1 tractlength-2) last-lip-refl))))
	      (set! (dline1 tractlength-1) (+ (dline1 tractlength-2) (- (dline2 tractlength-1) last-lip-refl)))
	      (set! (dline1 tractlength-2) temp)
	      (if (not (= noise-gain 0.0))
		  (begin
		    (set! noise-input (mus-random 1.0)) ;a guess
		    (set! noise-output (- noise-input inz2 (fir-filter noisef noise-output)))
		    (set! inz2 inz1)
		    (set! inz1 noise-input)
		    (set! (dline1 noise-pos) (+ (dline1 noise-pos) (* noise-output noise-gain s-noise)))))
	      (set! last-tract-plus (* (dline1 tractlength-1) lip-radius)))
	    (outa i (* amp (+ last-lip-out nose-last-output lt1)))
	    ))))))

#|
(with-sound (:statistics #t) (singer 0 .1 (list (list .4 ehh.shp test.glt 523.0 .8 0.0 .01) (list .6 oo.shp test.glt 523.0 .7 .1 .01))))

(with-sound ()
	    (singer 0 .1 (list (list .05 ehh.shp test.glt 523.0 0.8 0.0 .01) 
			       (list .15 ehh.shp test.glt 523.0 0.8 0.0 .01) 
			       (list .05 kkk.shp test.glt 523.0 0.0 0.0 .01) 
			       (list .05 kkk.shp test.glt 523.0 0.0 0.0 .01) 
			       (list .02 kk+.shp test.glt 523.0 0.0 1.0 .01) 
			       (list .08 kk+.shp test.glt 523.0 0.0 0.2 .01) 
			       (list .05 ooo.shp test.glt 523.0 0.8 0.0 .01) 
			       (list .15 ooo.shp test.glt 523.0 0.8 0.0 .01) 
			       (list .05 eee.shp test.glt 523.0 0.8 0.0 .01) 
			       (list .15 eee.shp test.glt 523.0 0.8 0.0 .01) 
			       (list .05 ehh.shp test.glt 523.0 0.8 0.0 .01) 
			       (list .15 ehh.shp test.glt 523.0 0.8 0.0 .01) 
			       (list .05 mmm.shp test.glt 523.0 0.8 0.0 .01) 
			       (list .15 mmm.shp test.glt 523.0 0.8 0.0 .01) 			      
			       (list .10 mmm.shp test.glt 523.0 0.0 0.0 .01) )))
|#

(define test.glt (list 10 .65 .65))
(define loud.glt (list 13 .6 .6))
(define soft.glt (list 13 0.65 0.73))
(define wide4.glt (list 18 0.534 0.56))
(define wide5.glt (list 10 0.65 0.65))
(define greekdefault.glt (list 20 0.65 0.672472))
(define lowbass.glt (list 99 0.5 0.17737593))


(define aa.shp (list 8 0.63110816 0.94615144 1.0756062 0.9254686 0.9928594 0.98307705 1.4507878 0.95167005 0.9 -0.4 1.0 0.0 0.0 0.0 0.0 0.0 0.0))
(define hh2.shp (list 8 0.928177 0.61326 0.39779 0.530387 0.679558 0.961326 1.44199 1.09392 0.7 -0.203125 1.0 0.0 554.1667 0.8 2000.0 0.772222 0.0))
(define dhh.shp (list 8 0.828729 1.45856 0.9882353 0.662983 0.9352941 1.2529411 0.40588236 1.1740758 0.7 -0.140625 7.0 0.023333002 3039.613 0.691692 1264.1677 0.404788 0.0))
(define aah.shp (list 8 0.8214024 0.7839217 1.0981537 0.9937591 0.817757 1.1907763 1.3149668 1.0705689 0.7 -0.4 1.0 0.0 0.0 0.0 0.0 0.0 0.0))
(define hhh.shp (list 8 0.928177 0.61326 0.39779 0.530387 0.679558 0.961326 1.44199 1.09392 0.7 -0.203125 1.0 0.046296295 554.1667 0.8 2000.0 0.7722222 0.0))
(define ohh.shp (list 8 1.02762 0.696133 0.39779 0.513812 0.6371682 1.4070797 1.80663 0.5044248 0.7 -0.2 1.0 0.0 1000.0 0.0 0.0 0.0 0.0))
(define ah.shp (list 8 0.7162393 0.6389201 0.8881412 0.6060006 1.293248 1.4140776 1.8503952 0.8622935 0.9 -0.4 1.0 0.0 0.0 0.0 0.0 0.0 0.0))
(define oo.shp (list 8 0.46043858 1.0865723 0.33916336 0.88724023 0.9989101 1.224445 0.39867023 0.506609 0.9 -0.4 1.0 0.0 0.0 0.0 0.0 0.0 0.0))
(define ahh.shp (list 8 0.928177 0.779006 0.629834 0.629834 1.02762 1.65746 1.62431 0.944751 0.7 -0.45 1.0 0.0 0.0 0.0 0.0 0.0 0.0))
(define ee-.shp (list 8 0.928177 1.37569 1.37569 0.679558 0.629834 0.24817872 0.56896555 0.662983 0.7 -0.403125 1.0 0.0 0.0 0.0 0.0 0.0 0.09677419))
(define hoo.shp (list 8 1.32597 1.29282 0.39779 0.530387 1.32597 1.34254 1.78182 0.46408796 0.7 -0.4 1.0 0.031045755 2215.7856 0.82698005 1026.6984 0.96960765 0.0))
(define ooo.shp (list 8 1.32597 1.29282 0.39779 0.530387 1.32597 1.34254 1.78182 0.464088 0.7 -0.4 1.0 0.0 0.0 0.0 0.0 0.0 0.0))
(define ee.shp (list 8 1.02 1.637 1.67 1.558 0.952 0.501 0.681 0.675 0.9 -0.4 1.0 0.0 0.0 0.0 0.0 0.0 0.0))
(define ih.shp (list 8 0.72092783 1.2719809 1.3881364 0.6532612 0.7501422 0.65654784 0.8194081 0.6556785 0.9 -0.4 1.0 0.0 0.0 0.0 0.0 0.0 0.0))
(define ee2.shp (list 8 0.9180887 1.3481673 1.3433423 0.74573994 0.593326 0.5647744 0.6692766 0.7419633 0.7 -0.405254 1.0 0.0 0.0 0.0 0.0 0.0 0.0))
(define ihh.shp (list 8 0.7906788 1.272475 1.4089537 0.68072784 0.62673146 0.7479623 0.7506758 0.7054355 0.9 -0.4 1.0 0.0 0.0 0.0 0.0 0.0 0.0))
(define open.shp (list 8 3.0 3.0 3.0 3.0 3.0 3.0 3.0 3.0 0.7 -0.45 1.0 0.0 0.0 0.0 1.0 0.0 0.0))
(define thh.shp (list 8 0.828729 1.45856 0.9882353 0.662983 0.9352941 1.2529411 0.40588236 1.1740758 0.7 -0.140625 7.0 0.101764 3039.613 0.691692 1264.1677 0.404788 0.0))
(define aw.shp (list 8 1.0525645 0.643587 0.935229 0.4901642 1.0743295 1.1822895 1.4161918 0.82537806 0.9 -0.4 1.0 0.0 0.0 0.0 0.0 0.0 0.0))
(define eee.shp (list 8 0.928177 1.37569 1.37569 0.679558 0.629834 0.646409 0.56896555 0.662983 0.7 -0.403125 1.0 0.0 0.0 0.0 0.0 0.0 0.0))
(define tt+.shp (list 8 0.928177 0.779006 0.629834 0.629834 1.02762 0.18584079 1.62431 0.944751 0.7 -0.45 6.0 0.388889 10514.583 0.854335 1315.2043 0.280428 0.0))
(define aww.shp (list 8 1.02762 0.696133 0.563536 0.513812 0.977901 1.37569 1.80663 0.712707 0.7 -0.2 1.0 0.0 1000.0 0.0 0.0 0.0 0.0))
(define eee2.shp (list 8 0.928177 1.37569 1.37569 0.679558 0.629834 0.646409 0.5117647 0.662983 0.7 -0.203125 7.3688526 0.0 5214.53 0.975806 0.0 0.0 0.0))
(define jjj.shp (list 8 0.928177 0.779006 0.629834 0.629834 1.02762 0.1592921 1.1464338 0.944751 0.7 -0.45 6.0 0.098039 2315.7278 0.7089554 3066.7 0.7983351 0.0))
(define ttt.shp (list 8 0.928177 0.779006 0.629834 0.629834 1.02762 0.0 1.62431 0.944751 0.7 -0.45 6.0 0.388889 10514.583 0.854335 1315.2043 0.280428 0.0))
(define bb2.shp (list 8 1.0 1.0 0.46902645 0.5486725 0.65486723 1.079646 1.3982301 0.0 0.7 -0.2 8.0 0.03 500.0 0.98 0.0 0.0 0.0))
(define eh.shp (list 8 0.7866194 1.1630946 1.2335452 0.93186677 0.94121367 0.7586716 1.3509308 0.8279036 0.9 -0.4 1.0 0.0 0.0 0.0 0.0 0.0 0.0))
(define kk+.shp (list 8 0.8214024 0.7839217 1.0981537 0.1592921 1.061947 1.1907763 1.3149668 1.0705689 0.7 -0.4 4.0 0.4 2000.0 0.93 0.0 0.0 0.0))
(define pipe1.shp (list 8 1.0 1.0 1.0 0.7 0.7 0.7 0.7 0.7 0.0 0.0 1.0 0.0 100.0 0.0 0.0 0.0 0.0))
(define tzz.shp (list 8 0.828729 1.45856 0.9882353 0.662983 0.9352941 1.2529411 0.40588236 1.1740758 0.7 -0.140625 7.0 0.101764 3039.613 0.691692 1264.1677 0.404788 0.0))
(define bbb.shp (list 8 1.0 1.0 0.46902645 0.5486725 0.65486723 1.079646 1.3982301 0.0 0.7 -0.2 8.0 0.03 500.0 0.98 0.0 0.0 0.0))
(define ehh.shp (list 8 0.682 1.554 1.581 1.367 1.315 1.579 0.843 1.476 0.7 -0.24507 1.0 0.0 0.0 0.0 0.0 0.0 0.0))
(define kk2.shp (list 8 0.82140243 0.7839217 1.0981537 0.0 1.061947 1.1907763 1.3149668 1.0705689 0.7 -0.4 5.0 0.01 2000.0 0.93 0.0 0.0 0.0))
(define pp+.shp (list 8 1.0 1.0 0.3362832 0.49557513 0.7079646 1.2389379 1.1327434 0.29203534 0.7 -0.2 8.0 0.040740736 0.0 0.89649165 2082.2144 0.8713607 0.0))
(define uhh.shp (list 8 0.928177 0.61326 0.39779 0.530387 0.679558 0.961326 1.44199 1.09392 0.7 -0.203125 1.0 0.0 0.0 0.0 0.0 0.0 0.0))
(define big.shp (list 8 3.0 3.0 3.0 3.0 3.0 3.0 3.0 3.0 0.0 0.0 1.0 0.0 0.0 0.0 1.0 0.0 0.0))
(define euu.shp (list 8 0.9285748 1.3756071 1.3747121 0.6794088 0.60398144 0.43471563 0.8356653 0.7158814 0.7 -0.403122 1.0 0.0 0.0 0.0 0.0 0.0 0.0))
(define kkk.shp (list 8 0.8214024 0.7839217 1.0981537 0.0 1.061947 1.1907763 1.3149668 1.0705689 0.7 -0.4 4.0 0.09444445 2000.0 0.93 0.0 0.0 0.0))
(define ppp.shp (list 8 1.0 1.0 0.3362832 0.49557513 0.7079646 1.2389379 1.1327434 0.0 0.7 -0.2 8.0 0.05 500.0 0.98 0.0 0.0 0.0))
(define uu.shp (list 8 0.45291674 1.0539645 0.39576897 0.8116293 1.0510263 1.1789232 0.47529656 0.62563825 0.9 -0.4 1.0 0.0 0.0 0.0 0.0 0.0 0.0))
(define fff.shp (list 8 0.93787295 0.70496833 0.8969878 0.60815966 0.9375178 0.7412625 1.1285298 0.2665695 0.7 -0.202603 8.0 0.10341219 8236.909 0.945306 79.28094 0.498648 0.0))
(define ll2.shp (list 8 0.928177 0.779006 0.71772796 0.807417 1.02762 1.65746 0.36206907 0.86510503 0.7 -0.258055 1.0 0.0 0.0 0.0 0.0 0.0 0.20806663))
(define uuu.shp (list 8 0.55 0.943094 1.035 0.434071 1.14681 1.487 0.555 0.656 0.9 -0.4 1.0 0.0 0.0 0.0 0.0 0.0 0.0))
(define lll.shp (list 8 0.928177 0.779006 0.7330638 0.8156748 1.02762 1.65746 0.3620689 0.944751 0.7 -0.103125 1.0 0.0 0.0 0.0 0.0 0.0 0.21774194))
(define rolledr.shp (list 8 0.3365169 0.9244819 1.0542682 0.4485168 1.0597233 0.054845095 0.66896766 0.8336522 0.9 -0.4 1.0 0.0 0.0 0.0 0.0 0.0 0.0))
(define vvv.shp (list 8 0.9400966 0.6775904 0.88759726 0.59890866 0.9485658 0.737778 1.1542239 0.23893797 0.7 -0.2 8.0 0.5 8500.0 0.95 0.0 0.5 0.0))
(define rolledrc.shp (list 8 0.928177 0.779006 0.629834 0.629834 1.02762 0.0 1.62431 0.944751 0.7 -0.45 1.0 0.0 0.0 0.0 0.0 0.0 0.0))
(define mmm.shp (list 8 1.0 1.0 1.0 1.0 1.0 1.0 1.0 0.0 0.7 -0.2 1.0 0.0 0.0 0.0 0.0 0.0 0.503268))
(define rolledro.shp (list 8 0.928177 0.779006 0.629834 0.629834 1.02762 0.42477876 1.62431 0.944751 0.7 -0.45 1.0 0.0 0.0 0.0 0.0 0.0 0.0))
(define breath.shp (list 8 0.928177 0.779006 0.629834 0.629834 1.02762 1.65746 1.62431 0.944751 0.7 -0.45 1.0 0.018518519 2588.6013 0.90612125 812.6343 0.9814815 0.0))
(define moo.shp (list 8 1.32597 1.29282 0.39779 0.530387 1.32597 1.34254 1.78182 0.0 0.7 -0.4 1.0 0.0 0.0 0.0 0.0 0.0 0.30645162))
(define rr2.shp (list 8 0.3365169 0.9244819 1.0542682 0.4485168 1.0597233 0.71856207 0.66896766 0.7274576 0.9 -0.4 1.0 0.0 0.0 0.0 0.0 32.000004 0.0))
(define chh.shp (list 8 0.928177 0.779006 0.629834 0.629834 1.02762 0.1592921 1.1464338 0.944751 0.7 -0.45 6.0 0.098039 2315.7278 0.7089554 3066.7 0.7983351 0.0))
(define gg2.shp (list 8 0.8214024 0.4122405 0.40788835 0.0 0.8495575 0.7129002 0.7308959 0.7785335 0.7 -0.4 4.0 0.05 2000.0 0.9 0.0 0.0 0.0))
(define nng.shp (list 8 1.0 1.0 1.0333333 0.0 1.0 0.99999994 0.9568965 1.3189656 0.7 -0.2 1.0 0.0 0.0 0.0 0.0 0.0 1.0))
(define rrr.shp (list 8 0.3365169 0.9244819 1.0542682 0.4485168 1.0597233 0.71856207 0.66896766 0.7274576 0.9 -0.4 1.0 0.0 0.0 0.0 0.0 0.0 0.0))
(define wsp.shp (list 8 0.928177 0.779006 0.629834 0.629834 1.02762 1.65746 1.62431 0.944751 0.7 -0.45 1.0 0.018518519 0.0 0.97 0.0 0.0 0.0))
(define ggg.shp (list 8 0.8214024 0.7839217 1.0981537 0.0 0.8495575 0.7129002 0.7308959 0.7785335 0.7 -0.4 4.0 0.05 2000.0 0.9 0.0 0.0 0.0))
(define nnn.shp (list 8 1.0 1.0 1.0 1.4579439 1.0 0.0 0.9568965 1.3189656 0.7 -0.2 1.0 0.0 0.0 0.0 0.0 0.0 0.503268))
(define sh2.shp (list 8 0.828729 1.45856 0.9882353 0.662983 0.9352941 1.2529411 0.40588236 0.9882353 0.7 -0.140625 7.0 0.0 2451.5984 0.928097 2957.0518 0.883636 0.0))
(define xx2.shp (list 8 0.928177 1.37569 1.37569 0.8495575 0.3451327 0.646409 0.56896555 0.662983 0.7 -0.403125 5.0 0.022222 2102.0833 0.805556 1735.4166 0.759259 0.0))
(define dd2.shp (list 8 0.928177 0.779006 0.629834 0.629834 1.02762 0.0 0.72165513 0.5996184 0.7 -0.45 6.0 0.02 4851.6665 0.953704 2500.0 0.966296 0.0))
(define ggg1.shp (list 8 0.8214024 0.7839217 1.0981537 0.18584079 1.061947 1.1907763 1.3149668 1.0705689 0.7 -0.4 4.0 0.4 2000.0 0.9 0.0 0.0 0.0))
(define noisahh.shp (list 8 0.928177 0.779006 0.629834 0.629834 1.02762 1.65746 1.62431 0.944751 0.7 -0.45 1.0 0.005 0.0 0.787037 3777.0835 0.759259 0.0))
(define shh.shp (list 8 0.828729 1.45856 0.9882353 0.662983 0.9352941 1.2529411 0.40588236 0.9882353 0.7 -0.140625 7.0 0.023333 2451.5984 0.9280972 2957.0518 0.88363576 0.0))
(define xxx.shp (list 8 0.928177 1.37569 1.37569 0.3451327 0.6371682 0.646409 0.56896555 0.662983 0.7 -0.403125 4.0 0.022222219 2102.0833 0.8055556 612.5 0.7592593 0.0))
(define ddd.shp (list 8 0.928177 0.779006 0.629834 0.629834 1.02762 0.0 0.72165513 0.5996184 0.7 -0.45 6.0 0.02 4851.6665 0.953704 2500.0 0.966296 0.0))
(define gxx.shp (list 8 0.928177 1.37569 1.37569 0.3451327 0.6371682 0.646409 0.56896555 0.662983 0.7 -0.403125 4.0 0.022222 2102.0833 0.805556 612.5 0.759259 0.0))
(define none.shp (list 8 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 0.0 0.0 1.0 0.0 0.0 0.0 0.0 0.0 0.0))
(define sss.shp (list 8 0.928177 1.3588235 1.3588235 0.679558 0.61764705 0.63529414 0.31764707 0.65294117 0.7 -0.103125 7.0 0.105292 1500.0 0.916452 4943.75 0.97222227 0.0))
(define zzz.shp (list 8 0.928177 1.3588235 1.3588235 0.679558 0.61764705 0.63529414 0.31764707 0.65294117 0.7 -0.103125 7.0 0.016 1500.0 0.9257112 4943.75 0.925926 0.0))
