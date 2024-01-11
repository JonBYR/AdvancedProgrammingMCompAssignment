#include "imageProcessing.h"
imageProcessing::imageProcessing(vector<string> remainingArgs, cv::Mat i) {
	if (remainingArgs.size() == 1) 
	{
		filterType = remainingArgs[0];
	}
	else if(remainingArgs.size() == 2)
	{
		filterType = remainingArgs[0];
		if (filterType == "box" || filterType == "greyscale" || filterType == "sharpening");
		//these operations require one argument so double check does not apply to them
		else doubleCheck(remainingArgs[1], requiredNumber);
	}
	else 
	{
		filterType = remainingArgs[0];
		if (filterType == "box" || filterType == "greyscale" || filterType == "sharpening");
		else doubleCheck(remainingArgs[1], requiredNumber);;
		requiredFunctionality = remainingArgs[2];
	}
	image = i;
}

