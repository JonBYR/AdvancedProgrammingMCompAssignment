#include "Blur.h"
Blur::Blur(vector<string> remainingArgs, cv::Mat i) : imageProcessing(remainingArgs, i) {
    adjustedImage = image.clone();
}
cv::Mat Blur::process() throw(InvalidOperationException) {
    if(filterType == "guassian") guassian();
    else if (filterType == "box") box();
    else if (filterType == "sharpening") sharpening();
    else {
        throw InvalidOperationException(filterType);
    }
    return adjustedImage;
}
void Blur::guassian() {
    for(int i = 0; i < MAX_KERNEL_LENGTH; i = i + 2) {
        cv::GaussianBlur(image, adjustedImage, cv::Size(i, i), 0, 0); //variables in size need to be odd and positive
        //Size may be i, i
    }
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
        /*
        if( display_dst( DELAY_BLUR ) != 0 )
        {
            return 0;
        }
        */
    }
}