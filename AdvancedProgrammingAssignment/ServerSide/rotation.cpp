#include "rotation.h"
rotation::rotation(vector<string> remainingArgs, cv::Mat i) : imageProcessing(remainingArgs, i) {
}
cv::Mat rotation::process() throw(InvalidOperationException) {
	if (requiredFunctionality == "counterclockwise") requiredNumber = 360 - requiredNumber; //counterclockwise will be the inverse of rotating by the current angle
	else if (requiredFunctionality == "clockwise") requiredNumber = requiredNumber;
	else { std::cout << "This is called" << std::endl;  throw InvalidOperationException(requiredFunctionality); }
	cv::Point2f centre((image.cols - 1) / 2.0, (image.rows - 1) / 2.0); //gets centre of image
	cv::Mat rotate_matrix = getRotationMatrix2D(centre, requiredNumber, 1.0); //function used to rotate image
	cv::Mat rotated_image;
	cv::warpAffine(image, rotated_image, rotate_matrix, image.size());
	return rotated_image;
}