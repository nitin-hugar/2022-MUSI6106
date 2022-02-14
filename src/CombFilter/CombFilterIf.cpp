
// standard headers

// project headers
#include "MUSI6106Config.h"

#include "ErrorDef.h"
#include "Util.h"

#include "CombFilterIf.h"
#include "CombFilter.h" // Header file that contains FIR and IIR filters

static const char*  kCMyProjectBuildDate = __DATE__;


CCombFilterIf::CCombFilterIf () :
    m_bIsInitialized(false),
    m_pCCombFilter(0),
    m_fSampleRate(0)
{
    // this should never hurt
    this->reset ();
}


CCombFilterIf::~CCombFilterIf ()
{
    this->reset ();
}

const int  CCombFilterIf::getVersion (const Version_t eVersionIdx)
{
    int iVersion = 0;

    switch (eVersionIdx)
    {
    case kMajor:
        iVersion    = MUSI6106_VERSION_MAJOR; 
        break;
    case kMinor:
        iVersion    = MUSI6106_VERSION_MINOR; 
        break;
    case kPatch:
        iVersion    = MUSI6106_VERSION_PATCH; 
        break;
    case kNumVersionInts:
        iVersion    = -1;
        break;
    }

    return iVersion;
}
const char*  CCombFilterIf::getBuildDate ()
{
    return kCMyProjectBuildDate;
}

Error_t CCombFilterIf::create (CCombFilterIf*& pCCombFilter)
{
    pCCombFilter = new CCombFilterIf () ;
    return Error_t::kNoError;
}

Error_t CCombFilterIf::destroy (CCombFilterIf*& pCCombFilter)
{
    delete pCCombFilter;
    pCCombFilter = NULL;
    return Error_t::kNoError;
}

Error_t CCombFilterIf::init (CombFilterType_t eFilterType, float fMaxDelayLengthInS, float fSampleRateInHz, int iNumChannels)
{

    switch(eFilterType)
    {
        case(kCombFIR):
            m_pCCombFilter -> init(fMaxDelayLengthInS, fSampleRateInHz, iNumChannels);

            return Error_t::kNoError;
            break;
        
        case (kCombIIR):
            // Run IIR Filter
            return Error_t::kNoError;
            break;
        
        default:
            // Run FIR Filter
            return Error_t::kNoError;
            break;
    }
    return Error_t::kNoError;
}

Error_t CCombFilterIf::reset ()
{
    if (!m_bIsInitialized)
    {
        init(CCombFilterIf::CombFilterType_t::kCombFIR, 1.0f, 48000.0f, 2);
        setParam(FilterParam_t::kParamGain, 1.0f);
        setParam(FilterParam_t::kParamDelay, 0);
    }
    return Error_t::kNoError;
}

Error_t CCombFilterIf::process (float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames)
{
    return Error_t::kNoError;
}

Error_t CCombFilterIf::setParam (FilterParam_t eParam, float fParamValue)
{
    switch (eParam)
    {
        case (FilterParam_t::kParamGain):
            m_pCCombFilter -> init();
            break;
            
        case (FilterParam_t::kParamDelay):
            m_pCCombFilter -> setDelay(fParamValue);
            break;
        
        default:
            m_pCCombFilter -> setGain(1.0f);
            m_pCCombFilter -> setDelay (0.0f);
            break;
    
    }
    return Error_t::kNoError;
}

float CCombFilterIf::getParam (FilterParam_t eParam) const
{
    switch (eParam)
    {
        case (FilterParam_t::kParamGain):
            return m_pCCombFilter -> getGain();
            break;
            
        case (FilterParam_t::kParamDelay):
            return m_pCCombFilter -> getDelay();
            break;
            
        default:
            return 0;
            break;
    }
}
