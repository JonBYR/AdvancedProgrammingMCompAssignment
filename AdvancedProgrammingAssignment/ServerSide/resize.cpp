#include "resize.h"
Resize::Resize(vector<string> remainingArgs, cv::Mat i) : imageProcessing(remainingArgs, i) {
    width = (int)requiredNumber;
    height = stoi(requiredFunctionality); //needed to create the dimensions for the resized image
}
cv::Mat Resize::process() throw(NonNumeric) {
    int width = (int)requiredNumber;
    if(isInt(requiredFunctionality) == true) {
        height = stoi(requiredFunctionality);
    }
    else 
    {
        throw NonNumeric(requiredFunctionality, 4); //in the user input this would be the fourth argument
    }
    resize(image, adjustedImage, cv::Size(width, height), cv::INTER_LINEAR); //function required for resizing in opencv
    return adjustedImage;
}
bool Resize::isInt(string& intString) {
    int size = intString.length();
    for(int i = 0; i < size; i++) {
        if(!isdigit(intString[i])) return false;
    }
    return true;
}