#pragma once
#include <iostream>
#include <string>
#include <exception>
using namespace std;
class MissingArgs : public exception {
public:
    MissingArgs(int num, const string& s);
    friend ostream& operator<<(ostream& os, const MissingArgs& e);
private:
    string message;
    int position;
};