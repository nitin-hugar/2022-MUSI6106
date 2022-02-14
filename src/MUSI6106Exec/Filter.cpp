//
// Created by Admin on 2/14/22.
//

#include "Filter.h"


Error_t Filter::setInputFilePath(std::string sFilepath)
{
    mInputFilePath = sFilepath;
    return Error_t::kNoError;
}

Error_t Filter::setOutputFilePath(std::string sFilePath)
{
    mOutputFilePath = sFilePath;
    return Error_t::kNoError;
}

Error_t Filter::setFilterType(std::string sFiltertype) {
    mFilterType = sFiltertype;
    return Error_t::kNoError;
}

Error_t Filter::setDelay(float fDelay) {
    mDelayInSeconds = fDelay;
    return Error_t::kNoError;
}

Error_t Filter::setGain(float fGain) {
    mGain = fGain;
    return Error_t::kNoError;
}

Error_t  Filter::processAudio(std::string sInputFilePath, std::string sOutputFilePath, std::string sFilterType, float fDelay,
                     float fGain) {

    //============================================================================
    // Initialize pointers and inputs
    //============================================================================

    static const int kBlockSize = 1024;

    float                   **ppfInputAudioData = nullptr,
                            **ppfOutputAudioData = nullptr;;

    CAudioFileIf            *phInputAudioFile = nullptr,
                            *phOutputAudioFile = nullptr;


    float                   delayTimeInSeconds,
            gain;

    CAudioFileIf::FileSpec_t stFileSpec;

    // Initialize CombFilter
    CCombFilterIf           *pCombFilter = nullptr;
    CCombFilterIf::CombFilterType_t eFilterType;


    delayTimeInSeconds = fDelay;
    gain = fGain;

    if (sFilterType == "FIR")
    {
        eFilterType = CCombFilterIf::kCombFIR;
    }
    else if (sFilterType == "IIR") {
        eFilterType = CCombFilterIf::kCombIIR;

    }

    //============================================================================
    // open the input wave file
    //============================================================================

    CAudioFileIf::create(phInputAudioFile);
    phInputAudioFile->openFile(sInputFilePath,
                               CAudioFileIf::kFileRead);
    if (!phInputAudioFile->isOpen())
    {
        cout << "Input wave file open error!" << endl;
        CAudioFileIf::destroy(phInputAudioFile);
    }
    phInputAudioFile->getFileSpec(stFileSpec);

    //============================================================================
    // Open the output audio file
    //============================================================================

    CAudioFileIf::create(phOutputAudioFile);
    phOutputAudioFile -> openFile(sOutputFilePath,
                                  CAudioFileIf::kFileWrite,
                                  &stFileSpec);

    if (!phOutputAudioFile->isOpen())
    {
        cout << "Output wave file open error!" << endl;
        CAudioFileIf::destroy(phOutputAudioFile);
    }

    // allocate memory
    ppfInputAudioData = new float*[stFileSpec.iNumChannels];
    ppfOutputAudioData = new float* [stFileSpec.iNumChannels];
    for (int i = 0; i < stFileSpec.iNumChannels; i++)
    {
        ppfInputAudioData[i] = new float[kBlockSize];
        ppfOutputAudioData[i] = new float[kBlockSize];
    }

    // Destroy and clear memory: Check if this is needed !

    if (ppfInputAudioData[0] == 0 || ppfOutputAudioData[0] == 0)
    {
        CAudioFileIf::destroy(phInputAudioFile);
        CAudioFileIf::destroy(phOutputAudioFile);
    }

    //============================================================================
    // Create CombFilters and allocate memory
    //============================================================================

    CCombFilterIf::create(pCombFilter);
    pCombFilter -> init(eFilterType, delayTimeInSeconds,
                        stFileSpec.fSampleRateInHz ,
                        stFileSpec.iNumChannels);
    float delayTimeInSamples = delayTimeInSeconds * stFileSpec.fSampleRateInHz;
    pCombFilter -> setParam(CCombFilterIf::FilterParam_t::kParamGain, gain );
    pCombFilter -> setParam(CCombFilterIf::FilterParam_t::kParamDelay, delayTimeInSamples );
    // Not setting Delay time again since it is redundant


    //============================================================================
    // Read, Filter and Write Audio
    //============================================================================


    while (!phInputAudioFile->isEof())
    {
        // set block length variable
        long long int iNumFrames = kBlockSize;

        // read data (iNumOfFrames might be updated!)
        phInputAudioFile->readData(ppfInputAudioData, iNumFrames);
        pCombFilter->process(ppfInputAudioData, ppfOutputAudioData, iNumFrames);
        phOutputAudioFile->writeData(ppfOutputAudioData, iNumFrames);

    }

    cout << "Filtering complete! The output audio is saved at : " << sOutputFilePath << endl;

    //============================================================================
    // Clean up and free memory
    //============================================================================

    CCombFilterIf::destroy(pCombFilter);
    phInputAudioFile -> closeFile();
    phOutputAudioFile -> closeFile();
    CAudioFileIf::destroy(phInputAudioFile);
    CAudioFileIf::destroy(phOutputAudioFile);
    for(int m = 0; m < stFileSpec.iNumChannels; m++ )
    {
        delete[] ppfInputAudioData[m];
        delete[] ppfOutputAudioData[m];
    }
    delete[] ppfInputAudioData;
    delete[] ppfOutputAudioData;
    ppfInputAudioData = nullptr;
    ppfOutputAudioData = nullptr;

    // all done

    return Error_t::kNoError;
}

