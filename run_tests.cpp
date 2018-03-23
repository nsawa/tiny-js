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
	//�X�N���v�g�t�@�C������\������B
	printf("TEST %s ", fileName);
	//�X�N���v�g�t�@�C���̃T�C�Y���擾����B
	struct stat st;
	if(stat(fileName, &st)) {
		printf("Cannot stat file! '%s'\n", fileName);
		return 0;	//���s(0)
	}
	int size = st.st_size;
	//�X�N���v�g�t�@�C����ǂݍ��ށB
	FILE* fp = fopen(fileName, "rb");	//If we open as text, the number of bytes read may be > the size we read.
	if(!fp) {
		printf("Unable to open file! '%s'\n", fileName);
		return 0;	//���s(0)
	}
	char* buffer = (char*)calloc(size + 1/*nul*/, 1);
	fread(buffer, 1, size, fp);
	fclose(fp);
	//�C���^�v���^���쐬����B
	ST_TinyJS* tinyJS = new ST_TinyJS();
	//�֐���o�^����B
	registerFunctions(tinyJS);
	registerMathFunctions(tinyJS);
	tinyJS->addNative("function print(str)", js_print, NULL);
	tinyJS->addNative("function dump()",     js_dump,  NULL);
	//�O���[�o���I�u�W�F�N�g�ɁA�e�X�g���ʂ̏����l(0:���s)��o�^����B
	tinyJS->root->addChild("result", ST_TinyJS_Var::newNumber(0));
	//�X�N���v�g�����s����B
	try {
		tinyJS->execute(buffer);
	} catch(ST_TinyJS_Exception* e) {
		printf("ERROR: %s\n", e->msg);
	}
	//�e�X�g���ʂ��擾����B
	int pass = tinyJS->root->getParameter("result")->getBoolean();
	if(pass) {
		printf("PASS\n");
	} else {
		//���s�Ȃ�΁A���O�t�@�C���𐶐�����B
		const char* buf = strdup_printf("%s.fail.js", fileName);
		FILE* f = fopen(buf, "wt");
		if(f) {
			const char* symbols = tinyJS->root->getJSON("");
			fprintf(f, "%s", symbols);
			fclose(f);
		}
		printf("FAIL - symbols written to %s\n", buf);
	}
	//�e�X�g���ʂ�Ԃ��B
	return pass;
}
//-----------------------------------------------------------------------------
int main(int argc, char** argv) {
	int result;
//{{�폜
//	//���������[�N���o���J�n����B
//	putenv("GC_LOG_FILE=CON");
//	GC_set_find_leak(1);
//}}�폜
	//�������w�肳��Ă��Ȃ���΁c
	if(argc == (1 + 0)) {
		//�e�X�g�ԍ�1����A�ő�ł�999�܂Łc
		int test_num, count = 0, passed = 0;	//test_num:�e�X�g�ԍ�,count=�e�X�g���s��,passed=�e�X�g������
		for(test_num = 1; test_num <= 999; test_num++) {
			//�e�X�g�ԍ��ɑΉ�����A�X�N���v�g�t�@�C�������쐬����B
			const char* buf = strdup_printf("tests/test%03d.js", test_num);
			//�X�N���v�g�t�@�C�������݂��Ȃ���΁A�e�X�g���I������B
			struct stat st;
			if(stat(buf, &st)) { break; }	//Check if the file exists - if not, assume we're at the end of our tests.
			//�e�X�g�����s���A�e�X�g�����Ȃ�΃e�X�g�������𑝂₷�B
			if(run_test(buf)) { passed++; }
			count++;	//�e�X�g���s���𑝂₷�B
//{{�폜
//			//���������[�N�����o����B
//			CHECK_LEAKS();
//}}�폜
		}
		//�e�X�g���s���A�e�X�g�������A�e�X�g���s����\������B
		printf("Done. %d tests, %d pass, %d fail\n", count, passed, count - passed);
		result = !(count == passed);	//�e�X�g���s����0�Ȃ�΁A����I��(0)�Ƃ���B
	//��������w�肳��Ă�����c
	} else if(argc == (1 + 1)) {
		//�w�肳�ꂽ�X�N���v�g�����s����B
		int passed = run_test(argv[1]);
//{{�폜
//		//���������[�N�����o����B
//		CHECK_LEAKS();
//}}�폜
		result = !passed;	//�e�X�g����(1)�Ȃ�΁A����I��(0)�Ƃ���B
	//��������ȏ�w�肳��Ă�����c
	} else {
		//�g������\������B
		printf("TinyJS test runner\n");
		printf("USAGE:\n");
		printf("    ./run_tests            : run all tests\n");
		printf("    ./run_tests test.js    : run just one test\n");
		result = EXIT_FAILURE;	//�ُ�I��(1)�Ƃ���B
	}
	//���݂̃v���Z�X���f�o�b�K�̃R���e�L�X�g�Ŏ��s����Ă���c
	if(IsDebuggerPresent()) {
		//�W���o�͂����_�C���N�g����Ă��Ȃ���΁c
		if(_isatty(fileno(stdout))) {
			//�L�[���͂�҂B(�f�o�b�K����DOS���������ɕ��Č��ʂ��m�F���Â炢����������邽�߂ł��B)
			printf("���s����ɂ͉����L�[�������Ă������� . . .");	//PAUSE�R�}���h�Ɠ������b�Z�[�W�ɂ����B
			_getch();
		}
	}
	return result;
}
