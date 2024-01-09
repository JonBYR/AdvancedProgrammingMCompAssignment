#include "NonNumeric.h"
NonNumeric::NonNumeric(string notNumber, int pos) {
    error = notNumber;
    position = pos;
}
/*
ostream& operator<<(ostream& os, const NonNumeric& e)
{
	os << e.error;
	return os;
}
*/
