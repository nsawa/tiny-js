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
//-----------------------------------------------------------------------------
//Frees the given link IF it isn't owned by anything else.
#define CLEAN(x) { CScriptVarLink* __v = x; if(__v && !__v->owned) { delete __v; } }
//Create a LINK to point to VAR and free the old link.
//BUT this is more clever - it tries to keep the old link if it's not owned to save allocations.
#define CREATE_LINK(LINK, VAR) { if(!LINK || LINK->owned) LINK = new CScriptVarLink(VAR); else LINK->replaceWith(VAR); }
//-----------------------------------------------------------------------------
//Utils
bool isNumber(const string& str) {
	for(size_t i=0;i<str.size();i++) {
		if(!isdigit(str[i])) { return false; }
	}
	return true;
}
void replace(string& str, char textFrom, const char* textTo) {
	int sLen = strlen(textTo);
	size_t p = str.find(textFrom);
	while(p != string::npos) {
		str = str.substr(0, p) + textTo + str.substr(p+1);
		p = str.find(textFrom, p+sLen);
	}
}
//convert the given string into a quoted string suitable for javascript
string getJSString(const string& str) {
	string nStr = str;
	for(size_t i=0;i<nStr.size();i++) {
		const char* replaceWith = "";
		bool replace = true;
		char buffer[5];
		switch(nStr[i]) {
		case '\\': replaceWith = "\\\\"; break;
		case '\n': replaceWith = "\\n";  break;
		case '\r': replaceWith = "\\r";  break;
		case '\a': replaceWith = "\\a";  break;
		case '"':  replaceWith = "\\\""; break;
		default:
			{
				int nCh = (unsigned char)nStr[i];
				if(!isprint(nCh)) {
					sprintf_s(buffer, 5, "\\x%02X", nCh);
					replaceWith = buffer;
				} else {
					replace = false;
				}
			}
			break;
		}
		if(replace) {
			nStr = nStr.substr(0, i) + replaceWith + nStr.substr(i+1);
			i += strlen(replaceWith)-1;
		}
	}
	return "\"" + nStr + "\"";
}
//-----------------------------------------------------------------------------
//CSCRIPTEXCEPTION
CScriptException::CScriptException(const string& exceptionText) {
	text = exceptionText;
}
//-----------------------------------------------------------------------------
//CSCRIPTLEX
CScriptLex::CScriptLex(const string& input) {
	data = _strdup(input.c_str());
	dataOwned = true;
	dataStart = 0;
	dataEnd = strlen(data);
	reset();
}
CScriptLex::CScriptLex(CScriptLex* owner, int startChar, int endChar) {
	data = owner->data;
	dataOwned = false;
	dataStart = startChar;
	dataEnd = endChar;
	reset();
}
CScriptLex::~CScriptLex(void)
{
	if(dataOwned)
		free((void*)data);
}
void CScriptLex::reset() {
	dataPos = dataStart;
	tokenStart = 0;
	tokenEnd = 0;
	tokenLastEnd = 0;
	tk = 0;
	tkStr = "";
	getNextCh();
	getNextCh();
	getNextToken();
}
void CScriptLex::match(int expected_tk) {
	if(tk!=expected_tk) {
		ostringstream errorString;
		errorString << "Got " << getTokenStr(tk) << " expected " << getTokenStr(expected_tk)
		<< " at " << getPosition(tokenStart);
		throw new CScriptException(errorString.str());
	}
	getNextToken();
}
string CScriptLex::getTokenStr(int token) {
	if(token>32 && token<128) {
		char buf[4] = "' '";
		buf[1] = (char)token;
		return buf;
	}
	switch(token) {
		case TINYJS_LEX_EOF : return "EOF";
		case TINYJS_LEX_ID : return "ID";
		case TINYJS_LEX_L_INT : return "INT";
		case TINYJS_LEX_L_FLOAT : return "FLOAT";
		case TINYJS_LEX_L_STR : return "STRING";
		case TINYJS_LEX_O_EQUAL : return "==";
		case TINYJS_LEX_O_TYPEEQUAL : return "===";
		case TINYJS_LEX_O_NEQUAL : return "!=";
		case TINYJS_LEX_O_NTYPEEQUAL : return "!==";
		case TINYJS_LEX_O_LEQUAL : return "<=";
		case TINYJS_LEX_O_LSHIFT : return "<<";
		case TINYJS_LEX_O_LSHIFTEQUAL : return "<<=";
		case TINYJS_LEX_O_GEQUAL : return ">=";
		case TINYJS_LEX_O_RSHIFT : return ">>";
		case TINYJS_LEX_O_RSHIFTUNSIGNED : return ">>";
		case TINYJS_LEX_O_RSHIFTEQUAL : return ">>=";
		case TINYJS_LEX_O_PLUSEQUAL : return "+=";
		case TINYJS_LEX_O_MINUSEQUAL : return "-=";
		case TINYJS_LEX_O_PLUSPLUS : return "++";
		case TINYJS_LEX_O_MINUSMINUS : return "--";
		case TINYJS_LEX_O_ANDEQUAL : return "&=";
		case TINYJS_LEX_O_ANDAND : return "&&";
		case TINYJS_LEX_O_OREQUAL : return "|=";
		case TINYJS_LEX_O_OROR : return "||";
		case TINYJS_LEX_O_XOREQUAL : return "^=";
		//reserved words
		case TINYJS_LEX_R_IF : return "if";
		case TINYJS_LEX_R_ELSE : return "else";
		case TINYJS_LEX_R_DO : return "do";
		case TINYJS_LEX_R_WHILE : return "while";
		case TINYJS_LEX_R_FOR : return "for";
		case TINYJS_LEX_R_BREAK : return "break";
		case TINYJS_LEX_R_CONTINUE : return "continue";
		case TINYJS_LEX_R_FUNCTION : return "function";
		case TINYJS_LEX_R_RETURN : return "return";
		case TINYJS_LEX_R_VAR : return "var";
		case TINYJS_LEX_R_TRUE : return "true";
		case TINYJS_LEX_R_FALSE : return "false";
		case TINYJS_LEX_R_NULL : return "null";
		case TINYJS_LEX_R_UNDEFINED : return "undefined";
		case TINYJS_LEX_R_NEW : return "new";
	}
	ostringstream msg;
	msg << "?[" << token << "]";
	return msg.str();
}
void CScriptLex::getNextCh() {
	currCh = nextCh;
	if(dataPos < dataEnd)
		nextCh = data[dataPos];
	else
		nextCh = 0;
	dataPos++;
}
void CScriptLex::getNextToken() {
	tk = TINYJS_LEX_EOF;
	tkStr.clear();
	while(currCh && isspace(currCh)) getNextCh();
	//newline comments
	if(currCh=='/' && nextCh=='/') {
		while(currCh && currCh!='\n') getNextCh();
		getNextCh();
		getNextToken();
		return;
	}
	//block comments
	if(currCh=='/' && nextCh=='*') {
		while(currCh && (currCh!='*' || nextCh!='/')) getNextCh();
		getNextCh();
		getNextCh();
		getNextToken();
		return;
	}
	//record beginning of this token
	tokenStart = dataPos-2;
	//tokens
	if(iscsymf(currCh)) {	//IDs
		while(iscsym(currCh)) {
			tkStr += currCh;
			getNextCh();
		}
		tk = TINYJS_LEX_ID;
			if(tkStr=="if") tk = TINYJS_LEX_R_IF;
		else if(tkStr=="else") tk = TINYJS_LEX_R_ELSE;
		else if(tkStr=="do") tk = TINYJS_LEX_R_DO;
		else if(tkStr=="while") tk = TINYJS_LEX_R_WHILE;
		else if(tkStr=="for") tk = TINYJS_LEX_R_FOR;
		else if(tkStr=="break") tk = TINYJS_LEX_R_BREAK;
		else if(tkStr=="continue") tk = TINYJS_LEX_R_CONTINUE;
		else if(tkStr=="function") tk = TINYJS_LEX_R_FUNCTION;
		else if(tkStr=="return") tk = TINYJS_LEX_R_RETURN;
		else if(tkStr=="var") tk = TINYJS_LEX_R_VAR;
		else if(tkStr=="true") tk = TINYJS_LEX_R_TRUE;
		else if(tkStr=="false") tk = TINYJS_LEX_R_FALSE;
		else if(tkStr=="null") tk = TINYJS_LEX_R_NULL;
		else if(tkStr=="undefined") tk = TINYJS_LEX_R_UNDEFINED;
		else if(tkStr=="new") tk = TINYJS_LEX_R_NEW;
	} else if(isdigit(currCh)) {	//Numbers
		bool isHex = false;
		if(currCh=='0') { tkStr += currCh; getNextCh(); }
		if(currCh=='x') {
		isHex = true;
		tkStr += currCh; getNextCh();
		}
		tk = TINYJS_LEX_L_INT;
		while(isdigit(currCh) || (isHex && isxdigit(currCh))) {
			tkStr += currCh;
			getNextCh();
		}
		if(!isHex && currCh=='.') {
			tk = TINYJS_LEX_L_FLOAT;
			tkStr += '.';
			getNextCh();
			while(isdigit(currCh)) {
				tkStr += currCh;
				getNextCh();
			}
		}
		//do fancy e-style floating point
		if(!isHex && (currCh=='e'||currCh=='E')) {
		tk = TINYJS_LEX_L_FLOAT;
		tkStr += currCh; getNextCh();
		if(currCh=='-') { tkStr += currCh; getNextCh(); }
		while(isdigit(currCh)) {
			tkStr += currCh; getNextCh();
		}
		}
	} else if(currCh=='"') {
		//strings...
		getNextCh();
		while(currCh && currCh!='"') {
			if(currCh == '\\') {
				getNextCh();
				switch(currCh) {
				case 'n' : tkStr += '\n'; break;
				case '"' : tkStr += '"'; break;
				case '\\' : tkStr += '\\'; break;
				default: tkStr += currCh;
				}
			} else {
				tkStr += currCh;
			}
			getNextCh();
		}
		getNextCh();
		tk = TINYJS_LEX_L_STR;
	} else if(currCh=='\'') {
		//strings again...
		getNextCh();
		while(currCh && currCh!='\'') {
			if(currCh == '\\') {
				getNextCh();
				switch(currCh) {
				case 'n' : tkStr += '\n'; break;
				case 'a' : tkStr += '\a'; break;
				case 'r' : tkStr += '\r'; break;
				case 't' : tkStr += '\t'; break;
				case '\'' : tkStr += '\''; break;
				case '\\' : tkStr += '\\'; break;
				case 'x' : {	//hex digits
							char buf[3] = "??";
							getNextCh(); buf[0] = currCh;
							getNextCh(); buf[1] = currCh;
							tkStr += (char)strtol(buf,0,16);
						} break;
				default: if(currCh>='0' && currCh<='7') {
							//octal digits
						char buf[4] = "???";
						buf[0] = currCh;
						getNextCh(); buf[1] = currCh;
						getNextCh(); buf[2] = currCh;
						tkStr += (char)strtol(buf,0,8);
						} else
						tkStr += currCh;
				}
			} else {
				tkStr += currCh;
			}
			getNextCh();
		}
		getNextCh();
		tk = TINYJS_LEX_L_STR;
	} else {
		//single chars
		tk = currCh;
		if(currCh) getNextCh();
		if(tk=='=' && currCh=='=') {	//==
			tk = TINYJS_LEX_O_EQUAL;
			getNextCh();
			if(currCh=='=') {	//===
			tk = TINYJS_LEX_O_TYPEEQUAL;
			getNextCh();
			}
		} else if(tk=='!' && currCh=='=') {	//!=
			tk = TINYJS_LEX_O_NEQUAL;
			getNextCh();
			if(currCh=='=') {	//!==
			tk = TINYJS_LEX_O_NTYPEEQUAL;
			getNextCh();
			}
		} else if(tk=='<' && currCh=='=') {
			tk = TINYJS_LEX_O_LEQUAL;
			getNextCh();
		} else if(tk=='<' && currCh=='<') {
			tk = TINYJS_LEX_O_LSHIFT;
			getNextCh();
			if(currCh=='=') {	//<<=
			tk = TINYJS_LEX_O_LSHIFTEQUAL;
			getNextCh();
			}
		} else if(tk=='>' && currCh=='=') {
			tk = TINYJS_LEX_O_GEQUAL;
			getNextCh();
		} else if(tk=='>' && currCh=='>') {
			tk = TINYJS_LEX_O_RSHIFT;
			getNextCh();
			if(currCh=='=') {	//>>=
			tk = TINYJS_LEX_O_RSHIFTEQUAL;
			getNextCh();
			} else if(currCh=='>') {	//>>>
			tk = TINYJS_LEX_O_RSHIFTUNSIGNED;
			getNextCh();
			}
		}  else if(tk=='+' && currCh=='=') {
			tk = TINYJS_LEX_O_PLUSEQUAL;
			getNextCh();
		}  else if(tk=='-' && currCh=='=') {
			tk = TINYJS_LEX_O_MINUSEQUAL;
			getNextCh();
		}  else if(tk=='+' && currCh=='+') {
			tk = TINYJS_LEX_O_PLUSPLUS;
			getNextCh();
		}  else if(tk=='-' && currCh=='-') {
			tk = TINYJS_LEX_O_MINUSMINUS;
			getNextCh();
		} else if(tk=='&' && currCh=='=') {
			tk = TINYJS_LEX_O_ANDEQUAL;
			getNextCh();
		} else if(tk=='&' && currCh=='&') {
			tk = TINYJS_LEX_O_ANDAND;
			getNextCh();
		} else if(tk=='|' && currCh=='=') {
			tk = TINYJS_LEX_O_OREQUAL;
			getNextCh();
		} else if(tk=='|' && currCh=='|') {
			tk = TINYJS_LEX_O_OROR;
			getNextCh();
		} else if(tk=='^' && currCh=='=') {
			tk = TINYJS_LEX_O_XOREQUAL;
			getNextCh();
		}
	}
	//This isn't quite right yet.
	tokenLastEnd = tokenEnd;
	tokenEnd = dataPos-3;
}
string CScriptLex::getSubString(int lastPosition) {
	int lastCharIdx = tokenLastEnd+1;
	if(lastCharIdx < dataEnd) {
		//Save a memory alloc by using our data array to create the substring.
		char old = data[lastCharIdx];
		data[lastCharIdx] = 0;
		string value = &data[lastPosition];
		data[lastCharIdx] = old;
		return value;
	} else {
		return string(&data[lastPosition]);
	}
}
CScriptLex* CScriptLex::getSubLex(int lastPosition) {
	int lastCharIdx = tokenLastEnd+1;
	if(lastCharIdx < dataEnd)
		return new CScriptLex(this, lastPosition, lastCharIdx);
	else
		return new CScriptLex(this, lastPosition, dataEnd );
}
string CScriptLex::getPosition(int pos) {
	if(pos<0) pos=tokenLastEnd;
	int line = 1,col = 1;
	for(int i=0;i<pos;i++) {
		char ch;
		if(i < dataEnd)
			ch = data[i];
		else
			ch = 0;
		col++;
		if(ch=='\n') {
			line++;
			col = 0;
		}
	}
	char buf[256];
	sprintf_s(buf, 256, "(line: %d, col: %d)", line, col);
	return buf;
}
//-----------------------------------------------------------------------------
//CSCRIPTVARLINK
CScriptVarLink::CScriptVarLink(CScriptVar* var, const string& name) {
	this->name = name;
	this->nextSibling = 0;
	this->prevSibling = 0;
	this->var = var->ref();
	this->owned = false;
}
CScriptVarLink::CScriptVarLink(const CScriptVarLink& link) {
	//Copy constructor
	this->name = link.name;
	this->nextSibling = 0;
	this->prevSibling = 0;
	this->var = link.var->ref();
	this->owned = false;
}
CScriptVarLink::~CScriptVarLink() {
	var->unref();
}
void CScriptVarLink::replaceWith(CScriptVar* newVar) {
	CScriptVar* oldVar = var;
	var = newVar->ref();
	oldVar->unref();
}
void CScriptVarLink::replaceWith(CScriptVarLink* newVar) {
	if(newVar)
	replaceWith(newVar->var);
	else
	replaceWith(new CScriptVar());
}
int CScriptVarLink::getIntName() {
	return atoi(name.c_str());
}
void CScriptVarLink::setIntName(int n) {
	char sIdx[64];
	sprintf_s(sIdx, sizeof(sIdx), "%d", n);
	name = sIdx;
}
//-----------------------------------------------------------------------------
//CSCRIPTVAR
CScriptVar::CScriptVar() {
	refs = 0;
	init();
	flags = TINYJS_SCRIPTVAR_UNDEFINED;
}
CScriptVar::CScriptVar(const string& str) {
	refs = 0;
	init();
	flags = TINYJS_SCRIPTVAR_STRING;
	data = str;
}
CScriptVar::CScriptVar(const string& varData, int varFlags) {
	refs = 0;
	init();
	flags = varFlags;
	if(varFlags & TINYJS_SCRIPTVAR_INTEGER) {
	intData = strtol(varData.c_str(),0,0);
	} else if(varFlags & TINYJS_SCRIPTVAR_DOUBLE) {
	doubleData = strtod(varData.c_str(),0);
	} else
	data = varData;
}
CScriptVar::CScriptVar(double val) {
	refs = 0;
	init();
	setDouble(val);
}
CScriptVar::CScriptVar(int val) {
	refs = 0;
	init();
	setInt(val);
}
CScriptVar::~CScriptVar(void) {
	removeAllChildren();
}
void CScriptVar::init() {
	firstChild = 0;
	lastChild = 0;
	flags = 0;
	jsCallback = 0;
	jsCallbackUserData = 0;
	data = TINYJS_BLANK_DATA;
	intData = 0;
	doubleData = 0;
}
CScriptVar* CScriptVar::getReturnVar() {
	return getParameter(TINYJS_RETURN_VAR);
}
void CScriptVar::setReturnVar(CScriptVar* var) {
	findChildOrCreate(TINYJS_RETURN_VAR)->replaceWith(var);
}
CScriptVar* CScriptVar::getParameter(const string& name) {
	return findChildOrCreate(name)->var;
}
CScriptVarLink* CScriptVar::findChild(const string& childName) {
	CScriptVarLink* v = firstChild;
	while(v) {
		if(v->name.compare(childName)==0)
			return v;
		v = v->nextSibling;
	}
	return 0;
}
CScriptVarLink* CScriptVar::findChildOrCreate(const string& childName, int varFlags) {
	CScriptVarLink* l = findChild(childName);
	if(l) return l;
	return addChild(childName, new CScriptVar(TINYJS_BLANK_DATA, varFlags));
}
CScriptVarLink* CScriptVar::findChildOrCreateByPath(const string& path) {
	size_t p = path.find('.');
	if(p == string::npos)
	return findChildOrCreate(path);
	return findChildOrCreate(path.substr(0,p), TINYJS_SCRIPTVAR_OBJECT)->var->
			findChildOrCreateByPath(path.substr(p+1));
}
CScriptVarLink* CScriptVar::addChild(const string& childName, CScriptVar* child) {
	if(isUndefined()) {
	flags = TINYJS_SCRIPTVAR_OBJECT;
	}
	//if no child supplied, create one
	if(!child)
	child = new CScriptVar();

	CScriptVarLink* link = new CScriptVarLink(child, childName);
	link->owned = true;
	if(lastChild) {
		lastChild->nextSibling = link;
		link->prevSibling = lastChild;
		lastChild = link;
	} else {
		firstChild = link;
		lastChild = link;
	}
	return link;
}
CScriptVarLink* CScriptVar::addChildNoDup(const string& childName, CScriptVar* child) {
	//if no child supplied, create one
	if(!child)
	child = new CScriptVar();

	CScriptVarLink* v = findChild(childName);
	if(v) {
		v->replaceWith(child);
	} else {
		v = addChild(childName, child);
	}
	return v;
}
void CScriptVar::removeChild(CScriptVar* child) {
	CScriptVarLink* link = firstChild;
	while(link) {
		if(link->var == child)
			break;
		link = link->nextSibling;
	}
	assert(link);
	removeLink(link);
}
void CScriptVar::removeLink(CScriptVarLink* link) {
	if(!link) return;
	if(link->nextSibling)
	link->nextSibling->prevSibling = link->prevSibling;
	if(link->prevSibling)
	link->prevSibling->nextSibling = link->nextSibling;
	if(lastChild == link)
		lastChild = link->prevSibling;
	if(firstChild == link)
		firstChild = link->nextSibling;
	delete link;
}
void CScriptVar::removeAllChildren() {
	CScriptVarLink* var = firstChild;
	while(var) {
		CScriptVarLink* tmp = var->nextSibling;
		delete var;
		var = tmp;
	}
	firstChild = 0;
	lastChild = 0;
}
CScriptVar* CScriptVar::getArrayIndex(int idx) {
	char sIdx[64];
	sprintf_s(sIdx, sizeof(sIdx), "%d", idx);
	CScriptVarLink* link = findChild(sIdx);
	if(link) return link->var;
	else return new CScriptVar(TINYJS_BLANK_DATA, TINYJS_SCRIPTVAR_NULL);	//undefined
}
void CScriptVar::setArrayIndex(int idx, CScriptVar* value) {
	char sIdx[64];
	sprintf_s(sIdx, sizeof(sIdx), "%d", idx);
	CScriptVarLink* link = findChild(sIdx);
	if(link) {
		if(value->isUndefined()) {
			removeLink(link);
		} else {
			link->replaceWith(value);
		}
	} else {
		if(!value->isUndefined()) {
			addChild(sIdx, value);
		}
	}
}
int CScriptVar::getArrayLength() {
	int highest = -1;
	if(!isArray()) return 0;

	CScriptVarLink* link = firstChild;
	while(link) {
		if(isNumber(link->name)) {
			int val = atoi(link->name.c_str());
			if(val > highest) { highest = val; }
		}
		link = link->nextSibling;
	}
	return highest+1;
}
int CScriptVar::getChildren() {
	int n = 0;
	CScriptVarLink* link = firstChild;
	while(link) {
	n++;
	link = link->nextSibling;
	}
	return n;
}
int CScriptVar::getInt() {
	//Strtol understands about hex and octal.
	if(isInt()) return intData;
	if(isNull()) return 0;
	if(isUndefined()) return 0;
	if(isDouble()) return (int)doubleData;
	return 0;
}
double CScriptVar::getDouble() {
	if(isDouble()) return doubleData;
	if(isInt()) return intData;
	if(isNull()) return 0;
	if(isUndefined()) return 0;
	return 0;	//or NaN?.
}
const string& CScriptVar::getString() {
	//Because we can't return a string that is generated on demand.
	//I should really just use char* :)
	static string s_null = "null";
	static string s_undefined = "undefined";
	if(isInt()) {
	char buffer[32];
	sprintf_s(buffer, sizeof(buffer), "%ld", intData);
	data = buffer;
	return data;
	}
	if(isDouble()) {
	char buffer[32];
	sprintf_s(buffer, sizeof(buffer), "%f", doubleData);
	data = buffer;
	return data;
	}
	if(isNull()) return s_null;
	if(isUndefined()) return s_undefined;
	//Are we just a string here?
	return data;
}
void CScriptVar::setInt(int val) {
	flags = (flags&~TINYJS_SCRIPTVAR_VARTYPEMASK) | TINYJS_SCRIPTVAR_INTEGER;
	intData = val;
	doubleData = 0;
	data = TINYJS_BLANK_DATA;
}
void CScriptVar::setDouble(double val) {
	flags = (flags&~TINYJS_SCRIPTVAR_VARTYPEMASK) | TINYJS_SCRIPTVAR_DOUBLE;
	doubleData = val;
	intData = 0;
	data = TINYJS_BLANK_DATA;
}
void CScriptVar::setString(const string& str) {
	//name sure it's not still a number or integer
	flags = (flags&~TINYJS_SCRIPTVAR_VARTYPEMASK) | TINYJS_SCRIPTVAR_STRING;
	data = str;
	intData = 0;
	doubleData = 0;
}
void CScriptVar::setUndefined() {
	//name sure it's not still a number or integer
	flags = (flags&~TINYJS_SCRIPTVAR_VARTYPEMASK) | TINYJS_SCRIPTVAR_UNDEFINED;
	data = TINYJS_BLANK_DATA;
	intData = 0;
	doubleData = 0;
	removeAllChildren();
}
void CScriptVar::setArray() {
	//name sure it's not still a number or integer
	flags = (flags&~TINYJS_SCRIPTVAR_VARTYPEMASK) | TINYJS_SCRIPTVAR_ARRAY;
	data = TINYJS_BLANK_DATA;
	intData = 0;
	doubleData = 0;
	removeAllChildren();
}
bool CScriptVar::equals(CScriptVar* v) {
	CScriptVar* resV = mathsOp(v, TINYJS_LEX_O_EQUAL);
	bool res = resV->getBool();
	delete resV;
	return res;
}
CScriptVar* CScriptVar::mathsOp(CScriptVar* b, int op) {
	CScriptVar* a = this;
	//Type equality check
	if(op == TINYJS_LEX_O_TYPEEQUAL || op == TINYJS_LEX_O_NTYPEEQUAL) {
		//check type first, then call again to check data
	bool eql = ((a->flags & TINYJS_SCRIPTVAR_VARTYPEMASK) ==
				(b->flags & TINYJS_SCRIPTVAR_VARTYPEMASK));
	if(eql) {
		CScriptVar* contents = a->mathsOp(b, TINYJS_LEX_O_EQUAL);
		if(!contents->getBool()) eql = false;
		if(!contents->refs) delete contents;
	}
				;
	if(op == TINYJS_LEX_O_TYPEEQUAL)
		return new CScriptVar(eql);
	else
		return new CScriptVar(!eql);
	}
	//do maths...
	if(a->isUndefined() && b->isUndefined()) {
	if(op == TINYJS_LEX_O_EQUAL) return new CScriptVar(true);
	else if(op == TINYJS_LEX_O_NEQUAL) return new CScriptVar(false);
	else return new CScriptVar();	//undefined
	} else if((a->isNumeric() || a->isUndefined()) &&
			(b->isNumeric() || b->isUndefined())) {
		if(!a->isDouble() && !b->isDouble()) {
			//use ints
			int da = a->getInt();
			int db = b->getInt();
			switch(op) {
				case '+': return new CScriptVar(da+db);
				case '-': return new CScriptVar(da-db);
				case '*': return new CScriptVar(da*db);
				case '/': return new CScriptVar(da/db);
				case '&': return new CScriptVar(da&db);
				case '|': return new CScriptVar(da|db);
				case '^': return new CScriptVar(da^db);
				case '%': return new CScriptVar(da%db);
				case TINYJS_LEX_O_EQUAL:	return new CScriptVar(da==db);
				case TINYJS_LEX_O_NEQUAL:	return new CScriptVar(da!=db);
				case '<':	return new CScriptVar(da<db);
				case TINYJS_LEX_O_LEQUAL:	return new CScriptVar(da<=db);
				case '>':	return new CScriptVar(da>db);
				case TINYJS_LEX_O_GEQUAL:	return new CScriptVar(da>=db);
				default: throw new CScriptException("Operation "+CScriptLex::getTokenStr(op)+" not supported on the Int datatype");
			}
		} else {
			//use doubles
			double da = a->getDouble();
			double db = b->getDouble();
			switch(op) {
				case '+': return new CScriptVar(da+db);
				case '-': return new CScriptVar(da-db);
				case '*': return new CScriptVar(da*db);
				case '/': return new CScriptVar(da/db);
				case TINYJS_LEX_O_EQUAL:	return new CScriptVar(da==db);
				case TINYJS_LEX_O_NEQUAL:	return new CScriptVar(da!=db);
				case '<':	return new CScriptVar(da<db);
				case TINYJS_LEX_O_LEQUAL:	return new CScriptVar(da<=db);
				case '>':	return new CScriptVar(da>db);
				case TINYJS_LEX_O_GEQUAL:	return new CScriptVar(da>=db);
				default: throw new CScriptException("Operation "+CScriptLex::getTokenStr(op)+" not supported on the Double datatype");
			}
		}
	} else if(a->isArray()) {
	//Just check pointers.
	switch(op) {
		case TINYJS_LEX_O_EQUAL: return new CScriptVar(a==b);
		case TINYJS_LEX_O_NEQUAL: return new CScriptVar(a!=b);
		default: throw new CScriptException("Operation "+CScriptLex::getTokenStr(op)+" not supported on the Array datatype");
	}
	} else if(a->isObject()) {
		//Just check pointers.
		switch(op) {
			case TINYJS_LEX_O_EQUAL: return new CScriptVar(a==b);
			case TINYJS_LEX_O_NEQUAL: return new CScriptVar(a!=b);
			default: throw new CScriptException("Operation "+CScriptLex::getTokenStr(op)+" not supported on the Object datatype");
		}
	} else {
	string da = a->getString();
	string db = b->getString();
		//use strings
	switch(op) {
		case '+':		return new CScriptVar(da+db, TINYJS_SCRIPTVAR_STRING);
		case TINYJS_LEX_O_EQUAL:	return new CScriptVar(da==db);
		case TINYJS_LEX_O_NEQUAL:	return new CScriptVar(da!=db);
		case '<':	return new CScriptVar(da<db);
		case TINYJS_LEX_O_LEQUAL:	return new CScriptVar(da<=db);
		case '>':	return new CScriptVar(da>db);
		case TINYJS_LEX_O_GEQUAL:	return new CScriptVar(da>=db);
		default: throw new CScriptException("Operation "+CScriptLex::getTokenStr(op)+" not supported on the string datatype");
	}
	}
	assert(0);
	return 0;
}
void CScriptVar::copySimpleData(CScriptVar* val) {
	data = val->data;
	intData = val->intData;
	doubleData = val->doubleData;
	flags = (flags & ~TINYJS_SCRIPTVAR_VARTYPEMASK) | (val->flags & TINYJS_SCRIPTVAR_VARTYPEMASK);
}
void CScriptVar::copyValue(CScriptVar* val) {
	if(val) {
	copySimpleData(val);
		//remove all current children
	removeAllChildren();
		//copy children of 'val'
	CScriptVarLink* child = val->firstChild;
	while(child) {
		CScriptVar* copied;
		//don't copy the 'parent' object...
		if(child->name != TINYJS_PROTOTYPE_CLASS)
		copied = child->var->deepCopy();
		else
		copied = child->var;

		addChild(child->name, copied);

		child = child->nextSibling;
	}
	} else {
	setUndefined();
	}
}
CScriptVar* CScriptVar::deepCopy() {
	CScriptVar* newVar = new CScriptVar();
	newVar->copySimpleData(this);
	//copy children
	CScriptVarLink* child = firstChild;
	while(child) {
		CScriptVar* copied;
		//don't copy the 'parent' object...
		if(child->name != TINYJS_PROTOTYPE_CLASS)
		copied = child->var->deepCopy();
		else
		copied = child->var;

		newVar->addChild(child->name, copied);
		child = child->nextSibling;
	}
	return newVar;
}
void CScriptVar::trace(string indentStr, const string& name) {
	TRACE("%s'%s' = '%s' %s\n",
		indentStr.c_str(),
		name.c_str(),
		getString().c_str(),
		getFlagsAsString().c_str());
	string indent = indentStr+" ";
	CScriptVarLink* link = firstChild;
	while(link) {
		link->var->trace(indent, link->name);
		link = link->nextSibling;
	}
}
string CScriptVar::getFlagsAsString() {
	string flagstr = "";
	if(flags & TINYJS_SCRIPTVAR_FUNCTION) { flagstr = flagstr + "FUNCTION "; }
	if(flags & TINYJS_SCRIPTVAR_OBJECT)   { flagstr = flagstr + "OBJECT "; }
	if(flags & TINYJS_SCRIPTVAR_ARRAY)    { flagstr = flagstr + "ARRAY "; }
	if(flags & TINYJS_SCRIPTVAR_NATIVE)   { flagstr = flagstr + "NATIVE "; }
	if(flags & TINYJS_SCRIPTVAR_DOUBLE)   { flagstr = flagstr + "DOUBLE "; }
	if(flags & TINYJS_SCRIPTVAR_INTEGER)  { flagstr = flagstr + "INTEGER "; }
	if(flags & TINYJS_SCRIPTVAR_STRING)   { flagstr = flagstr + "STRING "; }
	return flagstr;
}
string CScriptVar::getParsableString() {
	//Numbers can just be put in directly
	if(isNumeric()) {
		return getString();
	}
	if(isFunction()) {
		ostringstream funcStr;
		funcStr << "function (";
		//get list of parameters
		CScriptVarLink* link = firstChild;
		while(link) {
			funcStr << link->name;
			if(link->nextSibling) { funcStr << ","; }
			link = link->nextSibling;
		}
		//add function body
		funcStr << ") " << getString();
		return funcStr.str();
	}
	//if it is a string then we quote it
	if(isString()) {
		return getJSString(getString());
	}
	if(isNull()) {
		return "null";
	}
	return "undefined";
}
void CScriptVar::getJSON(ostringstream& destination, const string linePrefix) {
	if(isObject()) {
	string indentedLinePrefix = linePrefix+"  ";
		//children - handle with bracketed list
	destination << "{ \n";
	CScriptVarLink* link = firstChild;
	while(link) {
		destination << indentedLinePrefix;
		destination  << getJSString(link->name);
		destination  << " : ";
		link->var->getJSON(destination, indentedLinePrefix);
		link = link->nextSibling;
		if(link) {
		destination  << ",\n";
		}
	}
	destination << "\n" << linePrefix << "}";
	} else if(isArray()) {
	string indentedLinePrefix = linePrefix+"  ";
	destination << "[\n";
	int len = getArrayLength();
	if(len>10000) len=10000;	//we don't want to get stuck here!

	for(int i=0;i<len;i++) {
		getArrayIndex(i)->getJSON(destination, indentedLinePrefix);
		if(i<len-1) destination	<< ",\n";
	}

	destination << "\n" << linePrefix << "]";
	} else {
		//no children or a function... just write value directly
	destination << getParsableString();
	}
}
void CScriptVar::setCallback(JSCallback callback, void* userdata) {
	jsCallback = callback;
	jsCallbackUserData = userdata;
}
CScriptVar* CScriptVar::ref() {
	refs++;
	return this;
}
void CScriptVar::unref() {
	if(refs<=0) printf("OMFG, we have unreffed too far!\n");
	if((--refs)==0) {
	delete this;
	}
}
int CScriptVar::getRefs() {
	return refs;
}
//-----------------------------------------------------------------------------
//CSCRIPT
CTinyJS::CTinyJS() {
	l = 0;
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
	assert(!l);
	scopes.clear();
	stringClass->unref();
	arrayClass->unref();
	objectClass->unref();
	root->unref();
}
void CTinyJS::trace() {
	root->trace();
}
void CTinyJS::execute(const string& code) {
	CScriptLex* oldLex = l;
	vector<CScriptVar*> oldScopes = scopes;
	l = new CScriptLex(code);
#ifdef TINYJS_CALL_STACK
	call_stack.clear();
#endif//TINYJS_CALL_STACK
	scopes.clear();
	scopes.push_back(root);
	try {
		bool execute = true;
		while(l->tk) statement(execute);
	} catch(CScriptException* e) {
		ostringstream msg;
		msg << "Error " << e->text;
#ifdef TINYJS_CALL_STACK
		for(int i=(int)call_stack.size()-1;i>=0;i--)
		msg << "\n" << i << ": " << call_stack.at(i);
#endif//TINYJS_CALL_STACK
		msg << " at " << l->getPosition();
		delete l;
		l = oldLex;

		throw new CScriptException(msg.str());
	}
	delete l;
	l = oldLex;
	scopes = oldScopes;
}
CScriptVarLink CTinyJS::evaluateComplex(const string& code) {
	CScriptLex* oldLex = l;
	vector<CScriptVar*> oldScopes = scopes;

	l = new CScriptLex(code);
#ifdef TINYJS_CALL_STACK
	call_stack.clear();
#endif//TINYJS_CALL_STACK
	scopes.clear();
	scopes.push_back(root);
	CScriptVarLink* v = 0;
	try {
		bool execute = true;
		do {
		CLEAN(v);
		v = base(execute);
		if(l->tk!=TINYJS_LEX_EOF) l->match(';');
		} while(l->tk!=TINYJS_LEX_EOF);
	} catch(CScriptException* e) {
		ostringstream msg;
		msg << "Error " << e->text;
#ifdef TINYJS_CALL_STACK
		for(int i=(int)call_stack.size()-1;i>=0;i--) {
			msg << "\n" << i << ": " << call_stack.at(i);
		}
#endif//TINYJS_CALL_STACK
		msg << " at " << l->getPosition();
		delete l;
		l = oldLex;

		throw new CScriptException(msg.str());
	}
	delete l;
	l = oldLex;
	scopes = oldScopes;

	if(v) {
		CScriptVarLink r =* v;
		CLEAN(v);
		return r;
	}
	//return undefined...
	return CScriptVarLink(new CScriptVar());
}
string CTinyJS::evaluate(const string& code) {
	return evaluateComplex(code).var->getString();
}
void CTinyJS::parseFunctionArguments(CScriptVar* funcVar) {
	l->match('(');
	while(l->tk!=')') {
	funcVar->addChildNoDup(l->tkStr);
	l->match(TINYJS_LEX_ID);
	if(l->tk!=')') l->match(',');
	}
	l->match(')');
}
void CTinyJS::addNative(const string& funcDesc, JSCallback ptr, void* userdata) {
	CScriptLex* oldLex = l;
	l = new CScriptLex(funcDesc);

	CScriptVar* base = root;

	l->match(TINYJS_LEX_R_FUNCTION);
	string funcName = l->tkStr;
	l->match(TINYJS_LEX_ID);
	//Check for dots, we might want to do something like function String.substring ...
	while(l->tk == '.') {
	l->match('.');
	CScriptVarLink* link = base->findChild(funcName);
		//if it doesn't exist, make an object class
	if(!link) link = base->addChild(funcName, new CScriptVar(TINYJS_BLANK_DATA, TINYJS_SCRIPTVAR_OBJECT));
	base = link->var;
	funcName = l->tkStr;
	l->match(TINYJS_LEX_ID);
	}

	CScriptVar* funcVar = new CScriptVar(TINYJS_BLANK_DATA, TINYJS_SCRIPTVAR_FUNCTION | TINYJS_SCRIPTVAR_NATIVE);
	funcVar->setCallback(ptr, userdata);
	parseFunctionArguments(funcVar);
	delete l;
	l = oldLex;

	base->addChild(funcName, funcVar);
}
CScriptVarLink* CTinyJS::parseFunctionDefinition() {
	//Actually parse a function...
	l->match(TINYJS_LEX_R_FUNCTION);
	string funcName = TINYJS_TEMP_NAME;
	//We can have functions without names.
	if(l->tk==TINYJS_LEX_ID) {
	funcName = l->tkStr;
	l->match(TINYJS_LEX_ID);
	}
	CScriptVarLink* funcVar = new CScriptVarLink(new CScriptVar(TINYJS_BLANK_DATA, TINYJS_SCRIPTVAR_FUNCTION), funcName);
	parseFunctionArguments(funcVar->var);
	int funcBegin = l->tokenStart;
	bool noexecute = false;
	block(noexecute);
	funcVar->var->data = l->getSubString(funcBegin);
	return funcVar;
}
//Handle a function call (assumes we've parsed the function name and we're on the start bracket).
//'parent' is the object that contains this method, if there was one (otherwise it's just a normnal function).
CScriptVarLink* CTinyJS::functionCall(bool& execute, CScriptVarLink* function, CScriptVar* parent) {
	if(execute) {
		if(!function->var->isFunction()) {
			string errorMsg = "Expecting '";
			errorMsg = errorMsg + function->name + "' to be a function";
			throw new CScriptException(errorMsg.c_str());
		}
		l->match('(');
		//create a new symbol table entry for execution of this function
		CScriptVar* functionRoot = new CScriptVar(TINYJS_BLANK_DATA, TINYJS_SCRIPTVAR_FUNCTION);
		if(parent) {
			functionRoot->addChildNoDup("this", parent);
		}
		//grab in all parameters
		CScriptVarLink* v = function->var->firstChild;
		while(v) {
			CScriptVarLink* value = base(execute);
			if(execute) {
				if(value->var->isBasic()) {
					//pass by value
				functionRoot->addChild(v->name, value->var->deepCopy());
				} else {
					//pass by reference
				functionRoot->addChild(v->name, value->var);
				}
			}
			CLEAN(value);
			if(l->tk!=')') l->match(',');
			v = v->nextSibling;
		}
		l->match(')');
		//setup a return variable
		CScriptVarLink* returnVar = NULL;
		//execute function!
		//add the function's execute space to the symbol table so we can recurse
		CScriptVarLink* returnVarLink = functionRoot->addChild(TINYJS_RETURN_VAR);
		scopes.push_back(functionRoot);
#ifdef TINYJS_CALL_STACK
		call_stack.push_back(function->name + " from " + l->getPosition());
#endif//TINYJS_CALL_STACK
		if(function->var->isNative()) {
			assert(function->var->jsCallback);
			function->var->jsCallback(functionRoot, function->var->jsCallbackUserData);
		} else {
			//We just want to execute the block, but something could have messed up and left us with the wrong ScriptLex, so we want to be careful here...
			CScriptException* exception = 0;
			CScriptLex* oldLex = l;
			CScriptLex* newLex = new CScriptLex(function->var->getString());
			l = newLex;
			try {
			block(execute);
				//because return will probably have called this, and set execute to false
			execute = true;
			} catch(CScriptException* e) {
			exception = e;
			}
			delete newLex;
			l = oldLex;

			if(exception)
			throw exception;
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
		l->match('(');
		while(l->tk != ')') {
			CScriptVarLink* value = base(execute);
			CLEAN(value);
			if(l->tk!=')') l->match(',');
		}
		l->match(')');
		if(l->tk == '{') {	//TODO: why is this here?
			block(execute);
		}
		//Function will be a blank scriptvarlink if we're not executing, so just return it rather than an alloc/free.
		return function;
	}
}
CScriptVarLink* CTinyJS::factor(bool& execute) {
	if(l->tk=='(') {
		l->match('(');
		CScriptVarLink* a = base(execute);
		l->match(')');
		return a;
	}
	if(l->tk==TINYJS_LEX_R_TRUE) {
		l->match(TINYJS_LEX_R_TRUE);
		return new CScriptVarLink(new CScriptVar(1));
	}
	if(l->tk==TINYJS_LEX_R_FALSE) {
		l->match(TINYJS_LEX_R_FALSE);
		return new CScriptVarLink(new CScriptVar(0));
	}
	if(l->tk==TINYJS_LEX_R_NULL) {
		l->match(TINYJS_LEX_R_NULL);
		return new CScriptVarLink(new CScriptVar(TINYJS_BLANK_DATA,TINYJS_SCRIPTVAR_NULL));
	}
	if(l->tk==TINYJS_LEX_R_UNDEFINED) {
		l->match(TINYJS_LEX_R_UNDEFINED);
		return new CScriptVarLink(new CScriptVar(TINYJS_BLANK_DATA,TINYJS_SCRIPTVAR_UNDEFINED));
	}
	if(l->tk==TINYJS_LEX_ID) {
		CScriptVarLink* a = execute ? findInScopes(l->tkStr) : new CScriptVarLink(new CScriptVar());
		//printf("0x%08X for %s at %s\n", (unsigned int)a, l->tkStr.c_str(), l->getPosition().c_str());
		//The parent if we're executing a method call.
		CScriptVar* parent = 0;

		if(execute && !a) {
			//Variable doesn't exist! JavaScript says we should create it.
			//(We won't add it here. This is done in the assignment operator.)
			a = new CScriptVarLink(new CScriptVar(), l->tkStr);
		}
		l->match(TINYJS_LEX_ID);
		while(l->tk=='(' || l->tk=='.' || l->tk=='[') {
			if(l->tk=='(') {		//------------------------------------- Function Call.
				a = functionCall(execute, a, parent);
			} else if(l->tk == '.') {	//------------------------------------- Record Access.
				l->match('.');
				if(execute) {
				const string& name = l->tkStr;
				CScriptVarLink* child = a->var->findChild(name);
				if(!child) child = findInParentClasses(a->var, name);
				if(!child) {
					//If we haven't found this defined yet, use the built-in 'length' properly.
					if(a->var->isArray() && name == "length") {
					int l = a->var->getArrayLength();
					child = new CScriptVarLink(new CScriptVar(l));
					} else if(a->var->isString() && name == "length") {
					int l = a->var->getString().size();
					child = new CScriptVarLink(new CScriptVar(l));
					} else {
					child = a->var->addChild(name);
					}
				}
				parent = a->var;
				a = child;
				}
				l->match(TINYJS_LEX_ID);
			} else if(l->tk == '[') {	//------------------------------------- Array Access.
				l->match('[');
				CScriptVarLink* index = base(execute);
				l->match(']');
				if(execute) {
				CScriptVarLink* child = a->var->findChildOrCreate(index->var->getString());
				parent = a->var;
				a = child;
				}
				CLEAN(index);
			} else {
				assert(0);
			}
		}
		return a;
	}
	if(l->tk==TINYJS_LEX_L_INT || l->tk==TINYJS_LEX_L_FLOAT) {
		CScriptVar* a = new CScriptVar(l->tkStr,
			((l->tk==TINYJS_LEX_L_INT)?TINYJS_SCRIPTVAR_INTEGER:TINYJS_SCRIPTVAR_DOUBLE));
		l->match(l->tk);
		return new CScriptVarLink(a);
	}
	if(l->tk==TINYJS_LEX_L_STR) {
		CScriptVar* a = new CScriptVar(l->tkStr, TINYJS_SCRIPTVAR_STRING);
		l->match(TINYJS_LEX_L_STR);
		return new CScriptVarLink(a);
	}
	if(l->tk=='{') {
		CScriptVar* contents = new CScriptVar(TINYJS_BLANK_DATA, TINYJS_SCRIPTVAR_OBJECT);
		//JSON-style object definition.
		l->match('{');
		while(l->tk != '}') {
		string id = l->tkStr;
		//We only allow strings or IDs on the left hand side of an initialisation.
		if(l->tk==TINYJS_LEX_L_STR) {
			l->match(TINYJS_LEX_L_STR);
		} else {
			l->match(TINYJS_LEX_ID);
		}
		l->match(':');
		if(execute) {
			CScriptVarLink* a = base(execute);
			contents->addChild(id, a->var);
			CLEAN(a);
		}
			//no need to clean here, as it will definitely be used
		if(l->tk != '}') l->match(',');
		}

		l->match('}');
		return new CScriptVarLink(contents);
	}
	if(l->tk=='[') {
		CScriptVar* contents = new CScriptVar(TINYJS_BLANK_DATA, TINYJS_SCRIPTVAR_ARRAY);
		//JSON-style array.
		l->match('[');
		int idx = 0;
		while(l->tk != ']') {
		if(execute) {
			char idx_str[16];	//big enough for 2^32
			sprintf_s(idx_str, sizeof(idx_str), "%d",idx);

			CScriptVarLink* a = base(execute);
			contents->addChild(idx_str, a->var);
			CLEAN(a);
		}
			//no need to clean here, as it will definitely be used
		if(l->tk != ']') l->match(',');
		idx++;
		}
		l->match(']');
		return new CScriptVarLink(contents);
	}
	if(l->tk==TINYJS_LEX_R_FUNCTION) {
	CScriptVarLink* funcVar = parseFunctionDefinition();
		if(funcVar->name != TINYJS_TEMP_NAME)
		TRACE("Functions not defined at statement-level are not meant to have a name");
		return funcVar;
	}
	if(l->tk==TINYJS_LEX_R_NEW) {
		//new -> create a new object
	l->match(TINYJS_LEX_R_NEW);
	const string& className = l->tkStr;
	if(execute) {
		CScriptVarLink* objClassOrFunc = findInScopes(className);
		if(!objClassOrFunc) {
		TRACE("%s is not a valid class name", className.c_str());
		return new CScriptVarLink(new CScriptVar());
		}
		l->match(TINYJS_LEX_ID);
		CScriptVar* obj = new CScriptVar(TINYJS_BLANK_DATA, TINYJS_SCRIPTVAR_OBJECT);
		CScriptVarLink* objLink = new CScriptVarLink(obj);
		if(objClassOrFunc->var->isFunction()) {
		CLEAN(functionCall(execute, objClassOrFunc, obj));
		} else {
		obj->addChild(TINYJS_PROTOTYPE_CLASS, objClassOrFunc->var);
		if(l->tk == '(') {
			l->match('(');
			l->match(')');
		}
		}
		return objLink;
	} else {
		l->match(TINYJS_LEX_ID);
		if(l->tk == '(') {
		l->match('(');
		l->match(')');
		}
	}
	}
	//Nothing we can do here... just hope it's the end...
	l->match(TINYJS_LEX_EOF);
	return 0;
}
CScriptVarLink* CTinyJS::unary(bool& execute) {
	CScriptVarLink* a;
	if(l->tk=='!') {
		l->match('!');	//binary not
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
	while(l->tk=='*' || l->tk=='/' || l->tk=='%') {
		int op = l->tk;
		l->match(l->tk);
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
	if(l->tk=='-') {
		l->match('-');
		negate = true;
	}
	CScriptVarLink* a = term(execute);
	if(negate) {
		CScriptVar zero(0);
		CScriptVar* res = zero.mathsOp(a->var, '-');
		CREATE_LINK(a, res);
	}

	while(l->tk=='+' || l->tk=='-' ||
		l->tk==TINYJS_LEX_O_PLUSPLUS || l->tk==TINYJS_LEX_O_MINUSMINUS) {
		int op = l->tk;
		l->match(l->tk);
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
	if(l->tk==TINYJS_LEX_O_LSHIFT || l->tk==TINYJS_LEX_O_RSHIFT || l->tk==TINYJS_LEX_O_RSHIFTUNSIGNED) {
	int op = l->tk;
	l->match(op);
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
	while(l->tk==TINYJS_LEX_O_EQUAL || l->tk==TINYJS_LEX_O_NEQUAL ||
		l->tk==TINYJS_LEX_O_TYPEEQUAL || l->tk==TINYJS_LEX_O_NTYPEEQUAL ||
		l->tk==TINYJS_LEX_O_LEQUAL || l->tk==TINYJS_LEX_O_GEQUAL ||
		l->tk=='<' || l->tk=='>') {
		int op = l->tk;
		l->match(l->tk);
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
	while(l->tk=='&' || l->tk=='|' || l->tk=='^' || l->tk==TINYJS_LEX_O_ANDAND || l->tk==TINYJS_LEX_O_OROR) {
		bool noexecute = false;
		int op = l->tk;
		l->match(l->tk);
		bool shortCircuit = false;
		bool boolean = false;
		//if we have short-circuit ops, then if we know the outcome
		//we don't bother to execute the other op. Even if not
		//we need to tell mathsOp it's an & or |
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
	if(l->tk=='?') {
		l->match('?');
		if(!execute) {
			CLEAN(lhs);
			CLEAN(base(noexec));
			l->match(':');
			CLEAN(base(noexec));
		} else {
			bool first = lhs->var->getBool();
			CLEAN(lhs);
			if(first) {
				lhs = base(execute);
				l->match(':');
				CLEAN(base(noexec));
			} else {
				CLEAN(base(noexec));
				l->match(':');
				lhs = base(execute);
			}
		}
	}
	return lhs;
}
CScriptVarLink* CTinyJS::base(bool& execute) {
	CScriptVarLink* lhs = ternary(execute);
	if(l->tk=='=' || l->tk==TINYJS_LEX_O_PLUSEQUAL || l->tk==TINYJS_LEX_O_MINUSEQUAL) {
		//If we're assigning to this and we don't have a parent, add it to the symbol table root as per JavaScript.
		if(execute && !lhs->owned) {
		if(lhs->name.length()>0) {
			CScriptVarLink* realLhs = root->addChildNoDup(lhs->name, lhs->var);
			CLEAN(lhs);
			lhs = realLhs;
		} else
			TRACE("Trying to assign to an un-named type\n");
		}

		int op = l->tk;
		l->match(l->tk);
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
	l->match('{');
	if(execute) {
		while(l->tk && l->tk!='}')
			statement(execute);
		l->match('}');
	} else {
		//fast skip of blocks
		int brackets = 1;
		while(l->tk && brackets) {
			if(l->tk == '{') brackets++;
			if(l->tk == '}') brackets--;
			l->match(l->tk);
		}
	}
}
void CTinyJS::statement(bool& execute) {
	if(l->tk==TINYJS_LEX_ID ||
		l->tk==TINYJS_LEX_L_INT ||
		l->tk==TINYJS_LEX_L_FLOAT ||
		l->tk==TINYJS_LEX_L_STR ||
		l->tk=='-') {
		//Execute a simple statement that only contains basic arithmetic...
		CLEAN(base(execute));
		l->match(';');
	} else if(l->tk=='{') {
		//A block of code.
		block(execute);
	} else if(l->tk==';') {
		//Empty statement - to allow things like ;;;
		l->match(';');
	} else if(l->tk==TINYJS_LEX_R_VAR) {
		//Variable creation.
		//TODO - We need a better way of parsing the left hand side.
		//       Maybe just have a flag called can_create_var that we set and then we parse as if we're doing a normal equals.
		l->match(TINYJS_LEX_R_VAR);
		while(l->tk != ';') {
		CScriptVarLink* a = 0;
		if(execute) {
			a = scopes.back()->findChildOrCreate(l->tkStr);
		}
		l->match(TINYJS_LEX_ID);
		//Now do stuff defined with dots.
		while(l->tk == '.') {
			l->match('.');
			if(execute) {
				CScriptVarLink* lastA = a;
				a = lastA->var->findChildOrCreate(l->tkStr);
			}
			l->match(TINYJS_LEX_ID);
		}
		//Sort out initialiser.
		if(l->tk == '=') {
			l->match('=');
			CScriptVarLink* var = base(execute);
			if(execute)
				a->replaceWith(var);
			CLEAN(var);
		}
		if(l->tk != ';')
			l->match(',');
		}
		l->match(';');
	} else if(l->tk==TINYJS_LEX_R_IF) {
		l->match(TINYJS_LEX_R_IF);
		l->match('(');
		CScriptVarLink* var = base(execute);
		l->match(')');
		bool cond = execute && var->var->getBool();
		CLEAN(var);
		bool noexecute = false;	//Because we need to be abl;e to write to it.
		statement(cond ? execute : noexecute);
		if(l->tk==TINYJS_LEX_R_ELSE) {
			l->match(TINYJS_LEX_R_ELSE);
			statement(cond ? noexecute : execute);
		}
	} else if(l->tk==TINYJS_LEX_R_WHILE) {
		//We do repetition by pulling out the string representing our statement
		//there's definitely some opportunity for optimisation here
		l->match(TINYJS_LEX_R_WHILE);
		l->match('(');
		int whileCondStart = l->tokenStart;
		bool noexecute = false;
		CScriptVarLink* cond = base(execute);
		bool loopCond = execute && cond->var->getBool();
		CLEAN(cond);
		CScriptLex* whileCond = l->getSubLex(whileCondStart);
		l->match(')');
		int whileBodyStart = l->tokenStart;
		statement(loopCond ? execute : noexecute);
		CScriptLex* whileBody = l->getSubLex(whileBodyStart);
		CScriptLex* oldLex = l;
		int loopCount = TINYJS_LOOP_MAX_ITERATIONS;
		while(loopCond && loopCount-->0) {
			whileCond->reset();
			l = whileCond;
			cond = base(execute);
			loopCond = execute && cond->var->getBool();
			CLEAN(cond);
			if(loopCond) {
				whileBody->reset();
				l = whileBody;
				statement(execute);
			}
		}
		l = oldLex;
		delete whileCond;
		delete whileBody;

		if(loopCount<=0) {
			root->trace();
			TRACE("WHILE Loop exceeded %d iterations at %s\n", TINYJS_LOOP_MAX_ITERATIONS, l->getPosition().c_str());
			throw new CScriptException("LOOP_ERROR");
		}
	} else if(l->tk==TINYJS_LEX_R_FOR) {
		l->match(TINYJS_LEX_R_FOR);
		l->match('(');
		statement(execute);	//initialisation
		//l->match(';');
		int forCondStart = l->tokenStart;
		bool noexecute = false;
		CScriptVarLink* cond = base(execute);	//condition
		bool loopCond = execute && cond->var->getBool();
		CLEAN(cond);
		CScriptLex* forCond = l->getSubLex(forCondStart);
		l->match(';');
		int forIterStart = l->tokenStart;
		CLEAN(base(noexecute));	//iterator
		CScriptLex* forIter = l->getSubLex(forIterStart);
		l->match(')');
		int forBodyStart = l->tokenStart;
		statement(loopCond ? execute : noexecute);
		CScriptLex* forBody = l->getSubLex(forBodyStart);
		CScriptLex* oldLex = l;
		if(loopCond) {
			forIter->reset();
			l = forIter;
			CLEAN(base(execute));
		}
		int loopCount = TINYJS_LOOP_MAX_ITERATIONS;
		while(execute && loopCond && loopCount-->0) {
			forCond->reset();
			l = forCond;
			cond = base(execute);
			loopCond = cond->var->getBool();
			CLEAN(cond);
			if(execute && loopCond) {
				forBody->reset();
				l = forBody;
				statement(execute);
			}
			if(execute && loopCond) {
				forIter->reset();
				l = forIter;
				CLEAN(base(execute));
			}
		}
		l = oldLex;
		delete forCond;
		delete forIter;
		delete forBody;
		if(loopCount<=0) {
			root->trace();
			TRACE("FOR Loop exceeded %d iterations at %s\n", TINYJS_LOOP_MAX_ITERATIONS, l->getPosition().c_str());
			throw new CScriptException("LOOP_ERROR");
		}
	} else if(l->tk==TINYJS_LEX_R_RETURN) {
		l->match(TINYJS_LEX_R_RETURN);
		CScriptVarLink* result = 0;
		if(l->tk != ';')
		result = base(execute);
		if(execute) {
		CScriptVarLink* resultVar = scopes.back()->findChild(TINYJS_RETURN_VAR);
		if(resultVar)
			resultVar->replaceWith(result);
		else
			TRACE("RETURN statement, but not in a function.\n");
		execute = false;
		}
		CLEAN(result);
		l->match(';');
	} else if(l->tk==TINYJS_LEX_R_FUNCTION) {
		CScriptVarLink* funcVar = parseFunctionDefinition();
		if(execute) {
		if(funcVar->name == TINYJS_TEMP_NAME)
			TRACE("Functions defined at statement-level are meant to have a name\n");
		else
			scopes.back()->addChildNoDup(funcVar->name, funcVar->var);
		}
		CLEAN(funcVar);
	} else l->match(TINYJS_LEX_EOF);
}
//Get the given variable specified by a path (var1.var2.etc), or return 0
CScriptVar* CTinyJS::getScriptVariable(const string& path) {
	//traverse path
	size_t prevIdx = 0;
	size_t thisIdx = path.find('.');
	if(thisIdx == string::npos) thisIdx = path.length();
	CScriptVar* var = root;
	while(var && prevIdx<path.length()) {
		string el = path.substr(prevIdx, thisIdx-prevIdx);
		CScriptVarLink* varl = var->findChild(el);
		var = varl?varl->var:0;
		prevIdx = thisIdx+1;
		thisIdx = path.find('.', prevIdx);
		if(thisIdx == string::npos) thisIdx = path.length();
	}
	return var;
}
//Get the value of the given variable, or return 0
const string* CTinyJS::getVariable(const string& path) {
	CScriptVar* var = getScriptVariable(path);
	//return result
	if(var)
		return &var->getString();
	else
		return 0;
}
//set the value of the given variable, return trur if it exists and gets set
bool CTinyJS::setVariable(const string& path, const string& varData) {
	CScriptVar* var = getScriptVariable(path);
	//return result
	if(var) {
		if(var->isInt())
			var->setInt((int)strtol(varData.c_str(),0,0));
		else if(var->isDouble())
			var->setDouble(strtod(varData.c_str(),0));
		else
			var->setString(varData.c_str());
		return true;
	}
	else
		return false;
}
//Finds a child, looking recursively up the scopes
CScriptVarLink* CTinyJS::findInScopes(const string& childName) {
	for(int s=scopes.size()-1;s>=0;s--) {
	CScriptVarLink* v = scopes[s]->findChild(childName);
	if(v) return v;
	}
	return NULL;
}
//Look up in any parent classes of the given object
CScriptVarLink* CTinyJS::findInParentClasses(CScriptVar* object, const string& name) {
	//Look for links to actual parent classes
	CScriptVarLink* parentClass = object->findChild(TINYJS_PROTOTYPE_CLASS);
	while(parentClass) {
	CScriptVarLink* implementation = parentClass->var->findChild(name);
	if(implementation) return implementation;
	parentClass = parentClass->var->findChild(TINYJS_PROTOTYPE_CLASS);
	}
	//else fake it for strings and finally objects
	if(object->isString()) {
	CScriptVarLink* implementation = stringClass->findChild(name);
	if(implementation) return implementation;
	}
	if(object->isArray()) {
	CScriptVarLink* implementation = arrayClass->findChild(name);
	if(implementation) return implementation;
	}
	CScriptVarLink* implementation = objectClass->findChild(name);
	if(implementation) return implementation;
	return 0;
}
