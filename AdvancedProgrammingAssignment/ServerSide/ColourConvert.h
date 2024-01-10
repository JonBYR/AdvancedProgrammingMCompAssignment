#pragma once
#include "imageProcessing.h"
#include <algorithm>
#include <thread>
#include <mutex>
class ColourConvert : public imageProcessing {
public:
    ColourConvert(vector<string> remainingArgs, cv::Mat i);
    cv::Mat process() throw(InvalidOperationException);
private:
    void greyScale();
    //void greyScale(const int &startRow, const int &threadSize);
    cv::Mat adjustedImage;
    mutex threadLock;
};