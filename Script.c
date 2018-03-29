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
#include "app.h"
//=============================================================================
//static const char code[] = "var a=5; if(a==5) a=4; else a=3;";
//static const char code[] = "{ var a=4; var b=1; while(a>0) { b=b*2; a=a-1; } var c=5; }";
//static const char code[] = "{ var b=1; for(var i=0; i<4; i=i+1) b=b*2; }";
//static const char code[] = "function myfunc(x,y) { return x+y; } var a=myfunc(1,2); print(a);";
//-----------------------------------------------------------------------------
//function print(str: string): void
static void js_print(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	printf("> %s\n", TinyJS_Var_getString(TinyJS_Var_getParameter(funcRoot, "str")));
}
//-----------------------------------------------------------------------------
//function dump(): void
static void js_dump(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	TinyJS_trace(tinyJS, "");
}
//-----------------------------------------------------------------------------
int main(int argc, char** argv) {
	//インタプリタを作成する。
	ST_TinyJS* tinyJS = TinyJS_new();
	//関数を登録する。
	TinyJS_registerFunctions(tinyJS);
	TinyJS_registerMathFunctions(tinyJS);
	TinyJS_addNative(tinyJS, "function print(str)", js_print, NULL);
	TinyJS_addNative(tinyJS, "function dump()",     js_dump,  NULL);
	//Execute out bit of code - we could call 'evaluate' here if we wanted something returned.
	SEH_try {
		TinyJS_execute(tinyJS, "var lets_quit = 0; function quit() { lets_quit = 1; }");
		TinyJS_execute(tinyJS, "print(\"Interactive mode... Type quit(); to exit, or print(...); to print something, or dump() to dump the symbol table!\");");
	} SEH_catch(TinyJS_Exception) {
		printf("ERROR: %s\n", SEH_info.msg);
	} SEH_end
	//quit()関数が呼び出されるまで…
	while(!TinyJS_Var_getNumber(TinyJS_evaluate(tinyJS, "lets_quit"))) {
		//スクリプトを一行読み込む。
		char buffer[2048];
		if(!fgets(buffer, sizeof(buffer), stdin)) { break; }
		//スクリプトを一行実行する。
		SEH_try{
			TinyJS_execute(tinyJS, buffer);
		} SEH_catch(TinyJS_Exception) {
			printf("ERROR: %s\n", SEH_info.msg);
		} SEH_end
	}
	return 0;	//もし途中でエラーが発生していても、当プログラムは常に正常終了(0)を返す。
}
