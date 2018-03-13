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
//	
//*****************************************************************************
//Frees the given link IF it isn't owned by anything else.
#define CLEAN(x) { CScriptVarLink* __v = x; if(__v && !__v->owned) { delete __v; } }
//Create a LINK to point to VAR and free the old link.
//BUT this is more clever - it tries to keep the old link if it's not owned to save allocations.
#define CREATE_LINK(LINK, VAR) { if(!LINK || LINK->owned) LINK = new CScriptVarLink(VAR); else LINK->replaceWith(VAR); }
//*****************************************************************************
//	Utils
//*****************************************************************************
//Convert the given string into a quoted string suitable for javascript.
static string getJSString(const char* str) {
	string out;
	int c;
	while((c = *str++) != '\0') {
		switch(c) {
		case '\t': out += "\\t";  break;	//┐
		case '\n': out += "\\n";  break;	//├頻繁に使う制御文字のエスケープシーケンスだけ明示的に変換し、頻繁に使わない制御文字はその他の印字不可文字と共通で8進数形式で出力する事にした。
		case '\r': out += "\\r";  break;	//┘
		case '"':  out += "\\\""; break;
		case '\\': out += "\\\\"; break;
		default:
			{
				if(isprint(c)) {
					out += c;
				} else {
					char buf[256];
					snprintf(buf, sizeof buf, "\\%03o", (unsigned char)c);	//16進数形式は桁数制限が無く誤って後続の文字と繋がって解釈される恐れが有る。8進数形式は最大でも3桁までしか解釈されず左記の問題は生じない。8進数形式の方が安全なので8進数形式で出力する事にした。
					out += buf;
				}
			}
			break;
		}
	}
	return "\"" + out + "\"";
}
//*****************************************************************************
//	CScriptException
//*****************************************************************************
CScriptException::CScriptException(const char* exceptionText) {
	text = exceptionText;
}
//*****************************************************************************
//	CScriptLex
//*****************************************************************************
CScriptLex::CScriptLex(const char* input) {
	data      = strdup(input);
	dataOwned = true;
	dataStart = 0;
	dataEnd   = (int)strlen(data);
	reset();
}
//-----------------------------------------------------------------------------
CScriptLex::CScriptLex(CScriptLex* owner, int startChar, int endChar) {
	data      = owner->data;
	dataOwned = false;
	dataStart = startChar;
	dataEnd   = endChar;
	reset();
}
//-----------------------------------------------------------------------------
CScriptLex::~CScriptLex() {
	if(dataOwned) {
		free(data);
	}
}
//-----------------------------------------------------------------------------
//Reset this lex so we can start again.
void CScriptLex::reset() {
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
void CScriptLex::match(int expected_tk) {
	if(tk != expected_tk) {
		string errorString;
		errorString += "Got ";
		errorString += getTokenStr(tk);
		errorString += " expected ";
		errorString += getTokenStr(expected_tk);
		errorString += " at ";
		errorString += getPosition(tokenStart);
		throw new CScriptException(errorString.c_str());
	}
	getNextToken();
}
//-----------------------------------------------------------------------------
//Get the string representation of the given token.
string CScriptLex::getTokenStr(int token) {
	switch(token) {
	case TINYJS_LEX_EOF:              return "EOF";
	//ID
	case TINYJS_LEX_ID:               return "ID";
	//リテラル
	case TINYJS_LEX_L_FLOAT:          return "FLOAT";
	case TINYJS_LEX_L_INT:            return "INT";
	case TINYJS_LEX_L_STR:            return "STRING";
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
	char buf[256];
	if(isprint(token)) {
		snprintf(buf, sizeof buf, "'%c'", token);
	} else {
		snprintf(buf, sizeof buf, "?[%d]", token);
	}
	return buf;
}
//-----------------------------------------------------------------------------
void CScriptLex::getNextCh() {
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
void CScriptLex::getNextToken() {
//不要	tk = TINYJS_LEX_EOF;
	tkStr.clear();
	while(currCh && isspace(currCh)) { getNextCh(); }
	//Newline comments.
	if((currCh == '/') && (nextCh == '/')) {
		while(currCh && currCh != '\n') { getNextCh(); }
		getNextCh();	//'\n'の次へ進める。
		getNextToken();
		return;	//ここまで
	}
	//Block comments.
	if((currCh == '/') && (nextCh == '*')) {
		while(currCh && (currCh != '*' || nextCh != '/')) { getNextCh(); }
		getNextCh();	//'*'の次へ進める。
		getNextCh();	//'/'の次へ進める。
		getNextToken();
		return;	//ここまで
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
	if(iscsymf(currCh)) {	//予約語,又は,ID
		while(iscsym(currCh)) {
			tkStr += currCh;
			getNextCh();
		}
		     if(tkStr == "break")     { tk = TINYJS_LEX_R_BREAK; }
		else if(tkStr == "continue")  { tk = TINYJS_LEX_R_CONTINUE; }
		else if(tkStr == "do")        { tk = TINYJS_LEX_R_DO; }
		else if(tkStr == "else")      { tk = TINYJS_LEX_R_ELSE; }
		else if(tkStr == "false")     { tk = TINYJS_LEX_R_FALSE; }
		else if(tkStr == "for")       { tk = TINYJS_LEX_R_FOR; }
		else if(tkStr == "function")  { tk = TINYJS_LEX_R_FUNCTION; }
		else if(tkStr == "if")        { tk = TINYJS_LEX_R_IF; }
		else if(tkStr == "new")       { tk = TINYJS_LEX_R_NEW; }
		else if(tkStr == "null")      { tk = TINYJS_LEX_R_NULL; }
		else if(tkStr == "return")    { tk = TINYJS_LEX_R_RETURN; }
		else if(tkStr == "true")      { tk = TINYJS_LEX_R_TRUE; }
		else if(tkStr == "undefined") { tk = TINYJS_LEX_R_UNDEFINED; }
		else if(tkStr == "var")       { tk = TINYJS_LEX_R_VAR; }
		else if(tkStr == "while")     { tk = TINYJS_LEX_R_WHILE; }
		else                          { tk = TINYJS_LEX_ID; }
	} else if(isdigit(currCh)) {	//数値リテラル
		bool isHex = false;
		if(currCh == '0') {
			tkStr += currCh;		//'0'を追加する。
			getNextCh();			//'0'の次へ進める。
			if(((currCh == 'x') || (currCh == 'X')) && isxdigit(nextCh)/*この条件忘れないで!!0xの後に少なくとも一文字の16進数が必要!!*/) {
				isHex = true;
				tkStr += currCh;	//'x',又は,'X'を追加する。
				getNextCh();		//'x',又は,'X'の次へ進める。
			}
		}
		tk = TINYJS_LEX_L_INT;
		while(isdigit(currCh) || (isHex && isxdigit(currCh))) {
			tkStr += currCh;
			getNextCh();
		}
		if(!isHex && (currCh == '.')) {
			tk = TINYJS_LEX_L_FLOAT;
			tkStr += currCh;		//'.'を追加する。
			getNextCh();			//'.'の次へ進める。
			while(isdigit(currCh)) {
				tkStr += currCh;
				getNextCh();
			}
		}
		//Do fancy e-style floating point.
		if(!isHex && ((currCh == 'e') || (currCh == 'E'))) {
			tk = TINYJS_LEX_L_FLOAT;
			tkStr += currCh;
			getNextCh();
			if(currCh == '-') {
				tkStr += currCh;	//'-'を追加する。
				getNextCh();		//'-'の次へ進める。
			}
			while(isdigit(currCh)) {
				tkStr += currCh;
				getNextCh();
			}
		}
	} else if((currCh == '"') || (currCh == '\'')) {	//文字列リテラル
		const int quotCh = currCh;
		tk = TINYJS_LEX_L_STR;
		getNextCh();	//'"',又は,"'"の次へ進める。
		while(currCh && (currCh != quotCh)) {
			if(currCh == '\\') {
				getNextCh();	//'\'の次へ進める。
				switch(currCh) {
				case 't':  tkStr += '\t'; break;	//┐
				case 'n':  tkStr += '\n'; break;	//├頻繁に使う制御文字のエスケープシーケンスだけ対応した。頻繁に使わない制御文字はその他の印字不可文字と共通で8進数形式,又は,16進数形式での入力を前提とする。もし不足ならば追加しろ。
				case 'r':  tkStr += '\r'; break;	//┘
				case '\\': tkStr += '\\'; break;
				default:
					//Hex digits.
					if(((currCh == 'x') || (currCh == 'X')) && isxdigit(nextCh)/*この条件忘れないで!!\xの後に少なくとも一文字の16進数が必要!!*/) {
						string buf;
						for(;;) {		//C言語の文字列リテラルの仕様では、"\x????～"には桁数制限が無い。JavaScriptでも同じなのかわからない。(※TODO:要確認)
							getNextCh();	//初回は'x',又は,'X'の次へ進める。
							if(!isxdigit(currCh)) { break; }
							buf += currCh;
						}
						tkStr += (char)strtoul(buf.c_str(), NULL, 16);
					//Octal digits.
					} else if((currCh >= '0') && (currCh <= '7')) {
						string buf;
						while(buf.length() < 3) {	//C言語の文字列リテラルの仕様では、"\???"は1～3桁である。JavaScriptでも同じなのかわからない。(※TODO:要確認)
							if(!isxdigit(currCh)) { break; }
							buf += currCh;
							getNextCh();
						}
						tkStr += (char)strtoul(buf.c_str(), NULL, 8);
					} else {
						tkStr += currCh;
					}
					break;
				}
			} else {
				tkStr += currCh;
			}
			getNextCh();
		}
		getNextCh();
	} else {	//演算子,又は,EOF
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
}
//-----------------------------------------------------------------------------
//Return a sub-string from the given position up until right now.
string CScriptLex::getSubString(int lastPosition) {
	assert(lastPosition <= dataEnd);
	int lastCharIdx = tokenLastEnd + 1;	//一つ前に取得したトークンの最後の文字の次の位置。
	if(lastCharIdx <= dataEnd) {
		return string(&data[lastPosition], lastCharIdx - lastPosition);
	} else {
		return string(&data[lastPosition],     dataEnd - lastPosition);	//※こちらになる事は起こり得ないのではないだろうか?(※TODO:要検討)
	}
}
//-----------------------------------------------------------------------------
//Return a sub-lexer from the given position up until right now.
CScriptLex* CScriptLex::getSubLex(int lastPosition) {
	assert(lastPosition <= dataEnd);
	int lastCharIdx = tokenLastEnd + 1;	//一つ前に取得したトークンの最後の文字の次の位置。
	if(lastCharIdx <= dataEnd) {
		return new CScriptLex(this, lastPosition, lastCharIdx);
	} else {
		return new CScriptLex(this, lastPosition,     dataEnd);		//※こちらになる事は起こり得ないのではないだろうか?(※TODO:要検討)
	}
}
//-----------------------------------------------------------------------------
//Return a string representing the position in lines and columns of the tokenLastEnd.
string CScriptLex::getLastPosition() {
	return CScriptLex::getPosition(tokenLastEnd);
}
//-----------------------------------------------------------------------------
//Return a string representing the position in lines and columns of the character pos given.
string CScriptLex::getPosition(int pos) {
	if(pos > dataEnd) { pos = dataEnd; }	//※起こり得ないのではないだろうか?(※TODO:要検討)
	int line = 1, col = 1;
	for(int i = 0; i < pos; i++) {
		col++;
		if(data[i] == '\n') {
			line++;
			col = 1;
		}
	}
	char buf[256];
	snprintf(buf, sizeof buf, "(line: %d, col: %d)", line, col);
	return buf;
}
//*****************************************************************************
//	CScriptVarLink
//*****************************************************************************
CScriptVarLink::CScriptVarLink(CScriptVar* v, const char* name) {
	this->name        = name;
	this->nextSibling = NULL;
	this->prevSibling = NULL;
	this->var         = v->ref();
	this->owned       = false;
}
//-----------------------------------------------------------------------------
//Copy constructor.
CScriptVarLink::CScriptVarLink(const CScriptVarLink& l) {
	this->name        = l.name;
	this->nextSibling = NULL;
	this->prevSibling = NULL;
	this->var         = l.var->ref();
	this->owned       = false;
}
//-----------------------------------------------------------------------------
CScriptVarLink::~CScriptVarLink() {
	var->unref();
}
//-----------------------------------------------------------------------------
//Replace the Variable pointed to.
void CScriptVarLink::replaceWith(CScriptVar* v) {
	CScriptVar* oldVar = var;
	var = v->ref();
	oldVar->unref();	//エリアスセーフ
}
//-----------------------------------------------------------------------------
//Replace the Variable pointed to (just dereferences).
void CScriptVarLink::replaceWith(CScriptVarLink* l) {
	if(l) {
		replaceWith(l->var);
	} else {
		replaceWith(new CScriptVar());	//※(l=NULL)で呼び出される事は起こり得るのだろうか?(※TODO:要確認)
	}
}
//-----------------------------------------------------------------------------
//Get the name as an integer (for arrays).
int CScriptVarLink::getIntName() {
	return atoi(name.c_str());
}
//-----------------------------------------------------------------------------
//Set the name as an integer (for arrays).
void CScriptVarLink::setIntName(int n) {
	char buf[256];
	snprintf(buf, sizeof buf, "%d", n);
	name = buf;
}
//*****************************************************************************
//	CScriptVar
//*****************************************************************************
//Create undefined.
CScriptVar::CScriptVar() {
	init();
	flags = TINYJS_SCRIPTVAR_UNDEFINED;
}
//-----------------------------------------------------------------------------
//User defined.
CScriptVar::CScriptVar(const char* varData, int varFlags) {
	init();
	flags = varFlags;
	if(varFlags & TINYJS_SCRIPTVAR_DOUBLE) {
		doubleData = strtod(varData, NULL);
	} else if(varFlags & TINYJS_SCRIPTVAR_INTEGER) {
		intData = strtol(varData, NULL, 0);
	} else {
		data = varData;
	}
}
//-----------------------------------------------------------------------------
//Create a string.
CScriptVar::CScriptVar(const char* str) {
	init();
	flags = TINYJS_SCRIPTVAR_STRING;
	data = str;
}
//-----------------------------------------------------------------------------
CScriptVar::CScriptVar(int val) {
	init();
	flags = TINYJS_SCRIPTVAR_INTEGER;
	intData = val;
}
//-----------------------------------------------------------------------------
CScriptVar::CScriptVar(double val) {
	init();
	flags = TINYJS_SCRIPTVAR_DOUBLE;
	doubleData = val;
}
//-----------------------------------------------------------------------------
CScriptVar::~CScriptVar() {
	removeAllChildren();
}
//-----------------------------------------------------------------------------
void CScriptVar::init() {
	refs       = 0;	//参照カウントの初期値は0。作成側にて明示的に初回のref()を実行しろ。
	flags      = 0;
	doubleData = 0;
	intData    = 0;
	data       = TINYJS_BLANK_DATA;
	jsCallback = NULL;
	jsCallbackUserData = NULL;
	firstChild = NULL;
	lastChild  = NULL;
}
//-----------------------------------------------------------------------------
//If this is a function, get the result value (for use by native functions).
CScriptVar* CScriptVar::getReturnVar() {
	return getParameter(TINYJS_RETURN_VAR);
}
//-----------------------------------------------------------------------------
//Set the result value.
//Use this when setting complex return data as it avoids a deepCopy().
void CScriptVar::setReturnVar(CScriptVar* v) {
	findChildOrCreate(TINYJS_RETURN_VAR)->replaceWith(v);
}
//-----------------------------------------------------------------------------
//If this is a function, get the parameter with the given name (for use by native functions).
CScriptVar* CScriptVar::getParameter(const char* name) {
	return findChildOrCreate(name)->var;
}
//-----------------------------------------------------------------------------
//Tries to find a child with the given name, may return NULL.
CScriptVarLink* CScriptVar::findChild(const char* name) {
	CScriptVarLink* l = firstChild;
	while(l) {
		if(l->name.compare(name) == 0) { return l; }
		l = l->nextSibling;
	}
	return NULL;
}
//-----------------------------------------------------------------------------
//Tries to find a child with the given name, or will create it with the given flags.
CScriptVarLink* CScriptVar::findChildOrCreate(const char* name, int varFlags) {
	CScriptVarLink* l = findChild(name);
	if(l) { return l; }
	return addChild(name, new CScriptVar(TINYJS_BLANK_DATA, varFlags));
}
//-----------------------------------------------------------------------------
//Tries to find a child with the given path (separated by dots).
CScriptVarLink* CScriptVar::findChildOrCreateByPath(const char* path) {
	string sPath(path);
	int p = sPath.find('.');
	if(p == -1) { return findChildOrCreate(sPath.c_str()); }
	return findChildOrCreate(sPath.substr(0, p).c_str(), TINYJS_SCRIPTVAR_OBJECT)->var->findChildOrCreateByPath(sPath.substr(p + 1).c_str());
}
//-----------------------------------------------------------------------------
CScriptVarLink* CScriptVar::addChild(const char* name, CScriptVar* v) {
	if(isUndefined()) { flags = TINYJS_SCRIPTVAR_OBJECT; }
	if(!v) { v = new CScriptVar(); }	//If no child supplied, create one.
	CScriptVarLink* l = new CScriptVarLink(v, name);
	l->owned = true;
	if(lastChild) {
		lastChild->nextSibling = l;
		l->prevSibling = lastChild;
		lastChild  = l;
	} else {
		firstChild = l;
		lastChild  = l;
	}
	return l;
}
//-----------------------------------------------------------------------------
//Add a child overwriting any with the same name.
CScriptVarLink* CScriptVar::addChildNoDup(const char* name, CScriptVar* v) {
	if(!v) { v = new CScriptVar(); }	//If no child supplied, create one.
	CScriptVarLink* l = findChild(name);
	if(l) {
		l->replaceWith(v);
	} else {
		l = addChild(name, v);
	}
	return l;
}
//-----------------------------------------------------------------------------
void CScriptVar::removeChild(CScriptVar* v) {
	CScriptVarLink* l = firstChild;
	while(l) {
		if(l->var == v) { break; }
		l = l->nextSibling;
	}
	removeLink(l);
}
//-----------------------------------------------------------------------------
//Remove a specific link (this is faster than finding via a child).
void CScriptVar::removeLink(CScriptVarLink* l) {
	if(!l) { return; }
	if(l->nextSibling) { l->nextSibling->prevSibling = l->prevSibling; }
	if(l->prevSibling) { l->prevSibling->nextSibling = l->nextSibling; }
	if(lastChild  == l) { lastChild  = l->prevSibling; }
	if(firstChild == l) { firstChild = l->nextSibling; }
	delete l;
}
//-----------------------------------------------------------------------------
void CScriptVar::removeAllChildren() {
	CScriptVarLink* l = firstChild;
	while(l) {
		CScriptVarLink* tmp = l->nextSibling;
		delete l;
		l = tmp;
	}
	firstChild = NULL;
	lastChild  = NULL;
}
//-----------------------------------------------------------------------------
//The the value at an array index.
CScriptVar* CScriptVar::getArrayIndex(int i) {
	char buf[256];
	snprintf(buf, sizeof buf, "%d", i);
	CScriptVarLink* l = findChild(buf);
	if(l) {
		return l->var;
	} else {
		return new CScriptVar();	//undefined
	}
}
//-----------------------------------------------------------------------------
void CScriptVar::setArrayIndex(int i, CScriptVar* v) {
	char buf[256];
	snprintf(buf, sizeof buf, "%d", i);
	CScriptVarLink* l = findChild(buf);
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
int CScriptVar::getArrayLength() {
	if(!isArray()) { return 0; }
	int ubound = -1;
	CScriptVarLink* l = firstChild;
	while(l) {
		char* endptr;
		int i = strtoul(l->name.c_str(), &endptr, 10);
		if(*endptr == '\0') {
			if(i > ubound) { ubound = i; }
		}
		l = l->nextSibling;
	}
	return ubound + 1;
}
//-----------------------------------------------------------------------------
//Get the number of children.
int CScriptVar::getChildCount() {
	int n = 0;
	CScriptVarLink* l = firstChild;
	while(l) {
		n++;
		l = l->nextSibling;
	}
	return n;
}
//-----------------------------------------------------------------------------
bool CScriptVar::getBool() {
	return getInt() != 0;	//※色々問題が有る気がする。getInt()がオブジェクトや文字列に対して0を返し、getBool()でfalseになってしまう。せめてオブジェクトや空でない文字列はtrueと判断すべきではないか?(※TODO:要検討)
}
//-----------------------------------------------------------------------------
int CScriptVar::getInt() {
	if(isInt()) { return intData; }
	if(isDouble()) { return (int)doubleData; }
//不要	if(isNull()) { return 0; }
//不要	if(isUndefined()) { return 0; }	//※本当はNumber(undefined)⇒NaNだが…
	return 0;			//※本当は0とは限らないが…せめて文字列⇒数値変換ぐらいは実装すべきではないか?(※TODO:)
}
//-----------------------------------------------------------------------------
double CScriptVar::getDouble() {
	if(isDouble()) { return doubleData; }
	if(isInt()) { return intData; }
//不要	if(isNull()) { return 0; }
//不要	if(isUndefined()) { return 0; }	//※本当はNumber(undefined)⇒NaNだが…
	return 0;			//※本当は0とは限らないが…せめて文字列⇒数値変換ぐらいは実装すべきではないか?(※TODO:)
}
//-----------------------------------------------------------------------------
string CScriptVar::getString() {
	if(isInt()) {
		char buf[256];
		snprintf(buf, sizeof buf, "%ld", intData);
		data = buf;	//※なぜ!?
		return data;
	}
	if(isDouble()) {
		char buf[256];
		snprintf(buf, sizeof buf, "%f", doubleData);
		data = buf;	//※なぜ!?
		return data;
	}
	if(isNull()) { return "null"; }
	if(isUndefined()) { return "undefined"; }
	return data;	//※オブジェクトや配列の文字列化は未対応なのか?(※TODO:要検討)
}
//-----------------------------------------------------------------------------
void CScriptVar::setInt(int val) {
	flags      = (flags & ~TINYJS_SCRIPTVAR_VARTYPEMASK) | TINYJS_SCRIPTVAR_INTEGER;
	doubleData = 0;
	intData    = val;
	data       = TINYJS_BLANK_DATA;
	removeAllChildren();	//※元ソースではこの処理は無かったが、プリミティブと見なすならば必要では?(※要検討)
}
//-----------------------------------------------------------------------------
void CScriptVar::setDouble(double val) {
	flags      = (flags & ~TINYJS_SCRIPTVAR_VARTYPEMASK) | TINYJS_SCRIPTVAR_DOUBLE;
	doubleData = val;
	intData    = 0;
	data       = TINYJS_BLANK_DATA;
	removeAllChildren();	//※元ソースではこの処理は無かったが、プリミティブと見なすならば必要では?(※要検討)
}
//-----------------------------------------------------------------------------
void CScriptVar::setString(const char* str) {
	//Name sure it's not still a number or integer.
	flags      = (flags & ~TINYJS_SCRIPTVAR_VARTYPEMASK) | TINYJS_SCRIPTVAR_STRING;
	doubleData = 0;
	intData    = 0;
	data       = str;
	removeAllChildren();	//※元ソースではこの処理は無かったが、プリミティブと見なすならば必要では?(※要検討)
}
//-----------------------------------------------------------------------------
void CScriptVar::setUndefined() {
	//Name sure it's not still a number or integer.
	flags      = (flags & ~TINYJS_SCRIPTVAR_VARTYPEMASK) | TINYJS_SCRIPTVAR_UNDEFINED;
	doubleData = 0;
	intData    = 0;
	data       = TINYJS_BLANK_DATA;
	removeAllChildren();
}
//-----------------------------------------------------------------------------
void CScriptVar::setArray() {
	//Name sure it's not still a number or integer.
	flags      = (flags & ~TINYJS_SCRIPTVAR_VARTYPEMASK) | TINYJS_SCRIPTVAR_ARRAY;
	doubleData = 0;
	intData    = 0;
	data       = TINYJS_BLANK_DATA;
	removeAllChildren();	//※なぜ!?配列のクリアも兼ねるのか!?(※要検討)
}
//-----------------------------------------------------------------------------
bool CScriptVar::equals(CScriptVar* v) {
	CScriptVar* tmp = mathsOp(v, TINYJS_LEX_O_EQUAL);
	bool res = tmp->getBool();
	delete tmp;
	return res;
}
//-----------------------------------------------------------------------------
//Do a maths op with another script variable.
CScriptVar* CScriptVar::mathsOp(CScriptVar* v, int op) {
	CScriptVar* const a = this;	//┬読み易いように変数名を揃えているだけ。必須ではない。
	CScriptVar* const b = v;	//┘
	//Type equality check.
	if((op == TINYJS_LEX_O_TYPEEQUAL) || (op == TINYJS_LEX_O_NTYPEEQUAL)) {
		//check type first, then call again to check data
		bool eql = ((a->flags & TINYJS_SCRIPTVAR_VARTYPEMASK) == (b->flags & TINYJS_SCRIPTVAR_VARTYPEMASK));	//※整数と実数は同一と見なすべきではないか?だいたい整数と実数を分ける必要は有るのか?(※TODO:要検討)
		if(eql) {
			CScriptVar* tmp = a->mathsOp(b, TINYJS_LEX_O_EQUAL);
			if(!tmp->getBool()) { eql = false; }
			delete tmp;
		}
		if(op == TINYJS_LEX_O_NTYPEEQUAL) { eql = !eql; }
		return new CScriptVar(eql);
	}
	//Do maths...
	if((a->isUndefined() || a->isNull()) && (b->isUndefined() || b->isNull())) {
		if(op == TINYJS_LEX_O_EQUAL) {
			return new CScriptVar(true);	//(undefined==null)もtrueです。(本物のJavaScriptでも)
		} else if(op == TINYJS_LEX_O_NEQUAL) {
			return new CScriptVar(false);	//(undefined!=null)もfalseです。(本物のJavaScriptでも)
		} else {
			return new CScriptVar();	//undefined	※本当は違う。(undefined+null)⇒NaNや(undefined|null)⇒0など。とは言え実際にそれらの挙動に依存するようなスクリプトは書かないと思う。(※TODO:要検討)
		}
	} else if(a->isNumeric() && b->isNumeric()) {
		if(a->isInt() && b->isInt()) {
			//Use ints.
			int da = a->getInt();
			int db = b->getInt();
			switch(op) {
			case '+':                 return new CScriptVar(da +  db);
			case '-':                 return new CScriptVar(da -  db);
			case '*':                 return new CScriptVar(da *  db);
			case '/':                 return new CScriptVar(da /  db);
			case '&':                 return new CScriptVar(da &  db);
			case '|':                 return new CScriptVar(da |  db);
			case '^':                 return new CScriptVar(da ^  db);
			case '%':                 return new CScriptVar(da %  db);
			case TINYJS_LEX_O_EQUAL:  return new CScriptVar(da == db);
			case TINYJS_LEX_O_NEQUAL: return new CScriptVar(da != db);
			case '<':                 return new CScriptVar(da <  db);
			case TINYJS_LEX_O_LEQUAL: return new CScriptVar(da <= db);
			case '>':                 return new CScriptVar(da >  db);
			case TINYJS_LEX_O_GEQUAL: return new CScriptVar(da >= db);
			default:throw new CScriptException(("Operation " + CScriptLex::getTokenStr(op) + " not supported on the Int datatype.").c_str());
			}
		} else {
			//Use doubles.
			double da = a->getDouble();
			double db = b->getDouble();
			switch(op) {
			case '+':                 return new CScriptVar(da +  db);
			case '-':                 return new CScriptVar(da -  db);
			case '*':                 return new CScriptVar(da *  db);
			case '/':                 return new CScriptVar(da /  db);
			case TINYJS_LEX_O_EQUAL:  return new CScriptVar(da == db);
			case TINYJS_LEX_O_NEQUAL: return new CScriptVar(da != db);
			case '<':                 return new CScriptVar(da <  db);
			case TINYJS_LEX_O_LEQUAL: return new CScriptVar(da <= db);
			case '>':                 return new CScriptVar(da >  db);
			case TINYJS_LEX_O_GEQUAL: return new CScriptVar(da >= db);
			default:throw new CScriptException(("Operation " + CScriptLex::getTokenStr(op) + " not supported on the Double datatype.").c_str());	//※これは違うのでは!?本物のJavaScriptでは実数値にビット演算を適用した場合、暗黙に整数変換されて演算されるだけでエラーは起きないはず。
			}
		}
	} else if(a->isArray()) {
		//Just check pointers.
		switch(op) {
		case TINYJS_LEX_O_EQUAL:  return new CScriptVar(a == b);
		case TINYJS_LEX_O_NEQUAL: return new CScriptVar(a != b);
		default:throw new CScriptException(("Operation " + CScriptLex::getTokenStr(op) + " not supported on the Array datatype.").c_str());
		}
	} else if(a->isObject()) {
		//Just check pointers.
		switch(op) {
		case TINYJS_LEX_O_EQUAL:  return new CScriptVar(a == b);
		case TINYJS_LEX_O_NEQUAL: return new CScriptVar(a != b);
		default:throw new CScriptException(("Operation " + CScriptLex::getTokenStr(op) + " not supported on the Object datatype.").c_str());
		}
	} else {
		string da = a->getString();
		string db = b->getString();
		//Use strings.
		switch(op) {
		case '+':                 return new CScriptVar((da +  db).c_str(), TINYJS_SCRIPTVAR_STRING);
		case TINYJS_LEX_O_EQUAL:  return new CScriptVar( da == db);
		case TINYJS_LEX_O_NEQUAL: return new CScriptVar( da != db);
		case '<':                 return new CScriptVar( da <  db);
		case TINYJS_LEX_O_LEQUAL: return new CScriptVar( da <= db);
		case '>':                 return new CScriptVar( da >  db);
		case TINYJS_LEX_O_GEQUAL: return new CScriptVar( da >= db);
		default:throw new CScriptException(("Operation " + CScriptLex::getTokenStr(op) + " not supported on the string datatype.").c_str());
		}
	}
}
//-----------------------------------------------------------------------------
//Copy the basic data and flags from the variable given, with no children.
//Should be used internally only - by copyValue and deepCopy.
void CScriptVar::copySimpleData(CScriptVar* v) {
	flags      = (flags & ~TINYJS_SCRIPTVAR_VARTYPEMASK) | (v->flags & TINYJS_SCRIPTVAR_VARTYPEMASK);
	doubleData = v->doubleData;
	intData    = v->intData;
	data       = v->data;
}
//-----------------------------------------------------------------------------
//Copy the value from the value given.
void CScriptVar::copyValue(CScriptVar* v) {	//※TinyJS_Functions.cppのObject.clone()で使用されているが、本物のJavaScriptにObject.clone()は無いのでは?(※TODO:要検討)
	if(v) {
		copySimpleData(v);
		//Remove all current children.
		removeAllChildren();
		//Copy children of 'v'.
		CScriptVarLink* l = v->firstChild;
		while(l) {
			CScriptVar* tmp;
			//Don't copy the 'parent' object...
			if(l->name != TINYJS_PROTOTYPE_CLASS) {
				tmp = l->var->deepCopy();
			} else {
				tmp = l->var;
			}
			addChild(l->name.c_str(), tmp);
			l = l->nextSibling;
		}
	} else {
		setUndefined();
	}
}
//-----------------------------------------------------------------------------
//Deep copy this node and return the result.
CScriptVar* CScriptVar::deepCopy() {
	CScriptVar* v = new CScriptVar();
#if 0
	v->copySimpleData(this);
	//Copy children.
	CScriptVarLink* l = firstChild;
	while(l) {
		CScriptVar* tmp;
		//don't copy the 'parent' object...
		if(l->name != TINYJS_PROTOTYPE_CLASS) {
			tmp = l->var->deepCopy();
		} else {
			tmp = l->var;
		}
		v->addChild(l->name.c_str(), tmp);
		l = l->nextSibling;
	}
#else	//↓↓↓※これで足りるのでは?(※TODO:要検討)↓↓↓
	v->copyValue(this);
#endif
	return v;
}
//-----------------------------------------------------------------------------
//Dump out the contents of this using trace.
void CScriptVar::trace(const char* indent, const char* name) {
	TRACE("%s'%s' = '%s' %s\n",
		indent,
		name,
		getString().c_str(),
		getFlagsAsString().c_str());
	string indentStr = string(indent) + " ";
	CScriptVarLink* l = firstChild;
	while(l) {
		l->var->trace(indentStr.c_str(), l->name.c_str());
		l = l->nextSibling;
	}
}
//-----------------------------------------------------------------------------
//For debugging - just dump a string version of the flags.
string CScriptVar::getFlagsAsString() {
	string flagstr = "";
	if(flags & TINYJS_SCRIPTVAR_DOUBLE)   { flagstr = flagstr + "DOUBLE"; }
	if(flags & TINYJS_SCRIPTVAR_INTEGER)  { flagstr = flagstr + "INTEGER"; }
	if(flags & TINYJS_SCRIPTVAR_STRING)   { flagstr = flagstr + "STRING"; }
	if(flags & TINYJS_SCRIPTVAR_FUNCTION) { flagstr = flagstr + "FUNCTION"; }
	if(flags & TINYJS_SCRIPTVAR_OBJECT)   { flagstr = flagstr + "OBJECT"; }
	if(flags & TINYJS_SCRIPTVAR_ARRAY)    { flagstr = flagstr + "ARRAY"; }
	if(flags & TINYJS_SCRIPTVAR_NATIVE)   { flagstr = flagstr + "NATIVE"; }
	return flagstr;
}
//-----------------------------------------------------------------------------
//Get data as a parsable javascript string.
string CScriptVar::getParsableString() {
	//Numbers can just be put in directly
	if(isNumeric()) {
		return getString();
	}
	if(isFunction()) {
		string funcStr;
		funcStr += "function (";
		//get list of parameters
		CScriptVarLink* l = firstChild;
		while(l) {
			funcStr += l->name;
			if(l->nextSibling) { funcStr += ","; }
			l = l->nextSibling;
		}
		//add function body
		funcStr += ") ";
		funcStr += getString();
		return funcStr;
	}
	//if it is a string then we quote it
	if(isString()) {
		return getJSString(getString().c_str());
	}
	if(isNull()) {
		return "null";
	}
	return "undefined";
}
//-----------------------------------------------------------------------------
string CScriptVar::getJSON(const char* linePrefix) {
	string indentedLinePrefix = linePrefix;
	indentedLinePrefix += "  ";
	string destination;
	if(isObject()) {
		//children - handle with bracketed list
		destination += "{ \n";
		CScriptVarLink* l = firstChild;
		while(l) {
			destination += indentedLinePrefix;
			destination += getJSString(l->name.c_str());
			destination += " : ";
			destination += l->var->getJSON(indentedLinePrefix.c_str());
			l = l->nextSibling;
			if(l) {
				destination += ",\n";
			}
		}
		destination += "\n";
		destination += linePrefix;
		destination += "}";
	} else if(isArray()) {
		destination += "[\n";
		int len = getArrayLength();
		if(len>10000) len=10000;	//we don't want to get stuck here!

		for(int i=0;i<len;i++) {
			destination += getArrayIndex(i)->getJSON(indentedLinePrefix.c_str());
			if(i<len-1) {
				destination += ",\n";
			}
		}
		destination += "\n";
		destination += linePrefix;
		destination += "]";
	} else {
		//no children or a function... just write value directly
		destination += getParsableString();
	}
	return destination;
}
//-----------------------------------------------------------------------------
void CScriptVar::setCallback(JSCallback callback, void* userData) {
	jsCallback = callback;
	jsCallbackUserData = userData;
}
//-----------------------------------------------------------------------------
CScriptVar* CScriptVar::ref() {
	refs++;
	return this;
}
//-----------------------------------------------------------------------------
void CScriptVar::unref() {
	if(refs<=0) printf("OMFG, we have unreffed too far!\n");
	if((--refs)==0) {
		delete this;
	}
}
//-----------------------------------------------------------------------------
int CScriptVar::getRefs() {
	return refs;
}
//*****************************************************************************
//	CTinyJS
//*****************************************************************************
CTinyJS::CTinyJS() {
	lex = NULL;
	root = (new CScriptVar(TINYJS_BLANK_DATA, TINYJS_SCRIPTVAR_OBJECT))->ref();
	//Add built-in classes
	stringClass = (new CScriptVar(TINYJS_BLANK_DATA, TINYJS_SCRIPTVAR_OBJECT))->ref();
	arrayClass = (new CScriptVar(TINYJS_BLANK_DATA, TINYJS_SCRIPTVAR_OBJECT))->ref();
	objectClass = (new CScriptVar(TINYJS_BLANK_DATA, TINYJS_SCRIPTVAR_OBJECT))->ref();
	root->addChild("String", stringClass);
	root->addChild("Array", arrayClass);
	root->addChild("Object", objectClass);
}
CTinyJS::~CTinyJS() {
	assert(!lex);
	scopes.clear();
	stringClass->unref();
	arrayClass->unref();
	objectClass->unref();
	root->unref();
}
void CTinyJS::trace(const char* indent) {
	root->trace(indent, "root");
}
void CTinyJS::execute(const char* code) {
	CScriptLex* oldLex = lex;
	vector<CScriptVar*> oldScopes = scopes;
	lex = new CScriptLex(code);
#ifdef TINYJS_CALL_STACK
	call_stack.clear();
#endif//TINYJS_CALL_STACK
	scopes.clear();
	scopes.push_back(root);
	try {
		bool execute = true;
		while(lex->tk) { statement(execute); }
	} catch(CScriptException* e) {
		string msg;
		msg += "Error ";
		msg += e->text;
#ifdef TINYJS_CALL_STACK
		for(int i=(int)call_stack.size()-1;i>=0;i--) {
			char buf[256];
			msg += "\n";
			msg += itoa(i, buf, 10);
			msg += ": ";
			msg += call_stack[i];
		}
#endif//TINYJS_CALL_STACK
		msg += " at ";
		msg += lex->getLastPosition();
		delete lex;
		lex = oldLex;
		throw new CScriptException(msg.c_str());
	}
	delete lex;
	lex = oldLex;
	scopes = oldScopes;
}
CScriptVarLink CTinyJS::evaluateComplex(const char* code) {
	CScriptLex* oldLex = lex;
	vector<CScriptVar*> oldScopes = scopes;

	lex = new CScriptLex(code);
#ifdef TINYJS_CALL_STACK
	call_stack.clear();
#endif//TINYJS_CALL_STACK
	scopes.clear();
	scopes.push_back(root);
	CScriptVarLink* l = NULL;
	try {
		bool execute = true;
		do {
			CLEAN(l);
			l = base(execute);
		if(lex->tk!=TINYJS_LEX_EOF) lex->match(';');
		} while(lex->tk!=TINYJS_LEX_EOF);
	} catch(CScriptException* e) {
		string msg;
		msg += "Error ";
		msg += e->text;
#ifdef TINYJS_CALL_STACK
		for(int i = call_stack.size() - 1; i >= 0; i--) {
			msg += "\n";
			msg += i;
			msg += ": ";
			msg += call_stack[i];
		}
#endif//TINYJS_CALL_STACK
		msg += " at ";
		msg += lex->getLastPosition();
		delete lex;
		lex = oldLex;
		throw new CScriptException(msg.c_str());
	}
	delete lex;
	lex = oldLex;
	scopes = oldScopes;

	if(l) {
		CScriptVarLink r = *l;	//┐
		CLEAN(l);		//├※なぜ!?(※TODO:要理解)
		return r;		//┘
	}
	//return undefined...
	return CScriptVarLink(new CScriptVar());
}
string CTinyJS::evaluate(const char* code) {
	return evaluateComplex(code).var->getString();
}
void CTinyJS::parseFunctionArguments(CScriptVar* funcVar) {
	lex->match('(');
	while(lex->tk!=')') {
	funcVar->addChildNoDup(lex->tkStr.c_str());
	lex->match(TINYJS_LEX_ID);
	if(lex->tk!=')') lex->match(',');
	}
	lex->match(')');
}
void CTinyJS::addNative(const char* funcDesc, JSCallback ptr, void* userData) {
	CScriptLex* oldLex = lex;
	lex = new CScriptLex(funcDesc);

	CScriptVar* base = root;

	lex->match(TINYJS_LEX_R_FUNCTION);
	string funcName = lex->tkStr;
	lex->match(TINYJS_LEX_ID);
	//Check for dots, we might want to do something like function String.substring ...
	while(lex->tk == '.') {
		lex->match('.');
		CScriptVarLink* l = base->findChild(funcName.c_str());
			//if it doesn't exist, make an object class
		if(!l) {
			l = base->addChild(funcName.c_str(), new CScriptVar(TINYJS_BLANK_DATA, TINYJS_SCRIPTVAR_OBJECT));
		}
		base = l->var;
		funcName = lex->tkStr;
		lex->match(TINYJS_LEX_ID);
	}

	CScriptVar* funcVar = new CScriptVar(TINYJS_BLANK_DATA, TINYJS_SCRIPTVAR_FUNCTION | TINYJS_SCRIPTVAR_NATIVE);
	funcVar->setCallback(ptr, userData);
	parseFunctionArguments(funcVar);
	delete lex;
	lex = oldLex;

	base->addChild(funcName.c_str(), funcVar);
}
CScriptVarLink* CTinyJS::parseFunctionDefinition() {
	//Actually parse a function...
	lex->match(TINYJS_LEX_R_FUNCTION);
	string funcName = TINYJS_TEMP_NAME;
	//We can have functions without names.
	if(lex->tk==TINYJS_LEX_ID) {
		funcName = lex->tkStr;
		lex->match(TINYJS_LEX_ID);
	}
	CScriptVarLink* funcVar = new CScriptVarLink(new CScriptVar(TINYJS_BLANK_DATA, TINYJS_SCRIPTVAR_FUNCTION), funcName.c_str());
	parseFunctionArguments(funcVar->var);
	int funcBegin = lex->tokenStart;
	bool noexecute = false;
	block(noexecute);
	funcVar->var->data = lex->getSubString(funcBegin);
	return funcVar;
}
//Handle a function call (assumes we've parsed the function name and we're on the start bracket).
//'parent' is the object that contains this method, if there was one (otherwise it's just a normnal function).
CScriptVarLink* CTinyJS::functionCall(bool& execute, CScriptVarLink* func, CScriptVar* parent) {
	if(execute) {
		if(!func->var->isFunction()) {
			string errorMsg = "Expecting '";
			errorMsg = errorMsg + func->name + "' to be a function.";
			throw new CScriptException(errorMsg.c_str());
		}
		lex->match('(');
		//create a new symbol table entry for execution of this function
		CScriptVar* functionRoot = new CScriptVar(TINYJS_BLANK_DATA, TINYJS_SCRIPTVAR_FUNCTION);
		if(parent) {
			functionRoot->addChildNoDup("this", parent);
		}
		//grab in all parameters
		CScriptVarLink* l = func->var->firstChild;
		while(l) {
			CScriptVarLink* value = base(execute);
			if(execute) {
				if(value->var->isBasic()) {
					//pass by value
					functionRoot->addChild(l->name.c_str(), value->var->deepCopy());
				} else {
					//pass by reference
					functionRoot->addChild(l->name.c_str(), value->var);
				}
			}
			CLEAN(value);
			if(lex->tk != ')') { lex->match(','); }
			l = l->nextSibling;
		}
		lex->match(')');
		//setup a return variable
		CScriptVarLink* returnVar = NULL;
		//execute function!
		//add the function's execute space to the symbol table so we can recurse
		CScriptVarLink* returnVarLink = functionRoot->addChild(TINYJS_RETURN_VAR);
		scopes.push_back(functionRoot);
#ifdef TINYJS_CALL_STACK
		call_stack.push_back(func->name + " from " + lex->getLastPosition());
#endif//TINYJS_CALL_STACK
		if(func->var->isNative()) {
			assert(func->var->jsCallback);
			(*func->var->jsCallback)(this, functionRoot, func->var->jsCallbackUserData);
		} else {
			//We just want to execute the block, but something could have messed up and left us with the wrong ScriptLex, so we want to be careful here...
			CScriptException* exception = NULL;
			CScriptLex* oldLex = lex;
			CScriptLex* newLex = new CScriptLex(func->var->getString().c_str());
			lex = newLex;
			try {
				block(execute);
				//because return will probably have called this, and set execute to false
				execute = true;
			} catch(CScriptException* e) {
				exception = e;
			}
			delete newLex;
			lex = oldLex;
			if(exception) {
				throw exception;
			}
		}
#ifdef TINYJS_CALL_STACK
		if(!call_stack.empty()) { call_stack.pop_back(); }
#endif//TINYJS_CALL_STACK
		scopes.pop_back();
		//Get the real return var before we remove it from our function.
		returnVar = new CScriptVarLink(returnVarLink->var);
		functionRoot->removeLink(returnVarLink);
		delete functionRoot;
		if(returnVar) {
			return returnVar;
		} else {
			return new CScriptVarLink(new CScriptVar());
		}
	} else {
		//Function, but not executing - just parse args and be done.
		lex->match('(');
		while(lex->tk != ')') {
			CScriptVarLink* value = base(execute);
			CLEAN(value);
			if(lex->tk!=')') lex->match(',');
		}
		lex->match(')');
		if(lex->tk == '{') {	//TODO: why is this here?
			block(execute);
		}
		//Function will be a blank scriptvarlink if we're not executing, so just return it rather than an alloc/free.
		return func;
	}
}
CScriptVarLink* CTinyJS::factor(bool& execute) {
	if(lex->tk=='(') {
		lex->match('(');
		CScriptVarLink* a = base(execute);
		lex->match(')');
		return a;
	}
	if(lex->tk==TINYJS_LEX_R_TRUE) {
		lex->match(TINYJS_LEX_R_TRUE);
		return new CScriptVarLink(new CScriptVar(1));
	}
	if(lex->tk==TINYJS_LEX_R_FALSE) {
		lex->match(TINYJS_LEX_R_FALSE);
		return new CScriptVarLink(new CScriptVar(0));
	}
	if(lex->tk==TINYJS_LEX_R_NULL) {
		lex->match(TINYJS_LEX_R_NULL);
		return new CScriptVarLink(new CScriptVar(TINYJS_BLANK_DATA,TINYJS_SCRIPTVAR_NULL));
	}
	if(lex->tk==TINYJS_LEX_R_UNDEFINED) {
		lex->match(TINYJS_LEX_R_UNDEFINED);
		return new CScriptVarLink(new CScriptVar(TINYJS_BLANK_DATA,TINYJS_SCRIPTVAR_UNDEFINED));
	}
	if(lex->tk==TINYJS_LEX_ID) {
		CScriptVarLink* a = execute ? findInScopes(lex->tkStr.c_str()) : new CScriptVarLink(new CScriptVar());
		//printf("0x%08X for %s at %s\n", (unsigned int)a, lex->tkStr.c_str(), lex->getLastPosition().c_str());
		//The parent if we're executing a method call.
		CScriptVar* parent = 0;

		if(execute && !a) {
			//Variable doesn't exist! JavaScript says we should create it.
			//(We won't add it here. This is done in the assignment operator.)
			a = new CScriptVarLink(new CScriptVar(), lex->tkStr.c_str());
		}
		lex->match(TINYJS_LEX_ID);
		while(lex->tk=='(' || lex->tk=='.' || lex->tk=='[') {
			if(lex->tk=='(') {		//------------------------------------- Function Call.
				a = functionCall(execute, a, parent);
			} else if(lex->tk == '.') {	//------------------------------------- Record Access.
				lex->match('.');
				if(execute) {
					string name = lex->tkStr;
					CScriptVarLink* l = a->var->findChild(name.c_str());
					if(!l) {
						l = findInParentClasses(a->var, name.c_str());
					}
					if(!l) {
						//If we haven't found this defined yet, use the built-in 'length' properly.
						if(a->var->isArray() && name == "length") {
							int lex = a->var->getArrayLength();
							l = new CScriptVarLink(new CScriptVar(lex));
						} else if(a->var->isString() && name == "length") {
							int lex = a->var->getString().length();
							l = new CScriptVarLink(new CScriptVar(lex));
						} else {
							l = a->var->addChild(name.c_str());
						}
					}
					parent = a->var;
					a = l;
				}
				lex->match(TINYJS_LEX_ID);
			} else if(lex->tk == '[') {	//------------------------------------- Array Access.
				lex->match('[');
				CScriptVarLink* index = base(execute);
				lex->match(']');
				if(execute) {
					CScriptVarLink* l = a->var->findChildOrCreate(index->var->getString().c_str());
					parent = a->var;
					a = l;
				}
				CLEAN(index);
			} else {
				assert(0);
			}
		}
		return a;
	}
	if(lex->tk==TINYJS_LEX_L_INT || lex->tk==TINYJS_LEX_L_FLOAT) {
		CScriptVar* a = new CScriptVar(lex->tkStr.c_str(), ((lex->tk==TINYJS_LEX_L_INT)?TINYJS_SCRIPTVAR_INTEGER:TINYJS_SCRIPTVAR_DOUBLE));
		lex->match(lex->tk);
		return new CScriptVarLink(a);
	}
	if(lex->tk==TINYJS_LEX_L_STR) {
		CScriptVar* a = new CScriptVar(lex->tkStr.c_str(), TINYJS_SCRIPTVAR_STRING);
		lex->match(TINYJS_LEX_L_STR);
		return new CScriptVarLink(a);
	}
	if(lex->tk=='{') {
		CScriptVar* contents = new CScriptVar(TINYJS_BLANK_DATA, TINYJS_SCRIPTVAR_OBJECT);
		//JSON-style object definition.
		lex->match('{');
		while(lex->tk != '}') {
			string id = lex->tkStr;
			//We only allow strings or IDs on the left hand side of an initialisation.
			if(lex->tk==TINYJS_LEX_L_STR) {
				lex->match(TINYJS_LEX_L_STR);
			} else {
				lex->match(TINYJS_LEX_ID);
			}
			lex->match(':');
			if(execute) {
				CScriptVarLink* a = base(execute);
				contents->addChild(id.c_str(), a->var);
				CLEAN(a);
			}
			//no need to clean here, as it will definitely be used
			if(lex->tk != '}') lex->match(',');
		}
		lex->match('}');
		return new CScriptVarLink(contents);
	}
	if(lex->tk=='[') {
		CScriptVar* contents = new CScriptVar(TINYJS_BLANK_DATA, TINYJS_SCRIPTVAR_ARRAY);
		//JSON-style array.
		lex->match('[');
		int idx = 0;
		while(lex->tk != ']') {
			if(execute) {
				char buf[256];	//big enough for 2^32
				snprintf(buf, sizeof buf, "%d", idx);
				CScriptVarLink* a = base(execute);
				contents->addChild(buf, a->var);
				CLEAN(a);
			}
			//no need to clean here, as it will definitely be used
			if(lex->tk != ']') lex->match(',');
			idx++;
		}
		lex->match(']');
		return new CScriptVarLink(contents);
	}
	if(lex->tk==TINYJS_LEX_R_FUNCTION) {
	CScriptVarLink* funcVar = parseFunctionDefinition();
		if(funcVar->name != TINYJS_TEMP_NAME)
		TRACE("Functions not defined at statement-level are not meant to have a name.\n");
		return funcVar;
	}
	if(lex->tk==TINYJS_LEX_R_NEW) {
		//new -> create a new object
		lex->match(TINYJS_LEX_R_NEW);
		string className = lex->tkStr;
		if(execute) {
			CScriptVarLink* objClassOrFunc = findInScopes(className.c_str());
			if(!objClassOrFunc) {
				TRACE("%s is not a valid class name.\n", className.c_str());
				return new CScriptVarLink(new CScriptVar());
			}
			lex->match(TINYJS_LEX_ID);
			CScriptVar* obj = new CScriptVar(TINYJS_BLANK_DATA, TINYJS_SCRIPTVAR_OBJECT);
			CScriptVarLink* objLink = new CScriptVarLink(obj);
			if(objClassOrFunc->var->isFunction()) {
				CLEAN(functionCall(execute, objClassOrFunc, obj));
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
CScriptVarLink* CTinyJS::unary(bool& execute) {
	CScriptVarLink* a;
	if(lex->tk=='!') {
		lex->match('!');	//binary not
		a = factor(execute);
		if(execute) {
			CScriptVar zero(0);
			CScriptVar* res = a->var->mathsOp(&zero, TINYJS_LEX_O_EQUAL);
			CREATE_LINK(a, res);
		}
	} else
		a = factor(execute);
	return a;
}
CScriptVarLink* CTinyJS::term(bool& execute) {
	CScriptVarLink* a = unary(execute);
	while(lex->tk=='*' || lex->tk=='/' || lex->tk=='%') {
		int op = lex->tk;
		lex->match(lex->tk);
		CScriptVarLink* b = unary(execute);
		if(execute) {
			CScriptVar* res = a->var->mathsOp(b->var, op);
			CREATE_LINK(a, res);
		}
		CLEAN(b);
	}
	return a;
}
CScriptVarLink* CTinyJS::expression(bool& execute) {
	bool negate = false;
	if(lex->tk=='-') {
		lex->match('-');
		negate = true;
	}
	CScriptVarLink* a = term(execute);
	if(negate) {
		CScriptVar zero(0);
		CScriptVar* res = zero.mathsOp(a->var, '-');
		CREATE_LINK(a, res);
	}

	while(lex->tk=='+' || lex->tk=='-' ||
		lex->tk==TINYJS_LEX_O_PLUSPLUS || lex->tk==TINYJS_LEX_O_MINUSMINUS) {
		int op = lex->tk;
		lex->match(lex->tk);
		if(op==TINYJS_LEX_O_PLUSPLUS || op==TINYJS_LEX_O_MINUSMINUS) {
			if(execute) {
				CScriptVar one(1);
				CScriptVar* res = a->var->mathsOp(&one, op==TINYJS_LEX_O_PLUSPLUS ? '+' : '-');
				CScriptVarLink* oldValue = new CScriptVarLink(a->var);
				//in-place add/subtract
				a->replaceWith(res);
				CLEAN(a);
				a = oldValue;
			}
		} else {
			CScriptVarLink* b = term(execute);
			if(execute) {
				//not in-place, so just replace
				CScriptVar* res = a->var->mathsOp(b->var, op);
				CREATE_LINK(a, res);
			}
			CLEAN(b);
		}
	}
	return a;
}
CScriptVarLink* CTinyJS::shift(bool& execute) {
	CScriptVarLink* a = expression(execute);
	if(lex->tk==TINYJS_LEX_O_LSHIFT || lex->tk==TINYJS_LEX_O_RSHIFT || lex->tk==TINYJS_LEX_O_RSHIFTUNSIGNED) {
		int op = lex->tk;
		lex->match(op);
		CScriptVarLink* b = base(execute);
		int shift = execute ? b->var->getInt() : 0;
		CLEAN(b);
		if(execute) {
			if(op==TINYJS_LEX_O_LSHIFT) a->var->setInt(a->var->getInt() << shift);
			if(op==TINYJS_LEX_O_RSHIFT) a->var->setInt(a->var->getInt() >> shift);
			if(op==TINYJS_LEX_O_RSHIFTUNSIGNED) a->var->setInt(((unsigned int)a->var->getInt()) >> shift);
		}
	}
	return a;
}
CScriptVarLink* CTinyJS::condition(bool& execute) {
	CScriptVarLink* a = shift(execute);
	CScriptVarLink* b;
	while(lex->tk==TINYJS_LEX_O_EQUAL || lex->tk==TINYJS_LEX_O_NEQUAL ||
	      lex->tk==TINYJS_LEX_O_TYPEEQUAL || lex->tk==TINYJS_LEX_O_NTYPEEQUAL ||
	      lex->tk==TINYJS_LEX_O_LEQUAL || lex->tk==TINYJS_LEX_O_GEQUAL ||
	      lex->tk=='<' || lex->tk=='>') {
		int op = lex->tk;
		lex->match(lex->tk);
		b = shift(execute);
		if(execute) {
			CScriptVar* res = a->var->mathsOp(b->var, op);
			CREATE_LINK(a,res);
		}
		CLEAN(b);
	}
	return a;
}
CScriptVarLink* CTinyJS::logic(bool& execute) {
	CScriptVarLink* a = condition(execute);
	CScriptVarLink* b;
	while(lex->tk=='&' || lex->tk=='|' || lex->tk=='^' || lex->tk==TINYJS_LEX_O_ANDAND || lex->tk==TINYJS_LEX_O_OROR) {
		bool noexecute = false;
		int op = lex->tk;
		lex->match(lex->tk);
		bool shortCircuit = false;
		bool boolean = false;
		//If we have short-circuit ops, then if we know the outcome we don't bother to execute the other op.
		//Even if not we need to tell mathsOp it's an & or |.
		if(op==TINYJS_LEX_O_ANDAND) {
			op = '&';
			shortCircuit = !a->var->getBool();
			boolean = true;
		} else if(op==TINYJS_LEX_O_OROR) {
			op = '|';
			shortCircuit = a->var->getBool();
			boolean = true;
		}
		b = condition(shortCircuit ? noexecute : execute);
		if(execute && !shortCircuit) {
			if(boolean) {
				CScriptVar* newa = new CScriptVar(a->var->getBool());
				CScriptVar* newb = new CScriptVar(b->var->getBool());
				CREATE_LINK(a, newa);
				CREATE_LINK(b, newb);
			}
			CScriptVar* res = a->var->mathsOp(b->var, op);
			CREATE_LINK(a, res);
		}
		CLEAN(b);
	}
	return a;
}
CScriptVarLink* CTinyJS::ternary(bool& execute) {
	CScriptVarLink* lhs = logic(execute);
	bool noexec = false;
	if(lex->tk=='?') {
		lex->match('?');
		if(!execute) {
			CLEAN(lhs);
			CLEAN(base(noexec));
			lex->match(':');
			CLEAN(base(noexec));
		} else {
			bool first = lhs->var->getBool();
			CLEAN(lhs);
			if(first) {
				lhs = base(execute);
				lex->match(':');
				CLEAN(base(noexec));
			} else {
				CLEAN(base(noexec));
				lex->match(':');
				lhs = base(execute);
			}
		}
	}
	return lhs;
}
CScriptVarLink* CTinyJS::base(bool& execute) {
	CScriptVarLink* lhs = ternary(execute);
	if(lex->tk=='=' || lex->tk==TINYJS_LEX_O_PLUSEQUAL || lex->tk==TINYJS_LEX_O_MINUSEQUAL) {
		//If we're assigning to this and we don't have a parent, add it to the symbol table root as per JavaScript.
		if(execute && !lhs->owned) {
			if(lhs->name.length()>0) {
				CScriptVarLink* realLhs = root->addChildNoDup(lhs->name.c_str(), lhs->var);
				CLEAN(lhs);
				lhs = realLhs;
			} else {
				TRACE("Trying to assign to an un-named type.\n");
			}
		}
		int op = lex->tk;
		lex->match(lex->tk);
		CScriptVarLink* rhs = base(execute);
		if(execute) {
			if(op == '=') {
				lhs->replaceWith(rhs);
			} else if(op == TINYJS_LEX_O_PLUSEQUAL) {
				CScriptVar* res = lhs->var->mathsOp(rhs->var, '+');
				lhs->replaceWith(res);
			} else if(op == TINYJS_LEX_O_MINUSEQUAL) {
				CScriptVar* res = lhs->var->mathsOp(rhs->var, '-');
				lhs->replaceWith(res);
			} else {
				assert(0);
			}
		}
		CLEAN(rhs);
	}
	return lhs;
}
void CTinyJS::block(bool& execute) {
	lex->match('{');
	if(execute) {
		while(lex->tk && lex->tk!='}') {
			statement(execute);
		}
		lex->match('}');
	} else {
		//fast skip of blocks
		int brackets = 1;
		while(lex->tk && brackets) {
			if(lex->tk == '{') brackets++;
			if(lex->tk == '}') brackets--;
			lex->match(lex->tk);
		}
	}
}
void CTinyJS::statement(bool& execute) {
	if(lex->tk==TINYJS_LEX_ID ||
		lex->tk==TINYJS_LEX_L_INT ||
		lex->tk==TINYJS_LEX_L_FLOAT ||
		lex->tk==TINYJS_LEX_L_STR ||
		lex->tk=='-') {
		//Execute a simple statement that only contains basic arithmetic...
		CLEAN(base(execute));
		lex->match(';');
	} else if(lex->tk=='{') {
		//A block of code.
		block(execute);
	} else if(lex->tk==';') {
		//Empty statement - to allow things like ;;;
		lex->match(';');
	} else if(lex->tk==TINYJS_LEX_R_VAR) {
		//Variable creation.
		//TODO - We need a better way of parsing the left hand side.
		//       Maybe just have a flag called can_create_var that we set and then we parse as if we're doing a normal equals.
		lex->match(TINYJS_LEX_R_VAR);
		while(lex->tk != ';') {
			CScriptVarLink* a = 0;
			if(execute) {
				a = scopes.back()->findChildOrCreate(lex->tkStr.c_str());
			}
			lex->match(TINYJS_LEX_ID);
			//Now do stuff defined with dots.
			while(lex->tk == '.') {
				lex->match('.');
				if(execute) {
					CScriptVarLink* lastA = a;
					a = lastA->var->findChildOrCreate(lex->tkStr.c_str());
				}
				lex->match(TINYJS_LEX_ID);
			}
			//Sort out initialiser.
			if(lex->tk == '=') {
				lex->match('=');
				CScriptVarLink* var = base(execute);
				if(execute) {
					a->replaceWith(var);
				}
				CLEAN(var);
			}
			if(lex->tk != ';') {
				lex->match(',');
			}
		}
		lex->match(';');
	} else if(lex->tk==TINYJS_LEX_R_IF) {
		lex->match(TINYJS_LEX_R_IF);
		lex->match('(');
		CScriptVarLink* var = base(execute);
		lex->match(')');
		bool cond = execute && var->var->getBool();
		CLEAN(var);
		bool noexecute = false;	//Because we need to be abl;e to write to it.
		statement(cond ? execute : noexecute);
		if(lex->tk==TINYJS_LEX_R_ELSE) {
			lex->match(TINYJS_LEX_R_ELSE);
			statement(cond ? noexecute : execute);
		}
	} else if(lex->tk==TINYJS_LEX_R_WHILE) {
		//We do repetition by pulling out the string representing our statement
		//there's definitely some opportunity for optimisation here
		lex->match(TINYJS_LEX_R_WHILE);
		lex->match('(');
		int whileCondStart = lex->tokenStart;
		bool noexecute = false;
		CScriptVarLink* cond = base(execute);
		bool loopCond = execute && cond->var->getBool();
		CLEAN(cond);
		CScriptLex* whileCond = lex->getSubLex(whileCondStart);
		lex->match(')');
		int whileBodyStart = lex->tokenStart;
		statement(loopCond ? execute : noexecute);
		CScriptLex* whileBody = lex->getSubLex(whileBodyStart);
		CScriptLex* oldLex = lex;
		int loopCount = TINYJS_LOOP_MAX_ITERATIONS;
		while(loopCond && loopCount-->0) {
			whileCond->reset();
			lex = whileCond;
			cond = base(execute);
			loopCond = execute && cond->var->getBool();
			CLEAN(cond);
			if(loopCond) {
				whileBody->reset();
				lex = whileBody;
				statement(execute);
			}
		}
		lex = oldLex;
		delete whileCond;
		delete whileBody;
		if(loopCount<=0) {
			root->trace();
			TRACE("WHILE Loop exceeded %d iterations at %s.\n", TINYJS_LOOP_MAX_ITERATIONS, lex->getLastPosition().c_str());
			throw new CScriptException("LOOP_ERROR");
		}
	} else if(lex->tk==TINYJS_LEX_R_FOR) {
		lex->match(TINYJS_LEX_R_FOR);
		lex->match('(');
		statement(execute);	//initialisation
		//lex->match(';');
		int forCondStart = lex->tokenStart;
		bool noexecute = false;
		CScriptVarLink* cond = base(execute);	//condition
		bool loopCond = execute && cond->var->getBool();
		CLEAN(cond);
		CScriptLex* forCond = lex->getSubLex(forCondStart);
		lex->match(';');
		int forIterStart = lex->tokenStart;
		CLEAN(base(noexecute));	//iterator
		CScriptLex* forIter = lex->getSubLex(forIterStart);
		lex->match(')');
		int forBodyStart = lex->tokenStart;
		statement(loopCond ? execute : noexecute);
		CScriptLex* forBody = lex->getSubLex(forBodyStart);
		CScriptLex* oldLex = lex;
		if(loopCond) {
			forIter->reset();
			lex = forIter;
			CLEAN(base(execute));
		}
		int loopCount = TINYJS_LOOP_MAX_ITERATIONS;
		while(execute && loopCond && loopCount-->0) {
			forCond->reset();
			lex = forCond;
			cond = base(execute);
			loopCond = cond->var->getBool();
			CLEAN(cond);
			if(execute && loopCond) {
				forBody->reset();
				lex = forBody;
				statement(execute);
			}
			if(execute && loopCond) {
				forIter->reset();
				lex = forIter;
				CLEAN(base(execute));
			}
		}
		lex = oldLex;
		delete forCond;
		delete forIter;
		delete forBody;
		if(loopCount<=0) {
			root->trace();
			TRACE("FOR Loop exceeded %d iterations at %s.\n", TINYJS_LOOP_MAX_ITERATIONS, lex->getLastPosition().c_str());
			throw new CScriptException("LOOP_ERROR");
		}
	} else if(lex->tk==TINYJS_LEX_R_RETURN) {
		lex->match(TINYJS_LEX_R_RETURN);
		CScriptVarLink* result = NULL;
		if(lex->tk != ';') {
			result = base(execute);
		}
		if(execute) {
			CScriptVarLink* resultVar = scopes.back()->findChild(TINYJS_RETURN_VAR);
			if(resultVar) {
				resultVar->replaceWith(result);
			} else {
				TRACE("RETURN statement, but not in a function.\n");
			}
			execute = false;
		}
		CLEAN(result);
		lex->match(';');
	} else if(lex->tk==TINYJS_LEX_R_FUNCTION) {
		CScriptVarLink* funcVar = parseFunctionDefinition();
		if(execute) {
			if(funcVar->name == TINYJS_TEMP_NAME) {
				TRACE("Functions defined at statement-level are meant to have a name.\n");
			} else {
				scopes.back()->addChildNoDup(funcVar->name.c_str(), funcVar->var);
			}
		}
		CLEAN(funcVar);
	} else lex->match(TINYJS_LEX_EOF);
}
//Get the given variable specified by a path (var1.var2.etc), or return NULL.
CScriptVar* CTinyJS::getScriptVariable(const char* path) {
	string sPath(path);
	//traverse path
	int prevIdx = 0;
	int thisIdx = sPath.find('.');
	if(thisIdx == -1) { thisIdx = sPath.length(); }
	CScriptVar* var = root;
	while(var && (prevIdx < sPath.length())) {
		string el = sPath.substr(prevIdx, thisIdx-prevIdx);
		CScriptVarLink* varl = var->findChild(el.c_str());
		var = varl ? varl->var : NULL;
		prevIdx = thisIdx+1;
		thisIdx = sPath.find('.', prevIdx);
		if(thisIdx == -1) { thisIdx = sPath.length(); }
	}
	return var;
}
//{{削除。この関数がNULLを返せるようにするために他の関数に色々無理が生じている。この関数を廃止して明示的にgetScriptVariable()を使うようにすれば自然になる。だいたい当関数はアプリケーション用に用意されたもののようで、ライブラリ内の実装においては使用されていなかった。
////Get the value of the given variable, or return NULL
//const string* CTinyJS::getVariable(const char* path) {
//	CScriptVar* var = getScriptVariable(path);
//	//return result
//	if(var) {
//		return &var->getString();
//	} else {
//		return NULL;
//	}
//}
//}}削除。この関数がNULLを返せるようにするために他の関数に色々無理が生じている。この関数を廃止して明示的にgetScriptVariable()を使うようにすれば自然になる。だいたい当関数はアプリケーション用に用意されたもののようで、ライブラリ内の実装においては使用されていなかった。
//Set the value of the given variable, return trur if it exists and gets set.
bool CTinyJS::setVariable(const char* path, const char* data) {
	CScriptVar* var = getScriptVariable(path);
	//return result
	if(var) {
		if(var->isInt()) {
			var->setInt((int)strtol(data, NULL, 0));
		} else if(var->isDouble()) {
			var->setDouble(strtod(data, NULL));
		} else {
			var->setString(data);
		}
		return true;
	} else {
		return false;
	}
}
//Finds a child, looking recursively up the scopes.
CScriptVarLink* CTinyJS::findInScopes(const char* name) {
	for(int s = scopes.size() - 1; s >= 0; s--) {
		CScriptVarLink* l = scopes[s]->findChild(name);
		if(l) { return l; }
	}
	return NULL;
}
//Look up in any parent classes of the given object.
CScriptVarLink* CTinyJS::findInParentClasses(CScriptVar* obj, const char* name) {
	//Look for links to actual parent classes
	CScriptVarLink* parentClass = obj->findChild(TINYJS_PROTOTYPE_CLASS);
	while(parentClass) {
		CScriptVarLink* implementation = parentClass->var->findChild(name);
		if(implementation) return implementation;
		parentClass = parentClass->var->findChild(TINYJS_PROTOTYPE_CLASS);
	}
	//else fake it for strings and finally objects.
	if(obj->isString()) {
		CScriptVarLink* implementation = stringClass->findChild(name);
		if(implementation) return implementation;
	}
	if(obj->isArray()) {
		CScriptVarLink* implementation = arrayClass->findChild(name);
		if(implementation) return implementation;
	}
	CScriptVarLink* implementation = objectClass->findChild(name);
	if(implementation) return implementation;
	return 0;
}
