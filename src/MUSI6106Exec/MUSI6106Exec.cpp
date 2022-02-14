
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
void test1(std::string sInputFilePath, std::string sOutputFilePath,std::string sFilterType,
           float delayTimeInSeconds,float gain,int iBlockSize);
void test2(std::string sInputFilePath, std::string sOutputFilePath,std::string sFilterType,
           float delayTimeInSeconds,float gain,int iBlockSize);
Error_t test3();
Error_t test4();
Error_t test5();
void runAllTests();
void paramsTest ();


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
        std::cout << "Arguments: " << std::endl;
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


void test1(std::string sInputFilePath, std::string sOutputFilePath,std::string sFilterType,
                      float delayTimeInSeconds,float gain,int iBlockSize)
{
    Filter filter;
    filter.setParams(sInputFilePath, sOutputFilePath, sFilterType, delayTimeInSeconds, gain, iBlockSize);
    filter.processAudio();
    cout << "Test1 succeeded! File saved to " << sOutputFilePath << endl;
}

void runAllTests()
{


    //============================================================================
    // Test 1
    //============================================================================

    cout << "Running test 1" << endl;
    std::string t1_sInputFilePath = "/Users/apple/Desktop/4.wav";
    std::string t1_sOutputFilePath = "/Users/apple/Desktop/test1_output.wav";
    std::string t1_sFilterType = "FIR";
    float t1_delayTimeInSeconds = 0.01f;
    float t1_gain = -1.0f;
    int t1_iBlockSize = 2048;

    test1(t1_sInputFilePath, t1_sOutputFilePath,
          t1_sFilterType, t1_delayTimeInSeconds, t1_gain, t1_iBlockSize);


    //============================================================================
    // Test 2
    //============================================================================

    cout << "Running test 2" << endl;
    std::string t2_sInputFilePath = "/Users/apple/Desktop/4.wav";
    std::string t2_sOutputFilePath = "/Users/apple/Desktop/test2_output.wav";
    std::string t2_sFilterType = "IIR";
    float t2_delayTimeInSeconds = 0.1f;
    float t2_gain = 0.5f;
    int t2_iBlockSize = 2048;

    test1(t2_sInputFilePath, t2_sOutputFilePath,
          t2_sFilterType, t2_delayTimeInSeconds, t2_gain, t2_iBlockSize);

    //============================================================================
    // Test 3
    //============================================================================

    cout << "Running test 3" << endl;
    std::string t3_sInputFilePath = "/Users/apple/Desktop/4.wav";
    std::string t3_sOutputFilePath = "/Users/apple/Desktop/test3_output.wav";
    std::string t3_sFilterType = "FIR";
    float t3_delayTimeInSeconds = 0.01f;
    float t3_gain = -0.5f;
    int t3_iBlockSize = 512;

    test1(t3_sInputFilePath, t3_sOutputFilePath,
          t3_sFilterType, t3_delayTimeInSeconds, t3_gain, t3_iBlockSize);

    //============================================================================
    // Test 4
    //============================================================================
//
//    cout << "Running test 4" << endl;
//    std::string t4_sInputFilePath = "/Users/apple/Desktop/4.wav";
//    std::string t4_sOutputFilePath = "/Users/apple/Desktop/test4_output.wav";
//    std::string t4_sFilterType = "FIR";
//    float t4_delayTimeInSeconds = 0.01f;
//    float t4_gain = -1.0f;
//    int t4_iBlockSize = 512;
//
//    test1(t4_sInputFilePath, t4_sOutputFilePath,
//          t4_sFilterType, t4_delayTimeInSeconds, t4_gain, t4_iBlockSize);

    //============================================================================
    // Test 5
    //============================================================================

    cout << "Running test 5" << endl;
    std::string t5_sInputFilePath = "/Users/apple/Desktop/4.wav";
    std::string t5_sOutputFilePath = "/Users/apple/Desktop/test5_output.wav";
    std::string t5_sFilterType = "FIR";
    float t5_delayTimeInSeconds = -0.25f;
    float t5_gain = -2.0f;
    int t5_iBlockSize = 1024;

    test1(t5_sInputFilePath, t5_sOutputFilePath,
          t5_sFilterType, t5_delayTimeInSeconds, t5_gain, t5_iBlockSize);
    showClInfo();
}


void showClInfo()
{
    cout << "MUSI6106 Assignment Executable" << endl;
    cout << "(c) 2014-2022 by Alexander Lerch" << endl;
    cout  << endl;
}





