#include "Flip.h"
Flip::Flip (vector<string> remainingArgs, cv::Mat i) : imageProcessing(remainingArgs, i) {
    adjustedImage = cv::Mat::zeros(image.size(), image.type());
    }
cv::Mat Flip::process() throw(InvalidOperationException) {
    if(requiredNumber == 0 || requiredNumber == 1 || requiredNumber == -1) { //only acceptable numbers for flipping are 0 and 1
        flip(image, adjustedImage, (int)requiredNumber);
        return adjustedImage;
    }
    else {
        throw InvalidOperationException(to_string(requiredNumber));
    }
}