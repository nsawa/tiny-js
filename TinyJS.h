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
//*****************************************************************************
//	
//*****************************************************************************
#include "clip/clip.h"
//*****************************************************************************
//	std::stringの代用
//*****************************************************************************
class string : public gc_cleanup {
public:
	string(const char* p = "") {
		m_p = strdup(p);
	}
	string(const char* p, int n) {
		m_p = strndup(p, n);
	}
	string(const string& s) {	//コピーコンストラクタ
		m_p = strdup(s.m_p);
	}
	string& operator =(const string& s) {	//代入演算子
		m_p = strdup(s.m_p);
		return *this;
	}
	const char* c_str() const {
		return m_p;
	}
	int length() const {
		return strlen(m_p);
	}
	int find(int c, int i = 0) const {
		char s[2] = { (char)c, '\0' };
		return find(s, i);
	}
	int find(const char* p, int i = 0) const {
		assert((unsigned)i <= strlen(m_p));
		p = strstr(m_p + i, p);
		return p ? (p - m_p) : -1;
	}
	int find(const string& s, int i = 0) const {
		return find(s.c_str(), i);
	}
	string substr(int i, int n = -1) const {
		assert((unsigned)i <= strlen(m_p));
		string t;
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
//*****************************************************************************
//	
//*****************************************************************************
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
#define TINYJS_LEX_L_NUM		(UCHAR_MAX+2)
#define TINYJS_LEX_L_STR		(UCHAR_MAX+3)
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
#define TINYJS_VAR_UNDEFINED		0
#define TINYJS_VAR_NULL			1			//It seems null is its own data type.
#define TINYJS_VAR_NUMBER		2			//Number.
#define TINYJS_VAR_STRING		3			//String.
#define TINYJS_VAR_FUNCTION		4
#define TINYJS_VAR_OBJECT		5
#define TINYJS_VAR_ARRAY		6
#define TINYJS_VAR_NATIVE		7			//To specify this is a native function.
//-----------------------------------------------------------------------------
#define TINYJS_RETURN_VAR		"return"
#define TINYJS_PROTOTYPE_CLASS		"prototype"
#define TINYJS_TEMP_NAME		""
#define TINYJS_BLANK_DATA		""
//-----------------------------------------------------------------------------
class ST_TinyJS;
class ST_TinyJS_Var;
typedef void TinyJS_Callback(ST_TinyJS* tinyJS, ST_TinyJS_Var* v, void* userdata);
//*****************************************************************************
//	ST_TinyJS_Exception
//*****************************************************************************
class ST_TinyJS_Exception : public gc_cleanup {
public:
	ST_TinyJS_Exception(const char* exceptionText);
	string text;
};
//*****************************************************************************
//	ST_TinyJS_Lex
//*****************************************************************************
class ST_TinyJS_Lex : public gc_cleanup {
public:
	ST_TinyJS_Lex(const char* input, int startChar, int endChar);

	int	tk;		//The type of the token that we have.
	string	tkStr;		//Data contained in the token we have here.
	int	tokenStart;	//Position in the data at the beginning of the token we have here.
	int	tokenEnd;	//Position in the data at the last character of the token we have here.

	void reset();					//Reset this lex so we can start again.
	void match(int expected_tk);			//Lexical match wotsit.
	static string getTokenStr(int token);		//Get the string representation of the given token.

	string getSubString(int pos);			//Return a sub-string from the given position up until right now.
	ST_TinyJS_Lex* getSubLex(int lastPosition);	//Return a sub-lexer from the given position up until right now.

	string getLastPosition();			//Return a string representing the position in lines and columns of the tokenLastEnd.
	string getPosition(int pos);			//Return a string representing the position in lines and columns of the character pos given.

private:
	//When we go into a loop, we use getSubLex() to get a lexer for just the sub-part of the relevant string.
	//This doesn't re-allocate and copy the string, but instead copies the data pointer and sets dataOwned to false, and dataStart/dataEnd to the relevant things.
	const char*	data;			//Data string to get tokens from.
	int		dataStart, dataEnd;	//Start and end position in data string.
	int		dataPos;		//Position in data (we CAN go past the end of the string here)
	int		currCh, nextCh;
	int		tokenLastEnd;		//Position in the data at the last character of the last token.

	void getNextCh();
	void getNextToken();	//Get the text token from our text string.
};
//*****************************************************************************
//	ST_TinyJS_VarLink
//*****************************************************************************
class ST_TinyJS_VarLink : public gc_cleanup {
public:
	ST_TinyJS_VarLink(ST_TinyJS_Var* v, const char* name = TINYJS_TEMP_NAME);
	ST_TinyJS_VarLink(const ST_TinyJS_VarLink& link);	//Copy constructor.

	string			name;
	ST_TinyJS_Var*		var;
	bool			owned;

	void replaceWith(ST_TinyJS_Var* v);	//Replace the Variable pointed to.
	void replaceWith(ST_TinyJS_VarLink* l);	//Replace the Variable pointed to (just dereferences).
	int getIntName();			//Get the name as an integer (for arrays).
	void setIntName(int n);			//Set the name as an integer (for arrays).
};
//*****************************************************************************
//	ST_TinyJS_Var
//*****************************************************************************
//Variable class (containing a doubly-linked list of children).
class ST_TinyJS_Var : public gc_cleanup {
public:
	ST_TinyJS_Var();					//Create undefined.
	ST_TinyJS_Var(const char* varData, int varFlags);	//User defined.
	ST_TinyJS_Var(const char* str);			//Create a string.
	ST_TinyJS_Var(double val);

	ST_TinyJS_Var* getReturnVar();			//If this is a function, get the result value (for use by native functions).
	void setReturnVar(ST_TinyJS_Var* v);		//Set the result value. Use this when setting complex return data as it avoids a deepCopy().
	ST_TinyJS_Var* getParameter(const char* name);	//If this is a function, get the parameter with the given name (for use by native functions).

	ST_TinyJS_VarLink* findChild(const char* childName);						//Tries to find a child with the given name, may return NULL.
	ST_TinyJS_VarLink* findChildOrCreate(const char* name, int varType = TINYJS_VAR_UNDEFINED);	//Tries to find a child with the given name, or will create it with the given type.
	ST_TinyJS_VarLink* findChildOrCreateByPath(const char* path);					//Tries to find a child with the given path (separated by dots).
	ST_TinyJS_VarLink* addChild(const char* name, ST_TinyJS_Var* v = NULL);
	void removeChild(ST_TinyJS_Var* v);
	void removeLink(ST_TinyJS_VarLink* link);		//Remove a specific link (this is faster than finding via a child).
	void removeAllChildren();
	ST_TinyJS_Var* getArrayIndex(int i);		//The the value at an array index.
	void setArrayIndex(int i, ST_TinyJS_Var* v);	//Set the value at an array index.
	int getArrayLength();				//If this is an array, return the number of items in it (else 0).
	int getChildCount();				//Get the number of children.

	bool getBool();
	double getNumber();
	string getString();
	string getParsableString();			//Get data as a parsable javascript string.
	void setNumber(double val);
	void setString(const char* str);
	void setUndefined();
	void setArray();
	bool equals(ST_TinyJS_Var* v);

	bool isUndefined() { return type == TINYJS_VAR_UNDEFINED; }
	bool isNull()      { return type == TINYJS_VAR_NULL; }
	bool isNumber()    { return type == TINYJS_VAR_NUMBER; }
	bool isString()    { return type == TINYJS_VAR_STRING; }
	bool isFunction()  { return type == TINYJS_VAR_FUNCTION || type == TINYJS_VAR_NATIVE; }
	bool isObject()    { return type == TINYJS_VAR_OBJECT; }
	bool isArray()     { return type == TINYJS_VAR_ARRAY; }
	bool isNative()    { return type == TINYJS_VAR_NATIVE; }
	bool isBasic()     { return firstChild == NULL; }		//Is this *not* an array/object/etc.

	ST_TinyJS_Var* mathsOp(ST_TinyJS_Var* v, int op);			//Do a maths op with another script variable.
	void copyValue(ST_TinyJS_Var* v);					//Copy the value from the value given.
	ST_TinyJS_Var* deepCopy();						//Deep copy this node and return the result.

	void trace(const char* indent = "", const char* name = "");	//Dump out the contents of this using trace.
	string getTypeAsString();					//For debugging - just dump a string version of the type.
	string getJSON(const char* linePrefix = "");			//Write out all the JS code needed to recreate this script variable to the stream (as JSON).
	void setCallback(TinyJS_Callback* callback, void* userdata);	//Set the callback for native functions.

	GSList/*<ST_TinyJS_VarLink*>*/*	firstChild;
private:
	int				type;		//The type determine the type of the variable - number/string/etc.
	double				numData;	//The contents of this variable if it is a number.
	string				strData;	//The contents of this variable if it is a string.
	TinyJS_Callback*		callback;	//Callback for native functions.
	void*				userdata;	//User data passed as second argument to native functions.

	void init();					//Initialisation of data members.
	void copySimpleData(ST_TinyJS_Var* v);		//Copy the basic data and type from the variable given, with no children. Should be used internally only - by copyValue and deepCopy.

	friend class ST_TinyJS;
};
//*****************************************************************************
//	ST_TinyJS_Context
//*****************************************************************************
struct CTinyJS_Context : public gc_cleanup {
	ST_TinyJS_Lex*			lex;		//Current lexer.
	GSList/*<ST_TinyJS_Var*>*/*	scopes;		//Stack of scopes when parsing.		//※元は先頭がrootで末尾が現在のスタックだったが、逆にして、先頭が現在のスタックで末尾をrootにした。その方がfindInScopes()の実装上も都合が良いし、今後クロージャを作る時にも自然に実装出来るはずだ。リストの末尾方向(rootに向けての方向)へのリンクは、クロージャを作った時点から変更される事は無いので、単純にその時点でのscopesを保持すれば良くなるので。
#ifdef  TINYJS_CALL_STACK
	GSList/*<char*>*/*		call_stack;	//Names of places called so we can show when erroring.
#endif//TINYJS_CALL_STACK
};
//*****************************************************************************
//	ST_TinyJS
//*****************************************************************************
class ST_TinyJS : public gc_cleanup {
public:
	ST_TinyJS();

	void execute(const char* code);
	//Evaluate the given code and return a link to a javascript object, useful for(dangerous) JSON parsing.
	//If nothing to return, will return 'undefined' variable type.
	//ST_TinyJS_VarLink is returned as this will automatically unref the result as it goes out of scope.
	//If you want to keep it, you must use ref() and unref().
	ST_TinyJS_VarLink evaluateComplex(const char* code);
	//Evaluate the given code and return a string.
	//If nothing to return, will return 'undefined'.
	string evaluate(const char* code);

	//Add a native function to be called from TinyJS.
	//example:
	//│void scRandInt(ST_TinyJS_Var* v, void* userdata) { ... }
	//│tinyJS->addNative("function randInt(min, max)", scRandInt, NULL);
	//or
	//│void scSubstring(ST_TinyJS_Var* v, void* userdata) { ... }
	//│tinyJS->addNative("function String.substring(lo, hi)", scSubstring, NULL);
	void addNative(const char* funcDesc, TinyJS_Callback* callback, void* userdata);

	//Get the given variable specified by a path (var1.var2.etc), or return NULL.
	ST_TinyJS_Var* getScriptVariable(const char* path);
	//Set the value of the given variable, return trur if it exists and gets set.
	bool setVariable(const char* path, const char* varData);

	//Send all variables to stdout.
	void trace(const char* indent = "");

	ST_TinyJS_Var*			root;		//Root of symbol table.			//※要検討:グローバルオブジェクトの事です。Webブラウザでの実装の場合「window」が、Node.jsの場合は「global」がグローバルオブジェクトとなります。rootという変数名はやめて、window,又は,globalに変える方が良いのでは？
private:
	ST_TinyJS_Lex*			lex;		//Current lexer.
	GSList/*<ST_TinyJS_Var*>*/*	scopes;		//Stack of scopes when parsing.		//※元は先頭がrootで末尾が現在のスタックだったが、逆にして、先頭が現在のスタックで末尾をrootにした。その方がfindInScopes()の実装上も都合が良いし、今後クロージャを作る時にも自然に実装出来るはずだ。リストの末尾方向(rootに向けての方向)へのリンクは、クロージャを作った時点から変更される事は無いので、単純にその時点でのscopesを保持すれば良くなるので。
#ifdef  TINYJS_CALL_STACK
	GSList/*<char*>*/*		call_stack;	//Names of places called so we can show when erroring.
#endif//TINYJS_CALL_STACK

	ST_TinyJS_Var*			stringClass;	//Built in string class.
	ST_TinyJS_Var*			objectClass;	//Built in object class.
	ST_TinyJS_Var*			arrayClass;	//Built in array class.

	//Parsing - in order of precedence.
	ST_TinyJS_VarLink* functionCall(bool* pExec, ST_TinyJS_VarLink* function, ST_TinyJS_Var* parent);
	ST_TinyJS_VarLink* factor(bool* pExec);
	ST_TinyJS_VarLink* unary(bool* pExec);
	ST_TinyJS_VarLink* term(bool* pExec);
	ST_TinyJS_VarLink* expression(bool* pExec);
	ST_TinyJS_VarLink* shift(bool* pExec);
	ST_TinyJS_VarLink* condition(bool* pExec);
	ST_TinyJS_VarLink* logic(bool* pExec);
	ST_TinyJS_VarLink* ternary(bool* pExec);
	ST_TinyJS_VarLink* base(bool* pExec);
	void block(bool* pExec);
	void statement(bool* pExec);
	//Parsing utility functions.
	ST_TinyJS_VarLink* parseFunctionDefinition();
	void parseFunctionArguments(ST_TinyJS_Var* funcVar);

	ST_TinyJS_VarLink* findInScopes(const char* name);				//Finds a child, looking recursively up the scopes.
	ST_TinyJS_VarLink* findInParentClasses(ST_TinyJS_Var* obj, const char* name);	//Look up in any parent classes of the given object.

	string stack_trace(ST_TinyJS_Exception* e);	//元ソースにはこの関数は有りませんが、共通処理をまとめるために追加しました。
};
#endif//__TINYJS_H__
