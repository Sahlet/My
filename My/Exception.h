#pragma once
#include <exception>
#include <string>
#include <ostream>

namespace My {

	class Exception : public std::exception {
		long long errorCode;
		std::string errorCause;
	protected:
		Exception::Exception(long long errorCode, std::string errorCause, const char* what);
	public:
		Exception(long long errorCode = 0, std::string errorCause = "");
		Exception(std::string errorCause, long long errorCode = 0);
		Exception(Exception&&) = default;
		inline long long get_errorCode() const { return errorCode; }
		inline const std::string& get_errorCause() const { return errorCause; }
	};

	std::ostream& operator << (std::ostream& stream, const Exception& ex);

}