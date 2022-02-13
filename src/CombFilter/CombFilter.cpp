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

