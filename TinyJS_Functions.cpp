//
//	TinyJS - A single-file Javascript-alike engine
//
//	Authored By Gordon Williams <gw@pur3.co.uk>
//	Copyright (C) 2009 Pur3 Ltd
//
//	Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//	The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
//
//	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
//	IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
#include "TinyJS.h"
#include "TinyJS_Functions.h"
//-----------------------------------------------------------------------------
//Actual Functions
static void scTrace(CTinyJS* tinyJS, CScriptVar* var, void* userData) {
	tinyJS->trace();
}
static void scObjectDump(CTinyJS* tinyJS, CScriptVar* var, void* userData) {
	var->getParameter("this")->trace("> ");
}
static void scObjectClone(CTinyJS* tinyJS, CScriptVar* var, void* userData) {
	CScriptVar* obj = var->getParameter("this");
	var->getReturnVar()->copyValue(obj);
}
static void scMathRand(CTinyJS* tinyJS, CScriptVar* var, void* userData) {
	var->getReturnVar()->setDouble((double)rand()/RAND_MAX);
}
static void scMathRandInt(CTinyJS* tinyJS, CScriptVar* var, void* userData) {
	int min = var->getParameter("min")->getInt();
	int max = var->getParameter("max")->getInt();
	int val = min + (int)(rand()%(1+max-min));
	var->getReturnVar()->setInt(val);
}
static void scCharToInt(CTinyJS* tinyJS, CScriptVar* var, void* userData) {
	string str = var->getParameter("ch")->getString();;
	int val = 0;
	if(str.length() > 0) {
		val = str[0];
	}
	var->getReturnVar()->setInt(val);
}
static void scStringIndexOf(CTinyJS* tinyJS, CScriptVar* var, void* userData) {
	string str = var->getParameter("this")->getString();
	string search = var->getParameter("search")->getString();
	int p = str.find(search);
	int val = (p == -1) ? -1 : p;	//¦–³‘Êˆ—!!!!!!!!!!!!
	var->getReturnVar()->setInt(val);
}
static void scStringSubstring(CTinyJS* tinyJS, CScriptVar* var, void* userData) {
	string str = var->getParameter("this")->getString();
	int lo = var->getParameter("lo")->getInt();
	int hi = var->getParameter("hi")->getInt();
	int l = hi-lo;
	if(l>0 && lo>=0 && lo+l<=str.length()) {
		var->getReturnVar()->setString(str.substr(lo, l).c_str());
	} else {
		var->getReturnVar()->setString("");
	}
}
static void scStringCharAt(CTinyJS* tinyJS, CScriptVar* var, void* userData) {
	string str = var->getParameter("this")->getString();
	int p = var->getParameter("pos")->getInt();
	if(p>=0 && p<str.length()) {
		var->getReturnVar()->setString(str.substr(p, 1).c_str());
	} else {
		var->getReturnVar()->setString("");
	}
}
static void scStringCharCodeAt(CTinyJS* tinyJS, CScriptVar* var, void* userData) {
	string str = var->getParameter("this")->getString();
	int p = var->getParameter("pos")->getInt();
	if((p >= 0) && (p < str.length())) {
		var->getReturnVar()->setInt(str[p]);
	} else {
		var->getReturnVar()->setInt(0);
	}
}
static void scStringSplit(CTinyJS* tinyJS, CScriptVar* var, void* userData) {
	string str = var->getParameter("this")->getString();
	string sep = var->getParameter("separator")->getString();
	CScriptVar* result = var->getReturnVar();
	result->setArray();
	int length = 0;
	int pos = str.find(sep);
	while(pos != -1) {
		result->setArrayIndex(length++, new CScriptVar(str.substr(0,pos).c_str()));
		str = str.substr(pos+1);
		pos = str.find(sep);
	}
	if(str.length() > 0) {
		result->setArrayIndex(length++, new CScriptVar(str.c_str()));
	}
}
static void scStringFromCharCode(CTinyJS* tinyJS, CScriptVar* var, void* userData) {
	char str[2];
	str[0] = var->getParameter("char")->getInt();
	str[1] = 0;
	var->getReturnVar()->setString(str);
}
static void scIntegerParseInt(CTinyJS* tinyJS, CScriptVar* var, void* userData) {
	string str = var->getParameter("str")->getString();
	int val = strtol(str.c_str(), 0, 0);
	var->getReturnVar()->setInt(val);
}
static void scIntegerValueOf(CTinyJS* tinyJS, CScriptVar* var, void* userData) {
	string str = var->getParameter("str")->getString();
	int val = 0;
	if(str.length()==1) {
		val = str[0];
	}
	var->getReturnVar()->setInt(val);
}
static void scJSONStringify(CTinyJS* tinyJS, CScriptVar* var, void* userData) {
	string result = var->getParameter("obj")->getJSON();
	var->getReturnVar()->setString(result.c_str());
}
static void scExec(CTinyJS* tinyJS, CScriptVar* var, void* userData) {
	string str = var->getParameter("jsCode")->getString();
	tinyJS->execute(str.c_str());
}
static void scEval(CTinyJS* tinyJS, CScriptVar* var, void* userData) {
	string str = var->getParameter("jsCode")->getString();
	var->setReturnVar(tinyJS->evaluateComplex(str.c_str()).var);
}
static void scArrayContains(CTinyJS* tinyJS, CScriptVar* var, void* userData) {
	CScriptVar* obj = var->getParameter("obj");
	CScriptVarLink* v = var->getParameter("this")->firstChild;
	bool contains = false;
	while(v) {
		if(v->var->equals(obj)) {
			contains = true;
			break;
		}
		v = v->nextSibling;
	}
	var->getReturnVar()->setInt(contains);
}
static void scArrayRemove(CTinyJS* tinyJS, CScriptVar* var, void* userData) {
	CScriptVar* obj = var->getParameter("obj");
	vector<int> removedIndices;
	CScriptVarLink* v;
	//remove
	v = var->getParameter("this")->firstChild;
	while(v) {
		if(v->var->equals(obj)) {
			removedIndices.push_back(v->getIntName());
		}
		v = v->nextSibling;
	}
	//renumber
	v = var->getParameter("this")->firstChild;
	while(v) {
		int n = v->getIntName();
		int newn = n;
		for(int i = 0; i < removedIndices.size(); i++) {
			if(n >= removedIndices[i]) {
				newn--;
			}
		}
		if(newn != n) {
			v->setIntName(newn);
		}
		v = v->nextSibling;
	}
}
static void scArrayJoin(CTinyJS* tinyJS, CScriptVar* var, void* userData) {
	string sep = var->getParameter("separator")->getString();
	CScriptVar* arr = var->getParameter("this");
	string sstr;
	int l = arr->getArrayLength();
	for(int i=0;i<l;i++) {
		if(i>0) {
			sstr += sep;
		}
		sstr += arr->getArrayIndex(i)->getString();
	}
	var->getReturnVar()->setString(sstr.c_str());
}
//-----------------------------------------------------------------------------
//Register Functions
void registerFunctions(CTinyJS* tinyJS) {
	tinyJS->addNative("function exec(jsCode)",                 scExec,               NULL);	//Execute the given code.
	tinyJS->addNative("function eval(jsCode)",                 scEval,               NULL);	//Execute the given string (an expression) and return the result.
	tinyJS->addNative("function trace()",                      scTrace,              NULL);
	tinyJS->addNative("function Object.dump()",                scObjectDump,         NULL);
	tinyJS->addNative("function Object.clone()",               scObjectClone,        NULL);
	tinyJS->addNative("function Math.rand()",                  scMathRand,           NULL);
	tinyJS->addNative("function Math.randInt(min,max)",        scMathRandInt,        NULL);
	tinyJS->addNative("function charToInt(ch)",                scCharToInt,          NULL);	//Convert a character to an int - get its value.
	tinyJS->addNative("function String.indexOf(search)",       scStringIndexOf,      NULL);	//Find the position of a string in a string, -1 if not.
	tinyJS->addNative("function String.substring(lo,hi)",      scStringSubstring,    NULL);
	tinyJS->addNative("function String.charAt(pos)",           scStringCharAt,       NULL);
	tinyJS->addNative("function String.charCodeAt(pos)",       scStringCharCodeAt,   NULL);
	tinyJS->addNative("function String.fromCharCode(char)",    scStringFromCharCode, NULL);
	tinyJS->addNative("function String.split(separator)",      scStringSplit,        NULL);
	tinyJS->addNative("function Integer.parseInt(str)",        scIntegerParseInt,    NULL);	//String to int.
	tinyJS->addNative("function Integer.valueOf(str)",         scIntegerValueOf,     NULL);	//Value of a single character.
	tinyJS->addNative("function JSON.stringify(obj,replacer)", scJSONStringify,      NULL);	//Convert to JSON. Replacer is ignored at the moment.
	//JSON.parse is left out as you can (unsafely!) use eval instead.
	tinyJS->addNative("function Array.contains(obj)",          scArrayContains,      NULL);
	tinyJS->addNative("function Array.remove(obj)",            scArrayRemove,        NULL);
	tinyJS->addNative("function Array.join(separator)",        scArrayJoin,          NULL);
}
