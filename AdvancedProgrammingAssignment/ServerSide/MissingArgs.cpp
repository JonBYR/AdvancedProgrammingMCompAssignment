#include "MissingArgs.h"
MissingArgs::MissingArgs(int num, const string& s) {
	message = s;
	position = num;
}

ostream& operator<<(ostream& os, const MissingArgs& e) 
{
	os << "Operation " << e.message << " requires " << e.position << " arguments";
	return os;
}
