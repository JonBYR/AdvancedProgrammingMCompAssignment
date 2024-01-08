#pragma once
#include "imageProcessing.h"
class Resize : public imageProcessing
{
public:
    Resize(vector<string> remainingArgs, cv::Mat i);
    cv::Mat process() throw(NonNumeric);
private:
    cv::Mat adjustedImage;
    int width;
    int height;
    bool isInt(string& intString);
};