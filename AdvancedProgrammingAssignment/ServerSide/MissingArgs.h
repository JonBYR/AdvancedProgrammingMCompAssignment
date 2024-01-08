#pragma once
#include <iostream>
#include <exception>
using namespace std;
class MissingArgs : public exception {
public:
    MissingArgs();
    //friend ostream& operator<<(ostream& os, MissingArgs& e);
private:
    string message;
};