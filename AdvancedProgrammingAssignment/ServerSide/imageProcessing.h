#pragma once
#include <vector>
#include <string>
#include <sstream>
#include <opencv2/opencv.hpp>
#include "InvalidOperationException.h"
#include "NonNumeric.h"
using namespace std;
class imageProcessing {
public:
	imageProcessing(vector<string> remainingArgs, cv::Mat i);
	virtual cv::Mat process() = 0;
protected: //these need to be accessed by child classes
	cv::Mat image;
	string filterType;
	double requiredNumber;
	string requiredFunctionality;
private:
	bool isDouble;
};

inline void doubleCheck(string& arg, double& num) throw(NonNumeric) //not a function call, not tied to class so is still abstract
{
	auto doubleCheck = double();
	auto doubleStringStream = istringstream(arg);
	doubleStringStream >> doubleCheck; //method taken from https://stackoverflow.com/questions/29169153/how-do-i-verify-a-string-is-valid-double-even-if-it-has-a-point-in-it
	if (doubleStringStream.fail()) 
	{
		throw NonNumeric(arg, 2); //if the conversion fails it is not a double and so therefore return false
	}
	else if (doubleStringStream.eof()) 
	{
		num = doubleCheck;
	}
}