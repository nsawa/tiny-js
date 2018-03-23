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
//	This is a program to run all the tests in the tests folder...
//
//=============================================================================
#include "TinyJS.h"
#include "TinyJS_Functions.h"
#include "TinyJS_MathFunctions.h"
#include <io.h>		//_isatty()
#include <conio.h>	//_getch()
//=============================================================================
//function print(str: string): void
static void js_print(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	printf("> %s\n", funcRoot->getParameter("str")->getString());
}
//-----------------------------------------------------------------------------
//function dump(): void
static void js_dump(ST_TinyJS* tinyJS, ST_TinyJS_Var* funcRoot, void* userData) {
	tinyJS->trace("");
}
//-----------------------------------------------------------------------------
static int run_test(const char* fileName) {
	//スクリプトファイル名を表示する。
	printf("TEST %s ", fileName);
	//スクリプトファイルのサイズを取得する。
	struct stat st;
	if(stat(fileName, &st)) {
		printf("Cannot stat file! '%s'\n", fileName);
		return 0;	//失敗(0)
	}
	int size = st.st_size;
	//スクリプトファイルを読み込む。
	FILE* fp = fopen(fileName, "rb");	//If we open as text, the number of bytes read may be > the size we read.
	if(!fp) {
		printf("Unable to open file! '%s'\n", fileName);
		return 0;	//失敗(0)
	}
	char* buffer = (char*)calloc(size + 1/*nul*/, 1);
	fread(buffer, 1, size, fp);
	fclose(fp);
	//インタプリタを作成する。
	ST_TinyJS* tinyJS = new ST_TinyJS();
	//関数を登録する。
	registerFunctions(tinyJS);
	registerMathFunctions(tinyJS);
	tinyJS->addNative("function print(str)", js_print, NULL);
	tinyJS->addNative("function dump()",     js_dump,  NULL);
	//グローバルオブジェクトに、テスト結果の初期値(0:失敗)を登録する。
	tinyJS->root->addChild("result", ST_TinyJS_Var::newNumber(0));
	//スクリプトを実行する。
	try {
		tinyJS->execute(buffer);
	} catch(ST_TinyJS_Exception* e) {
		printf("ERROR: %s\n", e->msg);
	}
	//テスト結果を取得する。
	int pass = tinyJS->root->getParameter("result")->getBoolean();
	if(pass) {
		printf("PASS\n");
	} else {
		//失敗ならば、ログファイルを生成する。
		const char* buf = strdup_printf("%s.fail.js", fileName);
		FILE* f = fopen(buf, "wt");
		if(f) {
			const char* symbols = tinyJS->root->getJSON("");
			fprintf(f, "%s", symbols);
			fclose(f);
		}
		printf("FAIL - symbols written to %s\n", buf);
	}
	//テスト結果を返す。
	return pass;
}
//-----------------------------------------------------------------------------
int main(int argc, char** argv) {
	int result;
//{{削除
//	//メモリリーク検出を開始する。
//	putenv("GC_LOG_FILE=CON");
//	GC_set_find_leak(1);
//}}削除
	//引数が指定されていなければ…
	if(argc == (1 + 0)) {
		//テスト番号1から、最大でも999まで…
		int test_num, count = 0, passed = 0;	//test_num:テスト番号,count=テスト実行数,passed=テスト成功数
		for(test_num = 1; test_num <= 999; test_num++) {
			//テスト番号に対応する、スクリプトファイル名を作成する。
			const char* buf = strdup_printf("tests/test%03d.js", test_num);
			//スクリプトファイルが存在しなければ、テストを終了する。
			struct stat st;
			if(stat(buf, &st)) { break; }	//Check if the file exists - if not, assume we're at the end of our tests.
			//テストを実行し、テスト成功ならばテスト成功数を増やす。
			if(run_test(buf)) { passed++; }
			count++;	//テスト実行数を増やす。
//{{削除
//			//メモリリークを検出する。
//			CHECK_LEAKS();
//}}削除
		}
		//テスト実行数、テスト成功数、テスト失敗数を表示する。
		printf("Done. %d tests, %d pass, %d fail\n", count, passed, count - passed);
		result = !(count == passed);	//テスト失敗数が0ならば、正常終了(0)とする。
	//引数が一個指定されていたら…
	} else if(argc == (1 + 1)) {
		//指定されたスクリプトを実行する。
		int passed = run_test(argv[1]);
//{{削除
//		//メモリリークを検出する。
//		CHECK_LEAKS();
//}}削除
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
