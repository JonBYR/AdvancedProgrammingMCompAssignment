#include "ColourConvert.h"
ColourConvert::ColourConvert(vector<string> remainingArgs, cv::Mat i) : imageProcessing(remainingArgs, i) {
    adjustedImage = move(image); //using move as it is more memory efficent than copy operation
}
cv::Mat ColourConvert::process() {
    if (filterType == "greyscale") {
        int rows = adjustedImage.rows; //each row will be threaded individually
        vector<thread> threads;
        int counter = 10; //limit the number of threads being run at one time
        
        for (int i = 0; i <= rows; i++) {
            if (i % counter == 0)
            {
                for (auto& thread : threads) {
                    thread.join();
                }
                threads.clear();
                if (i == rows) break;
            }
            threads.push_back(thread([=]() { //[=] captures everything used by the class, copying by values, including variables i and adjustedImage
                for (int j = 0; j < adjustedImage.cols; j++) {
                    cv::Vec3b bgrVal = adjustedImage.at<cv::Vec3b>(i, j); //gets the RGB value at row i and col j, however it is also realised that this is in BGR
                    double blue = bgrVal[0];
                    double green = bgrVal[1];
                    double red = bgrVal[2]; //lambda used as thread constructor did not accept member function as an argument
                    double greyVal = (0.114 * blue) + (0.587 * green) + (0.299 * red); //combined value for greyscale
                    cv::Vec3b greyPixel;
                    greyPixel[0] = greyVal;
                    greyPixel[1] = greyVal;
                    greyPixel[2] = greyVal;
                    adjustedImage.at<cv::Vec3b>(i, j) = greyPixel; //overwrites the current value to be the greyScale value
                }
                }));
        }
    }
    else {
        throw InvalidOperationException(filterType);
    }
    return adjustedImage;
}

