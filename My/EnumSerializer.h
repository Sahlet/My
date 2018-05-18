//EnumSerialier.h

#ifndef ENUMSERIALIER_H_
#define ENUMSERIALIER_H_

#include <template_helpers.h>

template<class T>
struct EnumSerializer;

#define ENUM_ELEMENT_DESCRIPTION(ARG1) ARG1,
#define ENUM_CONVERSION_FROM_STRING(ARG1) if (s == #ARG1) return enum_name::ARG1;
#define ENUM_CONVERSION_TO_STRING(ARG1) if (s == enum_name::ARG1) return #ARG1;
#define EMPTY_DEFINE(...)
#define PASS_THE_SECOND_ARG(DEF, ARG2, ...) DEF(ARG2)

#define make_ENUM(ENUM_NAME, ...)																								\
enum class ENUM_NAME {																											\
	__VA_ARGS__																													\
};																																\
																																\
template<>																														\
struct EnumSerializer< ENUM_NAME > {																							\
	typedef ENUM_NAME enum_name;																								\
	static enum_name from_string(std::string s) {																				\
		THELPER_CALL_FOR_EACH(PASS_THE_SECOND_ARG, ENUM_CONVERSION_FROM_STRING, EMPTY_DEFINE, __VA_ARGS__);						\
		LOGE("FIND ME in '%s': unknown element '%s' of enum '"#ENUM_NAME"'", __PRETTY_FUNCTION__, s.c_str());					\
		return enum_name();																										\
	}																															\
	static std::string to_string(enum_name s) {																					\
		THELPER_CALL_FOR_EACH(PASS_THE_SECOND_ARG, ENUM_CONVERSION_TO_STRING, EMPTY_DEFINE, __VA_ARGS__);						\
		return "ERROR";																											\
	}																															\
};

#define make_ENUM_2(ENUM_NAME, ENUM_ELEMENTS_LIST)																				\
enum class ENUM_NAME {																											\
	ENUM_ELEMENTS_LIST(ENUM_ELEMENT_DESCRIPTION)																				\
};																																\
																																\
template<>																														\
struct EnumSerializer< ENUM_NAME > {																							\
	typedef ENUM_NAME enum_name;																								\
	static enum_name from_string(std::string s) {																				\
		ENUM_ELEMENTS_LIST(ENUM_CONVERSION_FROM_STRING);																		\
		LOGE("FIND ME in '%s': unknown element '%s' of enum '"#ENUM_NAME"'", __PRETTY_FUNCTION__, s.c_str());					\
		return enum_name();																										\
	}																															\
	static std::string to_string(enum_name s) {																					\
		ENUM_ELEMENTS_LIST(ENUM_CONVERSION_TO_STRING);																			\
		return "ERROR";																											\
	}																															\
};

#endif /* ENUMSERIALIER_H_ */
