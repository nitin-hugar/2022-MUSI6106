//
//  CombFilter.h
//  CombFilter
//
//  Created by Admin on 2/12/22.
//

#include "CombFilterIf.h"

class CCombFilterBase : public CCombFilterIf
{
public:
    Error_t init(CombFilterType_t eFilterType, float fMaxDelayLengthInS, float fSampleRateInHz, int iNumChannels);
    
    Error_t reset ();
    
    Error_t process(float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames);
    
    Error_t setParam (FilterParam_t eParam, float fParamValue);

    float getParam (FilterParam_t eParam);

protected:
    
    int kParamGain = 0;
    float kParamDelay = 0.0f;
};

class CCombFilterFIR: public CCombFilterBase
{
public:
private:
};

class CCombFilterIIR: public CCombFilterBase
{
public:
private:
};
