#pragma once
#include "imageProcessing.h"
class Resize : public imageProcessing
{
public:
    Resize(vector<string> remainingArgs, cv::Mat i) throw(MissingArgs);
    cv::Mat process() throw(SizeException);
private:
    cv::Mat adjustedImage;
    int width;
    int height;
    bool isInt(string& intString);
};