#pragma once
#include "imageProcessing.h"
class Rotation : public ImageProcessing {
public:
	Rotation(vector<string> remainingArgs, cv::Mat i) throw(MissingArgs);
	virtual cv::Mat process() throw(InvalidOperationException);
};