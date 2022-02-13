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
    this -> initCombFilter(2, 1.0, 0.0); // Change this to point to something
}


Error_t CCombFilterBase::setGain(float fParamValue)
{
    mGain = fParamValue;
}

Error_t CCombFilterBase::setDelay(float fParamValue)
{
    mDelay = fParamValue;
}

float CCombFilterBase::getGain()
{
    return mGain;
}

float CCombFilterBase::getDelay()
{
    return mDelay;
}

Error_t CCombFilterBase::setNumOfChannels (int fParamValue)
{
    mNumOfChannels = fParamValue;
}



Error_t CCombFilterBase::initCombFilter(int iNumOfChannels, float kParamGain, float kParamDelay)
{
    pCRingBuff = new CRingBuffer<float>* [iNumOfChannels];
    setNumOfChannels(iNumOfChannels);
    setGain(kParamGain);
    setDelay(kParamDelay);
    mDelay = kParamDelay;
}


Error_t CCombFilterFIR::process(float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames)

{
    for (int m; m < getNumOfChannels(); m++)
    {
        for (int n; n < iNumberOfFrames; n++ )
        {
            pCRingBuff[m] -> putPostInc(0.F * n);
        }
    }
    
    for (int m; m < getNumOfChannels(); m++)
    {
        pCRingBuff[m] -> setReadIdx(0);
    }
    for  (int m=0; m < getNumOfChannels(); m++)
    {
       for (int n=0; n< iNumberOfFrames; n++)
       {
           ppfOutputBuffer[m][n] = ppfInputBuffer[m][n] + CCombFilterBase:: getGain() * pCRingBuff[m] -> getPostInc();
           pCRingBuff[m] -> putPostInc (ppfInputBuffer [m][n] );
           
       }
    }
}


Error_t CCombFilterIIR::process(float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames)
{
    for (int m; m < getNumOfChannels(); m++)
    {
        for (int n; n < iNumberOfFrames; n++ )
        {
            pCRingBuff[m] -> putPostInc(0.F * n);
        }
    }
    
    for (int m; m < getNumOfChannels() ; m++)
    {
        pCRingBuff[m] -> setReadIdx(0);
    }
    for  (int m=0; m < getNumOfChannels() ; m++)
    {
       for (int n=0; n< iNumberOfFrames; n++)
       {
           ppfOutputBuffer[m][n] = ppfInputBuffer[m][n] + CCombFilterBase:: getGain() * pCRingBuff[m] -> getPostInc();
           pCRingBuff[m] -> putPostInc (ppfOutputBuffer [m][n] );
           
       }
    }
}

