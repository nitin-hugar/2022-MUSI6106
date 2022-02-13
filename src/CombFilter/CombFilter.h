//
//  CombFilter.h
//  CombFilter
//
//  Created by Admin on 2/12/22.
//

#include "CombFilterIf.h"
#include "RingBuffer.h"

class CCombFilterBase
{
public:
    
    Error_t initCombFilter(int iNumOfChannels, float                    kParamGain, float kParamDelay);

    Error_t setGain(float fParamValue);
    Error_t setNumOfChannels (int fParamValue);
    Error_t setDelay(float fParamValue);
    
    
    float getGain();
    int getNumOfChannels();
    float getDelay();

protected:
    CCombFilterBase();
    ~CCombFilterBase ();
    CRingBuffer<float>** pCRingBuff;
    
private:
    float mGain;
    int mDelay;
    int mNumOfChannels;
    int mSampleRateinHz;
};


class CCombFilterFIR : public CCombFilterBase
{
public:
    Error_t process(float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames);
    
private:
};

class CCombFilterIIR : public CCombFilterBase
{
public:
    Error_t process(float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames);
private:
};
