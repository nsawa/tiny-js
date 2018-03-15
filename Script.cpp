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
//=============================================================================
//
//	This is a simple program showing how to use TinyJS.
//
//=============================================================================
#include "TinyJS.h"
#include "TinyJS_Functions.h"
#include "TinyJS_MathFunctions.h"
#include <assert.h>
#include <stdio.h>
//=============================================================================
//static const char code[] = "var a=5; if(a==5) a=4; else a=3;";
//static const char code[] = "{ var a=4; var b=1; while(a>0) { b=b*2; a=a-1; } var c=5; }";
//static const char code[] = "{ var b=1; for(var i=0; i<4; i=i+1) b=b*2; }";
//static const char code[] = "function myfunc(x,y) { return x+y; } var a=myfunc(1,2); print(a);";
//-----------------------------------------------------------------------------
//function print(str: string): void
static void js_print(CTinyJS* tinyJS, CScriptVar* v, void* userdata) {
	printf("> %s\n", v->getParameter("str")->getString().c_str());
}
//-----------------------------------------------------------------------------
//function dump(): void
static void js_dump(CTinyJS* tinyJS, CScriptVar* v, void* userdata) {
	tinyJS->trace();
}
//-----------------------------------------------------------------------------
int main(int argc, char** argv) {
	//インタプリタを作成する。
	CTinyJS* tinyJS = new CTinyJS();
	//関数を登録する。
	registerFunctions(tinyJS);
	registerMathFunctions(tinyJS);
	tinyJS->addNative("function print(str)", js_print, NULL);
	tinyJS->addNative("function dump()",     js_dump,  NULL);
	//Execute out bit of code - we could call 'evaluate' here if we wanted something returned.
	try {
		tinyJS->execute("var lets_quit = 0; function quit() { lets_quit = 1; }");
		tinyJS->execute("print(\"Interactive mode... Type quit(); to exit, or print(...); to print something, or dump() to dump the symbol table!\");");
	} catch(CScriptException* e) {
		printf("ERROR: %s\n", e->text.c_str());
	}
	//quit()関数が呼び出されるまで…
	while(tinyJS->evaluate("lets_quit") == "0") {
		//スクリプトを一行読み込む。
		char buffer[2048];
		if(!fgets(buffer, sizeof(buffer), stdin)) { break; }
		//スクリプトを一行実行する。
		try {
			tinyJS->execute(buffer);
		} catch(CScriptException* e) {
			printf("ERROR: %s\n", e->text.c_str());
		}
	}
	//インタプリタを削除する。
	delete tinyJS;
	return 0;	//もし途中でエラーが発生していても、当プログラムは常に正常終了(0)を返す。
}
