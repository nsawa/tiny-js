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
//	This is a program to run all the tests in the tests folder...
//
//-----------------------------------------------------------------------------
#include "TinyJS.h"
#include "TinyJS_Functions.h"
#include "TinyJS_MathFunctions.h"
//-----------------------------------------------------------------------------
static bool run_test(const char* filename) {
	printf("TEST %s ", filename);
	struct stat st;
	if(stat(filename, &st)) {
		printf("Cannot stat file! '%s'\n", filename);
		return false;
	}
	int size = st.st_size;
	FILE* file = fopen(filename, "rb");	//If we open as text, the number of bytes read may be > the size we read.
	if(!file) {
		printf("Unable to open file! '%s'\n", filename);
		return false;
	}
	char* buffer = new (GC) char[size + 1];
	int actualRead = (int)fread(buffer, 1, size, file);
	buffer[actualRead] = 0;
	fclose(file);

	CTinyJS* tinyJS = new CTinyJS();
	registerFunctions(tinyJS);
	registerMathFunctions(tinyJS);
	tinyJS->root->addChild("result", new CScriptVar("0", TINYJS_SCRIPTVAR_INTEGER));
	try {
		tinyJS->execute(buffer);
	} catch(CScriptException* e) {
		printf("ERROR: %s\n", e->text.c_str());
	}
	bool pass = tinyJS->root->getParameter("result")->getBool();
	if(pass) {
		printf("PASS\n");
	} else {
		char path[64];
		sprintf(path, "%s.fail.js", filename);
		FILE* f = fopen(path, "wt");
		if(f) {
			std::ostringstream symbols;
			tinyJS->root->getJSON(symbols);
			fprintf(f, "%s", symbols.str().c_str());
			fclose(f);
		}
		printf("FAIL - symbols written to %s\n", path);
	}
	delete tinyJS;

	delete[] buffer;
	return pass;
}
int main(int argc, char** argv) {
	putenv("GC_LOG_FILE=CON");
	GC_set_find_leak(1);
	if(argc == 1) {
		int test_num, count = 0, passed = 0;
		for(test_num = 1; test_num <= 999; test_num++) {
			char path[32];
			sprintf(path, "tests/test%03d.js", test_num);
			struct stat st;
			if(stat(path, &st)) { break; }	//Check if the file exists - if not, assume we're at the end of our tests.
			count++;
			if(run_test(path)) { passed++; }
			CHECK_LEAKS();
		}
		printf("Done. %d tests, %d pass, %d fail\n", count, passed, count - passed);
		return !(count == passed);
	} else if(argc == 2) {
		return !run_test(argv[1]);
	} else {
		printf("TinyJS test runner\n");
		printf("USAGE:\n");
		printf("    ./run_tests            : run all tests\n");
		printf("    ./run_tests test.js    : run just one test\n");
		return 1;
	}
}
