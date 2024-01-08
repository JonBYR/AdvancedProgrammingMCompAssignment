#pragma once
#include "imageProcessing.h"
class rotation : public imageProcessing {
public:
	rotation(vector<string> remainingArgs, cv::Mat i);
	~rotation();
	virtual cv::Mat process() throw(InvalidOperationException);
};