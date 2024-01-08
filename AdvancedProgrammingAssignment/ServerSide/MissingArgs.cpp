#include "MissingArgs.h"
MissingArgs::MissingArgs() {
	message = "There are too few parameters given for your filter!";
}
/*
ostream& operator<<(ostream& os, MissingArgs& e) 
{
	os << e.message;
	return os;
}
*/