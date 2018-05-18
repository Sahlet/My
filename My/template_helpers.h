#ifndef _TEMPLATE_HELPERS_H_
#define _TEMPLATE_HELPERS_H_
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#define THELPER_F0(...)
#define THELPER_F1(F, S, div, arg) F(S, arg)
#define THELPER_F2(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_F1(F, S, div, __VA_ARGS__)
#define THELPER_F3(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_F2(F, S, div, __VA_ARGS__)
#define THELPER_F4(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_F3(F, S, div, __VA_ARGS__)
#define THELPER_F5(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_F4(F, S, div, __VA_ARGS__)
#define THELPER_F6(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_F5(F, S, div, __VA_ARGS__)
#define THELPER_F7(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_F6(F, S, div, __VA_ARGS__)
#define THELPER_F8(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_F7(F, S, div, __VA_ARGS__)
#define THELPER_F9(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_F8(F, S, div, __VA_ARGS__)
#define THELPER_F10(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_F9(F, S, div, __VA_ARGS__)
#define THELPER_F11(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_F10(F, S, div, __VA_ARGS__)
#define THELPER_F12(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_F11(F, S, div, __VA_ARGS__)
#define THELPER_F13(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_F12(F, S, div, __VA_ARGS__)
#define THELPER_F14(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_F13(F, S, div, __VA_ARGS__)
#define THELPER_F15(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_F14(F, S, div, __VA_ARGS__)
#define THELPER_F16(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_F15(F, S, div, __VA_ARGS__)
#define THELPER_F17(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_F16(F, S, div, __VA_ARGS__)
#define THELPER_F18(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_F17(F, S, div, __VA_ARGS__)
#define THELPER_F19(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_F18(F, S, div, __VA_ARGS__)
#define THELPER_F20(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_F19(F, S, div, __VA_ARGS__)
#define THELPER_F21(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_F20(F, S, div, __VA_ARGS__)
#define THELPER_F22(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_F21(F, S, div, __VA_ARGS__)
#define THELPER_F23(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_F22(F, S, div, __VA_ARGS__)
#define THELPER_F24(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_F23(F, S, div, __VA_ARGS__)
#define THELPER_F25(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_F24(F, S, div, __VA_ARGS__)
#define THELPER_F26(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_F25(F, S, div, __VA_ARGS__)
#define THELPER_F27(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_F26(F, S, div, __VA_ARGS__)
#define THELPER_F28(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_F27(F, S, div, __VA_ARGS__)
#define THELPER_F29(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_F28(F, S, div, __VA_ARGS__)
#define THELPER_F30(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_F29(F, S, div, __VA_ARGS__)
#define THELPER_F31(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_F30(F, S, div, __VA_ARGS__)
#define THELPER_F32(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_F31(F, S, div, __VA_ARGS__)
#define THELPER_F33(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_F32(F, S, div, __VA_ARGS__)
#define THELPER_F34(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_F33(F, S, div, __VA_ARGS__)
#define THELPER_F35(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_F34(F, S, div, __VA_ARGS__)
#define THELPER_F36(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_F35(F, S, div, __VA_ARGS__)
#define THELPER_F37(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_F36(F, S, div, __VA_ARGS__)
#define THELPER_F38(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_F37(F, S, div, __VA_ARGS__)
#define THELPER_F39(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_F38(F, S, div, __VA_ARGS__)
#define THELPER_F40(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_F39(F, S, div, __VA_ARGS__)
#define THELPER_F41(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_F40(F, S, div, __VA_ARGS__)
#define THELPER_F42(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_F41(F, S, div, __VA_ARGS__)
#define THELPER_F43(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_F42(F, S, div, __VA_ARGS__)
#define THELPER_F44(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_F43(F, S, div, __VA_ARGS__)
#define THELPER_F45(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_F44(F, S, div, __VA_ARGS__)
#define THELPER_F46(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_F45(F, S, div, __VA_ARGS__)
#define THELPER_F47(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_F46(F, S, div, __VA_ARGS__)
#define THELPER_F48(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_F47(F, S, div, __VA_ARGS__)
#define THELPER_F49(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_F48(F, S, div, __VA_ARGS__)
#define THELPER_F50(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_F49(F, S, div, __VA_ARGS__)
#define THELPER_F51(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_F50(F, S, div, __VA_ARGS__)
#define THELPER_F52(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_F51(F, S, div, __VA_ARGS__)
#define THELPER_F53(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_F52(F, S, div, __VA_ARGS__)
#define THELPER_F54(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_F53(F, S, div, __VA_ARGS__)
#define THELPER_F55(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_F54(F, S, div, __VA_ARGS__)
#define THELPER_F56(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_F55(F, S, div, __VA_ARGS__)
#define THELPER_F57(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_F56(F, S, div, __VA_ARGS__)
#define THELPER_F58(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_F57(F, S, div, __VA_ARGS__)
#define THELPER_F59(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_F58(F, S, div, __VA_ARGS__)
#define THELPER_F60(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_F59(F, S, div, __VA_ARGS__)
#define THELPER_F61(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_F60(F, S, div, __VA_ARGS__)
#define THELPER_F62(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_F61(F, S, div, __VA_ARGS__)
#define THELPER_F63(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_F62(F, S, div, __VA_ARGS__)
#define THELPER_F64(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_F63(F, S, div, __VA_ARGS__)

