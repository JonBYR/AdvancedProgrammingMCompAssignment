#pragma once
#include "imageProcessing.h"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
class Blur : public imageProcessing {
public:
    Blur(vector<string> remainingArgs, cv::Mat i);
    ~Blur();
    virtual cv::Mat process() throw(InvalidOperationException);
private:
    cv::Mat adjustedImage;
    void guassian();
    void box();
    void sharpening();
    const int MAX_KERNEL_LENGTH = 31;
};