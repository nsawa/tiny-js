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
//	TinyJS(�ڐA��)
//
//	Ported By Naoyuki Sawa <nsawa@piece-me.org>
//	Copyright (C) 2018 Piece Lab.
//
//	* Fri Mar 30 23:59:59 JST 2018 Naoyuki Sawa
//	- 1st �����[�X�B
//	- �uTinyJS�v(https://github.com/gfwilliams/tiny-js)���AP/ECE�œ��삷��悤�ɈڐA�������̂ł��B
//	  �I���W�i���ł�TinyJS��C++�ŏ�����Ă��܂����AP/ECE�J�����ɂ�GCC 2.7.2������C�R���p�C�����������̂ŁAC++����C����ɈڐA���܂����B
//	  C++��C����͂����Ԃ�Ⴄ���߁A�R�[�h�͂قڑS�ď��������Ă��܂��܂������A�A���S���Y���̓I���W�i���ł�TinyJS�Ƃ������������܂܂ł��B
//	  ���A���C���^�[�Q�b�g��P/ECE�ł����A(���Ȃ��Ƃ������_�ł�)Visual Studio 2017�ƁAVisual C++ 6.0�ł��r���h�o���Ă��܂��B
//	- �I���W�i���ł�TinyJS�͖����I�ȃ������Ǘ����s���Ă��܂����A�ڐA�łł̓K�[�x�[�W�R���N�^�[�̎g�p��O��Ƃ��ă������Ǘ����Ȃ��܂����B
//	  P/ECE�̏ꍇ��clipbmgc.c,����,clipgc.c���W���[�����CWindows�̏ꍇ��Boehm GC���g�p���ĉ������B
//	- �I���W�i���ł�TinyJS���A/clip/keep/tiny-js-master.7z �ɕۑ����Ă����܂����B
//	  (Latest commit 56a0c6d on Mar 24 2015�B2018/03/30���_�̍ŐV���r�W�����ł��B)
//
#ifndef __TINYJS_H__
#define __TINYJS_H__
#ifdef  __cplusplus
extern "C" {
#endif//__cplusplus
//*****************************************************************************
//	
//*****************************************************************************
#define TinyJS_Exception			MAKEFOURCC('T','i','J','S')
//-----------------------------------------------------------------------------
//ST_TinyJS_Lex.tk
#define TINYJS_LEX_EOF				0
//ID
#define TINYJS_LEX_ID				(UCHAR_MAX+1)
//���e����
#define TINYJS_LEX_L_NUM			(UCHAR_MAX+2)
#define TINYJS_LEX_L_STR			(UCHAR_MAX+3)
//���Z�q
#define TINYJS_LEX_O_ANDAND			(UCHAR_MAX+5)			//"&&"
#define TINYJS_LEX_O_ANDASSIGN			(UCHAR_MAX+6)			//"&="		//�������͂Ō��o����Ă��邪�A�\����͂ŏ�������Ă��Ȃ��B
#define TINYJS_LEX_O_EQUAL			(UCHAR_MAX+7)			//"=="
#define TINYJS_LEX_O_GEQUAL			(UCHAR_MAX+8)			//">="
#define TINYJS_LEX_O_LEQUAL			(UCHAR_MAX+9)			//"<="
#define TINYJS_LEX_O_LSHIFT			(UCHAR_MAX+10)			//"<<"
#define TINYJS_LEX_O_LSHIFTASSIGN		(UCHAR_MAX+11)			//"<<="		//�������͂Ō��o����Ă��邪�A�\����͂ŏ�������Ă��Ȃ��B
#define TINYJS_LEX_O_MINUSASSIGN		(UCHAR_MAX+12)			//"-="
#define TINYJS_LEX_O_MINUSMINUS			(UCHAR_MAX+13)			//"--"
#define TINYJS_LEX_O_NEQUAL			(UCHAR_MAX+14)			//"!="
#define TINYJS_LEX_O_NTYPEEQUAL			(UCHAR_MAX+15)			//"!=="
#define TINYJS_LEX_O_OREQUAL			(UCHAR_MAX+16)			//"|="		//�������͂Ō��o����Ă��邪�A�\����͂ŏ�������Ă��Ȃ��B
#define TINYJS_LEX_O_OROR			(UCHAR_MAX+17)			//"||"
#define TINYJS_LEX_O_PLUSASSIGN			(UCHAR_MAX+18)			//"+="
#define TINYJS_LEX_O_PLUSPLUS			(UCHAR_MAX+19)			//"++"
#define TINYJS_LEX_O_RSHIFT			(UCHAR_MAX+20)			//">>"
#define TINYJS_LEX_O_RSHIFTASSIGN		(UCHAR_MAX+21)			//">>="		//�������͂Ō��o����Ă��邪�A�\����͂ŏ�������Ă��Ȃ��B
#define TINYJS_LEX_O_RSHIFTUNSIGNED		(UCHAR_MAX+22)			//">>"
#define TINYJS_LEX_O_TYPEEQUAL			(UCHAR_MAX+23)			//"==="
#define TINYJS_LEX_O_XORASSIGN			(UCHAR_MAX+24)			//"^="		//�������͂Ō��o����Ă��邪�A�\����͂ŏ�������Ă��Ȃ��B
//�\���
#define TINYJS_LEX_R_BREAK			(UCHAR_MAX+25)			//"break"	//�������͂Ō��o����Ă��邪�A�\����͂ŏ�������Ă��Ȃ��B
#define TINYJS_LEX_R_CONTINUE			(UCHAR_MAX+26)			//"continue"	//�������͂Ō��o����Ă��邪�A�\����͂ŏ�������Ă��Ȃ��B
#define TINYJS_LEX_R_DO				(UCHAR_MAX+27)			//"do"		//�������͂Ō��o����Ă��邪�A�\����͂ŏ�������Ă��Ȃ��B
#define TINYJS_LEX_R_ELSE			(UCHAR_MAX+28)			//"else"
#define TINYJS_LEX_R_FALSE			(UCHAR_MAX+29)			//"false"
#define TINYJS_LEX_R_FOR			(UCHAR_MAX+30)			//"for"
#define TINYJS_LEX_R_FUNCTION			(UCHAR_MAX+31)			//"function"
#define TINYJS_LEX_R_IF				(UCHAR_MAX+32)			//"if"
#define TINYJS_LEX_R_NEW			(UCHAR_MAX+33)			//"new"
#define TINYJS_LEX_R_NULL			(UCHAR_MAX+34)			//"null"
#define TINYJS_LEX_R_RETURN			(UCHAR_MAX+35)			//"return"
#define TINYJS_LEX_R_TRUE			(UCHAR_MAX+36)			//"true"
#define TINYJS_LEX_R_UNDEFINED			(UCHAR_MAX+37)			//"undefined"
#define TINYJS_LEX_R_VAR			(UCHAR_MAX+38)			//"var"
#define TINYJS_LEX_R_WHILE			(UCHAR_MAX+39)			//"while"
//-----------------------------------------------------------------------------
//ST_TinyJS_Var.type
#define TINYJS_VAR_UNDEFINED			0
#define TINYJS_VAR_NULL				1				//It seems null is its own data type.
#define TINYJS_VAR_NUMBER			2				//Number.
#define TINYJS_VAR_STRING			3				//String.
#define TINYJS_VAR_FUNCTION			4
#define TINYJS_VAR_OBJECT			5
#define TINYJS_VAR_ARRAY			6
#define TINYJS_VAR_NATIVE			7				//To specify this is a native function.
//-----------------------------------------------------------------------------
typedef struct _ST_TinyJS			ST_TinyJS;
typedef struct _ST_TinyJS_Lex			ST_TinyJS_Lex;
typedef struct _ST_TinyJS_Var			ST_TinyJS_Var;
typedef struct _ST_TinyJS_VarLink		ST_TinyJS_VarLink;
typedef struct _ST_TinyJS_Context		ST_TinyJS_Context;
typedef void TinyJS_Callback(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData);
//*****************************************************************************
//	ST_TinyJS
//*****************************************************************************
/*typedef*/ struct _ST_TinyJS {
//public:
	ST_TinyJS_Var*				root;				//Root of symbol table.			//���v����:�O���[�o���I�u�W�F�N�g�̎��ł��BWeb�u���E�U�ł̎����̏ꍇ�uwindow�v���ANode.js�̏ꍇ�́uglobal�v���O���[�o���I�u�W�F�N�g�ƂȂ�܂��Broot�Ƃ����ϐ����͂�߂āAwindow,����,global�ɕς�������ǂ��̂ł́H
//private:
	ST_TinyJS_Lex*				lex;				//Current lexer.
	struct _GSList/*<ST_TinyJS_Var*>*/*	scopes;				//Stack of scopes when parsing.		//�����͐擪��root�Ŗ��������݂̃X�^�b�N���������A�t�ɂ��āA�擪�����݂̃X�^�b�N�Ŗ�����root�ɂ����B���̕���findInScopes()�̎�������s�����ǂ����A����N���[�W������鎞�ɂ����R�Ɏ����o����͂����B���X�g�̖�������(root�Ɍ����Ă̕���)�ւ̃����N�́A�N���[�W������������_����ύX����鎖�͖����̂ŁA�P���ɂ��̎��_�ł�scopes��ێ�����Ηǂ��Ȃ�̂ŁB
	struct _GSList/*<const char*>*/*	callStack;			//Names of places called so we can show when erroring.
	ST_TinyJS_Var*				stringClass;			//Built in string class.
	ST_TinyJS_Var*				objectClass;			//Built in object class.
	ST_TinyJS_Var*				arrayClass;			//Built in array class.
} /*ST_TinyJS*/;
//public:
/*static*/ ST_TinyJS* TinyJS_new();
void TinyJS_execute(ST_TinyJS* _this, const char* code);
ST_TinyJS_Var* TinyJS_evaluate(ST_TinyJS* _this, const char* code);
void TinyJS_addNative(ST_TinyJS* _this, const char* funcDesc, TinyJS_Callback* callback, void* userData);
void TinyJS_trace(ST_TinyJS* _this, const char* indent);
//private:
//Parsing - in order of precedence.
ST_TinyJS_VarLink* TinyJS_functionCall(ST_TinyJS* _this, int* pExec, ST_TinyJS_VarLink* func, ST_TinyJS_Var* obj/*NULL��*/);
ST_TinyJS_VarLink* TinyJS_factor(ST_TinyJS* _this, int* pExec);
ST_TinyJS_VarLink* TinyJS_unary(ST_TinyJS* _this, int* pExec);
ST_TinyJS_VarLink* TinyJS_term(ST_TinyJS* _this, int* pExec);
ST_TinyJS_VarLink* TinyJS_expression(ST_TinyJS* _this, int* pExec);
ST_TinyJS_VarLink* TinyJS_shift(ST_TinyJS* _this, int* pExec);
ST_TinyJS_VarLink* TinyJS_condition(ST_TinyJS* _this, int* pExec);
ST_TinyJS_VarLink* TinyJS_logic(ST_TinyJS* _this, int* pExec);
ST_TinyJS_VarLink* TinyJS_ternary(ST_TinyJS* _this, int* pExec);
ST_TinyJS_VarLink* TinyJS_base(ST_TinyJS* _this, int* pExec);
void TinyJS_block(ST_TinyJS* _this, int* pExec);
void TinyJS_statement(ST_TinyJS* _this, int* pExec);
ST_TinyJS_VarLink* TinyJS_parseFunctionDefinition(ST_TinyJS* _this);
void TinyJS_parseFunctionArguments(ST_TinyJS* _this, ST_TinyJS_Var* funcVar);
ST_TinyJS_VarLink* TinyJS_findInScopes(ST_TinyJS* _this, const char* name);
ST_TinyJS_VarLink* TinyJS_findInPrototypeClasses(ST_TinyJS* _this, ST_TinyJS_Var* v, const char* name);
ST_TinyJS_Context* TinyJS_saveContext(ST_TinyJS* _this);
void TinyJS_restoreContext(ST_TinyJS* _this, ST_TinyJS_Context* pSavedContext);
const char* TinyJS_stackTrace(ST_TinyJS* _this, const char* errMsg);
//*****************************************************************************
//	ST_TinyJS_Lex
//*****************************************************************************
/*typedef*/ struct _ST_TinyJS_Lex {
//public:
	int					tk;				//�g�[�N���̎�ށB(TINYJS_LEX_*,����,���̑��̈ꕶ�����Z�q,����,�s���ȕ���)
	const char*				tkStr;				//�g�[�N���̕�����B
	int					tokenStart;			//�Ō�ɓǂ񂾃g�[�N��(=tk,tkStr)�́A�ŏ��̕����̈ʒu�B
	int					tokenEnd;			//�Ō�ɓǂ񂾃g�[�N��(=tk,tkStr)�́A�Ō�̕����̎��̈ʒu�B
//private:
	//When we go into a loop, we use getSubLex() to get a lexer for just the sub-part of the relevant string.
	//This doesn't re-allocate and copy the string, but instead copies the data pointer and sets dataOwned to false, and dataStart/dataEnd to the relevant things.
	const char*				data;				//Data string to get tokens from.
	int					dataStart, dataEnd;		//Start and end position in data string.
	int					dataPos;			//Position in data (we CAN go past the end of the string here).
	int					currCh, nextCh;			//���݂̕����C���̕����B
	int					tokenLastEnd;			//��O�ɓǂ񂾃g�[�N���́A�Ō�̕����̎��̈ʒu�B
} /*ST_TinyJS_Lex*/;
//public:
/*static*/ ST_TinyJS_Lex* TinyJS_Lex_new(const char* input, int startChar, int endChar);
ST_TinyJS_Lex* TinyJS_Lex_reset(ST_TinyJS_Lex* _this);				//Reset this lex so we can start again.
void TinyJS_Lex_match(ST_TinyJS_Lex* _this, int tkExpected);			//Lexical match wotsit.
/*static*/ const char* TinyJS_Lex_getTokenStr(int token);			//Get the string representation of the given token.
const char* TinyJS_Lex_getSubString(ST_TinyJS_Lex* _this, int pos);		//Return a sub-string from the given position up until right now.
ST_TinyJS_Lex* TinyJS_Lex_getSubLex(ST_TinyJS_Lex* _this, int lastPosition);	//Return a sub-lexer from the given position up until right now.
const char* TinyJS_Lex_getLastPosition(ST_TinyJS_Lex* _this);			//Return a string representing the position in lines and columns of the tokenLastEnd - 1.
const char* TinyJS_Lex_getPosition(ST_TinyJS_Lex* _this, int pos);		//Return a string representing the position in lines and columns of the character pos given.
//private:
void TinyJS_Lex_getNextCh(ST_TinyJS_Lex* _this);
void TinyJS_Lex_getNextToken(ST_TinyJS_Lex* _this);				//Get the text token from our text string.
//*****************************************************************************
//	ST_TinyJS_Var
//*****************************************************************************
//Variable class (containing a doubly-linked list of children).
/*typedef*/ struct _ST_TinyJS_Var {
//public:
	struct _GSList/*<ST_TinyJS_VarLink*>*/*	firstChild;
//private:
	int					type;				//The type determine the type of the variable - number/string/etc.
	double					numData;			//The contents of this variable if it is a number.
	const char*				strData;			//The contents of this variable if it is a string.
	TinyJS_Callback*			callback;			//Callback for native functions.
	void*					userData;			//User data passed as second argument to native functions.
} /*ST_TinyJS_Var*/;
//public:
/*static*/ ST_TinyJS_Var* TinyJS_Var_newUndefined();
/*static*/ ST_TinyJS_Var* TinyJS_Var_newNull();
/*static*/ ST_TinyJS_Var* TinyJS_Var_newNumber(double val);
/*static*/ ST_TinyJS_Var* TinyJS_Var_newString(const char* str);
/*static*/ ST_TinyJS_Var* TinyJS_Var_newFunction();
/*static*/ ST_TinyJS_Var* TinyJS_Var_newObject();
/*static*/ ST_TinyJS_Var* TinyJS_Var_newArray();
/*static*/ ST_TinyJS_Var* TinyJS_Var_newNative(TinyJS_Callback* callback, void* userData);
void TinyJS_Var_setUndefined(ST_TinyJS_Var* _this);
void TinyJS_Var_setNull(ST_TinyJS_Var* _this);
void TinyJS_Var_setNumber(ST_TinyJS_Var* _this, double val);
void TinyJS_Var_setString(ST_TinyJS_Var* _this, const char* str);
void TinyJS_Var_setFunction(ST_TinyJS_Var* _this);
void TinyJS_Var_setObject(ST_TinyJS_Var* _this);
void TinyJS_Var_setArray(ST_TinyJS_Var* _this);
void TinyJS_Var_setNative(ST_TinyJS_Var* _this, TinyJS_Callback* callback, void* userData);
int TinyJS_Var_isUndefined(ST_TinyJS_Var* _this);
int TinyJS_Var_isNull(ST_TinyJS_Var* _this);
int TinyJS_Var_isNumber(ST_TinyJS_Var* _this);
int TinyJS_Var_isString(ST_TinyJS_Var* _this);
int TinyJS_Var_isFunction(ST_TinyJS_Var* _this);
int TinyJS_Var_isObject(ST_TinyJS_Var* _this);
int TinyJS_Var_isArray(ST_TinyJS_Var* _this);
int TinyJS_Var_isNative(ST_TinyJS_Var* _this);
int TinyJS_Var_isPrimitive(ST_TinyJS_Var* _this);
ST_TinyJS_Var* TinyJS_Var_getReturnVar(ST_TinyJS_Var* _this);					//If this is a function, get the result value (for use by native functions).
void TinyJS_Var_setReturnVar(ST_TinyJS_Var* _this, ST_TinyJS_Var* v);				//Set the result value. Use this when setting complex return data as it avoids a deepCopy().
ST_TinyJS_Var* TinyJS_Var_getParameter(ST_TinyJS_Var* _this, const char* name);			//If this is a function, get the parameter with the given name (for use by native functions).
ST_TinyJS_VarLink* TinyJS_Var_findChild(ST_TinyJS_Var* _this, const char* childName);		//Tries to find a child with the given name, may return NULL.
ST_TinyJS_VarLink* TinyJS_Var_findChildOrCreate(ST_TinyJS_Var* _this, const char* name);	//Tries to find a child with the given name, or will create it with the given type.
ST_TinyJS_VarLink* TinyJS_Var_addChild(ST_TinyJS_Var* _this, const char* name, ST_TinyJS_Var* v);
void TinyJS_Var_removeLink(ST_TinyJS_Var* _this, ST_TinyJS_VarLink* link);			//Remove a specific link (this is faster than finding via a child).
ST_TinyJS_Var* TinyJS_Var_getArrayIndex(ST_TinyJS_Var* _this, int i);				//The the value at an array index.
void TinyJS_Var_setArrayIndex(ST_TinyJS_Var* _this, int i, ST_TinyJS_Var* v);			//Set the value at an array index.
int TinyJS_Var_getArrayLength(ST_TinyJS_Var* _this);						//If this is an array, return the number of items in it (else 0).
int TinyJS_Var_getBoolean(ST_TinyJS_Var* _this);
double TinyJS_Var_getNumber(ST_TinyJS_Var* _this);
const char* TinyJS_Var_getString(ST_TinyJS_Var* _this);
const char* TinyJS_Var_getParsableString(ST_TinyJS_Var* _this);					//Get data as a parsable javascript string.
int TinyJS_Var_equals(ST_TinyJS_Var* _this, ST_TinyJS_Var* v);
ST_TinyJS_Var* TinyJS_Var_mathsOp(ST_TinyJS_Var* _this, ST_TinyJS_Var* v, int op);		//Do a maths op with another script variable.
ST_TinyJS_Var* TinyJS_Var_deepCopy(ST_TinyJS_Var* _this);					//Deep copy this node and return the result.
void TinyJS_Var_trace(ST_TinyJS_Var* _this, const char* indent, const char* name);		//Dump out the contents of this using trace.
const char* TinyJS_Var_getTypeAsString(ST_TinyJS_Var* _this);					//For debugging - just dump a string version of the type.
const char* TinyJS_Var_getJSON(ST_TinyJS_Var* _this, const char* linePrefix);			//Write out all the JS code needed to recreate this script variable to the stream (as JSON).
//private:
void TinyJS_Var_init(ST_TinyJS_Var* _this, int varType);					//Initialisation of data members.
//*****************************************************************************
//	ST_TinyJS_VarLink
//*****************************************************************************
/*typedef*/ struct _ST_TinyJS_VarLink {
//public:
	const char*				name;
	ST_TinyJS_Var*				var;
	int					owned;
} /*ST_TinyJS_VarLink*/;
//public:
/*static*/ ST_TinyJS_VarLink* TinyJS_VarLink_new(ST_TinyJS_Var* v);
void TinyJS_VarLink_replaceWith(ST_TinyJS_VarLink* _this, ST_TinyJS_Var* v);	//Replace the Variable pointed to.
int TinyJS_VarLink_getIntName(ST_TinyJS_VarLink* _this);			//Get the name as an integer (for arrays).
void TinyJS_VarLink_setIntName(ST_TinyJS_VarLink* _this, int n);		//Set the name as an integer (for arrays).
//*****************************************************************************
//	ST_TinyJS_Context
//*****************************************************************************
/*typedef*/ struct _ST_TinyJS_Context {
//public:
	ST_TinyJS_Lex*				lex;				//Current lexer.
	struct _GSList/*<ST_TinyJS_Var*>*/*	scopes;				//Stack of scopes when parsing.		//�����͐擪��root�Ŗ��������݂̃X�^�b�N���������A�t�ɂ��āA�擪�����݂̃X�^�b�N�Ŗ�����root�ɂ����B���̕���findInScopes()�̎�������s�����ǂ����A����N���[�W������鎞�ɂ����R�Ɏ����o����͂����B���X�g�̖�������(root�Ɍ����Ă̕���)�ւ̃����N�́A�N���[�W������������_����ύX����鎖�͖����̂ŁA�P���ɂ��̎��_�ł�scopes��ێ�����Ηǂ��Ȃ�̂ŁB
	struct _GSList/*<const char*>*/*	callStack;			//Names of places called so we can show when erroring.
} /*ST_TinyJS_Context*/;
#ifdef  __cplusplus
}//extern "C"
#endif//__cplusplus
#endif//__TINYJS_H__
