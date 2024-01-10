#include "NonNumeric.h"
NonNumeric::NonNumeric(string notNumber, int pos) {
    error = notNumber;
    position = pos;
}

ostream& operator<<(ostream& os, const NonNumeric& e)
{
	os << "At position" << e.position << " " << e.error << " should be numeric!";
	return os;
}

