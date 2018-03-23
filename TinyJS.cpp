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
//*****************************************************************************
//	Utils
//*****************************************************************************
static int noexec;
//-----------------------------------------------------------------------------
//Convert the given string into a quoted string suitable for javascript.
static const char* getJSString(const char* str) {
	GString* out = g_string_new(NULL);
	int c;
	while((c = *str++) != '\0') {
		switch(c) {
		case '\t': g_string_append(out, "\\t");  break;	//┐
		case '\n': g_string_append(out, "\\n");  break;	//├頻繁に使う制御文字のエスケープシーケンスだけ明示的に変換し、頻繁に使わない制御文字はその他の印字不可文字と共通で8進数形式で出力する事にした。
		case '\r': g_string_append(out, "\\r");  break;	//┘
		case '"':  g_string_append(out, "\\\""); break;
		case '\\': g_string_append(out, "\\\\"); break;
		default:
			{
				if(isprint(c)) {
					g_string_append_c(out, c);
				} else {
					g_string_append_printf(out, "\\%03o", (unsigned char)c);	//16進数形式は桁数制限が無く誤って後続の文字と繋がって解釈される恐れが有る。8進数形式は最大でも3桁までしか解釈されず左記の問題は生じない。8進数形式の方が安全なので8進数形式で出力する事にした。
				}
			}
			break;
		}
	}
	g_string_prepend(out, "\"");
	g_string_append( out, "\"");
	return out->str;
}
//*****************************************************************************
//	ST_TinyJS_Exception
//*****************************************************************************
ST_TinyJS_Exception::ST_TinyJS_Exception(const char* errMsg) {
	msg = errMsg;
}
//*****************************************************************************
//	ST_TinyJS_Lex
//*****************************************************************************
ST_TinyJS_Lex::ST_TinyJS_Lex(const char* input, int startChar, int endChar) {
	data      = input;
	dataStart = startChar;
	dataEnd   = endChar;
	reset();
}
//-----------------------------------------------------------------------------
//Reset this lex so we can start again.
ST_TinyJS_Lex* ST_TinyJS_Lex::reset() {
	dataPos      = dataStart;
	tokenStart   = 0;
	tokenEnd     = 0;
	tokenLastEnd = 0;
	tk           = 0;
	tkStr        = "";
	getNextCh();
	getNextCh();
	//
	//	～■■□～
	//	　↑↑↑
	//	　││└         dataPos
	//	　│└─ nextCh: dataPos - 1
	//	　└── currCh: dataPos - 2 = dataStart
	//
	getNextToken();
	return this;	//利便性のためにthisを返す事にした。
}
//-----------------------------------------------------------------------------
//Lexical match wotsit.
void ST_TinyJS_Lex::match(int tkExpected) {
	if(tk != tkExpected) {
		GString* errorString = g_string_new(NULL);
		g_string_append(errorString, "Got ");
		g_string_append(errorString, getTokenStr(tk));
		g_string_append(errorString, " expected ");
		g_string_append(errorString, getTokenStr(tkExpected));
		g_string_append(errorString, " at ");
		g_string_append(errorString, getPosition(tokenStart));
		throw new ST_TinyJS_Exception(errorString->str);
	}
	getNextToken();
}
//-----------------------------------------------------------------------------
//Get the string representation of the given token.
const char* ST_TinyJS_Lex::getTokenStr(int token) {
	switch(token) {
	case TINYJS_LEX_EOF:              return "EOF";
	//ID
	case TINYJS_LEX_ID:               return "ID";
	//リテラル
	case TINYJS_LEX_L_NUM:            return "NUM";
	case TINYJS_LEX_L_STR:            return "STR";
	//演算子
	case TINYJS_LEX_O_ANDAND:         return "&&";
	case TINYJS_LEX_O_ANDASSIGN:      return "&=";
	case TINYJS_LEX_O_EQUAL:          return "==";
	case TINYJS_LEX_O_GEQUAL:         return ">=";
	case TINYJS_LEX_O_LEQUAL:         return "<=";
	case TINYJS_LEX_O_LSHIFT:         return "<<";
	case TINYJS_LEX_O_LSHIFTASSIGN:   return "<<=";
	case TINYJS_LEX_O_MINUSASSIGN:    return "-=";
	case TINYJS_LEX_O_MINUSMINUS:     return "--";
	case TINYJS_LEX_O_NEQUAL:         return "!=";
	case TINYJS_LEX_O_NTYPEEQUAL:     return "!==";
	case TINYJS_LEX_O_OREQUAL:        return "|=";
	case TINYJS_LEX_O_OROR:           return "||";
	case TINYJS_LEX_O_PLUSASSIGN:     return "+=";
	case TINYJS_LEX_O_PLUSPLUS:       return "++";
	case TINYJS_LEX_O_RSHIFT:         return ">>";
	case TINYJS_LEX_O_RSHIFTASSIGN:   return ">>=";
	case TINYJS_LEX_O_RSHIFTUNSIGNED: return ">>>";
	case TINYJS_LEX_O_TYPEEQUAL:      return "===";
	case TINYJS_LEX_O_XORASSIGN:      return "^=";
	//予約語
	case TINYJS_LEX_R_BREAK:          return "break";
	case TINYJS_LEX_R_CONTINUE:       return "continue";
	case TINYJS_LEX_R_DO:             return "do";
	case TINYJS_LEX_R_ELSE:           return "else";
	case TINYJS_LEX_R_FALSE:          return "false";
	case TINYJS_LEX_R_FOR:            return "for";
	case TINYJS_LEX_R_FUNCTION:       return "function";
	case TINYJS_LEX_R_IF:             return "if";
	case TINYJS_LEX_R_NEW:            return "new";
	case TINYJS_LEX_R_NULL:           return "null";
	case TINYJS_LEX_R_RETURN:         return "return";
	case TINYJS_LEX_R_TRUE:           return "true";
	case TINYJS_LEX_R_UNDEFINED:      return "undefined";
	case TINYJS_LEX_R_VAR:            return "var";
	case TINYJS_LEX_R_WHILE:          return "while";
	}
	return strdup_printf((isprint(token) ? "'%c'" : "?[%d]"), token);
}
//-----------------------------------------------------------------------------
void ST_TinyJS_Lex::getNextCh() {
	currCh = nextCh;
	if(dataPos < dataEnd) {
		nextCh = data[dataPos];
//×		dataPos++;	//←┐
	} else {		//　│
		nextCh = '\0';	//　├もし入力範囲の終端を超えてもdataPosは無条件に進めなければいけない。reset(),及び,getNextToken()の処理がgetNextCh()を呼び出した回数分dataPosが進んでいる事を前提としているから。
	}			//　│
/*○*/	dataPos++;		//←┘
}
//-----------------------------------------------------------------------------
//Get the text token from our text string.
void ST_TinyJS_Lex::getNextToken() {
	for(;;) {
		//Skip whitespaces.
		while(currCh) {
			if(!isspace(currCh)) {
				break;
			}
			getNextCh();
		}
		//Newline comments.
		if((currCh == '/') && (nextCh == '/')) {
			getNextCh();
			getNextCh();
			while(currCh) {
				if(currCh == '\n') {
					getNextCh();
					break;
				}
				getNextCh();
			}
		//Block comments.
		} else if((currCh == '/') && (nextCh == '*')) {
			getNextCh();
			getNextCh();
			while(currCh) {
				if((currCh == '*') && (nextCh == '/')) {
					getNextCh();
					getNextCh();
					break;
				}
				getNextCh();
			}
		} else {
			break;
		}
	}
	//Record beginning of this token.
	tokenStart = dataPos - 2;
	//
	//	～■■□～
	//	　↑↑↑
	//	　││└         dataPos
	//	　│└─ nextCh: dataPos - 1
	//	　└── currCh: dataPos - 2 = tokenStart
	//
	//Tokens.
//不要	tk    = TINYJS_LEX_EOF;
	tkStr = "";
	GString* buf = g_string_new(NULL);
	//予約語,又は,ID
	if(iscsymf(currCh)) {
		while(iscsym(currCh)) {
			g_string_append_c(buf, currCh);
			getNextCh();
		}
		     if(!strcmp(buf->str, "break"))     { tk = TINYJS_LEX_R_BREAK; }
		else if(!strcmp(buf->str, "continue"))  { tk = TINYJS_LEX_R_CONTINUE; }
		else if(!strcmp(buf->str, "do"))        { tk = TINYJS_LEX_R_DO; }
		else if(!strcmp(buf->str, "else"))      { tk = TINYJS_LEX_R_ELSE; }
		else if(!strcmp(buf->str, "false"))     { tk = TINYJS_LEX_R_FALSE; }
		else if(!strcmp(buf->str, "for"))       { tk = TINYJS_LEX_R_FOR; }
		else if(!strcmp(buf->str, "function"))  { tk = TINYJS_LEX_R_FUNCTION; }
		else if(!strcmp(buf->str, "if"))        { tk = TINYJS_LEX_R_IF; }
		else if(!strcmp(buf->str, "new"))       { tk = TINYJS_LEX_R_NEW; }
		else if(!strcmp(buf->str, "null"))      { tk = TINYJS_LEX_R_NULL; }
		else if(!strcmp(buf->str, "return"))    { tk = TINYJS_LEX_R_RETURN; }
		else if(!strcmp(buf->str, "true"))      { tk = TINYJS_LEX_R_TRUE; }
		else if(!strcmp(buf->str, "undefined")) { tk = TINYJS_LEX_R_UNDEFINED; }
		else if(!strcmp(buf->str, "var"))       { tk = TINYJS_LEX_R_VAR; }
		else if(!strcmp(buf->str, "while"))     { tk = TINYJS_LEX_R_WHILE; }
		else                                    { tk = TINYJS_LEX_ID; }
	//数値リテラル
	} else if(isdigit(currCh)) {
		int isHex = 0;
		if(currCh == '0') {
			g_string_append_c(buf, currCh);
			getNextCh();
			if((currCh == 'x') || (currCh == 'X')) {
				isHex = 1;
				g_string_append_c(buf, currCh);
				getNextCh();
			}
		}
		tk = TINYJS_LEX_L_NUM;
		while(isdigit(currCh) || (isHex && isxdigit(currCh))) {
			g_string_append_c(buf, currCh);
			getNextCh();
		}
		if(!isHex && (currCh == '.')) {
			g_string_append_c(buf, currCh);
			getNextCh();
			while(isdigit(currCh)) {
				g_string_append_c(buf, currCh);
				getNextCh();
			}
		}
		//Do fancy e-style floating point.
		if(!isHex && ((currCh == 'e') || (currCh == 'E'))) {
			g_string_append_c(buf, currCh);
			getNextCh();
			if(currCh == '-') {
				g_string_append_c(buf, currCh);
				getNextCh();
			}
			while(isdigit(currCh)) {
				g_string_append_c(buf, currCh);
				getNextCh();
			}
		}
	//文字列リテラル
	} else if((currCh == '"') || (currCh == '\'')) {
		const int quotCh = currCh;
		tk = TINYJS_LEX_L_STR;
		getNextCh();
		while(currCh && (currCh != quotCh)) {
			if(currCh == '\\') {
				getNextCh();
				switch(currCh) {
				case 't':  g_string_append_c(buf, '\t'); break;	//┐
				case 'n':  g_string_append_c(buf, '\n'); break;	//├頻繁に使う制御文字のエスケープシーケンスだけ対応した。頻繁に使わない制御文字はその他の印字不可文字と共通で8進数形式,又は,16進数形式での入力を前提とする。もし不足ならば追加しろ。
				case 'r':  g_string_append_c(buf, '\r'); break;	//┘
				case '\\': g_string_append_c(buf, '\\'); break;
				default:
					//Hex digits.
					if((currCh == 'x') || (currCh == 'X')) {
						GString* tmp = g_string_new(NULL);
						for(;;) {			//C言語の文字列リテラルの仕様では、"\x????～"には桁数制限が無い。JavaScriptでも同じなのかわからない。(※TODO:要確認)
							getNextCh();
							if(!isxdigit(currCh)) { break; }
							g_string_append_c(tmp, currCh);
						}
						g_string_append_c(buf, (char)strtoul(tmp->str, NULL, 16));
					//Octal digits.
					} else if((currCh >= '0') && (currCh <= '7')) {
						GString* tmp = g_string_new(NULL);
						while(tmp->len < 3) {		//C言語の文字列リテラルの仕様では、"\???"の桁数は1桁～3桁である。JavaScriptでも同じなのかわからない。(※TODO:要確認)
							if(!isxdigit(currCh)) { break; }
							g_string_append_c(tmp, currCh);
							getNextCh();
						}
						g_string_append_c(buf, (char)strtoul(tmp->str, NULL, 8));
					} else {
						g_string_append_c(buf, currCh);
					}
					break;
				}
			} else {
				g_string_append_c(buf, currCh);
			}
			getNextCh();
		}
		getNextCh();
	//演算子,又は,EOF
	} else {
		tk = currCh;
		if(tk) {
			getNextCh();
			if((tk == '=') && (currCh == '=')) {
				tk = TINYJS_LEX_O_EQUAL;			//"=="
				getNextCh();
				if(currCh == '=') {
					tk = TINYJS_LEX_O_TYPEEQUAL;		//"==="
					getNextCh();
				}
			} else if((tk == '!') && (currCh == '=')) {
				tk = TINYJS_LEX_O_NEQUAL;			//"!="
				getNextCh();
				if(currCh == '=') {
					tk = TINYJS_LEX_O_NTYPEEQUAL;		//"!=="
					getNextCh();
				}
			} else if((tk == '<') && (currCh == '=')) {
				tk = TINYJS_LEX_O_LEQUAL;			//"<="
				getNextCh();
			} else if((tk == '<') && (currCh == '<')) {
				tk = TINYJS_LEX_O_LSHIFT;			//"<<"
				getNextCh();
				if(currCh == '=') {
					tk = TINYJS_LEX_O_LSHIFTASSIGN;		//"<<="
					getNextCh();
				}
			} else if(tk == '>' && currCh == '=') {
				tk = TINYJS_LEX_O_GEQUAL;			//">="
				getNextCh();
			} else if(tk == '>' && currCh == '>') {
				tk = TINYJS_LEX_O_RSHIFT;			//">>"
				getNextCh();
				if(currCh == '=') {
					tk = TINYJS_LEX_O_RSHIFTASSIGN;		//">>="
					getNextCh();
				} else if(currCh == '>') {
					tk = TINYJS_LEX_O_RSHIFTUNSIGNED;	//">>>"
					getNextCh();
				}
			} else if(tk == '+' && currCh == '=') {
				tk = TINYJS_LEX_O_PLUSASSIGN;			//"+="
				getNextCh();
			} else if(tk == '-' && currCh == '=') {
				tk = TINYJS_LEX_O_MINUSASSIGN;			//"-="
				getNextCh();
			} else if(tk == '+' && currCh == '+') {
				tk = TINYJS_LEX_O_PLUSPLUS;			//"++"
				getNextCh();
			} else if(tk == '-' && currCh == '-') {
				tk = TINYJS_LEX_O_MINUSMINUS;			//"--"
				getNextCh();
			} else if(tk == '&' && currCh == '=') {
				tk = TINYJS_LEX_O_ANDASSIGN;			//"&="
				getNextCh();
			} else if(tk == '&' && currCh == '&') {
				tk = TINYJS_LEX_O_ANDAND;			//"&&"
				getNextCh();
			} else if(tk == '|' && currCh == '=') {
				tk = TINYJS_LEX_O_OREQUAL;			//"|="
				getNextCh();
			} else if(tk == '|' && currCh == '|') {
				tk = TINYJS_LEX_O_OROR;				//"||"
				getNextCh();
			} else if(tk == '^' && currCh == '=') {
				tk = TINYJS_LEX_O_XORASSIGN;			//"^="
				getNextCh();
			} else {
				/** no job **/					//その他の一文字演算子,又は,不正な文字
			}
		} else {
			/** no job **/						//EOF
		}
	}
	//Record ending of last token.
	tokenLastEnd = tokenEnd;
	//Record ending of this token.
	tokenEnd = dataPos - 2;
	//
	//	～■■■□□□～
	//	　…┬┘↑↑↑
	//	　　│　││└         dataPos
	//	　　│　│└─ nextCh: dataPos - 1
	//	　　│　└── currCh: dataPos - 2 = tokenEnd
	//	　　└──── tkStr
	//
	tkStr = buf->str;	//忘れないで!!
}
//-----------------------------------------------------------------------------
//Return a sub-string from the given position up until right now.
const char* ST_TinyJS_Lex::getSubString(int lastPosition) {
	if(lastPosition > dataEnd) { DIE(); }	//バグ
	if(tokenLastEnd > dataEnd) { DIE(); }	//バグ
	return strndup(data + lastPosition, tokenLastEnd - lastPosition);
}
//-----------------------------------------------------------------------------
//Return a sub-lexer from the given position up until right now.
ST_TinyJS_Lex* ST_TinyJS_Lex::getSubLex(int lastPosition) {
	if(lastPosition > dataEnd) { DIE(); }	//バグ
	if(tokenLastEnd > dataEnd) { DIE(); }	//バグ
	return new ST_TinyJS_Lex(data, lastPosition, tokenLastEnd);
}
//-----------------------------------------------------------------------------
//Return a string representing the position in lines and columns of the tokenLastEnd - 1.
const char* ST_TinyJS_Lex::getLastPosition() {
	return ST_TinyJS_Lex::getPosition(tokenLastEnd - 1);
}
//-----------------------------------------------------------------------------
//Return a string representing the position in lines and columns of the character pos given.
const char* ST_TinyJS_Lex::getPosition(int pos) {
	if(pos > dataEnd) { pos = dataEnd; }
	int line = 1, col = 1;
	for(int i = 0; i < pos; i++) {
		col++;
		if(data[i] == '\n') {
			line++;
			col = 1;
		}
	}
	return strdup_printf("(line: %d, col: %d)", line, col);
}
//*****************************************************************************
//	ST_TinyJS_VarLink
//*****************************************************************************
ST_TinyJS_VarLink::ST_TinyJS_VarLink(ST_TinyJS_Var* v) {

	this->name  = "";
	this->var   = v;
	this->owned = 0;
}
//-----------------------------------------------------------------------------
//Replace the Variable pointed to.
void ST_TinyJS_VarLink::replaceWith(ST_TinyJS_Var* v) {
	if(!v) { DIE(); }	//バグ
	var = v;
}
//-----------------------------------------------------------------------------
//Get the name as an integer (for arrays).
int ST_TinyJS_VarLink::getIntName() {
	return atoi(name);
}
//-----------------------------------------------------------------------------
//Set the name as an integer (for arrays).
void ST_TinyJS_VarLink::setIntName(int n) {
	const char* buf = strdup_printf("%d", n);
	name = buf;
}
//*****************************************************************************
//	ST_TinyJS_Var
//*****************************************************************************
ST_TinyJS_Var* ST_TinyJS_Var::newUndefined() {
	ST_TinyJS_Var* v = new ST_TinyJS_Var();
	v->setUndefined();
	return v;
}
//-----------------------------------------------------------------------------
ST_TinyJS_Var* ST_TinyJS_Var::newNull() {
	ST_TinyJS_Var* v = new ST_TinyJS_Var();
	v->setNull();
	return v;
}
//-----------------------------------------------------------------------------
ST_TinyJS_Var* ST_TinyJS_Var::newNumber(double val) {
	ST_TinyJS_Var* v = new ST_TinyJS_Var();
	v->setNumber(val);
	return v;
}
//-----------------------------------------------------------------------------
ST_TinyJS_Var* ST_TinyJS_Var::newString(const char* str) {
	ST_TinyJS_Var* v = new ST_TinyJS_Var();
	v->setString(str);
	return v;
}
//-----------------------------------------------------------------------------
ST_TinyJS_Var* ST_TinyJS_Var::newFunction() {
	ST_TinyJS_Var* v = new ST_TinyJS_Var();
	v->setFunction();
	return v;
}
//-----------------------------------------------------------------------------
ST_TinyJS_Var* ST_TinyJS_Var::newObject() {
	ST_TinyJS_Var* v = new ST_TinyJS_Var();
	v->setObject();
	return v;
}
//-----------------------------------------------------------------------------
ST_TinyJS_Var* ST_TinyJS_Var::newArray() {
	ST_TinyJS_Var* v = new ST_TinyJS_Var();
	v->setArray();
	return v;
}
//-----------------------------------------------------------------------------
ST_TinyJS_Var* ST_TinyJS_Var::newNative(TinyJS_Callback* callback, void* userData) {
	ST_TinyJS_Var* v = new ST_TinyJS_Var();
	v->setNative(callback, userData);
	return v;
}
//-----------------------------------------------------------------------------
void ST_TinyJS_Var::setUndefined() {
	init(TINYJS_VAR_UNDEFINED);
}
//-----------------------------------------------------------------------------
void ST_TinyJS_Var::setNull() {
	init(TINYJS_VAR_NULL);
}
//-----------------------------------------------------------------------------
void ST_TinyJS_Var::setNumber(double val) {
	init(TINYJS_VAR_NUMBER);
	numData = val;
}
//-----------------------------------------------------------------------------
void ST_TinyJS_Var::setString(const char* str) {
	init(TINYJS_VAR_STRING);
	strData = str;
}
//-----------------------------------------------------------------------------
void ST_TinyJS_Var::setFunction() {
	init(TINYJS_VAR_FUNCTION);
}
//-----------------------------------------------------------------------------
void ST_TinyJS_Var::setObject() {
	init(TINYJS_VAR_OBJECT);
}
//-----------------------------------------------------------------------------
void ST_TinyJS_Var::setArray() {
	init(TINYJS_VAR_ARRAY);
}
//-----------------------------------------------------------------------------
void ST_TinyJS_Var::setNative(TinyJS_Callback* callback, void* userData) {
	init(TINYJS_VAR_NATIVE);
	this->callback = callback;
	this->userData = userData;
}
//-----------------------------------------------------------------------------
void ST_TinyJS_Var::init(int varType) {
	type       = varType;
	numData    = 0;
	strData    = "";
	callback   = NULL;
	userData   = NULL;
	firstChild = NULL;
}
//-----------------------------------------------------------------------------
//If this is a function, get the result value (for use by native functions).
ST_TinyJS_Var* ST_TinyJS_Var::getReturnVar() {
	ST_TinyJS_VarLink* resultVarLink = findChild("return");
	if(!resultVarLink) {
		throw new ST_TinyJS_Exception("getReturnVar() has been called outside a function.");
	}
	return resultVarLink->var;
}
//-----------------------------------------------------------------------------
//Set the result value.
//Use this when setting complex return data as it avoids a deepCopy().
void ST_TinyJS_Var::setReturnVar(ST_TinyJS_Var* v) {
	ST_TinyJS_VarLink* resultVarLink = findChild("return");
	if(!resultVarLink) {
		throw new ST_TinyJS_Exception("setReturnVar() has been called outside a function.");
	}
	resultVarLink->replaceWith(v);
}
//-----------------------------------------------------------------------------
//If this is a function, get the parameter with the given name (for use by native functions).
ST_TinyJS_Var* ST_TinyJS_Var::getParameter(const char* name) {
	return findChildOrCreate(name)->var;
}
//-----------------------------------------------------------------------------
//Tries to find a child with the given name, may return NULL.
ST_TinyJS_VarLink* ST_TinyJS_Var::findChild(const char* name) {
	GSList/*<ST_TinyJS_VarLink*>*/* list = firstChild;
	while(list) {
		ST_TinyJS_VarLink* l = (ST_TinyJS_VarLink*)list->data;
		if(!strcmp(l->name, name)) { return l; }
		list = list->next;
	}
	return NULL;
}
//-----------------------------------------------------------------------------
//Tries to find a child with the given name, or will create it with the given type.
ST_TinyJS_VarLink* ST_TinyJS_Var::findChildOrCreate(const char* name) {
	ST_TinyJS_VarLink* l = findChild(name);
	if(l) { return l; }
	return addChild(name, ST_TinyJS_Var::newUndefined());
}
//-----------------------------------------------------------------------------
ST_TinyJS_VarLink* ST_TinyJS_Var::addChild(const char* name, ST_TinyJS_Var* v) {
	if(isUndefined()) { type = TINYJS_VAR_OBJECT; }	//※なぜ？
	if(findChild(name)) {
		throw new ST_TinyJS_Exception(strdup_printf("'%s' already exists.", name));
	}
	ST_TinyJS_VarLink* l = new ST_TinyJS_VarLink(v);
	l->name  = name;
	l->owned = 1;
	firstChild = g_slist_append(firstChild, l);
	return l;
}
//-----------------------------------------------------------------------------
//Remove a specific link (this is faster than finding via a child).
void ST_TinyJS_Var::removeLink(ST_TinyJS_VarLink* l) {
	firstChild = g_slist_remove(firstChild, l);
}
//-----------------------------------------------------------------------------
//The the value at an array index.
ST_TinyJS_Var* ST_TinyJS_Var::getArrayIndex(int i) {
	const char* buf = strdup_printf("%d", i);
	ST_TinyJS_VarLink* l = findChild(buf);
	if(l) {
		return l->var;
	} else {
		return ST_TinyJS_Var::newUndefined();
	}
}
//-----------------------------------------------------------------------------
void ST_TinyJS_Var::setArrayIndex(int i, ST_TinyJS_Var* v) {
	const char* buf = strdup_printf("%d", i);
	ST_TinyJS_VarLink* l = findChild(buf);
	if(l) {
		if(v->isUndefined()) {
			removeLink(l);	//※本当はundefinedを代入しても配列要素は削除されないはずだが…当インタプリタにはdelete命令が無いようなのでその代用なのだろう。
		} else {
			l->replaceWith(v);
		}
	} else {
		if(v->isUndefined()) {
			/** no job **/	//※本当はundefinedを明示的に新規代入した場合も配列要素が生成されるはずだが…
		} else {
			addChild(buf, v);
		}
	}
}
//-----------------------------------------------------------------------------
//If this is an array, return the number of items in it (else 0).
int ST_TinyJS_Var::getArrayLength() {
	if(!isArray()) { return 0; }
	int ubound = -1;
	GSList/*<ST_TinyJS_VarLink*>*/* list = firstChild;
	while(list) {
		ST_TinyJS_VarLink* l = (ST_TinyJS_VarLink*)list->data;
		char* endptr;
		int i = strtoul(l->name, &endptr, 10);
		if(*endptr == '\0') {
			if(i > ubound) { ubound = i; }
		}
		list = list->next;
	}
	return ubound + 1;
}
//-----------------------------------------------------------------------------
int ST_TinyJS_Var::getBoolean() {
	return getNumber() != 0;	//※色々問題が有る気がする。getInt()がオブジェクトや文字列に対して0を返し、getBoolean()でfalseになってしまう。せめてオブジェクトや空でない文字列はtrueと判断すべきではないか?(※TODO:要検討)
}
//-----------------------------------------------------------------------------
double ST_TinyJS_Var::getNumber() {
	if(isNumber()) { return numData; }
//不要	if(isNull()) { return 0; }
//不要	if(isUndefined()) { return 0; }	//※本当はNumber(undefined)⇒NaNだが…
	return 0;			//※本当は0とは限らないが…せめて文字列⇒数値変換ぐらいは実装すべきではないか?(※TODO:)
}
//-----------------------------------------------------------------------------
const char* ST_TinyJS_Var::getString() {
	if(isNumber()) { return strdup_printf("%g", numData); }
	if(isNull()) { return "null"; }
	if(isUndefined()) { return "undefined"; }
	return strData;			//※オブジェクトや配列の文字列化は未対応なのか?(※TODO:要検討)
}
//-----------------------------------------------------------------------------
int ST_TinyJS_Var::equals(ST_TinyJS_Var* v) {
	ST_TinyJS_Var* tmp = mathsOp(v, TINYJS_LEX_O_EQUAL);
	return tmp->getBoolean();
}
//-----------------------------------------------------------------------------
//Do a maths op with another script variable.
ST_TinyJS_Var* ST_TinyJS_Var::mathsOp(ST_TinyJS_Var* v, int op) {
	ST_TinyJS_Var* const a = this;	//┬読み易いように変数名を揃えているだけ。必須ではない。
	ST_TinyJS_Var* const b = v;	//┘

	//※当関数の動作は本物のJavaScriptとだいぶん違う。簡略化のためにやむをえないとは思うが、もう少し本物のJavaScriptに近付けられないだろうか？(※TODO:要検討)

	//Type equality check.
	if((op == TINYJS_LEX_O_TYPEEQUAL) || (op == TINYJS_LEX_O_NTYPEEQUAL)) {
		//Check type first, then call again to check data.
		int eql = (a->type == b->type);
		if(eql) {
			ST_TinyJS_Var* tmp = a->mathsOp(b, TINYJS_LEX_O_EQUAL);
			if(!tmp->getBoolean()) { eql = 0; }
		}
		if(op == TINYJS_LEX_O_NTYPEEQUAL) { eql = !eql; }
		return ST_TinyJS_Var::newNumber(eql);
	}
	//Do maths...
	if((a->isUndefined() || a->isNull()) && (b->isUndefined() || b->isNull())) {
		if(op == TINYJS_LEX_O_EQUAL) {
			return ST_TinyJS_Var::newNumber(1);	//(undefined==null)もtrueです。(本物のJavaScriptでも)
		} else if(op == TINYJS_LEX_O_NEQUAL) {
			return ST_TinyJS_Var::newNumber(0);	//(undefined!=null)もfalseです。(本物のJavaScriptでも)
		} else {
			return ST_TinyJS_Var::newUndefined();	//undefined	※本当は違う。(undefined+null)⇒NaNや(undefined|null)⇒0など。とは言え実際にそれらの挙動に依存するようなスクリプトは書かないと思う。(※TODO:要検討)
		}
	} else if(a->isNumber() && b->isNumber()) {
		double da = a->getNumber();
		double db = b->getNumber();
		switch(op) {
		case '+':                 return ST_TinyJS_Var::newNumber(     da +      db);
		case '-':                 return ST_TinyJS_Var::newNumber(     da -      db);
		case '*':                 return ST_TinyJS_Var::newNumber(     da *      db);
		case '/':                 return ST_TinyJS_Var::newNumber(     da /      db);
		case '&':                 return ST_TinyJS_Var::newNumber((int)da & (int)db);
		case '|':                 return ST_TinyJS_Var::newNumber((int)da | (int)db);
		case '^':                 return ST_TinyJS_Var::newNumber((int)da ^ (int)db);
		case '%':                 return ST_TinyJS_Var::newNumber(fmod(da,       db));
		case TINYJS_LEX_O_EQUAL:  return ST_TinyJS_Var::newNumber(     da ==     db);
		case TINYJS_LEX_O_NEQUAL: return ST_TinyJS_Var::newNumber(     da !=     db);
		case '<':                 return ST_TinyJS_Var::newNumber(     da <      db);
		case TINYJS_LEX_O_LEQUAL: return ST_TinyJS_Var::newNumber(     da <=     db);
		case '>':                 return ST_TinyJS_Var::newNumber(     da >      db);
		case TINYJS_LEX_O_GEQUAL: return ST_TinyJS_Var::newNumber(     da >=     db);
		default:throw new ST_TinyJS_Exception(strdup_printf("Operation %s not supported on the number datatype.", ST_TinyJS_Lex::getTokenStr(op)));
		}
	} else if(a->isArray()) {
		//Just check pointers.
		switch(op) {
		case TINYJS_LEX_O_EQUAL:  return ST_TinyJS_Var::newNumber(a == b);
		case TINYJS_LEX_O_NEQUAL: return ST_TinyJS_Var::newNumber(a != b);
		default:throw new ST_TinyJS_Exception(strdup_printf("Operation %s not supported on the Array datatype.", ST_TinyJS_Lex::getTokenStr(op)));
		}
	} else if(a->isObject()) {
		//Just check pointers.
		switch(op) {
		case TINYJS_LEX_O_EQUAL:  return ST_TinyJS_Var::newNumber(a == b);
		case TINYJS_LEX_O_NEQUAL: return ST_TinyJS_Var::newNumber(a != b);
		default:throw new ST_TinyJS_Exception(strdup_printf("Operation %s not supported on the Object datatype.", ST_TinyJS_Lex::getTokenStr(op)));
		}
	} else {
		const char* da = a->getString();
		const char* db = b->getString();
		//Use strings.
		switch(op) {
		case '+':                 return ST_TinyJS_Var::newString(strconcat(da, db, NULL));
		case TINYJS_LEX_O_EQUAL:  return ST_TinyJS_Var::newNumber(strcmp(da, db) == 0);
		case TINYJS_LEX_O_NEQUAL: return ST_TinyJS_Var::newNumber(strcmp(da, db) != 0);
		case '<':                 return ST_TinyJS_Var::newNumber(strcmp(da, db) <  0);
		case TINYJS_LEX_O_LEQUAL: return ST_TinyJS_Var::newNumber(strcmp(da, db) <= 0);
		case '>':                 return ST_TinyJS_Var::newNumber(strcmp(da, db) >  0);
		case TINYJS_LEX_O_GEQUAL: return ST_TinyJS_Var::newNumber(strcmp(da, db) >= 0);
		default:throw new ST_TinyJS_Exception(strdup_printf("Operation %s not supported on the string datatype.", ST_TinyJS_Lex::getTokenStr(op)));
		}
	}
}
//-----------------------------------------------------------------------------
//Deep copy this node and return the result.
ST_TinyJS_Var* ST_TinyJS_Var::deepCopy() {
	ST_TinyJS_Var* v = ST_TinyJS_Var::newUndefined();
	//Copy the basic data and type from the variable given, with no children.
	v->type    = type;
	v->numData = numData;
	v->strData = strData;
	//Copy children.
	GSList/*<ST_TinyJS_VarLink*>*/* list = firstChild;
	while(list) {
		ST_TinyJS_VarLink* l = (ST_TinyJS_VarLink*)list->data;
		ST_TinyJS_Var* tmp;
		//Don't copy the 'prototype' object...
		if(!strcmp(l->name, "prototype")) {	//"prototype"ならば…
			tmp = l->var;
		} else {				//"prototype"以外ならば…
			tmp = l->var->deepCopy();
		}
		v->addChild(l->name, tmp);
		list = list->next;
	}
	return v;
}
//-----------------------------------------------------------------------------
//Dump out the contents of this using trace.
void ST_TinyJS_Var::trace(const char* indent, const char* name) {
	printf("%s'%s' = '%s' %s\n",
		indent,
		name,
		getString(),
		getTypeAsString());
	const char* indentStr = strconcat(indent, " ", NULL);
	GSList/*<ST_TinyJS_VarLink*>*/* list = firstChild;
	while(list) {
		ST_TinyJS_VarLink* l = (ST_TinyJS_VarLink*)list->data;
		l->var->trace(indentStr, l->name);
		list = list->next;
	}
}
//-----------------------------------------------------------------------------
//For debugging - just dump a string version of the type.
const char* ST_TinyJS_Var::getTypeAsString() {
	if(type == TINYJS_VAR_NUMBER)   { return "NUMBER"; }
	if(type == TINYJS_VAR_STRING)   { return "STRING"; }
	if(type == TINYJS_VAR_FUNCTION) { return "FUNCTION"; }
	if(type == TINYJS_VAR_OBJECT)   { return "OBJECT"; }
	if(type == TINYJS_VAR_ARRAY)    { return "ARRAY"; }
	if(type == TINYJS_VAR_NATIVE)   { return "NATIVE"; }
	return "";
}
//-----------------------------------------------------------------------------
//Get data as a parsable javascript string.
const char* ST_TinyJS_Var::getParsableString() {
	//Numbers can just be put in directly.
	if(isNumber()) { return getString(); }
	if(isFunction()) {
		GString* funcStr = g_string_new("function (");
		//Get list of parameters.
		GSList/*<ST_TinyJS_VarLink*>*/* list = firstChild;
		while(list) {
			ST_TinyJS_VarLink* l = (ST_TinyJS_VarLink*)list->data;
			g_string_append(funcStr, l->name);
			if(list->next) { g_string_append(funcStr, ","); }
			list = list->next;
		}
		//Add function body.
		g_string_append(funcStr, ") ");
		g_string_append(funcStr, getString());
		return funcStr->str;
	}
	//If it is a string then we quote it.
	if(isString()) { return getJSString(getString()); }
	if(isNull()) { return "null"; }
	return "undefined";	//※TODO:オブジェクトや配列も間違って"undefined"と表示されてしまうのではないか?
}
//-----------------------------------------------------------------------------
//Write out all the JS code needed to recreate this script variable to the stream (as JSON).
const char* ST_TinyJS_Var::getJSON(const char* linePrefix) {
	GString* indentedLinePrefix = g_string_new(linePrefix);
	g_string_append(indentedLinePrefix, "  ");
	GString* destination = g_string_new(NULL);
	if(isObject()) {
		//Children - handle with bracketed list.
		g_string_append(destination, "{\n");
		GSList/*<ST_TinyJS_VarLink*>*/* list = firstChild;
		while(list) {
			ST_TinyJS_VarLink* l = (ST_TinyJS_VarLink*)list->data;
			g_string_append(destination, indentedLinePrefix->str);
			g_string_append(destination, getJSString(l->name));
			g_string_append(destination, ": ");
			g_string_append(destination, l->var->getJSON(indentedLinePrefix->str));
			if(list->next) { g_string_append(destination, ","); }
			g_string_append(destination, "\n");
			list = list->next;
		}
		g_string_append(destination, linePrefix);
		g_string_append(destination, "}");
	} else if(isArray()) {
		g_string_append(destination, "[\n");
		int len = getArrayLength();
		if(len > 10000) { len = 10000; }	//We don't want to get stuck here!
		for(int i = 0; i < len; i++) {
			g_string_append(destination, getArrayIndex(i)->getJSON(indentedLinePrefix->str));
			if(i < len - 1) { g_string_append(destination, ","); }
			g_string_append(destination, "\n");
		}
		g_string_append(destination, linePrefix);
		g_string_append(destination, "]");
	} else {
		//No children or a function... just write value directly.
		g_string_append(destination, getParsableString());
	}
	return destination->str;
}
//*****************************************************************************
//	ST_TinyJS
//*****************************************************************************
ST_TinyJS::ST_TinyJS() {
	lex       = NULL;
	scopes    = NULL;
	callStack = NULL;
	root      = ST_TinyJS_Var::newObject();
	//Add built-in classes.
	stringClass = ST_TinyJS_Var::newObject();
	arrayClass  = ST_TinyJS_Var::newObject();
	objectClass = ST_TinyJS_Var::newObject();
	root->addChild("String", stringClass);
	root->addChild("Array",  arrayClass);
	root->addChild("Object", objectClass);
}
//-----------------------------------------------------------------------------
void ST_TinyJS::execute(const char* code) {
	ST_TinyJS_Context* pSavedContext = saveContext();
	{
		//{{InitContext
		lex       = new ST_TinyJS_Lex(code, 0, strlen(code));
		scopes    = g_slist_prepend(NULL, root);
		callStack = NULL;
		//}}InitContext
		try {
			int bExec = 1;
			while(lex->tk) { statement(&bExec); }
		} catch(ST_TinyJS_Exception* e) {
			const char* msg = stackTrace(e->msg);
			restoreContext(pSavedContext);
			throw new ST_TinyJS_Exception(msg);
		}
	}
	restoreContext(pSavedContext);
}
//-----------------------------------------------------------------------------
//Evaluate the given code and return a javascript object, useful for(dangerous) JSON parsing.
//If nothing to return, will return 'undefined' variable type.
ST_TinyJS_Var* ST_TinyJS::evaluate(const char* code) {
	ST_TinyJS_VarLink* l = NULL;
	ST_TinyJS_Context* pSavedContext = saveContext();
	{
		//{{InitContext
		lex       = new ST_TinyJS_Lex(code, 0, strlen(code));
		scopes    = g_slist_prepend(NULL, root);
		callStack = NULL;
		//}}InitContext
		try {
			int bExec = 1;
			do {
				l = base(&bExec);
				if(lex->tk != TINYJS_LEX_EOF) { lex->match(';'); }
			} while(lex->tk != TINYJS_LEX_EOF);
		} catch(ST_TinyJS_Exception* e) {
			const char* msg = stackTrace(e->msg);
			restoreContext(pSavedContext);
			throw new ST_TinyJS_Exception(msg);
		}
	}
	restoreContext(pSavedContext);
	return l ? l->var : ST_TinyJS_Var::newUndefined();
}
//-----------------------------------------------------------------------------
//Add a native function to be called from TinyJS.
//example:
//│void scRandInt(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) { ... }
//│tinyJS->addNative("function randInt(min, max)", scRandInt, NULL);
//or
//│void scSubstring(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) { ... }
//│tinyJS->addNative("function String.substring(lo, hi)", scSubstring, NULL);
void ST_TinyJS::addNative(const char* funcDesc, TinyJS_Callback* callback, void* userData) {
	ST_TinyJS_Context* pSavedContext = saveContext();
	{
		//{{InitContext
		lex = new ST_TinyJS_Lex(funcDesc, 0, strlen(funcDesc));
		//}}InitContext
		ST_TinyJS_Var* base = root;
		lex->match(TINYJS_LEX_R_FUNCTION);
		const char* funcName = lex->tkStr;
		lex->match(TINYJS_LEX_ID);
		//Check for dots, we might want to do something like function String.substring ...
		while(lex->tk == '.') {
			lex->match('.');
			ST_TinyJS_VarLink* l = base->findChild(funcName);
			//If it doesn't exist, make an object class.
			if(!l) { l = base->addChild(funcName, ST_TinyJS_Var::newObject()); }
			base = l->var;
			funcName = lex->tkStr;
			lex->match(TINYJS_LEX_ID);
		}
		ST_TinyJS_Var* funcVar = ST_TinyJS_Var::newNative(callback, userData);
		parseFunctionArguments(funcVar);
		base->addChild(funcName, funcVar);
	}
	restoreContext(pSavedContext);
}
//-----------------------------------------------------------------------------
//Send all variables to stdout.
void ST_TinyJS::trace(const char* indent) {
	root->trace(indent, "root");
}
//-----------------------------------------------------------------------------
//Handle a function call (assumes we've parsed the function name and we're on the start bracket).
//'_this' is the object that contains this method, if there was one (otherwise it's just a normnal function).
ST_TinyJS_VarLink* ST_TinyJS::functionCall(int* pExec, ST_TinyJS_VarLink* func, ST_TinyJS_Var* _this/*NULL可*/) {
	if(*pExec) {
		if(!func->var->isFunction()) {
			throw new ST_TinyJS_Exception(strdup_printf("Expecting '%s' to be a function.", func->name));
		}
		lex->match('(');
		//Create a new symbol table entry for execution of this function.
		ST_TinyJS_Var* funcRoot = ST_TinyJS_Var::newObject();
		if(_this) {
			funcRoot->addChild("this", _this);
		}
		//Grab in all parameters.
		GSList/*<ST_TinyJS_VarLink*>*/* list = func->var->firstChild;
		while(list) {
			ST_TinyJS_VarLink* l = (ST_TinyJS_VarLink*)list->data;
			ST_TinyJS_VarLink* tmp = base(pExec);
			if(*pExec) {
				if(tmp->var->isBasic()) {
					//pass by value
					funcRoot->addChild(l->name, tmp->var->deepCopy());
				} else {
					//pass by reference
					funcRoot->addChild(l->name, tmp->var);
				}
			}
			if(list->next) { lex->match(','); }
			list = list->next;
		}
		lex->match(')');
		//Execute function!
		//Add the function's execute space to the symbol table so we can recurse.
		ST_TinyJS_VarLink* returnVarLink = funcRoot->addChild("return", ST_TinyJS_Var::newUndefined());
		ST_TinyJS_Context* pSavedContext = saveContext();
		{
			//We just want to execute the block, but something could have messed up and left us with the wrong ScriptLex, so we want to be careful here...
			const char* funcBody = func->var->getString();
			//{{InitContext
			lex       = new ST_TinyJS_Lex(funcBody, 0, strlen(funcBody));
			scopes    = g_slist_prepend(scopes, funcRoot);
			callStack = g_slist_prepend(callStack, strdup_printf("%s from %s", func->name, lex->getLastPosition()));
			//}}InitContext
			try {
				if(func->var->isNative()) {
					(*func->var->callback)(this, funcRoot, func->var->userData);
				} else {
					block(pExec);
					//Because return will probably have called this, and set execute to false.
					*pExec = 1;
				}
			} catch(ST_TinyJS_Exception* /*e*/) {
				restoreContext(pSavedContext);
				throw;
			}
		}
		restoreContext(pSavedContext);
	//不要	funcRoot->removeLink(returnVarLink);	⇒funcRootは当関数内を抜けたら無効なので、クリーンアップする必要も無い。
		return returnVarLink;
	} else {
		//Function, but not executing - just parse args and be done.
		lex->match('(');
		while(lex->tk != ')') {
			ST_TinyJS_VarLink* l = base(pExec);
			if(lex->tk != ')') { lex->match(','); }
		}
		lex->match(')');
		if(lex->tk == '{') {	//TODO: Why is this here?
			block(pExec);
		}
		return new ST_TinyJS_VarLink(ST_TinyJS_Var::newUndefined());
	}
}
//-----------------------------------------------------------------------------
//因子
ST_TinyJS_VarLink* ST_TinyJS::factor(int* pExec) {
	//(～)
	if(lex->tk == '(') {
		lex->match('(');
		ST_TinyJS_VarLink* a = base(pExec);
		lex->match(')');
		return a;
	//true
	} else if(lex->tk == TINYJS_LEX_R_TRUE) {
		lex->match(TINYJS_LEX_R_TRUE);
		return new ST_TinyJS_VarLink(ST_TinyJS_Var::newNumber(1));
	//false
	} else if(lex->tk == TINYJS_LEX_R_FALSE) {
		lex->match(TINYJS_LEX_R_FALSE);
		return new ST_TinyJS_VarLink(ST_TinyJS_Var::newNumber(0));
	//null
	} else if(lex->tk == TINYJS_LEX_R_NULL) {
		lex->match(TINYJS_LEX_R_NULL);
		return new ST_TinyJS_VarLink(ST_TinyJS_Var::newNull());
	//undefined
	} else if(lex->tk == TINYJS_LEX_R_UNDEFINED) {
		lex->match(TINYJS_LEX_R_UNDEFINED);
		return new ST_TinyJS_VarLink(ST_TinyJS_Var::newUndefined());
	//ID
	} else if(lex->tk == TINYJS_LEX_ID) {
		ST_TinyJS_VarLink* a = NULL;
		if(*pExec) {
			a = findInScopes(lex->tkStr);
		}
		if(!a) {
			//Variable doesn't exist! JavaScript says we should create it.
			//(We won't add it here. This is done in the assignment operator.)
			a = new ST_TinyJS_VarLink(ST_TinyJS_Var::newUndefined());
			a->name = lex->tkStr;
		}
		lex->match(TINYJS_LEX_ID);
		//ID(～),又は,ID.ID,又は,ID[～]を、IDに還元して繰り返す。
		ST_TinyJS_Var* _this = NULL;	//The 'this' if we're executing a method call.
		while((lex->tk == '(') ||
		      (lex->tk == '.') ||
		      (lex->tk == '[')) {
			switch(lex->tk) {
			default:DIE();	//バグ
			//Function Call.
			case '(':
				{
					a = functionCall(pExec, a, _this);
				}
				break;
			//Record Access.
			case '.':
				{
					lex->match('.');
					if(*pExec) {
						const char* name = lex->tkStr;
						ST_TinyJS_VarLink* l = a->var->findChild(name);
						if(!l) { l = findInPrototypeClasses(a->var, name); }
						if(!l) {
							//If we haven't found this defined yet, use the built-in 'length' properly.
							if(a->var->isArray() && !strcmp(name, "length")) {
								int len = a->var->getArrayLength();
								l = new ST_TinyJS_VarLink(ST_TinyJS_Var::newNumber(len));
							} else if(a->var->isString() && !strcmp(name, "length")) {
								int len = strlen(a->var->getString());
								l = new ST_TinyJS_VarLink(ST_TinyJS_Var::newNumber(len));
							} else {
								l = a->var->addChild(name, ST_TinyJS_Var::newUndefined());
							}
						}
						_this = a->var;
						a = l;
					}
					lex->match(TINYJS_LEX_ID);
				}
				break;
			//Array Access.
			case '[':
				{
					lex->match('[');
					ST_TinyJS_VarLink* index = base(pExec);
					lex->match(']');
					if(*pExec) {
						ST_TinyJS_VarLink* l = a->var->findChildOrCreate(index->var->getString());
						_this = a->var;
						a = l;
					}
				}
				break;
			}
		}
		return a;
	//数値リテラル
	} else if(lex->tk == TINYJS_LEX_L_NUM) {
		//strtod()は10進実数と16進整数を解釈するが、8進整数を解釈しないので、8進整数のみ明示的に変換する事にした。
		double value;
		char* endptr;
		if((lex->tkStr[0] == '0') && (strspn(lex->tkStr, "01234567") == strlen(lex->tkStr))) {
			value = strtoul(lex->tkStr, &endptr, 8);
		} else {
			value = strtod(lex->tkStr, &endptr);
		}
		if(*endptr) { DIE(); }	//バグ
		ST_TinyJS_Var* v = ST_TinyJS_Var::newNumber(value);
		lex->match(TINYJS_LEX_L_NUM);
		return new ST_TinyJS_VarLink(v);
	//文字列リテラル
	} else if(lex->tk == TINYJS_LEX_L_STR) {
		ST_TinyJS_Var* v = ST_TinyJS_Var::newString(lex->tkStr);
		lex->match(TINYJS_LEX_L_STR);
		return new ST_TinyJS_VarLink(v);
	//「{id:value,...}」
	} else if(lex->tk == '{') {
		lex->match('{');
		ST_TinyJS_Var* contents = ST_TinyJS_Var::newObject();
		while(lex->tk != '}') {
			const char* id = lex->tkStr;
			//We only allow strings or IDs on the left hand side of an initialisation.
			lex->match((lex->tk == TINYJS_LEX_ID) ? TINYJS_LEX_ID : TINYJS_LEX_L_STR);
			lex->match(':');
			ST_TinyJS_VarLink* l = base(pExec);
			contents->addChild(id, l->var);
			if(lex->tk != '}') { lex->match(','); }
		}
		lex->match('}');
		return new ST_TinyJS_VarLink(contents);
	//「[value,...]」
	} else if(lex->tk == '[') {
		lex->match('[');
		ST_TinyJS_Var* contents = ST_TinyJS_Var::newArray();
		int i = 0;
		while(lex->tk != ']') {
			ST_TinyJS_VarLink* l = base(pExec);
			contents->addChild(strdup_printf("%d", i), l->var);
			if(lex->tk != ']') { lex->match(','); }
			i++;
		}
		lex->match(']');
		return new ST_TinyJS_VarLink(contents);
	//「function(仮引数名,...){～}」
	} else if(lex->tk == TINYJS_LEX_R_FUNCTION) {
		ST_TinyJS_VarLink* funcDef = parseFunctionDefinition();
		if(strcmp(funcDef->name, "")) {
			throw new ST_TinyJS_Exception("Functions not defined at statement-level are not meant to have a name.");
		}
		return funcDef;
	//「new 関数名(～)」,又は,「new プロトタイプ名」,又は,「new プロトタイプ名()」
	} else if(lex->tk == TINYJS_LEX_R_NEW) {
		lex->match(TINYJS_LEX_R_NEW);
		const char* className = lex->tkStr;
		lex->match(TINYJS_LEX_ID);
		ST_TinyJS_Var* _this = ST_TinyJS_Var::newObject();
		if(*pExec) {
			ST_TinyJS_VarLink* objClassOrFunc = findInScopes(className);
			if(!objClassOrFunc) {
				throw new ST_TinyJS_Exception(strdup_printf("%s is not a valid class name.", className));
			}
			if(objClassOrFunc->var->isFunction()) {
				functionCall(pExec, objClassOrFunc, _this);
			} else {
				_this->addChild("prototype", objClassOrFunc->var);
				if(lex->tk == '(') {
					lex->match('(');
					lex->match(')');
				}
			}
		} else {
			if(lex->tk == '(') {
				lex->match('(');
				lex->match(')');
			}
		}
		return new ST_TinyJS_VarLink(_this);
	} else {
		throw new ST_TinyJS_Exception("Syntax error.");
	}
}
//-----------------------------------------------------------------------------
//単項演算子
ST_TinyJS_VarLink* ST_TinyJS::unary(int* pExec) {
	ST_TinyJS_VarLink* a;
	if(lex->tk == '-') {
		lex->match('-');
		a = unary(pExec);
		if(*pExec) {
			ST_TinyJS_Var* tmp = ST_TinyJS_Var::newNumber(0);
			ST_TinyJS_Var* res = tmp->mathsOp(a->var, '-');
			a = new ST_TinyJS_VarLink(res);
		}
	} else if(lex->tk == '!') {
		lex->match('!');
		a = unary(pExec);
		if(*pExec) {
			ST_TinyJS_Var* tmp = ST_TinyJS_Var::newNumber(0);
			ST_TinyJS_Var* res = a->var->mathsOp(tmp, TINYJS_LEX_O_EQUAL);
			a = new ST_TinyJS_VarLink(res);
		}
	} else {
		a = factor(pExec);
	}
	return a;
}
//-----------------------------------------------------------------------------
//項
ST_TinyJS_VarLink* ST_TinyJS::term(int* pExec) {
	ST_TinyJS_VarLink* a = unary(pExec);
	while((lex->tk == '*') ||
	      (lex->tk == '/') ||
	      (lex->tk == '%')) {
		int op = lex->tk;
		lex->match(op);
		ST_TinyJS_VarLink* b = unary(pExec);
		if(*pExec) {
			a = new ST_TinyJS_VarLink(a->var->mathsOp(b->var, op));
		}
	}
	return a;
}
//-----------------------------------------------------------------------------
//式
ST_TinyJS_VarLink* ST_TinyJS::expression(int* pExec) {
	ST_TinyJS_VarLink* a = term(pExec);
	while((lex->tk == '+') ||
	      (lex->tk == '-') ||
	      (lex->tk == TINYJS_LEX_O_PLUSPLUS) ||	//"++"
	      (lex->tk == TINYJS_LEX_O_MINUSMINUS)) {	//"--"
		int op = lex->tk;
		lex->match(op);
		if((op == TINYJS_LEX_O_PLUSPLUS) ||	//"++"
		   (op == TINYJS_LEX_O_MINUSMINUS)) {	//"--"
			if(*pExec) {
				ST_TinyJS_VarLink* oldVarLink = new ST_TinyJS_VarLink(a->var);
				ST_TinyJS_Var* tmp = ST_TinyJS_Var::newNumber(1);
				a->replaceWith(a->var->mathsOp(tmp, (op == TINYJS_LEX_O_PLUSPLUS) ? '+' : '-'));	//In-place add/subtract.
				a = oldVarLink;
			}
		} else {
			ST_TinyJS_VarLink* b = term(pExec);
			if(*pExec) {
				a = new ST_TinyJS_VarLink(a->var->mathsOp(b->var, op));			//Not in-place, so just replace.
			}
		}
	}
	return a;
}
//-----------------------------------------------------------------------------
//シフト式
ST_TinyJS_VarLink* ST_TinyJS::shift(int* pExec) {
	ST_TinyJS_VarLink* a = expression(pExec);
	if((lex->tk == TINYJS_LEX_O_LSHIFT) ||
	   (lex->tk == TINYJS_LEX_O_RSHIFT) ||
	   (lex->tk == TINYJS_LEX_O_RSHIFTUNSIGNED)) {
		int op = lex->tk;
		lex->match(op);
		ST_TinyJS_VarLink* b = base(pExec);
		if(*pExec) {
			int value = a->var->getNumber();
			int shift = b->var->getNumber();
			switch(op) {
			default:DIE();	//バグ
			case TINYJS_LEX_O_LSHIFT:         value =           value << shift; break;
			case TINYJS_LEX_O_RSHIFT:         value =           value >> shift; break;
			case TINYJS_LEX_O_RSHIFTUNSIGNED: value = (unsigned)value >> shift; break;
			}
			a = new ST_TinyJS_VarLink(ST_TinyJS_Var::newNumber(value));
		}
	}
	return a;
}
//-----------------------------------------------------------------------------
//条件式
ST_TinyJS_VarLink* ST_TinyJS::condition(int* pExec) {
	ST_TinyJS_VarLink* a = shift(pExec);
	while((lex->tk == '<') ||
	      (lex->tk == '>') ||
	      (lex->tk == TINYJS_LEX_O_LEQUAL) ||	//"<="
	      (lex->tk == TINYJS_LEX_O_GEQUAL) ||	//">="
	      (lex->tk == TINYJS_LEX_O_EQUAL) ||	//"=="
	      (lex->tk == TINYJS_LEX_O_NEQUAL) ||	//"!="
	      (lex->tk == TINYJS_LEX_O_TYPEEQUAL) ||	//"==="
	      (lex->tk == TINYJS_LEX_O_NTYPEEQUAL)) {	//"!=="
		int op = lex->tk;
		lex->match(op);
		ST_TinyJS_VarLink* b = shift(pExec);
		if(*pExec) {
			a = new ST_TinyJS_VarLink(a->var->mathsOp(b->var, op));
		}
	}
	return a;
}
//-----------------------------------------------------------------------------
//論理式
ST_TinyJS_VarLink* ST_TinyJS::logic(int* pExec) {
	ST_TinyJS_VarLink* a = condition(pExec);
	while((lex->tk == '&') ||
	      (lex->tk == '|') ||
	      (lex->tk == '^') ||
	      (lex->tk == TINYJS_LEX_O_ANDAND) ||	//"&&"
	      (lex->tk == TINYJS_LEX_O_OROR)) {		//"||"
		int op = lex->tk;
		lex->match(op);
		//If we have short-circuit ops, then if we know the outcome we don't bother to execute the other op.
		//Even if not we need to tell mathsOp it's an & or |.
		int shortCircuit, isBoolean;
		if(op == TINYJS_LEX_O_ANDAND) {		//"&&"
			op = '&';
			shortCircuit = !a->var->getBoolean();
			isBoolean    = 1;
		} else if(op == TINYJS_LEX_O_OROR) {	//"||"
			op = '|';
			shortCircuit =  a->var->getBoolean();
			isBoolean    = 1;
		} else {
			shortCircuit = 0;
			isBoolean    = 0;
		}
		ST_TinyJS_VarLink* b = condition(shortCircuit ? &noexec : pExec);
		if(*pExec) {
			if(!shortCircuit) {
				if(isBoolean) {
					a = new ST_TinyJS_VarLink(ST_TinyJS_Var::newNumber(a->var->getBoolean()));
					b = new ST_TinyJS_VarLink(ST_TinyJS_Var::newNumber(b->var->getBoolean()));
				}
				a = new ST_TinyJS_VarLink(a->var->mathsOp(b->var, op));
			}
		}
	}
	return a;
}
//-----------------------------------------------------------------------------
//三項演算子
ST_TinyJS_VarLink* ST_TinyJS::ternary(int* pExec) {
	ST_TinyJS_VarLink* a = logic(pExec);
	if(lex->tk == '?') {
		lex->match('?');
		if(a->var->getBoolean()) {
			a = base(pExec);
			lex->match(':');
			base(&noexec);
		} else {
			base(&noexec);
			lex->match(':');
			a = base(pExec);
		}
	}
	return a;
}
//-----------------------------------------------------------------------------
//代入式
ST_TinyJS_VarLink* ST_TinyJS::base(int* pExec) {
	ST_TinyJS_VarLink* lhs = ternary(pExec);
	if((lex->tk == '=') ||
	   (lex->tk == TINYJS_LEX_O_PLUSASSIGN) ||
	   (lex->tk == TINYJS_LEX_O_MINUSASSIGN)) {
		//If we're assigning to this and we don't have a prototype, add it to the symbol table root as per JavaScript.
		if(*pExec) {
			if(!lhs->owned) {
				if(!strcmp(lhs->name, "")) {
					throw new ST_TinyJS_Exception("Trying to assign to an un-named type.");
				}
				lhs = root->addChild(lhs->name, lhs->var);
			}
		}
		int op = lex->tk;
		lex->match(op);
		ST_TinyJS_VarLink* rhs = base(pExec);
		if(*pExec) {
			switch(op) {
			default:DIE();	//バグ
			case '=':                      lhs->replaceWith(                  rhs->var      ); break;
			case TINYJS_LEX_O_PLUSASSIGN:  lhs->replaceWith(lhs->var->mathsOp(rhs->var, '+')); break;
			case TINYJS_LEX_O_MINUSASSIGN: lhs->replaceWith(lhs->var->mathsOp(rhs->var, '-')); break;
			}
		}
	}
	return lhs;
}
//-----------------------------------------------------------------------------
//「{～}」
void ST_TinyJS::block(int* pExec) {
	lex->match('{');
	while(lex->tk && (lex->tk != '}')) {
		statement(pExec);
	}
	lex->match('}');
}
//-----------------------------------------------------------------------------
//文
void ST_TinyJS::statement(int* pExec) {
	ST_TinyJS_Var* scope = (ST_TinyJS_Var*)scopes->data;
	if((lex->tk == '-') ||
	   (lex->tk == TINYJS_LEX_ID) ||
	   (lex->tk == TINYJS_LEX_L_NUM) ||
	   (lex->tk == TINYJS_LEX_L_STR)) {
		//Execute a simple statement that only contains basic arithmetic...
		base(pExec);
		lex->match(';');
	} else if(lex->tk == '{') {
		//A block of code.
		block(pExec);
	} else if(lex->tk == ';') {
		//Empty statement - to allow things like ;;;.
		lex->match(';');
	} else if(lex->tk == TINYJS_LEX_R_VAR) {
		//Variable creation.
		//TODO - We need a better way of parsing the left hand side.
		//       Maybe just have a flag called can_create_var that we set and then we parse as if we're doing a normal equals.
		lex->match(TINYJS_LEX_R_VAR);
		do {
			ST_TinyJS_VarLink* l = NULL;
			if(*pExec) {
				l = scope->findChildOrCreate(lex->tkStr);
			}
			lex->match(TINYJS_LEX_ID);
			//Now do stuff defined with dots.
			while(lex->tk == '.') {
				lex->match('.');
				if(*pExec) {
					l = l->var->findChildOrCreate(lex->tkStr);
				}
				lex->match(TINYJS_LEX_ID);
			}
			//Sort out initialiser.
			if(lex->tk == '=') {
				lex->match('=');
				ST_TinyJS_VarLink* tmp = base(pExec);
				if(*pExec) {
					l->replaceWith(tmp->var);
				}
			}
			if(lex->tk != ';') { lex->match(','); }
		} while(lex->tk != ';');
		lex->match(';');
	} else if(lex->tk == TINYJS_LEX_R_IF) {
		lex->match(TINYJS_LEX_R_IF);
		lex->match('(');
		ST_TinyJS_VarLink* cond = base(pExec);				//Condition.
		lex->match(')');
		statement(cond->var->getBoolean() ? pExec : &noexec);		//If body.
		if(lex->tk == TINYJS_LEX_R_ELSE) {
			lex->match(TINYJS_LEX_R_ELSE);
			statement(cond->var->getBoolean() ? &noexec : pExec);	//Else body.
		}
	} else if(lex->tk == TINYJS_LEX_R_WHILE) {
		lex->match(TINYJS_LEX_R_WHILE);
		lex->match('(');
		int whileCondStart = lex->tokenStart;
		base(&noexec);		//Condition.
		ST_TinyJS_Lex* whileCond = lex->getSubLex(whileCondStart);
		lex->match(')');
		int whileBodyStart = lex->tokenStart;
		statement(&noexec);	//Body.
		ST_TinyJS_Lex* whileBody = lex->getSubLex(whileBodyStart);
		ST_TinyJS_Context* pSavedContext = saveContext();
		if(*pExec) {
			for(;;) {
				lex = whileCond->reset();
				ST_TinyJS_VarLink* cond = base(pExec);	//Condition.
				if(!cond->var->getBoolean()) { break; }
				lex = whileBody->reset();		//Body.
				statement(pExec);
			}
		}
		restoreContext(pSavedContext);
	} else if(lex->tk == TINYJS_LEX_R_FOR) {
		lex->match(TINYJS_LEX_R_FOR);
		lex->match('(');
		statement(pExec);	//Initialisation.
		int forCondStart = lex->tokenStart;
		base(&noexec);		//Condition.
		ST_TinyJS_Lex* forCond = lex->getSubLex(forCondStart);
		lex->match(';');
		int forIterStart = lex->tokenStart;
		base(&noexec);		//Iterator.
		ST_TinyJS_Lex* forIter = lex->getSubLex(forIterStart);
		lex->match(')');
		int forBodyStart = lex->tokenStart;
		statement(&noexec);	//Body.
		ST_TinyJS_Lex* forBody = lex->getSubLex(forBodyStart);
		ST_TinyJS_Context* pSavedContext = saveContext();
		if(*pExec) {
			for(;;) {
				lex = forCond->reset();
				ST_TinyJS_VarLink* cond = base(pExec);	//Condition.
				if(!cond->var->getBoolean()) { break; }
				lex = forBody->reset();
				statement(pExec);			//Body.
				lex = forIter->reset();
				base(pExec);				//Iterator.
			}
		}
		restoreContext(pSavedContext);
	} else if(lex->tk == TINYJS_LEX_R_RETURN) {
		lex->match(TINYJS_LEX_R_RETURN);
		ST_TinyJS_VarLink* result = NULL;
		if(lex->tk != ';') {
			result = base(pExec);
		}
		if(*pExec) {
			if(result) {
				scope->setReturnVar(result->var);
			}
			*pExec = 0;	//return文以降は実行せずにパースだけを行う。
		}
		lex->match(';');
	} else if(lex->tk == TINYJS_LEX_R_FUNCTION) {
		ST_TinyJS_VarLink* funcDef = parseFunctionDefinition();
		if(*pExec) {
			if(!strcmp(funcDef->name, "")) {
				throw new ST_TinyJS_Exception("Functions defined at statement-level are meant to have a name.");
			}
			scope->addChild(funcDef->name, funcDef->var);
		}
	} else {
		lex->match(TINYJS_LEX_EOF);
	}
}
//-----------------------------------------------------------------------------
//「function 関数名(仮引数名,...){～}」,又は,「function(仮引数名,...){～}」
ST_TinyJS_VarLink* ST_TinyJS::parseFunctionDefinition() {
	lex->match(TINYJS_LEX_R_FUNCTION);
	const char* funcName = "";
	if(lex->tk == TINYJS_LEX_ID) {	//We can have functions without names.
		funcName = lex->tkStr;
		lex->match(TINYJS_LEX_ID);
	}
	ST_TinyJS_Var* funcVar = ST_TinyJS_Var::newFunction();
	parseFunctionArguments(funcVar);
	int funcBegin = lex->tokenStart;
	block(&noexec);
	funcVar->strData = lex->getSubString(funcBegin);
	ST_TinyJS_VarLink* l = new ST_TinyJS_VarLink(funcVar);
	l->name = funcName;
	return l;
}
//-----------------------------------------------------------------------------
//「(仮引数名,...)」
void ST_TinyJS::parseFunctionArguments(ST_TinyJS_Var* funcVar) {
	lex->match('(');
	while(lex->tk != ')') {
		funcVar->addChild(lex->tkStr, ST_TinyJS_Var::newUndefined());
		lex->match(TINYJS_LEX_ID);
		if(lex->tk != ')') { lex->match(','); }
	}
	lex->match(')');
}
//-----------------------------------------------------------------------------
//Finds a child, looking recursively up the scopes.
ST_TinyJS_VarLink* ST_TinyJS::findInScopes(const char* name) {
	GSList/*<ST_TinyJS_Var*>*/* list = scopes;
	while(list) {
		ST_TinyJS_Var* scope = (ST_TinyJS_Var*)list->data;
		ST_TinyJS_VarLink* l = scope->findChild(name);
		if(l) { return l; }
		list = list->next;
	}
	return NULL;
}
//-----------------------------------------------------------------------------
//Look up in any prototype classes of the given object.
ST_TinyJS_VarLink* ST_TinyJS::findInPrototypeClasses(ST_TinyJS_Var* v, const char* name) {
	//Look for links to actual prototype classes.
	ST_TinyJS_VarLink* prototypeClass = v->findChild("prototype");
	while(prototypeClass) {
		ST_TinyJS_VarLink* implementation = prototypeClass->var->findChild(name);
		if(implementation) { return implementation; }
		prototypeClass = prototypeClass->var->findChild("prototype");
	}
	//else fake it for strings and finally objects.
	if(v->isString()) {
		ST_TinyJS_VarLink* implementation = stringClass->findChild(name);
		if(implementation) { return implementation; }
	}
	if(v->isArray()) {
		ST_TinyJS_VarLink* implementation = arrayClass->findChild(name);
		if(implementation) { return implementation; }
	}
	return objectClass->findChild(name);	//may be NULL
}
//-----------------------------------------------------------------------------
ST_TinyJS_Context* ST_TinyJS::saveContext() {
	ST_TinyJS_Context* pSavedContext = (ST_TinyJS_Context*)malloc(sizeof(ST_TinyJS_Context));
	pSavedContext->lex       = lex;
	pSavedContext->scopes    = scopes;
	pSavedContext->callStack = callStack;
	return pSavedContext;
}
//-----------------------------------------------------------------------------
void ST_TinyJS::restoreContext(ST_TinyJS_Context* pSavedContext) {
	lex       = pSavedContext->lex;
	scopes    = pSavedContext->scopes;
	callStack = pSavedContext->callStack;
}
//-----------------------------------------------------------------------------
const char* ST_TinyJS::stackTrace(const char* errMsg) {
	GString* buf = g_string_new(NULL);
	g_string_append(buf, "Error ");
	g_string_append(buf, errMsg);
	{
		GSList/*<const char*>*/* list = callStack;
		int i = 0;
		while(list) {
			g_string_append(buf, "\n");
			g_string_append_printf(buf, "%d", i);
			g_string_append(buf, ": ");
			g_string_append(buf, (const char*)list->data);
			i++;
		}
	}
	g_string_append(buf, " at ");
	g_string_append(buf, lex->getLastPosition());
	return buf->str;
}
