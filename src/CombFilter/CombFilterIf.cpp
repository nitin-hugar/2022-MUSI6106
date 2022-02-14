
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
    pCCombFilter -> reset();
    delete pCCombFilter;
    pCCombFilter = nullptr;
    return Error_t::kNoError;
}

Error_t CCombFilterIf::init (CombFilterType_t eFilterType, float fMaxDelayLengthInS, float fSampleRateInHz, int iNumChannels)
{

    if (eFilterType == kCombFIR)
    {
        m_pCCombFilter = new CCombFilterFIR;
        m_pCCombFilter -> initCombFilter(fMaxDelayLengthInS, fSampleRateInHz, iNumChannels);
    }
    else if (eFilterType == kCombIIR)
    {
        m_pCCombFilter = new CCombFilterIIR;
        m_pCCombFilter -> initCombFilter(fMaxDelayLengthInS, fSampleRateInHz, iNumChannels);
    }

    m_bIsInitialized = true;
    return Error_t::kNoError;
}

Error_t CCombFilterIf::reset ()
{

    m_pCCombFilter = nullptr;
    m_fSampleRate = 0;
    setParam(FilterParam_t::kParamGain, 0.0f);
    setParam(FilterParam_t::kParamDelay, 0.0f);

    m_bIsInitialized= false;
    return Error_t::kNoError;
}

Error_t CCombFilterIf::process (float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames)
{
//    if (!m_bIsInitialized)
//    {
//        return Error_t::kNotInitializedError;
//    }
    m_pCCombFilter -> process( ppfInputBuffer, ppfOutputBuffer, iNumberOfFrames);
    return Error_t::kNoError;
}

Error_t CCombFilterIf::setParam (FilterParam_t eParam, float fParamValue)
{
    switch (eParam)
    {
        case (FilterParam_t::kParamGain):
            m_pCCombFilter -> setGain(fParamValue);
            break;
            
        case (FilterParam_t::kParamDelay):
            m_pCCombFilter -> setDelay(fParamValue);
            break;
        
        default:
            // Setting default values if no parameters are set
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
