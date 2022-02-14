//
//  CombFilter.h
//  CombFilter
//
//  Created by Admin on 2/12/22.
//

#include "CombFilterIf.h"
#include "RingBuffer.h"

class CCombFilterBase : public CCombFilterIf
{
public:

    Error_t initCombFilter(float fMaxDelayLengthInS, float fSampleRateInHz, int iNumChannels);
    Error_t setGain(float fParamValue);
    Error_t setNumOfChannels (int fParamValue);
    Error_t setDelay(float fParamValue);
    Error_t setSampleRate(float fSampleRate);
    virtual Error_t processFilter(float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames) = 0;

    float getGain();
    int getNumOfChannels();
    float getDelay();

protected:
//    CCombFilterBase();
//    ~CCombFilterBase ();
    CRingBuffer<float>** pCRingBuff;
private:
    float mGain;
    float mDelayInSamples;
    int mNumOfChannels;
    float mSampleRateinHz;
};


class CCombFilterFIR : public CCombFilterBase
{
public:
    Error_t processFilter(float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames) override;
};

class CCombFilterIIR : public CCombFilterBase
{
public:
    Error_t processFilter(float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames) override;
};
