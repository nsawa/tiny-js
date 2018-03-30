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
//-----------------------------------------------------------------------------
//
//	TinyJS(移植版)
//
//	Ported By Naoyuki Sawa <nsawa@piece-me.org>
//	Copyright (C) 2018 Piece Lab.
//
//	* Fri Mar 30 23:59:59 JST 2018 Naoyuki Sawa
//	- 1st リリース。
//	- 「TinyJS」(https://github.com/gfwilliams/tiny-js)を、P/ECEで動作するように移植したものです。
//	  オリジナル版のTinyJSはC++で書かれていますが、P/ECE開発環境にはGCC 2.7.2相当のCコンパイラしか無いので、C++からC言語に移植しました。
//	  C++とC言語はだいぶん違うため、コードはほぼ全て書き換えてしまいましたが、アルゴリズムはオリジナル版のTinyJSとだいたい同じままです。
//	  尚、メインターゲットはP/ECEですが、(少なくとも現時点では)Visual Studio 2017と、Visual C++ 6.0でもビルド出来ています。
//
#include "clip.h"
//-----------------------------------------------------------------------------
static void scExec(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	const char* str = TinyJS_Var_getString(TinyJS_Var_getParameter(funcRoot, "code"));
	TinyJS_execute(tinyJS, str);
}
//-----------------------------------------------------------------------------
static void scEval(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	const char* str = TinyJS_Var_getString(TinyJS_Var_getParameter(funcRoot, "code"));
	TinyJS_Var_setReturnVar(funcRoot, TinyJS_evaluate(tinyJS, str));
}
//-----------------------------------------------------------------------------
static void scTrace(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	TinyJS_trace(tinyJS, "");
}
//-----------------------------------------------------------------------------
static void scObjectDump(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	TinyJS_Var_trace(TinyJS_Var_getParameter(funcRoot, "this"), "> ", "");
}
//-----------------------------------------------------------------------------
static void scObjectClone(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	ST_TinyJS_Var* obj = TinyJS_Var_getParameter(funcRoot, "this");
	TinyJS_Var_setReturnVar(funcRoot, TinyJS_Var_deepCopy(obj));
}
//-----------------------------------------------------------------------------
static void scMathRand(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	TinyJS_Var_setNumber(TinyJS_Var_getReturnVar(funcRoot), (double)rand() / (double)RAND_MAX);
}
//-----------------------------------------------------------------------------
static void scMathRandInt(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	int min = TinyJS_Var_getNumber(TinyJS_Var_getParameter(funcRoot, "min"));
	int max = TinyJS_Var_getNumber(TinyJS_Var_getParameter(funcRoot, "max"));
	int val = min + (rand() % (1 + max - min));
	TinyJS_Var_setNumber(TinyJS_Var_getReturnVar(funcRoot), val);
}
//-----------------------------------------------------------------------------
static void scCharToInt(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	const char* str = TinyJS_Var_getString(TinyJS_Var_getParameter(funcRoot, "ch"));
	int val = str[0];
	TinyJS_Var_setNumber(TinyJS_Var_getReturnVar(funcRoot), val);
}
//-----------------------------------------------------------------------------
static void scStringIndexOf(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	const char* str = TinyJS_Var_getString(TinyJS_Var_getParameter(funcRoot, "this"));
	const char* search = TinyJS_Var_getString(TinyJS_Var_getParameter(funcRoot, "search"));
	const char* p = strstr(str, search);
	int val = p ? (p - str) : -1;
	TinyJS_Var_setNumber(TinyJS_Var_getReturnVar(funcRoot), val);
}
//-----------------------------------------------------------------------------
static void scStringSubstring(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	const char* str = TinyJS_Var_getString(TinyJS_Var_getParameter(funcRoot, "this"));
	int lo = TinyJS_Var_getNumber(TinyJS_Var_getParameter(funcRoot, "lo"));
	int hi = TinyJS_Var_getNumber(TinyJS_Var_getParameter(funcRoot, "hi"));
	int l = hi - lo;
	if((l > 0) && (lo >= 0) && ((int)(lo + l) <= (int)strlen(str))) {
		TinyJS_Var_setString(TinyJS_Var_getReturnVar(funcRoot), strndup(str + lo, l));
	} else {
		TinyJS_Var_setString(TinyJS_Var_getReturnVar(funcRoot), "");
	}
}
//-----------------------------------------------------------------------------
static void scStringCharAt(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	const char* str = TinyJS_Var_getString(TinyJS_Var_getParameter(funcRoot, "this"));
	int p = TinyJS_Var_getNumber(TinyJS_Var_getParameter(funcRoot, "pos"));
	if((p >= 0) && ((int)p < (int)strlen(str))) {
		TinyJS_Var_setString(TinyJS_Var_getReturnVar(funcRoot), strndup(str + p, 1));
	} else {
		TinyJS_Var_setString(TinyJS_Var_getReturnVar(funcRoot), "");
	}
}
//-----------------------------------------------------------------------------
static void scStringCharCodeAt(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	const char* str = TinyJS_Var_getString(TinyJS_Var_getParameter(funcRoot, "this"));
	int p = TinyJS_Var_getNumber(TinyJS_Var_getParameter(funcRoot, "pos"));
	if((unsigned)p < strlen(str)) {
		TinyJS_Var_setNumber(TinyJS_Var_getReturnVar(funcRoot), str[p]);
	} else {
		TinyJS_Var_setNumber(TinyJS_Var_getReturnVar(funcRoot), 0);
	}
}
//-----------------------------------------------------------------------------
static void scStringFromCharCode(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	const char* str = strdup_printf("%c", TinyJS_Var_getNumber(TinyJS_Var_getParameter(funcRoot, "ch")));
	TinyJS_Var_setString(TinyJS_Var_getReturnVar(funcRoot), str);
}
//-----------------------------------------------------------------------------
static void scStringSplit(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	const char* str = TinyJS_Var_getString(TinyJS_Var_getParameter(funcRoot, "this"));
	const char* sep = TinyJS_Var_getString(TinyJS_Var_getParameter(funcRoot, "separator"));
	ST_TinyJS_Var* result = TinyJS_Var_getReturnVar(funcRoot);
	TinyJS_Var_setArray(result);
	{
		int length = 0;
		const char* p = strstr(str, sep);
		int pos = p ? (p - str) : -1;
		while(pos != -1) {
			TinyJS_Var_setArrayIndex(result, length++, TinyJS_Var_newString(strndup(str, pos)));
			str = str + (pos + 1);
			p = strstr(str, sep);
			pos = p ? (p - str) : -1;
		}
		if(strlen(str)) {
			TinyJS_Var_setArrayIndex(result, length++, TinyJS_Var_newString(str));
		}
	}
}
//-----------------------------------------------------------------------------
static void scIntegerParseInt(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	const char* str = TinyJS_Var_getString(TinyJS_Var_getParameter(funcRoot, "str"));
	int val = strtol(str, NULL, 0);
	TinyJS_Var_setNumber(TinyJS_Var_getReturnVar(funcRoot), val);
}
//-----------------------------------------------------------------------------
static void scIntegerValueOf(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	const char* str = TinyJS_Var_getString(TinyJS_Var_getParameter(funcRoot, "str"));
	int val = 0;
	if(strlen(str) == 1) { val = str[0]; }
	TinyJS_Var_setNumber(TinyJS_Var_getReturnVar(funcRoot), val);
}
//-----------------------------------------------------------------------------
static void scJSONStringify(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	const char* str = TinyJS_Var_getJSON(TinyJS_Var_getParameter(funcRoot, "obj"), "");
	TinyJS_Var_setString(TinyJS_Var_getReturnVar(funcRoot), str);
}
//-----------------------------------------------------------------------------
static void scArrayContains(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	ST_TinyJS_Var* obj = TinyJS_Var_getParameter(funcRoot, "obj");
	GSList/*<ST_TinyJS_VarLink*>*/* list = TinyJS_Var_getParameter(funcRoot, "this")->firstChild;
	int contains = 0;
	while(list) {
		ST_TinyJS_VarLink* l = (ST_TinyJS_VarLink*)list->data;
		if(TinyJS_Var_equals(l->var, obj)) {
			contains = 1;
			break;
		}
		list = list->next;
	}
	TinyJS_Var_setNumber(TinyJS_Var_getReturnVar(funcRoot), contains);
}
//-----------------------------------------------------------------------------
static void scArrayRemove(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	ST_TinyJS_Var* obj = TinyJS_Var_getParameter(funcRoot, "obj");
	ST_TinyJS_Var* arr = TinyJS_Var_getParameter(funcRoot, "this");
	GSList/*<ST_TinyJS_VarLink*>*/* list1 = arr->firstChild;
	while(list1) {
		ST_TinyJS_VarLink* l1 = (ST_TinyJS_VarLink*)list1->data;
		list1 = list1->next;	//「arr->TinyJS_Var_removeLink(l1)」によってlist1が開放される可能性が有るので予め次へ進めておく。
		if(TinyJS_Var_equals(l1->var, obj)) {
			int i1 = TinyJS_VarLink_getIntName(l1);
			TinyJS_Var_removeLink(arr, l1);	//オリジナル版のTinyJSにはこの処理が無かった。オリジナル版のTinyJSはこの処理を忘れていないか?
			{
				GSList/*<ST_TinyJS_VarLink*>*/* list2 = arr->firstChild;
				while(list2) {
					ST_TinyJS_VarLink* l2 = (ST_TinyJS_VarLink*)list2->data;
					int i2 = TinyJS_VarLink_getIntName(l2);
					if(i2 > i1) { TinyJS_VarLink_setIntName(l2, i2 - 1); }
					list2 = list2->next;	//こちらは開放される可能性は無いので最後に次へ進めて問題無い。
				}
			}
		}
	}
}
//-----------------------------------------------------------------------------
static void scArrayJoin(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	const char* sep = TinyJS_Var_getString(TinyJS_Var_getParameter(funcRoot, "separator"));
	ST_TinyJS_Var* arr = TinyJS_Var_getParameter(funcRoot, "this");
	GString* sstr = g_string_new(NULL);
	int l = TinyJS_Var_getArrayLength(arr);
	int i;
	for(i = 0; i < l; i++) {
		if(i) { g_string_append(sstr, sep); }
		g_string_append(sstr, TinyJS_Var_getString(TinyJS_Var_getArrayIndex(arr, i)));
	}
	TinyJS_Var_setString(TinyJS_Var_getReturnVar(funcRoot), sstr->str);
}
//-----------------------------------------------------------------------------
//Register Functions
void TinyJS_registerFunctions(ST_TinyJS* tinyJS) {
	TinyJS_addNative(tinyJS, "function exec(code)",                   scExec,               NULL);	//Execute the given code.
	TinyJS_addNative(tinyJS, "function eval(code)",                   scEval,               NULL);	//Execute the given string (an expression) and return the result.
	TinyJS_addNative(tinyJS, "function trace()",                      scTrace,              NULL);
	TinyJS_addNative(tinyJS, "function Object.dump()",                scObjectDump,         NULL);
	TinyJS_addNative(tinyJS, "function Object.clone()",               scObjectClone,        NULL);
	TinyJS_addNative(tinyJS, "function Math.rand()",                  scMathRand,           NULL);
	TinyJS_addNative(tinyJS, "function Math.randInt(min,max)",        scMathRandInt,        NULL);
	TinyJS_addNative(tinyJS, "function charToInt(ch)",                scCharToInt,          NULL);	//Convert a character to an int - get its value.
	TinyJS_addNative(tinyJS, "function String.indexOf(search)",       scStringIndexOf,      NULL);	//Find the position of a string in a string, -1 if not.
	TinyJS_addNative(tinyJS, "function String.substring(lo,hi)",      scStringSubstring,    NULL);
	TinyJS_addNative(tinyJS, "function String.charAt(pos)",           scStringCharAt,       NULL);
	TinyJS_addNative(tinyJS, "function String.charCodeAt(pos)",       scStringCharCodeAt,   NULL);
	TinyJS_addNative(tinyJS, "function String.fromCharCode(ch)",      scStringFromCharCode, NULL);
	TinyJS_addNative(tinyJS, "function String.split(separator)",      scStringSplit,        NULL);
	TinyJS_addNative(tinyJS, "function Integer.parseInt(str)",        scIntegerParseInt,    NULL);	//String to int.
	TinyJS_addNative(tinyJS, "function Integer.valueOf(str)",         scIntegerValueOf,     NULL);	//Value of a single character.
	TinyJS_addNative(tinyJS, "function JSON.stringify(obj,replacer)", scJSONStringify,      NULL);	//Convert to JSON. Replacer is ignored at the moment.
	//JSON.parse is left out as you can (unsafely!) use eval instead.
	TinyJS_addNative(tinyJS, "function Array.contains(obj)",          scArrayContains,      NULL);
	TinyJS_addNative(tinyJS, "function Array.remove(obj)",            scArrayRemove,        NULL);
	TinyJS_addNative(tinyJS, "function Array.join(separator)",        scArrayJoin,          NULL);
}
