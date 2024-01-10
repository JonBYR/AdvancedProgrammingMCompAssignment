#include "ColourConvert.h"
ColourConvert::ColourConvert(vector<string> remainingArgs, cv::Mat i) : imageProcessing(remainingArgs, i) {
    adjustedImage = move(image); //using move as it is more memory efficent than copy operation
}
cv::Mat ColourConvert::process() {
    if (filterType == "greyscale") {
        int rows = adjustedImage.rows; //each row will be threaded individually
        vector<thread> threads;
        
        for (int i = 0; i < rows; i++) {
            threads.push_back(thread([&]() { //[&] captures everything used by the class, including variables i and adjustedImage
                for (int j = 0; j < adjustedImage.cols; j++) {
                    cv::Vec3b bgrVal = adjustedImage.at<cv::Vec3b>(i, j); //gets the RGB value at row i and col j, however it is also realised that this is in BGR
                    double blue = bgrVal[0];
                    double green = bgrVal[1];
                    double red = bgrVal[2];
                    double greyVal = (0.114 * blue) + (0.587 * green) + (0.299 * red); //combined value for greyscale
                    cv::Vec3b greyPixel;
                    greyPixel[0] = greyVal;
                    greyPixel[1] = greyVal;
                    greyPixel[2] = greyVal;
                    adjustedImage.at<cv::Vec3b>(i, j) = greyPixel; //overwrites the current value to be the greyScale value
                }
                }));
        }
        for (auto& thread : threads) {
            thread.join();
        }
        
        //greyScale();
    }
    //if (filterType == "greyscale") greyScale();
    else {
        throw InvalidOperationException(filterType);
    }
    return adjustedImage;
}
/*
void ColourConvert::greyScale() {
    cv::cvtColor(image, adjustedImage, cv::COLOR_BGR2GRAY); //opencv function
}
*/

void ColourConvert::greyScale() {
    int counter = 0;
    for(int i = 0; i < adjustedImage.rows; i++) {
        for(int j = 0; j < adjustedImage.cols; j++) {
            cv::Vec3b bgrVal = adjustedImage.at<cv::Vec3b>(i, j); //gets the RGB value at row i and col j, however it is also realised that this is in BGR
            double blue = bgrVal[0];
            double green = bgrVal[1];
            double red = bgrVal[2];
            double greyVal = (0.114 * blue) + (0.587 * green) + (0.299 * red); //combined value for greyscale
            cv::Vec3b greyPixel;
            greyPixel[0] = greyVal;
            greyPixel[1] = greyVal;
            greyPixel[2] = greyVal;
            adjustedImage.at<cv::Vec3b>(i, j) = greyPixel; //overwrites the current value to be the greyScale value
        }
    }
    return;
}
