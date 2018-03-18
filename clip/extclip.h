//*****************************************************************************
//	
//*****************************************************************************
#define STRICT
#include <windows.h>
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#define _USE_MATH_DEFINES
#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <mbstring.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#include <conio.h>		//_getch()
#include <io.h>			//_isatty()
//-----------------------------------------------------------------------------
#define MAX_DIR			_MAX_DIR
#define MAX_DRIVE		_MAX_DRIVE
#define MAX_EXT			_MAX_EXT
#define MAX_FNAME		_MAX_FNAME
//#define MAX_PATH		_MAX_PATH	��c:\program files (x86)\windows kits\10\include\10.0.16299.0\shared\minwindef.h(60)
#define makepath		_makepath
#define mbschr			_mbschr
#define mbsdec			_mbsdec
#define mbsinc			_mbsinc
#define mbsnextc		_mbsnextc
#define mbspbrk			_mbspbrk
#define mbsstr			_mbsstr
#define splitpath		_splitpath
//*****************************************************************************
//	
//*****************************************************************************
#pragma warning(push)
#pragma warning(disable:4200)	//��W���̊g���@�\���g�p����Ă��܂�:�\���̂܂��͋��p�̒��ɃT�C�Y��0�̔z�񂪂���܂��B	//�����.cpp����C���N���[�h�������ɏo��x���ł��B
#include "clipdie.h"
#include "clipdlst.h"
#include "cliphash.h"
#include "clipseh.h"
#include "clipstl.h"
#include "clipstr.h"
#include "garray.h"
#include "ght_hash_table.h"
#include "gslist.h"
#include "gstring.h"
#include "strv.h"
#pragma warning(pop)
//-----------------------------------------------------------------------------
//clip/clipdie.h
#undef  TRACE			//clipdie.h�ł̒�`���������āATinyJS.h�̒�`���g���B
//clip/clipmisc.h
#define ENTER_CS		((void)0)
#define LEAVE_CS		((void)0)
//#define CONTAINING_RECORD(address,type,field)	((type*)((char*)(address) - offsetof(type,field)))	��c:\program files (x86)\windows kits\10\include\10.0.16299.0\um\winnt.h(1367)
//clip/include/stdio.h
#define scprintf		_scprintf
#define vscprintf		_vscprintf
int asprintf(char** strp, const char* fmt, ...);
int vasprintf(char** strp, const char* fmt, va_list ap);
//clip/include/stdlib.h
void qsort_r(void* base, size_t num, size_t width, int(*compare)(const void*, const void*, void*), void* arg);
int heapsort_r(void* base, size_t num, size_t width, int (*compare)(const void*, const void*, void*), void* arg);
int mergesort_r(void* base, size_t num, size_t width, int (*compare)(const void*, const void*, void*), void* arg);
//clip/include/errno.h
#define program_invocation_name		"TinyJS"
#define program_invocation_short_name	program_invocation_name
//clip/cliplibc.c
#define basename(path)		(path)
//*****************************************************************************
//	
//*****************************************************************************
#include "../gc/leak_detector.h"
#ifdef  __cplusplus
#pragma warning(push)
#pragma warning(disable:4595)
#include "../gc/gc_cpp.h"
#include "../gc/gc_allocator.h"
#pragma warning(pop)
#endif//__cplusplus
#ifndef _WIN64
#pragma comment(lib, "gc.lib")
#else //_WIN64
#pragma comment(lib, "gc_x64.lib")
#endif//_WIN64
//*****************************************************************************
//	
//*****************************************************************************
#pragma warning(disable:4200)	//��W���̊g���@�\���g�p����Ă��܂�:�\���̂܂��͋��p�̒��ɃT�C�Y��0�̔z�񂪂���܂��B	//�����.cpp����C���N���[�h�������ɏo��x���ł��B
#pragma warning(disable:4244)	//'...'����'...'�ւ̕ϊ��ł��B�f�[�^��������\��������܂��B
#pragma warning(disable:4267)	//'...'����'...'�ɕϊ����܂����B�f�[�^�������Ă��邩������܂���B
#pragma warning(disable:4838)	//'...'����'...'�ւ̕ϊ��ɂ͏k���ϊ����K�v�ł�
