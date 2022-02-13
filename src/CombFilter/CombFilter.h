//
//  CombFilter.h
//  CombFilter
//
//  Created by Admin on 2/12/22.
//

#include "CombFilterIf.h"

class CCombFilterBase
{
public:
    CCombFilterBase (int DelayLengthIns, int iNumChannels, float gain=0.2)
    {
        
    }
    
    
    Error_t setGain(float fParamValue);
    Error_t setDelay(float fParamValue);
    float getGain();
    float getDelay();

private:
    
    float kParamGain;
    float kParamDelay;
};


class CCombFilterFIR : public CCombFilterBase
{
public:
private:
};

class CCombFilterIIR : public CCombFilterBase
{
public:
private:
};
