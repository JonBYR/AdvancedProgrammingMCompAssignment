#include "Flip.h"
Flip::Flip (vector<string> remainingArgs, cv::Mat i) : imageProcessing(remainingArgs, i) {
    adjustedImage = cv::Mat::zeros(image.size(), image.type()); //in order to map the flipped image it must be mapped to an empty Mat
    }
cv::Mat Flip::process() throw(InvalidOperationException, FlipException) {
    if(requiredNumber == 0 || requiredNumber == 1 || requiredNumber == -1) { //only acceptable numbers for flipping are 0 and 1 and -1
        flip(image, adjustedImage, (int)requiredNumber); //requiredNumber is a double so it's casted to an int to ensure operation is performed
        return adjustedImage;
    }
    else {
        throw FlipException(requiredNumber);
    }
}