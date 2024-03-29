#pragma once
#include "imageProcessing.h"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
class ColourAdjustment : public ImageProcessing 
{
    public:
        ColourAdjustment(vector<string> remainingArgs, cv::Mat i);
        virtual cv::Mat process() throw(InvalidOperationException);
    private:
        cv::Mat adjustedImage;
        void brightness();
        void contrast();
        void gamma();
};