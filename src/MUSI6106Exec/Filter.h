//
// Created by Admin on 2/14/22.
//

#ifndef MUSI6106_FILTER_H
#define MUSI6106_FILTER_H
#include <iostream>
#include "ErrorDef.h"
#include "AudioFileIf.h"
#include "CombFilterIf.h"

using std::cout;
using std::endl;

class Filter
{
public:

    Error_t setParams(std::string sInputFilePath,std::string sOutputFilePath,
                      std::string sFilterType,float fDelay,
                      float fGain, int iBlockSize);

    Error_t processAudio();

private:
    std::string mInputFilePath,
                mOutputFilePath,
                mFilterType;

    float       mDelayInSeconds,
                mGain;

    int         mBlockSize;
};





#endif //MUSI6106_FILTER_H
