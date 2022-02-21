//
//  CombFilter.cpp
//  CombFilter
//
//  Created by Admin on 2/12/22.
//

#include "CombFilter.h"
#include <iostream>


//CCombFilterBase::CCombFilterBase()
//{
//}
//
//CCombFilterBase::~CCombFilterBase()
//{
//    this -> reset();
//}

Error_t CCombFilterBase::setGain(float fParamValue)
{
    assert(fParamValue <= 1.0f || fParamValue >= -1.0f );
    mGain = fParamValue;
    return Error_t::kNoError;
}

Error_t CCombFilterBase::setDelay(float fParamValue)
{
    assert(fParamValue >= 0.0f );
    mDelayInSamples = fParamValue;
    return Error_t::kNoError;
}

float CCombFilterBase::getGain()
{
    return mGain;
}

float CCombFilterBase::getDelay()
{
    return mDelayInSamples;
}

Error_t CCombFilterBase::setNumOfChannels (int fParamValue)
{
    mNumOfChannels = fParamValue;
    return Error_t::kNoError;
}

Error_t CCombFilterBase::initCombFilter(float fMaxDelayLengthInS, float fSampleRateInHz, int iNumChannels) {

    mDelayInSamples = fMaxDelayLengthInS * fSampleRateInHz;
    pCRingBuff = new CRingBuffer<float>* [iNumChannels];
    setNumOfChannels(iNumChannels);
    setSampleRate(fSampleRateInHz);

    for (int m =0; m < iNumChannels; m++)
    {
        pCRingBuff[m] = new CRingBuffer<float> (mDelayInSamples);
        pCRingBuff[m] -> reset();
    }
    return Error_t::kNoError;
}

int CCombFilterBase::getNumOfChannels() {
    return mNumOfChannels;
}

Error_t CCombFilterBase::setSampleRate(float fSampleRate)
{
    assert(fSampleRate >= 0.0f);
    mSampleRateinHz = fSampleRate;
    return Error_t::kNoError;
}

Error_t CCombFilterFIR::processFilter(float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames)

{

    for  (int m=0; m < getNumOfChannels(); m++)
    {
       for (int n=0; n < iNumberOfFrames ; n++)
       {
           ppfOutputBuffer[m][n] = ppfInputBuffer[m][n] + getGain() * pCRingBuff[m] -> getPostInc();
           pCRingBuff[m] -> putPostInc (ppfInputBuffer [m][n] );
       }
    }
    return Error_t::kNoError;
}


Error_t CCombFilterIIR::processFilter(float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames)
{
    for  (int m=0; m < getNumOfChannels() ; m++)
    {
       for (int n=0; n < iNumberOfFrames; n++)
       {
           ppfOutputBuffer[m][n] = ppfInputBuffer[m][n] + getGain() * pCRingBuff[m] -> getPostInc();
           pCRingBuff[m] -> putPostInc (ppfOutputBuffer [m][n] );
           
       }
    }
    return Error_t::kNoError;
}

