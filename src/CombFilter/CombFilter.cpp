//
//  CombFilter.cpp
//  CombFilter
//
//  Created by Admin on 2/12/22.
//

#include "CombFilter.h"


Error_t CCombFilterBase::init(CombFilterType_t eFilterType, float fMaxDelayLengthInS, float fSampleRateInHz, int iNumChannels)
{
    switch (eFilterType)
    {
        case kCombFIR:
            //
            break;
        case kCombIIR:
            
            break;
        default:
            // select FIR by default
            break;
    }
}

Error_t CCombFilterBase::reset()
{
    CCombFilterBase::init(CombFilterType_t::kCombFIR, 2, 48000.0f, 2); //Set the initials to pre-decided values
}

Error_t CCombFilterBase::process(float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames)
{
    //
}

Error_t CCombFilterBase::setParam(FilterParam_t eParam, float fParamValue)
{
    switch (eParam)
    {
        case FilterParam_t::kParamGain:
            CCombFilterBase::kParamGain = fParamValue;
            return Error_t::kNoError;
            break;
        
        case FilterParam_t::kParamDelay:
            CCombFilterBase::kParamDelay = fParamValue;
            return Error_t::kNoError;
            break;
    }
}


float CCombFilterBase::getParam(FilterParam_t eParam)
    {
    switch (eParam)
    {
        case FilterParam_t::kParamGain:
            return CCombFilterBase::kParamGain;
            break;
        
        case FilterParam_t::kParamDelay:
            return CCombFilterBase::kParamDelay;
            break;
    }
}
