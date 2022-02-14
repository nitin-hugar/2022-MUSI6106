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

    Error_t setInputFilePath(std::string sFilepath);
    Error_t setOutputFilePath(std::string sFilePath);
    Error_t setFilterType(std::string sFiltertype);
    Error_t setDelay(float fDelay);
    Error_t setGain(float fGain);

    Error_t processAudio(std::string sInputFilePath, std::string sOutputFilePath,
                         std::string sFilterType,
                         float fDelay, float fGain);

private:
    std::string mInputFilePath,
                mOutputFilePath,
                mFilterType;

    float       mDelayInSeconds,
                mGain;
};





#endif //MUSI6106_FILTER_H
