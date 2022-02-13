//
//  CombFilter.cpp
//  CombFilter
//
//  Created by Admin on 2/12/22.
//

#include "CombFilter.h"
#include <iostream>


Error_t CCombFilterBase::setGain(float fParamValue)
{
    kParamGain = fParamValue;
}

Error_t CCombFilterBase::setDelay(float fParamValue)
{
    kParamDelay = fParamValue;
}

float CCombFilterBase::getGain()
{
    return kParamGain;
}

float CCombFilterBase::getDelay()
{
    return kParamDelay;
}

Error_t CCombFilterFIR::process(float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames)

for (int m; m < iNumOfChannels; m++)
{
    for (int n; n < iNumOfFrames; n++ )
    {
        mRingBufferPtr[m] -> putPostInc()
    }
}


{
    for  (int m=0; m < mNumOfChannels; m++)
       {
           for (int n=0; n< iNumberOfFrames; n++)
           {
               ppfOutputBuffer[m][n] = ppfInputBuffer[m][n] + CCombFilterBase::kParamGain * mRingBufferPtr[m] -> getPostInc();
               mRingBufferPtr[m] -> putPostInc (ppfInputBuffer [m][n] );
               
           }
       }
}

Error_t CCombFilterIIR::process(float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames)
{
    for (int m=0; m< iNumOfChannels; m++)
    {
        for (int n=0; n < iNumberOfFrames; n++ )
        {
            
        }
    }
}

