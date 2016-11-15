#include "Exception.h"

namespace My {

	Exception::Exception(long long errorCode, std::string errorCause, const char* what) : std::exception(what), errorCode(errorCode), errorCause(std::move(errorCause)) {}
	Exception::Exception(long long errorCode, std::string errorCause) : std::exception("mysqlException"), errorCode(errorCode), errorCause(std::move(errorCause)) {}
	Exception::Exception(std::string errorCause, long long errorCode) : std::exception("mysqlException"), errorCode(errorCode), errorCause(std::move(errorCause)) {}

	std::ostream& operator << (std::ostream& stream, const Exception& ex) {
		stream << "ERROR: Code " << ex.get_errorCode() << " ; Cause \" " << ex.get_errorCause() << "\"" << std::endl;
		return stream;
	}

}