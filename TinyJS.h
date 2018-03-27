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
#include "clip/clip.h"
//*****************************************************************************
//	
//*****************************************************************************
#define TinyJS_Exception		MAKEFOURCC('T','i','J','S')
//-----------------------------------------------------------------------------
//ST_TinyJS_Lex.tk
#define TINYJS_LEX_EOF			0
//ID
#define TINYJS_LEX_ID			(UCHAR_MAX+1)
//リテラル
#define TINYJS_LEX_L_NUM		(UCHAR_MAX+2)
#define TINYJS_LEX_L_STR		(UCHAR_MAX+3)
//演算子
#define TINYJS_LEX_O_ANDAND		(UCHAR_MAX+5)		//"&&"
#define TINYJS_LEX_O_ANDASSIGN		(UCHAR_MAX+6)		//"&="		//※字句解析で検出されているが、構文解析で処理されていない。
#define TINYJS_LEX_O_EQUAL		(UCHAR_MAX+7)		//"=="
#define TINYJS_LEX_O_GEQUAL		(UCHAR_MAX+8)		//">="
#define TINYJS_LEX_O_LEQUAL		(UCHAR_MAX+9)		//"<="
#define TINYJS_LEX_O_LSHIFT		(UCHAR_MAX+10)		//"<<"
#define TINYJS_LEX_O_LSHIFTASSIGN	(UCHAR_MAX+11)		//"<<="		//※字句解析で検出されているが、構文解析で処理されていない。
#define TINYJS_LEX_O_MINUSASSIGN	(UCHAR_MAX+12)		//"-="
#define TINYJS_LEX_O_MINUSMINUS		(UCHAR_MAX+13)		//"--"
#define TINYJS_LEX_O_NEQUAL		(UCHAR_MAX+14)		//"!="
#define TINYJS_LEX_O_NTYPEEQUAL		(UCHAR_MAX+15)		//"!=="
#define TINYJS_LEX_O_OREQUAL		(UCHAR_MAX+16)		//"|="		//※字句解析で検出されているが、構文解析で処理されていない。
#define TINYJS_LEX_O_OROR		(UCHAR_MAX+17)		//"||"
#define TINYJS_LEX_O_PLUSASSIGN		(UCHAR_MAX+18)		//"+="
#define TINYJS_LEX_O_PLUSPLUS		(UCHAR_MAX+19)		//"++"
#define TINYJS_LEX_O_RSHIFT		(UCHAR_MAX+20)		//">>"
#define TINYJS_LEX_O_RSHIFTASSIGN	(UCHAR_MAX+21)		//">>="		//※字句解析で検出されているが、構文解析で処理されていない。
#define TINYJS_LEX_O_RSHIFTUNSIGNED	(UCHAR_MAX+22)		//">>"
#define TINYJS_LEX_O_TYPEEQUAL		(UCHAR_MAX+23)		//"==="
#define TINYJS_LEX_O_XORASSIGN		(UCHAR_MAX+24)		//"^="		//※字句解析で検出されているが、構文解析で処理されていない。
//予約語
#define TINYJS_LEX_R_BREAK		(UCHAR_MAX+25)		//"break"	//※字句解析で検出されているが、構文解析で処理されていない。
#define TINYJS_LEX_R_CONTINUE		(UCHAR_MAX+26)		//"continue"	//※字句解析で検出されているが、構文解析で処理されていない。
#define TINYJS_LEX_R_DO			(UCHAR_MAX+27)		//"do"		//※字句解析で検出されているが、構文解析で処理されていない。
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
//ST_TinyJS_Var.type
#define TINYJS_VAR_UNDEFINED		0
#define TINYJS_VAR_NULL			1			//It seems null is its own data type.
#define TINYJS_VAR_NUMBER		2			//Number.
#define TINYJS_VAR_STRING		3			//String.
#define TINYJS_VAR_FUNCTION		4
#define TINYJS_VAR_OBJECT		5
#define TINYJS_VAR_ARRAY		6
#define TINYJS_VAR_NATIVE		7			//To specify this is a native function.
//-----------------------------------------------------------------------------
class ST_TinyJS;
class ST_TinyJS_Lex;
class ST_TinyJS_Var;
class ST_TinyJS_VarLink;
struct ST_TinyJS_Context;
typedef void TinyJS_Callback(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData);
//*****************************************************************************
//	ST_TinyJS
//*****************************************************************************
class ST_TinyJS : public gc_cleanup {
private:
	ST_TinyJS();
public:
	static ST_TinyJS* TinyJS_new();
	//
	void TinyJS_execute(const char* code);
	ST_TinyJS_Var* TinyJS_evaluate(const char* code);
	void TinyJS_addNative(const char* funcDesc, TinyJS_Callback* callback, void* userData);
	//
	void TinyJS_trace(const char* indent);
	//
	ST_TinyJS_Var*			root;		//Root of symbol table.			//※要検討:グローバルオブジェクトの事です。Webブラウザでの実装の場合「window」が、Node.jsの場合は「global」がグローバルオブジェクトとなります。rootという変数名はやめて、window,又は,globalに変える方が良いのでは？
private:
	ST_TinyJS_Lex*			lex;		//Current lexer.
	GSList/*<ST_TinyJS_Var*>*/*	scopes;		//Stack of scopes when parsing.		//※元は先頭がrootで末尾が現在のスタックだったが、逆にして、先頭が現在のスタックで末尾をrootにした。その方がfindInScopes()の実装上も都合が良いし、今後クロージャを作る時にも自然に実装出来るはずだ。リストの末尾方向(rootに向けての方向)へのリンクは、クロージャを作った時点から変更される事は無いので、単純にその時点でのscopesを保持すれば良くなるので。
	GSList/*<const char*>*/*	callStack;	//Names of places called so we can show when erroring.
	ST_TinyJS_Var*			stringClass;	//Built in string class.
	ST_TinyJS_Var*			objectClass;	//Built in object class.
	ST_TinyJS_Var*			arrayClass;	//Built in array class.
	//Parsing - in order of precedence.
	ST_TinyJS_VarLink* TinyJS_functionCall(int* pExec, ST_TinyJS_VarLink* func, ST_TinyJS_Var* _this/*NULL可*/);
	ST_TinyJS_VarLink* TinyJS_factor(int* pExec);
	ST_TinyJS_VarLink* TinyJS_unary(int* pExec);
	ST_TinyJS_VarLink* TinyJS_term(int* pExec);
	ST_TinyJS_VarLink* TinyJS_expression(int* pExec);
	ST_TinyJS_VarLink* TinyJS_shift(int* pExec);
	ST_TinyJS_VarLink* TinyJS_condition(int* pExec);
	ST_TinyJS_VarLink* TinyJS_logic(int* pExec);
	ST_TinyJS_VarLink* TinyJS_ternary(int* pExec);
	ST_TinyJS_VarLink* TinyJS_base(int* pExec);
	void TinyJS_block(int* pExec);
	void TinyJS_statement(int* pExec);
	//
	ST_TinyJS_VarLink* TinyJS_parseFunctionDefinition();
	void TinyJS_parseFunctionArguments(ST_TinyJS_Var* funcVar);
	ST_TinyJS_VarLink* TinyJS_findInScopes(const char* name);
	ST_TinyJS_VarLink* TinyJS_findInPrototypeClasses(ST_TinyJS_Var* v, const char* name);
	ST_TinyJS_Context* TinyJS_saveContext();
	void TinyJS_restoreContext(ST_TinyJS_Context* pSavedContext);
	const char* TinyJS_stackTrace(const char* errMsg);
};
//*****************************************************************************
//	ST_TinyJS_Lex
//*****************************************************************************
class ST_TinyJS_Lex : public gc_cleanup {
public:
	ST_TinyJS_Lex(const char* input, int startChar, int endChar);
	//
	ST_TinyJS_Lex* reset();					//Reset this lex so we can start again.
	void TinyJS_Lex_match(int tkExpected);			//Lexical match wotsit.
	static const char* TinyJS_Lex_getTokenStr(int token);	//Get the string representation of the given token.
	//
	const char* TinyJS_Lex_getSubString(int pos);		//Return a sub-string from the given position up until right now.
	ST_TinyJS_Lex* TinyJS_Lex_getSubLex(int lastPosition);	//Return a sub-lexer from the given position up until right now.
	//
	const char* TinyJS_Lex_getLastPosition();		//Return a string representing the position in lines and columns of the tokenLastEnd.
	const char* TinyJS_Lex_getPosition(int pos);		//Return a string representing the position in lines and columns of the character pos given.
	//
	int		tk;					//トークンの種類。(TINYJS_LEX_*,又は,その他の一文字演算子,又は,不正な文字)
	const char*	tkStr;					//トークンの文字列。
	int		tokenStart;				//最後に読んだトークン(=tk,tkStr)の、最初の文字の位置。
	int		tokenEnd;				//最後に読んだトークン(=tk,tkStr)の、最後の文字の次の位置。
private:
	//When we go into a loop, we use getSubLex() to get a lexer for just the sub-part of the relevant string.
	//This doesn't re-allocate and copy the string, but instead copies the data pointer and sets dataOwned to false, and dataStart/dataEnd to the relevant things.
	const char*	data;					//Data string to get tokens from.
	int		dataStart, dataEnd;			//Start and end position in data string.
	int		dataPos;				//Position in data (we CAN go past the end of the string here).
	int		currCh, nextCh;				//現在の文字，次の文字。
	int		tokenLastEnd;				//一つ前に読んだトークンの、最後の文字の次の位置。
	//
	void TinyJS_Lex_getNextCh();
	void TinyJS_Lex_getNextToken();				//Get the text token from our text string.
};
//*****************************************************************************
//	ST_TinyJS_Var
//*****************************************************************************
//Variable class (containing a doubly-linked list of children).
class ST_TinyJS_Var : public gc_cleanup {
public:
	static ST_TinyJS_Var* TinyJS_Var_newUndefined();
	static ST_TinyJS_Var* TinyJS_Var_newNull();
	static ST_TinyJS_Var* TinyJS_Var_newNumber(double val);
	static ST_TinyJS_Var* TinyJS_Var_newString(const char* str);
	static ST_TinyJS_Var* TinyJS_Var_newFunction();
	static ST_TinyJS_Var* TinyJS_Var_newObject();
	static ST_TinyJS_Var* TinyJS_Var_newArray();
	static ST_TinyJS_Var* TinyJS_Var_newNative(TinyJS_Callback* callback, void* userData);
	//
	void TinyJS_Var_setUndefined();
	void TinyJS_Var_setNull();
	void TinyJS_Var_setNumber(double val);
	void TinyJS_Var_setString(const char* str);
	void TinyJS_Var_setFunction();
	void TinyJS_Var_setObject();
	void TinyJS_Var_setArray();
	void TinyJS_Var_setNative(TinyJS_Callback* callback, void* userData);
	//
	int TinyJS_Var_isUndefined();
	int TinyJS_Var_isNull();
	int TinyJS_Var_isNumber();
	int TinyJS_Var_isString();
	int TinyJS_Var_isFunction();
	int TinyJS_Var_isObject();
	int TinyJS_Var_isArray();
	int TinyJS_Var_isNative();
	int TinyJS_Var_isPrimitive();
	//
	ST_TinyJS_Var* getReturnVar();					//If this is a function, get the result value (for use by native functions).
	void setReturnVar(ST_TinyJS_Var* v);				//Set the result value. Use this when setting complex return data as it avoids a deepCopy().
	ST_TinyJS_Var* getParameter(const char* name);			//If this is a function, get the parameter with the given name (for use by native functions).
	//
	ST_TinyJS_VarLink* findChild(const char* childName);		//Tries to find a child with the given name, may return NULL.
	ST_TinyJS_VarLink* findChildOrCreate(const char* name);		//Tries to find a child with the given name, or will create it with the given type.
	ST_TinyJS_VarLink* addChild(const char* name, ST_TinyJS_Var* v);
	void removeLink(ST_TinyJS_VarLink* link);			//Remove a specific link (this is faster than finding via a child).
	//
	ST_TinyJS_Var* getArrayIndex(int i);				//The the value at an array index.
	void TinyJS_Var_setArrayIndex(int i, ST_TinyJS_Var* v);		//Set the value at an array index.
	int TinyJS_Var_getArrayLength();				//If this is an array, return the number of items in it (else 0).
	//
	int TinyJS_Var_getBoolean();
	double TinyJS_Var_getNumber();
	const char* TinyJS_Var_getString();
	const char* TinyJS_Var_getParsableString();			//Get data as a parsable javascript string.
	//
	int TinyJS_Var_equals(ST_TinyJS_Var* v);
	ST_TinyJS_Var* TinyJS_Var_mathsOp(ST_TinyJS_Var* v, int op);	//Do a maths op with another script variable.
	ST_TinyJS_Var* TinyJS_Var_deepCopy();				//Deep copy this node and return the result.
	//
	void TinyJS_Var_trace(const char* indent, const char* name);	//Dump out the contents of this using trace.
	const char* TinyJS_Var_getTypeAsString();			//For debugging - just dump a string version of the type.
	const char* TinyJS_Var_getJSON(const char* linePrefix);		//Write out all the JS code needed to recreate this script variable to the stream (as JSON).
	//
	GSList/*<ST_TinyJS_VarLink*>*/*	firstChild;
private:
	int			type;			//The type determine the type of the variable - number/string/etc.
	double			numData;		//The contents of this variable if it is a number.
	const char*		strData;		//The contents of this variable if it is a string.
	TinyJS_Callback*	callback;		//Callback for native functions.
	void*			userData;		//User data passed as second argument to native functions.
	//
	ST_TinyJS_Var() { /** no job **/ }		//コンストラクタを直接呼び出さず、newUndefined()等の関数を使って下さい。
	void TinyJS_Var_init(int varType);				//Initialisation of data members.
	//
	friend class ST_TinyJS;
};
//*****************************************************************************
//	ST_TinyJS_VarLink
//*****************************************************************************
class ST_TinyJS_VarLink : public gc_cleanup {
public:
	ST_TinyJS_VarLink(ST_TinyJS_Var* v);
	//
	void TinyJS_VarLink_replaceWith(ST_TinyJS_Var* v);	//Replace the Variable pointed to.
	int TinyJS_VarLink_getIntName();			//Get the name as an integer (for arrays).
	void TinyJS_VarLink_setIntName(int n);			//Set the name as an integer (for arrays).
	//
	const char*		name;
	ST_TinyJS_Var*		var;
	int			owned;
};
//*****************************************************************************
//	ST_TinyJS_Context
//*****************************************************************************
struct ST_TinyJS_Context {
	ST_TinyJS_Lex*			lex;		//Current lexer.
	GSList/*<ST_TinyJS_Var*>*/*	scopes;		//Stack of scopes when parsing.		//※元は先頭がrootで末尾が現在のスタックだったが、逆にして、先頭が現在のスタックで末尾をrootにした。その方がfindInScopes()の実装上も都合が良いし、今後クロージャを作る時にも自然に実装出来るはずだ。リストの末尾方向(rootに向けての方向)へのリンクは、クロージャを作った時点から変更される事は無いので、単純にその時点でのscopesを保持すれば良くなるので。
	GSList/*<const char*>*/*	callStack;	//Names of places called so we can show when erroring.
};
#endif//__TINYJS_H__
