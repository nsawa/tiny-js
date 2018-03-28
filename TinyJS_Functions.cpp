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
#include "app.h"
//-----------------------------------------------------------------------------
static void scExec(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	const char* str = funcRoot->TinyJS_Var_getParameter("jsCode")->TinyJS_Var_getString();
	tinyJS->TinyJS_execute(str);
}
//-----------------------------------------------------------------------------
static void scEval(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	const char* str = funcRoot->TinyJS_Var_getParameter("jsCode")->TinyJS_Var_getString();
	funcRoot->TinyJS_Var_setReturnVar(tinyJS->TinyJS_evaluate(str));
}
//-----------------------------------------------------------------------------
static void scTrace(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	tinyJS->TinyJS_trace("");
}
//-----------------------------------------------------------------------------
static void scObjectDump(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	funcRoot->TinyJS_Var_getParameter("this")->TinyJS_Var_trace("> ", "");
}
//-----------------------------------------------------------------------------
static void scObjectClone(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	ST_TinyJS_Var* obj = funcRoot->TinyJS_Var_getParameter("this");
	funcRoot->TinyJS_Var_setReturnVar(obj->TinyJS_Var_deepCopy());
}
//-----------------------------------------------------------------------------
static void scMathRand(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	funcRoot->TinyJS_Var_getReturnVar()->TinyJS_Var_setNumber((double)rand() / (double)RAND_MAX);
}
//-----------------------------------------------------------------------------
static void scMathRandInt(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	int min = funcRoot->TinyJS_Var_getParameter("min")->TinyJS_Var_getNumber();
	int max = funcRoot->TinyJS_Var_getParameter("max")->TinyJS_Var_getNumber();
	int val = min + (rand() % (1 + max - min));
	funcRoot->TinyJS_Var_getReturnVar()->TinyJS_Var_setNumber(val);
}
//-----------------------------------------------------------------------------
static void scCharToInt(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	const char* str = funcRoot->TinyJS_Var_getParameter("ch")->TinyJS_Var_getString();
	int val = str[0];
	funcRoot->TinyJS_Var_getReturnVar()->TinyJS_Var_setNumber(val);
}
//-----------------------------------------------------------------------------
static void scStringIndexOf(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	const char* str = funcRoot->TinyJS_Var_getParameter("this")->TinyJS_Var_getString();
	const char* search = funcRoot->TinyJS_Var_getParameter("search")->TinyJS_Var_getString();
	const char* p = strstr(str, search);
	int val = p ? (p - str) : -1;
	funcRoot->TinyJS_Var_getReturnVar()->TinyJS_Var_setNumber(val);
}
//-----------------------------------------------------------------------------
static void scStringSubstring(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	const char* str = funcRoot->TinyJS_Var_getParameter("this")->TinyJS_Var_getString();
	int lo = funcRoot->TinyJS_Var_getParameter("lo")->TinyJS_Var_getNumber();
	int hi = funcRoot->TinyJS_Var_getParameter("hi")->TinyJS_Var_getNumber();
	int l = hi - lo;
	if((l > 0) && (lo >= 0) && ((lo + l) <= strlen(str))) {
		funcRoot->TinyJS_Var_getReturnVar()->TinyJS_Var_setString(strndup(str + lo, l));
	} else {
		funcRoot->TinyJS_Var_getReturnVar()->TinyJS_Var_setString("");
	}
}
//-----------------------------------------------------------------------------
static void scStringCharAt(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	const char* str = funcRoot->TinyJS_Var_getParameter("this")->TinyJS_Var_getString();
	int p = funcRoot->TinyJS_Var_getParameter("pos")->TinyJS_Var_getNumber();
	if(p >= 0 && p < strlen(str)) {
		funcRoot->TinyJS_Var_getReturnVar()->TinyJS_Var_setString(strndup(str + p, 1));
	} else {
		funcRoot->TinyJS_Var_getReturnVar()->TinyJS_Var_setString("");
	}
}
//-----------------------------------------------------------------------------
static void scStringCharCodeAt(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	const char* str = funcRoot->TinyJS_Var_getParameter("this")->TinyJS_Var_getString();
	int p = funcRoot->TinyJS_Var_getParameter("pos")->TinyJS_Var_getNumber();
	if((unsigned)p < strlen(str)) {
		funcRoot->TinyJS_Var_getReturnVar()->TinyJS_Var_setNumber(str[p]);
	} else {
		funcRoot->TinyJS_Var_getReturnVar()->TinyJS_Var_setNumber(0);
	}
}
//-----------------------------------------------------------------------------
static void scStringFromCharCode(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	const char* str = strdup_printf("%c", funcRoot->TinyJS_Var_getParameter("char")->TinyJS_Var_getNumber());
	funcRoot->TinyJS_Var_getReturnVar()->TinyJS_Var_setString(str);
}
//-----------------------------------------------------------------------------
static void scStringSplit(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	const char* str = funcRoot->TinyJS_Var_getParameter("this")->TinyJS_Var_getString();
	const char* sep = funcRoot->TinyJS_Var_getParameter("separator")->TinyJS_Var_getString();
	ST_TinyJS_Var* result = funcRoot->TinyJS_Var_getReturnVar();
	result->TinyJS_Var_setArray();
	int length = 0;
	const char* p = strstr(str, sep);
	int pos = p ? (p - str) : -1;
	while(pos != -1) {
		result->TinyJS_Var_setArrayIndex(length++, ST_TinyJS_Var::TinyJS_Var_newString(strndup(str, pos)));
		str = str + (pos + 1);
		p = strstr(str, sep);
		pos = p ? (p - str) : -1;
	}
	if(strlen(str)) {
		result->TinyJS_Var_setArrayIndex(length++, ST_TinyJS_Var::TinyJS_Var_newString(str));
	}
}
//-----------------------------------------------------------------------------
static void scIntegerParseInt(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	const char* str = funcRoot->TinyJS_Var_getParameter("str")->TinyJS_Var_getString();
	int val = strtol(str, NULL, 0);
	funcRoot->TinyJS_Var_getReturnVar()->TinyJS_Var_setNumber(val);
}
//-----------------------------------------------------------------------------
static void scIntegerValueOf(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	const char* str = funcRoot->TinyJS_Var_getParameter("str")->TinyJS_Var_getString();
	int val = 0;
	if(strlen(str) == 1) { val = str[0]; }
	funcRoot->TinyJS_Var_getReturnVar()->TinyJS_Var_setNumber(val);
}
//-----------------------------------------------------------------------------
static void scJSONStringify(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	const char* str = funcRoot->TinyJS_Var_getParameter("obj")->TinyJS_Var_getJSON("");
	funcRoot->TinyJS_Var_getReturnVar()->TinyJS_Var_setString(str);
}
//-----------------------------------------------------------------------------
static void scArrayContains(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	ST_TinyJS_Var* obj = funcRoot->TinyJS_Var_getParameter("obj");
	GSList/*<ST_TinyJS_VarLink*>*/* list = funcRoot->TinyJS_Var_getParameter("this")->firstChild;
	int contains = 0;
	while(list) {
		ST_TinyJS_VarLink* l = (ST_TinyJS_VarLink*)list->data;
		if(l->var->TinyJS_Var_equals(obj)) {
			contains = 1;
			break;
		}
		list = list->next;
	}
	funcRoot->TinyJS_Var_getReturnVar()->TinyJS_Var_setNumber(contains);
}
//-----------------------------------------------------------------------------
static void scArrayRemove(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	ST_TinyJS_Var* obj = funcRoot->TinyJS_Var_getParameter("obj");
	ST_TinyJS_Var* arr = funcRoot->TinyJS_Var_getParameter("this");
	GSList/*<ST_TinyJS_VarLink*>*/* list1 = arr->firstChild;
	while(list1) {
		ST_TinyJS_VarLink* l1 = (ST_TinyJS_VarLink*)list1->data;
		list1 = list1->next;	//「arr->TinyJS_Var_removeLink(l1)」によってlist1が開放される可能性が有るので予め次へ進めておく。
		if(l1->var->TinyJS_Var_equals(obj)) {
			int i1 = l1->TinyJS_VarLink_getIntName();
			arr->TinyJS_Var_removeLink(l1);	//オリジナル版のTinyJSにはこの処理が無かった。オリジナル版のTinyJSはこの処理を忘れていないか?
			{
				GSList/*<ST_TinyJS_VarLink*>*/* list2 = arr->firstChild;
				while(list2) {
					ST_TinyJS_VarLink* l2 = (ST_TinyJS_VarLink*)list2->data;
					int i2 = l2->TinyJS_VarLink_getIntName();
					if(i2 > i1) { l2->TinyJS_VarLink_setIntName(i2 - 1); }
					list2 = list2->next;	//こちらは開放される可能性は無いので最後に次へ進めて問題無い。
				}
			}
		}
	}
}
//-----------------------------------------------------------------------------
static void scArrayJoin(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	const char* sep = funcRoot->TinyJS_Var_getParameter("separator")->TinyJS_Var_getString();
	ST_TinyJS_Var* arr = funcRoot->TinyJS_Var_getParameter("this");
	GString* sstr = g_string_new(NULL);
	int l = arr->TinyJS_Var_getArrayLength();
	for(int i = 0; i < l; i++) {
		if(i) { g_string_append(sstr, sep); }
		g_string_append(sstr, arr->TinyJS_Var_getArrayIndex(i)->TinyJS_Var_getString());
	}
	funcRoot->TinyJS_Var_getReturnVar()->TinyJS_Var_setString(sstr->str);
}
//-----------------------------------------------------------------------------
//Register Functions
void TinyJS_registerFunctions(ST_TinyJS* tinyJS) {
	tinyJS->TinyJS_addNative("function exec(jsCode)",                 scExec,               NULL);	//Execute the given code.
	tinyJS->TinyJS_addNative("function eval(jsCode)",                 scEval,               NULL);	//Execute the given string (an expression) and return the result.
	tinyJS->TinyJS_addNative("function trace()",                      scTrace,              NULL);
	tinyJS->TinyJS_addNative("function Object.dump()",                scObjectDump,         NULL);
	tinyJS->TinyJS_addNative("function Object.clone()",               scObjectClone,        NULL);
	tinyJS->TinyJS_addNative("function Math.rand()",                  scMathRand,           NULL);
	tinyJS->TinyJS_addNative("function Math.randInt(min,max)",        scMathRandInt,        NULL);
	tinyJS->TinyJS_addNative("function charToInt(ch)",                scCharToInt,          NULL);	//Convert a character to an int - get its value.
	tinyJS->TinyJS_addNative("function String.indexOf(search)",       scStringIndexOf,      NULL);	//Find the position of a string in a string, -1 if not.
	tinyJS->TinyJS_addNative("function String.substring(lo,hi)",      scStringSubstring,    NULL);
	tinyJS->TinyJS_addNative("function String.charAt(pos)",           scStringCharAt,       NULL);
	tinyJS->TinyJS_addNative("function String.charCodeAt(pos)",       scStringCharCodeAt,   NULL);
	tinyJS->TinyJS_addNative("function String.fromCharCode(char)",    scStringFromCharCode, NULL);
	tinyJS->TinyJS_addNative("function String.split(separator)",      scStringSplit,        NULL);
	tinyJS->TinyJS_addNative("function Integer.parseInt(str)",        scIntegerParseInt,    NULL);	//String to int.
	tinyJS->TinyJS_addNative("function Integer.valueOf(str)",         scIntegerValueOf,     NULL);	//Value of a single character.
	tinyJS->TinyJS_addNative("function JSON.stringify(obj,replacer)", scJSONStringify,      NULL);	//Convert to JSON. Replacer is ignored at the moment.
	//JSON.parse is left out as you can (unsafely!) use eval instead.
	tinyJS->TinyJS_addNative("function Array.contains(obj)",          scArrayContains,      NULL);
	tinyJS->TinyJS_addNative("function Array.remove(obj)",            scArrayRemove,        NULL);
	tinyJS->TinyJS_addNative("function Array.join(separator)",        scArrayJoin,          NULL);
}
