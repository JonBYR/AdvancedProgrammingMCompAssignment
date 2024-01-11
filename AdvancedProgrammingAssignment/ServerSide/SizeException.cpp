#include "SizeException.h"
SizeException::SizeException(string m) {
	message = m;
}
ostream& operator<<(ostream& os, const SizeException& e)
{
	os << "Value at position 6 of size operation " << e.message << " must only be an integer!"; //exception called only for size operation
	return os;  
}