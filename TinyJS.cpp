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
//	ST_TinyJS
//*****************************************************************************
/*static*/ ST_TinyJS* TinyJS_new() {
	ST_TinyJS* _this = new ST_TinyJS();
	_this->lex       = NULL;
	_this->scopes    = NULL;
	_this->callStack = NULL;
	_this->root      = TinyJS_Var_newObject();
	//Add built-in classes.
	_this->stringClass = TinyJS_Var_newObject();
	_this->arrayClass  = TinyJS_Var_newObject();
	_this->objectClass = TinyJS_Var_newObject();
	_this->root->TinyJS_Var_addChild("String", _this->stringClass);
	_this->root->TinyJS_Var_addChild("Array",  _this->arrayClass);
	_this->root->TinyJS_Var_addChild("Object", _this->objectClass);
	return _this;
}
//-----------------------------------------------------------------------------
void TinyJS_execute(ST_TinyJS* _this, const char* code) {
	ST_TinyJS_Context* pSavedContext = this->TinyJS_saveContext();
	{
		//{{InitContext
		this->lex       = TinyJS_Lex_new(code, 0, strlen(code));
		this->scopes    = g_slist_prepend(NULL, this->root);
		this->callStack = NULL;
		//}}InitContext
		SEH_try {
			int bExec = 1;
			while(this->lex->tk) { this->TinyJS_statement(&bExec); }
		} SEH_catch(TinyJS_Exception) {
			const char* msg = this->TinyJS_stackTrace(SEH_info.msg);
			this->TinyJS_restoreContext(pSavedContext);
			SEH_throw_msg(TinyJS_Exception, msg);
		} SEH_end
	}
	this->TinyJS_restoreContext(pSavedContext);
}
//-----------------------------------------------------------------------------
//Evaluate the given code and return a javascript object, useful for(dangerous) JSON parsing.
//If nothing to return, will return 'undefined' variable type.
ST_TinyJS_Var* TinyJS_evaluate(ST_TinyJS* _this, const char* code) {
	ST_TinyJS_VarLink* l = NULL;
	ST_TinyJS_Context* pSavedContext = this->TinyJS_saveContext();
	{
		//{{InitContext
		this->lex       = TinyJS_Lex_new(code, 0, strlen(code));
		this->scopes    = g_slist_prepend(NULL, this->root);
		this->callStack = NULL;
		//}}InitContext
		SEH_try {
			int bExec = 1;
			do {
				l = this->TinyJS_base(&bExec);
				if(this->lex->tk != TINYJS_LEX_EOF) { this->lex->TinyJS_Lex_match(';'); }
			} while(this->lex->tk != TINYJS_LEX_EOF);
		} SEH_catch(TinyJS_Exception) {
			const char* msg = this->TinyJS_stackTrace(SEH_info.msg);
			this->TinyJS_restoreContext(pSavedContext);
			SEH_throw_msg(TinyJS_Exception, msg);
		} SEH_end
	}
	this->TinyJS_restoreContext(pSavedContext);
	return l ? l->var : TinyJS_Var_newUndefined();
}
//-----------------------------------------------------------------------------
//Add a native function to be called from TinyJS.
//example:
//│void scRandInt(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) { ... }
//│tinyJS->TinyJS_addNative("function randInt(min, max)", scRandInt, NULL);
//or
//│void scSubstring(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) { ... }
//│tinyJS->TinyJS_addNative("function String.substring(lo, hi)", scSubstring, NULL);
void TinyJS_addNative(ST_TinyJS* _this, const char* funcDesc, TinyJS_Callback* callback, void* userData) {
	ST_TinyJS_Context* pSavedContext = this->TinyJS_saveContext();
	{
		//{{InitContext
		this->lex = TinyJS_Lex_new(funcDesc, 0, strlen(funcDesc));
		//}}InitContext
		ST_TinyJS_Var* base = this->root;
		this->lex->TinyJS_Lex_match(TINYJS_LEX_R_FUNCTION);
		const char* funcName = this->lex->tkStr;
		this->lex->TinyJS_Lex_match(TINYJS_LEX_ID);
		//Check for dots, we might want to do something like function String.substring ...
		while(this->lex->tk == '.') {
			this->lex->TinyJS_Lex_match('.');
			ST_TinyJS_VarLink* l = base->TinyJS_Var_findChild(funcName);
			//If it doesn't exist, make an object class.
			if(!l) { l = base->TinyJS_Var_addChild(funcName, TinyJS_Var_newObject()); }
			base = l->var;
			funcName = this->lex->tkStr;
			this->lex->TinyJS_Lex_match(TINYJS_LEX_ID);
		}
		ST_TinyJS_Var* funcVar = TinyJS_Var_newNative(callback, userData);
		this->TinyJS_parseFunctionArguments(funcVar);
		base->TinyJS_Var_addChild(funcName, funcVar);
	}
	this->TinyJS_restoreContext(pSavedContext);
}
//-----------------------------------------------------------------------------
//Send all variables to stdout.
void TinyJS_trace(ST_TinyJS* _this, const char* indent) {
	this->root->TinyJS_Var_trace(indent, "root");
}
//-----------------------------------------------------------------------------
//Handle a function call (assumes we've parsed the function name and we're on the start bracket).
//'obj' is the object that contains this method, if there was one (otherwise it's just a normnal function).
ST_TinyJS_VarLink* TinyJS_functionCall(ST_TinyJS* _this, int* pExec, ST_TinyJS_VarLink* func, ST_TinyJS_Var* obj/*NULL可*/) {
	if(*pExec) {
		if(!func->var->TinyJS_Var_isFunction()) {
			SEH_throw_msg(TinyJS_Exception, strdup_printf("Expecting '%s' to be a function.", func->name));
		}
		this->lex->TinyJS_Lex_match('(');
		//Create a new symbol table entry for execution of this function.
		ST_TinyJS_Var* funcRoot = TinyJS_Var_newObject();
		if(obj) {
			funcRoot->TinyJS_Var_addChild("this", obj);
		}
		//Grab in all parameters.
		GSList/*<ST_TinyJS_VarLink*>*/* list = func->var->firstChild;
		while(list) {
			ST_TinyJS_VarLink* l = (ST_TinyJS_VarLink*)list->data;
			ST_TinyJS_VarLink* tmp = this->TinyJS_base(pExec);
			if(*pExec) {
				if(tmp->var->TinyJS_Var_isPrimitive()) {
					//pass by value
					funcRoot->TinyJS_Var_addChild(l->name, tmp->var->TinyJS_Var_deepCopy());
				} else {
					//pass by reference
					funcRoot->TinyJS_Var_addChild(l->name, tmp->var);
				}
			}
			if(list->next) { this->lex->TinyJS_Lex_match(','); }
			list = list->next;
		}
		this->lex->TinyJS_Lex_match(')');
		//Execute function!
		//Add the function's execute space to the symbol table so we can recurse.
		ST_TinyJS_VarLink* returnVarLink = funcRoot->TinyJS_Var_addChild("return", TinyJS_Var_newUndefined());
		ST_TinyJS_Context* pSavedContext = this->TinyJS_saveContext();
		{
			//We just want to execute the block, but something could have messed up and left us with the wrong ScriptLex, so we want to be careful here...
			const char* funcBody = func->var->TinyJS_Var_getString();
			//{{InitContext
			this->lex       = TinyJS_Lex_new(funcBody, 0, strlen(funcBody));
			this->scopes    = g_slist_prepend(this->scopes, funcRoot);
			this->callStack = g_slist_prepend(this->callStack, strdup_printf("%s from %s", func->name, this->lex->TinyJS_Lex_getLastPosition()));
			//}}InitContext
			SEH_try {
				if(func->var->TinyJS_Var_isNative()) {
					(*func->var->callback)(this, funcRoot, func->var->userData);
				} else {
					this->TinyJS_block(pExec);
					//Because return will probably have called this, and set execute to false.
					*pExec = 1;
				}
			} SEH_catch(TinyJS_Exception) {
				this->TinyJS_restoreContext(pSavedContext);
				throw;
			} SEH_end
		}
		this->TinyJS_restoreContext(pSavedContext);
	//不要	funcRoot->TinyJS_Var_removeLink(returnVarLink);	⇒funcRootは当関数内を抜けたら無効なので、クリーンアップする必要も無い。
		return returnVarLink;
	} else {
		//Function, but not executing - just parse args and be done.
		this->lex->TinyJS_Lex_match('(');
		while(this->lex->tk != ')') {
			ST_TinyJS_VarLink* l = this->TinyJS_base(pExec);
			if(this->lex->tk != ')') { this->lex->TinyJS_Lex_match(','); }
		}
		this->lex->TinyJS_Lex_match(')');
		if(this->lex->tk == '{') {	//TODO: Why is this here?
			this->TinyJS_block(pExec);
		}
		return TinyJS_VarLink_new(TinyJS_Var_newUndefined());
	}
}
//-----------------------------------------------------------------------------
//因子
ST_TinyJS_VarLink* TinyJS_factor(ST_TinyJS* _this, int* pExec) {
	//(～)
	if(this->lex->tk == '(') {
		this->lex->TinyJS_Lex_match('(');
		ST_TinyJS_VarLink* a = this->TinyJS_base(pExec);
		this->lex->TinyJS_Lex_match(')');
		return a;
	//true
	} else if(this->lex->tk == TINYJS_LEX_R_TRUE) {
		this->lex->TinyJS_Lex_match(TINYJS_LEX_R_TRUE);
		return TinyJS_VarLink_new(TinyJS_Var_newNumber(1));
	//false
	} else if(this->lex->tk == TINYJS_LEX_R_FALSE) {
		this->lex->TinyJS_Lex_match(TINYJS_LEX_R_FALSE);
		return TinyJS_VarLink_new(TinyJS_Var_newNumber(0));
	//null
	} else if(this->lex->tk == TINYJS_LEX_R_NULL) {
		this->lex->TinyJS_Lex_match(TINYJS_LEX_R_NULL);
		return TinyJS_VarLink_new(TinyJS_Var_newNull());
	//undefined
	} else if(this->lex->tk == TINYJS_LEX_R_UNDEFINED) {
		this->lex->TinyJS_Lex_match(TINYJS_LEX_R_UNDEFINED);
		return TinyJS_VarLink_new(TinyJS_Var_newUndefined());
	//ID
	} else if(this->lex->tk == TINYJS_LEX_ID) {
		ST_TinyJS_VarLink* a = NULL;
		if(*pExec) {
			a = this->TinyJS_findInScopes(this->lex->tkStr);
		}
		if(!a) {
			//Variable doesn't exist! JavaScript says we should create it.
			//(We won't add it here. This is done in the assignment operator.)
			a = TinyJS_VarLink_new(TinyJS_Var_newUndefined());
			a->name = this->lex->tkStr;
		}
		this->lex->TinyJS_Lex_match(TINYJS_LEX_ID);
		//ID(～),又は,ID.ID,又は,ID[～]を、IDに還元して繰り返す。
		ST_TinyJS_Var* obj = NULL;	//The 'this' if we're executing a method call.
		while((this->lex->tk == '(') ||
		      (this->lex->tk == '.') ||
		      (this->lex->tk == '[')) {
			switch(this->lex->tk) {
			default:DIE();	//バグ
			//Function Call.
			case '(':
				{
					a = this->TinyJS_functionCall(pExec, a, obj);
				}
				break;
			//Record Access.
			case '.':
				{
					this->lex->TinyJS_Lex_match('.');
					if(*pExec) {
						const char* name = this->lex->tkStr;
						ST_TinyJS_VarLink* l = a->var->TinyJS_Var_findChild(name);
						if(!l) { l = this->TinyJS_findInPrototypeClasses(a->var, name); }
						if(!l) {
							//If we haven't found this defined yet, use the built-in 'length' properly.
							if(a->var->TinyJS_Var_isArray() && !strcmp(name, "length")) {
								int len = a->var->TinyJS_Var_getArrayLength();
								l = TinyJS_VarLink_new(TinyJS_Var_newNumber(len));
							} else if(a->var->TinyJS_Var_isString() && !strcmp(name, "length")) {
								int len = strlen(a->var->TinyJS_Var_getString());
								l = TinyJS_VarLink_new(TinyJS_Var_newNumber(len));
							} else {
								l = a->var->TinyJS_Var_addChild(name, TinyJS_Var_newUndefined());
							}
						}
						obj = a->var;
						a = l;
					}
					this->lex->TinyJS_Lex_match(TINYJS_LEX_ID);
				}
				break;
			//Array Access.
			case '[':
				{
					this->lex->TinyJS_Lex_match('[');
					ST_TinyJS_VarLink* index = this->TinyJS_base(pExec);
					this->lex->TinyJS_Lex_match(']');
					if(*pExec) {
						ST_TinyJS_VarLink* l = a->var->TinyJS_Var_findChildOrCreate(index->var->TinyJS_Var_getString());
						obj = a->var;
						a = l;
					}
				}
				break;
			}
		}
		return a;
	//数値リテラル
	} else if(this->lex->tk == TINYJS_LEX_L_NUM) {
		//strtod()は10進実数と16進整数を解釈するが、8進整数を解釈しないので、8進整数のみ明示的に変換する事にした。
		double value;
		char* endptr;
		if((this->lex->tkStr[0] == '0') && (strspn(this->lex->tkStr, "01234567") == strlen(this->lex->tkStr))) {
			value = strtoul(this->lex->tkStr, &endptr, 8);
		} else {
			value = strtod(this->lex->tkStr, &endptr);
		}
		if(*endptr) { DIE(); }	//バグ
		ST_TinyJS_Var* v = TinyJS_Var_newNumber(value);
		this->lex->TinyJS_Lex_match(TINYJS_LEX_L_NUM);
		return TinyJS_VarLink_new(v);
	//文字列リテラル
	} else if(this->lex->tk == TINYJS_LEX_L_STR) {
		ST_TinyJS_Var* v = TinyJS_Var_newString(this->lex->tkStr);
		this->lex->TinyJS_Lex_match(TINYJS_LEX_L_STR);
		return TinyJS_VarLink_new(v);
	//「{id:value,...}」
	} else if(this->lex->tk == '{') {
		this->lex->TinyJS_Lex_match('{');
		ST_TinyJS_Var* contents = TinyJS_Var_newObject();
		while(this->lex->tk != '}') {
			const char* id = this->lex->tkStr;
			//We only allow strings or IDs on the left hand side of an initialisation.
			this->lex->TinyJS_Lex_match((this->lex->tk == TINYJS_LEX_ID) ? TINYJS_LEX_ID : TINYJS_LEX_L_STR);
			this->lex->TinyJS_Lex_match(':');
			ST_TinyJS_VarLink* l = this->TinyJS_base(pExec);
			contents->TinyJS_Var_addChild(id, l->var);
			if(this->lex->tk != '}') { this->lex->TinyJS_Lex_match(','); }
		}
		this->lex->TinyJS_Lex_match('}');
		return TinyJS_VarLink_new(contents);
	//「[value,...]」
	} else if(this->lex->tk == '[') {
		this->lex->TinyJS_Lex_match('[');
		ST_TinyJS_Var* contents = TinyJS_Var_newArray();
		int i = 0;
		while(this->lex->tk != ']') {
			ST_TinyJS_VarLink* l = this->TinyJS_base(pExec);
			contents->TinyJS_Var_addChild(strdup_printf("%d", i), l->var);
			if(this->lex->tk != ']') { this->lex->TinyJS_Lex_match(','); }
			i++;
		}
		this->lex->TinyJS_Lex_match(']');
		return TinyJS_VarLink_new(contents);
	//「function(仮引数名,...){～}」
	} else if(this->lex->tk == TINYJS_LEX_R_FUNCTION) {
		ST_TinyJS_VarLink* funcDef = this->TinyJS_parseFunctionDefinition();
		if(strcmp(funcDef->name, "")) {
			SEH_throw_msg(TinyJS_Exception, "Functions not defined at statement-level are not meant to have a name.");
		}
		return funcDef;
	//「new 関数名(～)」,又は,「new プロトタイプ名」,又は,「new プロトタイプ名()」
	} else if(this->lex->tk == TINYJS_LEX_R_NEW) {
		this->lex->TinyJS_Lex_match(TINYJS_LEX_R_NEW);
		const char* className = this->lex->tkStr;
		this->lex->TinyJS_Lex_match(TINYJS_LEX_ID);
		ST_TinyJS_Var* obj = TinyJS_Var_newObject();
		if(*pExec) {
			ST_TinyJS_VarLink* objClassOrFunc = this->TinyJS_findInScopes(className);
			if(!objClassOrFunc) {
				SEH_throw_msg(TinyJS_Exception, strdup_printf("%s is not a valid class name.", className));
			}
			if(objClassOrFunc->var->TinyJS_Var_isFunction()) {
				this->TinyJS_functionCall(pExec, objClassOrFunc, obj);
			} else {
				obj->TinyJS_Var_addChild("prototype", objClassOrFunc->var);
				if(this->lex->tk == '(') {
					this->lex->TinyJS_Lex_match('(');
					this->lex->TinyJS_Lex_match(')');
				}
			}
		} else {
			if(this->lex->tk == '(') {
				this->lex->TinyJS_Lex_match('(');
				this->lex->TinyJS_Lex_match(')');
			}
		}
		return TinyJS_VarLink_new(obj);
	} else {
		SEH_throw_msg(TinyJS_Exception, "Syntax error.");
	}
}
//-----------------------------------------------------------------------------
//単項演算子
ST_TinyJS_VarLink* TinyJS_unary(ST_TinyJS* _this, int* pExec) {
	ST_TinyJS_VarLink* a;
	if(this->lex->tk == '-') {
		this->lex->TinyJS_Lex_match('-');
		a = this->TinyJS_unary(pExec);
		if(*pExec) {
			ST_TinyJS_Var* tmp = TinyJS_Var_newNumber(0);
			ST_TinyJS_Var* res = tmp->TinyJS_Var_mathsOp(a->var, '-');
			a = TinyJS_VarLink_new(res);
		}
	} else if(this->lex->tk == '!') {
		this->lex->TinyJS_Lex_match('!');
		a = this->TinyJS_unary(pExec);
		if(*pExec) {
			ST_TinyJS_Var* tmp = TinyJS_Var_newNumber(0);
			ST_TinyJS_Var* res = a->var->TinyJS_Var_mathsOp(tmp, TINYJS_LEX_O_EQUAL);
			a = TinyJS_VarLink_new(res);
		}
	} else {
		a = this->TinyJS_factor(pExec);
	}
	return a;
}
//-----------------------------------------------------------------------------
//項
ST_TinyJS_VarLink* TinyJS_term(ST_TinyJS* _this, int* pExec) {
	ST_TinyJS_VarLink* a = this->TinyJS_unary(pExec);
	while((this->lex->tk == '*') ||
	      (this->lex->tk == '/') ||
	      (this->lex->tk == '%')) {
		int op = this->lex->tk;
		this->lex->TinyJS_Lex_match(op);
		ST_TinyJS_VarLink* b = this->TinyJS_unary(pExec);
		if(*pExec) {
			a = TinyJS_VarLink_new(a->var->TinyJS_Var_mathsOp(b->var, op));
		}
	}
	return a;
}
//-----------------------------------------------------------------------------
//式
ST_TinyJS_VarLink* TinyJS_expression(ST_TinyJS* _this, int* pExec) {
	ST_TinyJS_VarLink* a = this->TinyJS_term(pExec);
	while((this->lex->tk == '+') ||
	      (this->lex->tk == '-') ||
	      (this->lex->tk == TINYJS_LEX_O_PLUSPLUS) ||	//"++"
	      (this->lex->tk == TINYJS_LEX_O_MINUSMINUS)) {	//"--"
		int op = this->lex->tk;
		this->lex->TinyJS_Lex_match(op);
		if((op == TINYJS_LEX_O_PLUSPLUS) ||	//"++"
		   (op == TINYJS_LEX_O_MINUSMINUS)) {	//"--"
			if(*pExec) {
				ST_TinyJS_VarLink* oldVarLink = TinyJS_VarLink_new(a->var);
				ST_TinyJS_Var* tmp = TinyJS_Var_newNumber(1);
				a->TinyJS_VarLink_replaceWith(a->var->TinyJS_Var_mathsOp(tmp, (op == TINYJS_LEX_O_PLUSPLUS) ? '+' : '-'));	//In-place add/subtract.
				a = oldVarLink;
			}
		} else {
			ST_TinyJS_VarLink* b = this->TinyJS_term(pExec);
			if(*pExec) {
				a = TinyJS_VarLink_new(a->var->TinyJS_Var_mathsOp(b->var, op));			//Not in-place, so just replace.
			}
		}
	}
	return a;
}
//-----------------------------------------------------------------------------
//シフト式
ST_TinyJS_VarLink* TinyJS_shift(ST_TinyJS* _this, int* pExec) {
	ST_TinyJS_VarLink* a = this->TinyJS_expression(pExec);
	if((this->lex->tk == TINYJS_LEX_O_LSHIFT) ||
	   (this->lex->tk == TINYJS_LEX_O_RSHIFT) ||
	   (this->lex->tk == TINYJS_LEX_O_RSHIFTUNSIGNED)) {
		int op = this->lex->tk;
		this->lex->TinyJS_Lex_match(op);
		ST_TinyJS_VarLink* b = this->TinyJS_base(pExec);
		if(*pExec) {
			int value = a->var->TinyJS_Var_getNumber();
			int shift = b->var->TinyJS_Var_getNumber();
			switch(op) {
			default:DIE();	//バグ
			case TINYJS_LEX_O_LSHIFT:         value =           value << shift; break;
			case TINYJS_LEX_O_RSHIFT:         value =           value >> shift; break;
			case TINYJS_LEX_O_RSHIFTUNSIGNED: value = (unsigned)value >> shift; break;
			}
			a = TinyJS_VarLink_new(TinyJS_Var_newNumber(value));
		}
	}
	return a;
}
//-----------------------------------------------------------------------------
//条件式
ST_TinyJS_VarLink* TinyJS_condition(ST_TinyJS* _this, int* pExec) {
	ST_TinyJS_VarLink* a = this->TinyJS_shift(pExec);
	while((this->lex->tk == '<') ||
	      (this->lex->tk == '>') ||
	      (this->lex->tk == TINYJS_LEX_O_LEQUAL) ||		//"<="
	      (this->lex->tk == TINYJS_LEX_O_GEQUAL) ||		//">="
	      (this->lex->tk == TINYJS_LEX_O_EQUAL) ||		//"=="
	      (this->lex->tk == TINYJS_LEX_O_NEQUAL) ||		//"!="
	      (this->lex->tk == TINYJS_LEX_O_TYPEEQUAL) ||	//"==="
	      (this->lex->tk == TINYJS_LEX_O_NTYPEEQUAL)) {	//"!=="
		int op = this->lex->tk;
		this->lex->TinyJS_Lex_match(op);
		ST_TinyJS_VarLink* b = this->TinyJS_shift(pExec);
		if(*pExec) {
			a = TinyJS_VarLink_new(a->var->TinyJS_Var_mathsOp(b->var, op));
		}
	}
	return a;
}
//-----------------------------------------------------------------------------
//論理式
ST_TinyJS_VarLink* TinyJS_logic(ST_TinyJS* _this, int* pExec) {
	ST_TinyJS_VarLink* a = this->TinyJS_condition(pExec);
	while((this->lex->tk == '&') ||
	      (this->lex->tk == '|') ||
	      (this->lex->tk == '^') ||
	      (this->lex->tk == TINYJS_LEX_O_ANDAND) ||	//"&&"
	      (this->lex->tk == TINYJS_LEX_O_OROR)) {	//"||"
		int op = this->lex->tk;
		this->lex->TinyJS_Lex_match(op);
		//If we have short-circuit ops, then if we know the outcome we don't bother to execute the other op.
		//Even if not we need to tell TinyJS_Var_mathsOp it's an & or |.
		int shortCircuit, isBoolean;
		if(op == TINYJS_LEX_O_ANDAND) {		//"&&"
			op = '&';
			shortCircuit = !a->var->TinyJS_Var_getBoolean();
			isBoolean    = 1;
		} else if(op == TINYJS_LEX_O_OROR) {	//"||"
			op = '|';
			shortCircuit =  a->var->TinyJS_Var_getBoolean();
			isBoolean    = 1;
		} else {
			shortCircuit = 0;
			isBoolean    = 0;
		}
		ST_TinyJS_VarLink* b = this->TinyJS_condition(shortCircuit ? &noexec : pExec);
		if(*pExec) {
			if(!shortCircuit) {
				if(isBoolean) {
					a = TinyJS_VarLink_new(TinyJS_Var_newNumber(a->var->TinyJS_Var_getBoolean()));
					b = TinyJS_VarLink_new(TinyJS_Var_newNumber(b->var->TinyJS_Var_getBoolean()));
				}
				a = TinyJS_VarLink_new(a->var->TinyJS_Var_mathsOp(b->var, op));
			}
		}
	}
	return a;
}
//-----------------------------------------------------------------------------
//三項演算子
ST_TinyJS_VarLink* TinyJS_ternary(ST_TinyJS* _this, int* pExec) {
	ST_TinyJS_VarLink* a = this->TinyJS_logic(pExec);
	if(this->lex->tk == '?') {
		this->lex->TinyJS_Lex_match('?');
		if(a->var->TinyJS_Var_getBoolean()) {
			a = this->TinyJS_base(ST_TinyJS* _this, pExec);
			this->lex->TinyJS_Lex_match(':');
			this->TinyJS_base(&noexec);
		} else {
			this->TinyJS_base(&noexec);
			this->lex->TinyJS_Lex_match(':');
			a = this->TinyJS_base(pExec);
		}
	}
	return a;
}
//-----------------------------------------------------------------------------
//代入式
ST_TinyJS_VarLink* TinyJS_base(int* pExec) {
	ST_TinyJS_VarLink* lhs = this->TinyJS_ternary(pExec);
	if((this->lex->tk == '=') ||
	   (this->lex->tk == TINYJS_LEX_O_PLUSASSIGN) ||
	   (this->lex->tk == TINYJS_LEX_O_MINUSASSIGN)) {
		//If we're assigning to this and we don't have a prototype, add it to the symbol table root as per JavaScript.
		if(*pExec) {
			if(!lhs->owned) {
				if(!strcmp(lhs->name, "")) {
					SEH_throw_msg(TinyJS_Exception, "Trying to assign to an un-named type.");
				}
				lhs = this->root->TinyJS_Var_addChild(lhs->name, lhs->var);
			}
		}
		int op = this->lex->tk;
		this->lex->TinyJS_Lex_match(op);
		ST_TinyJS_VarLink* rhs = this->TinyJS_base(pExec);
		if(*pExec) {
			switch(op) {
			default:DIE();	//バグ
			case '=':                      lhs->TinyJS_VarLink_replaceWith(                  rhs->var      ); break;
			case TINYJS_LEX_O_PLUSASSIGN:  lhs->TinyJS_VarLink_replaceWith(lhs->var->TinyJS_Var_mathsOp(rhs->var, '+')); break;
			case TINYJS_LEX_O_MINUSASSIGN: lhs->TinyJS_VarLink_replaceWith(lhs->var->TinyJS_Var_mathsOp(rhs->var, '-')); break;
			}
		}
	}
	return lhs;
}
//-----------------------------------------------------------------------------
//「{～}」
void TinyJS_block(ST_TinyJS* _this, int* pExec) {
	this->lex->TinyJS_Lex_match('{');
	while(this->lex->tk && (this->lex->tk != '}')) {
		this->TinyJS_statement(pExec);
	}
	this->lex->TinyJS_Lex_match('}');
}
//-----------------------------------------------------------------------------
//文
void TinyJS_statement(ST_TinyJS* _this, int* pExec) {
	ST_TinyJS_Var* scope = (ST_TinyJS_Var*)this->scopes->data;
	if((this->lex->tk == '-') ||
	   (this->lex->tk == TINYJS_LEX_ID) ||
	   (this->lex->tk == TINYJS_LEX_L_NUM) ||
	   (this->lex->tk == TINYJS_LEX_L_STR)) {
		//Execute a simple statement that only contains basic arithmetic...
		this->TinyJS_base(pExec);
		this->lex->TinyJS_Lex_match(';');
	} else if(this->lex->tk == '{') {
		//A block of code.
		this->TinyJS_block(pExec);
	} else if(this->lex->tk == ';') {
		//Empty statement - to allow things like ;;;.
		this->lex->TinyJS_Lex_match(';');
	} else if(this->lex->tk == TINYJS_LEX_R_VAR) {
		//Variable creation.
		//TODO - We need a better way of parsing the left hand side.
		//       Maybe just have a flag called can_create_var that we set and then we parse as if we're doing a normal equals.
		this->lex->TinyJS_Lex_match(TINYJS_LEX_R_VAR);
		do {
			ST_TinyJS_VarLink* l = NULL;
			if(*pExec) {
				l = scope->TinyJS_Var_findChildOrCreate(this->lex->tkStr);
			}
			this->lex->TinyJS_Lex_match(TINYJS_LEX_ID);
			//Now do stuff defined with dots.
			while(this->lex->tk == '.') {
				this->lex->TinyJS_Lex_match('.');
				if(*pExec) {
					l = l->var->TinyJS_Var_findChildOrCreate(this->lex->tkStr);
				}
				this->lex->TinyJS_Lex_match(TINYJS_LEX_ID);
			}
			//Sort out initialiser.
			if(this->lex->tk == '=') {
				this->lex->TinyJS_Lex_match('=');
				ST_TinyJS_VarLink* tmp = this->TinyJS_base(pExec);
				if(*pExec) {
					l->TinyJS_VarLink_replaceWith(tmp->var);
				}
			}
			if(this->lex->tk != ';') { this->lex->TinyJS_Lex_match(','); }
		} while(this->lex->tk != ';');
		this->lex->TinyJS_Lex_match(';');
	} else if(this->lex->tk == TINYJS_LEX_R_IF) {
		this->lex->TinyJS_Lex_match(TINYJS_LEX_R_IF);
		this->lex->TinyJS_Lex_match('(');
		ST_TinyJS_VarLink* cond = this->TinyJS_base(pExec);					//Condition.
		this->lex->TinyJS_Lex_match(')');
		this->TinyJS_statement(cond->var->TinyJS_Var_getBoolean() ? pExec : &noexec);		//If body.
		if(this->lex->tk == TINYJS_LEX_R_ELSE) {
			this->lex->TinyJS_Lex_match(TINYJS_LEX_R_ELSE);
			this->TinyJS_statement(cond->var->TinyJS_Var_getBoolean() ? &noexec : pExec);	//Else body.
		}
	} else if(this->lex->tk == TINYJS_LEX_R_WHILE) {
		this->lex->TinyJS_Lex_match(TINYJS_LEX_R_WHILE);
		this->lex->TinyJS_Lex_match('(');
		int whileCondStart = this->lex->tokenStart;
		this->TinyJS_base(&noexec);		//Condition.
		ST_TinyJS_Lex* whileCond = this->lex->TinyJS_Lex_getSubLex(whileCondStart);
		this->lex->TinyJS_Lex_match(')');
		int whileBodyStart = this->lex->tokenStart;
		this->TinyJS_statement(&noexec);	//Body.
		ST_TinyJS_Lex* whileBody = this->lex->TinyJS_Lex_getSubLex(whileBodyStart);
		ST_TinyJS_Context* pSavedContext = this->TinyJS_saveContext();
		if(*pExec) {
			for(;;) {
				this->lex = whileCond->TinyJS_Lex_reset();
				ST_TinyJS_VarLink* cond = this->TinyJS_base(pExec);	//Condition.
				if(!cond->var->TinyJS_Var_getBoolean()) { break; }
				this->lex = whileBody->TinyJS_Lex_reset();		//Body.
				this->TinyJS_statement(pExec);
			}
		}
		this->TinyJS_restoreContext(pSavedContext);
	} else if(this->lex->tk == TINYJS_LEX_R_FOR) {
		this->lex->TinyJS_Lex_match(TINYJS_LEX_R_FOR);
		this->lex->TinyJS_Lex_match('(');
		this->TinyJS_statement(pExec);		//Initialisation.
		int forCondStart = this->lex->tokenStart;
		this->TinyJS_base(&noexec);		//Condition.
		ST_TinyJS_Lex* forCond = this->lex->TinyJS_Lex_getSubLex(forCondStart);
		this->lex->TinyJS_Lex_match(';');
		int forIterStart = this->lex->tokenStart;
		this->TinyJS_base(&noexec);		//Iterator.
		ST_TinyJS_Lex* forIter = this->lex->TinyJS_Lex_getSubLex(forIterStart);
		this->lex->TinyJS_Lex_match(')');
		int forBodyStart = this->lex->tokenStart;
		this->TinyJS_statement(&noexec);	//Body.
		ST_TinyJS_Lex* forBody = this->lex->TinyJS_Lex_getSubLex(forBodyStart);
		ST_TinyJS_Context* pSavedContext = this->TinyJS_saveContext();
		if(*pExec) {
			for(;;) {
				this->lex = forCond->TinyJS_Lex_reset();
				ST_TinyJS_VarLink* cond = this->TinyJS_base(pExec);	//Condition.
				if(!cond->var->TinyJS_Var_getBoolean()) { break; }
				this->lex = forBody->TinyJS_Lex_reset();
				this->TinyJS_statement(pExec);				//Body.
				this->lex = forIter->TinyJS_Lex_reset();
				this->TinyJS_base(pExec);				//Iterator.
			}
		}
		this->TinyJS_restoreContext(pSavedContext);
	} else if(this->lex->tk == TINYJS_LEX_R_RETURN) {
		this->lex->TinyJS_Lex_match(TINYJS_LEX_R_RETURN);
		ST_TinyJS_VarLink* result = NULL;
		if(this->lex->tk != ';') {
			result = this->TinyJS_base(pExec);
		}
		if(*pExec) {
			if(result) {
				scope->TinyJS_Var_setReturnVar(result->var);
			}
			*pExec = 0;	//return文以降は実行せずにパースだけを行う。
		}
		this->lex->TinyJS_Lex_match(';');
	} else if(this->lex->tk == TINYJS_LEX_R_FUNCTION) {
		ST_TinyJS_VarLink* funcDef = this->TinyJS_parseFunctionDefinition();
		if(*pExec) {
			if(!strcmp(funcDef->name, "")) {
				SEH_throw_msg(TinyJS_Exception, "Functions defined at statement-level are meant to have a name.");
			}
			scope->TinyJS_Var_addChild(funcDef->name, funcDef->var);
		}
	} else {
		this->lex->TinyJS_Lex_match(TINYJS_LEX_EOF);
	}
}
//-----------------------------------------------------------------------------
//「function 関数名(仮引数名,...){～}」,又は,「function(仮引数名,...){～}」
ST_TinyJS_VarLink* TinyJS_parseFunctionDefinition(ST_TinyJS* _this) {
	this->lex->TinyJS_Lex_match(TINYJS_LEX_R_FUNCTION);
	const char* funcName = "";
	if(this->lex->tk == TINYJS_LEX_ID) {	//We can have functions without names.
		funcName = this->lex->tkStr;
		this->lex->TinyJS_Lex_match(TINYJS_LEX_ID);
	}
	ST_TinyJS_Var* funcVar = TinyJS_Var_newFunction();
	this->TinyJS_parseFunctionArguments(funcVar);
	int funcBegin = this->lex->tokenStart;
	this->TinyJS_block(&noexec);
	funcVar->strData = this->lex->TinyJS_Lex_getSubString(funcBegin);
	ST_TinyJS_VarLink* l = TinyJS_VarLink_new(funcVar);
	l->name = funcName;
	return l;
}
//-----------------------------------------------------------------------------
//「(仮引数名,...)」
void TinyJS_parseFunctionArguments(ST_TinyJS* _this, ST_TinyJS_Var* funcVar) {
	this->lex->TinyJS_Lex_match('(');
	while(this->lex->tk != ')') {
		funcVar->TinyJS_Var_addChild(this->lex->tkStr, TinyJS_Var_newUndefined());
		this->lex->TinyJS_Lex_match(TINYJS_LEX_ID);
		if(this->lex->tk != ')') { this->lex->TinyJS_Lex_match(','); }
	}
	this->lex->TinyJS_Lex_match(')');
}
//-----------------------------------------------------------------------------
//Finds a child, looking recursively up the scopes.
ST_TinyJS_VarLink* TinyJS_findInScopes(ST_TinyJS* _this, const char* name) {
	GSList/*<ST_TinyJS_Var*>*/* list = this->scopes;
	while(list) {
		ST_TinyJS_Var* scope = (ST_TinyJS_Var*)list->data;
		ST_TinyJS_VarLink* l = scope->TinyJS_Var_findChild(name);
		if(l) { return l; }
		list = list->next;
	}
	return NULL;
}
//-----------------------------------------------------------------------------
//Look up in any prototype classes of the given object.
ST_TinyJS_VarLink* TinyJS_findInPrototypeClasses(ST_TinyJS* _this, ST_TinyJS_Var* v, const char* name) {
	//Look for links to actual prototype classes.
	ST_TinyJS_VarLink* prototypeClass = v->TinyJS_Var_findChild("prototype");
	while(prototypeClass) {
		ST_TinyJS_VarLink* implementation = prototypeClass->var->TinyJS_Var_findChild(name);
		if(implementation) { return implementation; }
		prototypeClass = prototypeClass->var->TinyJS_Var_findChild("prototype");
	}
	//else fake it for strings and finally objects.
	if(v->TinyJS_Var_isString()) {
		ST_TinyJS_VarLink* implementation = this->stringClass->TinyJS_Var_findChild(name);
		if(implementation) { return implementation; }
	}
	if(v->TinyJS_Var_isArray()) {
		ST_TinyJS_VarLink* implementation = this->arrayClass->TinyJS_Var_findChild(name);
		if(implementation) { return implementation; }
	}
	return this->objectClass->TinyJS_Var_findChild(name);	//may be NULL
}
//-----------------------------------------------------------------------------
ST_TinyJS_Context* TinyJS_saveContext(ST_TinyJS* _this) {
	ST_TinyJS_Context* pSavedContext = (ST_TinyJS_Context*)malloc(sizeof(ST_TinyJS_Context));
	pSavedContext->lex       = this->lex;
	pSavedContext->scopes    = this->scopes;
	pSavedContext->callStack = this->callStack;
	return pSavedContext;
}
//-----------------------------------------------------------------------------
void TinyJS_restoreContext(ST_TinyJS* _this, ST_TinyJS_Context* pSavedContext) {
	this->lex       = pSavedContext->lex;
	this->scopes    = pSavedContext->scopes;
	this->callStack = pSavedContext->callStack;
}
//-----------------------------------------------------------------------------
const char* TinyJS_stackTrace(ST_TinyJS* _this, const char* errMsg) {
	GString* buf = g_string_new(NULL);
	g_string_append(buf, "Error ");
	g_string_append(buf, errMsg);
	{
		GSList/*<const char*>*/* list = this->callStack;
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
	g_string_append(buf, this->lex->TinyJS_Lex_getLastPosition());
	return buf->str;
}
//*****************************************************************************
//	ST_TinyJS_Lex
//*****************************************************************************
/*static*/ ST_TinyJS_Lex* TinyJS_Lex_new(const char* input, int startChar, int endChar) {
	ST_TinyJS_Lex* _this = new ST_TinyJS_Lex();
	_this->data      = input;
	_this->dataStart = startChar;
	_this->dataEnd   = endChar;
	_this->TinyJS_Lex_reset();
	return _this;
}
//-----------------------------------------------------------------------------
//Reset this lex so we can start again.
ST_TinyJS_Lex* TinyJS_Lex_reset(ST_TinyJS_Lex* _this) {
	this->dataPos      = this->dataStart;
	this->tokenStart   = 0;
	this->tokenEnd     = 0;
	this->tokenLastEnd = 0;
	this->tk           = 0;
	this->tkStr        = "";
	this->TinyJS_Lex_getNextCh();
	this->TinyJS_Lex_getNextCh();
	//
	//	～■■□～
	//	　↑↑↑
	//	　││└         dataPos
	//	　│└─ nextCh: dataPos - 1
	//	　└── currCh: dataPos - 2 = dataStart
	//
	this->TinyJS_Lex_getNextToken();
	return this;	//利便性のためにthisを返す事にした。
}
//-----------------------------------------------------------------------------
//Lexical match wotsit.
void TinyJS_Lex_match(ST_TinyJS_Lex* _this, int tkExpected) {
	if(this->tk != tkExpected) {
		GString* errorString = g_string_new(NULL);
		g_string_append(errorString, "Got ");
		g_string_append(errorString, TinyJS_Lex_getTokenStr(this->tk));
		g_string_append(errorString, " expected ");
		g_string_append(errorString, TinyJS_Lex_getTokenStr(tkExpected));
		g_string_append(errorString, " at ");
		g_string_append(errorString, this->TinyJS_Lex_getPosition(this->tokenStart));
		SEH_throw_msg(TinyJS_Exception, errorString->str);
	}
	this->TinyJS_Lex_getNextToken();
}
//-----------------------------------------------------------------------------
//Get the string representation of the given token.
/*static*/ const char* TinyJS_Lex_getTokenStr(int token) {
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
//Return a sub-string from the given position up until right now.
const char* TinyJS_Lex_getSubString(ST_TinyJS_Lex* _this, int lastPosition) {
	if(      lastPosition > this->dataEnd) { DIE(); }	//バグ
	if(this->tokenLastEnd > this->dataEnd) { DIE(); }	//バグ
	return strndup(this->data + lastPosition, this->tokenLastEnd - lastPosition);
}
//-----------------------------------------------------------------------------
//Return a sub-lexer from the given position up until right now.
ST_TinyJS_Lex* TinyJS_Lex_getSubLex(ST_TinyJS_Lex* _this, int lastPosition) {
	if(      lastPosition > this->dataEnd) { DIE(); }	//バグ
	if(this->tokenLastEnd > this->dataEnd) { DIE(); }	//バグ
	return TinyJS_Lex_new(this->data, lastPosition, this->tokenLastEnd);
}
//-----------------------------------------------------------------------------
//Return a string representing the position in lines and columns of the tokenLastEnd - 1.
const char* TinyJS_Lex_getLastPosition(ST_TinyJS_Lex* _this) {
	return TinyJS_Lex_getPosition(this->tokenLastEnd - 1);
}
//-----------------------------------------------------------------------------
//Return a string representing the position in lines and columns of the character pos given.
const char* TinyJS_Lex_getPosition(ST_TinyJS_Lex* _this, int pos) {
	if(pos > this->dataEnd) { pos = this->dataEnd; }
	int line = 1, col = 1;
	for(int i = 0; i < pos; i++) {
		col++;
		if(this->data[i] == '\n') {
			line++;
			col = 1;
		}
	}
	return strdup_printf("(line: %d, col: %d)", line, col);
}
//-----------------------------------------------------------------------------
void TinyJS_Lex_getNextCh(ST_TinyJS_Lex* _this) {
	this->currCh = this->nextCh;
	if(this->dataPos < this->dataEnd) {
		this->nextCh = this->data[this->dataPos];
//×		this->dataPos++;	//←┐
	} else {			//　│
		this->nextCh = '\0';	//　├もし入力範囲の終端を超えてもdataPosは無条件に進めなければいけない。TinyJS_Lex_reset(),及び,TinyJS_Lex_getNextToken()の処理がgetNextCh()を呼び出した回数分dataPosが進んでいる事を前提としているから。
	}				//　│
/*○*/	this->dataPos++;		//←┘
}
//-----------------------------------------------------------------------------
//Get the text token from our text string.
void TinyJS_Lex_getNextToken(ST_TinyJS_Lex* _this) {
	for(;;) {
		//Skip whitespaces.
		while(this->currCh) {
			if(!isspace(this->currCh)) {
				break;
			}
			this->TinyJS_Lex_getNextCh();
		}
		//Newline comments.
		if((this->currCh == '/') && (this->nextCh == '/')) {
			this->TinyJS_Lex_getNextCh();
			this->TinyJS_Lex_getNextCh();
			while(this->currCh) {
				if(this->currCh == '\n') {
					this->TinyJS_Lex_getNextCh();
					break;
				}
				this->TinyJS_Lex_getNextCh();
			}
		//Block comments.
		} else if((this->currCh == '/') && (this->nextCh == '*')) {
			this->TinyJS_Lex_getNextCh();
			this->TinyJS_Lex_getNextCh();
			while(this->currCh) {
				if((this->currCh == '*') && (this->nextCh == '/')) {
					this->TinyJS_Lex_getNextCh();
					this->TinyJS_Lex_getNextCh();
					break;
				}
				this->TinyJS_Lex_getNextCh();
			}
		} else {
			break;
		}
	}
	//Record beginning of this token.
	this->tokenStart = this->dataPos - 2;
	//
	//	～■■□～
	//	　↑↑↑
	//	　││└         dataPos
	//	　│└─ nextCh: dataPos - 1
	//	　└── currCh: dataPos - 2 = tokenStart
	//
	//Tokens.
//不要	this->tk    = TINYJS_LEX_EOF;
	this->tkStr = "";
	GString* buf = g_string_new(NULL);
	//予約語,又は,ID
	if(iscsymf(this->currCh)) {
		while(iscsym(this->currCh)) {
			g_string_append_c(buf, this->currCh);
			this->TinyJS_Lex_getNextCh();
		}
		     if(!strcmp(buf->str, "break"))     { this->tk = TINYJS_LEX_R_BREAK; }
		else if(!strcmp(buf->str, "continue"))  { this->tk = TINYJS_LEX_R_CONTINUE; }
		else if(!strcmp(buf->str, "do"))        { this->tk = TINYJS_LEX_R_DO; }
		else if(!strcmp(buf->str, "else"))      { this->tk = TINYJS_LEX_R_ELSE; }
		else if(!strcmp(buf->str, "false"))     { this->tk = TINYJS_LEX_R_FALSE; }
		else if(!strcmp(buf->str, "for"))       { this->tk = TINYJS_LEX_R_FOR; }
		else if(!strcmp(buf->str, "function"))  { this->tk = TINYJS_LEX_R_FUNCTION; }
		else if(!strcmp(buf->str, "if"))        { this->tk = TINYJS_LEX_R_IF; }
		else if(!strcmp(buf->str, "new"))       { this->tk = TINYJS_LEX_R_NEW; }
		else if(!strcmp(buf->str, "null"))      { this->tk = TINYJS_LEX_R_NULL; }
		else if(!strcmp(buf->str, "return"))    { this->tk = TINYJS_LEX_R_RETURN; }
		else if(!strcmp(buf->str, "true"))      { this->tk = TINYJS_LEX_R_TRUE; }
		else if(!strcmp(buf->str, "undefined")) { this->tk = TINYJS_LEX_R_UNDEFINED; }
		else if(!strcmp(buf->str, "var"))       { this->tk = TINYJS_LEX_R_VAR; }
		else if(!strcmp(buf->str, "while"))     { this->tk = TINYJS_LEX_R_WHILE; }
		else                                    { this->tk = TINYJS_LEX_ID; }
	//数値リテラル
	} else if(isdigit(this->currCh)) {
		int isHex = 0;
		if(this->currCh == '0') {
			g_string_append_c(buf, this->currCh);
			this->TinyJS_Lex_getNextCh();
			if((this->currCh == 'x') || (this->currCh == 'X')) {
				isHex = 1;
				g_string_append_c(buf, this->currCh);
				this->TinyJS_Lex_getNextCh();
			}
		}
		this->tk = TINYJS_LEX_L_NUM;
		while(isdigit(this->currCh) || (isHex && isxdigit(this->currCh))) {
			g_string_append_c(buf, this->currCh);
			this->TinyJS_Lex_getNextCh();
		}
		if(!isHex && (this->currCh == '.')) {
			g_string_append_c(buf, this->currCh);
			this->TinyJS_Lex_getNextCh();
			while(isdigit(this->currCh)) {
				g_string_append_c(buf, this->currCh);
				this->TinyJS_Lex_getNextCh();
			}
		}
		//Do fancy e-style floating point.
		if(!isHex && ((this->currCh == 'e') || (this->currCh == 'E'))) {
			g_string_append_c(buf, this->currCh);
			this->TinyJS_Lex_getNextCh();
			if(this->currCh == '-') {
				g_string_append_c(buf, this->currCh);
				this->TinyJS_Lex_getNextCh();
			}
			while(isdigit(this->currCh)) {
				g_string_append_c(buf, this->currCh);
				this->TinyJS_Lex_getNextCh();
			}
		}
	//文字列リテラル
	} else if((this->currCh == '"') || (this->currCh == '\'')) {
		const int quotCh = this->currCh;
		this->tk = TINYJS_LEX_L_STR;
		this->TinyJS_Lex_getNextCh();
		while(this->currCh && (this->currCh != quotCh)) {
			if(this->currCh == '\\') {
				this->TinyJS_Lex_getNextCh();
				switch(this->currCh) {
				case 't':  g_string_append_c(buf, '\t'); break;	//┐
				case 'n':  g_string_append_c(buf, '\n'); break;	//├頻繁に使う制御文字のエスケープシーケンスだけ対応した。頻繁に使わない制御文字はその他の印字不可文字と共通で8進数形式,又は,16進数形式での入力を前提とする。もし不足ならば追加しろ。
				case 'r':  g_string_append_c(buf, '\r'); break;	//┘
				case '\\': g_string_append_c(buf, '\\'); break;
				default:
					//Hex digits.
					if((this->currCh == 'x') || (this->currCh == 'X')) {
						GString* tmp = g_string_new(NULL);
						for(;;) {			//C言語の文字列リテラルの仕様では、"\x????～"には桁数制限が無い。JavaScriptでも同じなのかわからない。(※TODO:要確認)
							this->TinyJS_Lex_getNextCh();
							if(!isxdigit(this->currCh)) { break; }
							g_string_append_c(tmp, this->currCh);
						}
						g_string_append_c(buf, (char)strtoul(tmp->str, NULL, 16));
					//Octal digits.
					} else if((this->currCh >= '0') && (this->currCh <= '7')) {
						GString* tmp = g_string_new(NULL);
						while(tmp->len < 3) {		//C言語の文字列リテラルの仕様では、"\???"の桁数は1桁～3桁である。JavaScriptでも同じなのかわからない。(※TODO:要確認)
							if(!isxdigit(this->currCh)) { break; }
							g_string_append_c(tmp, this->currCh);
							this->TinyJS_Lex_getNextCh();
						}
						g_string_append_c(buf, (char)strtoul(tmp->str, NULL, 8));
					} else {
						g_string_append_c(buf, this->currCh);
					}
					break;
				}
			} else {
				g_string_append_c(buf, this->currCh);
			}
			this->TinyJS_Lex_getNextCh();
		}
		this->TinyJS_Lex_getNextCh();
	//演算子,又は,EOF
	} else {
		this->tk = this->currCh;
		if(this->tk) {
			this->TinyJS_Lex_getNextCh();
			if((this->tk == '=') && (this->currCh == '=')) {
				this->tk = TINYJS_LEX_O_EQUAL;				//"=="
				this->TinyJS_Lex_getNextCh();
				if(this->currCh == '=') {
					this->tk = TINYJS_LEX_O_TYPEEQUAL;		//"==="
					this->TinyJS_Lex_getNextCh();
				}
			} else if((this->tk == '!') && (this->currCh == '=')) {
				this->tk = TINYJS_LEX_O_NEQUAL;				//"!="
				this->TinyJS_Lex_getNextCh();
				if(this->currCh == '=') {
					this->tk = TINYJS_LEX_O_NTYPEEQUAL;		//"!=="
					this->TinyJS_Lex_getNextCh();
				}
			} else if((this->tk == '<') && (this->currCh == '=')) {
				this->tk = TINYJS_LEX_O_LEQUAL;				//"<="
				this->TinyJS_Lex_getNextCh();
			} else if((this->tk == '<') && (this->currCh == '<')) {
				this->tk = TINYJS_LEX_O_LSHIFT;				//"<<"
				this->TinyJS_Lex_getNextCh();
				if(this->currCh == '=') {
					this->tk = TINYJS_LEX_O_LSHIFTASSIGN;		//"<<="
					this->TinyJS_Lex_getNextCh();
				}
			} else if((this->tk == '>') && (this->currCh == '=')) {
				this->tk = TINYJS_LEX_O_GEQUAL;				//">="
				this->TinyJS_Lex_getNextCh();
			} else if((this->tk == '>') && (this->currCh == '>')) {
				this->tk = TINYJS_LEX_O_RSHIFT;				//">>"
				this->TinyJS_Lex_getNextCh();
				if(this->currCh == '=') {
					this->tk = TINYJS_LEX_O_RSHIFTASSIGN;		//">>="
					this->TinyJS_Lex_getNextCh();
				} else if(this->currCh == '>') {
					this->tk = TINYJS_LEX_O_RSHIFTUNSIGNED;		//">>>"
					this->TinyJS_Lex_getNextCh();
				}
			} else if((this->tk == '+') && (this->currCh == '=')) {
				this->tk = TINYJS_LEX_O_PLUSASSIGN;			//"+="
				this->TinyJS_Lex_getNextCh();
			} else if((this->tk == '-') && (this->currCh == '=')) {
				this->tk = TINYJS_LEX_O_MINUSASSIGN;			//"-="
				this->TinyJS_Lex_getNextCh();
			} else if((this->tk == '+') && (this->currCh == '+')) {
				this->tk = TINYJS_LEX_O_PLUSPLUS;			//"++"
				this->TinyJS_Lex_getNextCh();
			} else if((this->tk == '-') && (this->currCh == '-')) {
				this->tk = TINYJS_LEX_O_MINUSMINUS;			//"--"
				this->TinyJS_Lex_getNextCh();
			} else if((this->tk == '&') && (this->currCh == '=')) {
				this->tk = TINYJS_LEX_O_ANDASSIGN;			//"&="
				this->TinyJS_Lex_getNextCh();
			} else if((this->tk == '&') && (this->currCh == '&')) {
				this->tk = TINYJS_LEX_O_ANDAND;				//"&&"
				this->TinyJS_Lex_getNextCh();
			} else if((this->tk == '|') && (this->currCh == '=')) {
				this->tk = TINYJS_LEX_O_OREQUAL;			//"|="
				this->TinyJS_Lex_getNextCh();
			} else if((this->tk == '|') && (this->currCh == '|')) {
				this->tk = TINYJS_LEX_O_OROR;				//"||"
				this->TinyJS_Lex_getNextCh();
			} else if((this->tk == '^') && (this->currCh == '=')) {
				this->tk = TINYJS_LEX_O_XORASSIGN;			//"^="
				this->TinyJS_Lex_getNextCh();
			} else {
				/** no job **/						//その他の一文字演算子,又は,不正な文字
			}
		} else {
			/** no job **/							//EOF
		}
	}
	//Record ending of last token.
	this->tokenLastEnd = this->tokenEnd;
	//Record ending of this token.
	this->tokenEnd = this->dataPos - 2;
	//
	//	～■■■□□□～
	//	　…┬┘↑↑↑
	//	　　│　││└         dataPos
	//	　　│　│└─ nextCh: dataPos - 1
	//	　　│　└── currCh: dataPos - 2 = tokenEnd
	//	　　└──── tkStr
	//
	this->tkStr = buf->str;	//忘れないで!!
}
//*****************************************************************************
//	ST_TinyJS_Var
//*****************************************************************************
/*static*/ ST_TinyJS_Var* TinyJS_Var_newUndefined() {
	ST_TinyJS_Var* v = new ST_TinyJS_Var();
	v->TinyJS_Var_setUndefined();
	return v;
}
//-----------------------------------------------------------------------------
/*static*/ ST_TinyJS_Var* TinyJS_Var_newNull() {
	ST_TinyJS_Var* v = new ST_TinyJS_Var();
	v->TinyJS_Var_setNull();
	return v;
}
//-----------------------------------------------------------------------------
/*static*/ ST_TinyJS_Var* TinyJS_Var_newNumber(double val) {
	ST_TinyJS_Var* v = new ST_TinyJS_Var();
	v->TinyJS_Var_setNumber(val);
	return v;
}
//-----------------------------------------------------------------------------
/*static*/ ST_TinyJS_Var* TinyJS_Var_newString(const char* str) {
	ST_TinyJS_Var* v = new ST_TinyJS_Var();
	v->TinyJS_Var_setString(str);
	return v;
}
//-----------------------------------------------------------------------------
/*static*/ ST_TinyJS_Var* TinyJS_Var_newFunction() {
	ST_TinyJS_Var* v = new ST_TinyJS_Var();
	v->TinyJS_Var_setFunction();
	return v;
}
//-----------------------------------------------------------------------------
/*static*/ ST_TinyJS_Var* TinyJS_Var_newObject() {
	ST_TinyJS_Var* v = new ST_TinyJS_Var();
	v->TinyJS_Var_setObject();
	return v;
}
//-----------------------------------------------------------------------------
/*static*/ ST_TinyJS_Var* TinyJS_Var_newArray() {
	ST_TinyJS_Var* v = new ST_TinyJS_Var();
	v->TinyJS_Var_setArray();
	return v;
}
//-----------------------------------------------------------------------------
/*static*/ ST_TinyJS_Var* TinyJS_Var_newNative(TinyJS_Callback* callback, void* userData) {
	ST_TinyJS_Var* v = new ST_TinyJS_Var();
	v->TinyJS_Var_setNative(callback, userData);
	return v;
}
//-----------------------------------------------------------------------------
void TinyJS_Var_setUndefined(ST_TinyJS_Var* _this) {
	this->TinyJS_Var_init(TINYJS_VAR_UNDEFINED);
}
//-----------------------------------------------------------------------------
void TinyJS_Var_setNull(ST_TinyJS_Var* _this) {
	this->TinyJS_Var_init(TINYJS_VAR_NULL);
}
//-----------------------------------------------------------------------------
void TinyJS_Var_setNumber(ST_TinyJS_Var* _this, double val) {
	this->TinyJS_Var_init(TINYJS_VAR_NUMBER);
	this->numData = val;
}
//-----------------------------------------------------------------------------
void TinyJS_Var_setString(ST_TinyJS_Var* _this, const char* str) {
	this->TinyJS_Var_init(TINYJS_VAR_STRING);
	this->strData = str;
}
//-----------------------------------------------------------------------------
void TinyJS_Var_setFunction(ST_TinyJS_Var* _this) {
	this->TinyJS_Var_init(TINYJS_VAR_FUNCTION);
}
//-----------------------------------------------------------------------------
void TinyJS_Var_setObject(ST_TinyJS_Var* _this) {
	this->TinyJS_Var_init(TINYJS_VAR_OBJECT);
}
//-----------------------------------------------------------------------------
void TinyJS_Var_setArray(ST_TinyJS_Var* _this) {
	this->TinyJS_Var_init(TINYJS_VAR_ARRAY);
}
//-----------------------------------------------------------------------------
void TinyJS_Var_setNative(ST_TinyJS_Var* _this, TinyJS_Callback* callback, void* userData) {
	this->TinyJS_Var_init(TINYJS_VAR_NATIVE);
	this->callback = callback;
	this->userData = userData;
}
//-----------------------------------------------------------------------------
int TinyJS_Var_isUndefined(ST_TinyJS_Var* _this) {
	return this->type == TINYJS_VAR_UNDEFINED;
}
//-----------------------------------------------------------------------------
int TinyJS_Var_isNull(ST_TinyJS_Var* _this) {
	return this->type == TINYJS_VAR_NULL;
}
//-----------------------------------------------------------------------------
int TinyJS_Var_isNumber(ST_TinyJS_Var* _this) {
	return this->type == TINYJS_VAR_NUMBER;
}
//-----------------------------------------------------------------------------
int TinyJS_Var_isString(ST_TinyJS_Var* _this) {
	return this->type == TINYJS_VAR_STRING;
}
//-----------------------------------------------------------------------------
int TinyJS_Var_isFunction(ST_TinyJS_Var* _this) {
	return (this->type == TINYJS_VAR_FUNCTION) || (this->type == TINYJS_VAR_NATIVE);
}
//-----------------------------------------------------------------------------
int TinyJS_Var_isObject(ST_TinyJS_Var* _this) {
	return this->type == TINYJS_VAR_OBJECT;
}
//-----------------------------------------------------------------------------
int TinyJS_Var_isArray(ST_TinyJS_Var* _this) {
	return this->type == TINYJS_VAR_ARRAY;
}
//-----------------------------------------------------------------------------
int TinyJS_Var_isNative(ST_TinyJS_Var* _this) {
	return this->type == TINYJS_VAR_NATIVE;
}
//-----------------------------------------------------------------------------
int TinyJS_Var_isPrimitive(ST_TinyJS_Var* _this) {
	return (this->type == TINYJS_VAR_UNDEFINED) ||
	       (this->type == TINYJS_VAR_NULL) ||
	       (this->type == TINYJS_VAR_NUMBER) ||
	       (this->type == TINYJS_VAR_STRING);
}
//-----------------------------------------------------------------------------
//If this is a function, get the result value (for use by native functions).
ST_TinyJS_Var* TinyJS_Var_getReturnVar(ST_TinyJS_Var* _this) {
	ST_TinyJS_VarLink* resultVarLink = TinyJS_Var_findChild("return");
	if(!resultVarLink) {
		SEH_throw_msg(TinyJS_Exception, "TinyJS_Var_getReturnVar() has been called outside a function.");
	}
	return resultVarLink->var;
}
//-----------------------------------------------------------------------------
//Set the result value.
//Use this when setting complex return data as it avoids a TinyJS_Var_deepCopy().
void TinyJS_Var_setReturnVar(ST_TinyJS_Var* _this, ST_TinyJS_Var* v) {
	ST_TinyJS_VarLink* resultVarLink = TinyJS_Var_findChild("return");
	if(!resultVarLink) {
		SEH_throw_msg(TinyJS_Exception, "TinyJS_Var_setReturnVar() has been called outside a function.");
	}
	resultVarLink->TinyJS_VarLink_replaceWith(v);
}
//-----------------------------------------------------------------------------
//If this is a function, get the parameter with the given name (for use by native functions).
ST_TinyJS_Var* TinyJS_Var_getParameter(ST_TinyJS_Var* _this, const char* name) {
	return TinyJS_Var_findChildOrCreate(name)->var;
}
//-----------------------------------------------------------------------------
//Tries to find a child with the given name, may return NULL.
ST_TinyJS_VarLink* TinyJS_Var_findChild(ST_TinyJS_Var* _this, const char* name) {
	GSList/*<ST_TinyJS_VarLink*>*/* list = this->firstChild;
	while(list) {
		ST_TinyJS_VarLink* l = (ST_TinyJS_VarLink*)list->data;
		if(!strcmp(l->name, name)) { return l; }
		list = list->next;
	}
	return NULL;
}
//-----------------------------------------------------------------------------
//Tries to find a child with the given name, or will create it with the given type.
ST_TinyJS_VarLink* TinyJS_Var_findChildOrCreate(ST_TinyJS_Var* _this, const char* name) {
	ST_TinyJS_VarLink* l = TinyJS_Var_findChild(name);
	if(l) { return l; }
	return TinyJS_Var_addChild(name, TinyJS_Var_newUndefined());
}
//-----------------------------------------------------------------------------
ST_TinyJS_VarLink* TinyJS_Var_addChild(ST_TinyJS_Var* _this, const char* name, ST_TinyJS_Var* v) {
	if(TinyJS_Var_isUndefined()) { this->type = TINYJS_VAR_OBJECT; }	//※なぜ？
	if(TinyJS_Var_findChild(name)) {
		SEH_throw_msg(TinyJS_Exception, strdup_printf("'%s' already exists.", name));
	}
	ST_TinyJS_VarLink* l = TinyJS_VarLink_new(v);
	l->name  = name;
	l->owned = 1;
	this->firstChild = g_slist_append(this->firstChild, l);
	return l;
}
//-----------------------------------------------------------------------------
//Remove a specific link (this is faster than finding via a child).
void TinyJS_Var_removeLink(ST_TinyJS_Var* _this, ST_TinyJS_VarLink* l) {
	this->firstChild = g_slist_remove(this->firstChild, l);
}
//-----------------------------------------------------------------------------
//The the value at an array index.
ST_TinyJS_Var* TinyJS_Var_getArrayIndex(ST_TinyJS_Var* _this, int i) {
	const char* buf = strdup_printf("%d", i);
	ST_TinyJS_VarLink* l = TinyJS_Var_findChild(buf);
	if(l) {
		return l->var;
	} else {
		return TinyJS_Var_newUndefined();
	}
}
//-----------------------------------------------------------------------------
//Set the value at an array index.
void TinyJS_Var_setArrayIndex(ST_TinyJS_Var* _this, int i, ST_TinyJS_Var* v) {
	const char* buf = strdup_printf("%d", i);
	ST_TinyJS_VarLink* l = TinyJS_Var_findChild(buf);
	if(l) {
		if(v->TinyJS_Var_isUndefined()) {
			TinyJS_Var_removeLink(l);	//※本当はundefinedを代入しても配列要素は削除されないはずだが…当インタプリタにはdelete命令が無いようなのでその代用なのだろう。
		} else {
			l->TinyJS_VarLink_replaceWith(v);
		}
	} else {
		if(v->TinyJS_Var_isUndefined()) {
			/** no job **/	//※本当はundefinedを明示的に新規代入した場合も配列要素が生成されるはずだが…
		} else {
			TinyJS_Var_addChild(buf, v);
		}
	}
}
//-----------------------------------------------------------------------------
//If this is an array, return the number of items in it (else 0).
int TinyJS_Var_getArrayLength(ST_TinyJS_Var* _this) {
	if(!TinyJS_Var_isArray()) { return 0; }
	int ubound = -1;
	GSList/*<ST_TinyJS_VarLink*>*/* list = this->firstChild;
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
int TinyJS_Var_getBoolean(ST_TinyJS_Var* _this) {
	return this->TinyJS_Var_getNumber() != 0;	//※色々問題が有る気がする。getInt()がオブジェクトや文字列に対して0を返し、TinyJS_Var_getBoolean()でfalseになってしまう。せめてオブジェクトや空でない文字列はtrueと判断すべきではないか?(※TODO:要検討)
}
//-----------------------------------------------------------------------------
double TinyJS_Var_getNumber(ST_TinyJS_Var* _this) {
	if(TinyJS_Var_isNumber()) { return this->numData; }
//不要	if(TinyJS_Var_isNull()) { return 0; }
//不要	if(TinyJS_Var_isUndefined()) { return 0; }	//※本当はNumber(undefined)⇒NaNだが…
	return 0;			//※本当は0とは限らないが…せめて文字列⇒数値変換ぐらいは実装すべきではないか?(※TODO:)
}
//-----------------------------------------------------------------------------
const char* TinyJS_Var_getString(ST_TinyJS_Var* _this) {
	if(TinyJS_Var_isNumber()) { return strdup_printf("%g", this->numData); }
	if(TinyJS_Var_isNull()) { return "null"; }
	if(TinyJS_Var_isUndefined()) { return "undefined"; }
	return this->strData;		//※オブジェクトや配列の文字列化は未対応なのか?(※TODO:要検討)
}
//-----------------------------------------------------------------------------
//Get data as a parsable javascript string.
const char* TinyJS_Var_getParsableString(ST_TinyJS_Var* _this) {
	//Numbers can just be put in directly.
	if(TinyJS_Var_isNumber()) { return this->TinyJS_Var_getString(); }
	if(TinyJS_Var_isFunction()) {
		GString* funcStr = g_string_new("function (");
		//Get list of parameters.
		GSList/*<ST_TinyJS_VarLink*>*/* list = this->firstChild;
		while(list) {
			ST_TinyJS_VarLink* l = (ST_TinyJS_VarLink*)list->data;
			g_string_append(funcStr, l->name);
			if(list->next) { g_string_append(funcStr, ","); }
			list = list->next;
		}
		//Add function body.
		g_string_append(funcStr, ") ");
		g_string_append(funcStr, this->TinyJS_Var_getString());
		return funcStr->str;
	}
	//If it is a string then we quote it.
	if(TinyJS_Var_isString()) { return getJSString(TinyJS_Var_getString()); }
	if(TinyJS_Var_isNull()) { return "null"; }
	return "undefined";	//※TODO:オブジェクトや配列も間違って"undefined"と表示されてしまうのではないか?
}
//-----------------------------------------------------------------------------
int TinyJS_Var_equals(ST_TinyJS_Var* _this, ST_TinyJS_Var* v) {
	ST_TinyJS_Var* tmp = this->TinyJS_Var_mathsOp(v, TINYJS_LEX_O_EQUAL);
	return tmp->TinyJS_Var_getBoolean();
}
//-----------------------------------------------------------------------------
//Do a maths op with another script variable.
//※当関数の動作は本物のJavaScriptとだいぶん違う。簡略化のためにやむをえないとは思うが、もう少し本物のJavaScriptに近付けられないだろうか？(※TODO:要検討)
ST_TinyJS_Var* TinyJS_Var_mathsOp(ST_TinyJS_Var* _this, ST_TinyJS_Var* v, int op) {
	ST_TinyJS_Var* const a = this;	//┬読み易いように変数名を揃えているだけ。必須ではない。
	ST_TinyJS_Var* const b = v;	//┘
	//Type equality check.
	if((op == TINYJS_LEX_O_TYPEEQUAL) || (op == TINYJS_LEX_O_NTYPEEQUAL)) {
		//Check type first, then call again to check data.
		int eql = (a->type == b->type);
		if(eql) {
			ST_TinyJS_Var* tmp = a->TinyJS_Var_mathsOp(b, TINYJS_LEX_O_EQUAL);
			if(!tmp->TinyJS_Var_getBoolean()) { eql = 0; }
		}
		if(op == TINYJS_LEX_O_NTYPEEQUAL) { eql = !eql; }
		return TinyJS_Var_newNumber(eql);
	}
	//Do maths...
	if((a->TinyJS_Var_isUndefined() || a->TinyJS_Var_isNull()) && (b->TinyJS_Var_isUndefined() || b->TinyJS_Var_isNull())) {
		if(op == TINYJS_LEX_O_EQUAL) {
			return TinyJS_Var_newNumber(1);	//(undefined==null)もtrueです。(本物のJavaScriptでも)
		} else if(op == TINYJS_LEX_O_NEQUAL) {
			return TinyJS_Var_newNumber(0);	//(undefined!=null)もfalseです。(本物のJavaScriptでも)
		} else {
			return TinyJS_Var_newUndefined();	//undefined	※本当は違う。(undefined+null)⇒NaNや(undefined|null)⇒0など。とは言え実際にそれらの挙動に依存するようなスクリプトは書かないと思う。(※TODO:要検討)
		}
	} else if(a->TinyJS_Var_isNumber() && b->TinyJS_Var_isNumber()) {
		double da = a->TinyJS_Var_getNumber();
		double db = b->TinyJS_Var_getNumber();
		switch(op) {
		case '+':                 return TinyJS_Var_newNumber(     da +      db);
		case '-':                 return TinyJS_Var_newNumber(     da -      db);
		case '*':                 return TinyJS_Var_newNumber(     da *      db);
		case '/':                 return TinyJS_Var_newNumber(     da /      db);
		case '&':                 return TinyJS_Var_newNumber((int)da & (int)db);
		case '|':                 return TinyJS_Var_newNumber((int)da | (int)db);
		case '^':                 return TinyJS_Var_newNumber((int)da ^ (int)db);
		case '%':                 return TinyJS_Var_newNumber(fmod(da,       db));
		case TINYJS_LEX_O_EQUAL:  return TinyJS_Var_newNumber(     da ==     db);
		case TINYJS_LEX_O_NEQUAL: return TinyJS_Var_newNumber(     da !=     db);
		case '<':                 return TinyJS_Var_newNumber(     da <      db);
		case TINYJS_LEX_O_LEQUAL: return TinyJS_Var_newNumber(     da <=     db);
		case '>':                 return TinyJS_Var_newNumber(     da >      db);
		case TINYJS_LEX_O_GEQUAL: return TinyJS_Var_newNumber(     da >=     db);
		default:SEH_throw_msg(TinyJS_Exception, strdup_printf("Operation %s not supported on the number datatype.", TinyJS_Lex_getTokenStr(op)));
		}
	} else if(a->TinyJS_Var_isArray()) {
		//Just check pointers.
		switch(op) {
		case TINYJS_LEX_O_EQUAL:  return TinyJS_Var_newNumber(a == b);
		case TINYJS_LEX_O_NEQUAL: return TinyJS_Var_newNumber(a != b);
		default:SEH_throw_msg(TinyJS_Exception, strdup_printf("Operation %s not supported on the Array datatype.", TinyJS_Lex_getTokenStr(op)));
		}
	} else if(a->TinyJS_Var_isObject()) {
		//Just check pointers.
		switch(op) {
		case TINYJS_LEX_O_EQUAL:  return TinyJS_Var_newNumber(a == b);
		case TINYJS_LEX_O_NEQUAL: return TinyJS_Var_newNumber(a != b);
		default:SEH_throw_msg(TinyJS_Exception, strdup_printf("Operation %s not supported on the Object datatype.", TinyJS_Lex_getTokenStr(op)));
		}
	} else {
		const char* da = a->TinyJS_Var_getString();
		const char* db = b->TinyJS_Var_getString();
		//Use strings.
		switch(op) {
		case '+':                 return TinyJS_Var_newString(strconcat(da, db, NULL));
		case TINYJS_LEX_O_EQUAL:  return TinyJS_Var_newNumber(strcmp(da, db) == 0);
		case TINYJS_LEX_O_NEQUAL: return TinyJS_Var_newNumber(strcmp(da, db) != 0);
		case '<':                 return TinyJS_Var_newNumber(strcmp(da, db) <  0);
		case TINYJS_LEX_O_LEQUAL: return TinyJS_Var_newNumber(strcmp(da, db) <= 0);
		case '>':                 return TinyJS_Var_newNumber(strcmp(da, db) >  0);
		case TINYJS_LEX_O_GEQUAL: return TinyJS_Var_newNumber(strcmp(da, db) >= 0);
		default:SEH_throw_msg(TinyJS_Exception, strdup_printf("Operation %s not supported on the string datatype.", TinyJS_Lex_getTokenStr(op)));
		}
	}
}
//-----------------------------------------------------------------------------
//Deep copy this node and return the result.
ST_TinyJS_Var* TinyJS_Var_deepCopy(ST_TinyJS_Var* _this) {
	ST_TinyJS_Var* v = TinyJS_Var_newUndefined();
	//Copy the basic data and type from the variable given, with no children.
	v->type    = this->type;
	v->numData = this->numData;
	v->strData = this->strData;
	//Copy children.
	GSList/*<ST_TinyJS_VarLink*>*/* list = this->firstChild;
	while(list) {
		ST_TinyJS_VarLink* l = (ST_TinyJS_VarLink*)list->data;
		ST_TinyJS_Var* tmp;
		//Don't copy the 'prototype' object...
		if(!strcmp(l->name, "prototype")) {	//"prototype"ならば…
			tmp = l->var;
		} else {				//"prototype"以外ならば…
			tmp = l->var->TinyJS_Var_deepCopy();
		}
		v->TinyJS_Var_addChild(l->name, tmp);
		list = list->next;
	}
	return v;
}
//-----------------------------------------------------------------------------
//Dump out the contents of this using trace.
void TinyJS_Var_trace(ST_TinyJS_Var* _this, const char* indent, const char* name) {
	printf("%s'%s' = '%s' %s\n",
		indent,
		name,
		TinyJS_Var_getString(),
		TinyJS_Var_getTypeAsString());
	const char* indentStr = strconcat(indent, " ", NULL);
	GSList/*<ST_TinyJS_VarLink*>*/* list = this->firstChild;
	while(list) {
		ST_TinyJS_VarLink* l = (ST_TinyJS_VarLink*)list->data;
		l->var->TinyJS_Var_trace(indentStr, l->name);
		list = list->next;
	}
}
//-----------------------------------------------------------------------------
//For debugging - just dump a string version of the type.
const char* TinyJS_Var_getTypeAsString(ST_TinyJS_Var* _this) {
	if(this->type == TINYJS_VAR_NUMBER)   { return "NUMBER"; }
	if(this->type == TINYJS_VAR_STRING)   { return "STRING"; }
	if(this->type == TINYJS_VAR_FUNCTION) { return "FUNCTION"; }
	if(this->type == TINYJS_VAR_OBJECT)   { return "OBJECT"; }
	if(this->type == TINYJS_VAR_ARRAY)    { return "ARRAY"; }
	if(this->type == TINYJS_VAR_NATIVE)   { return "NATIVE"; }
	return "";
}
//-----------------------------------------------------------------------------
//Write out all the JS code needed to recreate this script variable to the stream (as JSON).
const char* TinyJS_Var_getJSON(ST_TinyJS_Var* _this, const char* linePrefix) {
	GString* indentedLinePrefix = g_string_new(linePrefix);
	g_string_append(indentedLinePrefix, "  ");
	GString* destination = g_string_new(NULL);
	if(TinyJS_Var_isObject()) {
		//Children - handle with bracketed list.
		g_string_append(destination, "{\n");
		GSList/*<ST_TinyJS_VarLink*>*/* list = this->firstChild;
		while(list) {
			ST_TinyJS_VarLink* l = (ST_TinyJS_VarLink*)list->data;
			g_string_append(destination, indentedLinePrefix->str);
			g_string_append(destination, getJSString(l->name));
			g_string_append(destination, ": ");
			g_string_append(destination, l->var->TinyJS_Var_getJSON(indentedLinePrefix->str));
			if(list->next) { g_string_append(destination, ","); }
			g_string_append(destination, "\n");
			list = list->next;
		}
		g_string_append(destination, linePrefix);
		g_string_append(destination, "}");
	} else if(TinyJS_Var_isArray()) {
		g_string_append(destination, "[\n");
		int len = this->TinyJS_Var_getArrayLength();
		if(len > 10000) { len = 10000; }	//We don't want to get stuck here!
		for(int i = 0; i < len; i++) {
			g_string_append(destination, TinyJS_Var_getArrayIndex(i)->TinyJS_Var_getJSON(indentedLinePrefix->str));
			if(i < len - 1) { g_string_append(destination, ","); }
			g_string_append(destination, "\n");
		}
		g_string_append(destination, linePrefix);
		g_string_append(destination, "]");
	} else {
		//No children or a function... just write value directly.
		g_string_append(destination, this->TinyJS_Var_getParsableString());
	}
	return destination->str;
}
//-----------------------------------------------------------------------------
void TinyJS_Var_init(ST_TinyJS_Var* _this, int varType) {
	this->type       = varType;
	this->numData    = 0;
	this->strData    = "";
	this->callback   = NULL;
	this->userData   = NULL;
	this->firstChild = NULL;
}
//*****************************************************************************
//	ST_TinyJS_VarLink
//*****************************************************************************
/*static*/ ST_TinyJS_VarLink* TinyJS_VarLink_new(ST_TinyJS_Var* v) {
	ST_TinyJS_VarLink* _this = new ST_TinyJS_VarLink();
	_this->name  = "";
	_this->var   = v;
	_this->owned = 0;
	return _this;
}
//-----------------------------------------------------------------------------
//Replace the Variable pointed to.
void TinyJS_VarLink_replaceWith(ST_TinyJS_VarLink* _this, ST_TinyJS_Var* v) {
	if(!v) { DIE(); }	//バグ
	this->var = v;
}
//-----------------------------------------------------------------------------
//Get the name as an integer (for arrays).
int TinyJS_VarLink_getIntName(ST_TinyJS_VarLink* _this) {
	return atoi(this->name);
}
//-----------------------------------------------------------------------------
//Set the name as an integer (for arrays).
void TinyJS_VarLink_setIntName(ST_TinyJS_VarLink* _this, int n) {
	const char* buf = strdup_printf("%d", n);
	this->name = buf;
}
