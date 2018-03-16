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
//	std::string�̑�p
//*****************************************************************************
class string : public gc_cleanup {
public:
	string(const char* p = "") {
		m_p = strdup(p);
	}
	string(const char* p, int n) {
		m_p = strndup(p, n);
	}
	string(const string& s) {	//�R�s�[�R���X�g���N�^
		m_p = strdup(s.m_p);
	}
	string& operator =(const string& s) {	//������Z�q
		char* new_p = strdup(s.m_p);
		free(m_p);	//�G���A�X�Z�[�t
		m_p = new_p;
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
		free(t.m_p);	//�Y��Ȃ���!!
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
//���e����
#define TINYJS_LEX_L_FLOAT		(UCHAR_MAX+2)
#define TINYJS_LEX_L_INT		(UCHAR_MAX+3)
#define TINYJS_LEX_L_STR		(UCHAR_MAX+4)
//���Z�q
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
//�\���
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
#define TINYJS_SCRIPTVAR_NULL		(1<<0)			//It seems null is its own data type.	//��					//��
#define TINYJS_SCRIPTVAR_DOUBLE		(1<<1)			//Floating point double.		//��TINYJS_SCRIPTVAR_NUMERICMASK	//��
#define TINYJS_SCRIPTVAR_INTEGER	(1<<2)			//Integer number.			//��					//��
#define TINYJS_SCRIPTVAR_STRING		(1<<3)			//String.									//��TINYJS_SCRIPTVAR_VARTYPEMASK
#define TINYJS_SCRIPTVAR_FUNCTION	(1<<4)													//��
#define TINYJS_SCRIPTVAR_OBJECT		(1<<5)													//��
#define TINYJS_SCRIPTVAR_ARRAY		(1<<6)													//��
#define TINYJS_SCRIPTVAR_NATIVE		(1<<7)			//To specify this is a native function.
#define TINYJS_SCRIPTVAR_NUMERICMASK	(TINYJS_SCRIPTVAR_NULL|TINYJS_SCRIPTVAR_DOUBLE|TINYJS_SCRIPTVAR_INTEGER)
#define TINYJS_SCRIPTVAR_VARTYPEMASK	(TINYJS_SCRIPTVAR_NULL|TINYJS_SCRIPTVAR_DOUBLE|TINYJS_SCRIPTVAR_INTEGER|TINYJS_SCRIPTVAR_STRING|TINYJS_SCRIPTVAR_FUNCTION|TINYJS_SCRIPTVAR_OBJECT|TINYJS_SCRIPTVAR_ARRAY)
//-----------------------------------------------------------------------------
#define TINYJS_RETURN_VAR		"return"
#define TINYJS_PROTOTYPE_CLASS		"prototype"
#define TINYJS_TEMP_NAME		""
#define TINYJS_BLANK_DATA		""
//-----------------------------------------------------------------------------
class CTinyJS;
class CScriptVar;
typedef void TinyJS_Callback(CTinyJS* tinyJS, CScriptVar* v, void* userdata);
//*****************************************************************************
//	CScriptException
//*****************************************************************************
class CScriptException : public gc_cleanup {
public:
	CScriptException(const char* exceptionText);
	string text;
};
//*****************************************************************************
//	CScriptLex
//*****************************************************************************
class CScriptLex : public gc_cleanup {
public:
	CScriptLex(const char* input);
	CScriptLex(CScriptLex* owner, int startChar, int endChar);
	~CScriptLex();

	int	tk;		//The type of the token that we have.
	string	tkStr;		//Data contained in the token we have here.
	int	tokenStart;	//Position in the data at the beginning of the token we have here.
	int	tokenEnd;	//Position in the data at the last character of the token we have here.

	void reset();					//Reset this lex so we can start again.
	void match(int expected_tk);			//Lexical match wotsit.
	static string getTokenStr(int token);		//Get the string representation of the given token.

	string getSubString(int pos);			//Return a sub-string from the given position up until right now.
	CScriptLex* getSubLex(int lastPosition);	//Return a sub-lexer from the given position up until right now.

	string getLastPosition();			//Return a string representing the position in lines and columns of the tokenLastEnd.
	string getPosition(int pos);			//Return a string representing the position in lines and columns of the character pos given.

private:
	//When we go into a loop, we use getSubLex() to get a lexer for just the sub-part of the relevant string.
	//This doesn't re-allocate and copy the string, but instead copies the data pointer and sets dataOwned to false, and dataStart/dataEnd to the relevant things.
	char*	data;			//Data string to get tokens from.
	bool	dataOwned;		//Do we own this data string?
	int	dataStart, dataEnd;	//Start and end position in data string.
	int	dataPos;		//Position in data (we CAN go past the end of the string here)
	int	currCh, nextCh;
	int	tokenLastEnd;		//Position in the data at the last character of the last token.

	void getNextCh();
	void getNextToken();		//Get the text token from our text string.
};
//*****************************************************************************
//	CScriptVarLink
//*****************************************************************************
class CScriptVarLink : public gc_cleanup {
public:
	CScriptVarLink(CScriptVar* v, const char* name = TINYJS_TEMP_NAME);
	CScriptVarLink(const CScriptVarLink& link);	//Copy constructor.
	~CScriptVarLink();

	string			name;
	CScriptVar*		var;
	bool			owned;

	void replaceWith(CScriptVar* v);	//Replace the Variable pointed to.
	void replaceWith(CScriptVarLink* l);	//Replace the Variable pointed to (just dereferences).
	int getIntName();			//Get the name as an integer (for arrays).
	void setIntName(int n);			//Set the name as an integer (for arrays).
};
//*****************************************************************************
//	CScriptVar
//*****************************************************************************
//Variable class (containing a doubly-linked list of children).
class CScriptVar : public gc_cleanup {
public:
	CScriptVar();					//Create undefined.
	CScriptVar(const char* varData, int varFlags);	//User defined.
	CScriptVar(const char* str);			//Create a string.
	CScriptVar(int val);
	CScriptVar(double val);

	CScriptVar* getReturnVar();			//If this is a function, get the result value (for use by native functions).
	void setReturnVar(CScriptVar* v);		//Set the result value. Use this when setting complex return data as it avoids a deepCopy().
	CScriptVar* getParameter(const char* name);	//If this is a function, get the parameter with the given name (for use by native functions).

	CScriptVarLink* findChild(const char* childName);						//Tries to find a child with the given name, may return NULL.
	CScriptVarLink* findChildOrCreate(const char* name, int varFlags = TINYJS_SCRIPTVAR_UNDEFINED);	//Tries to find a child with the given name, or will create it with the given flags.
	CScriptVarLink* findChildOrCreateByPath(const char* path);					//Tries to find a child with the given path (separated by dots).
	CScriptVarLink* addChild(const char* name, CScriptVar* v = NULL);
	CScriptVarLink* addChildNoDup(const char* name, CScriptVar* v = NULL);				//Add a child overwriting any with the same name.
	void removeChild(CScriptVar* v);
	void removeLink(CScriptVarLink* link);		//Remove a specific link (this is faster than finding via a child).
	void removeAllChildren();
	CScriptVar* getArrayIndex(int i);		//The the value at an array index.
	void setArrayIndex(int i, CScriptVar* v);	//Set the value at an array index.
	int getArrayLength();				//If this is an array, return the number of items in it (else 0).
	int getChildCount();				//Get the number of children.

	bool getBool();
	int getInt();
	double getDouble();
	string getString();
	string getParsableString();			//Get data as a parsable javascript string.
	void setInt(int val);
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
	bool isBasic() { return firstChild == NULL; }			//Is this *not* an array/object/etc.

	CScriptVar* mathsOp(CScriptVar* v, int op);			//Do a maths op with another script variable.
	void copyValue(CScriptVar* v);					//Copy the value from the value given.
	CScriptVar* deepCopy();						//Deep copy this node and return the result.

	void trace(const char* indent = "", const char* name = "");	//Dump out the contents of this using trace.
	string getFlagsAsString();					//For debugging - just dump a string version of the flags.
	string getJSON(const char* linePrefix = "");			//Write out all the JS code needed to recreate this script variable to the stream (as JSON).
	void setCallback(TinyJS_Callback* callback, void* userdata);	//Set the callback for native functions.

	GSList/*<CScriptVarLink*>*/*	firstChild;

	//For memory management/garbage collection.
	CScriptVar* ref();				//Add reference to this variable.
	void unref();					//Remove a reference, and delete this variable if required.
//{{�폜:���炩�ɕs�v�Ȃ̂ō폜�B�O������Q�ƃJ�E���g��������̂͊Q�ɂ����Ȃ�Ȃ��B
//	int getRefs();					//Get the number of references to this script variable.
//}}�폜:���炩�ɕs�v�Ȃ̂ō폜�B�O������Q�ƃJ�E���g��������̂͊Q�ɂ����Ȃ�Ȃ��B
private:
	int				refs;		//The number of references held to this - used for garbage collection.

	int				flags;		//The flags determine the type of the variable - int/double/string/etc.
	double				doubleData;	//The contents of this variable if it is a double.
	int				intData;	//The contents of this variable if it is an int.
	string				data;		//The contents of this variable if it is a string.
	TinyJS_Callback*		callback;	//Callback for native functions.
	void*				userdata;	//User data passed as second argument to native functions.

	~CScriptVar();					//�����I�ɍ폜�o���Ȃ��悤�ɁA�f�X�g���N�^��private�ɂ����Bunref()�ɂ���Ă̂ݍ폜�o����B

	void init();					//Initialisation of data members.
	void copySimpleData(CScriptVar* v);		//Copy the basic data and flags from the variable given, with no children. Should be used internally only - by copyValue and deepCopy.

	friend class CTinyJS;
};
//*****************************************************************************
//	CTinyJS
//*****************************************************************************
class CTinyJS : public gc_cleanup {
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
	//��void scRandInt(CScriptVar* v, void* userdata) { ... }
	//��tinyJS->addNative("function randInt(min, max)", scRandInt, NULL);
	//or
	//��void scSubstring(CScriptVar* v, void* userdata) { ... }
	//��tinyJS->addNative("function String.substring(lo, hi)", scSubstring, NULL);
	void addNative(const char* funcDesc, TinyJS_Callback* callback, void* userdata);

	//Get the given variable specified by a path (var1.var2.etc), or return NULL.
	CScriptVar* getScriptVariable(const char* path);
//{{�폜�B���̊֐���NULL��Ԃ���悤�ɂ��邽�߂ɑ��̊֐��ɐF�X�����������Ă���B���̊֐���p�~���Ė����I��getScriptVariable()���g���悤�ɂ���Ύ��R�ɂȂ�B�����������֐��̓A�v���P�[�V�����p�ɗp�ӂ��ꂽ���̂̂悤�ŁA���C�u�������̎����ɂ����Ă͎g�p����Ă��Ȃ������B
//	//Get the value of the given variable, or return NULL.
//	const string* getVariable(const char* path);
//}}�폜�B���̊֐���NULL��Ԃ���悤�ɂ��邽�߂ɑ��̊֐��ɐF�X�����������Ă���B���̊֐���p�~���Ė����I��getScriptVariable()���g���悤�ɂ���Ύ��R�ɂȂ�B�����������֐��̓A�v���P�[�V�����p�ɗp�ӂ��ꂽ���̂̂悤�ŁA���C�u�������̎����ɂ����Ă͎g�p����Ă��Ȃ������B
	//Set the value of the given variable, return trur if it exists and gets set.
	bool setVariable(const char* path, const char* varData);

	//Send all variables to stdout.
	void trace(const char* indent = "");

	CScriptVar*			root;		//Root of symbol table.			//���v����:�O���[�o���I�u�W�F�N�g�̎��ł��BWeb�u���E�U�ł̎����̏ꍇ�uwindow�v���ANode.js�̏ꍇ�́uglobal�v���O���[�o���I�u�W�F�N�g�ƂȂ�܂��Broot�Ƃ����ϐ����͂�߂āAwindow,����,global�ɕς�������ǂ��̂ł́H
private:
	CScriptLex*			lex;		//Current lexer.
	GSList/*<CScriptVar*>*/*	scopes;		//Stack of scopes when parsing.		//�����͐擪��root�Ŗ��������݂̃X�^�b�N���������A�t�ɂ��āA�擪�����݂̃X�^�b�N�Ŗ�����root�ɂ����B���̕���findInScopes()�̎�������s�����ǂ����A����N���[�W������鎞�ɂ����R�Ɏ����o����͂����B���X�g�̖�������(root�Ɍ����Ă̕���)�ւ̃����N�́A�N���[�W������������_����ύX����鎖�͖����̂ŁA�P���ɂ��̎��_�ł�scopes��ێ�����Ηǂ��Ȃ�̂ŁB
#ifdef  TINYJS_CALL_STACK
	GSList/*<char*>*/*		call_stack;	//Names of places called so we can show when erroring.
#endif//TINYJS_CALL_STACK

	CScriptVar*			stringClass;	//Built in string class.
	CScriptVar*			objectClass;	//Built in object class.
	CScriptVar*			arrayClass;	//Built in array class.

	//Parsing - in order of precedence.
	CScriptVarLink* functionCall(bool* pExec, CScriptVarLink* function, CScriptVar* parent);
	CScriptVarLink* factor(bool* pExec);
	CScriptVarLink* unary(bool* pExec);
	CScriptVarLink* term(bool* pExec);
	CScriptVarLink* expression(bool* pExec);
	CScriptVarLink* shift(bool* pExec);
	CScriptVarLink* condition(bool* pExec);
	CScriptVarLink* logic(bool* pExec);
	CScriptVarLink* ternary(bool* pExec);
	CScriptVarLink* base(bool* pExec);
	void block(bool* pExec);
	void statement(bool* pExec);
	//Parsing utility functions.
	CScriptVarLink* parseFunctionDefinition();
	void parseFunctionArguments(CScriptVar* funcVar);

	CScriptVarLink* findInScopes(const char* name);				//Finds a child, looking recursively up the scopes.
	CScriptVarLink* findInParentClasses(CScriptVar* obj, const char* name);	//Look up in any parent classes of the given object.

	string stack_trace(CScriptException* e);	//���\�[�X�ɂ͂��̊֐��͗L��܂��񂪁A���ʏ������܂Ƃ߂邽�߂ɒǉ����܂����B
};
#endif//__TINYJS_H__
