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
#ifndef __TINYJS_H__
#define __TINYJS_H__
//=============================================================================
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <assert.h>
#include <sys/stat.h>
//-----------------------------------------------------------------------------
#include "gc/leak_detector.h"
#ifdef  __cplusplus
#pragma warning(push)
#pragma warning(disable:4595)
#include "gc/gc_cpp.h"
#include "gc/gc_allocator.h"
#pragma warning(pop)
#endif//__cplusplus
#ifndef _WIN64
#pragma comment(lib, "gc.lib")
#else //_WIN64
#pragma comment(lib, "gc_x64.lib")
#endif//_WIN64
//=============================================================================
//std::string
class string : public gc_cleanup {
public:
	string(const char* p = "") {
		m_p = strdup(p);
	}
	string(const string& s) {	//コピーコンストラクタ
		m_p = strdup(s.m_p);
	}
	string& operator =(const string& s) {	//代入演算子
		free(m_p);
		m_p = strdup(s.m_p);
		return *this;
	}
	~string() {
		free(m_p);
	}
	const char* c_str() const {
		return m_p;
	}
	int length() const {
		return (int)strlen(m_p);
	}
	int find(int c, int i = 0) const {
		char s[2] = { (char)c, '\0' };
		return find(s, i);
	}
	int find(const char* p, int i = 0) const {
		assert((unsigned)i <= strlen(m_p));
		p = strstr(m_p + i, p);
		return p ? (int)(p - m_p) : -1;
	}
	int find(const string& s, int i = 0) const {
		return find(s.c_str(), i);
	}
	string substr(int i, int n = -1) const {
		assert((unsigned)i <= strlen(m_p));
		string t;
		free(t.m_p);
		if(n == -1) {
			t.m_p = strdup(m_p + i);
		} else {
			t.m_p = strndup(m_p + i, n);
		}
		return t;
	}
	int operator [](int i) const {
		assert((unsigned)i < (unsigned)strlen(m_p));
		return m_p[i];
	}
	void clear() {
		free(m_p);
		m_p = strdup("");
	}
	string& operator +=(int c) {
		char s[2] = { (char)c, '\0' };
		return *this += s;
	}
	string& operator +=(const char* p) {
		m_p = (char*)realloc(m_p, strlen(m_p) + strlen(p) + 1);
		strcat(m_p, p);
		return *this;
	}
	string& operator +=(const string& s) {
		return *this += s.c_str();
	}
	int compare(const char* p) const   { return strcmp(m_p, p); }
	int compare(const string& s) const { return compare(s.m_p); }
	int operator ==(const char* p) const   { return compare(    p) == 0; }
	int operator ==(const string& s) const { return compare(s.m_p) == 0; }
	int operator !=(const char* p) const   { return compare(    p) != 0; }
	int operator !=(const string& s) const { return compare(s.m_p) != 0; }
	int operator < (const char* p) const   { return compare(    p) <  0; }
	int operator < (const string& s) const { return compare(s.m_p) <  0; }
	int operator <=(const char* p) const   { return compare(    p) <= 0; }
	int operator <=(const string& s) const { return compare(s.m_p) <= 0; }
	int operator > (const char* p) const   { return compare(    p) >  0; }
	int operator > (const string& s) const { return compare(s.m_p) >  0; }
	int operator >=(const char* p) const   { return compare(    p) >= 0; }
	int operator >=(const string& s) const { return compare(s.m_p) >= 0; }
private:
	char*	m_p;
};
inline string operator +(const string& s1, const string& s2) {
	string t(s1);
	return t += s2;
}
inline string operator +(const char* p, const string& s) {
	return string(p) + s;
}
inline string operator +(const string& s, const char* p) {
	return s + string(p);
}
//-----------------------------------------------------------------------------
//std::vector<T>
template<class T> class vector : public gc_cleanup {
public:
	explicit vector(int n = 0) {
		m_n = n;
		m_p = new (GC) T[n];
	}
	vector(const vector<T>& v) {	//コピーコンストラクタ
		m_n = v.m_n;
		m_p = new (GC) T[v.m_n];
		for(int i = 0; i < v.m_n; i++) {
			m_p[i] = v.m_p[i];
		}
	}
	vector& operator =(const vector<T>& v) {	//代入演算子
		delete [] m_p;
		m_n = v.m_n;
		m_p = new (GC) T[v.m_n];
		for(int i = 0; i < v.m_n; i++) {
			m_p[i] = v.m_p[i];
		}
		return *this;
	}
	~vector() {
		delete [] m_p;
	}
	void clear() {
		delete [] m_p;
		m_n = 0;
		m_p = new (GC) T[0];
	}
	void push_back(const T& t) {
		T* p = new (GC) T[m_n + 1];
		for(int i = 0; i < m_n + 1; i++) {
			if(i < m_n) {
				p[i] = m_p[i];
			} else {
				p[i] = t;
			}
		}
		delete [] m_p;
		m_n++;
		m_p = p;
	}
	void pop_back() {
		assert(m_n);
		T* p = new (GC) T[m_n - 1];
		for(int i = 0; i < m_n - 1; i++) {
			p[i] = m_p[i];
		}
		delete [] m_p;
		m_n--;
		m_p = p;
	}
	const T& back() const {
		assert(m_n);
		return m_p[m_n - 1];
	}
	int size() const {
		return m_n;
	}
	int empty() const {
		return m_n == 0;
	}
	const T& operator [](int i) const {
		assert((unsigned)i < (unsigned)m_n);
		return m_p[i];
	}
private:
	int	m_n;
	T*	m_p;
};
//=============================================================================
#ifndef TRACE
#define TRACE printf
#endif//TRACE
#define TINYJS_CALL_STACK		//If defined, this keeps a note of all calls and where from in memory. This is slower, but good for debugging.
#define TINYJS_LOOP_MAX_ITERATIONS	8192
//-----------------------------------------------------------------------------
//LEX_TYPES
#define TINYJS_LEX_EOF			0
//ID
#define TINYJS_LEX_ID			(UCHAR_MAX+1)
//リテラル
#define TINYJS_LEX_L_FLOAT		(UCHAR_MAX+2)
#define TINYJS_LEX_L_INT		(UCHAR_MAX+3)
#define TINYJS_LEX_L_STR		(UCHAR_MAX+4)
//演算子
#define TINYJS_LEX_O_ANDAND		(UCHAR_MAX+5)		//"&&"
#define TINYJS_LEX_O_ANDEQUAL		(UCHAR_MAX+6)		//"&="
#define TINYJS_LEX_O_EQUAL		(UCHAR_MAX+7)		//"=="
#define TINYJS_LEX_O_GEQUAL		(UCHAR_MAX+8)		//">="
#define TINYJS_LEX_O_LEQUAL		(UCHAR_MAX+9)		//"<="
#define TINYJS_LEX_O_LSHIFT		(UCHAR_MAX+10)		//"<<"
#define TINYJS_LEX_O_LSHIFTEQUAL	(UCHAR_MAX+11)		//"<<="
#define TINYJS_LEX_O_MINUSEQUAL		(UCHAR_MAX+12)		//"-="
#define TINYJS_LEX_O_MINUSMINUS		(UCHAR_MAX+13)		//"--"
#define TINYJS_LEX_O_NEQUAL		(UCHAR_MAX+14)		//"!="
#define TINYJS_LEX_O_NTYPEEQUAL		(UCHAR_MAX+15)		//"!=="
#define TINYJS_LEX_O_OREQUAL		(UCHAR_MAX+16)		//"|="
#define TINYJS_LEX_O_OROR		(UCHAR_MAX+17)		//"||"
#define TINYJS_LEX_O_PLUSEQUAL		(UCHAR_MAX+18)		//"+="
#define TINYJS_LEX_O_PLUSPLUS		(UCHAR_MAX+19)		//"++"
#define TINYJS_LEX_O_RSHIFT		(UCHAR_MAX+20)		//">>"
#define TINYJS_LEX_O_RSHIFTEQUAL	(UCHAR_MAX+21)		//">>="
#define TINYJS_LEX_O_RSHIFTUNSIGNED	(UCHAR_MAX+22)		//">>"
#define TINYJS_LEX_O_TYPEEQUAL		(UCHAR_MAX+23)		//"==="
#define TINYJS_LEX_O_XOREQUAL		(UCHAR_MAX+24)		//"^="
//予約語
#define TINYJS_LEX_R_BREAK		(UCHAR_MAX+25)		//"break"
#define TINYJS_LEX_R_CONTINUE		(UCHAR_MAX+26)		//"continue"
#define TINYJS_LEX_R_DO			(UCHAR_MAX+27)		//"do"
#define TINYJS_LEX_R_ELSE		(UCHAR_MAX+28)		//"else"
#define TINYJS_LEX_R_FALSE		(UCHAR_MAX+29)		//"false"
#define TINYJS_LEX_R_FOR		(UCHAR_MAX+30)		//"for"
#define TINYJS_LEX_R_FUNCTION		(UCHAR_MAX+31)		//"function"
#define TINYJS_LEX_R_IF			(UCHAR_MAX+32)		//"if"
#define TINYJS_LEX_R_NEW		(UCHAR_MAX+33)		//"new"
#define TINYJS_LEX_R_NULL		(UCHAR_MAX+34)		//"null"
#define TINYJS_LEX_R_RETURN		(UCHAR_MAX+35)		//"return"
#define TINYJS_LEX_R_TRUE		(UCHAR_MAX+36)		//"true"
#define TINYJS_LEX_R_UNDEFINED		(UCHAR_MAX+37)		//"undefined"
#define TINYJS_LEX_R_VAR		(UCHAR_MAX+38)		//"var"
#define TINYJS_LEX_R_WHILE		(UCHAR_MAX+39)		//"while"
//-----------------------------------------------------------------------------
//SCRIPTVAR_FLAGS
#define TINYJS_SCRIPTVAR_UNDEFINED	0
#define TINYJS_SCRIPTVAR_NULL		(1<<0)			//It seems null is its own data type.	//┐					//┐
#define TINYJS_SCRIPTVAR_DOUBLE		(1<<1)			//Floating point double.		//├TINYJS_SCRIPTVAR_NUMERICMASK	//│
#define TINYJS_SCRIPTVAR_INTEGER	(1<<2)			//Integer number.			//┘					//│
#define TINYJS_SCRIPTVAR_STRING		(1<<3)			//String.									//├TINYJS_SCRIPTVAR_VARTYPEMASK
#define TINYJS_SCRIPTVAR_FUNCTION	(1<<4)													//│
#define TINYJS_SCRIPTVAR_OBJECT		(1<<5)													//│
#define TINYJS_SCRIPTVAR_ARRAY		(1<<6)													//┘
#define TINYJS_SCRIPTVAR_NATIVE		(1<<7)			//To specify this is a native function.
#define TINYJS_SCRIPTVAR_NUMERICMASK	(TINYJS_SCRIPTVAR_NULL|TINYJS_SCRIPTVAR_DOUBLE|TINYJS_SCRIPTVAR_INTEGER)
#define TINYJS_SCRIPTVAR_VARTYPEMASK	(TINYJS_SCRIPTVAR_NULL|TINYJS_SCRIPTVAR_DOUBLE|TINYJS_SCRIPTVAR_INTEGER|TINYJS_SCRIPTVAR_STRING|TINYJS_SCRIPTVAR_FUNCTION|TINYJS_SCRIPTVAR_OBJECT|TINYJS_SCRIPTVAR_ARRAY)
//-----------------------------------------------------------------------------
#define TINYJS_RETURN_VAR		"return"
#define TINYJS_PROTOTYPE_CLASS		"prototype"
#define TINYJS_TEMP_NAME		""
#define TINYJS_BLANK_DATA		""
//=============================================================================
class CScriptException {
public:
	CScriptException(const char* exceptionText);
	string text;
};
//=============================================================================
class CScriptLex {
public:
	CScriptLex(const char* input);
	CScriptLex(CScriptLex* owner, int startChar, int endChar);
	~CScriptLex();

