#pragma once
#include "imageProcessing.h"
#include <opencv2/core/core.hpp> 
#include <opencv2/highgui/highgui.hpp> 
class Flip : public imageProcessing {
public:
    Flip(vector<string> remainingArgs, cv::Mat i);
    virtual cv::Mat process() throw(FlipException);
private:
    cv::Mat adjustedImage;
};