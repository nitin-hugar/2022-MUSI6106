
#include <iostream>
#include <ctime>

#include "MUSI6106Config.h"

#include "AudioFileIf.h"
#include "CombFilterIf.h"
#include <cassert>
#include <stdlib.h>


using std::cout;
using std::endl;

//============================================================================
// Define Tests
//============================================================================
void    showClInfo ();
int test1();
int test2();
int test3();
int test4();
int test5();
int runAllTests();


// main function
int main(int argc, char* argv[])
{

    //============================================================================
    // Initialize pointers and inputs
    //============================================================================

    static const int kBlockSize = 1024;

    float                   **ppfInputAudioData = nullptr,
                            **ppfOutputAudioData = nullptr;;

    CAudioFileIf            *phInputAudioFile = nullptr,
                            *phOutputAudioFile = nullptr;

    std::string             sInputFilePath,
                            sOutputFilePath,
                            sFilterType;

    float                   delayTimeInSeconds,
                            gain;

    CAudioFileIf::FileSpec_t stFileSpec;
    showClInfo();

    // Initialize CombFilter
    CCombFilterIf           *pCombFilter = nullptr;
    CCombFilterIf::CombFilterType_t eFilterType;


    //============================================================================
    // Parse CL Arguments
    //============================================================================


    if(argc < 2)
    {
        cout << "Usage: " << argv[1] << "<input audio Path>" << endl;
        cout << argv[2] << "<output audio path>" << endl;
        cout << argv[3] << "filter type: <FIR> or <IIR>" << endl;
        cout << argv[4] << "<delay in seconds> (should be 0 or positive)" << endl;
        cout << argv[5] << "<gain> (-1.0 .... 1.0)"<< endl;
    }
    else
    {
        sInputFilePath = argv[1];
        sOutputFilePath = argv[2];
        sFilterType = argv[3];
        delayTimeInSeconds = atof(argv[4]);
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

    if (ppfInputAudioData[0] == 0 || ppfOutputAudioData[0] == 0)
    {
        CAudioFileIf::destroy(phInputAudioFile);
        CAudioFileIf::destroy(phOutputAudioFile);
        return -1;
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
    return 0;

}

void     showClInfo()
{
    cout << "MUSI6106 Assignment Executable" << endl;
    cout << "(c) 2014-2022 by Alexander Lerch" << endl;
    cout  << endl;
}

