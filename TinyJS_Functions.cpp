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
	const char* str = v->getParameter("ch")->getString();
	int val = str[0];
	v->getReturnVar()->setNumber(val);
}
static void scStringIndexOf(ST_TinyJS* tinyJS, ST_TinyJS_Var* v, void* userdata) {
	const char* str = v->getParameter("this")->getString();
	const char* search = v->getParameter("search")->getString();
	const char* p = strstr(str, search);
	int val = p ? (p - str) : -1;
	v->getReturnVar()->setNumber(val);
}
static void scStringSubstring(ST_TinyJS* tinyJS, ST_TinyJS_Var* v, void* userdata) {
	const char* str = v->getParameter("this")->getString();
	int lo = v->getParameter("lo")->getNumber();
	int hi = v->getParameter("hi")->getNumber();
	int l = hi - lo;
	if((l > 0) && (lo >= 0) && ((lo + l) <= strlen(str))) {
		v->getReturnVar()->setString(strndup(str + lo, l));
	} else {
		v->getReturnVar()->setString("");
	}
}
static void scStringCharAt(ST_TinyJS* tinyJS, ST_TinyJS_Var* v, void* userdata) {
	const char* str = v->getParameter("this")->getString();
	int p = v->getParameter("pos")->getNumber();
	if(p >= 0 && p < strlen(str)) {
		v->getReturnVar()->setString(strndup(str + p, 1));
	} else {
		v->getReturnVar()->setString("");
	}
}
static void scStringCharCodeAt(ST_TinyJS* tinyJS, ST_TinyJS_Var* v, void* userdata) {
	const char* str = v->getParameter("this")->getString();
	int p = v->getParameter("pos")->getNumber();
	if((unsigned)p < strlen(str)) {
		v->getReturnVar()->setNumber(str[p]);
	} else {
		v->getReturnVar()->setNumber(0);
	}
}
static void scStringSplit(ST_TinyJS* tinyJS, ST_TinyJS_Var* v, void* userdata) {
	const char* str = v->getParameter("this")->getString();
	const char* sep = v->getParameter("separator")->getString();
	ST_TinyJS_Var* result = v->getReturnVar();
	result->setArray();
	int length = 0;
	const char* p = strstr(str, sep);
	int pos = p ? (p - str) : -1;
	while(pos != -1) {
		result->setArrayIndex(length++, new ST_TinyJS_Var(strndup(str, pos)));
		str = str + (pos + 1);
		p = strstr(str, sep);
		pos = p ? (p - str) : -1;
	}
	if(strlen(str)) {
		result->setArrayIndex(length++, new ST_TinyJS_Var(str));
	}
}
static void scStringFromCharCode(ST_TinyJS* tinyJS, ST_TinyJS_Var* v, void* userdata) {
	const char* str = strdup_printf("%c", v->getParameter("char")->getNumber());
	v->getReturnVar()->setString(str);
}
static void scIntegerParseInt(ST_TinyJS* tinyJS, ST_TinyJS_Var* v, void* userdata) {
	const char* str = v->getParameter("str")->getString();
	int val = strtol(str, NULL, 0);
	v->getReturnVar()->setNumber(val);
}
static void scIntegerValueOf(ST_TinyJS* tinyJS, ST_TinyJS_Var* v, void* userdata) {
	const char* str = v->getParameter("str")->getString();
	int val = 0;
	if(strlen(str) == 1) { val = str[0]; }
	v->getReturnVar()->setNumber(val);
}
static void scJSONStringify(ST_TinyJS* tinyJS, ST_TinyJS_Var* v, void* userdata) {
	const char* result = v->getParameter("obj")->getJSON();
	v->getReturnVar()->setString(result);
}
static void scExec(ST_TinyJS* tinyJS, ST_TinyJS_Var* v, void* userdata) {
	const char* str = v->getParameter("jsCode")->getString();
	tinyJS->execute(str);
}
static void scEval(ST_TinyJS* tinyJS, ST_TinyJS_Var* v, void* userdata) {
	const char* str = v->getParameter("jsCode")->getString();
	v->setReturnVar(tinyJS->evaluateComplex(str));
}
static void scArrayContains(ST_TinyJS* tinyJS, ST_TinyJS_Var* v, void* userdata) {
	ST_TinyJS_Var* obj = v->getParameter("obj");
	GSList/*<ST_TinyJS_VarLink*>*/* list = v->getParameter("this")->firstChild;
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
	GSList/*<ST_TinyJS_VarLink*>*/* list1 = arr->firstChild;
	while(list1) {
		ST_TinyJS_VarLink* l1 = (ST_TinyJS_VarLink*)list1->data;
		list1 = list1->next;	//�uarr->removeLink(l1)�v�ɂ����list1���J�������\�����L��̂ŗ\�ߎ��֐i�߂Ă����B
		if(l1->var->equals(obj)) {
			int i1 = l1->getIntName();
			arr->removeLink(l1);	//�I���W�i���ł�TinyJS�ɂ͂��̏��������������B�I���W�i���ł�TinyJS�͂��̏�����Y��Ă��Ȃ���?
			{
				GSList/*<ST_TinyJS_VarLink*>*/* list2 = arr->firstChild;
				while(list2) {
					ST_TinyJS_VarLink* l2 = (ST_TinyJS_VarLink*)list2->data;
					int i2 = l2->getIntName();
					if(i2 > i1) { l2->setIntName(i2 - 1); }
					list2 = list2->next;	//������͊J�������\���͖����̂ōŌ�Ɏ��֐i�߂Ė�薳���B
				}
			}
		}
	}
}
static void scArrayJoin(ST_TinyJS* tinyJS, ST_TinyJS_Var* v, void* userdata) {
	const char* sep = v->getParameter("separator")->getString();
	ST_TinyJS_Var* arr = v->getParameter("this");
	GString* sstr = g_string_new(NULL);
	int l = arr->getArrayLength();
	for(int i = 0; i < l; i++) {
		if(i) { g_string_append(sstr, sep); }
		g_string_append(sstr, arr->getArrayIndex(i)->getString());
	}
	v->getReturnVar()->setString(sstr->str);
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
