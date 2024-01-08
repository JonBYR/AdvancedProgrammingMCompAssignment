#pragma once
#include <string>
#include <exception>
using namespace std;
class InvalidOperationException : public exception {
public:
	InvalidOperationException(string userError);
	//friend ostream& operator<<(ostream& os, const InvalidOperationException& e);
private:
	string message;
};