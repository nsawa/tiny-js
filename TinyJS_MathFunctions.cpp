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
#include "TinyJS.h"
#include "TinyJS_Functions.h"
#include "TinyJS_MathFunctions.h"
//-----------------------------------------------------------------------------
//Math.abs(x) - Returns absolute of given value.
static void scMathAbs(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	double a = funcRoot->TinyJS_Var_getParameter("a")->TinyJS_Var_getNumber();
	funcRoot->TinyJS_Var_getReturnVar()->TinyJS_Var_setNumber(fabs(a));
}
//-----------------------------------------------------------------------------
//Math.round(a) - Returns nearest round of given value.
static void scMathRound(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	double a = funcRoot->TinyJS_Var_getParameter("a")->TinyJS_Var_getNumber();
	funcRoot->TinyJS_Var_getReturnVar()->TinyJS_Var_setNumber(round(a));
}
//-----------------------------------------------------------------------------
//Math.min(a,b) - Returns minimum of two given values.
static void scMathMin(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	double a = funcRoot->TinyJS_Var_getParameter("a")->TinyJS_Var_getNumber();
	double b = funcRoot->TinyJS_Var_getParameter("b")->TinyJS_Var_getNumber();
	funcRoot->TinyJS_Var_getReturnVar()->TinyJS_Var_setNumber(fmin(a, b));
}
//-----------------------------------------------------------------------------
//Math.max(a,b) - Returns maximum of two given values.
static void scMathMax(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	double a = funcRoot->TinyJS_Var_getParameter("a")->TinyJS_Var_getNumber();
	double b = funcRoot->TinyJS_Var_getParameter("b")->TinyJS_Var_getNumber();
	funcRoot->TinyJS_Var_getReturnVar()->TinyJS_Var_setNumber(fmax(a, b));
}
//-----------------------------------------------------------------------------
//Math.range(x,a,b) - Returns value limited between two given values.
static void scMathRange(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	double x = funcRoot->TinyJS_Var_getParameter("x")->TinyJS_Var_getNumber();
	double a = funcRoot->TinyJS_Var_getParameter("a")->TinyJS_Var_getNumber();
	double b = funcRoot->TinyJS_Var_getParameter("b")->TinyJS_Var_getNumber();
	funcRoot->TinyJS_Var_getReturnVar()->TinyJS_Var_setNumber((x < a) ? a : (x > b) ? b : x);
}
//-----------------------------------------------------------------------------
//Math.sign(a) - Returns sign of given value (-1==negative,0=zero,1=positive).
static void scMathSign(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	double a = funcRoot->TinyJS_Var_getParameter("a")->TinyJS_Var_getNumber();
	funcRoot->TinyJS_Var_getReturnVar()->TinyJS_Var_setNumber((a < 0) ? -1 : (a > 0) ? 1 : 0);
}
//-----------------------------------------------------------------------------
//Math.PI() - Returns PI value.
static void scMathPI(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	funcRoot->TinyJS_Var_getReturnVar()->TinyJS_Var_setNumber(M_PI);
}
//-----------------------------------------------------------------------------
//Math.toDegrees(a) - Returns degree value of a given angle in radians.
static void scMathToDegrees(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	double a = funcRoot->TinyJS_Var_getParameter("a")->TinyJS_Var_getNumber();
	funcRoot->TinyJS_Var_getReturnVar()->TinyJS_Var_setNumber(a * (180.0 / M_PI));
}
//-----------------------------------------------------------------------------
//Math.toRadians(a) - Returns radians value of a given angle in degrees.
static void scMathToRadians(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	double a = funcRoot->TinyJS_Var_getParameter("a")->TinyJS_Var_getNumber();
	funcRoot->TinyJS_Var_getReturnVar()->TinyJS_Var_setNumber(a * (M_PI / 180.0));
}
//-----------------------------------------------------------------------------
//Math.sin(a) - Returns trig. sine of given angle in radians.
static void scMathSin(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	double a = funcRoot->TinyJS_Var_getParameter("a")->TinyJS_Var_getNumber();
	funcRoot->TinyJS_Var_getReturnVar()->TinyJS_Var_setNumber(sin(a));
}
//-----------------------------------------------------------------------------
//Math.asin(a) - Returns trig. arcsine of given angle in radians.
static void scMathASin(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	double a = funcRoot->TinyJS_Var_getParameter("a")->TinyJS_Var_getNumber();
	funcRoot->TinyJS_Var_getReturnVar()->TinyJS_Var_setNumber(asin(a));
}
//-----------------------------------------------------------------------------
//Math.cos(a) - Returns trig. cosine of given angle in radians.
static void scMathCos(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	double a = funcRoot->TinyJS_Var_getParameter("a")->TinyJS_Var_getNumber();
	funcRoot->TinyJS_Var_getReturnVar()->TinyJS_Var_setNumber(cos(a));
}
//-----------------------------------------------------------------------------
//Math.acos(a) - Returns trig. arccosine of given angle in radians.
static void scMathACos(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	double a = funcRoot->TinyJS_Var_getParameter("a")->TinyJS_Var_getNumber();
	funcRoot->TinyJS_Var_getReturnVar()->TinyJS_Var_setNumber(acos(a));
}
//-----------------------------------------------------------------------------
//Math.tan(a) - Returns trig. tangent of given angle in radians.
static void scMathTan(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	double a = funcRoot->TinyJS_Var_getParameter("a")->TinyJS_Var_getNumber();
	funcRoot->TinyJS_Var_getReturnVar()->TinyJS_Var_setNumber(tan(a));
}
//-----------------------------------------------------------------------------
//Math.atan(a) - Returns trig. arctangent of given angle in radians.
static void scMathATan(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	double a = funcRoot->TinyJS_Var_getParameter("a")->TinyJS_Var_getNumber();
	funcRoot->TinyJS_Var_getReturnVar()->TinyJS_Var_setNumber(atan(a));
}
//-----------------------------------------------------------------------------
//Math.sinh(a) - Returns trig. hyperbolic sine of given angle in radians.
static void scMathSinh(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	double a = funcRoot->TinyJS_Var_getParameter("a")->TinyJS_Var_getNumber();
	funcRoot->TinyJS_Var_getReturnVar()->TinyJS_Var_setNumber(sinh(a));
}
//-----------------------------------------------------------------------------
//Math.asinh(a) - Returns trig. hyperbolic arcsine of given angle in radians.
static void scMathASinh(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	double a = funcRoot->TinyJS_Var_getParameter("a")->TinyJS_Var_getNumber();
	funcRoot->TinyJS_Var_getReturnVar()->TinyJS_Var_setNumber(asinh(a));
}
//-----------------------------------------------------------------------------
//Math.cosh(a) - Returns trig. hyperbolic cosine of given angle in radians.
static void scMathCosh(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	double a = funcRoot->TinyJS_Var_getParameter("a")->TinyJS_Var_getNumber();
	funcRoot->TinyJS_Var_getReturnVar()->TinyJS_Var_setNumber(cosh(a));
}
//-----------------------------------------------------------------------------
//Math.acosh(a) - Returns trig. hyperbolic arccosine of given angle in radians.
static void scMathACosh(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	double a = funcRoot->TinyJS_Var_getParameter("a")->TinyJS_Var_getNumber();
	funcRoot->TinyJS_Var_getReturnVar()->TinyJS_Var_setNumber(acosh(a));
}
//-----------------------------------------------------------------------------
//Math.tanh(a) - Returns trig. hyperbolic tangent of given angle in radians.
static void scMathTanh(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	double a = funcRoot->TinyJS_Var_getParameter("a")->TinyJS_Var_getNumber();
	funcRoot->TinyJS_Var_getReturnVar()->TinyJS_Var_setNumber(tanh(a));
}
//-----------------------------------------------------------------------------
//Math.atan(a) - Returns trig. hyperbolic arctangent of given angle in radians.
static void scMathATanh(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	double a = funcRoot->TinyJS_Var_getParameter("a")->TinyJS_Var_getNumber();
	funcRoot->TinyJS_Var_getReturnVar()->TinyJS_Var_setNumber(atan(a));
}
//-----------------------------------------------------------------------------
//Math.E() - Returns E Neplero value.
static void scMathE(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	funcRoot->TinyJS_Var_getReturnVar()->TinyJS_Var_setNumber(M_E);
}
//-----------------------------------------------------------------------------
//Math.log(a) - Returns natural logaritm (base E) of given value.
static void scMathLog(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	double a = funcRoot->TinyJS_Var_getParameter("a")->TinyJS_Var_getNumber();
	funcRoot->TinyJS_Var_getReturnVar()->TinyJS_Var_setNumber(log(a));
}
//-----------------------------------------------------------------------------
//Math.log10(a) - Returns logaritm(base 10) of given value.
static void scMathLog10(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	double a = funcRoot->TinyJS_Var_getParameter("a")->TinyJS_Var_getNumber();
	funcRoot->TinyJS_Var_getReturnVar()->TinyJS_Var_setNumber(log10(a));
}
//-----------------------------------------------------------------------------
//Math.exp(a) - Returns e raised to the power of a given number.
static void scMathExp(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	double a = funcRoot->TinyJS_Var_getParameter("a")->TinyJS_Var_getNumber();
	funcRoot->TinyJS_Var_getReturnVar()->TinyJS_Var_setNumber(exp(a));
}
//-----------------------------------------------------------------------------
//Math.pow(a,b) - Returns the result of a number raised to a power (a)^(b).
static void scMathPow(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	double a = funcRoot->TinyJS_Var_getParameter("a")->TinyJS_Var_getNumber();
	double b = funcRoot->TinyJS_Var_getParameter("b")->TinyJS_Var_getNumber();
	funcRoot->TinyJS_Var_getReturnVar()->TinyJS_Var_setNumber(pow(a, b));
}
//-----------------------------------------------------------------------------
//Math.sqr(a) - Returns square of given value.
static void scMathSqr(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	double a = funcRoot->TinyJS_Var_getParameter("a")->TinyJS_Var_getNumber();
	funcRoot->TinyJS_Var_getReturnVar()->TinyJS_Var_setNumber((a * a));
}
//-----------------------------------------------------------------------------
//Math.sqrt(a) - Returns square root of given value.
static void scMathSqrt(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	double a = funcRoot->TinyJS_Var_getParameter("a")->TinyJS_Var_getNumber();
	funcRoot->TinyJS_Var_getReturnVar()->TinyJS_Var_setNumber(sqrt(a));
}
//-----------------------------------------------------------------------------
//Register Functions.
void TinyJS_registerMathFunctions(ST_TinyJS* tinyJS) {
	//Math and Trigonometry functions.
	tinyJS->TinyJS_addNative("function Math.abs(a)",       scMathAbs,       NULL);
	tinyJS->TinyJS_addNative("function Math.round(a)",     scMathRound,     NULL);
	tinyJS->TinyJS_addNative("function Math.min(a,b)",     scMathMin,       NULL);
	tinyJS->TinyJS_addNative("function Math.max(a,b)",     scMathMax,       NULL);
	tinyJS->TinyJS_addNative("function Math.range(x,a,b)", scMathRange,     NULL);
	tinyJS->TinyJS_addNative("function Math.sign(a)",      scMathSign,      NULL);
	tinyJS->TinyJS_addNative("function Math.PI()",         scMathPI,        NULL);
	tinyJS->TinyJS_addNative("function Math.toDegrees(a)", scMathToDegrees, NULL);
	tinyJS->TinyJS_addNative("function Math.toRadians(a)", scMathToRadians, NULL);
	tinyJS->TinyJS_addNative("function Math.sin(a)",       scMathSin,       NULL);
	tinyJS->TinyJS_addNative("function Math.asin(a)",      scMathASin,      NULL);
	tinyJS->TinyJS_addNative("function Math.cos(a)",       scMathCos,       NULL);
	tinyJS->TinyJS_addNative("function Math.acos(a)",      scMathACos,      NULL);
	tinyJS->TinyJS_addNative("function Math.tan(a)",       scMathTan,       NULL);
	tinyJS->TinyJS_addNative("function Math.atan(a)",      scMathATan,      NULL);
	tinyJS->TinyJS_addNative("function Math.sinh(a)",      scMathSinh,      NULL);
	tinyJS->TinyJS_addNative("function Math.asinh(a)",     scMathASinh,     NULL);
	tinyJS->TinyJS_addNative("function Math.cosh(a)",      scMathCosh,      NULL);
	tinyJS->TinyJS_addNative("function Math.acosh(a)",     scMathACosh,     NULL);
	tinyJS->TinyJS_addNative("function Math.tanh(a)",      scMathTanh,      NULL);
	tinyJS->TinyJS_addNative("function Math.atanh(a)",     scMathATanh,     NULL);
	tinyJS->TinyJS_addNative("function Math.E()",          scMathE,         NULL);
	tinyJS->TinyJS_addNative("function Math.log(a)",       scMathLog,       NULL);
	tinyJS->TinyJS_addNative("function Math.log10(a)",     scMathLog10,     NULL);
	tinyJS->TinyJS_addNative("function Math.exp(a)",       scMathExp,       NULL);
	tinyJS->TinyJS_addNative("function Math.pow(a,b)",     scMathPow,       NULL);
	tinyJS->TinyJS_addNative("function Math.sqr(a)",       scMathSqr,       NULL);
	tinyJS->TinyJS_addNative("function Math.sqrt(a)",      scMathSqrt,      NULL);
}
