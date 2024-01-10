#include "InvalidOperationException.h"
InvalidOperationException::InvalidOperationException(string userError) {
	message = userError; //this will allow any child class to impliment this custom exception for any mis inputs
}
ostream& operator<<(ostream& os, const InvalidOperationException& e)
{
	os << "Operation of type " << e.message << " is not valid!";
	return os;
}


