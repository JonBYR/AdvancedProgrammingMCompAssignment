#include "Blur.h"
Blur::Blur(vector<string> remainingArgs, cv::Mat i) : ImageProcessing(remainingArgs, i) {
    adjustedImage = image.clone(); //opencv operations listed below needs a clone of the original image
}
cv::Mat Blur::process() throw(InvalidOperationException) {
    if (filterType == "box") box();
    else if (filterType == "sharpening") sharpening();
    else {
        throw InvalidOperationException(filterType); //in case filterType variable is not caught in serverSide application it is caught in process
    }
    return adjustedImage;
}
void Blur::sharpening() { //sharpening taken from https://answers.opencv.org/question/216383/how-could-do-sharpness-images/
    cv::Mat sharpeningKernel = (cv::Mat_<double>(3, 3) << -1, -1, -1,
    -1, 9, -1,
    -1, -1, -1);
    filter2D(image, adjustedImage, -1, sharpeningKernel); //open cv function
}
void Blur::box() {
    for (int i = 1; i < MAX_KERNEL_LENGTH; i = i + 2)
    {
        blur(image, adjustedImage, cv::Size( i, i ), cv::Point(-1,-1) ); //open cv function
    }
}