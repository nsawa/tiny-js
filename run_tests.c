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
//=============================================================================
//
//	This is a program to run all the tests in the tests folder...
//
//=============================================================================
#include "clip.h"
//=============================================================================
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
static int run_test(ST_TinyJS* tinyJS, const char* fileName) {
	//スクリプトファイル名を表示する。
	printf("TEST %s ", fileName);
	{
		//スクリプトファイルのサイズを取得する。
		struct stat st;
		if(stat(fileName, &st)) {
			printf("Cannot stat file! '%s'\n", fileName);
			return 0;	//失敗(0)
		}
		{
			int size = st.st_size;
			//スクリプトファイルを読み込む。
			FILE* fp = fopen(fileName, "rb");	//If we open as text, the number of bytes read may be > the size we read.
			if(!fp) {
				printf("Unable to open file! '%s'\n", fileName);
				return 0;	//失敗(0)
			}
			{
				char* buffer = calloc(size + 1/*nul*/, 1);
				fread(buffer, 1, size, fp);
				fclose(fp);
				//グローバルオブジェクトに、テスト結果の初期値(0:失敗)を登録する。
			//不要	TinyJS_Var_addChild(tinyJS->root, "result", TinyJS_Var_newNumber(0));	⇒不要。undefinedに対するTinyJS_Var_getBoolean()は0を返すので、明示的にresult=0を登録しておいても未定義でも同じだから。
				//スクリプトを実行する。
				SEH_try {
					TinyJS_execute(tinyJS, buffer);
				} SEH_catch(TinyJS_Exception) {
					printf("ERROR: %s\n", SEH_info.msg);
				} SEH_end
				{
					//テスト結果を取得する。
					int pass = TinyJS_Var_getBoolean(TinyJS_Var_getParameter(tinyJS->root, "result"));
					if(pass) {
						printf("PASS\n");
					} else {
						//失敗ならば、ログファイルを生成する。
						const char* buf = strdup_printf("%s.fail.js", fileName);
						FILE* f = fopen(buf, "wt");
						if(f) {
							const char* symbols = TinyJS_Var_getJSON(tinyJS->root, "");
							fprintf(f, "%s", symbols);
							fclose(f);
						}
						printf("FAIL - symbols written to %s\n", buf);
					}
					//テスト結果を返す。
					return pass;
				}
			}
		}
	}
}
//-----------------------------------------------------------------------------
int main(int argc, char** argv) {
	int result;
	//インタプリタを作成する。
	ST_TinyJS* tinyJS = TinyJS_new();
	//関数を登録する。
	TinyJS_registerFunctions(tinyJS);
	TinyJS_registerMathFunctions(tinyJS);
	TinyJS_addNative(tinyJS, "function print(str)", js_print, NULL);
	TinyJS_addNative(tinyJS, "function dump()",     js_dump,  NULL);
	//引数が指定されていなければ…
	if(argc == (1 + 0)) {
		//グローバルオブジェクトを退避する。
		ST_TinyJS_Var* root = TinyJS_Var_deepCopy(tinyJS->root);
		//テスト番号1から、最大でも999まで…
		int test_num, count = 0, passed = 0;	//test_num:テスト番号,count=テスト実行数,passed=テスト成功数
		for(test_num = 1; test_num <= 999; test_num++) {
			//テスト番号に対応する、スクリプトファイル名を作成する。
			const char* buf = strdup_printf("tests/test%03d.js", test_num);
			//スクリプトファイルが存在しなければ、テストを終了する。
			struct stat st;
			if(stat(buf, &st)) { break; }	//Check if the file exists - if not, assume we're at the end of our tests.
			//グローバルオブジェクトを複製し、今回の実行環境とする。
			tinyJS->root = TinyJS_Var_deepCopy(root);
			//テストを実行し、テスト成功ならばテスト成功数を増やす。
			if(run_test(tinyJS, buf)) { passed++; }
			count++;	//テスト実行数を増やす。
		}
		//テスト実行数、テスト成功数、テスト失敗数を表示する。
		printf("Done. %d tests, %d pass, %d fail\n", count, passed, count - passed);
		result = !(count == passed);	//テスト失敗数が0ならば、正常終了(0)とする。
	//引数が一個指定されていたら…
	} else if(argc == (1 + 1)) {
		//指定されたスクリプトを実行する。
		int passed = run_test(tinyJS, argv[1]);
		result = !passed;	//テスト成功(1)ならば、正常終了(0)とする。
	//引数が二個以上指定されていたら…
	} else {
		//使い方を表示する。
		printf("TinyJS test runner\n");
		printf("USAGE:\n");
		printf("    ./run_tests            : run all tests\n");
		printf("    ./run_tests test.js    : run just one test\n");
		result = EXIT_FAILURE;	//異常終了(1)とする。
	}
	//現在のプロセスがデバッガのコンテキストで実行されてたら…
	if(IsDebuggerPresent()) {
		//標準出力がリダイレクトされていなければ…
		if(_isatty(fileno(stdout))) {
			//キー入力を待つ。(デバッガ環境でDOS窓がすぐに閉じて結果が確認しづらい問題を回避するためです。)
			printf("続行するには何かキーを押してください . . .");	//PAUSEコマンドと同じメッセージにした。
			_getch();
		}
	}
	return result;
}
