#include "Blur.h"
Blur::Blur(vector<string> remainingArgs, cv::Mat i) : imageProcessing(remainingArgs, i) {
    adjustedImage = image.clone();
}
cv::Mat Blur::process() throw(InvalidOperationException) {
    if (filterType == "box") box();
    else if (filterType == "sharpening") sharpening();
    else {
        throw InvalidOperationException(filterType);
    }
    return adjustedImage;
}
void Blur::sharpening() { //sharpening taken from https://answers.opencv.org/question/216383/how-could-do-sharpness-images/
    cv::Mat sharpeningKernel = (cv::Mat_<double>(3, 3) << -1, -1, -1,
    -1, 9, -1,
    -1, -1, -1);
    filter2D(image, adjustedImage, -1, sharpeningKernel);
}
void Blur::box() {
    for (int i = 1; i < MAX_KERNEL_LENGTH; i = i + 2)
    {
        blur(image, adjustedImage, cv::Size( i, i ), cv::Point(-1,-1) );
    }
}