	int	currCh, nextCh;
	int	tk;		//The type of the token that we have.
	int	tokenStart;	//Position in the data at the beginning of the token we have here.
	int	tokenEnd;	//Position in the data at the last character of the token we have here.
	int	tokenLastEnd;	//Position in the data at the last character of the last token.
	string	tkStr;		//Data contained in the token we have here.

	void match(int expected_tk);			//Lexical match wotsit.
	static string getTokenStr(int token);		//Get the string representation of the given token.
	void reset();					//Reset this lex so we can start again.

	string getSubString(int pos);			//Return a sub-string from the given position up until right now.
	CScriptLex* getSubLex(int lastPosition);	//Return a sub-lexer from the given position up until right now.

	string getPosition(int pos = -1);		//Return a string representing the position in lines and columns of the character pos given.

protected:
	//When we go into a loop, we use getSubLex() to get a lexer for just the sub-part of the relevant string.
	//This doesn't re-allocate and copy the string, but instead copies the data pointer and sets dataOwned to false, and dataStart/dataEnd to the relevant things.
	char*	data;			//Data string to get tokens from.
	int	dataStart, dataEnd;	//Start and end position in data string.
	bool	dataOwned;		//Do we own this data string?
	int	dataPos;		//Position in data (we CAN go past the end of the string here)

