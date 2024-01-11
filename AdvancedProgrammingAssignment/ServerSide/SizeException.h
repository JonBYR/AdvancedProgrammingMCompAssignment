#pragma once
#include <iostream>
#include <exception>
#include <string>
using namespace std;
class SizeException : public exception
{
public:
	SizeException(string m);
	friend ostream& operator<<(ostream& os, const SizeException& e);
private:
	string message;
};

