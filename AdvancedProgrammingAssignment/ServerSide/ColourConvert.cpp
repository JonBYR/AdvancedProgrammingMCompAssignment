#include "ColourConvert.h"
ColourConvert::ColourConvert(vector<string> remainingArgs, cv::Mat i) : imageProcessing(remainingArgs, i) {
    //adjustedImage(move(image)); //use this if using the threaded functions, otherwise delete this line
    adjustedImage = i;
    //using inbuilt move constructor to copy information from image to adjustedImage
}
cv::Mat ColourConvert::process() {
    if (filterType == "greyscale") {
        int rows = adjustedImage.rows; //each row will be threaded individually
        vector<thread> threads;
        for (int i = 0; i < rows; i++) {
            threads.push_back(thread([&]() { //[&] captures everything used by the class, including variables i and adjustedImage
                for (int j = 0; j < adjustedImage.cols; j++) {
                    cv::Vec3b bgrVal = adjustedImage.at<float>(i, j); //gets the RGB value at row i and col j, however it is also realised that this is in BGR
                    double blue = bgrVal[0];
                    double green = bgrVal[1];
                    double red = bgrVal[2];
                    double greyVal = (0.114 * blue) + (0.587 * green) + (0.299 * red); //combined value for greyscale
                    adjustedImage.at<float>(i, j) = greyVal; //overwrites the current value to be the greyScale value
                }
                }));
        }
        for (auto& thread : threads) {
            thread.join();
        }
    }
    //if (filterType == "greyscale") greyScale();
    else {
        throw InvalidOperationException(filterType);
    }
    return adjustedImage;
}
void ColourConvert::greyScale() {
    cv::cvtColor(image, adjustedImage, cv::COLOR_BGR2GRAY); //opencv function
}


void ColourConvert::greyScale(const int &startRow, const int &threadSize) {
    for(int i = startRow; i < startRow + threadSize; i++) {
        for(int j = 0; j < adjustedImage.cols; j++) {
            cv::Vec3b bgrVal = adjustedImage.at<float>(i, j); //gets the RGB value at row i and col j, however it is also realised that this is in BGR
            double blue = bgrVal[0];
            double green = bgrVal[1];
            double red = bgrVal[2];
            double greyVal = (0.114 * blue) + (0.587 * green) + (0.299 * red); //combined value for greyscale
            adjustedImage.at<float>(i, j) = greyVal; //overwrites the current value to be the greyScale value
        }
    }
    /*
    lock
    loop through
    for (img)
        img.at

    unlock
    */
    return;
}