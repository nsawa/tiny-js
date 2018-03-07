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
static void scTrace(CScriptVar* var, void* userdata) {
	CTinyJS* tinyJS = (CTinyJS*)userdata;
	tinyJS->root->trace();
}
static void scObjectDump(CScriptVar* var, void* userdata) {
	var->getParameter("this")->trace("> ");
}
static void scObjectClone(CScriptVar* var, void* userdata) {
	CScriptVar* obj = var->getParameter("this");
	var->getReturnVar()->copyValue(obj);
}
static void scMathRand(CScriptVar* var, void* userdata) {
	var->getReturnVar()->setDouble((double)rand()/RAND_MAX);
}
static void scMathRandInt(CScriptVar* var, void* userdata) {
	int min = var->getParameter("min")->getInt();
	int max = var->getParameter("max")->getInt();
	int val = min + (int)(rand()%(1+max-min));
	var->getReturnVar()->setInt(val);
}
static void scCharToInt(CScriptVar* var, void* userdata) {
	string str = var->getParameter("ch")->getString();;
	int val = 0;
	if(str.length()>0) {
		val = (int)str.c_str()[0];
	}
	var->getReturnVar()->setInt(val);
}
static void scStringIndexOf(CScriptVar* var, void* userdata) {
	string str = var->getParameter("this")->getString();
	string search = var->getParameter("search")->getString();
	size_t p = str.find(search);
	int val = (p==string::npos) ? -1 : p;
	var->getReturnVar()->setInt(val);
}
static void scStringSubstring(CScriptVar* var, void* userdata) {
	string str = var->getParameter("this")->getString();
	int lo = var->getParameter("lo")->getInt();
	int hi = var->getParameter("hi")->getInt();
	int l = hi-lo;
	if(l>0 && lo>=0 && lo+l<=(int)str.length()) {
		var->getReturnVar()->setString(str.substr(lo, l));
	} else {
		var->getReturnVar()->setString("");
	}
}
static void scStringCharAt(CScriptVar* var, void* userdata) {
	string str = var->getParameter("this")->getString();
	int p = var->getParameter("pos")->getInt();
	if(p>=0 && p<(int)str.length()) {
		var->getReturnVar()->setString(str.substr(p, 1));
	} else {
		var->getReturnVar()->setString("");
	}
}
static void scStringCharCodeAt(CScriptVar* var, void* userdata) {
	string str = var->getParameter("this")->getString();
	int p = var->getParameter("pos")->getInt();
	if(p>=0 && p<(int)str.length()) {
		var->getReturnVar()->setInt(str.at(p));
	} else {
		var->getReturnVar()->setInt(0);
	}
}
static void scStringSplit(CScriptVar* var, void* userdata) {
	string str = var->getParameter("this")->getString();
	string sep = var->getParameter("separator")->getString();
	CScriptVar* result = var->getReturnVar();
	result->setArray();
	int length = 0;
	size_t pos = str.find(sep);
	while(pos != string::npos) {
		result->setArrayIndex(length++, new CScriptVar(str.substr(0,pos)));
		str = str.substr(pos+1);
		pos = str.find(sep);
	}
	if(str.size()>0) {
		result->setArrayIndex(length++, new CScriptVar(str));
	}
}
static void scStringFromCharCode(CScriptVar* var, void* userdata) {
	char str[2];
	str[0] = var->getParameter("char")->getInt();
	str[1] = 0;
	var->getReturnVar()->setString(str);
}
static void scIntegerParseInt(CScriptVar* var, void* userdata) {
	string str = var->getParameter("str")->getString();
	int val = strtol(str.c_str(),0,0);
	var->getReturnVar()->setInt(val);
}
static void scIntegerValueOf(CScriptVar* var, void* userdata) {
	string str = var->getParameter("str")->getString();
	int val = 0;
	if(str.length()==1) {
		val = str[0];
	}
	var->getReturnVar()->setInt(val);
}
static void scJSONStringify(CScriptVar* var, void* userdata) {
	ostringstream result;
	var->getParameter("obj")->getJSON(result);
	var->getReturnVar()->setString(result.str());
}
static void scExec(CScriptVar* var, void* userdata) {
	CTinyJS* tinyJS = (CTinyJS*)userdata;
	string str = var->getParameter("jsCode")->getString();
	tinyJS->execute(str);
}
static void scEval(CScriptVar* var, void* userdata) {
	CTinyJS* tinyJS = (CTinyJS*)userdata;
	string str = var->getParameter("jsCode")->getString();
	var->setReturnVar(tinyJS->evaluateComplex(str).var);
}
static void scArrayContains(CScriptVar* var, void* userdata) {
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
static void scArrayRemove(CScriptVar* var, void* userdata) {
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
	for(size_t i=0;i<removedIndices.size();i++)
		if(n>=removedIndices[i]) {
			newn--;
		}
		if(newn!=n) {
			v->setIntName(newn);
		}
		v = v->nextSibling;
	}
}
static void scArrayJoin(CScriptVar* var, void* userdata) {
	string sep = var->getParameter("separator")->getString();
	CScriptVar* arr = var->getParameter("this");
	ostringstream sstr;
	int l = arr->getArrayLength();
	for(int i=0;i<l;i++) {
		if(i>0) {
			sstr << sep;
		}
		sstr << arr->getArrayIndex(i)->getString();
	}
	var->getReturnVar()->setString(sstr.str());
}
//-----------------------------------------------------------------------------
//Register Functions
void registerFunctions(CTinyJS* tinyJS) {
	tinyJS->addNative("function exec(jsCode)", scExec, tinyJS);				//Execute the given code.
	tinyJS->addNative("function eval(jsCode)", scEval, tinyJS);				//Execute the given string (an expression) and return the result.
	tinyJS->addNative("function trace()", scTrace, tinyJS);
	tinyJS->addNative("function Object.dump()", scObjectDump, 0);
	tinyJS->addNative("function Object.clone()", scObjectClone, 0);
	tinyJS->addNative("function Math.rand()", scMathRand, 0);
	tinyJS->addNative("function Math.randInt(min, max)", scMathRandInt, 0);
	tinyJS->addNative("function charToInt(ch)", scCharToInt, 0);				//Convert a character to an int - get its value.
	tinyJS->addNative("function String.indexOf(search)", scStringIndexOf, 0);		//Find the position of a string in a string, -1 if not.
	tinyJS->addNative("function String.substring(lo,hi)", scStringSubstring, 0);
	tinyJS->addNative("function String.charAt(pos)", scStringCharAt, 0);
	tinyJS->addNative("function String.charCodeAt(pos)", scStringCharCodeAt, 0);
	tinyJS->addNative("function String.fromCharCode(char)", scStringFromCharCode, 0);
	tinyJS->addNative("function String.split(separator)", scStringSplit, 0);
	tinyJS->addNative("function Integer.parseInt(str)", scIntegerParseInt, 0);		//String to int.
	tinyJS->addNative("function Integer.valueOf(str)", scIntegerValueOf, 0);		//Value of a single character.
	tinyJS->addNative("function JSON.stringify(obj, replacer)", scJSONStringify, 0);	//Convert to JSON. Replacer is ignored at the moment.
	//JSON.parse is left out as you can (unsafely!) use eval instead.
	tinyJS->addNative("function Array.contains(obj)", scArrayContains, 0);
	tinyJS->addNative("function Array.remove(obj)", scArrayRemove, 0);
	tinyJS->addNative("function Array.join(separator)", scArrayJoin, 0);
}
