#pragma once
#include <string>
#include <exception>
using namespace std;
class NonNumeric : public exception {
public:
    NonNumeric(string notNumber, int pos);
    //friend ostream& operator<<(ostream& os, const NonNumeric& e);
private:
    string error;
    int position;
};