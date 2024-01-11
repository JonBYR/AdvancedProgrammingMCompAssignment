#include "FlipException.h"
FlipException::FlipException(double num) {
	number = num;
}
ostream& operator<<(ostream& os, const FlipException& e) { //friend functions are allowed to access private variables of a class outside of class
	os << "Operation flip requires numbers 0, 1 or -1, number " << e.number << " is not valid"; //this exception is only called for the flip operation
	return os;
}