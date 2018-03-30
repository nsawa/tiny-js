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
//	TinyJS(移植版)
//
//	Ported By Naoyuki Sawa <nsawa@piece-me.org>
//	Copyright (C) 2018 Piece Lab.
//
//	* Fri Mar 30 23:59:59 JST 2018 Naoyuki Sawa
//	- 1st リリース。
//	- 「TinyJS」(https://github.com/gfwilliams/tiny-js)を、P/ECEで動作するように移植したものです。
//	  オリジナル版のTinyJSはC++で書かれていますが、P/ECE開発環境にはGCC 2.7.2相当のCコンパイラしか無いので、C++からC言語に移植しました。
//	  C++とC言語はだいぶん違うため、コードはほぼ全て書き換えてしまいましたが、アルゴリズムはオリジナル版のTinyJSとだいたい同じままです。
//	  尚、メインターゲットはP/ECEですが、(少なくとも現時点では)Visual Studio 2017と、Visual C++ 6.0でもビルド出来ています。
//
#include "clip.h"
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
	ST_TinyJS* _this = (ST_TinyJS*)malloc(sizeof(ST_TinyJS));
	_this->lex       = NULL;
	_this->scopes    = NULL;
	_this->callStack = NULL;
	_this->root      = TinyJS_Var_newObject();
	//Add built-in classes.
	_this->stringClass = TinyJS_Var_newObject();
	_this->arrayClass  = TinyJS_Var_newObject();
	_this->objectClass = TinyJS_Var_newObject();
	TinyJS_Var_addChild(_this->root, "String", _this->stringClass);
	TinyJS_Var_addChild(_this->root, "Array",  _this->arrayClass);
	TinyJS_Var_addChild(_this->root, "Object", _this->objectClass);
	return _this;
}
//-----------------------------------------------------------------------------
void TinyJS_execute(ST_TinyJS* _this, const char* code) {
	ST_TinyJS_Context* pSavedContext = TinyJS_saveContext(_this);
	{
		//{{InitContext
		_this->lex       = TinyJS_Lex_new(code, 0, strlen(code));
		_this->scopes    = g_slist_prepend(NULL, _this->root);
		_this->callStack = NULL;
		//}}InitContext
		SEH_try {
			int bExec = 1;
			while(_this->lex->tk) { TinyJS_statement(_this, &bExec); }
		} SEH_catch(TinyJS_Exception) {
			const char* msg = TinyJS_stackTrace(_this, SEH_info.msg);
			TinyJS_restoreContext(_this, pSavedContext);
			SEH_throw_msg(TinyJS_Exception, msg);
		} SEH_end
	}
	TinyJS_restoreContext(_this, pSavedContext);
}
//-----------------------------------------------------------------------------
//Evaluate the given code and return a javascript object, useful for(dangerous) JSON parsing.
//If nothing to return, will return 'undefined' variable type.
ST_TinyJS_Var* TinyJS_evaluate(ST_TinyJS* _this, const char* code) {
	ST_TinyJS_VarLink* l = NULL;
	ST_TinyJS_Context* pSavedContext = TinyJS_saveContext(_this);
	{
		//{{InitContext
		_this->lex       = TinyJS_Lex_new(code, 0, strlen(code));
		_this->scopes    = g_slist_prepend(NULL, _this->root);
		_this->callStack = NULL;
		//}}InitContext
		SEH_try {
			int bExec = 1;
			do {
				l = TinyJS_base(_this, &bExec);
				if(_this->lex->tk != TINYJS_LEX_EOF) { TinyJS_Lex_match(_this->lex, ';'); }
			} while(_this->lex->tk != TINYJS_LEX_EOF);
		} SEH_catch(TinyJS_Exception) {
			const char* msg = TinyJS_stackTrace(_this, SEH_info.msg);
			TinyJS_restoreContext(_this, pSavedContext);
			SEH_throw_msg(TinyJS_Exception, msg);
		} SEH_end
	}
	TinyJS_restoreContext(_this, pSavedContext);
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
	ST_TinyJS_Context* pSavedContext = TinyJS_saveContext(_this);
	{
		//{{InitContext
		_this->lex = TinyJS_Lex_new(funcDesc, 0, strlen(funcDesc));
		//}}InitContext
		{
			ST_TinyJS_Var* base = _this->root;
			TinyJS_Lex_match(_this->lex, TINYJS_LEX_R_FUNCTION);
			{
				const char* funcName = _this->lex->tkStr;
				TinyJS_Lex_match(_this->lex, TINYJS_LEX_ID);
				//Check for dots, we might want to do something like function String.substring ...
				while(_this->lex->tk == '.') {
					TinyJS_Lex_match(_this->lex, '.');
					{
						ST_TinyJS_VarLink* l = TinyJS_Var_findChild(base, funcName);
						//If it doesn't exist, make an object class.
						if(!l) { l = TinyJS_Var_addChild(base, funcName, TinyJS_Var_newObject()); }
						base = l->var;
					}
					funcName = _this->lex->tkStr;
					TinyJS_Lex_match(_this->lex, TINYJS_LEX_ID);
				}
				{
					ST_TinyJS_Var* funcVar = TinyJS_Var_newNative(callback, userData);
					TinyJS_parseFunctionArguments(_this, funcVar);
					TinyJS_Var_addChild(base, funcName, funcVar);
				}
			}
		}
	}
	TinyJS_restoreContext(_this, pSavedContext);
}
//-----------------------------------------------------------------------------
//Send all variables to stdout.
void TinyJS_trace(ST_TinyJS* _this, const char* indent) {
	TinyJS_Var_trace(_this->root, indent, "root");
}
//-----------------------------------------------------------------------------
//Handle a function call (assumes we've parsed the function name and we're on the start bracket).
//'obj' is the object that contains this method, if there was one (otherwise it's just a normnal function).
ST_TinyJS_VarLink* TinyJS_functionCall(ST_TinyJS* _this, int* pExec, ST_TinyJS_VarLink* func, ST_TinyJS_Var* obj/*NULL可*/) {
	if(*pExec) {
		if(!TinyJS_Var_isFunction(func->var)) {
			SEH_throw_msg(TinyJS_Exception, strdup_printf("Expecting '%s' to be a function.", func->name));
		}
		TinyJS_Lex_match(_this->lex, '(');
		{
			//Create a new symbol table entry for execution of this function.
			ST_TinyJS_Var* funcRoot = TinyJS_Var_newObject();
			if(obj) {
				TinyJS_Var_addChild(funcRoot, "this", obj);
			}
			{
				//Grab in all parameters.
				GSList/*<ST_TinyJS_VarLink*>*/* list = func->var->firstChild;
				while(list) {
					ST_TinyJS_VarLink* l = (ST_TinyJS_VarLink*)list->data;
					ST_TinyJS_VarLink* tmp = TinyJS_base(_this, pExec);
					if(*pExec) {
						if(TinyJS_Var_isPrimitive(tmp->var)) {
							//pass by value
							TinyJS_Var_addChild(funcRoot, l->name, TinyJS_Var_deepCopy(tmp->var));
						} else {
							//pass by reference
							TinyJS_Var_addChild(funcRoot, l->name, tmp->var);
						}
					}
					if(list->next) { TinyJS_Lex_match(_this->lex, ','); }
					list = list->next;
				}
			}
			TinyJS_Lex_match(_this->lex, ')');
			{
				//Execute function!
				//Add the function's execute space to the symbol table so we can recurse.
				ST_TinyJS_VarLink* returnVarLink = TinyJS_Var_addChild(funcRoot, "return", TinyJS_Var_newUndefined());
				ST_TinyJS_Context* pSavedContext = TinyJS_saveContext(_this);
				{
					//We just want to execute the block, but something could have messed up and left us with the wrong ScriptLex, so we want to be careful here...
					const char* funcBody = TinyJS_Var_getString(func->var);
					//{{InitContext
					_this->lex       = TinyJS_Lex_new(funcBody, 0, strlen(funcBody));
					_this->scopes    = g_slist_prepend(_this->scopes, funcRoot);
					_this->callStack = g_slist_prepend(_this->callStack, strdup_printf("%s from %s", func->name, TinyJS_Lex_getLastPosition(_this->lex)));
					//}}InitContext
					SEH_try {
						if(TinyJS_Var_isNative(func->var)) {
							(*func->var->callback)(_this, funcRoot, func->var->userData);
						} else {
							TinyJS_block(_this, pExec);
							//Because return will probably have called this, and set execute to false.
							*pExec = 1;
						}
					} SEH_catch(TinyJS_Exception) {
						TinyJS_restoreContext(_this, pSavedContext);
						SEH_throw_last;
					} SEH_end
				}
				TinyJS_restoreContext(_this, pSavedContext);
			//不要	funcRoot->TinyJS_Var_removeLink(returnVarLink);	⇒funcRootは当関数内を抜けたら無効なので、クリーンアップする必要も無い。
				return returnVarLink;
			}
		}
	} else {
		//Function, but not executing - just parse args and be done.
		TinyJS_Lex_match(_this->lex, '(');
		while(_this->lex->tk != ')') {
			ST_TinyJS_VarLink* l = TinyJS_base(_this, pExec);
			if(_this->lex->tk != ')') { TinyJS_Lex_match(_this->lex, ','); }
		}
		TinyJS_Lex_match(_this->lex, ')');
		if(_this->lex->tk == '{') {	//TODO: Why is this here?
			TinyJS_block(_this, pExec);
		}
		return TinyJS_VarLink_new(TinyJS_Var_newUndefined());
	}
}
//-----------------------------------------------------------------------------
//因子
ST_TinyJS_VarLink* TinyJS_factor(ST_TinyJS* _this, int* pExec) {
	//(～)
	if(_this->lex->tk == '(') {
		TinyJS_Lex_match(_this->lex, '(');
		{
			ST_TinyJS_VarLink* a = TinyJS_base(_this, pExec);
			TinyJS_Lex_match(_this->lex, ')');
			return a;
		}
	//true
	} else if(_this->lex->tk == TINYJS_LEX_R_TRUE) {
		TinyJS_Lex_match(_this->lex, TINYJS_LEX_R_TRUE);
		return TinyJS_VarLink_new(TinyJS_Var_newNumber(1));
	//false
	} else if(_this->lex->tk == TINYJS_LEX_R_FALSE) {
		TinyJS_Lex_match(_this->lex, TINYJS_LEX_R_FALSE);
		return TinyJS_VarLink_new(TinyJS_Var_newNumber(0));
	//null
	} else if(_this->lex->tk == TINYJS_LEX_R_NULL) {
		TinyJS_Lex_match(_this->lex, TINYJS_LEX_R_NULL);
		return TinyJS_VarLink_new(TinyJS_Var_newNull());
	//undefined
	} else if(_this->lex->tk == TINYJS_LEX_R_UNDEFINED) {
		TinyJS_Lex_match(_this->lex, TINYJS_LEX_R_UNDEFINED);
		return TinyJS_VarLink_new(TinyJS_Var_newUndefined());
	//ID
	} else if(_this->lex->tk == TINYJS_LEX_ID) {
		ST_TinyJS_VarLink* a = NULL;
		if(*pExec) {
			a = TinyJS_findInScopes(_this, _this->lex->tkStr);
		}
		if(!a) {
			//Variable doesn't exist! JavaScript says we should create it.
			//(We won't add it here. This is done in the assignment operator.)
			a = TinyJS_VarLink_new(TinyJS_Var_newUndefined());
			a->name = _this->lex->tkStr;
		}
		TinyJS_Lex_match(_this->lex, TINYJS_LEX_ID);
		{
			//ID(～),又は,ID.ID,又は,ID[～]を、IDに還元して繰り返す。
			ST_TinyJS_Var* obj = NULL;	//The 'this' if we're executing a method call.
			while((_this->lex->tk == '(') ||
			      (_this->lex->tk == '.') ||
			      (_this->lex->tk == '[')) {
				switch(_this->lex->tk) {
				default:DIE();	//バグ
				//Function Call.
				case '(':
					{
						a = TinyJS_functionCall(_this, pExec, a, obj);
					}
					break;
				//Record Access.
				case '.':
					{
						TinyJS_Lex_match(_this->lex, '.');
						if(*pExec) {
							const char* name = _this->lex->tkStr;
							ST_TinyJS_VarLink* l = TinyJS_Var_findChild(a->var, name);
							if(!l) { l = TinyJS_findInPrototypeClasses(_this, a->var, name); }
							if(!l) {
								//If we haven't found this defined yet, use the built-in 'length' properly.
								if(TinyJS_Var_isArray(a->var) && !strcmp(name, "length")) {
									int len = TinyJS_Var_getArrayLength(a->var);
									l = TinyJS_VarLink_new(TinyJS_Var_newNumber(len));
								} else if(TinyJS_Var_isString(a->var) && !strcmp(name, "length")) {
									int len = strlen(TinyJS_Var_getString(a->var));
									l = TinyJS_VarLink_new(TinyJS_Var_newNumber(len));
								} else {
									l = TinyJS_Var_addChild(a->var, name, TinyJS_Var_newUndefined());
								}
							}
							obj = a->var;
							a = l;
						}
						TinyJS_Lex_match(_this->lex, TINYJS_LEX_ID);
					}
					break;
				//Array Access.
				case '[':
					{
						TinyJS_Lex_match(_this->lex, '[');
						{
							ST_TinyJS_VarLink* index = TinyJS_base(_this, pExec);
							TinyJS_Lex_match(_this->lex, ']');
							if(*pExec) {
								ST_TinyJS_VarLink* l = TinyJS_Var_findChildOrCreate(a->var, TinyJS_Var_getString(index->var));
								obj = a->var;
								a = l;
							}
						}
					}
					break;
				}
			}
		}
		return a;
	//数値リテラル
	} else if(_this->lex->tk == TINYJS_LEX_L_NUM) {
		double value;
		char* endptr;
#if     (defined(_MSC_VER) && (_MSC_VER > 1200))	//Visual C++ .NET
		//strtod()は10進実数と16進整数を解釈するが、8進整数を解釈しないので、8進整数は明示的に変換する事にした。
		if((_this->lex->tkStr[0] == '0') && (strspn(_this->lex->tkStr, "01234567") == strlen(_this->lex->tkStr))) {
			value = strtoul(_this->lex->tkStr, &endptr, 8);
		} else {
			value = strtod(_this->lex->tkStr, &endptr);
		}
#else //(defined(_MSC_VER) && (_MSC_VER > 1200))	//Visual C++ 6.0 or P/ECE開発環境
		//strtod()は10進実数を解釈するが、8進整数と16進整数を解釈しないので、8進整数と16進整数は明示的に変換する事にした。
		if((_this->lex->tkStr[0] == '0') && (strspn(_this->lex->tkStr, "01234567") == strlen(_this->lex->tkStr))) {
			value = strtoul(_this->lex->tkStr, &endptr, 8);
		} else if((_this->lex->tkStr[0] == '0') && ((_this->lex->tkStr[1] == 'x') || (_this->lex->tkStr[1] == 'X'))) {
			value = strtoul(_this->lex->tkStr, &endptr, 16);
		} else {
			value = strtod(_this->lex->tkStr, &endptr);
		}
#endif//(defined(_MSC_VER) && (_MSC_VER > 1200))
		if(*endptr) { DIE(); }	//バグ
		{
			ST_TinyJS_Var* v = TinyJS_Var_newNumber(value);
			TinyJS_Lex_match(_this->lex, TINYJS_LEX_L_NUM);
			return TinyJS_VarLink_new(v);
		}
	//文字列リテラル
	} else if(_this->lex->tk == TINYJS_LEX_L_STR) {
		ST_TinyJS_Var* v = TinyJS_Var_newString(_this->lex->tkStr);
		TinyJS_Lex_match(_this->lex, TINYJS_LEX_L_STR);
		return TinyJS_VarLink_new(v);
	//「{id:value,...}」
	} else if(_this->lex->tk == '{') {
		TinyJS_Lex_match(_this->lex, '{');
		{
			ST_TinyJS_Var* contents = TinyJS_Var_newObject();
			while(_this->lex->tk != '}') {
				const char* id = _this->lex->tkStr;
				//We only allow strings or IDs on the left hand side of an initialisation.
				TinyJS_Lex_match(_this->lex, (_this->lex->tk == TINYJS_LEX_ID) ? TINYJS_LEX_ID : TINYJS_LEX_L_STR);
				TinyJS_Lex_match(_this->lex, ':');
				{
					ST_TinyJS_VarLink* l = TinyJS_base(_this, pExec);
					TinyJS_Var_addChild(contents, id, l->var);
				}
				if(_this->lex->tk != '}') { TinyJS_Lex_match(_this->lex, ','); }
			}
			TinyJS_Lex_match(_this->lex, '}');
			return TinyJS_VarLink_new(contents);
		}
	//「[value,...]」
	} else if(_this->lex->tk == '[') {
		TinyJS_Lex_match(_this->lex, '[');
		{
			ST_TinyJS_Var* contents = TinyJS_Var_newArray();
			int i = 0;
			while(_this->lex->tk != ']') {
				ST_TinyJS_VarLink* l = TinyJS_base(_this, pExec);
				TinyJS_Var_addChild(contents, strdup_printf("%d", i), l->var);
				if(_this->lex->tk != ']') { TinyJS_Lex_match(_this->lex, ','); }
				i++;
			}
			TinyJS_Lex_match(_this->lex, ']');
			return TinyJS_VarLink_new(contents);
		}
	//「function(仮引数名,...){～}」
	} else if(_this->lex->tk == TINYJS_LEX_R_FUNCTION) {
		ST_TinyJS_VarLink* funcDef = TinyJS_parseFunctionDefinition(_this);
		if(strcmp(funcDef->name, "")) {
			SEH_throw_msg(TinyJS_Exception, "Functions not defined at statement-level are not meant to have a name.");
		}
		return funcDef;
	//「new 関数名(～)」,又は,「new プロトタイプ名」,又は,「new プロトタイプ名()」
	} else if(_this->lex->tk == TINYJS_LEX_R_NEW) {
		TinyJS_Lex_match(_this->lex, TINYJS_LEX_R_NEW);
		{
			const char* className = _this->lex->tkStr;
			TinyJS_Lex_match(_this->lex, TINYJS_LEX_ID);
			{
				ST_TinyJS_Var* obj = TinyJS_Var_newObject();
				if(*pExec) {
					ST_TinyJS_VarLink* objClassOrFunc = TinyJS_findInScopes(_this, className);
					if(!objClassOrFunc) {
						SEH_throw_msg(TinyJS_Exception, strdup_printf("%s is not a valid class name.", className));
					}
					if(TinyJS_Var_isFunction(objClassOrFunc->var)) {
						TinyJS_functionCall(_this, pExec, objClassOrFunc, obj);
					} else {
						TinyJS_Var_addChild(obj, "prototype", objClassOrFunc->var);
						if(_this->lex->tk == '(') {
							TinyJS_Lex_match(_this->lex, '(');
							TinyJS_Lex_match(_this->lex, ')');
						}
					}
				} else {
					if(_this->lex->tk == '(') {
						TinyJS_Lex_match(_this->lex, '(');
						TinyJS_Lex_match(_this->lex, ')');
					}
				}
				return TinyJS_VarLink_new(obj);
			}
		}
	} else {
		SEH_throw_msg(TinyJS_Exception, "Syntax error.");
	}
}
//-----------------------------------------------------------------------------
//単項演算子
ST_TinyJS_VarLink* TinyJS_unary(ST_TinyJS* _this, int* pExec) {
	ST_TinyJS_VarLink* a;
	if(_this->lex->tk == '-') {
		TinyJS_Lex_match(_this->lex, '-');
		a = TinyJS_unary(_this, pExec);
		if(*pExec) {
			ST_TinyJS_Var* tmp = TinyJS_Var_newNumber(0);
			ST_TinyJS_Var* res = TinyJS_Var_mathsOp(tmp, a->var, '-');
			a = TinyJS_VarLink_new(res);
		}
	} else if(_this->lex->tk == '!') {
		TinyJS_Lex_match(_this->lex, '!');
		a = TinyJS_unary(_this, pExec);
		if(*pExec) {
			ST_TinyJS_Var* tmp = TinyJS_Var_newNumber(0);
			ST_TinyJS_Var* res = TinyJS_Var_mathsOp(a->var, tmp, TINYJS_LEX_O_EQUAL);
			a = TinyJS_VarLink_new(res);
		}
	} else {
		a = TinyJS_factor(_this, pExec);
	}
	return a;
}
//-----------------------------------------------------------------------------
//項
ST_TinyJS_VarLink* TinyJS_term(ST_TinyJS* _this, int* pExec) {
	ST_TinyJS_VarLink* a = TinyJS_unary(_this, pExec);
	while((_this->lex->tk == '*') ||
	      (_this->lex->tk == '/') ||
	      (_this->lex->tk == '%')) {
		int op = _this->lex->tk;
		TinyJS_Lex_match(_this->lex, op);
		{
			ST_TinyJS_VarLink* b = TinyJS_unary(_this, pExec);
			if(*pExec) {
				a = TinyJS_VarLink_new(TinyJS_Var_mathsOp(a->var, b->var, op));
			}
		}
	}
	return a;
}
//-----------------------------------------------------------------------------
//式
ST_TinyJS_VarLink* TinyJS_expression(ST_TinyJS* _this, int* pExec) {
	ST_TinyJS_VarLink* a = TinyJS_term(_this, pExec);
	while((_this->lex->tk == '+') ||
	      (_this->lex->tk == '-') ||
	      (_this->lex->tk == TINYJS_LEX_O_PLUSPLUS) ||	//"++"
	      (_this->lex->tk == TINYJS_LEX_O_MINUSMINUS)) {	//"--"
		int op = _this->lex->tk;
		TinyJS_Lex_match(_this->lex, op);
		if((op == TINYJS_LEX_O_PLUSPLUS) ||	//"++"
		   (op == TINYJS_LEX_O_MINUSMINUS)) {	//"--"
			if(*pExec) {
				ST_TinyJS_VarLink* oldVarLink = TinyJS_VarLink_new(a->var);
				ST_TinyJS_Var* tmp = TinyJS_Var_newNumber(1);
				TinyJS_VarLink_replaceWith(a, TinyJS_Var_mathsOp(a->var, tmp, (op == TINYJS_LEX_O_PLUSPLUS) ? '+' : '-'));	//In-place add/subtract.
				a = oldVarLink;
			}
		} else {
			ST_TinyJS_VarLink* b = TinyJS_term(_this, pExec);
			if(*pExec) {
				a = TinyJS_VarLink_new(TinyJS_Var_mathsOp(a->var, b->var, op));						//Not in-place, so just replace.
			}
		}
	}
	return a;
}
//-----------------------------------------------------------------------------
//シフト式
ST_TinyJS_VarLink* TinyJS_shift(ST_TinyJS* _this, int* pExec) {
	ST_TinyJS_VarLink* a = TinyJS_expression(_this, pExec);
	if((_this->lex->tk == TINYJS_LEX_O_LSHIFT) ||
	   (_this->lex->tk == TINYJS_LEX_O_RSHIFT) ||
	   (_this->lex->tk == TINYJS_LEX_O_RSHIFTUNSIGNED)) {
		int op = _this->lex->tk;
		TinyJS_Lex_match(_this->lex, op);
		{
			ST_TinyJS_VarLink* b = TinyJS_base(_this, pExec);
			if(*pExec) {
				int value = TinyJS_Var_getNumber(a->var);
				int shift = TinyJS_Var_getNumber(b->var);
				switch(op) {
				default:DIE();	//バグ
				case TINYJS_LEX_O_LSHIFT:         value =           value << shift; break;
				case TINYJS_LEX_O_RSHIFT:         value =           value >> shift; break;
				case TINYJS_LEX_O_RSHIFTUNSIGNED: value = (unsigned)value >> shift; break;
				}
				a = TinyJS_VarLink_new(TinyJS_Var_newNumber(value));
			}
		}
	}
	return a;
}
//-----------------------------------------------------------------------------
//条件式
ST_TinyJS_VarLink* TinyJS_condition(ST_TinyJS* _this, int* pExec) {
	ST_TinyJS_VarLink* a = TinyJS_shift(_this, pExec);
	while((_this->lex->tk == '<') ||
	      (_this->lex->tk == '>') ||
	      (_this->lex->tk == TINYJS_LEX_O_LEQUAL) ||	//"<="
	      (_this->lex->tk == TINYJS_LEX_O_GEQUAL) ||	//">="
	      (_this->lex->tk == TINYJS_LEX_O_EQUAL) ||		//"=="
	      (_this->lex->tk == TINYJS_LEX_O_NEQUAL) ||	//"!="
	      (_this->lex->tk == TINYJS_LEX_O_TYPEEQUAL) ||	//"==="
	      (_this->lex->tk == TINYJS_LEX_O_NTYPEEQUAL)) {	//"!=="
		int op = _this->lex->tk;
		TinyJS_Lex_match(_this->lex, op);
		{
			ST_TinyJS_VarLink* b = TinyJS_shift(_this, pExec);
			if(*pExec) {
				a = TinyJS_VarLink_new(TinyJS_Var_mathsOp(a->var, b->var, op));
			}
		}
	}
	return a;
}
//-----------------------------------------------------------------------------
//論理式
ST_TinyJS_VarLink* TinyJS_logic(ST_TinyJS* _this, int* pExec) {
	ST_TinyJS_VarLink* a = TinyJS_condition(_this, pExec);
	while((_this->lex->tk == '&') ||
	      (_this->lex->tk == '|') ||
	      (_this->lex->tk == '^') ||
	      (_this->lex->tk == TINYJS_LEX_O_ANDAND) ||	//"&&"
	      (_this->lex->tk == TINYJS_LEX_O_OROR)) {		//"||"
		int op = _this->lex->tk;
		TinyJS_Lex_match(_this->lex, op);
		{
			//If we have short-circuit ops, then if we know the outcome we don't bother to execute the other op.
			//Even if not we need to tell TinyJS_Var_mathsOp it's an & or |.
			int shortCircuit, isBoolean;
			if(op == TINYJS_LEX_O_ANDAND) {		//"&&"
				op = '&';
				shortCircuit = !TinyJS_Var_getBoolean(a->var);
				isBoolean    = 1;
			} else if(op == TINYJS_LEX_O_OROR) {	//"||"
				op = '|';
				shortCircuit =  TinyJS_Var_getBoolean(a->var);
				isBoolean    = 1;
			} else {
				shortCircuit = 0;
				isBoolean    = 0;
			}
			{
				ST_TinyJS_VarLink* b = TinyJS_condition(_this, shortCircuit ? &noexec : pExec);
				if(*pExec) {
					if(!shortCircuit) {
						if(isBoolean) {
							a = TinyJS_VarLink_new(TinyJS_Var_newNumber(TinyJS_Var_getBoolean(a->var)));
							b = TinyJS_VarLink_new(TinyJS_Var_newNumber(TinyJS_Var_getBoolean(b->var)));
						}
						a = TinyJS_VarLink_new(TinyJS_Var_mathsOp(a->var, b->var, op));
					}
				}
			}
		}
	}
	return a;
}
//-----------------------------------------------------------------------------
//三項演算子
ST_TinyJS_VarLink* TinyJS_ternary(ST_TinyJS* _this, int* pExec) {
	ST_TinyJS_VarLink* a = TinyJS_logic(_this, pExec);
	if(_this->lex->tk == '?') {
		TinyJS_Lex_match(_this->lex, '?');
		if(TinyJS_Var_getBoolean(a->var)) {
			a = TinyJS_base(_this, pExec);
			TinyJS_Lex_match(_this->lex, ':');
			TinyJS_base(_this, &noexec);
		} else {
			TinyJS_base(_this, &noexec);
			TinyJS_Lex_match(_this->lex, ':');
			a = TinyJS_base(_this, pExec);
		}
	}
	return a;
}
//-----------------------------------------------------------------------------
//代入式
ST_TinyJS_VarLink* TinyJS_base(ST_TinyJS* _this, int* pExec) {
	ST_TinyJS_VarLink* lhs = TinyJS_ternary(_this, pExec);
	if((_this->lex->tk == '=') ||
	   (_this->lex->tk == TINYJS_LEX_O_PLUSASSIGN) ||
	   (_this->lex->tk == TINYJS_LEX_O_MINUSASSIGN)) {
		//If we're assigning to this and we don't have a prototype, add it to the symbol table root as per JavaScript.
		if(*pExec) {
			if(!lhs->owned) {
				if(!strcmp(lhs->name, "")) {
					SEH_throw_msg(TinyJS_Exception, "Trying to assign to an un-named type.");
				}
				lhs = TinyJS_Var_addChild(_this->root, lhs->name, lhs->var);
			}
		}
		{
			int op = _this->lex->tk;
			TinyJS_Lex_match(_this->lex, op);
			{
				ST_TinyJS_VarLink* rhs = TinyJS_base(_this, pExec);
				if(*pExec) {
					switch(op) {
					default:DIE();	//バグ
					case '=':                      TinyJS_VarLink_replaceWith(lhs,                              rhs->var      ); break;
					case TINYJS_LEX_O_PLUSASSIGN:  TinyJS_VarLink_replaceWith(lhs, TinyJS_Var_mathsOp(lhs->var, rhs->var, '+')); break;
					case TINYJS_LEX_O_MINUSASSIGN: TinyJS_VarLink_replaceWith(lhs, TinyJS_Var_mathsOp(lhs->var, rhs->var, '-')); break;
					}
				}
			}
		}
	}
	return lhs;
}
//-----------------------------------------------------------------------------
//「{～}」
void TinyJS_block(ST_TinyJS* _this, int* pExec) {
	TinyJS_Lex_match(_this->lex, '{');
	while(_this->lex->tk && (_this->lex->tk != '}')) {
		TinyJS_statement(_this, pExec);
	}
	TinyJS_Lex_match(_this->lex, '}');
}
//-----------------------------------------------------------------------------
//文
void TinyJS_statement(ST_TinyJS* _this, int* pExec) {
	ST_TinyJS_Var* scope = (ST_TinyJS_Var*)_this->scopes->data;
	if((_this->lex->tk == '-') ||
	   (_this->lex->tk == TINYJS_LEX_ID) ||
	   (_this->lex->tk == TINYJS_LEX_L_NUM) ||
	   (_this->lex->tk == TINYJS_LEX_L_STR)) {
		//Execute a simple statement that only contains basic arithmetic...
		TinyJS_base(_this, pExec);
		TinyJS_Lex_match(_this->lex, ';');
	} else if(_this->lex->tk == '{') {
		//A block of code.
		TinyJS_block(_this, pExec);
	} else if(_this->lex->tk == ';') {
		//Empty statement - to allow things like ;;;.
		TinyJS_Lex_match(_this->lex, ';');
	} else if(_this->lex->tk == TINYJS_LEX_R_VAR) {
		//Variable creation.
		//TODO - We need a better way of parsing the left hand side.
		//       Maybe just have a flag called can_create_var that we set and then we parse as if we're doing a normal equals.
		TinyJS_Lex_match(_this->lex, TINYJS_LEX_R_VAR);
		do {
			ST_TinyJS_VarLink* l = NULL;
			if(*pExec) {
				l = TinyJS_Var_findChildOrCreate(scope, _this->lex->tkStr);
			}
			TinyJS_Lex_match(_this->lex, TINYJS_LEX_ID);
			//Now do stuff defined with dots.
			while(_this->lex->tk == '.') {
				TinyJS_Lex_match(_this->lex, '.');
				if(*pExec) {
					l = TinyJS_Var_findChildOrCreate(l->var, _this->lex->tkStr);
				}
				TinyJS_Lex_match(_this->lex, TINYJS_LEX_ID);
			}
			//Sort out initialiser.
			if(_this->lex->tk == '=') {
				TinyJS_Lex_match(_this->lex, '=');
				{
					ST_TinyJS_VarLink* tmp = TinyJS_base(_this, pExec);
					if(*pExec) {
						TinyJS_VarLink_replaceWith(l, tmp->var);
					}
				}
			}
			if(_this->lex->tk != ';') { TinyJS_Lex_match(_this->lex, ','); }
		} while(_this->lex->tk != ';');
		TinyJS_Lex_match(_this->lex, ';');
	} else if(_this->lex->tk == TINYJS_LEX_R_IF) {
		TinyJS_Lex_match(_this->lex, TINYJS_LEX_R_IF);
		TinyJS_Lex_match(_this->lex, '(');
		{
			ST_TinyJS_VarLink* cond = TinyJS_base(_this, pExec);					//Condition.
			TinyJS_Lex_match(_this->lex, ')');
			TinyJS_statement(_this, TinyJS_Var_getBoolean(cond->var) ? pExec : &noexec);		//If body.
			if(_this->lex->tk == TINYJS_LEX_R_ELSE) {
				TinyJS_Lex_match(_this->lex, TINYJS_LEX_R_ELSE);
				TinyJS_statement(_this, TinyJS_Var_getBoolean(cond->var) ? &noexec : pExec);	//Else body.
			}
		}
	} else if(_this->lex->tk == TINYJS_LEX_R_WHILE) {
		TinyJS_Lex_match(_this->lex, TINYJS_LEX_R_WHILE);
		TinyJS_Lex_match(_this->lex, '(');
		{
			int whileCondStart = _this->lex->tokenStart;
			TinyJS_base(_this, &noexec);		//Condition.
			{
				ST_TinyJS_Lex* whileCond = TinyJS_Lex_getSubLex(_this->lex, whileCondStart);
				TinyJS_Lex_match(_this->lex, ')');
				{
					int whileBodyStart = _this->lex->tokenStart;
					TinyJS_statement(_this, &noexec);	//Body.
					{
						ST_TinyJS_Lex* whileBody = TinyJS_Lex_getSubLex(_this->lex, whileBodyStart);
						ST_TinyJS_Context* pSavedContext = TinyJS_saveContext(_this);
						if(*pExec) {
							for(;;) {
								_this->lex = TinyJS_Lex_reset(whileCond);
								{
									ST_TinyJS_VarLink* cond = TinyJS_base(_this, pExec);	//Condition.
									if(!TinyJS_Var_getBoolean(cond->var)) { break; }
								}
								_this->lex = TinyJS_Lex_reset(whileBody);		//Body.
								TinyJS_statement(_this, pExec);
							}
						}
						TinyJS_restoreContext(_this, pSavedContext);
					}
				}
			}
		}
	} else if(_this->lex->tk == TINYJS_LEX_R_FOR) {
		TinyJS_Lex_match(_this->lex, TINYJS_LEX_R_FOR);
		TinyJS_Lex_match(_this->lex, '(');
		TinyJS_statement(_this, pExec);		//Initialisation.
		{
			int forCondStart = _this->lex->tokenStart;
			TinyJS_base(_this, &noexec);		//Condition.
			{
				ST_TinyJS_Lex* forCond = TinyJS_Lex_getSubLex(_this->lex, forCondStart);
				TinyJS_Lex_match(_this->lex, ';');
				{
					int forIterStart = _this->lex->tokenStart;
					TinyJS_base(_this, &noexec);		//Iterator.
					{
						ST_TinyJS_Lex* forIter = TinyJS_Lex_getSubLex(_this->lex, forIterStart);
						TinyJS_Lex_match(_this->lex, ')');
						{
							int forBodyStart = _this->lex->tokenStart;
							TinyJS_statement(_this, &noexec);	//Body.
							{
								ST_TinyJS_Lex* forBody = TinyJS_Lex_getSubLex(_this->lex, forBodyStart);
								ST_TinyJS_Context* pSavedContext = TinyJS_saveContext(_this);
								if(*pExec) {
									for(;;) {
										_this->lex = TinyJS_Lex_reset(forCond);
										{
											ST_TinyJS_VarLink* cond = TinyJS_base(_this, pExec);	//Condition.
											if(!TinyJS_Var_getBoolean(cond->var)) { break; }
										}
										_this->lex = TinyJS_Lex_reset(forBody);
										TinyJS_statement(_this, pExec);				//Body.
										_this->lex = TinyJS_Lex_reset(forIter);
										TinyJS_base(_this, pExec);				//Iterator.
									}
								}
								TinyJS_restoreContext(_this, pSavedContext);
							}
						}
					}
				}
			}
		}
	} else if(_this->lex->tk == TINYJS_LEX_R_RETURN) {
		TinyJS_Lex_match(_this->lex, TINYJS_LEX_R_RETURN);
		{
			ST_TinyJS_VarLink* result = NULL;
			if(_this->lex->tk != ';') {
				result = TinyJS_base(_this, pExec);
			}
			if(*pExec) {
				if(result) {
					TinyJS_Var_setReturnVar(scope, result->var);
				}
				*pExec = 0;	//return文以降は実行せずにパースだけを行う。
			}
		}
		TinyJS_Lex_match(_this->lex, ';');
	} else if(_this->lex->tk == TINYJS_LEX_R_FUNCTION) {
		ST_TinyJS_VarLink* funcDef = TinyJS_parseFunctionDefinition(_this);
		if(*pExec) {
			if(!strcmp(funcDef->name, "")) {
				SEH_throw_msg(TinyJS_Exception, "Functions defined at statement-level are meant to have a name.");
			}
			TinyJS_Var_addChild(scope, funcDef->name, funcDef->var);
		}
	} else {
		TinyJS_Lex_match(_this->lex, TINYJS_LEX_EOF);
	}
}
//-----------------------------------------------------------------------------
//「function 関数名(仮引数名,...){～}」,又は,「function(仮引数名,...){～}」
ST_TinyJS_VarLink* TinyJS_parseFunctionDefinition(ST_TinyJS* _this) {
	TinyJS_Lex_match(_this->lex, TINYJS_LEX_R_FUNCTION);
	{
		const char* funcName = "";
		if(_this->lex->tk == TINYJS_LEX_ID) {	//We can have functions without names.
			funcName = _this->lex->tkStr;
			TinyJS_Lex_match(_this->lex, TINYJS_LEX_ID);
		}
		{
			ST_TinyJS_Var* funcVar = TinyJS_Var_newFunction();
			TinyJS_parseFunctionArguments(_this, funcVar);
			{
				int funcBegin = _this->lex->tokenStart;
				TinyJS_block(_this, &noexec);
				funcVar->strData = TinyJS_Lex_getSubString(_this->lex, funcBegin);
			}
			{
				ST_TinyJS_VarLink* l = TinyJS_VarLink_new(funcVar);
				l->name = funcName;
				return l;
			}
		}
	}
}
//-----------------------------------------------------------------------------
//「(仮引数名,...)」
void TinyJS_parseFunctionArguments(ST_TinyJS* _this, ST_TinyJS_Var* funcVar) {
	TinyJS_Lex_match(_this->lex, '(');
	while(_this->lex->tk != ')') {
		TinyJS_Var_addChild(funcVar, _this->lex->tkStr, TinyJS_Var_newUndefined());
		TinyJS_Lex_match(_this->lex, TINYJS_LEX_ID);
		if(_this->lex->tk != ')') { TinyJS_Lex_match(_this->lex, ','); }
	}
	TinyJS_Lex_match(_this->lex, ')');
}
//-----------------------------------------------------------------------------
//Finds a child, looking recursively up the scopes.
ST_TinyJS_VarLink* TinyJS_findInScopes(ST_TinyJS* _this, const char* name) {
	GSList/*<ST_TinyJS_Var*>*/* list = _this->scopes;
	while(list) {
		ST_TinyJS_Var* scope = (ST_TinyJS_Var*)list->data;
		ST_TinyJS_VarLink* l = TinyJS_Var_findChild(scope, name);
		if(l) { return l; }
		list = list->next;
	}
	return NULL;
}
//-----------------------------------------------------------------------------
//Look up in any prototype classes of the given object.
ST_TinyJS_VarLink* TinyJS_findInPrototypeClasses(ST_TinyJS* _this, ST_TinyJS_Var* v, const char* name) {
	//Look for links to actual prototype classes.
	ST_TinyJS_VarLink* prototypeClass = TinyJS_Var_findChild(v, "prototype");
	while(prototypeClass) {
		ST_TinyJS_VarLink* implementation = TinyJS_Var_findChild(prototypeClass->var, name);
		if(implementation) { return implementation; }
		prototypeClass = TinyJS_Var_findChild(prototypeClass->var, "prototype");
	}
	//else fake it for strings and finally objects.
	if(TinyJS_Var_isString(v)) {
		ST_TinyJS_VarLink* implementation = TinyJS_Var_findChild(_this->stringClass, name);
		if(implementation) { return implementation; }
	}
	if(TinyJS_Var_isArray(v)) {
		ST_TinyJS_VarLink* implementation = TinyJS_Var_findChild(_this->arrayClass, name);
		if(implementation) { return implementation; }
	}
	return TinyJS_Var_findChild(_this->objectClass, name);	//may be NULL
}
//-----------------------------------------------------------------------------
ST_TinyJS_Context* TinyJS_saveContext(ST_TinyJS* _this) {
	ST_TinyJS_Context* pSavedContext = (ST_TinyJS_Context*)malloc(sizeof(ST_TinyJS_Context));
	pSavedContext->lex       = _this->lex;
	pSavedContext->scopes    = _this->scopes;
	pSavedContext->callStack = _this->callStack;
	return pSavedContext;
}
//-----------------------------------------------------------------------------
void TinyJS_restoreContext(ST_TinyJS* _this, ST_TinyJS_Context* pSavedContext) {
	_this->lex       = pSavedContext->lex;
	_this->scopes    = pSavedContext->scopes;
	_this->callStack = pSavedContext->callStack;
}
//-----------------------------------------------------------------------------
const char* TinyJS_stackTrace(ST_TinyJS* _this, const char* errMsg) {
	GString* buf = g_string_new(NULL);
	g_string_append(buf, "Error ");
	g_string_append(buf, errMsg);
	{
		GSList/*<const char*>*/* list = _this->callStack;
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
	g_string_append(buf, TinyJS_Lex_getLastPosition(_this->lex));
	return buf->str;
}
//*****************************************************************************
//	ST_TinyJS_Lex
//*****************************************************************************
/*static*/ ST_TinyJS_Lex* TinyJS_Lex_new(const char* input, int startChar, int endChar) {
	ST_TinyJS_Lex* _this = (ST_TinyJS_Lex*)malloc(sizeof(ST_TinyJS_Lex));
	_this->data      = input;
	_this->dataStart = startChar;
	_this->dataEnd   = endChar;
	TinyJS_Lex_reset(_this);
	return _this;
}
//-----------------------------------------------------------------------------
//Reset this lex so we can start again.
ST_TinyJS_Lex* TinyJS_Lex_reset(ST_TinyJS_Lex* _this) {
	_this->dataPos      = _this->dataStart;
	_this->tokenStart   = 0;
	_this->tokenEnd     = 0;
	_this->tokenLastEnd = 0;
	_this->tk           = 0;
	_this->tkStr        = "";
	TinyJS_Lex_getNextCh(_this);
	TinyJS_Lex_getNextCh(_this);
	//
	//	～■■□～
	//	　↑↑↑
	//	　││└         dataPos
	//	　│└─ nextCh: dataPos - 1
	//	　└── currCh: dataPos - 2 = dataStart
	//
	TinyJS_Lex_getNextToken(_this);
	return _this;	//利便性のためにthisを返す事にした。
}
//-----------------------------------------------------------------------------
//Lexical match wotsit.
void TinyJS_Lex_match(ST_TinyJS_Lex* _this, int tkExpected) {
	if(_this->tk != tkExpected) {
		GString* errorString = g_string_new(NULL);
		g_string_append(errorString, "Got ");
		g_string_append(errorString, TinyJS_Lex_getTokenStr(_this->tk));
		g_string_append(errorString, " expected ");
		g_string_append(errorString, TinyJS_Lex_getTokenStr(tkExpected));
		g_string_append(errorString, " at ");
		g_string_append(errorString, TinyJS_Lex_getPosition(_this, _this->tokenStart));
		SEH_throw_msg(TinyJS_Exception, errorString->str);
	}
	TinyJS_Lex_getNextToken(_this);
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
	if(      lastPosition > _this->dataEnd) { DIE(); }	//バグ
	if(_this->tokenLastEnd > _this->dataEnd) { DIE(); }	//バグ
	return strndup(_this->data + lastPosition, _this->tokenLastEnd - lastPosition);
}
//-----------------------------------------------------------------------------
//Return a sub-lexer from the given position up until right now.
ST_TinyJS_Lex* TinyJS_Lex_getSubLex(ST_TinyJS_Lex* _this, int lastPosition) {
	if(      lastPosition > _this->dataEnd) { DIE(); }	//バグ
	if(_this->tokenLastEnd > _this->dataEnd) { DIE(); }	//バグ
	return TinyJS_Lex_new(_this->data, lastPosition, _this->tokenLastEnd);
}
//-----------------------------------------------------------------------------
//Return a string representing the position in lines and columns of the tokenLastEnd - 1.
const char* TinyJS_Lex_getLastPosition(ST_TinyJS_Lex* _this) {
	return TinyJS_Lex_getPosition(_this, _this->tokenLastEnd - 1);
}
//-----------------------------------------------------------------------------
//Return a string representing the position in lines and columns of the character pos given.
const char* TinyJS_Lex_getPosition(ST_TinyJS_Lex* _this, int pos) {
	if(pos > _this->dataEnd) { pos = _this->dataEnd; }
	{
		int line = 1, col = 1;
		int i;
		for(i = 0; i < pos; i++) {
			col++;
			if(_this->data[i] == '\n') {
				line++;
				col = 1;
			}
		}
		return strdup_printf("(line: %d, col: %d)", line, col);
	}
}
//-----------------------------------------------------------------------------
void TinyJS_Lex_getNextCh(ST_TinyJS_Lex* _this) {
	_this->currCh = _this->nextCh;
	if(_this->dataPos < _this->dataEnd) {
		_this->nextCh = _this->data[_this->dataPos];
//×		_this->dataPos++;	//←┐
	} else {			//　│
		_this->nextCh = '\0';	//　├もし入力範囲の終端を超えてもdataPosは無条件に進めなければいけない。TinyJS_Lex_reset(),及び,TinyJS_Lex_getNextToken()の処理がgetNextCh()を呼び出した回数分dataPosが進んでいる事を前提としているから。
	}				//　│
/*○*/	_this->dataPos++;		//←┘
}
//-----------------------------------------------------------------------------
//Get the text token from our text string.
void TinyJS_Lex_getNextToken(ST_TinyJS_Lex* _this) {
	for(;;) {
		//Skip whitespaces.
		while(_this->currCh) {
			if(!isspace(_this->currCh)) {
				break;
			}
			TinyJS_Lex_getNextCh(_this);
		}
		//Newline comments.
		if((_this->currCh == '/') && (_this->nextCh == '/')) {
			TinyJS_Lex_getNextCh(_this);
			TinyJS_Lex_getNextCh(_this);
			while(_this->currCh) {
				if(_this->currCh == '\n') {
					TinyJS_Lex_getNextCh(_this);
					break;
				}
				TinyJS_Lex_getNextCh(_this);
			}
		//Block comments.
		} else if((_this->currCh == '/') && (_this->nextCh == '*')) {
			TinyJS_Lex_getNextCh(_this);
			TinyJS_Lex_getNextCh(_this);
			while(_this->currCh) {
				if((_this->currCh == '*') && (_this->nextCh == '/')) {
					TinyJS_Lex_getNextCh(_this);
					TinyJS_Lex_getNextCh(_this);
					break;
				}
				TinyJS_Lex_getNextCh(_this);
			}
		} else {
			break;
		}
	}
	//Record beginning of this token.
	_this->tokenStart = _this->dataPos - 2;
	//
	//	～■■□～
	//	　↑↑↑
	//	　││└         dataPos
	//	　│└─ nextCh: dataPos - 1
	//	　└── currCh: dataPos - 2 = tokenStart
	//
	//Tokens.
//不要	_this->tk    = TINYJS_LEX_EOF;
	_this->tkStr = "";
	{
		GString* buf = g_string_new(NULL);
		//予約語,又は,ID
		if(iscsymf(_this->currCh)) {
			while(iscsym(_this->currCh)) {
				g_string_append_c(buf, _this->currCh);
				TinyJS_Lex_getNextCh(_this);
			}
			     if(!strcmp(buf->str, "break"))     { _this->tk = TINYJS_LEX_R_BREAK; }
			else if(!strcmp(buf->str, "continue"))  { _this->tk = TINYJS_LEX_R_CONTINUE; }
			else if(!strcmp(buf->str, "do"))        { _this->tk = TINYJS_LEX_R_DO; }
			else if(!strcmp(buf->str, "else"))      { _this->tk = TINYJS_LEX_R_ELSE; }
			else if(!strcmp(buf->str, "false"))     { _this->tk = TINYJS_LEX_R_FALSE; }
			else if(!strcmp(buf->str, "for"))       { _this->tk = TINYJS_LEX_R_FOR; }
			else if(!strcmp(buf->str, "function"))  { _this->tk = TINYJS_LEX_R_FUNCTION; }
			else if(!strcmp(buf->str, "if"))        { _this->tk = TINYJS_LEX_R_IF; }
			else if(!strcmp(buf->str, "new"))       { _this->tk = TINYJS_LEX_R_NEW; }
			else if(!strcmp(buf->str, "null"))      { _this->tk = TINYJS_LEX_R_NULL; }
			else if(!strcmp(buf->str, "return"))    { _this->tk = TINYJS_LEX_R_RETURN; }
			else if(!strcmp(buf->str, "true"))      { _this->tk = TINYJS_LEX_R_TRUE; }
			else if(!strcmp(buf->str, "undefined")) { _this->tk = TINYJS_LEX_R_UNDEFINED; }
			else if(!strcmp(buf->str, "var"))       { _this->tk = TINYJS_LEX_R_VAR; }
			else if(!strcmp(buf->str, "while"))     { _this->tk = TINYJS_LEX_R_WHILE; }
			else                                    { _this->tk = TINYJS_LEX_ID; }
		//数値リテラル
		} else if(isdigit(_this->currCh)) {
			int isHex = 0;
			if(_this->currCh == '0') {
				g_string_append_c(buf, _this->currCh);
				TinyJS_Lex_getNextCh(_this);
				if((_this->currCh == 'x') || (_this->currCh == 'X')) {
					isHex = 1;
					g_string_append_c(buf, _this->currCh);
					TinyJS_Lex_getNextCh(_this);
				}
			}
			_this->tk = TINYJS_LEX_L_NUM;
			while(isdigit(_this->currCh) || (isHex && isxdigit(_this->currCh))) {
				g_string_append_c(buf, _this->currCh);
				TinyJS_Lex_getNextCh(_this);
			}
			if(!isHex && (_this->currCh == '.')) {
				g_string_append_c(buf, _this->currCh);
				TinyJS_Lex_getNextCh(_this);
				while(isdigit(_this->currCh)) {
					g_string_append_c(buf, _this->currCh);
					TinyJS_Lex_getNextCh(_this);
				}
			}
			//Do fancy e-style floating point.
			if(!isHex && ((_this->currCh == 'e') || (_this->currCh == 'E'))) {
				g_string_append_c(buf, _this->currCh);
				TinyJS_Lex_getNextCh(_this);
				if(_this->currCh == '-') {
					g_string_append_c(buf, _this->currCh);
					TinyJS_Lex_getNextCh(_this);
				}
				while(isdigit(_this->currCh)) {
					g_string_append_c(buf, _this->currCh);
					TinyJS_Lex_getNextCh(_this);
				}
			}
		//文字列リテラル
		} else if((_this->currCh == '"') || (_this->currCh == '\'')) {
			const int quotCh = _this->currCh;
			_this->tk = TINYJS_LEX_L_STR;
			TinyJS_Lex_getNextCh(_this);
			while(_this->currCh && (_this->currCh != quotCh)) {
				if(_this->currCh == '\\') {
					TinyJS_Lex_getNextCh(_this);
					switch(_this->currCh) {
					case 't':  g_string_append_c(buf, '\t'); break;	//┐
					case 'n':  g_string_append_c(buf, '\n'); break;	//├頻繁に使う制御文字のエスケープシーケンスだけ対応した。頻繁に使わない制御文字はその他の印字不可文字と共通で8進数形式,又は,16進数形式での入力を前提とする。もし不足ならば追加しろ。
					case 'r':  g_string_append_c(buf, '\r'); break;	//┘
					case '\\': g_string_append_c(buf, '\\'); break;
					default:
						//Hex digits.
						if((_this->currCh == 'x') || (_this->currCh == 'X')) {
							GString* tmp = g_string_new(NULL);
							for(;;) {			//C言語の文字列リテラルの仕様では、"\x????～"には桁数制限が無い。JavaScriptでも同じなのかわからない。(※TODO:要確認)
								TinyJS_Lex_getNextCh(_this);
								if(!isxdigit(_this->currCh)) { break; }
								g_string_append_c(tmp, _this->currCh);
							}
							g_string_append_c(buf, (char)strtoul(tmp->str, NULL, 16));
						//Octal digits.
						} else if((_this->currCh >= '0') && (_this->currCh <= '7')) {
							GString* tmp = g_string_new(NULL);
							while(tmp->len < 3) {		//C言語の文字列リテラルの仕様では、"\???"の桁数は1桁～3桁である。JavaScriptでも同じなのかわからない。(※TODO:要確認)
								if(!isxdigit(_this->currCh)) { break; }
								g_string_append_c(tmp, _this->currCh);
								TinyJS_Lex_getNextCh(_this);
							}
							g_string_append_c(buf, (char)strtoul(tmp->str, NULL, 8));
						} else {
							g_string_append_c(buf, _this->currCh);
						}
						break;
					}
				} else {
					g_string_append_c(buf, _this->currCh);
				}
				TinyJS_Lex_getNextCh(_this);
			}
			TinyJS_Lex_getNextCh(_this);
		//演算子,又は,EOF
		} else {
			_this->tk = _this->currCh;
			if(_this->tk) {
				TinyJS_Lex_getNextCh(_this);
				if((_this->tk == '=') && (_this->currCh == '=')) {
					_this->tk = TINYJS_LEX_O_EQUAL;				//"=="
					TinyJS_Lex_getNextCh(_this);
					if(_this->currCh == '=') {
						_this->tk = TINYJS_LEX_O_TYPEEQUAL;		//"==="
						TinyJS_Lex_getNextCh(_this);
					}
				} else if((_this->tk == '!') && (_this->currCh == '=')) {
					_this->tk = TINYJS_LEX_O_NEQUAL;			//"!="
					TinyJS_Lex_getNextCh(_this);
					if(_this->currCh == '=') {
						_this->tk = TINYJS_LEX_O_NTYPEEQUAL;		//"!=="
						TinyJS_Lex_getNextCh(_this);
					}
				} else if((_this->tk == '<') && (_this->currCh == '=')) {
					_this->tk = TINYJS_LEX_O_LEQUAL;			//"<="
					TinyJS_Lex_getNextCh(_this);
				} else if((_this->tk == '<') && (_this->currCh == '<')) {
					_this->tk = TINYJS_LEX_O_LSHIFT;			//"<<"
					TinyJS_Lex_getNextCh(_this);
					if(_this->currCh == '=') {
						_this->tk = TINYJS_LEX_O_LSHIFTASSIGN;		//"<<="
						TinyJS_Lex_getNextCh(_this);
					}
				} else if((_this->tk == '>') && (_this->currCh == '=')) {
					_this->tk = TINYJS_LEX_O_GEQUAL;			//">="
					TinyJS_Lex_getNextCh(_this);
				} else if((_this->tk == '>') && (_this->currCh == '>')) {
					_this->tk = TINYJS_LEX_O_RSHIFT;			//">>"
					TinyJS_Lex_getNextCh(_this);
					if(_this->currCh == '=') {
						_this->tk = TINYJS_LEX_O_RSHIFTASSIGN;		//">>="
						TinyJS_Lex_getNextCh(_this);
					} else if(_this->currCh == '>') {
						_this->tk = TINYJS_LEX_O_RSHIFTUNSIGNED;	//">>>"
						TinyJS_Lex_getNextCh(_this);
					}
				} else if((_this->tk == '+') && (_this->currCh == '=')) {
					_this->tk = TINYJS_LEX_O_PLUSASSIGN;			//"+="
					TinyJS_Lex_getNextCh(_this);
				} else if((_this->tk == '-') && (_this->currCh == '=')) {
					_this->tk = TINYJS_LEX_O_MINUSASSIGN;			//"-="
					TinyJS_Lex_getNextCh(_this);
				} else if((_this->tk == '+') && (_this->currCh == '+')) {
					_this->tk = TINYJS_LEX_O_PLUSPLUS;			//"++"
					TinyJS_Lex_getNextCh(_this);
				} else if((_this->tk == '-') && (_this->currCh == '-')) {
					_this->tk = TINYJS_LEX_O_MINUSMINUS;			//"--"
					TinyJS_Lex_getNextCh(_this);
				} else if((_this->tk == '&') && (_this->currCh == '=')) {
					_this->tk = TINYJS_LEX_O_ANDASSIGN;			//"&="
					TinyJS_Lex_getNextCh(_this);
				} else if((_this->tk == '&') && (_this->currCh == '&')) {
					_this->tk = TINYJS_LEX_O_ANDAND;			//"&&"
					TinyJS_Lex_getNextCh(_this);
				} else if((_this->tk == '|') && (_this->currCh == '=')) {
					_this->tk = TINYJS_LEX_O_OREQUAL;			//"|="
					TinyJS_Lex_getNextCh(_this);
				} else if((_this->tk == '|') && (_this->currCh == '|')) {
					_this->tk = TINYJS_LEX_O_OROR;				//"||"
					TinyJS_Lex_getNextCh(_this);
				} else if((_this->tk == '^') && (_this->currCh == '=')) {
					_this->tk = TINYJS_LEX_O_XORASSIGN;			//"^="
					TinyJS_Lex_getNextCh(_this);
				} else {
					/** no job **/						//その他の一文字演算子,又は,不正な文字
				}
			} else {
				/** no job **/							//EOF
			}
		}
		//Record ending of last token.
		_this->tokenLastEnd = _this->tokenEnd;
		//Record ending of this token.
		_this->tokenEnd = _this->dataPos - 2;
		//
		//	～■■■□□□～
		//	　…┬┘↑↑↑
		//	　　│　││└         dataPos
		//	　　│　│└─ nextCh: dataPos - 1
		//	　　│　└── currCh: dataPos - 2 = tokenEnd
		//	　　└──── tkStr
		//
		_this->tkStr = buf->str;	//忘れないで!!
	}
}
//*****************************************************************************
//	ST_TinyJS_Var
//*****************************************************************************
/*static*/ ST_TinyJS_Var* TinyJS_Var_newUndefined() {
	ST_TinyJS_Var* _this = (ST_TinyJS_Var*)malloc(sizeof(ST_TinyJS_Var));
	TinyJS_Var_setUndefined(_this);
	return _this;
}
//-----------------------------------------------------------------------------
/*static*/ ST_TinyJS_Var* TinyJS_Var_newNull() {
	ST_TinyJS_Var* _this = (ST_TinyJS_Var*)malloc(sizeof(ST_TinyJS_Var));
	TinyJS_Var_setNull(_this);
	return _this;
}
//-----------------------------------------------------------------------------
/*static*/ ST_TinyJS_Var* TinyJS_Var_newNumber(double val) {
	ST_TinyJS_Var* _this = (ST_TinyJS_Var*)malloc(sizeof(ST_TinyJS_Var));
	TinyJS_Var_setNumber(_this, val);
	return _this;
}
//-----------------------------------------------------------------------------
/*static*/ ST_TinyJS_Var* TinyJS_Var_newString(const char* str) {
	ST_TinyJS_Var* _this = (ST_TinyJS_Var*)malloc(sizeof(ST_TinyJS_Var));
	TinyJS_Var_setString(_this, str);
	return _this;
}
//-----------------------------------------------------------------------------
/*static*/ ST_TinyJS_Var* TinyJS_Var_newFunction() {
	ST_TinyJS_Var* _this = (ST_TinyJS_Var*)malloc(sizeof(ST_TinyJS_Var));
	TinyJS_Var_setFunction(_this);
	return _this;
}
//-----------------------------------------------------------------------------
/*static*/ ST_TinyJS_Var* TinyJS_Var_newObject() {
	ST_TinyJS_Var* _this = (ST_TinyJS_Var*)malloc(sizeof(ST_TinyJS_Var));
	TinyJS_Var_setObject(_this);
	return _this;
}
//-----------------------------------------------------------------------------
/*static*/ ST_TinyJS_Var* TinyJS_Var_newArray() {
	ST_TinyJS_Var* _this = (ST_TinyJS_Var*)malloc(sizeof(ST_TinyJS_Var));
	TinyJS_Var_setArray(_this);
	return _this;
}
//-----------------------------------------------------------------------------
/*static*/ ST_TinyJS_Var* TinyJS_Var_newNative(TinyJS_Callback* callback, void* userData) {
	ST_TinyJS_Var* _this = (ST_TinyJS_Var*)malloc(sizeof(ST_TinyJS_Var));
	TinyJS_Var_setNative(_this, callback, userData);
	return _this;
}
//-----------------------------------------------------------------------------
void TinyJS_Var_setUndefined(ST_TinyJS_Var* _this) {
	TinyJS_Var_init(_this, TINYJS_VAR_UNDEFINED);
}
//-----------------------------------------------------------------------------
void TinyJS_Var_setNull(ST_TinyJS_Var* _this) {
	TinyJS_Var_init(_this, TINYJS_VAR_NULL);
}
//-----------------------------------------------------------------------------
void TinyJS_Var_setNumber(ST_TinyJS_Var* _this, double val) {
	TinyJS_Var_init(_this, TINYJS_VAR_NUMBER);
	_this->numData = val;
}
//-----------------------------------------------------------------------------
void TinyJS_Var_setString(ST_TinyJS_Var* _this, const char* str) {
	TinyJS_Var_init(_this, TINYJS_VAR_STRING);
	_this->strData = str;
}
//-----------------------------------------------------------------------------
void TinyJS_Var_setFunction(ST_TinyJS_Var* _this) {
	TinyJS_Var_init(_this, TINYJS_VAR_FUNCTION);
}
//-----------------------------------------------------------------------------
void TinyJS_Var_setObject(ST_TinyJS_Var* _this) {
	TinyJS_Var_init(_this, TINYJS_VAR_OBJECT);
}
//-----------------------------------------------------------------------------
void TinyJS_Var_setArray(ST_TinyJS_Var* _this) {
	TinyJS_Var_init(_this, TINYJS_VAR_ARRAY);
}
//-----------------------------------------------------------------------------
void TinyJS_Var_setNative(ST_TinyJS_Var* _this, TinyJS_Callback* callback, void* userData) {
	TinyJS_Var_init(_this, TINYJS_VAR_NATIVE);
	_this->callback = callback;
	_this->userData = userData;
}
//-----------------------------------------------------------------------------
int TinyJS_Var_isUndefined(ST_TinyJS_Var* _this) {
	return _this->type == TINYJS_VAR_UNDEFINED;
}
//-----------------------------------------------------------------------------
int TinyJS_Var_isNull(ST_TinyJS_Var* _this) {
	return _this->type == TINYJS_VAR_NULL;
}
//-----------------------------------------------------------------------------
int TinyJS_Var_isNumber(ST_TinyJS_Var* _this) {
	return _this->type == TINYJS_VAR_NUMBER;
}
//-----------------------------------------------------------------------------
int TinyJS_Var_isString(ST_TinyJS_Var* _this) {
	return _this->type == TINYJS_VAR_STRING;
}
//-----------------------------------------------------------------------------
int TinyJS_Var_isFunction(ST_TinyJS_Var* _this) {
	return (_this->type == TINYJS_VAR_FUNCTION) || (_this->type == TINYJS_VAR_NATIVE);
}
//-----------------------------------------------------------------------------
int TinyJS_Var_isObject(ST_TinyJS_Var* _this) {
	return _this->type == TINYJS_VAR_OBJECT;
}
//-----------------------------------------------------------------------------
int TinyJS_Var_isArray(ST_TinyJS_Var* _this) {
	return _this->type == TINYJS_VAR_ARRAY;
}
//-----------------------------------------------------------------------------
int TinyJS_Var_isNative(ST_TinyJS_Var* _this) {
	return _this->type == TINYJS_VAR_NATIVE;
}
//-----------------------------------------------------------------------------
int TinyJS_Var_isPrimitive(ST_TinyJS_Var* _this) {
	return (_this->type == TINYJS_VAR_UNDEFINED) ||
	       (_this->type == TINYJS_VAR_NULL) ||
	       (_this->type == TINYJS_VAR_NUMBER) ||
	       (_this->type == TINYJS_VAR_STRING);
}
//-----------------------------------------------------------------------------
//If this is a function, get the result value (for use by native functions).
ST_TinyJS_Var* TinyJS_Var_getReturnVar(ST_TinyJS_Var* _this) {
	ST_TinyJS_VarLink* resultVarLink = TinyJS_Var_findChild(_this, "return");
	if(!resultVarLink) {
		SEH_throw_msg(TinyJS_Exception, "TinyJS_Var_getReturnVar() has been called outside a function.");
	}
	return resultVarLink->var;
}
//-----------------------------------------------------------------------------
//Set the result value.
//Use this when setting complex return data as it avoids a TinyJS_Var_deepCopy().
void TinyJS_Var_setReturnVar(ST_TinyJS_Var* _this, ST_TinyJS_Var* v) {
	ST_TinyJS_VarLink* resultVarLink = TinyJS_Var_findChild(_this, "return");
	if(!resultVarLink) {
		SEH_throw_msg(TinyJS_Exception, "TinyJS_Var_setReturnVar() has been called outside a function.");
	}
	TinyJS_VarLink_replaceWith(resultVarLink, v);
}
//-----------------------------------------------------------------------------
//If this is a function, get the parameter with the given name (for use by native functions).
ST_TinyJS_Var* TinyJS_Var_getParameter(ST_TinyJS_Var* _this, const char* name) {
	return TinyJS_Var_findChildOrCreate(_this, name)->var;
}
//-----------------------------------------------------------------------------
//Tries to find a child with the given name, may return NULL.
ST_TinyJS_VarLink* TinyJS_Var_findChild(ST_TinyJS_Var* _this, const char* name) {
	GSList/*<ST_TinyJS_VarLink*>*/* list = _this->firstChild;
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
	ST_TinyJS_VarLink* l = TinyJS_Var_findChild(_this, name);
	if(l) { return l; }
	return TinyJS_Var_addChild(_this, name, TinyJS_Var_newUndefined());
}
//-----------------------------------------------------------------------------
ST_TinyJS_VarLink* TinyJS_Var_addChild(ST_TinyJS_Var* _this, const char* name, ST_TinyJS_Var* v) {
	if(TinyJS_Var_isUndefined(_this)) { _this->type = TINYJS_VAR_OBJECT; }	//※なぜ？
	if(TinyJS_Var_findChild(_this, name)) {
		SEH_throw_msg(TinyJS_Exception, strdup_printf("'%s' already exists.", name));
	}
	{
		ST_TinyJS_VarLink* l = TinyJS_VarLink_new(v);
		l->name  = name;
		l->owned = 1;
		_this->firstChild = g_slist_append(_this->firstChild, l);
		return l;
	}
}
//-----------------------------------------------------------------------------
//Remove a specific link (this is faster than finding via a child).
void TinyJS_Var_removeLink(ST_TinyJS_Var* _this, ST_TinyJS_VarLink* l) {
	_this->firstChild = g_slist_remove(_this->firstChild, l);
}
//-----------------------------------------------------------------------------
//The the value at an array index.
ST_TinyJS_Var* TinyJS_Var_getArrayIndex(ST_TinyJS_Var* _this, int i) {
	const char* buf = strdup_printf("%d", i);
	ST_TinyJS_VarLink* l = TinyJS_Var_findChild(_this, buf);
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
	ST_TinyJS_VarLink* l = TinyJS_Var_findChild(_this, buf);
	if(l) {
		if(TinyJS_Var_isUndefined(v)) {
			TinyJS_Var_removeLink(_this, l);	//※本当はundefinedを代入しても配列要素は削除されないはずだが…当インタプリタにはdelete命令が無いようなのでその代用なのだろう。
		} else {
			TinyJS_VarLink_replaceWith(l, v);
		}
	} else {
		if(TinyJS_Var_isUndefined(v)) {
			/** no job **/	//※本当はundefinedを明示的に新規代入した場合も配列要素が生成されるはずだが…
		} else {
			TinyJS_Var_addChild(_this, buf, v);
		}
	}
}
//-----------------------------------------------------------------------------
//If this is an array, return the number of items in it (else 0).
int TinyJS_Var_getArrayLength(ST_TinyJS_Var* _this) {
	if(!TinyJS_Var_isArray(_this)) { return 0; }
	{
		int ubound = -1;
		GSList/*<ST_TinyJS_VarLink*>*/* list = _this->firstChild;
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
}
//-----------------------------------------------------------------------------
int TinyJS_Var_getBoolean(ST_TinyJS_Var* _this) {
	return TinyJS_Var_getNumber(_this) != 0;	//※色々問題が有る気がする。getInt()がオブジェクトや文字列に対して0を返し、TinyJS_Var_getBoolean()でfalseになってしまう。せめてオブジェクトや空でない文字列はtrueと判断すべきではないか?(※TODO:要検討)
}
//-----------------------------------------------------------------------------
double TinyJS_Var_getNumber(ST_TinyJS_Var* _this) {
	if(TinyJS_Var_isNumber(_this)) { return _this->numData; }
//不要	if(TinyJS_Var_isNull(_this)) { return 0; }
//不要	if(TinyJS_Var_isUndefined(_this)) { return 0; }	//※本当はNumber(undefined)⇒NaNだが…
	return 0;	//※本当は0とは限らないが…せめて文字列⇒数値変換ぐらいは実装すべきではないか?(※TODO:)
}
//-----------------------------------------------------------------------------
const char* TinyJS_Var_getString(ST_TinyJS_Var* _this) {
	if(TinyJS_Var_isNumber(_this)) { return strdup_printf("%g", _this->numData); }
	if(TinyJS_Var_isNull(_this)) { return "null"; }
	if(TinyJS_Var_isUndefined(_this)) { return "undefined"; }
	return _this->strData;	//※オブジェクトや配列の文字列化は未対応なのか?(※TODO:要検討)
}
//-----------------------------------------------------------------------------
//Get data as a parsable javascript string.
const char* TinyJS_Var_getParsableString(ST_TinyJS_Var* _this) {
	//Numbers can just be put in directly.
	if(TinyJS_Var_isNumber(_this)) { return TinyJS_Var_getString(_this); }
	if(TinyJS_Var_isFunction(_this)) {
		GString* funcStr = g_string_new("function (");
		//Get list of parameters.
		GSList/*<ST_TinyJS_VarLink*>*/* list = _this->firstChild;
		while(list) {
			ST_TinyJS_VarLink* l = (ST_TinyJS_VarLink*)list->data;
			g_string_append(funcStr, l->name);
			if(list->next) { g_string_append(funcStr, ","); }
			list = list->next;
		}
		//Add function body.
		g_string_append(funcStr, ") ");
		g_string_append(funcStr, TinyJS_Var_getString(_this));
		return funcStr->str;
	}
	//If it is a string then we quote it.
	if(TinyJS_Var_isString(_this)) { return getJSString(TinyJS_Var_getString(_this)); }
	if(TinyJS_Var_isNull(_this)) { return "null"; }
	return "undefined";	//※TODO:オブジェクトや配列も間違って"undefined"と表示されてしまうのではないか?
}
//-----------------------------------------------------------------------------
int TinyJS_Var_equals(ST_TinyJS_Var* _this, ST_TinyJS_Var* v) {
	ST_TinyJS_Var* tmp = TinyJS_Var_mathsOp(_this, v, TINYJS_LEX_O_EQUAL);
	return TinyJS_Var_getBoolean(tmp);
}
//-----------------------------------------------------------------------------
//Do a maths op with another script variable.
//※当関数の動作は本物のJavaScriptとだいぶん違う。簡略化のためにやむをえないとは思うが、もう少し本物のJavaScriptに近付けられないだろうか？(※TODO:要検討)
ST_TinyJS_Var* TinyJS_Var_mathsOp(ST_TinyJS_Var* _this, ST_TinyJS_Var* v, int op) {
	ST_TinyJS_Var* const a = _this;	//┬読み易いように変数名を揃えているだけ。必須ではない。
	ST_TinyJS_Var* const b = v;	//┘
	//Type equality check.
	if((op == TINYJS_LEX_O_TYPEEQUAL) || (op == TINYJS_LEX_O_NTYPEEQUAL)) {
		//Check type first, then call again to check data.
		int eql = (a->type == b->type);
		if(eql) {
			ST_TinyJS_Var* tmp = TinyJS_Var_mathsOp(a, b, TINYJS_LEX_O_EQUAL);
			if(!TinyJS_Var_getBoolean(tmp)) { eql = 0; }
		}
		if(op == TINYJS_LEX_O_NTYPEEQUAL) { eql = !eql; }
		return TinyJS_Var_newNumber(eql);
	}
	//Do maths...
	if((TinyJS_Var_isUndefined(a) || TinyJS_Var_isNull(a)) && (TinyJS_Var_isUndefined(b) || TinyJS_Var_isNull(b))) {
		if(op == TINYJS_LEX_O_EQUAL) {
			return TinyJS_Var_newNumber(1);	//(undefined==null)もtrueです。(本物のJavaScriptでも)
		} else if(op == TINYJS_LEX_O_NEQUAL) {
			return TinyJS_Var_newNumber(0);	//(undefined!=null)もfalseです。(本物のJavaScriptでも)
		} else {
			return TinyJS_Var_newUndefined();	//undefined	※本当は違う。(undefined+null)⇒NaNや(undefined|null)⇒0など。とは言え実際にそれらの挙動に依存するようなスクリプトは書かないと思う。(※TODO:要検討)
		}
	} else if(TinyJS_Var_isNumber(a) && TinyJS_Var_isNumber(b)) {
		double da = TinyJS_Var_getNumber(a);
		double db = TinyJS_Var_getNumber(b);
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
	} else if(TinyJS_Var_isArray(a)) {
		//Just check pointers.
		switch(op) {
		case TINYJS_LEX_O_EQUAL:  return TinyJS_Var_newNumber(a == b);
		case TINYJS_LEX_O_NEQUAL: return TinyJS_Var_newNumber(a != b);
		default:SEH_throw_msg(TinyJS_Exception, strdup_printf("Operation %s not supported on the Array datatype.", TinyJS_Lex_getTokenStr(op)));
		}
	} else if(TinyJS_Var_isObject(a)) {
		//Just check pointers.
		switch(op) {
		case TINYJS_LEX_O_EQUAL:  return TinyJS_Var_newNumber(a == b);
		case TINYJS_LEX_O_NEQUAL: return TinyJS_Var_newNumber(a != b);
		default:SEH_throw_msg(TinyJS_Exception, strdup_printf("Operation %s not supported on the Object datatype.", TinyJS_Lex_getTokenStr(op)));
		}
	} else {
		const char* da = TinyJS_Var_getString(a);
		const char* db = TinyJS_Var_getString(b);
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
	v->type    = _this->type;
	v->numData = _this->numData;
	v->strData = _this->strData;
	{
		//Copy children.
		GSList/*<ST_TinyJS_VarLink*>*/* list = _this->firstChild;
		while(list) {
			ST_TinyJS_VarLink* l = (ST_TinyJS_VarLink*)list->data;
			ST_TinyJS_Var* tmp;
			//Don't copy the 'prototype' object...
			if(!strcmp(l->name, "prototype")) {	//"prototype"ならば…
				tmp = l->var;
			} else {				//"prototype"以外ならば…
				tmp = TinyJS_Var_deepCopy(l->var);
			}
			TinyJS_Var_addChild(v, l->name, tmp);
			list = list->next;
		}
	}
	return v;
}
//-----------------------------------------------------------------------------
//Dump out the contents of this using trace.
void TinyJS_Var_trace(ST_TinyJS_Var* _this, const char* indent, const char* name) {
	printf("%s'%s' = '%s' %s\n",
		indent,
		name,
		TinyJS_Var_getString(_this),
		TinyJS_Var_getTypeAsString(_this));
	{
		const char* indentStr = strconcat(indent, " ", NULL);
		GSList/*<ST_TinyJS_VarLink*>*/* list = _this->firstChild;
		while(list) {
			ST_TinyJS_VarLink* l = (ST_TinyJS_VarLink*)list->data;
			TinyJS_Var_trace(l->var, indentStr, l->name);
			list = list->next;
		}
	}
}
//-----------------------------------------------------------------------------
//For debugging - just dump a string version of the type.
const char* TinyJS_Var_getTypeAsString(ST_TinyJS_Var* _this) {
	if(_this->type == TINYJS_VAR_NUMBER)   { return "NUMBER"; }
	if(_this->type == TINYJS_VAR_STRING)   { return "STRING"; }
	if(_this->type == TINYJS_VAR_FUNCTION) { return "FUNCTION"; }
	if(_this->type == TINYJS_VAR_OBJECT)   { return "OBJECT"; }
	if(_this->type == TINYJS_VAR_ARRAY)    { return "ARRAY"; }
	if(_this->type == TINYJS_VAR_NATIVE)   { return "NATIVE"; }
	return "";
}
//-----------------------------------------------------------------------------
//Write out all the JS code needed to recreate this script variable to the stream (as JSON).
const char* TinyJS_Var_getJSON(ST_TinyJS_Var* _this, const char* linePrefix) {
	GString* indentedLinePrefix = g_string_new(linePrefix);
	g_string_append(indentedLinePrefix, "  ");
	{
		GString* destination = g_string_new(NULL);
		if(TinyJS_Var_isObject(_this)) {
			//Children - handle with bracketed list.
			g_string_append(destination, "{\n");
			{
				GSList/*<ST_TinyJS_VarLink*>*/* list = _this->firstChild;
				while(list) {
					ST_TinyJS_VarLink* l = (ST_TinyJS_VarLink*)list->data;
					g_string_append(destination, indentedLinePrefix->str);
					g_string_append(destination, getJSString(l->name));
					g_string_append(destination, ": ");
					g_string_append(destination, TinyJS_Var_getJSON(l->var, indentedLinePrefix->str));
					if(list->next) { g_string_append(destination, ","); }
					g_string_append(destination, "\n");
					list = list->next;
				}
			}
			g_string_append(destination, linePrefix);
			g_string_append(destination, "}");
		} else if(TinyJS_Var_isArray(_this)) {
			g_string_append(destination, "[\n");
			{
				int len = TinyJS_Var_getArrayLength(_this);
				if(len > 10000) { len = 10000; }	//We don't want to get stuck here!
				{
					int i;
					for(i = 0; i < len; i++) {
						g_string_append(destination, TinyJS_Var_getJSON(TinyJS_Var_getArrayIndex(_this, i), indentedLinePrefix->str));
						if(i < len - 1) { g_string_append(destination, ","); }
						g_string_append(destination, "\n");
					}
				}
			}
			g_string_append(destination, linePrefix);
			g_string_append(destination, "]");
		} else {
			//No children or a function... just write value directly.
			g_string_append(destination, TinyJS_Var_getParsableString(_this));
		}
		return destination->str;
	}
}
//-----------------------------------------------------------------------------
void TinyJS_Var_init(ST_TinyJS_Var* _this, int varType) {
	_this->type       = varType;
	_this->numData    = 0;
	_this->strData    = "";
	_this->callback   = NULL;
	_this->userData   = NULL;
	_this->firstChild = NULL;
}
//*****************************************************************************
//	ST_TinyJS_VarLink
//*****************************************************************************
/*static*/ ST_TinyJS_VarLink* TinyJS_VarLink_new(ST_TinyJS_Var* v) {
	ST_TinyJS_VarLink* _this = (ST_TinyJS_VarLink*)malloc(sizeof(ST_TinyJS_VarLink));
	_this->name  = "";
	_this->var   = v;
	_this->owned = 0;
	return _this;
}
//-----------------------------------------------------------------------------
//Replace the Variable pointed to.
void TinyJS_VarLink_replaceWith(ST_TinyJS_VarLink* _this, ST_TinyJS_Var* v) {
	if(!v) { DIE(); }	//バグ
	_this->var = v;
}
//-----------------------------------------------------------------------------
//Get the name as an integer (for arrays).
int TinyJS_VarLink_getIntName(ST_TinyJS_VarLink* _this) {
	return atoi(_this->name);
}
//-----------------------------------------------------------------------------
//Set the name as an integer (for arrays).
void TinyJS_VarLink_setIntName(ST_TinyJS_VarLink* _this, int n) {
	const char* buf = strdup_printf("%d", n);
	_this->name = buf;
}
