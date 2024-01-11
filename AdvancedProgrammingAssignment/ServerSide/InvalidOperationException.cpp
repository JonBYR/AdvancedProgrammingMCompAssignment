#include "InvalidOperationException.h"
InvalidOperationException::InvalidOperationException(string userError) {
	message = userError; //this will allow any child class to impliment this custom exception for any misinputs
}
ostream& operator<<(ostream& os, const InvalidOperationException& e)
{
	os << "Operation of type " << e.message << " is not valid!"; //this exception will be applied to any erroneous arguments, such as filterType or requiredFunctionality
	return os; //a return of type ostream is needed. 
}


