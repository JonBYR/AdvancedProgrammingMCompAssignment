#include "resize.h"
Resize::Resize(vector<string> remainingArgs, cv::Mat i) throw(MissingArgs) : ImageProcessing(remainingArgs, i) {
    if (remainingArgs.size() < 3) throw MissingArgs(6, filterType); //resize requirs the filterType and the amount to be resized by
    width = 0;
    height = 0;
}
cv::Mat Resize::process() throw(SizeException) {
    int width = (int)requiredNumber; //double is cast to an integer value
    if(isInt(requiredFunctionality) == true) {
        height = stoi(requiredFunctionality);
    }
    else 
    {
        throw SizeException(requiredFunctionality); //this is the sixth argumment the user will input and a special exception for Resize operation is called
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