#define THELPER_INDEXOF(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, ...) _64
#define THELPER_CALL_FOR_EACH(F, S, div, ...) THELPER_CALL_FOR_EACH_HIDDEN(F, S, div, __VA_ARGS__)
#define THELPER_CALL_FOR_EACH_HIDDEN(F, S, div, ...) THELPER_INDEXOF(__VA_ARGS__, \
		THELPER_F64, THELPER_F63, THELPER_F62, THELPER_F61, THELPER_F60, \
		THELPER_F59, THELPER_F58, THELPER_F57, THELPER_F56, THELPER_F55, THELPER_F54, THELPER_F53, THELPER_F52, THELPER_F51, THELPER_F50, \
		THELPER_F49, THELPER_F48, THELPER_F47, THELPER_F46, THELPER_F45, THELPER_F44, THELPER_F43, THELPER_F42, THELPER_F41, THELPER_F40, \
		THELPER_F39, THELPER_F38, THELPER_F37, THELPER_F36, THELPER_F35, THELPER_F34, THELPER_F33, THELPER_F32, THELPER_F31, THELPER_F30, \
		THELPER_F29, THELPER_F28, THELPER_F27, THELPER_F26, THELPER_F25, THELPER_F24, THELPER_F23, THELPER_F22, THELPER_F21, THELPER_F20, \
		THELPER_F19, THELPER_F18, THELPER_F17, THELPER_F16, THELPER_F15, THELPER_F14, THELPER_F13, THELPER_F12, THELPER_F11, THELPER_F10, \
		THELPER_F9, THELPER_F8, THELPER_F7, THELPER_F6, THELPER_F5, THELPER_F4, THELPER_F3, THELPER_F2, THELPER_F1, THELPER_F0)\
		(F, S, div, __VA_ARGS__)

//-----------------------------------------------------------------------------
#define THELPER_FI0(...)
#define THELPER_FI1(F, S, div, arg) F(S, arg)
#define THELPER_FI2(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_FI1(F, S, div, __VA_ARGS__)
#define THELPER_FI3(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_FI2(F, S, div, __VA_ARGS__)
#define THELPER_FI4(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_FI3(F, S, div, __VA_ARGS__)
#define THELPER_FI5(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_FI4(F, S, div, __VA_ARGS__)
#define THELPER_FI6(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_FI5(F, S, div, __VA_ARGS__)
#define THELPER_FI7(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_FI6(F, S, div, __VA_ARGS__)
#define THELPER_FI8(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_FI7(F, S, div, __VA_ARGS__)
#define THELPER_FI9(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_FI8(F, S, div, __VA_ARGS__)
#define THELPER_FI10(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_FI9(F, S, div, __VA_ARGS__)
#define THELPER_FI11(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_FI10(F, S, div, __VA_ARGS__)
#define THELPER_FI12(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_FI11(F, S, div, __VA_ARGS__)
#define THELPER_FI13(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_FI12(F, S, div, __VA_ARGS__)
#define THELPER_FI14(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_FI13(F, S, div, __VA_ARGS__)
#define THELPER_FI15(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_FI14(F, S, div, __VA_ARGS__)
#define THELPER_FI16(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_FI15(F, S, div, __VA_ARGS__)
#define THELPER_FI17(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_FI16(F, S, div, __VA_ARGS__)
#define THELPER_FI18(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_FI17(F, S, div, __VA_ARGS__)
#define THELPER_FI19(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_FI18(F, S, div, __VA_ARGS__)
#define THELPER_FI20(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_FI19(F, S, div, __VA_ARGS__)
#define THELPER_FI21(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_FI20(F, S, div, __VA_ARGS__)
#define THELPER_FI22(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_FI21(F, S, div, __VA_ARGS__)
#define THELPER_FI23(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_FI22(F, S, div, __VA_ARGS__)
#define THELPER_FI24(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_FI23(F, S, div, __VA_ARGS__)
#define THELPER_FI25(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_FI24(F, S, div, __VA_ARGS__)
#define THELPER_FI26(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_FI25(F, S, div, __VA_ARGS__)
#define THELPER_FI27(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_FI26(F, S, div, __VA_ARGS__)
#define THELPER_FI28(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_FI27(F, S, div, __VA_ARGS__)
#define THELPER_FI29(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_FI28(F, S, div, __VA_ARGS__)
#define THELPER_FI30(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_FI29(F, S, div, __VA_ARGS__)
#define THELPER_FI31(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_FI30(F, S, div, __VA_ARGS__)
#define THELPER_FI32(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_FI31(F, S, div, __VA_ARGS__)
#define THELPER_FI33(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_FI32(F, S, div, __VA_ARGS__)
#define THELPER_FI34(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_FI33(F, S, div, __VA_ARGS__)
#define THELPER_FI35(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_FI34(F, S, div, __VA_ARGS__)
#define THELPER_FI36(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_FI35(F, S, div, __VA_ARGS__)
#define THELPER_FI37(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_FI36(F, S, div, __VA_ARGS__)
#define THELPER_FI38(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_FI37(F, S, div, __VA_ARGS__)
#define THELPER_FI39(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_FI38(F, S, div, __VA_ARGS__)
#define THELPER_FI40(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_FI39(F, S, div, __VA_ARGS__)
#define THELPER_FI41(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_FI40(F, S, div, __VA_ARGS__)
#define THELPER_FI42(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_FI41(F, S, div, __VA_ARGS__)
#define THELPER_FI43(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_FI42(F, S, div, __VA_ARGS__)
#define THELPER_FI44(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_FI43(F, S, div, __VA_ARGS__)
#define THELPER_FI45(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_FI44(F, S, div, __VA_ARGS__)
#define THELPER_FI46(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_FI45(F, S, div, __VA_ARGS__)
#define THELPER_FI47(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_FI46(F, S, div, __VA_ARGS__)
#define THELPER_FI48(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_FI47(F, S, div, __VA_ARGS__)
#define THELPER_FI49(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_FI48(F, S, div, __VA_ARGS__)
#define THELPER_FI50(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_FI49(F, S, div, __VA_ARGS__)
#define THELPER_FI51(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_FI50(F, S, div, __VA_ARGS__)
#define THELPER_FI52(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_FI51(F, S, div, __VA_ARGS__)
#define THELPER_FI53(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_FI52(F, S, div, __VA_ARGS__)
#define THELPER_FI54(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_FI53(F, S, div, __VA_ARGS__)
#define THELPER_FI55(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_FI54(F, S, div, __VA_ARGS__)
#define THELPER_FI56(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_FI55(F, S, div, __VA_ARGS__)
#define THELPER_FI57(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_FI56(F, S, div, __VA_ARGS__)
#define THELPER_FI58(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_FI57(F, S, div, __VA_ARGS__)
#define THELPER_FI59(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_FI58(F, S, div, __VA_ARGS__)
#define THELPER_FI60(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_FI59(F, S, div, __VA_ARGS__)
#define THELPER_FI61(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_FI60(F, S, div, __VA_ARGS__)
#define THELPER_FI62(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_FI61(F, S, div, __VA_ARGS__)
#define THELPER_FI63(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_FI62(F, S, div, __VA_ARGS__)
#define THELPER_FI64(F, S, div, arg, ...) F(S, arg) div(S) \
	THELPER_FI63(F, S, div, __VA_ARGS__)

