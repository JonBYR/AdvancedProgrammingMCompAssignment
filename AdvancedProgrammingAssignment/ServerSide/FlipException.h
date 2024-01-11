#pragma once
#include <iostream>
#include <string>
#include <exception>
using namespace std;
class FlipException : public exception
{
public:
	FlipException(double num);
	friend ostream& operator<<(ostream& os, const FlipException& e);
private:
	double number;
};

