#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <opencv2/opencv.hpp>
#include "InvalidOperationException.h"
#include "NonNumeric.h"
#include "MissingArgs.h"
#include "FlipException.h"
#include "SizeException.h"
using namespace std; //all includes and namespaces are inherited by child classes, including exceptions
class imageProcessing {
public:
	imageProcessing(vector<string> remainingArgs, cv::Mat i);
	virtual cv::Mat process() = 0;
protected: //these need to be accessed by child classes
	cv::Mat image; //some child classes will require the original image
	string filterType; //filterType refers to the type of filter operation i.e rotation
	double requiredNumber; //requiredNumber is the number that is required for the filter operation, i.e 90 means rotate by 90 degrees
	string requiredFunctionality; //requiredFunctionality is any remaining information needed for a filter i.e rotating 90 degrees clockwise. Only needed for rotation and resizing
private:
	bool isDouble;
};

inline void doubleCheck(string& arg, double& num) throw(NonNumeric) //not a function call, not tied to class so class is still abstract
{ //inline functions are performed at compile time and do not have function call overhead (execution time to call function)
	auto doubleCheck = double();
	auto doubleStringStream = istringstream(arg);
	doubleStringStream >> doubleCheck; //method taken from https://stackoverflow.com/questions/29169153/how-do-i-verify-a-string-is-valid-double-even-if-it-has-a-point-in-it
	if (doubleStringStream.fail()) 
	{
		throw NonNumeric(arg, 5); //if the conversion fails it is not a double, when all arguments at position 5 should be a double, so error is thrown to detail this
	}
	else if (doubleStringStream.eof()) 
	{
		num = doubleCheck;
	}
}