#define THELPER_IINDEXOF(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, ...) _64
#define THELPER_CALL_FOR_EACH2(F, S, div, ...) THELPER_CALL_FOR_EACH2_HIDDEN(F, S, div, __VA_ARGS__)
#define THELPER_CALL_FOR_EACH2_HIDDEN(F, S, div, ...) THELPER_IINDEXOF(__VA_ARGS__, \
		THELPER_FI64, THELPER_FI63, THELPER_FI62, THELPER_FI61, THELPER_FI60, \
		THELPER_FI59, THELPER_FI58, THELPER_FI57, THELPER_FI56, THELPER_FI55, THELPER_FI54, THELPER_FI53, THELPER_FI52, THELPER_FI51, THELPER_FI50, \
		THELPER_FI49, THELPER_FI48, THELPER_FI47, THELPER_FI46, THELPER_FI45, THELPER_FI44, THELPER_FI43, THELPER_FI42, THELPER_FI41, THELPER_FI40, \
		THELPER_FI39, THELPER_FI38, THELPER_FI37, THELPER_FI36, THELPER_FI35, THELPER_FI34, THELPER_FI33, THELPER_FI32, THELPER_FI31, THELPER_FI30, \
		THELPER_FI29, THELPER_FI28, THELPER_FI27, THELPER_FI26, THELPER_FI25, THELPER_FI24, THELPER_FI23, THELPER_FI22, THELPER_FI21, THELPER_FI20, \
		THELPER_FI19, THELPER_FI18, THELPER_FI17, THELPER_FI16, THELPER_FI15, THELPER_FI14, THELPER_FI13, THELPER_FI12, THELPER_FI11, THELPER_FI10, \
		THELPER_FI9, THELPER_FI8, THELPER_FI7, THELPER_FI6, THELPER_FI5, THELPER_FI4, THELPER_FI3, THELPER_FI2, THELPER_FI1, THELPER_FI0)\
		(F, S, div, __VA_ARGS__)

//-----------------------------------------------------------------------------
#define THELPER_COUNT(...) THELPER_INDEXOF(__VA_ARGS__, 64, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)

//-----------------------------------------------------------------------------
#define THELPER_STRINGIFY(a) THELPER_STRINGIFY_HIDDEN(a)
#define THELPER_STRINGIFY_HIDDEN(a) #a

//-----------------------------------------------------------------------------
#define THELPER_CONCAT(a, b) THELPER_CONCAT_HIDDEN(a, b)
#define THELPER_CONCAT_HIDDEN(a, b) a ## b

//-----------------------------------------------------------------------------
#endif
