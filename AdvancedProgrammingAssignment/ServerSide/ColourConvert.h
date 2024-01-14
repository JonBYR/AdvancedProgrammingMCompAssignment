#pragma once
#include "imageProcessing.h"
#include <algorithm>
#include <thread>
#include <mutex>
class ColourConvert : public ImageProcessing {
public:
    ColourConvert(vector<string> remainingArgs, cv::Mat i);
    cv::Mat process() throw(InvalidOperationException);
private:
    cv::Mat adjustedImage;
};