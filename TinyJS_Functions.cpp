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
static void scTrace(CTinyJS* tinyJS, CScriptVar* v, void* userdata) {
	tinyJS->trace();
}
static void scObjectDump(CTinyJS* tinyJS, CScriptVar* v, void* userdata) {
	v->getParameter("this")->trace("> ");
}
static void scObjectClone(CTinyJS* tinyJS, CScriptVar* v, void* userdata) {
	CScriptVar* obj = v->getParameter("this");
	v->getReturnVar()->copyValue(obj);
}
static void scMathRand(CTinyJS* tinyJS, CScriptVar* v, void* userdata) {
	v->getReturnVar()->setDouble((double)rand()/RAND_MAX);
}
static void scMathRandInt(CTinyJS* tinyJS, CScriptVar* v, void* userdata) {
	int min = v->getParameter("min")->getInt();
	int max = v->getParameter("max")->getInt();
	int val = min + (int)(rand()%(1+max-min));
	v->getReturnVar()->setInt(val);
}
static void scCharToInt(CTinyJS* tinyJS, CScriptVar* v, void* userdata) {
	string str = v->getParameter("ch")->getString();;
	int val = 0;
	if(str.length() > 0) {
		val = str[0];
	}
	v->getReturnVar()->setInt(val);
}
static void scStringIndexOf(CTinyJS* tinyJS, CScriptVar* v, void* userdata) {
	string str = v->getParameter("this")->getString();
	string search = v->getParameter("search")->getString();
	int p = str.find(search);
	int val = (p == -1) ? -1 : p;	//¦–³‘Êˆ—!!!!!!!!!!!!
	v->getReturnVar()->setInt(val);
}
static void scStringSubstring(CTinyJS* tinyJS, CScriptVar* v, void* userdata) {
	string str = v->getParameter("this")->getString();
	int lo = v->getParameter("lo")->getInt();
	int hi = v->getParameter("hi")->getInt();
	int l = hi-lo;
	if(l>0 && lo>=0 && lo+l<=str.length()) {
		v->getReturnVar()->setString(str.substr(lo, l).c_str());
	} else {
		v->getReturnVar()->setString("");
	}
}
static void scStringCharAt(CTinyJS* tinyJS, CScriptVar* v, void* userdata) {
	string str = v->getParameter("this")->getString();
	int p = v->getParameter("pos")->getInt();
	if(p>=0 && p<str.length()) {
		v->getReturnVar()->setString(str.substr(p, 1).c_str());
	} else {
		v->getReturnVar()->setString("");
	}
}
static void scStringCharCodeAt(CTinyJS* tinyJS, CScriptVar* v, void* userdata) {
	string str = v->getParameter("this")->getString();
	int p = v->getParameter("pos")->getInt();
	if((p >= 0) && (p < str.length())) {
		v->getReturnVar()->setInt(str[p]);
	} else {
		v->getReturnVar()->setInt(0);
	}
}
static void scStringSplit(CTinyJS* tinyJS, CScriptVar* v, void* userdata) {
	string str = v->getParameter("this")->getString();
	string sep = v->getParameter("separator")->getString();
	CScriptVar* result = v->getReturnVar();
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
static void scStringFromCharCode(CTinyJS* tinyJS, CScriptVar* v, void* userdata) {
	char str[2];
	str[0] = v->getParameter("char")->getInt();
	str[1] = 0;
	v->getReturnVar()->setString(str);
}
static void scIntegerParseInt(CTinyJS* tinyJS, CScriptVar* v, void* userdata) {
	string str = v->getParameter("str")->getString();
	int val = strtol(str.c_str(), NULL, 0);
	v->getReturnVar()->setInt(val);
}
static void scIntegerValueOf(CTinyJS* tinyJS, CScriptVar* v, void* userdata) {
	string str = v->getParameter("str")->getString();
	int val = 0;
	if(str.length()==1) {
		val = str[0];
	}
	v->getReturnVar()->setInt(val);
}
static void scJSONStringify(CTinyJS* tinyJS, CScriptVar* v, void* userdata) {
	string result = v->getParameter("obj")->getJSON();
	v->getReturnVar()->setString(result.c_str());
}
static void scExec(CTinyJS* tinyJS, CScriptVar* v, void* userdata) {
	string str = v->getParameter("jsCode")->getString();
	tinyJS->execute(str.c_str());
}
static void scEval(CTinyJS* tinyJS, CScriptVar* v, void* userdata) {
	string str = v->getParameter("jsCode")->getString();
	v->setReturnVar(tinyJS->evaluateComplex(str.c_str()).var);
}
static void scArrayContains(CTinyJS* tinyJS, CScriptVar* v, void* userdata) {
	CScriptVar* obj = v->getParameter("obj");
	GSList* list = v->getParameter("this")->firstChild;
	bool contains = false;
	while(list) {
		CScriptVarLink* l = (CScriptVarLink*)list->data;
		if(l->var->equals(obj)) {
			contains = true;
			break;
		}
		list = list->next;
	}
	v->getReturnVar()->setInt(contains);
}
static void scArrayRemove(CTinyJS* tinyJS, CScriptVar* v, void* userdata) {
	CScriptVar* obj = v->getParameter("obj");
	vector<int> removedIndices;
	GSList* list;
	//remove
	list = v->getParameter("this")->firstChild;
	while(list) {
		CScriptVarLink* l = (CScriptVarLink*)list->data;
		if(l->var->equals(obj)) {
			removedIndices.push_back(l->getIntName());
		}
		list = list->next;
	}
	//renumber
	list = v->getParameter("this")->firstChild;
	while(list) {
		CScriptVarLink* l = (CScriptVarLink*)list->data;
		int n = l->getIntName();
		int newn = n;
		for(int i = 0; i < removedIndices.size(); i++) {
			if(n >= removedIndices[i]) {
				newn--;
			}
		}
		if(newn != n) {
			l->setIntName(newn);
		}
		list = list->next;
	}
}
static void scArrayJoin(CTinyJS* tinyJS, CScriptVar* v, void* userdata) {
	string sep = v->getParameter("separator")->getString();
	CScriptVar* arr = v->getParameter("this");
	string sstr;
	int l = arr->getArrayLength();
	for(int i=0;i<l;i++) {
		if(i>0) {
			sstr += sep;
		}
		sstr += arr->getArrayIndex(i)->getString();
	}
	v->getReturnVar()->setString(sstr.c_str());
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