	void getNextCh();
	void getNextToken();		//Get the text token from our text string
};
//=============================================================================
class CTinyJS;
class CScriptVar;
typedef void (*JSCallback)(CTinyJS* tinyJS, CScriptVar* var, void* userData);
//=============================================================================
class CScriptVarLink {
public:
	string			name;
	CScriptVarLink*		nextSibling;
	CScriptVarLink*		prevSibling;
	CScriptVar*		var;
	bool			owned;

	CScriptVarLink(CScriptVar* var, const char* name = TINYJS_TEMP_NAME);
	CScriptVarLink(const CScriptVarLink& link);	//Copy constructor.
	~CScriptVarLink();
	void replaceWith(CScriptVar* newVar);		//Replace the Variable pointed to.
	void replaceWith(CScriptVarLink* newVar);	//Replace the Variable pointed to (just dereferences).
	int getIntName();				//Get the name as an integer (for arrays).
	void setIntName(int n);				//Set the name as an integer (for arrays).
};
//=============================================================================
//Variable class (containing a doubly-linked list of children).
class CScriptVar {
public:
	CScriptVar();					//Create undefined.
	CScriptVar(const char* varData, int varFlags);	//User defined.
	CScriptVar(const char* str);			//Create a string.
	CScriptVar(double varData);
	CScriptVar(int val);
	~CScriptVar();

