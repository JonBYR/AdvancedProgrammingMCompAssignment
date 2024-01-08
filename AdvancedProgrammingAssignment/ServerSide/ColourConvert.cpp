#include "ColourConvert.h"
ColourConvert::ColourConvert(vector<string> remainingArgs, cv::Mat i) : imageProcessing(remainingArgs, i) {
    //adjustedImage(move(i)); //use this if using the threaded functions, otherwise delete this line
    //using inbuilt move constructor to copy information from image to adjustedImage
}
cv::Mat ColourConvert::process() {
    /*
    int rows = adjustedImage.rows; //to partition the image it must be done in seperate rows
    int partition = rows / 3 //choosing a max number of rows each thread can be
    vector<thread> threads;
    for(int i = 0 < i < rows; i++) {
        if(i == partition - 1) {
            threads.push_back(thread(&ColourConvert::greyscale, this, i, partition));
        }
        if(i == rows - 1) { //if the max size of rows is not reached but the number of rows is instead
            threads.push_back(thread(&ColourConvert::greyscale, this, i, partition - i));
        }
    }
    */
    if(filterType == "greyscale") greyScale();
    else if(filterType == "hsv") HSV();
    else {
        throw InvalidOperationException(filterType);
    }
    return adjustedImage;
}
void ColourConvert::greyScale() {
    cv::cvtColor(image, adjustedImage, cv::COLOR_BGR2GRAY); //opencv function
}
void ColourConvert::HSV() {
    cv::cvtColor(image, adjustedImage, cv::COLOR_BGR2HSV);
}

/*
void ColourConvert::greyscale(const int& startRow, const int& threadSize) {
    for(int i = startRow; i < startRow + threadSize; i++) {
        for(int j = 0; j < adjustedImange.cols; j++) {
            vec3b bgrVal = adjustedImage.at(i,j) //gets the RGB value at row i and col j, however it is also realised that this is in BGR
            double blue = bgrVal[0];
            double green = bgrVal[1];
            double red = bgrVal[2];
            double greyVal = (0.114 * blue) + (0.587 * green) + (0.299 * red); //combined value for greyscale
            adjustedImage.at(i, j) = greyVal; //overwrites the current value to be the greyScale value
        }
    }
    lock
    loop through
    for (img)
        img.at

    unlock
}
void ColourConvert::HSV(const int& startRow, const int& threadSize) {
    for(int i = startRow; i < startRow + threadSize; i++) {
        for(int j = 0; j < adjustedImange.cols; j++) {
            vec3b bgrVal = adjustedImage.at(i,j) //gets the RGB value at row i and col j, however it is also realised that this is in BGR
            double blue = bgrVal[0];
            double green = bgrVal[1];
            double red = bgrVal[2];
            vector<double> colours = {blue, green, red};
            sort(colours.begin(), colours.end());
            double volume = colours[2] //volume is the max of blue, green or red
            double saturation, hue;
            if(volume == 0) {
                saturation = 0;
                hue = 0;
            }
            else 
            {
                saturation = (volume - colours[0])/volume; //saturation formula
            }
            if(red == green && red == blue && green == blue) {
                hue = 0;
            }
            else 
            {
                if(volume == red) hue = (60 * (green - blue))/(volume - colours[0]);
                else if(volume == green) hue = 120 + (60 * (blue - red)/(volume - colours[0]));
                else if(volume == blue) hue = 240 + (60 * (red - green)/(volume - colours[0]));
                //https://docs.opencv.org/4.x/de/d25/imgproc_color_conversions.html formula for hue
            }
            adjustedImage.at(i, j) = vec3b(hue, saturation, volume);
        }
    }
}
*/