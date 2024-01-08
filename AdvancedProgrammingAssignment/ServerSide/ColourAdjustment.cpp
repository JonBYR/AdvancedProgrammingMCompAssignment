#include "ColourAdjustment.h"
ColourAdjustment::ColourAdjustment(vector<string> remainingArgs, cv::Mat i) : imageProcessing(remainingArgs, i) {
    adjustedImage = cv::Mat::zeros(image.size(), image.type());
}
cv::Mat ColourAdjustment::process() throw(InvalidOperationException) {
    if(filterType == "brightness") brightness();
    else if (filterType == "contrast") contrast();
    else if (filterType == "gamma") gamma();
    else {
        throw InvalidOperationException(filterType);
    }
    return adjustedImage;
}
void ColourAdjustment::brightness() {
    double alpha = requiredNumber;
    for(int j = 0; j < image.rows; j++) {
        for(int i = 0; i < image.cols; i++) {
            for(int c = 0; c < image.channels(); c++) {
                adjustedImage.at<cv::Vec3b>(j, i)[c] = cv::saturate_cast<uchar>(image.at<cv::Vec3b>(j, i)[c] + alpha);
            }
        }
    } //formula used by opencv for adjusting brightness
}
void ColourAdjustment::contrast() {
    double alpha = requiredNumber; //needs to be positive so potentially create another exception for this
    for(int j = 0; j < image.rows; j++) {
        for(int i = 0; i < image.cols; i++) {
            for(int c = 0; c < image.channels(); c++) {
                adjustedImage.at<cv::Vec3b>(j, i)[c] = cv::saturate_cast<uchar>(alpha * image.at<cv::Vec3b>(j, i)[c]);
            }
        }
    } //formula used by opencv for adjusting contrast
}
void ColourAdjustment::gamma() {
    cv::Mat lookupTable(1, 256, CV_8U);
    uchar* p = lookupTable.ptr(); //converts lookuptable to a pointer of type uchar
    for(int i = 0; i < 256; ++i) {
        p[i] = cv::saturate_cast<uchar>(pow(i / 255.0, requiredNumber) * 255.0); //as it is a pointer, the values in the lookuptable are affected when p[i] is affected
    }
    adjustedImage = image.clone();
    LUT(image, lookupTable, adjustedImage);
}
