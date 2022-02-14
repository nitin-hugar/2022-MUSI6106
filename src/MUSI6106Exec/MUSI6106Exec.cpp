
#include <iostream>
#include <ctime>

#include "MUSI6106Config.h"

#include "AudioFileIf.h"
#include "CombFilterIf.h"


using std::cout;
using std::endl;

// local function declarations
void    showClInfo ();

/////////////////////////////////////////////////////////////////////////////////
// main function
int main(int argc, char* argv[])
{
    
//    std::string sInputFilePath,                 //!< file paths
//                sOutputFilePath;

    static const int kBlockSize = 1024;

    // clock_t time = 0;

    float **ppfInputAudioData = nullptr;
    float **ppfOutputAudioData = nullptr;
    CAudioFileIf *phInputAudioFile = nullptr;
    CAudioFileIf *phOutputAudioFile = nullptr;

    CAudioFileIf::FileSpec_t stFileSpec;
    showClInfo();

    //============================================================================
    // Initialize test values
    //============================================================================

    std::string sInputFilePath = "/Users/apple/Desktop/4.wav";
    std::string sOutputFilePath = "/Users/apple/Desktop/filteredOutput.wav";
    float delayTimeInSeconds = 1.5;
    float gain = 0.5;

    // Initialize CombFilter
    CCombFilterIf *pCombFilter = nullptr;
    CCombFilterIf::CombFilterType_t eFilterType = CCombFilterIf::kCombFIR;

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
        return -1;
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
        return -1;
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

//    if (ppfInputAudioData[0] == 0 || ppfOutputAudioData[0] == 0)
//    {
//        CAudioFileIf::destroy(phInputAudioFile);
//        CAudioFileIf::destroy(phOutputAudioFile);
//        return -1;
//    }

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

    phInputAudioFile -> closeFile();
    phOutputAudioFile -> closeFile();
    CAudioFileIf::destroy(phInputAudioFile);
    CAudioFileIf::destroy(phOutputAudioFile);
    for(int m = 0; m < stFileSpec.iNumChannels; m++ )
    {
        delete[] ppfInputAudioData;
        delete[] ppfOutputAudioData;
    }
    delete[] ppfInputAudioData;
    delete[] ppfOutputAudioData;
    ppfInputAudioData = nullptr;
    ppfOutputAudioData = nullptr;

    // all done
    return 0;

}


void     showClInfo()
{
    cout << "MUSI6106 Assignment Executable" << endl;
    cout << "(c) 2014-2022 by Alexander Lerch" << endl;
    cout  << endl;
}

