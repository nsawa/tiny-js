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
#include <math.h>
#include <cstdlib>
#include <sstream>
#include "TinyJS.h"
#include "TinyJS_Functions.h"
#include "TinyJS_MathFunctions.h"
using namespace std;
//-----------------------------------------------------------------------------
#define k_E			exp(1.0)
#define k_PI			3.1415926535897932384626433832795

#define F_ABS(a)		((a)>=0 ? (a) : (-(a)))
#define F_MIN(a,b)		((a)>(b) ? (b) : (a))
#define F_MAX(a,b)		((a)>(b) ? (a) : (b))
#define F_SGN(a)		((a)>0 ? 1 : ((a)<0 ? -1 : 0))
#define F_RNG(a,min,max)	((a)<(min) ? min : ((a)>(max) ? max : a))
#define F_ROUND(a)		((a)>0 ? (int) ((a)+0.5) : (int) ((a)-0.5))

//CScriptVar shortcut macro.
#define scIsInt(a)		(v->getParameter(a)->isInt())
#define scIsDouble(a)		(v->getParameter(a)->isDouble())
#define scGetInt(a)		(v->getParameter(a)->getInt())
#define scGetDouble(a)		(v->getParameter(a)->getDouble())
#define scReturnInt(a)		(v->getReturnVar()->setInt(a))
#define scReturnDouble(a)	(v->getReturnVar()->setDouble(a))
//-----------------------------------------------------------------------------
//Math.abs(x) - Returns absolute of given value.
static void scMathAbs(CTinyJS* tinyJS, CScriptVar* v, void* userdata) {
	if(scIsInt("a")) {
		scReturnInt(F_ABS(scGetInt("a")));
	} else if(scIsDouble("a")) {
		scReturnDouble(F_ABS(scGetDouble("a")));
	}
}
//Math.round(a) - Returns nearest round of given value.
static void scMathRound(CTinyJS* tinyJS, CScriptVar* v, void* userdata) {
	if(scIsInt("a")) {
		scReturnInt(F_ROUND(scGetInt("a")));
	} else if(scIsDouble("a")) {
		scReturnDouble(F_ROUND(scGetDouble("a")));
	}
}
//Math.min(a,b) - Returns minimum of two given values.
static void scMathMin(CTinyJS* tinyJS, CScriptVar* v, void* userdata) {
	if((scIsInt("a")) && (scIsInt("b"))) {
		scReturnInt(F_MIN(scGetInt("a"), scGetInt("b")));
	} else {
		scReturnDouble(F_MIN(scGetDouble("a"), scGetDouble("b")));
	}
}
//Math.max(a,b) - Returns maximum of two given values.
static void scMathMax(CTinyJS* tinyJS, CScriptVar* v, void* userdata) {
	if((scIsInt("a")) && (scIsInt("b"))) {
		scReturnInt(F_MAX(scGetInt("a"), scGetInt("b")));
	} else {
		scReturnDouble(F_MAX(scGetDouble("a"), scGetDouble("b")));
	}
}
//Math.range(x,a,b) - Returns value limited between two given values.
static void scMathRange(CTinyJS* tinyJS, CScriptVar* v, void* userdata) {
	if((scIsInt("x"))) {
		scReturnInt(F_RNG(scGetInt("x"), scGetInt("a"), scGetInt("b")));
	} else {
		scReturnDouble(F_RNG(scGetDouble("x"), scGetDouble("a"), scGetDouble("b")));
	}
}
//Math.sign(a) - Returns sign of given value (-1==negative,0=zero,1=positive).
static void scMathSign(CTinyJS* tinyJS, CScriptVar* v, void* userdata) {
	if(scIsInt("a")) {
		scReturnInt(F_SGN(scGetInt("a")));
	} else if(scIsDouble("a")) {
		scReturnDouble(F_SGN(scGetDouble("a")));
	}
}
//Math.PI() - Returns PI value.
static void scMathPI(CTinyJS* tinyJS, CScriptVar* v, void* userdata) {
	scReturnDouble(k_PI);
}
//Math.toDegrees(a) - Returns degree value of a given angle in radians.
static void scMathToDegrees(CTinyJS* tinyJS, CScriptVar* v, void* userdata) {
	scReturnDouble((180.0/k_PI)*(scGetDouble("a")));
}
//Math.toRadians(a) - Returns radians value of a given angle in degrees.
static void scMathToRadians(CTinyJS* tinyJS, CScriptVar* v, void* userdata) {
	scReturnDouble((k_PI/180.0)*(scGetDouble("a")));
}
//Math.sin(a) - Returns trig. sine of given angle in radians.
static void scMathSin(CTinyJS* tinyJS, CScriptVar* v, void* userdata) {
	scReturnDouble(sin(scGetDouble("a")));
}
//Math.asin(a) - Returns trig. arcsine of given angle in radians.
static void scMathASin(CTinyJS* tinyJS, CScriptVar* v, void* userdata) {
	scReturnDouble(asin(scGetDouble("a")));
}
//Math.cos(a) - Returns trig. cosine of given angle in radians.
static void scMathCos(CTinyJS* tinyJS, CScriptVar* v, void* userdata) {
	scReturnDouble(cos(scGetDouble("a")));
}
//Math.acos(a) - Returns trig. arccosine of given angle in radians.
static void scMathACos(CTinyJS* tinyJS, CScriptVar* v, void* userdata) {
	scReturnDouble(acos(scGetDouble("a")));
}
//Math.tan(a) - Returns trig. tangent of given angle in radians.
static void scMathTan(CTinyJS* tinyJS, CScriptVar* v, void* userdata) {
	scReturnDouble(tan(scGetDouble("a")));
}
//Math.atan(a) - Returns trig. arctangent of given angle in radians.
static void scMathATan(CTinyJS* tinyJS, CScriptVar* v, void* userdata) {
	scReturnDouble(atan(scGetDouble("a")));
}
//Math.sinh(a) - Returns trig. hyperbolic sine of given angle in radians.
static void scMathSinh(CTinyJS* tinyJS, CScriptVar* v, void* userdata) {
	scReturnDouble(sinh(scGetDouble("a")));
}
//Math.asinh(a) - Returns trig. hyperbolic arcsine of given angle in radians.
static void scMathASinh(CTinyJS* tinyJS, CScriptVar* v, void* userdata) {
	scReturnDouble(asinh(scGetDouble("a")));
}
//Math.cosh(a) - Returns trig. hyperbolic cosine of given angle in radians.
static void scMathCosh(CTinyJS* tinyJS, CScriptVar* v, void* userdata) {
	scReturnDouble(cosh(scGetDouble("a")));
}
//Math.acosh(a) - Returns trig. hyperbolic arccosine of given angle in radians.
static void scMathACosh(CTinyJS* tinyJS, CScriptVar* v, void* userdata) {
	scReturnDouble(acosh(scGetDouble("a")));
}
//Math.tanh(a) - Returns trig. hyperbolic tangent of given angle in radians.
static void scMathTanh(CTinyJS* tinyJS, CScriptVar* v, void* userdata) {
	scReturnDouble(tanh(scGetDouble("a")));
}
//Math.atan(a) - Returns trig. hyperbolic arctangent of given angle in radians.
static void scMathATanh(CTinyJS* tinyJS, CScriptVar* v, void* userdata) {
	scReturnDouble(atan(scGetDouble("a")));
}
//Math.E() - Returns E Neplero value.
static void scMathE(CTinyJS* tinyJS, CScriptVar* v, void* userdata) {
	scReturnDouble(k_E);
}
//Math.log(a) - Returns natural logaritm (base E) of given value.
static void scMathLog(CTinyJS* tinyJS, CScriptVar* v, void* userdata) {
	scReturnDouble(log(scGetDouble("a")));
}
//Math.log10(a) - Returns logaritm(base 10) of given value.
static void scMathLog10(CTinyJS* tinyJS, CScriptVar* v, void* userdata) {
	scReturnDouble(log10(scGetDouble("a")));
}
//Math.exp(a) - Returns e raised to the power of a given number.
static void scMathExp(CTinyJS* tinyJS, CScriptVar* v, void* userdata) {
	scReturnDouble(exp(scGetDouble("a")));
}
//Math.pow(a,b) - Returns the result of a number raised to a power (a)^(b).
static void scMathPow(CTinyJS* tinyJS, CScriptVar* v, void* userdata) {
	scReturnDouble(pow(scGetDouble("a"), scGetDouble("b")));
}
//Math.sqr(a) - Returns square of given value.
static void scMathSqr(CTinyJS* tinyJS, CScriptVar* v, void* userdata) {
	scReturnDouble((scGetDouble("a") * scGetDouble("a")));
}
//Math.sqrt(a) - Returns square root of given value.
static void scMathSqrt(CTinyJS* tinyJS, CScriptVar* v, void* userdata) {
	scReturnDouble(sqrt(scGetDouble("a")));
}
//-----------------------------------------------------------------------------
//Register Functions.
void registerMathFunctions(CTinyJS* tinyJS) {
	//Math and Trigonometry functions.
	tinyJS->addNative("function Math.abs(a)",       scMathAbs,       NULL);
	tinyJS->addNative("function Math.round(a)",     scMathRound,     NULL);
	tinyJS->addNative("function Math.min(a,b)",     scMathMin,       NULL);
	tinyJS->addNative("function Math.max(a,b)",     scMathMax,       NULL);
	tinyJS->addNative("function Math.range(x,a,b)", scMathRange,     NULL);
	tinyJS->addNative("function Math.sign(a)",      scMathSign,      NULL);
	tinyJS->addNative("function Math.PI()",         scMathPI,        NULL);
	tinyJS->addNative("function Math.toDegrees(a)", scMathToDegrees, NULL);
	tinyJS->addNative("function Math.toRadians(a)", scMathToRadians, NULL);
	tinyJS->addNative("function Math.sin(a)",       scMathSin,       NULL);
	tinyJS->addNative("function Math.asin(a)",      scMathASin,      NULL);
	tinyJS->addNative("function Math.cos(a)",       scMathCos,       NULL);
	tinyJS->addNative("function Math.acos(a)",      scMathACos,      NULL);
	tinyJS->addNative("function Math.tan(a)",       scMathTan,       NULL);
	tinyJS->addNative("function Math.atan(a)",      scMathATan,      NULL);
	tinyJS->addNative("function Math.sinh(a)",      scMathSinh,      NULL);
	tinyJS->addNative("function Math.asinh(a)",     scMathASinh,     NULL);
	tinyJS->addNative("function Math.cosh(a)",      scMathCosh,      NULL);
	tinyJS->addNative("function Math.acosh(a)",     scMathACosh,     NULL);
	tinyJS->addNative("function Math.tanh(a)",      scMathTanh,      NULL);
	tinyJS->addNative("function Math.atanh(a)",     scMathATanh,     NULL);
	tinyJS->addNative("function Math.E()",          scMathE,         NULL);
	tinyJS->addNative("function Math.log(a)",       scMathLog,       NULL);
	tinyJS->addNative("function Math.log10(a)",     scMathLog10,     NULL);
	tinyJS->addNative("function Math.exp(a)",       scMathExp,       NULL);
	tinyJS->addNative("function Math.pow(a,b)",     scMathPow,       NULL);
	tinyJS->addNative("function Math.sqr(a)",       scMathSqr,       NULL);
	tinyJS->addNative("function Math.sqrt(a)",      scMathSqrt,      NULL);
}