	CScriptVar* getReturnVar();				//If this is a function, get the result value (for use by native functions).
	void setReturnVar(CScriptVar* var);			//Set the result value. Use this when setting complex return data as it avoids a deepCopy().
	CScriptVar* getParameter(const char* name);		//If this is a function, get the parameter with the given name (for use by native functions).

	CScriptVarLink* findChild(const char* childName);							//Tries to find a child with the given name, may return 0.
	CScriptVarLink* findChildOrCreate(const char* childName, int varFlags = TINYJS_SCRIPTVAR_UNDEFINED);	//Tries to find a child with the given name, or will create it with the given flags.
	CScriptVarLink* findChildOrCreateByPath(const char* path);						//Tries to find a child with the given path (separated by dots).
	CScriptVarLink* addChild(const char* childName, CScriptVar* child = NULL);
	CScriptVarLink* addChildNoDup(const char* childName, CScriptVar* child = NULL);	//add a child overwriting any with the same name.
	void removeChild(CScriptVar* child);
	void removeLink(CScriptVarLink* link);			//Remove a specific link (this is faster than finding via a child).
	void removeAllChildren();
	CScriptVar* getArrayIndex(int idx);			//The the value at an array index.
	void setArrayIndex(int idx, CScriptVar* value);		//Set the value at an array index.
	int getArrayLength();					//If this is an array, return the number of items in it (else 0).
	int getChildren();					//Get the number of children.

