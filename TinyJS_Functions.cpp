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
static void scTrace(ST_TinyJS* tinyJS, ST_TinyJS_Var* v, void* userdata) {
	tinyJS->trace();
}
static void scObjectDump(ST_TinyJS* tinyJS, ST_TinyJS_Var* v, void* userdata) {
	v->getParameter("this")->trace("> ");
}
static void scObjectClone(ST_TinyJS* tinyJS, ST_TinyJS_Var* v, void* userdata) {
	ST_TinyJS_Var* obj = v->getParameter("this");
	v->getReturnVar()->copyValue(obj);
}
static void scMathRand(ST_TinyJS* tinyJS, ST_TinyJS_Var* v, void* userdata) {
	v->getReturnVar()->setNumber((double)rand() / (double)RAND_MAX);
}
static void scMathRandInt(ST_TinyJS* tinyJS, ST_TinyJS_Var* v, void* userdata) {
	int min = v->getParameter("min")->getNumber();
	int max = v->getParameter("max")->getNumber();
	int val = min + (rand() % (1 + max - min));
	v->getReturnVar()->setNumber(val);
}
static void scCharToInt(ST_TinyJS* tinyJS, ST_TinyJS_Var* v, void* userdata) {
	string str = v->getParameter("ch")->getString();;
	int val = 0;
	if(str.length() > 0) {
		val = str[0];
	}
	v->getReturnVar()->setNumber(val);
}
static void scStringIndexOf(ST_TinyJS* tinyJS, ST_TinyJS_Var* v, void* userdata) {
	string str = v->getParameter("this")->getString();
	string search = v->getParameter("search")->getString();
	int val = str.find(search);
	v->getReturnVar()->setNumber(val);
}
static void scStringSubstring(ST_TinyJS* tinyJS, ST_TinyJS_Var* v, void* userdata) {
	string str = v->getParameter("this")->getString();
	int lo = v->getParameter("lo")->getNumber();
	int hi = v->getParameter("hi")->getNumber();
	int l = hi-lo;
	if(l>0 && lo>=0 && lo+l<=str.length()) {
		v->getReturnVar()->setString(str.substr(lo, l).c_str());
	} else {
		v->getReturnVar()->setString("");
	}
}
static void scStringCharAt(ST_TinyJS* tinyJS, ST_TinyJS_Var* v, void* userdata) {
	string str = v->getParameter("this")->getString();
	int p = v->getParameter("pos")->getNumber();
	if(p>=0 && p<str.length()) {
		v->getReturnVar()->setString(str.substr(p, 1).c_str());
	} else {
		v->getReturnVar()->setString("");
	}
}
static void scStringCharCodeAt(ST_TinyJS* tinyJS, ST_TinyJS_Var* v, void* userdata) {
	string str = v->getParameter("this")->getString();
	int p = v->getParameter("pos")->getNumber();
	if((p >= 0) && (p < str.length())) {
		v->getReturnVar()->setNumber(str[p]);
	} else {
		v->getReturnVar()->setNumber(0);
	}
}
static void scStringSplit(ST_TinyJS* tinyJS, ST_TinyJS_Var* v, void* userdata) {
	string str = v->getParameter("this")->getString();
	string sep = v->getParameter("separator")->getString();
	ST_TinyJS_Var* result = v->getReturnVar();
	result->setArray();
	int length = 0;
	int pos = str.find(sep);
	while(pos != -1) {
		result->setArrayIndex(length++, new ST_TinyJS_Var(str.substr(0,pos).c_str()));
		str = str.substr(pos+1);
		pos = str.find(sep);
	}
	if(str.length() > 0) {
		result->setArrayIndex(length++, new ST_TinyJS_Var(str.c_str()));
	}
}
static void scStringFromCharCode(ST_TinyJS* tinyJS, ST_TinyJS_Var* v, void* userdata) {
	char str[2];
	str[0] = v->getParameter("char")->getNumber();
	str[1] = 0;
	v->getReturnVar()->setString(str);
}
static void scIntegerParseInt(ST_TinyJS* tinyJS, ST_TinyJS_Var* v, void* userdata) {
	string str = v->getParameter("str")->getString();
	int val = strtol(str.c_str(), NULL, 0);
	v->getReturnVar()->setNumber(val);
}
static void scIntegerValueOf(ST_TinyJS* tinyJS, ST_TinyJS_Var* v, void* userdata) {
	string str = v->getParameter("str")->getString();
	int val = 0;
	if(str.length()==1) {
		val = str[0];
	}
	v->getReturnVar()->setNumber(val);
}
static void scJSONStringify(ST_TinyJS* tinyJS, ST_TinyJS_Var* v, void* userdata) {
	string result = v->getParameter("obj")->getJSON();
	v->getReturnVar()->setString(result.c_str());
}
static void scExec(ST_TinyJS* tinyJS, ST_TinyJS_Var* v, void* userdata) {
	string str = v->getParameter("jsCode")->getString();
	tinyJS->execute(str.c_str());
}
static void scEval(ST_TinyJS* tinyJS, ST_TinyJS_Var* v, void* userdata) {
	string str = v->getParameter("jsCode")->getString();
	v->setReturnVar(tinyJS->evaluateComplex(str.c_str()).var);
}
static void scArrayContains(ST_TinyJS* tinyJS, ST_TinyJS_Var* v, void* userdata) {
	ST_TinyJS_Var* obj = v->getParameter("obj");
	GSList* list = v->getParameter("this")->firstChild;
	bool contains = false;
	while(list) {
		ST_TinyJS_VarLink* l = (ST_TinyJS_VarLink*)list->data;
		if(l->var->equals(obj)) {
			contains = true;
			break;
		}
		list = list->next;
	}
	v->getReturnVar()->setNumber(contains);
}
static void scArrayRemove(ST_TinyJS* tinyJS, ST_TinyJS_Var* v, void* userdata) {
	ST_TinyJS_Var* obj = v->getParameter("obj");
	ST_TinyJS_Var* arr = v->getParameter("this");
	GSList* list1 = arr->firstChild;
	while(list1) {
		ST_TinyJS_VarLink* l1 = (ST_TinyJS_VarLink*)list1->data;
		list1 = list1->next;	//「arr->removeLink(l1)」によってlist1が開放される可能性が有るので予め次へ進めておく。
		if(l1->var->equals(obj)) {
			int i1 = l1->getIntName();
			arr->removeLink(l1);	//オリジナル版のTinyJSはこの処理を忘れていないか?
			{
				GSList* list2 = arr->firstChild;
				while(list2) {
					ST_TinyJS_VarLink* l2 = (ST_TinyJS_VarLink*)list2->data;
					int i2 = l2->getIntName();
					if(i2 > i1) { l2->setIntName(i2 - 1); }
					list2 = list2->next;	//こちらは開放される可能性は無いので最後に次へ進めて問題無い。
				}
			}
		}
	}
}
static void scArrayJoin(ST_TinyJS* tinyJS, ST_TinyJS_Var* v, void* userdata) {
	string sep = v->getParameter("separator")->getString();
	ST_TinyJS_Var* arr = v->getParameter("this");
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
void registerFunctions(ST_TinyJS* tinyJS) {
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
