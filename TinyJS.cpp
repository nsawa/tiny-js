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
//-----------------------------------------------------------------------------
//strtod()は10進実数と16進整数を解釈するが、8進整数を解釈しないので、8進整数のみ明示的に変換する事にした。
static double strtonum(const char* varData) {
	double result;
	char* endptr;
	if((varData[0] == '0') && (strspn(varData, "01234567") == strlen(varData))) {
		result = strtoul(varData, &endptr, 8);
	} else {
		result = strtod(varData, &endptr);
	}
	if(*endptr) { DIE(); }	//バグ
	return result;
}
//*****************************************************************************
//	ST_TinyJS_Exception
//*****************************************************************************
ST_TinyJS_Exception::ST_TinyJS_Exception(const char* exceptionText) {
	text = exceptionText;
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
void ST_TinyJS_Lex::reset() {
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
	//	　││└        dataPos
	//	　│└─nextCh: dataPos - 1
	//	　└──currCh: dataPos - 2 = dataStart
	//
	getNextToken();
}
//-----------------------------------------------------------------------------
//Lexical match wotsit.
void ST_TinyJS_Lex::match(int expected_tk) {
	if(tk != expected_tk) {
		GString* errorString = g_string_new(NULL);
		g_string_append(errorString, "Got ");
		g_string_append(errorString, getTokenStr(tk));
		g_string_append(errorString, " expected ");
		g_string_append(errorString, getTokenStr(expected_tk));
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
	case TINYJS_LEX_O_ANDEQUAL:       return "&=";
	case TINYJS_LEX_O_EQUAL:          return "==";
	case TINYJS_LEX_O_GEQUAL:         return ">=";
	case TINYJS_LEX_O_LEQUAL:         return "<=";
	case TINYJS_LEX_O_LSHIFT:         return "<<";
	case TINYJS_LEX_O_LSHIFTEQUAL:    return "<<=";
	case TINYJS_LEX_O_MINUSEQUAL:     return "-=";
	case TINYJS_LEX_O_MINUSMINUS:     return "--";
	case TINYJS_LEX_O_NEQUAL:         return "!=";
	case TINYJS_LEX_O_NTYPEEQUAL:     return "!==";
	case TINYJS_LEX_O_OREQUAL:        return "|=";
	case TINYJS_LEX_O_OROR:           return "||";
	case TINYJS_LEX_O_PLUSEQUAL:      return "+=";
	case TINYJS_LEX_O_PLUSPLUS:       return "++";
	case TINYJS_LEX_O_RSHIFT:         return ">>";
	case TINYJS_LEX_O_RSHIFTEQUAL:    return ">>=";
	case TINYJS_LEX_O_RSHIFTUNSIGNED: return ">>";
	case TINYJS_LEX_O_TYPEEQUAL:      return "===";
	case TINYJS_LEX_O_XOREQUAL:       return "^=";
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
		while(currCh && isspace(currCh)) { getNextCh(); }
		//Newline comments.
		if((currCh == '/') && (nextCh == '/')) {
			while(currCh && currCh != '\n') { getNextCh(); }
			getNextCh();	//'\n'の次へ進める。
		//Block comments.
		} else if((currCh == '/') && (nextCh == '*')) {
			while(currCh && (currCh != '*' || nextCh != '/')) { getNextCh(); }
			getNextCh();	//'*'の次へ進める。
			getNextCh();	//'/'の次へ進める。
		} else {
			break;
		}
	}
	//Record beginning of this token.
	GString* buf = g_string_new(NULL);
//不要	tk    = TINYJS_LEX_EOF;
	tkStr = "";
	tokenStart = dataPos - 2;
	//
	//	～■■□～
	//	　↑↑↑
	//	　││└         dataPos
	//	　│└─ nextCh: dataPos - 1
	//	　└── currCh: dataPos - 2 = tokenStart
	//
	//Tokens.
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
		bool isHex = false;
		if(currCh == '0') {
			g_string_append_c(buf, currCh);		//'0'を追加する。
			getNextCh();				//'0'の次へ進める。
			if(((currCh == 'x') || (currCh == 'X')) && isxdigit(nextCh)/*この条件忘れないで!!0xの後に少なくとも一文字の16進数が必要!!*/) {
				isHex = true;
				g_string_append_c(buf, currCh);	//'x',又は,'X'を追加する。
				getNextCh();			//'x',又は,'X'の次へ進める。
			}
		}
		tk = TINYJS_LEX_L_NUM;
		while(isdigit(currCh) || (isHex && isxdigit(currCh))) {
			g_string_append_c(buf, currCh);
			getNextCh();
		}
		if(!isHex && (currCh == '.')) {
			g_string_append_c(buf, currCh);		//'.'を追加する。
			getNextCh();				//'.'の次へ進める。
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
				g_string_append_c(buf, currCh);	//'-'を追加する。
				getNextCh();			//'-'の次へ進める。
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
		getNextCh();	//'"',又は,"'"の次へ進める。
		while(currCh && (currCh != quotCh)) {
			if(currCh == '\\') {
				getNextCh();	//'\'の次へ進める。
				switch(currCh) {
				case 't':  g_string_append_c(buf, '\t'); break;	//┐
				case 'n':  g_string_append_c(buf, '\n'); break;	//├頻繁に使う制御文字のエスケープシーケンスだけ対応した。頻繁に使わない制御文字はその他の印字不可文字と共通で8進数形式,又は,16進数形式での入力を前提とする。もし不足ならば追加しろ。
				case 'r':  g_string_append_c(buf, '\r'); break;	//┘
				case '\\': g_string_append_c(buf, '\\'); break;
				default:
					//Hex digits.
					if(((currCh == 'x') || (currCh == 'X')) && isxdigit(nextCh)/*この条件忘れないで!!\xの後に少なくとも一文字の16進数が必要!!*/) {
						GString* tmp = g_string_new(NULL);
						for(;;) {		//C言語の文字列リテラルの仕様では、"\x????～"には桁数制限が無い。JavaScriptでも同じなのかわからない。(※TODO:要確認)
							getNextCh();	//初回は'x',又は,'X'の次へ進める。
							if(!isxdigit(currCh)) { break; }
							g_string_append_c(tmp, currCh);
						}
						g_string_append_c(buf, (char)strtoul(tmp->str, NULL, 16));
					//Octal digits.
					} else if((currCh >= '0') && (currCh <= '7')) {
						GString* tmp = g_string_new(NULL);
						while(tmp->len < 3) {	//C言語の文字列リテラルの仕様では、"\???"は1～3桁である。JavaScriptでも同じなのかわからない。(※TODO:要確認)
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
			if((tk == '=') && (currCh == '=')) {		//"=="
				tk = TINYJS_LEX_O_EQUAL;
				getNextCh();
				if(currCh == '=') {			//"==="
					tk = TINYJS_LEX_O_TYPEEQUAL;
					getNextCh();
				}
			} else if((tk == '!') && (currCh == '=')) {	//"!="
				tk = TINYJS_LEX_O_NEQUAL;
				getNextCh();
				if(currCh == '=') {			//"!=="
					tk = TINYJS_LEX_O_NTYPEEQUAL;
					getNextCh();
				}
			} else if((tk == '<') && (currCh == '=')) {	//"<="
				tk = TINYJS_LEX_O_LEQUAL;
				getNextCh();
			} else if((tk == '<') && (currCh == '<')) {	//"<<"
				tk = TINYJS_LEX_O_LSHIFT;
				getNextCh();
				if(currCh == '=') {			//"<<="
					tk = TINYJS_LEX_O_LSHIFTEQUAL;
					getNextCh();
				}
			} else if(tk == '>' && currCh == '=') {		//">="
				tk = TINYJS_LEX_O_GEQUAL;
				getNextCh();
			} else if(tk == '>' && currCh == '>') {		//">>"
				tk = TINYJS_LEX_O_RSHIFT;
				getNextCh();
				if(currCh == '=') {			//">>="
					tk = TINYJS_LEX_O_RSHIFTEQUAL;
					getNextCh();
				} else if(currCh == '>') {		//">>>"
					tk = TINYJS_LEX_O_RSHIFTUNSIGNED;
					getNextCh();
				}
			} else if(tk == '+' && currCh == '=') {		//"+="
				tk = TINYJS_LEX_O_PLUSEQUAL;
				getNextCh();
			} else if(tk == '-' && currCh == '=') {		//"-="
				tk = TINYJS_LEX_O_MINUSEQUAL;
				getNextCh();
			} else if(tk == '+' && currCh == '+') {		//"++"
				tk = TINYJS_LEX_O_PLUSPLUS;
				getNextCh();
			} else if(tk == '-' && currCh == '-') {		//"--"
				tk = TINYJS_LEX_O_MINUSMINUS;
				getNextCh();
			} else if(tk == '&' && currCh == '=') {		//"&="
				tk = TINYJS_LEX_O_ANDEQUAL;
				getNextCh();
			} else if(tk == '&' && currCh == '&') {		//"&&"
				tk = TINYJS_LEX_O_ANDAND;
				getNextCh();
			} else if(tk == '|' && currCh == '=') {		//"|="
				tk = TINYJS_LEX_O_OREQUAL;
				getNextCh();
			} else if(tk == '|' && currCh == '|') {		//"||"
				tk = TINYJS_LEX_O_OROR;
				getNextCh();
			} else if(tk == '^' && currCh == '=') {		//"^="
				tk = TINYJS_LEX_O_XOREQUAL;
				getNextCh();
			} else {
				/** no job **/				//その他の一文字演算子,又は,不正な文字
			}
		} else {
			/** no job **/					//EOF
		}
	}
	//This isn't quite right yet.
	tokenLastEnd = tokenEnd;
	//Record ending of this token.
	tokenEnd = dataPos - 3;
	//
	//	～■□□□～
	//	　↑↑↑↑
	//	　│││└         dataPos
	//	　││└─ nextCh: dataPos - 1
	//	　│└── currCh: dataPos - 2
	//	　└───         dataPos - 3 = tokenEnd	tokenEndはトークンの最後の文字の位置を示す。トークンの最後の文字の次の位置ではない事に注意せよ。
	//
	tkStr = buf->str;	//忘れないで!!
}
//-----------------------------------------------------------------------------
//Return a sub-string from the given position up until right now.
const char* ST_TinyJS_Lex::getSubString(int lastPosition) {
	assert(lastPosition <= dataEnd);
	int lastCharIdx = tokenLastEnd + 1;	//一つ前に取得したトークンの最後の文字の次の位置。
	if(lastCharIdx <= dataEnd) {
		return strndup(data + lastPosition, lastCharIdx - lastPosition);
	} else {
		return strndup(data + lastPosition,     dataEnd - lastPosition);	//※こちらになる事は起こり得ないのではないだろうか?(※TODO:要検討)
	}
}
//-----------------------------------------------------------------------------
//Return a sub-lexer from the given position up until right now.
ST_TinyJS_Lex* ST_TinyJS_Lex::getSubLex(int lastPosition) {
	assert(lastPosition <= dataEnd);
	int lastCharIdx = tokenLastEnd + 1;	//一つ前に取得したトークンの最後の文字の次の位置。
	if(lastCharIdx <= dataEnd) {
		return new ST_TinyJS_Lex(data, lastPosition, lastCharIdx);
	} else {
		return new ST_TinyJS_Lex(data, lastPosition,     dataEnd);		//※こちらになる事は起こり得ないのではないだろうか?(※TODO:要検討)
	}
}
//-----------------------------------------------------------------------------
//Return a string representing the position in lines and columns of the tokenLastEnd.
const char* ST_TinyJS_Lex::getLastPosition() {
	return ST_TinyJS_Lex::getPosition(tokenLastEnd);
}
//-----------------------------------------------------------------------------
//Return a string representing the position in lines and columns of the character pos given.
const char* ST_TinyJS_Lex::getPosition(int pos) {
	if(pos > dataEnd) { pos = dataEnd; }	//※起こり得ないのではないだろうか?(※TODO:要検討)
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
ST_TinyJS_VarLink::ST_TinyJS_VarLink(ST_TinyJS_Var* v, const char* name) {
	this->name  = name;
	this->var   = v;
	this->owned = false;
}
//-----------------------------------------------------------------------------
//Copy constructor.
ST_TinyJS_VarLink::ST_TinyJS_VarLink(const ST_TinyJS_VarLink& l) {
	this->name  = l.name;
	this->var   = l.var;
	this->owned = false;
}
//-----------------------------------------------------------------------------
//Replace the Variable pointed to.
void ST_TinyJS_VarLink::replaceWith(ST_TinyJS_Var* v) {
	var = v;
}
//-----------------------------------------------------------------------------
//Replace the Variable pointed to (just dereferences).
void ST_TinyJS_VarLink::replaceWith(ST_TinyJS_VarLink* l) {
	if(l) {
		replaceWith(l->var);
	} else {
		replaceWith(new ST_TinyJS_Var());	//※(l=NULL)で呼び出される事は起こり得るのだろうか?(※TODO:要確認)
	}
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
//Create undefined.
ST_TinyJS_Var::ST_TinyJS_Var() {
	init();
	type = TINYJS_VAR_UNDEFINED;
}
//-----------------------------------------------------------------------------
//User defined.
ST_TinyJS_Var::ST_TinyJS_Var(const char* varData, int varType) {
	init();
	type = varType;
	if(varType == TINYJS_VAR_NUMBER) {
		numData = strtonum(varData);
	} else {
		strData = varData;
	}
}
//-----------------------------------------------------------------------------
//Create a string.
ST_TinyJS_Var::ST_TinyJS_Var(const char* str) {
	init();
	type = TINYJS_VAR_STRING;
	strData = str;
}
//-----------------------------------------------------------------------------
ST_TinyJS_Var::ST_TinyJS_Var(double val) {
	init();
	type = TINYJS_VAR_NUMBER;
	numData = val;
}
//-----------------------------------------------------------------------------
void ST_TinyJS_Var::init() {
	type       = 0;
	numData    = 0;
	strData    = TINYJS_BLANK_DATA;
	callback   = NULL;
	userdata   = NULL;
	firstChild = NULL;
}
//-----------------------------------------------------------------------------
//If this is a function, get the result value (for use by native functions).
ST_TinyJS_Var* ST_TinyJS_Var::getReturnVar() {
	return getParameter(TINYJS_RETURN_VAR);
}
//-----------------------------------------------------------------------------
//Set the result value.
//Use this when setting complex return data as it avoids a deepCopy().
void ST_TinyJS_Var::setReturnVar(ST_TinyJS_Var* v) {
	findChildOrCreate(TINYJS_RETURN_VAR)->replaceWith(v);
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
ST_TinyJS_VarLink* ST_TinyJS_Var::findChildOrCreate(const char* name, int varType) {
	ST_TinyJS_VarLink* l = findChild(name);
	if(l) { return l; }
	return addChild(name, new ST_TinyJS_Var(TINYJS_BLANK_DATA, varType));
}
//-----------------------------------------------------------------------------
//Tries to find a child with the given path (separated by dots).
ST_TinyJS_VarLink* ST_TinyJS_Var::findChildOrCreateByPath(const char* path) {
	const char* p = strchr(path, '.');
	if(p) {
		return findChildOrCreate(strndup(path, p - path), TINYJS_VAR_OBJECT)->var->findChildOrCreateByPath(p + 1/*'.'*/);
	} else {
		return findChildOrCreate(path);
	}
}
//-----------------------------------------------------------------------------
ST_TinyJS_VarLink* ST_TinyJS_Var::addChild(const char* name, ST_TinyJS_Var* v) {
	if(!v) { v = new ST_TinyJS_Var(); }	//If no child supplied, create one.
	ST_TinyJS_VarLink* l = findChild(name);
	if(l) {
		l->replaceWith(v);
	} else {
		if(isUndefined()) { type = TINYJS_VAR_OBJECT; }
		l = new ST_TinyJS_VarLink(v, name);
		l->owned = true;
		firstChild = g_slist_append(firstChild, l);
	}
	return l;
}
//-----------------------------------------------------------------------------
void ST_TinyJS_Var::removeChild(ST_TinyJS_Var* v) {
	GSList/*<ST_TinyJS_VarLink*>*/* list = firstChild;
	while(list) {
		ST_TinyJS_VarLink* l = (ST_TinyJS_VarLink*)list->data;
		if(l->var == v) {
			removeLink(l);
			break;
		}
		list = list->next;
	}
}
//-----------------------------------------------------------------------------
//Remove a specific link (this is faster than finding via a child).
void ST_TinyJS_Var::removeLink(ST_TinyJS_VarLink* l) {
	firstChild = g_slist_remove(firstChild, l);
}
//-----------------------------------------------------------------------------
void ST_TinyJS_Var::removeAllChildren() {
	while(firstChild) {
		ST_TinyJS_VarLink* l = (ST_TinyJS_VarLink*)firstChild->data;
		removeLink(l);
	}
}
//-----------------------------------------------------------------------------
//The the value at an array index.
ST_TinyJS_Var* ST_TinyJS_Var::getArrayIndex(int i) {
	const char* buf = strdup_printf("%d", i);
	ST_TinyJS_VarLink* l = findChild(buf);
	if(l) {
		return l->var;
	} else {
		return new ST_TinyJS_Var();	//undefined
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
//Get the number of children.
int ST_TinyJS_Var::getChildCount() {
	return g_slist_length(firstChild);
}
//-----------------------------------------------------------------------------
bool ST_TinyJS_Var::getBool() {
	return getNumber() != 0;	//※色々問題が有る気がする。getInt()がオブジェクトや文字列に対して0を返し、getBool()でfalseになってしまう。せめてオブジェクトや空でない文字列はtrueと判断すべきではないか?(※TODO:要検討)
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
	return strData;	//※オブジェクトや配列の文字列化は未対応なのか?(※TODO:要検討)
}
//-----------------------------------------------------------------------------
void ST_TinyJS_Var::setNumber(double val) {
	type    = TINYJS_VAR_NUMBER;
	numData = val;
	strData = TINYJS_BLANK_DATA;
	removeAllChildren();	//※元ソースではこの処理は無かったが、プリミティブと見なすならば必要では?(※要検討)
}
//-----------------------------------------------------------------------------
void ST_TinyJS_Var::setString(const char* str) {
	//Name sure it's not still a number or integer.
	type    = TINYJS_VAR_STRING;
	numData = 0;
	strData = str;
	removeAllChildren();	//※元ソースではこの処理は無かったが、プリミティブと見なすならば必要では?(※要検討)
}
//-----------------------------------------------------------------------------
void ST_TinyJS_Var::setUndefined() {
	//Name sure it's not still a number or integer.
	type    = TINYJS_VAR_UNDEFINED;
	numData = 0;
	strData = TINYJS_BLANK_DATA;
	removeAllChildren();
}
//-----------------------------------------------------------------------------
void ST_TinyJS_Var::setArray() {
	//Name sure it's not still a number or integer.
	type    = TINYJS_VAR_ARRAY;
	numData = 0;
	strData = TINYJS_BLANK_DATA;
	removeAllChildren();	//※なぜ!?配列のクリアも兼ねるのか!?(※要検討)
}
//-----------------------------------------------------------------------------
bool ST_TinyJS_Var::equals(ST_TinyJS_Var* v) {
	ST_TinyJS_Var* tmp = mathsOp(v, TINYJS_LEX_O_EQUAL);
	return tmp->getBool();
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
		bool eql = (a->type == b->type);
		if(eql) {
			ST_TinyJS_Var* tmp = a->mathsOp(b, TINYJS_LEX_O_EQUAL);
			if(!tmp->getBool()) { eql = false; }
		}
		if(op == TINYJS_LEX_O_NTYPEEQUAL) { eql = !eql; }
		return new ST_TinyJS_Var(eql);
	}
	//Do maths...
	if((a->isUndefined() || a->isNull()) && (b->isUndefined() || b->isNull())) {
		if(op == TINYJS_LEX_O_EQUAL) {
			return new ST_TinyJS_Var(1.0);	//(undefined==null)もtrueです。(本物のJavaScriptでも)
		} else if(op == TINYJS_LEX_O_NEQUAL) {
			return new ST_TinyJS_Var(0.0);	//(undefined!=null)もfalseです。(本物のJavaScriptでも)
		} else {
			return new ST_TinyJS_Var();	//undefined	※本当は違う。(undefined+null)⇒NaNや(undefined|null)⇒0など。とは言え実際にそれらの挙動に依存するようなスクリプトは書かないと思う。(※TODO:要検討)
		}
	} else if(a->isNumber() && b->isNumber()) {
		double da = a->getNumber();
		double db = b->getNumber();
		switch(op) {
		case '+':                 return new ST_TinyJS_Var(     da +      db);
		case '-':                 return new ST_TinyJS_Var(     da -      db);
		case '*':                 return new ST_TinyJS_Var(     da *      db);
		case '/':                 return new ST_TinyJS_Var(     da /      db);
		case '&':                 return new ST_TinyJS_Var((int)da & (int)db);
		case '|':                 return new ST_TinyJS_Var((int)da | (int)db);
		case '^':                 return new ST_TinyJS_Var((int)da ^ (int)db);
		case '%':                 return new ST_TinyJS_Var(fmod(da,       db));
		case TINYJS_LEX_O_EQUAL:  return new ST_TinyJS_Var(     da ==     db);
		case TINYJS_LEX_O_NEQUAL: return new ST_TinyJS_Var(     da !=     db);
		case '<':                 return new ST_TinyJS_Var(     da <      db);
		case TINYJS_LEX_O_LEQUAL: return new ST_TinyJS_Var(     da <=     db);
		case '>':                 return new ST_TinyJS_Var(     da >      db);
		case TINYJS_LEX_O_GEQUAL: return new ST_TinyJS_Var(     da >=     db);
		default:throw new ST_TinyJS_Exception(strdup_printf("Operation %s not supported on the number datatype.", ST_TinyJS_Lex::getTokenStr(op)));
		}
	} else if(a->isArray()) {
		//Just check pointers.
		switch(op) {
		case TINYJS_LEX_O_EQUAL:  return new ST_TinyJS_Var(a == b);
		case TINYJS_LEX_O_NEQUAL: return new ST_TinyJS_Var(a != b);
		default:throw new ST_TinyJS_Exception(strdup_printf("Operation %s not supported on the Array datatype.", ST_TinyJS_Lex::getTokenStr(op)));
		}
	} else if(a->isObject()) {
		//Just check pointers.
		switch(op) {
		case TINYJS_LEX_O_EQUAL:  return new ST_TinyJS_Var(a == b);
		case TINYJS_LEX_O_NEQUAL: return new ST_TinyJS_Var(a != b);
		default:throw new ST_TinyJS_Exception(strdup_printf("Operation %s not supported on the Object datatype.", ST_TinyJS_Lex::getTokenStr(op)));
		}
	} else {
		const char* da = a->getString();
		const char* db = b->getString();
		//Use strings.
		switch(op) {
		case '+':                 return new ST_TinyJS_Var(strconcat(da, db, NULL), TINYJS_VAR_STRING);
		case TINYJS_LEX_O_EQUAL:  return new ST_TinyJS_Var(strcmp(da, db) == 0);
		case TINYJS_LEX_O_NEQUAL: return new ST_TinyJS_Var(strcmp(da, db) != 0);
		case '<':                 return new ST_TinyJS_Var(strcmp(da, db) <  0);
		case TINYJS_LEX_O_LEQUAL: return new ST_TinyJS_Var(strcmp(da, db) <= 0);
		case '>':                 return new ST_TinyJS_Var(strcmp(da, db) >  0);
		case TINYJS_LEX_O_GEQUAL: return new ST_TinyJS_Var(strcmp(da, db) >= 0);
		default:throw new ST_TinyJS_Exception(strdup_printf("Operation %s not supported on the string datatype.", ST_TinyJS_Lex::getTokenStr(op)));
		}
	}
}
//-----------------------------------------------------------------------------
//Copy the basic data and type from the variable given, with no children.
//Should be used internally only - by copyValue and deepCopy.
void ST_TinyJS_Var::copySimpleData(ST_TinyJS_Var* v) {
	type    = v->type;
	numData = v->numData;
	strData = v->strData;
}
//-----------------------------------------------------------------------------
//Copy the value from the value given.
void ST_TinyJS_Var::copyValue(ST_TinyJS_Var* v) {	//※TinyJS_Functions.cppのObject.clone()で使用されているが、本物のJavaScriptにObject.clone()は無いのでは?(※TODO:要検討)
	if(v) {
		copySimpleData(v);
		//Remove all current children.
		removeAllChildren();
		//Copy children of 'v'.
		GSList/*<ST_TinyJS_VarLink*>*/* list = v->firstChild;
		while(list) {
			ST_TinyJS_VarLink* l = (ST_TinyJS_VarLink*)list->data;
			ST_TinyJS_Var* tmp;
			//Don't copy the 'parent' object...
			if(!strcmp(l->name, TINYJS_PROTOTYPE_CLASS)) {	//TINYJS_PROTOTYPE_CLASSならば…
				tmp = l->var;
			} else {						//TINYJS_PROTOTYPE_CLASS以外ならば…
				tmp = l->var->deepCopy();
			}
			addChild(l->name, tmp);
			list = list->next;
		}
	} else {
		setUndefined();
	}
}
//-----------------------------------------------------------------------------
//Deep copy this node and return the result.
ST_TinyJS_Var* ST_TinyJS_Var::deepCopy() {
	ST_TinyJS_Var* v = new ST_TinyJS_Var();
	v->copyValue(this);
	return v;
}
//-----------------------------------------------------------------------------
//Dump out the contents of this using trace.
void ST_TinyJS_Var::trace(const char* indent, const char* name) {
	TRACE("%s'%s' = '%s' %s\n",
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
//-----------------------------------------------------------------------------
//Set the callback for native functions.
void ST_TinyJS_Var::setCallback(TinyJS_Callback* callback, void* userdata) {
	this->callback = callback;
	this->userdata = userdata;
}
//*****************************************************************************
//	ST_TinyJS
//*****************************************************************************
ST_TinyJS::ST_TinyJS() {
	lex         = NULL;
	scopes      = NULL;
	call_stack  = NULL;
	root        = (new ST_TinyJS_Var(TINYJS_BLANK_DATA, TINYJS_VAR_OBJECT));
	//Add built-in classes.
	stringClass = (new ST_TinyJS_Var(TINYJS_BLANK_DATA, TINYJS_VAR_OBJECT));
	arrayClass  = (new ST_TinyJS_Var(TINYJS_BLANK_DATA, TINYJS_VAR_OBJECT));
	objectClass = (new ST_TinyJS_Var(TINYJS_BLANK_DATA, TINYJS_VAR_OBJECT));
	root->addChild("String", stringClass);
	root->addChild("Array",  arrayClass);
	root->addChild("Object", objectClass);
}
//-----------------------------------------------------------------------------
void ST_TinyJS::trace(const char* indent) {
	root->trace(indent, "root");
}
//-----------------------------------------------------------------------------
void ST_TinyJS::execute(const char* code) {
	//{{SaveContext
	ST_TinyJS_Lex* save_lex                  = lex;
	GSList/*<ST_TinyJS_Var*>*/* save_scopes  = scopes;
	GSList/*<const char*>*/* save_call_stack = call_stack;
	//}}SaveContext
	{
		//{{InitContext
		lex        = new ST_TinyJS_Lex(code, 0, strlen(code));
		scopes     = g_slist_prepend(NULL, root);
		call_stack = NULL;
		//}}InitContext
		try {
			bool bExec = true;
			while(lex->tk) { statement(&bExec); }
		} catch(ST_TinyJS_Exception* e) {
			const char* msg = stack_trace(e);
			//{{RestoreContext
			lex        = save_lex;
			scopes     = save_scopes;
			call_stack = save_call_stack;
			//}}RestoreContext
			throw new ST_TinyJS_Exception(msg);
		}
		if(!scopes || (scopes->data != root) || g_slist_delete_link(scopes, scopes)) { DIE(); }	//[root]に戻っているはずであり、rootを取り除けば[]になるはず。
		if(call_stack) { DIE(); }			//[]に戻っているはず。
	}
	//{{RestoreContext
	lex        = save_lex;
	scopes     = save_scopes;
	call_stack = save_call_stack;
	//}}RestoreContext
}
//-----------------------------------------------------------------------------
//Evaluate the given code and return a link to a javascript object, useful for(dangerous) JSON parsing.
//If nothing to return, will return 'undefined' variable type.
//ST_TinyJS_VarLink is returned as this will automatically unref the result as it goes out of scope.
//If you want to keep it, you must use ref() and unref().
ST_TinyJS_Var* ST_TinyJS::evaluateComplex(const char* code) {
	ST_TinyJS_VarLink* l = NULL;
	//{{SaveContext
	ST_TinyJS_Lex* save_lex                  = lex;
	GSList/*<ST_TinyJS_Var*>*/* save_scopes  = scopes;
	GSList/*<const char*>*/* save_call_stack = call_stack;
	//}}SaveContext
	{
		//{{InitContext
		lex        = new ST_TinyJS_Lex(code, 0, strlen(code));
		scopes     = g_slist_prepend(NULL, root);	//[root]
		call_stack = NULL;				//[]
		//}}InitContext
		try {
			bool bExec = true;
			do {
				l = base(&bExec);
				if(lex->tk != TINYJS_LEX_EOF) { lex->match(';'); }
			} while(lex->tk != TINYJS_LEX_EOF);
		} catch(ST_TinyJS_Exception* e) {
			const char* msg = stack_trace(e);
			//{{RestoreContext
			lex        = save_lex;
			scopes     = save_scopes;
			call_stack = save_call_stack;
			//}}RestoreContext
			throw new ST_TinyJS_Exception(msg);
		}
		if(!scopes || (scopes->data != root) || g_slist_delete_link(scopes, scopes)) { DIE(); }	//[root]に戻っているはずであり、rootを取り除けば[]になるはず。
		if(call_stack) { DIE(); }			//[]に戻っているはず。
	}
	//{{RestoreContext
	lex        = save_lex;
	scopes     = save_scopes;
	call_stack = save_call_stack;
	//}}RestoreContext
	return l ? l->var : new ST_TinyJS_Var();
}
//-----------------------------------------------------------------------------
//Evaluate the given code and return a string.
//If nothing to return, will return 'undefined'.
const char* ST_TinyJS::evaluate(const char* code) {
	return evaluateComplex(code)->getString();
}
//-----------------------------------------------------------------------------
//Add a native function to be called from TinyJS.
//example:
//│void scRandInt(ST_TinyJS_Var* v, void* userdata) { ... }
//│tinyJS->addNative("function randInt(min, max)", scRandInt, NULL);
//or
//│void scSubstring(ST_TinyJS_Var* v, void* userdata) { ... }
//│tinyJS->addNative("function String.substring(lo, hi)", scSubstring, NULL);
void ST_TinyJS::addNative(const char* funcDesc, TinyJS_Callback* callback, void* userdata) {
	//{{SaveContext
	ST_TinyJS_Lex* save_lex = lex;
	//}}SaveContext
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
			if(!l) { l = base->addChild(funcName, new ST_TinyJS_Var(TINYJS_BLANK_DATA, TINYJS_VAR_OBJECT)); }
			base = l->var;
			funcName = lex->tkStr;
			lex->match(TINYJS_LEX_ID);
		}
		ST_TinyJS_Var* funcVar = new ST_TinyJS_Var(TINYJS_BLANK_DATA, TINYJS_VAR_NATIVE);
		funcVar->setCallback(callback, userdata);
		parseFunctionArguments(funcVar);
		base->addChild(funcName, funcVar);
	}
	//{{RestoreContext
	lex = save_lex;
	//}}RestoreContext
}
//-----------------------------------------------------------------------------
//Handle a function call (assumes we've parsed the function name and we're on the start bracket).
//'parent' is the object that contains this method, if there was one (otherwise it's just a normnal function).
ST_TinyJS_VarLink* ST_TinyJS::functionCall(bool* pExec, ST_TinyJS_VarLink* func, ST_TinyJS_Var* parent) {
	if(*pExec) {
		if(!func->var->isFunction()) {
			const char* errorMsg = strdup_printf("Expecting '%s' to be a function.", func->name);
			throw new ST_TinyJS_Exception(errorMsg);
		}
		lex->match('(');
		//Create a new symbol table entry for execution of this function.
		ST_TinyJS_Var* functionRoot = new ST_TinyJS_Var(TINYJS_BLANK_DATA, TINYJS_VAR_FUNCTION);
		if(parent) {
if(functionRoot->findChild("this")){DIE();}
			functionRoot->addChild("this", parent);
		}
		//Grab in all parameters.
		GSList/*<ST_TinyJS_VarLink*>*/* list = func->var->firstChild;
		while(list) {
			ST_TinyJS_VarLink* l = (ST_TinyJS_VarLink*)list->data;
			ST_TinyJS_VarLink* tmp = base(pExec);
			if(*pExec) {
				if(tmp->var->isBasic()) {
					//pass by value
					functionRoot->addChild(l->name, tmp->var->deepCopy());
				} else {
					//pass by reference
					functionRoot->addChild(l->name, tmp->var);
				}
			}
			if(list->next) { lex->match(','); }
			list = list->next;
		}
		lex->match(')');
		//Execute function!
		//Add the function's execute space to the symbol table so we can recurse.
		ST_TinyJS_VarLink* returnVarLink = functionRoot->addChild(TINYJS_RETURN_VAR);
		//{{SaveContext
		ST_TinyJS_Lex* save_lex                  = lex;
		GSList/*<ST_TinyJS_Var*>*/* save_scopes  = scopes;
		GSList/*<const char*>*/* save_call_stack = call_stack;
		//}}SaveContext
		{
			//We just want to execute the block, but something could have messed up and left us with the wrong ScriptLex, so we want to be careful here...
			const char* funcBody = func->var->getString();
			//{{InitContext
			lex        = new ST_TinyJS_Lex(funcBody, 0, strlen(funcBody));
			scopes     = g_slist_prepend(scopes, functionRoot);
			call_stack = g_slist_prepend(call_stack, strdup_printf("%s from %s", func->name, lex->getLastPosition()));
			//}}InitContext
			try {
				if(func->var->isNative()) {
					assert(func->var->callback);
					(*func->var->callback)(this, functionRoot, func->var->userdata);
				} else {
					block(pExec);
					//Because return will probably have called this, and set execute to false.
					*pExec = true;
				}
			} catch(ST_TinyJS_Exception* /*e*/) {
				//{{RestoreContext
				lex        = save_lex;
				scopes     = save_scopes;
				call_stack = save_call_stack;
				//}}RestoreContext
				throw;
			}
		}
		//{{RestoreContext
		lex        = save_lex;
		scopes     = save_scopes;
		call_stack = save_call_stack;
		//}}RestoreContext
		functionRoot->removeLink(returnVarLink);
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
		//Function will be a blank scriptvarlink if we're not executing, so just return it rather than an alloc/free.
		return func;
	}
}
//-----------------------------------------------------------------------------
//因子
ST_TinyJS_VarLink* ST_TinyJS::factor(bool* pExec) {
	//(～)
	if(lex->tk == '(') {
		lex->match('(');
		ST_TinyJS_VarLink* a = base(pExec);
		lex->match(')');
		return a;
	}
	//true
	if(lex->tk == TINYJS_LEX_R_TRUE) {
		lex->match(TINYJS_LEX_R_TRUE);
		return new ST_TinyJS_VarLink(new ST_TinyJS_Var(1.0));
	}
	//false
	if(lex->tk == TINYJS_LEX_R_FALSE) {
		lex->match(TINYJS_LEX_R_FALSE);
		return new ST_TinyJS_VarLink(new ST_TinyJS_Var(0.0));
	}
	//null
	if(lex->tk == TINYJS_LEX_R_NULL) {
		lex->match(TINYJS_LEX_R_NULL);
		return new ST_TinyJS_VarLink(new ST_TinyJS_Var(TINYJS_BLANK_DATA,TINYJS_VAR_NULL));
	}
	//undefined
	if(lex->tk == TINYJS_LEX_R_UNDEFINED) {
		lex->match(TINYJS_LEX_R_UNDEFINED);
		return new ST_TinyJS_VarLink(new ST_TinyJS_Var(TINYJS_BLANK_DATA,TINYJS_VAR_UNDEFINED));
	}
	//ID
	if(lex->tk == TINYJS_LEX_ID) {
		ST_TinyJS_VarLink* a = NULL;
		if(*pExec) { a = findInScopes(lex->tkStr); }
		if(!a) {
			//Variable doesn't exist! JavaScript says we should create it.
			//(We won't add it here. This is done in the assignment operator.)
			a = new ST_TinyJS_VarLink(new ST_TinyJS_Var(), lex->tkStr);
		}
		lex->match(TINYJS_LEX_ID);
		//ID(～),又は,ID.ID,又は,ID[～]を、IDに還元して繰り返す。
		ST_TinyJS_Var* parent = NULL;	//The parent if we're executing a method call.
		while((lex->tk == '(') || (lex->tk == '.') || (lex->tk == '[')) {
			//Function Call.
			if(lex->tk == '(') {
				a = functionCall(pExec, a, parent);
			//Record Access.
			} else if(lex->tk == '.') {
				lex->match('.');
				if(*pExec) {
					const char* name = lex->tkStr;
					ST_TinyJS_VarLink* l = a->var->findChild(name);
					if(!l) { l = findInParentClasses(a->var, name); }
					if(!l) {
						//If we haven't found this defined yet, use the built-in 'length' properly.
						if(a->var->isArray() && !strcmp(name, "length")) {
							int len = a->var->getArrayLength();
							l = new ST_TinyJS_VarLink(new ST_TinyJS_Var(len));
						} else if(a->var->isString() && !strcmp(name, "length")) {
							int len = strlen(a->var->getString());
							l = new ST_TinyJS_VarLink(new ST_TinyJS_Var(len));
						} else {
							l = a->var->addChild(name);
						}
					}
					parent = a->var;
					a = l;
				}
				lex->match(TINYJS_LEX_ID);
			//Array Access.
			} else if(lex->tk == '[') {
				lex->match('[');
				ST_TinyJS_VarLink* index = base(pExec);
				lex->match(']');
				if(*pExec) {
					ST_TinyJS_VarLink* l = a->var->findChildOrCreate(index->var->getString());
					parent = a->var;
					a = l;
				}
			} else {
				DIE();	//バグ
			}
		}
		return a;
	}
	//数値リテラル
	if(lex->tk == TINYJS_LEX_L_NUM) {
		ST_TinyJS_Var* a = new ST_TinyJS_Var(lex->tkStr, TINYJS_VAR_NUMBER);
		lex->match(lex->tk);
		return new ST_TinyJS_VarLink(a);
	}
	//文字列リテラル
	if(lex->tk == TINYJS_LEX_L_STR) {
		ST_TinyJS_Var* a = new ST_TinyJS_Var(lex->tkStr, TINYJS_VAR_STRING);
		lex->match(TINYJS_LEX_L_STR);
		return new ST_TinyJS_VarLink(a);
	}
	//{id:value,...}
	if(lex->tk == '{') {
		lex->match('{');
		//JSON-style object definition.
		ST_TinyJS_Var* contents = new ST_TinyJS_Var(TINYJS_BLANK_DATA, TINYJS_VAR_OBJECT);
		while(lex->tk != '}') {
			const char* id = lex->tkStr;
			//We only allow strings or IDs on the left hand side of an initialisation.
			if(lex->tk == TINYJS_LEX_L_STR) {
				lex->match(TINYJS_LEX_L_STR);
			} else {
				lex->match(TINYJS_LEX_ID);
			}
			lex->match(':');
			ST_TinyJS_VarLink* a = base(pExec);
			contents->addChild(id, a->var);
			if(lex->tk != '}') { lex->match(','); }
		}
		lex->match('}');
		return new ST_TinyJS_VarLink(contents);
	}
	//[value,...]
	if(lex->tk == '[') {
		lex->match('[');
		//JSON-style array.
		ST_TinyJS_Var* contents = new ST_TinyJS_Var(TINYJS_BLANK_DATA, TINYJS_VAR_ARRAY);
		int idx = 0;
		while(lex->tk != ']') {
			ST_TinyJS_VarLink* a = base(pExec);
			contents->addChild(strdup_printf("%d", idx), a->var);
			if(lex->tk != ']') { lex->match(','); }
			idx++;
		}
		lex->match(']');
		return new ST_TinyJS_VarLink(contents);
	}
	if(lex->tk == TINYJS_LEX_R_FUNCTION) {
		ST_TinyJS_VarLink* funcVar = parseFunctionDefinition();
		if(strcmp(funcVar->name, TINYJS_TEMP_NAME)) {	//TINYJS_TEMP_NAME以外ならば…
			TRACE("Functions not defined at statement-level are not meant to have a name.\n");
		}
		return funcVar;
	}
	if(lex->tk == TINYJS_LEX_R_NEW) {
		//new -> create a new object
		lex->match(TINYJS_LEX_R_NEW);
		const char* className = lex->tkStr;
		if(*pExec) {
			ST_TinyJS_VarLink* objClassOrFunc = findInScopes(className);
			if(!objClassOrFunc) {
				TRACE("%s is not a valid class name.\n", className);
				return new ST_TinyJS_VarLink(new ST_TinyJS_Var());
			}
			lex->match(TINYJS_LEX_ID);
			ST_TinyJS_Var* obj = new ST_TinyJS_Var(TINYJS_BLANK_DATA, TINYJS_VAR_OBJECT);
			ST_TinyJS_VarLink* objLink = new ST_TinyJS_VarLink(obj);
			if(objClassOrFunc->var->isFunction()) {
				functionCall(pExec, objClassOrFunc, obj);
			} else {
				obj->addChild(TINYJS_PROTOTYPE_CLASS, objClassOrFunc->var);
				if(lex->tk == '(') {
					lex->match('(');
					lex->match(')');
				}
			}
			return objLink;
		} else {
			lex->match(TINYJS_LEX_ID);
			if(lex->tk == '(') {
				lex->match('(');
				lex->match(')');
			}
		}
	}
	//Nothing we can do here... just hope it's the end...
	lex->match(TINYJS_LEX_EOF);
	return 0;
}
//-----------------------------------------------------------------------------
//単項演算子を含む式
ST_TinyJS_VarLink* ST_TinyJS::unary(bool* pExec) {
	ST_TinyJS_VarLink* a;
	if(lex->tk == '!') {
		lex->match('!');
		a = factor(pExec);
		if(*pExec) {
			ST_TinyJS_Var zero(0.0);
			ST_TinyJS_Var* res = a->var->mathsOp(&zero, TINYJS_LEX_O_EQUAL);
			a = new ST_TinyJS_VarLink(res);
		}
	} else {
		a = factor(pExec);
	}
	return a;
}
//-----------------------------------------------------------------------------
//項
ST_TinyJS_VarLink* ST_TinyJS::term(bool* pExec) {
	ST_TinyJS_VarLink* a = unary(pExec);
	while((lex->tk == '*') || (lex->tk == '/') || (lex->tk == '%')) {
		int op = lex->tk;
		lex->match(op);
		ST_TinyJS_VarLink* b = unary(pExec);
		if(*pExec) {
			ST_TinyJS_Var* res = a->var->mathsOp(b->var, op);
			a = new ST_TinyJS_VarLink(res);
		}
	}
	return a;
}
//-----------------------------------------------------------------------------
//式
ST_TinyJS_VarLink* ST_TinyJS::expression(bool* pExec) {
	bool negate = false;
	if(lex->tk == '-') {
		lex->match('-');
		negate = true;
	}
	ST_TinyJS_VarLink* a = term(pExec);
	if(negate) {
		ST_TinyJS_Var zero(0.0);
		ST_TinyJS_Var* res = zero.mathsOp(a->var, '-');
		a = new ST_TinyJS_VarLink(res);
	}

	while((lex->tk == '+') || (lex->tk == '-') || (lex->tk == TINYJS_LEX_O_PLUSPLUS) || (lex->tk == TINYJS_LEX_O_MINUSMINUS)) {
		int op = lex->tk;
		lex->match(op);
		if((op == TINYJS_LEX_O_PLUSPLUS) || (op == TINYJS_LEX_O_MINUSMINUS)) {
			if(*pExec) {
				ST_TinyJS_Var one(1);
				ST_TinyJS_Var* res = a->var->mathsOp(&one, op == TINYJS_LEX_O_PLUSPLUS ? '+' : '-');
				ST_TinyJS_VarLink* oldValue = new ST_TinyJS_VarLink(a->var);
				//in-place add/subtract
				a->replaceWith(res);
				a = oldValue;
			}
		} else {
			ST_TinyJS_VarLink* b = term(pExec);
			if(*pExec) {
				//not in-place, so just replace
				ST_TinyJS_Var* res = a->var->mathsOp(b->var, op);
				a = new ST_TinyJS_VarLink(res);
			}
		}
	}
	return a;
}
//-----------------------------------------------------------------------------
ST_TinyJS_VarLink* ST_TinyJS::shift(bool* pExec) {
	ST_TinyJS_VarLink* a = expression(pExec);
	if((lex->tk == TINYJS_LEX_O_LSHIFT) || (lex->tk == TINYJS_LEX_O_RSHIFT) || (lex->tk == TINYJS_LEX_O_RSHIFTUNSIGNED)) {
		int op = lex->tk;
		lex->match(op);
		ST_TinyJS_VarLink* b = base(pExec);
		int shift = *pExec ? b->var->getNumber() : 0;
		if(*pExec) {
			if(op == TINYJS_LEX_O_LSHIFT) { a->var->setNumber((int)a->var->getNumber() << shift); }
			if(op == TINYJS_LEX_O_RSHIFT) { a->var->setNumber((int)a->var->getNumber() >> shift); }
			if(op == TINYJS_LEX_O_RSHIFTUNSIGNED) { a->var->setNumber(((unsigned int)a->var->getNumber()) >> shift); }
		}
	}
	return a;
}
//-----------------------------------------------------------------------------
ST_TinyJS_VarLink* ST_TinyJS::condition(bool* pExec) {
	ST_TinyJS_VarLink* a = shift(pExec);
	ST_TinyJS_VarLink* b;
	while((lex->tk == TINYJS_LEX_O_EQUAL) || (lex->tk == TINYJS_LEX_O_NEQUAL) ||
	      (lex->tk == TINYJS_LEX_O_TYPEEQUAL) || (lex->tk == TINYJS_LEX_O_NTYPEEQUAL) ||
	      (lex->tk == TINYJS_LEX_O_LEQUAL) || (lex->tk == TINYJS_LEX_O_GEQUAL) ||
	      (lex->tk == '<') || (lex->tk == '>')) {
		int op = lex->tk;
		lex->match(op);
		b = shift(pExec);
		if(*pExec) {
			ST_TinyJS_Var* res = a->var->mathsOp(b->var, op);
			a = new ST_TinyJS_VarLink(res);
		}
	}
	return a;
}
//-----------------------------------------------------------------------------
ST_TinyJS_VarLink* ST_TinyJS::logic(bool* pExec) {
	ST_TinyJS_VarLink* a = condition(pExec);
	ST_TinyJS_VarLink* b;
	while((lex->tk == '&') || (lex->tk == '|') || (lex->tk == '^') || (lex->tk == TINYJS_LEX_O_ANDAND) || (lex->tk == TINYJS_LEX_O_OROR)) {
		bool noexec = false;
		int op = lex->tk;
		lex->match(op);
		bool shortCircuit = false;
		bool boolean = false;
		//If we have short-circuit ops, then if we know the outcome we don't bother to execute the other op.
		//Even if not we need to tell mathsOp it's an & or |.
		if(op == TINYJS_LEX_O_ANDAND) {
			op = '&';
			shortCircuit = !a->var->getBool();
			boolean = true;
		} else if(op == TINYJS_LEX_O_OROR) {
			op = '|';
			shortCircuit = a->var->getBool();
			boolean = true;
		}
		b = condition(shortCircuit ? &noexec : pExec);
		if(*pExec && !shortCircuit) {
			if(boolean) {
				ST_TinyJS_Var* newa = new ST_TinyJS_Var(a->var->getBool());
				ST_TinyJS_Var* newb = new ST_TinyJS_Var(b->var->getBool());
				a = new ST_TinyJS_VarLink(newa);
				b = new ST_TinyJS_VarLink(newb);
			}
			ST_TinyJS_Var* res = a->var->mathsOp(b->var, op);
			a = new ST_TinyJS_VarLink(res);
		}
	}
	return a;
}
//-----------------------------------------------------------------------------
ST_TinyJS_VarLink* ST_TinyJS::ternary(bool* pExec) {
	ST_TinyJS_VarLink* lhs = logic(pExec);
	bool noexec = false;
	if(lex->tk == '?') {
		lex->match('?');
		if(!*pExec) {
			base(&noexec);
			lex->match(':');
			base(&noexec);
		} else {
			bool first = lhs->var->getBool();
			if(first) {
				lhs = base(pExec);
				lex->match(':');
				base(&noexec);
			} else {
				base(&noexec);
				lex->match(':');
				lhs = base(pExec);
			}
		}
	}
	return lhs;
}
//-----------------------------------------------------------------------------
ST_TinyJS_VarLink* ST_TinyJS::base(bool* pExec) {
	ST_TinyJS_VarLink* lhs = ternary(pExec);
	if((lex->tk == '=') ||
	   (lex->tk == TINYJS_LEX_O_PLUSEQUAL) ||
	   (lex->tk == TINYJS_LEX_O_MINUSEQUAL)) {
		//If we're assigning to this and we don't have a parent, add it to the symbol table root as per JavaScript.
		if(*pExec) {
			if(!lhs->owned) {
				if(strlen(lhs->name)) {
					ST_TinyJS_VarLink* realLhs = root->addChild(lhs->name, lhs->var);
					lhs = realLhs;
				} else {
					TRACE("Trying to assign to an un-named type.\n");
				}
			}
		}
		int op = lex->tk;
		lex->match(op);
		ST_TinyJS_VarLink* rhs = base(pExec);
		if(*pExec) {
			if(op == '=') {
				lhs->replaceWith(rhs);
			} else if(op == TINYJS_LEX_O_PLUSEQUAL) {
				ST_TinyJS_Var* res = lhs->var->mathsOp(rhs->var, '+');
				lhs->replaceWith(res);
			} else if(op == TINYJS_LEX_O_MINUSEQUAL) {
				ST_TinyJS_Var* res = lhs->var->mathsOp(rhs->var, '-');
				lhs->replaceWith(res);
			} else {
				DIE();
			}
		}
	}
	return lhs;
}
//-----------------------------------------------------------------------------
void ST_TinyJS::block(bool* pExec) {
	lex->match('{');
	if(*pExec) {
		while(lex->tk && (lex->tk != '}')) {
			statement(pExec);
		}
		lex->match('}');
	} else {
		//fast skip of blocks
		int brackets = 1;
		while(lex->tk && brackets) {
			if(lex->tk == '{') { brackets++; }
			if(lex->tk == '}') { brackets--; }
			lex->match(lex->tk);
		}
	}
}
//-----------------------------------------------------------------------------
void ST_TinyJS::statement(bool* pExec) {
	if((lex->tk == TINYJS_LEX_ID) ||
	   (lex->tk == TINYJS_LEX_L_NUM) ||
	   (lex->tk == TINYJS_LEX_L_STR) ||
	   (lex->tk == '-')) {
		//Execute a simple statement that only contains basic arithmetic...
		base(pExec);
		lex->match(';');
	} else if(lex->tk == '{') {
		//A block of code.
		block(pExec);
	} else if(lex->tk == ';') {
		//Empty statement - to allow things like ;;;
		lex->match(';');
	} else if(lex->tk == TINYJS_LEX_R_VAR) {
		//Variable creation.
		//TODO - We need a better way of parsing the left hand side.
		//       Maybe just have a flag called can_create_var that we set and then we parse as if we're doing a normal equals.
		lex->match(TINYJS_LEX_R_VAR);
		while(lex->tk != ';') {
			ST_TinyJS_VarLink* a = 0;
			if(*pExec) {
				ST_TinyJS_Var* v = (ST_TinyJS_Var*)scopes->data;
				a = v->findChildOrCreate(lex->tkStr);
			}
			lex->match(TINYJS_LEX_ID);
			//Now do stuff defined with dots.
			while(lex->tk == '.') {
				lex->match('.');
				if(*pExec) {
					ST_TinyJS_VarLink* lastA = a;
					a = lastA->var->findChildOrCreate(lex->tkStr);
				}
				lex->match(TINYJS_LEX_ID);
			}
			//Sort out initialiser.
			if(lex->tk == '=') {
				lex->match('=');
				ST_TinyJS_VarLink* l = base(pExec);
				if(*pExec) {
					a->replaceWith(l);
				}
			}
			if(lex->tk != ';') {
				lex->match(',');
			}
		}
		lex->match(';');
	} else if(lex->tk == TINYJS_LEX_R_IF) {
		lex->match(TINYJS_LEX_R_IF);
		lex->match('(');
		ST_TinyJS_VarLink* l = base(pExec);
		lex->match(')');
		bool cond = *pExec && l->var->getBool();
		bool noexec = false;	//Because we need to be abl;e to write to it.
		statement(cond ? pExec : &noexec);
		if(lex->tk == TINYJS_LEX_R_ELSE) {
			lex->match(TINYJS_LEX_R_ELSE);
			statement(cond ? &noexec : pExec);
		}
	} else if(lex->tk == TINYJS_LEX_R_WHILE) {
		//We do repetition by pulling out the string representing our statement
		//there's definitely some opportunity for optimisation here
		lex->match(TINYJS_LEX_R_WHILE);
		lex->match('(');
		int whileCondStart = lex->tokenStart;
		bool noexec = false;
		ST_TinyJS_VarLink* cond = base(pExec);
		bool loopCond = *pExec && cond->var->getBool();
		ST_TinyJS_Lex* whileCond = lex->getSubLex(whileCondStart);
		lex->match(')');
		int whileBodyStart = lex->tokenStart;
		statement(loopCond ? pExec : &noexec);
		ST_TinyJS_Lex* whileBody = lex->getSubLex(whileBodyStart);
		//{{SaveContext
		ST_TinyJS_Lex* save_lex = lex;
		//}}SaveContext
		while(loopCond) {
			whileCond->reset();
			lex = whileCond;
			cond = base(pExec);
			loopCond = *pExec && cond->var->getBool();
			if(loopCond) {
				whileBody->reset();
				lex = whileBody;
				statement(pExec);
			}
		}
		//{{RestoreContext
		lex = save_lex;
		//}}RestoreContext
	} else if(lex->tk == TINYJS_LEX_R_FOR) {
		lex->match(TINYJS_LEX_R_FOR);
		lex->match('(');
		statement(pExec);	//initialisation
		//lex->match(';');
		int forCondStart = lex->tokenStart;
		bool noexec = false;
		ST_TinyJS_VarLink* cond = base(pExec);	//condition
		bool loopCond = *pExec && cond->var->getBool();
		ST_TinyJS_Lex* forCond = lex->getSubLex(forCondStart);
		lex->match(';');
		int forIterStart = lex->tokenStart;
		base(&noexec);	//iterator
		ST_TinyJS_Lex* forIter = lex->getSubLex(forIterStart);
		lex->match(')');
		int forBodyStart = lex->tokenStart;
		statement(loopCond ? pExec : &noexec);
		ST_TinyJS_Lex* forBody = lex->getSubLex(forBodyStart);
		//{{SaveContext
		ST_TinyJS_Lex* save_lex = lex;
		//}}SaveContext
		if(loopCond) {
			forIter->reset();
			lex = forIter;
			base(pExec);
		}
		while(*pExec && loopCond) {
			forCond->reset();
			lex = forCond;
			cond = base(pExec);
			loopCond = cond->var->getBool();
			if(*pExec && loopCond) {
				forBody->reset();
				lex = forBody;
				statement(pExec);
			}
			if(*pExec && loopCond) {
				forIter->reset();
				lex = forIter;
				base(pExec);
			}
		}
		//{{RestoreContext
		lex = save_lex;
		//}}RestoreContext
	} else if(lex->tk == TINYJS_LEX_R_RETURN) {
		lex->match(TINYJS_LEX_R_RETURN);
		ST_TinyJS_VarLink* result = NULL;
		if(lex->tk != ';') {
			result = base(pExec);
		}
		if(*pExec) {
			ST_TinyJS_Var* v = (ST_TinyJS_Var*)scopes->data;
			ST_TinyJS_VarLink* resultVar = v->findChild(TINYJS_RETURN_VAR);
			if(resultVar) {
				resultVar->replaceWith(result);
			} else {
				TRACE("RETURN statement, but not in a function.\n");
			}
			*pExec = false;
		}
		lex->match(';');
	} else if(lex->tk == TINYJS_LEX_R_FUNCTION) {
		ST_TinyJS_VarLink* funcVar = parseFunctionDefinition();
		if(*pExec) {
			if(!strcmp(funcVar->name, TINYJS_TEMP_NAME)) {	//TINYJS_TEMP_NAMEならば…
				TRACE("Functions defined at statement-level are meant to have a name.\n");
			} else {						//TINYJS_TEMP_NAME以外ならば…
				ST_TinyJS_Var* v = (ST_TinyJS_Var*)scopes->data;
				v->addChild(funcVar->name, funcVar->var);
			}
		}
	} else {
		lex->match(TINYJS_LEX_EOF);
	}
}
//-----------------------------------------------------------------------------
//Get the given variable specified by a path (var1.var2.etc), or return NULL.
ST_TinyJS_Var* ST_TinyJS::getScriptVariable(const char* path) {
	//traverse path
	int prevIdx = 0;
	int thisIdx = strchrnul(path, '.') - path;
	ST_TinyJS_Var* v = root;
	while(v && (prevIdx < thisIdx)) {
		const char* el = strndup(path + prevIdx, thisIdx - prevIdx);
		ST_TinyJS_VarLink* l = v->findChild(el);
		v = l ? l->var : NULL;
		prevIdx = thisIdx + 1/*'.'*/;
		thisIdx = strchrnul(path + prevIdx, '.') - path;
	}
	return v;
}
//-----------------------------------------------------------------------------
//Set the value of the given variable, return trur if it exists and gets set.
bool ST_TinyJS::setVariable(const char* path, const char* varData) {
	ST_TinyJS_Var* v = getScriptVariable(path);
	//return result
	if(v) {
		if(v->isNumber()) {
			v->setNumber(strtonum(varData));
		} else {
			v->setString(varData);
		}
		return true;
	} else {
		return false;
	}
}
//-----------------------------------------------------------------------------
ST_TinyJS_VarLink* ST_TinyJS::parseFunctionDefinition() {
	//Actually parse a function...
	lex->match(TINYJS_LEX_R_FUNCTION);
	const char* funcName = TINYJS_TEMP_NAME;
	//We can have functions without names.
	if(lex->tk == TINYJS_LEX_ID) {
		funcName = lex->tkStr;
		lex->match(TINYJS_LEX_ID);
	}
	ST_TinyJS_VarLink* funcVar = new ST_TinyJS_VarLink(new ST_TinyJS_Var(TINYJS_BLANK_DATA, TINYJS_VAR_FUNCTION), funcName);
	parseFunctionArguments(funcVar->var);
	int funcBegin = lex->tokenStart;
	bool noexec = false;
	block(&noexec);
	funcVar->var->strData = lex->getSubString(funcBegin);
	return funcVar;
}
//-----------------------------------------------------------------------------
void ST_TinyJS::parseFunctionArguments(ST_TinyJS_Var* funcVar) {
	lex->match('(');
	while(lex->tk != ')') {
		funcVar->addChild(lex->tkStr);
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
		ST_TinyJS_Var* v = (ST_TinyJS_Var*)list->data;
		ST_TinyJS_VarLink* l = v->findChild(name);
		if(l) { return l; }
		list = list->next;
	}
	return NULL;
}
//-----------------------------------------------------------------------------
//Look up in any parent classes of the given object.
ST_TinyJS_VarLink* ST_TinyJS::findInParentClasses(ST_TinyJS_Var* obj, const char* name) {
	//Look for links to actual parent classes
	ST_TinyJS_VarLink* parentClass = obj->findChild(TINYJS_PROTOTYPE_CLASS);
	while(parentClass) {
		ST_TinyJS_VarLink* implementation = parentClass->var->findChild(name);
		if(implementation) return implementation;
		parentClass = parentClass->var->findChild(TINYJS_PROTOTYPE_CLASS);
	}
	//else fake it for strings and finally objects.
	if(obj->isString()) {
		ST_TinyJS_VarLink* implementation = stringClass->findChild(name);
		if(implementation) { return implementation; }
	}
	if(obj->isArray()) {
		ST_TinyJS_VarLink* implementation = arrayClass->findChild(name);
		if(implementation) { return implementation; }
	}
	ST_TinyJS_VarLink* implementation = objectClass->findChild(name);
	if(implementation) return implementation;
	return 0;
}
//-----------------------------------------------------------------------------
//元ソースにはこの関数は有りませんが、共通処理をまとめるために追加しました。
const char* ST_TinyJS::stack_trace(ST_TinyJS_Exception* e) {
	GString* msg = g_string_new(NULL);
	g_string_append(msg, "Error ");
	g_string_append(msg, e->text);
	GSList/*<const char*>*/* list = call_stack;
	int i = 0;
	while(list) {
		g_string_append(msg, "\n");
		g_string_append_printf(msg, "%d", i);
		g_string_append(msg, ": ");
		g_string_append(msg, (const char*)list->data);
		i++;
	}
	g_string_append(msg, " at ");
	g_string_append(msg, lex->getLastPosition());
	return msg->str;
}
