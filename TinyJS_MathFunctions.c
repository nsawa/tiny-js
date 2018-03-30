//
//	TinyJS - A single-file Javascript-alike engine - Math and Trigonometry functions
//
//	Authored By O.Z.L.B. <ozlbinfo@gmail.com>
//	Copyright (C) 2011 O.Z.L.B.
//
//	Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//	The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
//
//	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
//	IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
#include "clip.h"
//-----------------------------------------------------------------------------
//Math.abs(x) - Returns absolute of given value.
static void scMathAbs(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	double a = TinyJS_Var_getNumber(TinyJS_Var_getParameter(funcRoot, "a"));
	TinyJS_Var_setNumber(TinyJS_Var_getReturnVar(funcRoot), fabs(a));
}
//-----------------------------------------------------------------------------
//Math.round(a) - Returns nearest round of given value.
static void scMathRound(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	double a = TinyJS_Var_getNumber(TinyJS_Var_getParameter(funcRoot, "a"));
	TinyJS_Var_setNumber(TinyJS_Var_getReturnVar(funcRoot), round(a));
}
//-----------------------------------------------------------------------------
//Math.min(a,b) - Returns minimum of two given values.
static void scMathMin(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	double a = TinyJS_Var_getNumber(TinyJS_Var_getParameter(funcRoot, "a"));
	double b = TinyJS_Var_getNumber(TinyJS_Var_getParameter(funcRoot, "b"));
	TinyJS_Var_setNumber(TinyJS_Var_getReturnVar(funcRoot), fmin(a, b));
}
//-----------------------------------------------------------------------------
//Math.max(a,b) - Returns maximum of two given values.
static void scMathMax(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	double a = TinyJS_Var_getNumber(TinyJS_Var_getParameter(funcRoot, "a"));
	double b = TinyJS_Var_getNumber(TinyJS_Var_getParameter(funcRoot, "b"));
	TinyJS_Var_setNumber(TinyJS_Var_getReturnVar(funcRoot), fmax(a, b));
}
//-----------------------------------------------------------------------------
//Math.range(x,a,b) - Returns value limited between two given values.
static void scMathRange(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	double x = TinyJS_Var_getNumber(TinyJS_Var_getParameter(funcRoot, "x"));
	double a = TinyJS_Var_getNumber(TinyJS_Var_getParameter(funcRoot, "a"));
	double b = TinyJS_Var_getNumber(TinyJS_Var_getParameter(funcRoot, "b"));
	TinyJS_Var_setNumber(TinyJS_Var_getReturnVar(funcRoot), (x < a) ? a : (x > b) ? b : x);
}
//-----------------------------------------------------------------------------
//Math.sign(a) - Returns sign of given value (-1==negative,0=zero,1=positive).
static void scMathSign(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	double a = TinyJS_Var_getNumber(TinyJS_Var_getParameter(funcRoot, "a"));
	TinyJS_Var_setNumber(TinyJS_Var_getReturnVar(funcRoot), (a < 0) ? -1 : (a > 0) ? 1 : 0);
}
//-----------------------------------------------------------------------------
//Math.PI() - Returns PI value.
static void scMathPI(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	TinyJS_Var_setNumber(TinyJS_Var_getReturnVar(funcRoot), M_PI);
}
//-----------------------------------------------------------------------------
//Math.toDegrees(a) - Returns degree value of a given angle in radians.
static void scMathToDegrees(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	double a = TinyJS_Var_getNumber(TinyJS_Var_getParameter(funcRoot, "a"));
	TinyJS_Var_setNumber(TinyJS_Var_getReturnVar(funcRoot), a * (180.0 / M_PI));
}
//-----------------------------------------------------------------------------
//Math.toRadians(a) - Returns radians value of a given angle in degrees.
static void scMathToRadians(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	double a = TinyJS_Var_getNumber(TinyJS_Var_getParameter(funcRoot, "a"));
	TinyJS_Var_setNumber(TinyJS_Var_getReturnVar(funcRoot), a * (M_PI / 180.0));
}
//-----------------------------------------------------------------------------
//Math.sin(a) - Returns trig. sine of given angle in radians.
static void scMathSin(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	double a = TinyJS_Var_getNumber(TinyJS_Var_getParameter(funcRoot, "a"));
	TinyJS_Var_setNumber(TinyJS_Var_getReturnVar(funcRoot), sin(a));
}
//-----------------------------------------------------------------------------
//Math.asin(a) - Returns trig. arcsine of given angle in radians.
static void scMathASin(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	double a = TinyJS_Var_getNumber(TinyJS_Var_getParameter(funcRoot, "a"));
	TinyJS_Var_setNumber(TinyJS_Var_getReturnVar(funcRoot), asin(a));
}
//-----------------------------------------------------------------------------
//Math.cos(a) - Returns trig. cosine of given angle in radians.
static void scMathCos(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	double a = TinyJS_Var_getNumber(TinyJS_Var_getParameter(funcRoot, "a"));
	TinyJS_Var_setNumber(TinyJS_Var_getReturnVar(funcRoot), cos(a));
}
//-----------------------------------------------------------------------------
//Math.acos(a) - Returns trig. arccosine of given angle in radians.
static void scMathACos(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	double a = TinyJS_Var_getNumber(TinyJS_Var_getParameter(funcRoot, "a"));
	TinyJS_Var_setNumber(TinyJS_Var_getReturnVar(funcRoot), acos(a));
}
//-----------------------------------------------------------------------------
//Math.tan(a) - Returns trig. tangent of given angle in radians.
static void scMathTan(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	double a = TinyJS_Var_getNumber(TinyJS_Var_getParameter(funcRoot, "a"));
	TinyJS_Var_setNumber(TinyJS_Var_getReturnVar(funcRoot), tan(a));
}
//-----------------------------------------------------------------------------
//Math.atan(a) - Returns trig. arctangent of given angle in radians.
static void scMathATan(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	double a = TinyJS_Var_getNumber(TinyJS_Var_getParameter(funcRoot, "a"));
	TinyJS_Var_setNumber(TinyJS_Var_getReturnVar(funcRoot), atan(a));
}
//-----------------------------------------------------------------------------
//Math.sinh(a) - Returns trig. hyperbolic sine of given angle in radians.
static void scMathSinh(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	double a = TinyJS_Var_getNumber(TinyJS_Var_getParameter(funcRoot, "a"));
	TinyJS_Var_setNumber(TinyJS_Var_getReturnVar(funcRoot), sinh(a));
}
//-----------------------------------------------------------------------------
#if     (defined(_MSC_VER) && (_MSC_VER > 1200))	//Visual C++ .NET
//Math.asinh(a) - Returns trig. hyperbolic arcsine of given angle in radians.
static void scMathASinh(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	double a = TinyJS_Var_getNumber(TinyJS_Var_getParameter(funcRoot, "a"));
	TinyJS_Var_setNumber(TinyJS_Var_getReturnVar(funcRoot), asinh(a));
}
#endif//(defined(_MSC_VER) && (_MSC_VER > 1200))
//-----------------------------------------------------------------------------
//Math.cosh(a) - Returns trig. hyperbolic cosine of given angle in radians.
static void scMathCosh(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	double a = TinyJS_Var_getNumber(TinyJS_Var_getParameter(funcRoot, "a"));
	TinyJS_Var_setNumber(TinyJS_Var_getReturnVar(funcRoot), cosh(a));
}
//-----------------------------------------------------------------------------
#if     (defined(_MSC_VER) && (_MSC_VER > 1200))	//Visual C++ .NET
//Math.acosh(a) - Returns trig. hyperbolic arccosine of given angle in radians.
static void scMathACosh(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	double a = TinyJS_Var_getNumber(TinyJS_Var_getParameter(funcRoot, "a"));
	TinyJS_Var_setNumber(TinyJS_Var_getReturnVar(funcRoot), acosh(a));
}
#endif//(defined(_MSC_VER) && (_MSC_VER > 1200))
//-----------------------------------------------------------------------------
//Math.tanh(a) - Returns trig. hyperbolic tangent of given angle in radians.
static void scMathTanh(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	double a = TinyJS_Var_getNumber(TinyJS_Var_getParameter(funcRoot, "a"));
	TinyJS_Var_setNumber(TinyJS_Var_getReturnVar(funcRoot), tanh(a));
}
//-----------------------------------------------------------------------------
//Math.atan(a) - Returns trig. hyperbolic arctangent of given angle in radians.
static void scMathATanh(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	double a = TinyJS_Var_getNumber(TinyJS_Var_getParameter(funcRoot, "a"));
	TinyJS_Var_setNumber(TinyJS_Var_getReturnVar(funcRoot), atan(a));
}
//-----------------------------------------------------------------------------
//Math.E() - Returns E Neplero value.
static void scMathE(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	TinyJS_Var_setNumber(TinyJS_Var_getReturnVar(funcRoot), M_E);
}
//-----------------------------------------------------------------------------
//Math.log(a) - Returns natural logaritm (base E) of given value.
static void scMathLog(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	double a = TinyJS_Var_getNumber(TinyJS_Var_getParameter(funcRoot, "a"));
	TinyJS_Var_setNumber(TinyJS_Var_getReturnVar(funcRoot), log(a));
}
//-----------------------------------------------------------------------------
//Math.log10(a) - Returns logaritm(base 10) of given value.
static void scMathLog10(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	double a = TinyJS_Var_getNumber(TinyJS_Var_getParameter(funcRoot, "a"));
	TinyJS_Var_setNumber(TinyJS_Var_getReturnVar(funcRoot), log10(a));
}
//-----------------------------------------------------------------------------
//Math.exp(a) - Returns e raised to the power of a given number.
static void scMathExp(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	double a = TinyJS_Var_getNumber(TinyJS_Var_getParameter(funcRoot, "a"));
	TinyJS_Var_setNumber(TinyJS_Var_getReturnVar(funcRoot), exp(a));
}
//-----------------------------------------------------------------------------
//Math.pow(a,b) - Returns the result of a number raised to a power (a)^(b).
static void scMathPow(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	double a = TinyJS_Var_getNumber(TinyJS_Var_getParameter(funcRoot, "a"));
	double b = TinyJS_Var_getNumber(TinyJS_Var_getParameter(funcRoot, "b"));
	TinyJS_Var_setNumber(TinyJS_Var_getReturnVar(funcRoot), pow(a, b));
}
//-----------------------------------------------------------------------------
//Math.sqr(a) - Returns square of given value.
static void scMathSqr(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	double a = TinyJS_Var_getNumber(TinyJS_Var_getParameter(funcRoot, "a"));
	TinyJS_Var_setNumber(TinyJS_Var_getReturnVar(funcRoot), (a * a));
}
//-----------------------------------------------------------------------------
//Math.sqrt(a) - Returns square root of given value.
static void scMathSqrt(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	double a = TinyJS_Var_getNumber(TinyJS_Var_getParameter(funcRoot, "a"));
	TinyJS_Var_setNumber(TinyJS_Var_getReturnVar(funcRoot), sqrt(a));
}
//-----------------------------------------------------------------------------
//Register Functions.
void TinyJS_registerMathFunctions(ST_TinyJS* tinyJS) {
	//Math and Trigonometry functions.
	TinyJS_addNative(tinyJS, "function Math.abs(a)",       scMathAbs,       NULL);
	TinyJS_addNative(tinyJS, "function Math.round(a)",     scMathRound,     NULL);
	TinyJS_addNative(tinyJS, "function Math.min(a,b)",     scMathMin,       NULL);
	TinyJS_addNative(tinyJS, "function Math.max(a,b)",     scMathMax,       NULL);
	TinyJS_addNative(tinyJS, "function Math.range(x,a,b)", scMathRange,     NULL);
	TinyJS_addNative(tinyJS, "function Math.sign(a)",      scMathSign,      NULL);
	TinyJS_addNative(tinyJS, "function Math.PI()",         scMathPI,        NULL);
	TinyJS_addNative(tinyJS, "function Math.toDegrees(a)", scMathToDegrees, NULL);
	TinyJS_addNative(tinyJS, "function Math.toRadians(a)", scMathToRadians, NULL);
	TinyJS_addNative(tinyJS, "function Math.sin(a)",       scMathSin,       NULL);
	TinyJS_addNative(tinyJS, "function Math.asin(a)",      scMathASin,      NULL);
	TinyJS_addNative(tinyJS, "function Math.cos(a)",       scMathCos,       NULL);
	TinyJS_addNative(tinyJS, "function Math.acos(a)",      scMathACos,      NULL);
	TinyJS_addNative(tinyJS, "function Math.tan(a)",       scMathTan,       NULL);
	TinyJS_addNative(tinyJS, "function Math.atan(a)",      scMathATan,      NULL);
	TinyJS_addNative(tinyJS, "function Math.sinh(a)",      scMathSinh,      NULL);
#if     (defined(_MSC_VER) && (_MSC_VER > 1200))	//Visual C++ .NET
	TinyJS_addNative(tinyJS, "function Math.asinh(a)",     scMathASinh,     NULL);
#endif//(defined(_MSC_VER) && (_MSC_VER > 1200))
	TinyJS_addNative(tinyJS, "function Math.cosh(a)",      scMathCosh,      NULL);
#if     (defined(_MSC_VER) && (_MSC_VER > 1200))	//Visual C++ .NET
	TinyJS_addNative(tinyJS, "function Math.acosh(a)",     scMathACosh,     NULL);
#endif//(defined(_MSC_VER) && (_MSC_VER > 1200))
	TinyJS_addNative(tinyJS, "function Math.tanh(a)",      scMathTanh,      NULL);
	TinyJS_addNative(tinyJS, "function Math.atanh(a)",     scMathATanh,     NULL);
	TinyJS_addNative(tinyJS, "function Math.E()",          scMathE,         NULL);
	TinyJS_addNative(tinyJS, "function Math.log(a)",       scMathLog,       NULL);
	TinyJS_addNative(tinyJS, "function Math.log10(a)",     scMathLog10,     NULL);
	TinyJS_addNative(tinyJS, "function Math.exp(a)",       scMathExp,       NULL);
	TinyJS_addNative(tinyJS, "function Math.pow(a,b)",     scMathPow,       NULL);
	TinyJS_addNative(tinyJS, "function Math.sqr(a)",       scMathSqr,       NULL);
	TinyJS_addNative(tinyJS, "function Math.sqrt(a)",      scMathSqrt,      NULL);
}
