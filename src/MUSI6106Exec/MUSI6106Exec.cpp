
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
    float delayTimeInSeconds = 0.25;
    float gain = 1.0;

    // Initialize CombFilter
    CCombFilterIf *pCombFilter;
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

    if (ppfInputAudioData == 0 || ppfOutputAudioData == 0) // Also check if 'or' is needed
    {
        CAudioFileIf::destroy(phInputAudioFile);
        CAudioFileIf::destroy(phOutputAudioFile);
        return -1;
    }
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
    pCombFilter ->init(eFilterType, delayTimeInSeconds,
                       stFileSpec.fSampleRateInHz ,
                       stFileSpec.iNumChannels);
    pCombFilter -> setParam(CCombFilterIf::FilterParam_t::kParamGain, gain );

    // Not setting Delay time again since it is redundant


    //============================================================================
    //
    //============================================================================



    // get audio data and write it to the output text file (one column per channel)
    while (!phAudioFile->isEof())
    {
        // set block length variable
        long long iNumFrames = kBlockSize;

        // read data (iNumOfFrames might be updated!)
        phAudioFile->readData(ppfAudioData, iNumFrames);

        cout << "\r" << "reading and writing";

        // write
        for (int i = 0; i < iNumFrames; i++)
        {
            for (int c = 0; c < stFileSpec.iNumChannels; c++)
            {
                hOutputFile << ppfAudioData[c][i] << "\t";
            }
            hOutputFile << endl;
        }
    }

    cout << "\nreading/writing done in: \t" << (clock() - time) * 1.F / CLOCKS_PER_SEC << " seconds." << endl;

    //////////////////////////////////////////////////////////////////////////////
    // clean-up (close files and free memory)
    CAudioFileIf::destroy(phAudioFile);
    hOutputFile.close();

    for (int i = 0; i < stFileSpec.iNumChannels; i++)
        delete[] ppfAudioData[i];
    delete[] ppfAudioData;
    ppfAudioData = 0;

    // all done
    return 0;

}


void     showClInfo()
{
    cout << "MUSI6106 Assignment Executable" << endl;
    cout << "(c) 2014-2022 by Alexander Lerch" << endl;
    cout  << endl;

    return;
}

