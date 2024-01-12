#include "rotation.h"
rotation::rotation(vector<string> remainingArgs, cv::Mat i) throw(MissingArgs) : imageProcessing(remainingArgs, i) {
	if (remainingArgs.size() < 3) throw MissingArgs(6, filterType); //rotation requires the filterType, the amount to rotate by and the direction of rotation 
}
cv::Mat rotation::process() throw(InvalidOperationException) {
	if (requiredFunctionality == "counterclockwise") requiredNumber = 360 - requiredNumber; //counterclockwise will be the inverse of rotating by the current angle clockwise
	else if (requiredFunctionality == "clockwise") requiredNumber = requiredNumber;
	else { throw InvalidOperationException(requiredFunctionality); } //the final string given by the user is not a valid operation for rotation
	cv::Point2f centre((image.cols - 1) / 2.0, (image.rows - 1) / 2.0); //gets centre of image
	cv::Mat rotate_matrix = getRotationMatrix2D(centre, -requiredNumber, 1.0); //function used to rotate image, negative numbers are used to rotate clockwise
	cv::Mat rotated_image;
	cv::warpAffine(image, rotated_image, rotate_matrix, image.size()); //function required for rotation
	return rotated_image;
}