	int getInt();
	bool getBool() { return getInt() != 0; }
	double getDouble();
	string getString();
	string getParsableString();				//Get data as a parsable javascript string.
	void setInt(int num);
	void setDouble(double val);
	void setString(const char* str);
	void setUndefined();
	void setArray();
	bool equals(CScriptVar* v);

	bool isInt() { return (flags & TINYJS_SCRIPTVAR_INTEGER) != 0; }
	bool isDouble() { return (flags & TINYJS_SCRIPTVAR_DOUBLE) != 0; }
	bool isString() { return (flags & TINYJS_SCRIPTVAR_STRING) != 0; }
	bool isNumeric() { return (flags & TINYJS_SCRIPTVAR_NUMERICMASK) != 0; }
	bool isFunction() { return (flags & TINYJS_SCRIPTVAR_FUNCTION) != 0; }
	bool isObject() { return (flags & TINYJS_SCRIPTVAR_OBJECT) != 0; }
	bool isArray() { return (flags & TINYJS_SCRIPTVAR_ARRAY) != 0; }
	bool isNative() { return (flags & TINYJS_SCRIPTVAR_NATIVE) != 0; }
	bool isUndefined() { return (flags & TINYJS_SCRIPTVAR_VARTYPEMASK) == TINYJS_SCRIPTVAR_UNDEFINED; }
	bool isNull() { return (flags & TINYJS_SCRIPTVAR_NULL) != 0; }
	bool isBasic() { return firstChild == 0; }			//Is this *not* an array/object/etc.

	CScriptVar* mathsOp(CScriptVar* b, int op);			//Do a maths op with another script variable.
	void copyValue(CScriptVar* val);				//Copy the value from the value given.
	CScriptVar* deepCopy();						//Deep copy this node and return the result.

	void trace(const char* indentStr = "", const char* name = "");	//Dump out the contents of this using trace.
	string getFlagsAsString();					//For debugging - just dump a string version of the flags.
	string getJSON(const char* linePrefix = "");			//Write out all the JS code needed to recreate this script variable to the stream (as JSON).
	void setCallback(JSCallback callback, void* userData);		//Set the callback for native functions.

	CScriptVarLink* firstChild;
	CScriptVarLink* lastChild;

	//For memory management/garbage collection.
	CScriptVar* ref();			//Add reference to this variable.
	void unref();				//Remove a reference, and delete this variable if required.
	int getRefs();				//Get the number of references to this script variable.
protected:
	int		refs;			//The number of references held to this - used for garbage collection.

	string		data;			//The contents of this variable if it is a string.
	int		intData;		//The contents of this variable if it is an int.
	double		doubleData;		//The contents of this variable if it is a double.
	int		flags;			//The flags determine the type of the variable - int/double/string/etc.
	JSCallback	jsCallback;		//Callback for native functions.
	void*		jsCallbackUserData;	//User data passed as second argument to native functions.

	void init();				//Initialisation of data members.
	void copySimpleData(CScriptVar* val);	//Copy the basic data and flags from the variable given, with no children. Should be used internally only - by copyValue and deepCopy.

