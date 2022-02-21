
#include <iostream>
#include "MUSI6106Config.h"
#include <stdlib.h>
#include "Filter.h"
#include <map>

using std::cout;
using std::endl;

//============================================================================
// Define Tests
//============================================================================
void    showClInfo ();
void testFunction(std::string sInputFilePath, std::string sOutputFilePath,std::string sFilterType,
           float delayTimeInSeconds,float gain,int iBlockSize);

void runAllTests();



// main function
int main(int argc, char* argv[])
{

    static const int kBlockSize = 1024;
    //============================================================================
    // Parse CL Arguments
    //============================================================================

    if (argc == 1)
    {
        cout << "Running Tests" << endl;
        runAllTests();
        return -1;
    }

    if(argc > 1 && argc != 6)
    {
        std::cout << "Usage: " << std::endl;
        std::cout << "<input audio Path>" << std::endl;
        std::cout << "<output audio path>" << std::endl;
        std::cout << "filter type: <FIR> or <IIR>" << std::endl;
        std::cout << "<delay in seconds> (>= 0)" << std::endl;
        std::cout << "<gain> (-1.0 .... 1.0)"<< std::endl;
        return -1;
    }
    else
    {
        std::string sInputFilePath = argv[1];
        std::string sOutputFilePath = argv[2];
        std::string sFilterType = argv[3];
        float delayTimeInSeconds = atof(argv[4]);
        float gain = atof(argv[5]);

        Filter filter;
        filter.setParams(sInputFilePath, sOutputFilePath, sFilterType,
                         delayTimeInSeconds, gain, kBlockSize);
        filter.processAudio();
        showClInfo();
    }
}


void testFunction(std::string sInputFilePath, std::string sOutputFilePath,std::string sFilterType,
                      float delayTimeInSeconds,float gain,int iBlockSize)
{
    Filter filter;
    filter.setParams(sInputFilePath, sOutputFilePath, sFilterType, delayTimeInSeconds, gain, iBlockSize);
    filter.processAudio();
    cout << "File saved to " << sOutputFilePath << endl;
}

void runAllTests()
{


    //============================================================================
    // Test 1

    // Input is SineWave of 440Hz. If delay time is 1 / (2*freq) = 0.001134s, the output should be zero
    //============================================================================

    cout << "Running test 1" << endl;
    std::string t1_sInputFilePath = "../../InputAudio/sine440.wav";
    std::string t1_sOutputFilePath = "../../OutputAudio/test1_output.wav";
    std::string t1_sFilterType = "FIR";
    float t1_delayTimeInSeconds = 0.001134f;
    float t1_gain = 1.0f;
    int t1_iBlockSize = 2048;

    testFunction(t1_sInputFilePath, t1_sOutputFilePath,
          t1_sFilterType, t1_delayTimeInSeconds, t1_gain, t1_iBlockSize);


    //============================================================================
    // Test 2
    // Input is SineWave of 440Hz. If delay time is same as the time period, the output magnitude would keep increasing
    //============================================================================

    cout << "Running test 2" << endl;
    std::string t2_sInputFilePath = "../../InputAudio/sine440.wav";
    std::string t2_sOutputFilePath = "../../OutputAudio/test2_output.wav";
    std::string t2_sFilterType = "IIR";
    float t2_delayTimeInSeconds = 0.002272f; // T = 1/440
    float t2_gain = 1.0f;
    int t2_iBlockSize = 2048;

    testFunction(t2_sInputFilePath, t2_sOutputFilePath,
          t2_sFilterType, t2_delayTimeInSeconds, t2_gain, t2_iBlockSize);

    //============================================================================
    // Test 3
    // Test that the filters work for varying block sizes 512, 1024, 4096
    //============================================================================

    cout << "Running test 3" << endl;
    std::string t3_sInputFilePath = "../../InputAudio/sine440.wav";
    std::string t3_sOutputFilePath[2][3] = {{"../../OutputAudio/test3FIR_512.wav", "../../OutputAudio/test3FIR_1024.wav",
                                        "../../OutputAudio/test3FIR_2048.wav"}, {"../../OutputAudio/test3IIR_512.wav", "../../OutputAudio/test3IIR_1024.wav",
                "../../OutputAudio/test3IIR_2048.wav"}};
    std::string t3_sFilterType[] = {"FIR", "IIR"};
    float t3_delayTimeInSeconds = 0.5f;
    float t3_gain = 1.0f;
    int t3_iBlockSize[] = {512, 1024, 4096};

    for (int j = 0; j < 2; j++)
    {
        for (int i = 0; i < 3; i++)
        {
            testFunction(t3_sInputFilePath, t3_sOutputFilePath[j][i],
                         t3_sFilterType[j], t3_delayTimeInSeconds, t3_gain,
                         t3_iBlockSize[i]);
        }
    }

    //============================================================================
    // Test 4
    // Test for Silent input
    //============================================================================

    cout << "Running test 4" << endl;
    std::string t4_sInputFilePath = "../../InputAudio/silence.wav";
    std::string t4_sOutputFilePath[2] = {"../../OutputAudio/test4FIR_output.wav", "../../OutputAudio/test4IIR_output.wav"};
    std::string t4_sFilterType[] = {"FIR", "IIR"};
    float t4_delayTimeInSeconds = 0.5f;
    float t4_gain = 1.0f;
    int t4_iBlockSize = 1024;

    for (int i =0; i< 2 ; i++)
    {
        testFunction(t4_sInputFilePath, t4_sOutputFilePath[i],
                     t4_sFilterType[i], t4_delayTimeInSeconds, t4_gain, t4_iBlockSize);
    }


    //============================================================================
    // Test 5
    // There are Illegal values for gain and delay. The program should throw an error
    //============================================================================

    cout << "Running test 5" << endl;
    std::string t5_sInputFilePath = "../../InputAudio/sine440.wav";
    std::string t5_sOutputFilePath = "../../OutputAudio/test5_output.wav";
    std::string t5_sFilterType = "FIR";
    float t5_delayTimeInSeconds = -0.25f;
    float t5_gain = -2.0f;
    int t5_iBlockSize = 1024;


    testFunction(t5_sInputFilePath, t5_sOutputFilePath,
          t5_sFilterType, t5_delayTimeInSeconds, t5_gain, t5_iBlockSize);
    showClInfo();
}


void showClInfo()
{
    cout << "MUSI6106 Assignment Executable" << endl;
    cout << "(c) 2014-2022 by Alexander Lerch" << endl;
    cout  << endl;
}





