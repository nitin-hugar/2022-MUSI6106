//
//  CombFilter.cpp
//  CombFilter
//
//  Created by Admin on 2/12/22.
//

#include "CombFilter.h"
#include <iostream>

CCombFilterBase :: CCombFilterBase()
{
}


Error_t CCombFilterBase::setGain(float fParamValue)
{
    mGain = fParamValue;
}

Error_t CCombFilterBase::setDelay(float fParamValue)
{
    mDelayInSamples = fParamValue;
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
}

Error_t CCombFilterBase::init(float fMaxDelayLengthInS, float fSampleRateInHz, int iNumChannels) {

    pCRingBuff = new CRingBuffer<float>* [iNumChannels];
    setNumOfChannels(iNumChannels);
    CCombFilterIf::setParam(CCombFilterIf::FilterParam_t::kParamDelay, fMaxDelayLengthInS * fSampleRateInHz );

    for (int m =0; m < getNumOfChannels(); m++)
    {
        for (int n = 0; n < getDelay(); n++) {
            pCRingBuff[m]->putPostInc(0.F * n);
        }
    }
    for (int i = 0; i < getNumOfChannels(); i++)
    {
        pCRingBuff[i]->setReadIdx(0);
    }

}

int CCombFilterBase::getNumOfChannels() {
    return mNumOfChannels;
}




Error_t CCombFilterFIR::process(float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames)

{

    for  (int m=0; m < getNumOfChannels(); m++)
    {
       for (int n=0; n < iNumberOfFrames ; n++)
       {
           ppfOutputBuffer[m][n] = ppfInputBuffer[m][n] + CCombFilterBase:: getGain() * pCRingBuff[m] -> getPostInc();
           pCRingBuff[m] -> putPostInc (ppfInputBuffer [m][n] );
       }
    }
}


Error_t CCombFilterIIR::process(float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames)
{
    for  (int m=0; m < getNumOfChannels() ; m++)
    {
       for (int n=0; n < iNumberOfFrames; n++)
       {
           ppfOutputBuffer[m][n] = ppfInputBuffer[m][n] + CCombFilterBase:: getGain() * pCRingBuff[m] -> getPostInc();
           pCRingBuff[m] -> putPostInc (ppfOutputBuffer [m][n] );
           
       }
    }
}