	friend class CTinyJS;
};
//=============================================================================
class CTinyJS {
public:
	CTinyJS();
	~CTinyJS();

	void execute(const char* code);
	//Evaluate the given code and return a link to a javascript object, useful for(dangerous) JSON parsing.
	//If nothing to return, will return 'undefined' variable type.
	//CScriptVarLink is returned as this will automatically unref the result as it goes out of scope.
	//If you want to keep it, you must use ref() and unref().
	CScriptVarLink evaluateComplex(const char* code);
	//Evaluate the given code and return a string.
	//If nothing to return, will return 'undefined'.
	string evaluate(const char* code);

	//Add a native function to be called from TinyJS.
	//example:
	//│void scRandInt(CScriptVar* var, void* userData) { ... }
	//│tinyJS->addNative("function randInt(min, max)", scRandInt, 0);
	//or
	//│void scSubstring(CScriptVar* var, void* userData) { ... }
	//│tinyJS->addNative("function String.substring(lo, hi)", scSubstring, 0);
	void addNative(const char* funcDesc, JSCallback ptr, void* userData);

	//Get the given variable specified by a path (var1.var2.etc), or return 0.
	CScriptVar* getScriptVariable(const char* path);
//{{削除。この関数がNULLを返せるようにするために他の関数に色々無理が生じている。この関数を廃止して明示的にgetScriptVariable()を使うようにすれば自然になる。だいたい当関数はアプリケーション用に用意されたもののようで、ライブラリ内の実装においては使用されていなかった。
//	//Get the value of the given variable, or return 0.
//	const string* getVariable(const char* path);
//}}削除。この関数がNULLを返せるようにするために他の関数に色々無理が生じている。この関数を廃止して明示的にgetScriptVariable()を使うようにすれば自然になる。だいたい当関数はアプリケーション用に用意されたもののようで、ライブラリ内の実装においては使用されていなかった。
	//Set the value of the given variable, return trur if it exists and gets set.
	bool setVariable(const char* path, const char* varData);

	//Send all variables to stdout.
	void trace(const char* indentStr = "");

	CScriptVar*		root;		//Root of symbol table.		//※要検討:グローバルオブジェクトの事です。Webブラウザでの実装の場合「window」が、Node.jsの場合は「global」がグローバルオブジェクトとなります。rootという変数名はやめて、window,又は,globalに変える方が良いのでは？
private:
	CScriptLex*		l;		//Current lexer.
	vector<CScriptVar*>	scopes;		//Stack of scopes when parsing.
#ifdef  TINYJS_CALL_STACK
	vector<string>		call_stack;	//Names of places called so we can show when erroring.
#endif//TINYJS_CALL_STACK

	CScriptVar*		stringClass;	//Built in string class.
	CScriptVar*		objectClass;	//Built in object class.
	CScriptVar*		arrayClass;	//Built in array class.

	//Parsing - in order of precedence.
	CScriptVarLink* functionCall(bool& execute, CScriptVarLink* function, CScriptVar* parent);
	CScriptVarLink* factor(bool& execute);
	CScriptVarLink* unary(bool& execute);
	CScriptVarLink* term(bool& execute);
	CScriptVarLink* expression(bool& execute);
	CScriptVarLink* shift(bool& execute);
	CScriptVarLink* condition(bool& execute);
	CScriptVarLink* logic(bool& execute);
	CScriptVarLink* ternary(bool& execute);
	CScriptVarLink* base(bool& execute);
	void block(bool& execute);
	void statement(bool& execute);
	//Parsing utility functions.
	CScriptVarLink* parseFunctionDefinition();
	void parseFunctionArguments(CScriptVar* funcVar);

	CScriptVarLink* findInScopes(const char* childName);				//Finds a child, looking recursively up the scopes.
	CScriptVarLink* findInParentClasses(CScriptVar* object, const char* name);	//Look up in any parent classes of the given object.
};
#endif//__TINYJS_H__
