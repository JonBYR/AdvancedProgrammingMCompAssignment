#include "imageProcessing.h"
ImageProcessing::ImageProcessing(vector<string> remainingArgs, cv::Mat i) {
	if (remainingArgs.size() == 1) 
	{
		filterType = remainingArgs[0];
	}
	else if(remainingArgs.size() == 2)
	{
		filterType = remainingArgs[0];
		if (filterType == "box" || filterType == "greyscale" || filterType == "sharpening");
		//these operations require one argument so double check does not apply to them
		else doubleCheck(remainingArgs[1], requiredNumber); //calls inline function, meaning that all lines within the function are the next lines executed by the else statement
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

