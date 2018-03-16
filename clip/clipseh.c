/*
 *	clipseh.c
 *
 *	�\������O���� (Structured Exception Handling)
 *
 *	CLiP - Common Library for P/ECE
 *	Copyright (C) 2015-2016 Naoyuki Sawa
 *
 *	* Fri Jul 03 01:00:00 JST 2015 Naoyuki Sawa
 *	- 1st �����[�X�B
 *	* Fri Jul 03 21:00:32 JST 2015 Naoyuki Sawa
 *	- clipseh.c�Ɂu�g�p��v��ǋL���܂����B
 *	* Sun Oct 11 10:36:38 JST 2015 Naoyuki Sawa
 *	- gcc���]�v�Ȍx���△�ʂȃR�[�h�𐶐����鎖���L�����������܂����B
 *	  �ڍׂ�clipseh.c�́u2015/10/11�ɍs�����C�����e�ɂ��Ă̐����v���Q�Ƃ��ĉ������B
 *	* Sun Oct 11 13:00:37 JST 2015 Naoyuki Sawa
 *	- �ȉ���_�̋@�\��ǉ����܂����B
 *	  �����̒ǉ��@�\�́AP/ECE�J����(GCC)�ł�Windows(VC++6.0)�ł��g�p�o���܂��B
 *	  1. �O���[�o���ϐ�SEH_info��ǉ����܂����B
 *	     ��O�����u���b�N�̒��ŁA��O�𓊓������\�[�X�R�[�h�̈ʒu���Q�Əo����悤�ɂȂ�܂����B
 *	  2. �}�N��SEH_DIE()��ǉ����܂����B
 *	     ��O�����u���b�N�̒��ŉ񕜏o���Ȃ����ɁA��O�𓊓������\�[�X�R�[�h�̈ʒu��\�����Ē�~�o����悤�ɂȂ�܂����B
 *	* Sun Oct 11 16:11:49 JST 2015 Naoyuki Sawa
 *	- ���[�e�B���e�B�}�N��SEH_VERIFY()��ǉ����܂����B
 *	* Tue Oct 13 22:12:29 JST 2015 Naoyuki Sawa
 *	- ���[�e�B���e�B�}�N��SEH_use_shared,SEH_try_shared,SEH_VERIFY_SHARED()��ǉ����܂����B
 *	* Wed Oct 14 21:50:46 JST 2015 Naoyuki Sawa
 *	- SEH_info�\���̂ɁA'msg'�t�B�[���h��ǉ����܂����B
 *	  ����ɔ����ASEH_throw_msg()��ǉ����܂����B
 *	- SEH_die()���\������t�@�C�������A����܂ł̓t���p�X�������̂��A�x�[�X�������ɕύX���܂����B
 *	  SEH_throw()�ŗ�O�𓊓�����͎̂��샂�W���[���̃\�[�X�t�@�C�������ł���A�t���p�X�ɂ��Ȃ��Ă�����o���邩��ł��B
 *	  �t���p�X�ŕ\������ƒ��߂��Č��h���̂ŁA�x�[�X��������\�����鎖�ɂ��܂����B
 *	- SEH_die()�ɁASEH_info.msg�̕\����ǉ����܂����B
 *	* Sat Jan 16 21:34:44 JST 2016 Naoyuki Sawa
 *	- Win32�Ńf�o�b�K������s���ꂽ�ꍇ�́A�ŏ�ʂ̗�O�t���[�������������Ȃ��悤�ɂ��܂����B
 *	  �ύX�O�́A�����I�ȗ�O�t���[�������ŗ�O���������ꂽ���A�ŏ�ʂ̗�O�t���[���܂Ŋ����߂��ĕߑ����Ă��܂������A
 *	  ����́A�����I�ȗ�O�t���[�������ŗ�O���������ꂽ���A���̏�Ńu���[�N����悤�ɂȂ�܂��B(�f�o�b�O���s���̂�)
 *	  �ڍׂ́ASEH_init()�̃R�����g���Q�Ƃ��ĉ������B
 *	- ����̕ύX�ɔ����A�}�N���̍\����啝�ɕύX���܂����B
 *	  �}�N���̍\����啝�ɕύX���܂������A���������R�[�h�͏�L�̓_�ȊO�ς���Ă��Ȃ��͂��ł��B
 *	  ���ɁAWin32�ȊO�̃v���b�g�t�H�[���ɑ΂��āA����̕ύX�ɂ��e���͈�ؖ����͂��ł��B
 *	  ������肪�������ꍇ�́A�O��܂ł̃A�[�J�C�u�̃\�[�X�R�[�h�Ɣ�r���ĉ������B
 */
#include "clip.h"
/****************************************************************************
 *	�O���[�o���ϐ�
 ****************************************************************************/
jmp_buf* SEH_root;		//�A�v���P�[�V���������ڎQ�Ƃ��Ȃ��ŉ������B
struct SEH_info SEH_info;	//�Ō�ɗ�O�𓊓������\�[�X�R�[�h�̈ʒu�B��O�����u���b�N�̒��ŃA�v���P�[�V���������ڎQ�Əo���܂��B
/****************************************************************************
 *	���������p�֐�
 ****************************************************************************/
/*__declspec(noreturn)*/ void SEH_die(const char* file, int line, int code) /*__attribute__((noreturn))*/ {	//�A�v���P�[�V���������ڌĂяo���Ȃ��ŉ������B
	die("%s(%d): Uncaught exception 0x%X from %s(%d) '%s'",
		basename((char*)file), line, code,
		basename((char*)SEH_info.file), SEH_info.line,
		SEH_info.msg ? SEH_info.msg : "");
}
/*--------------------------------------------------------------------------*/
#ifdef  PIECE
//SEH_setjmp()��SEH_longjmp()��C����Ŏ������Ă͂����܂���B
//�Ăяo���K�w���������setjmp()��longjmp()���Ӑ}�ʂ�ɓ��삵�Ȃ�����ł��B�A�Z���u���Ŏ�������K�v���L�邽��2015/10/11�̑΍��P/ECE��p�ł��B
//�����Ƃ��AVC++6.0�ł͌�q�̗]�v�Ȍx���͏o�Ȃ����������ʂȃR�[�h�������Ă��\��Ȃ��̂ŁAVC++6.0�ł͑΍��O�̃}�N�������̂܂܎g�����ɂ��܂����B
int SEH_setjmp(jmp_buf node);				//�A�v���P�[�V���������ڌĂяo���Ȃ��ŉ������B
void SEH_longjmp(int code) __attribute__((noreturn));	//�A�v���P�[�V���������ڌĂяo���Ȃ��ŉ������B
asm("
		.code
		.align		1
		.global		SEH_setjmp
		.global		SEH_longjmp
SEH_setjmp:
		;//%r12 := node
		xld.w		[SEH_root], %r12	;//               (SEH_root = &node)
		xjp		setjmp			;//return setjmp(*(SEH_root = &node))
SEH_longjmp:
		;//%r12 := code
		ld.w		%r13, %r12		;//%r13 :=                     code
		xld.w		%r12, [SEH_root]	;//%r12 :=        (*SEH_root)
		xjp		longjmp			;//return longjmp((*SEH_root), code)
");
#endif//PIECE
/****************************************************************************
 *	�g�p��									{{2015/10/11�R�����g�ǉ�:2015/10/11�̏C���ɂ��W�J���ʂ������ς��܂������A���L�́u�g�p��v�̓W�J���ʂ͂܂����������Ă��܂���B�]���ĉ��L�̐����͌��݂̓W�J���ʂƏ�������Ă��܂��B�����̗���͕ς���Ă��Ȃ��̂ŁA�u�g�p��v�Ƃ��Ă͂��̂܂܎Q�Ƃ��Ė�肠��܂���B}}
 ****************************************************************************/
#if 0
/*--------------------------------------------------------------------------*/
//�ŏ����̏������̗�B
//�ŏ����߂��Ă��܂�Ӗ�������܂���B(��O�u���b�N�������Ȃ��̂Ɠ���������B)
static void Sample1() {
	SEH_init();
	printf("<1>\n");
	SEH_try {
		printf("<2>\n");
	} SEH_end
	printf("<3>\n");
}
//�������W�J���ʁ�����
static void Sample1() {
	{ jmp_buf __SEH_node__;
	  const int SEH_code = setjmp(*(SEH_root = &__SEH_node__));
	  if(SEH_code) { die("Uncaught exception: 0x%08X", SEH_code); } }
	printf("<1>\n");
	{ jmp_buf __SEH_node__, * const __SEH_save__ = SEH_root;
	  const int SEH_code = setjmp(*(SEH_root = &__SEH_node__));
	  if(!SEH_code) {
		printf("<2>\n");
	    SEH_root = __SEH_save__;				��������O���������Ȃ������ꍇ�ɗ�O�t���[���������߂�����
	  } else {
	    SEH_root = __SEH_save__;				��������O���ߑ�����Ȃ������ꍇ�ɗ�O�t���[���������߂�����
	    SEH_throw(SEH_code);				�������ߑ�����Ȃ�������O����ʂ̗�O�t���[���֍ē������鏈��
	} }
	printf("<3>\n");
}
/*--------------------------------------------------------------------------*/
//��{�I�ȗ�B
const char AppException1[1];
const char AppException2[1];
static void Sample2() {
	SEH_init();
	printf("<1>\n");
	SEH_try {
		printf("<2>\n");
		SEH_throw(AppException2);			��������O�R�[�h�ɂ́A'0'�ȊO�̔C�ӂ̒l���w��ł��܂��B�A�v���P�[�V�����S�̂ň�ӂƂ��邽�߂ɁA�ϐ��A�h���X���O�R�[�h�Ƃ���̂��ǂ��ł��B
	} SEH_catch(AppException1) {
		printf("<3>\n");
	} SEH_catch(AppException2) {
		printf("<4>\n");
	} SEH_catch_all {
		printf("<5> 0x%08X\n", SEH_code);		��������O�����u���b�N�̒��ł́A����������O�R�[�h��'SEH_code'�ŎQ�Əo����B
	} SEH_end
	printf("<6>\n");
}
//�������W�J���ʁ�����
static void Sample2() {
	{ jmp_buf __SEH_node__;
	  const int SEH_code = setjmp(*(SEH_root = &__SEH_node__));
	  if(SEH_code) { die("Uncaught exception: 0x%08X", SEH_code); } }
	printf("<1>\n");
	{ jmp_buf __SEH_node__, * const __SEH_save__ = SEH_root;
	  const int SEH_code = setjmp(*(SEH_root = &__SEH_node__));
	  if(!SEH_code) {
		printf("<2>\n");
		longjmp((*SEH_root), (int)(AppException2));
	  SEH_root = __SEH_save__;				��������O���������Ȃ������ꍇ�ɗ�O�t���[���������߂�����
	} else if(SEH_code == (int)(AppException1)) {
	  SEH_root = __SEH_save__;				��������O��ߑ������ꍇ�ɗ�O�t���[���������߂�����
		printf("<3>\n");
	  SEH_root = __SEH_save__;				���������ʏ����ƂȂ邪�������͖����B
	} else if(SEH_code == (int)(AppException2)) {
	  SEH_root = __SEH_save__;				��������O��ߑ������ꍇ�ɗ�O�t���[���������߂�����
		printf("<4>\n");
	  SEH_root = __SEH_save__;				���������ʏ����ƂȂ邪�������͖����B
	} else if(SEH_code == (int)(SEH_code)) {
	  SEH_root = __SEH_save__;				��������O��ߑ������ꍇ�ɗ�O�t���[���������߂�����
		printf("<5> 0x%08X\n", SEH_code);
	    SEH_root = __SEH_save__;				���������ʏ����ƂȂ邪�������͖����B
	  } else {
	    SEH_root = __SEH_save__;				��������O���ߑ�����Ȃ������ꍇ�ɗ�O�t���[���������߂�����
	    longjmp((*SEH_root), (int)(SEH_code));		�������ߑ�����Ȃ�������O����ʂ̗�O�t���[���֍ē������鏈��
	} }
	printf("<6>\n");
}
/*--------------------------------------------------------------------------*/
//��O�����u���b�N�̒�����return�Ŕ������B
const char AppException1[1];
const char AppException2[1];
int Sample3() {
	SEH_init();
	printf("<1>\n");
	SEH_try {
		printf("<2>\n");
		if(now & 1) { SEH_return 1; }			������try�u���b�N�̒�����ureturn�v�ŗ�O�u���b�N�𔲂��Ă͂����Ȃ��B���ɁuSEH_return�v���g���ĉ������B�uSEH_return;�v�uSEH_return �߂�l;�v�̂ǂ���ɂ��g�p�\�ł��B
								�@�@�@�uSEH_return�v��if�u���b�N�ɓ����ꍇ��'{�`}'�ň͂��ĉ������B�uif(now & 1) SEH_return 1;�v�Ə����Ă͂����܂���B
								�@�@�@try�u���b�N�̒�����ugoto�v�ubreak�v�ucontinue�v���ŗ�O�u���b�N�𔲂��鎖���֎~�ł��B�����͕K�v�ȃP�[�X�����Ȃ��Ǝv�����̂ŁA��֎�i�͗p�ӂ��Ȃ����ɂ��܂����B
	} SEH_catch(AppException1) {
		printf("<3>\n");
		return 2;					������catch�u���b�N�̒��ł́A�ureturn�v�ł��uSEH_return�v�ł��\���܂���B
	} SEH_catch(AppException2) {
		printf("<4>\n");
		SEH_return 3;					������catch�u���b�N�̒��ł́A�ureturn�v�ł��uSEH_return�v�ł��\���܂���B
	} SEH_end
	printf("<6>\n");
	return 0;
}
//�������W�J���ʁ�����
static void Sample1() {
	{ jmp_buf __SEH_node__;
	  const int SEH_code = setjmp(*(SEH_root = &__SEH_node__));
	  if(SEH_code) { die("Uncaught exception: 0x%08X", SEH_code); } }
	printf("<1>\n");
	{ jmp_buf __SEH_node__, * const __SEH_save__ = SEH_root;
	  const int SEH_code = setjmp(*(SEH_root = &__SEH_node__));
	  if(!SEH_code) {
		printf("<2>\n");
		if(now & 1) { SEH_root = __SEH_save__; return 1; }
	  SEH_root = __SEH_save__;
	} else if(SEH_code == (int)(AppException1)) {
	  SEH_root = __SEH_save__;
		printf("<3>\n");
		return 2;
	  SEH_root = __SEH_save__;
	} else if(SEH_code == (int)(AppException2)) {
	  SEH_root = __SEH_save__;
		printf("<4>\n");
		SEH_root = __SEH_save__; return 3;		�������uSEH_root = __SEH_save__;�v�͖��ʏ����ƂȂ邪�������͖����B
	    SEH_root = __SEH_save__;
	  } else {
	    SEH_root = __SEH_save__;
	    SEH_throw(SEH_code);
	} }
	printf("<5>\n");
	return 0;
}
/*--------------------------------------------------------------------------*/
//catch�u���b�N�̒������O���ē��������B
const char AppException1[1];
const char AppException2[1];
static void Sample2() {
	SEH_init();
	printf("<1>\n");
	SEH_try {
		printf("<2>\n");
		SEH_throw(AppException1);
	} SEH_catch(AppException1) {
		printf("<3>\n");
		SEH_throw(AppException2);			������catch�u���b�N�̒�����A��ʂ̗�O�t���[���֕ʂ̗�O�𓊓��o���܂��B
	} SEH_catch(AppException2) {
		printf("<4>\n");
		SEH_throw_last;					������catch�u���b�N�̒�����A��ʂ̗�O�t���[���֓�����O�𓊓��o���܂��B
	} SEH_end
	printf("<6>\n");
}
//�������W�J���ʁ�����
static void Sample2() {
	{ jmp_buf __SEH_node__;
	  const int SEH_code = setjmp(*(SEH_root = &__SEH_node__));
	  if(SEH_code) { die("Uncaught exception: 0x%08X", SEH_code); } }
	printf("<1>\n");
	{ jmp_buf __SEH_node__, * const __SEH_save__ = SEH_root;
	  const int SEH_code = setjmp(*(SEH_root = &__SEH_node__));
	  if(!SEH_code) {
		printf("<2>\n");
		longjmp((*SEH_root), (int)(AppException1));
	  SEH_root = __SEH_save__;
	} else if(SEH_code == (int)(AppException1)) {
	  SEH_root = __SEH_save__;
		printf("<3>\n");
		longjmp((*SEH_root), (int)(AppException2));	������catch�u���b�N�̒�����A��ʂ̗�O�t���[���֕ʂ̗�O�𓊓����鏈��
	  SEH_root = __SEH_save__;
	} else if(SEH_code == (int)(AppException2)) {
	  SEH_root = __SEH_save__;
		printf("<4>\n");
		longjmp((*SEH_root), (int)(SEH_code));		������catch�u���b�N�̒�����A��ʂ̗�O�t���[���֓�����O�𓊓����鏈��
	    SEH_root = __SEH_save__;
	  } else {
	    SEH_root = __SEH_save__;
	    longjmp((*SEH_root), (int)(SEH_code));
	} }
	printf("<5>\n");
}
/*--------------------------------------------------------------------------*/
#endif
/****************************************************************************
 *	2015/10/11�ɍs�����C�����e�ɂ��Ă̐���
 ****************************************************************************/
#if 0
/*--------------------------------------------------------------------------*/
�ȉ��̑΍��P/ECE��p�ł��BVC++6.0�ł͑΍��O�̃}�N�������̂܂܎g�����ɂ��܂����B���̗��R��SEH_setjmp()��SEH_longjmp()�̏��̃R�����g���Q�Ƃ��ĉ������B

�@--- ���̍��� ---

setjmp(),longjmp()�ŁA�O���[�o���ϐ���jmp_buf���g�����ꍇ�ɁA�ȉ��̂悤�Ȗ�肪�����鎖���L��܂�:

��	jmp_buf buf;
��	int test(int x) {	//warning: argument `x' might be clobbered by `longjmp' or `vfork'	�������Ӑ}�ǂ���ł���ɂ�������炸�A�]�v�Ȍx�����o�܂��B	�����_��
��		if(setjmp(buf)) { x++; }
��		return x;
��	}

���ȉ��̂悤�ȃR�[�h����������܂��B

��	test:
��			xsub		%sp, %sp, 4		;//�@�@�@��gcc��setjmp()��F�����āAsetjmp()�Ŗ߂��ė����ꍇ�̂��߂ɁAx�����W�X�^�ɕێ������X�^�b�N��Ɋi�[���Ă��܂��B
��			xld.w		[%sp], %r12		;//���������m���ɂ��̏ꍇ�͓K�؂Ȃ̂ł����Asetjmp()�Ŗ߂��ė��Ȃ����������Ă���ꍇ�́A���ʂȃR�[�h�ƂȂ��Ă��܂��܂��B	�����_��
��			xld.w		%r12, buf
��			xcall		setjmp
��			cmp		%r10, 0
��			jreq		test_L10
��			xld.w		%r11, [%sp]
��			add		%r11, 1
��			xld.w		[%sp], %r11
��	test_L10:
��			xld.w		%r10, [%sp]
��			xadd		%sp, %sp, 4
��			ret

��L�̖��̌����́Agcc��setjmp()��F�����āA���S�ȃR�[�h�𐶐����悤�Ƃ��Ă��邩��ł��B
����͗ǂ��̂ł����Asetjmp()�Ŗ߂��ė��Ȃ����������Ă���ꍇ�ɂ́A�]�v�Ȍx���△�ʂȃR�[�h�ƂȂ��Ă��܂��܂��B

�A--- ��肪�\������O�����ɗ^����e�� ---

���Ƃ��΁A�����W���[��(�\������O����)�ɂ����ẮAsetjmp()�Ŗ߂��ė��ē����R�[�h���x�ʂ鎖�͖������������Ă��܂��B
�������Agcc�ɂ͂��ꂪ����Ȃ��̂ŁA�]�v�Ȍx���△�ʂȃR�[�h����������Ă��܂��܂��B

��	int test(int x) {	//warning: argument `x' might be clobbered by `longjmp' or `vfork'	�������Ӑ}�ǂ���ł���ɂ�������炸�A�]�v�Ȍx�����o�܂��B	�����_��
��		SEH_try {
��			x++;
��		} SEH_catch_all {
��		} SEH_end
��		return x;
��	}

���ȉ��̂悤�ȃR�[�h����������܂��B

��	test:
��			xsub		%sp, %sp, 32		;//�@�@�@��gcc��setjmp()��F�����āAsetjmp()�Ŗ߂��ė����ꍇ�̂��߂ɁAx�����W�X�^�ɕێ������X�^�b�N��Ɋi�[���Ă��܂��B
��			xld.w		[%sp+24], %r12		;//���������m���ɂ��̏ꍇ�͓K�؂Ȃ̂ł����Asetjmp()�Ŗ߂��ė��Ȃ����������Ă���ꍇ�́A���ʂȃR�[�h�ƂȂ��Ă��܂��܂��B	�����_��
��			xld.w		%r11, [SEH_root]
��			xld.w		[%sp+28], %r11
��			xld.w		[SEH_root], %sp
��			ld.w		%r12, %sp
��			xcall		setjmp
��			cmp		%r10, 0
��			xjrne		test_L10
��			xld.w		%r11, [%sp+24]
��			add		%r11, 1
��			xld.w		[%sp+24], %r11
��	test_L10:
��			xld.w		%r11, [%sp+28]
��			xld.w		[SEH_root], %r11
��			xld.w		%r10, [%sp+24]
��			xadd		%sp, %sp, 32
��			ret

�B--- �΍����@ ---

���̍����́Agcc��setjmp()��F�����āA(���̕K�v�������P�[�X�ł�)�A���S�ȃR�[�h�𐶐����悤�Ƃ��鎖�ł��B
�����ŁAsetjmp()��ʂ̖��O�̊֐��Ń��b�v���āAgcc�ɔF�������Ȃ��悤�ɂ��ĉ�����܂����B
longjmp()�͂��̂܂܂ł��ǂ������̂ł����Alongjmp()�����b�v��������������ǂ��Ȃ�̂ŁAlongjmp()�����b�v���鎖�ɂ��܂����B
�������ďo�����֐����ASEH_setjmp(),SEH_longjmp()�ł��B

���Ȃ݂�gcc�́A'setjmp'�Ƃ����֐��������łȂ��A'_setjmp'��'__setjmp'�Ƃ����֐������Asetjmp()�ƔF������悤�ł��B
_setjmp()��__setjmp()�Ƃ������O�Ń��b�v�֐����쐬����ƁA���͉���o���܂���ł����B
'SEH_setjmp'�Ƃ����֐����ɂ���΁A���v�ł����B

�B--- �C����̌��� ---

��	int test(int x) {	//�������x�����o�Ȃ��Ȃ�܂����B	��������
��		SEH_try {
��			x++;
��		} SEH_catch_all {
��		} SEH_end
��		return x;
��	}

���ȉ��̂悤�ȃR�[�h����������܂��B

��	test:
��			pushn		%r1
��			xsub		%sp, %sp, 24
��			ld.w		%r0, %r12		;//������x�����W�X�^�ɕێ������悤�ɂȂ�܂����B	��������
��			xld.w		%r1, [SEH_root]
��			ld.w		%r12, %sp
��			xcall		SEH_setjmp
��			cmp		%r10, 0
��			xjrne		test_L10
��			xadd		%r0, %r0, 1
��	test_L10:
��			xld.w		[SEH_root], %r1
��			ld.w		%r10, %r0
��			xadd		%sp, %sp, 24
��			popn		%r1
��			ret

�C--- �C����̒��ӓ_ ---

�����ɂ́A����s�����C���́A�������Ȃ������ւ̏C���ł��B
���Ƃ��΁A�ȉ��̂悤�ȃP�[�X�ŁA�p���Ė��𐶂��鎖���L��܂��B

��	int test2(char* buf) { SEH_throw(1); return 0; }
��	void test1() {
��		char* buf;
��		SEH_try {			//(A)
��			buf = malloc(16);	//(B)
��			test2(buf);
��		} SEH_catch_all {
��			free(buf);		//������buf�́A(B)�Ŏ擾�����l�ł͂Ȃ��A(A)�̎��_�ŕێ������l(=�s��l)�ɂȂ��Ă��܂��B	warning: `buf' might be used uninitialized in this function
��			SEH_return;
��		} SEH_end
��	}

�C���O�Ȃ�΁Abuf�ϐ����X�^�b�N��Ɋm�ۂ���Ă���̂ŁA���͐����܂���B
�܂�A����s�����C���ɂ���āA�p���Ė��𐶂��Ă��܂��܂��B

�������A��L�̖��́A�\������O�����̕��ʂ̎g�����ɂ����ẮA���ɂȂ�Ȃ��ł��傤�B
�\������O�����̕��ʂ̎g�����ł́A�ȉ��̂悤�ɏ����Ǝv������ł��B

��	int test2(char* buf) { SEH_throw(1); return 0; }
��	void test1() {
��		char* buf = malloc(16);		//�������I�Ƀ��\�[�X�̊m�ێ��s��Ԃ��֐��̌Ăяo���́Atry�u���b�N�̑O�ɏ����̂����ʂ��Ǝv������B
��		if(buf) {			//��
��			SEH_try {
��				test2(buf);
��			} SEH_catch_all {
��				free(buf);	//������buf�́A�������Amalloc()�Ŏ擾�����l��ێ����Ă���B
��				SEH_return;
��			} SEH_end
��		}
��	}

����Ȃ킯�ŁA�\������O�����̕��ʂ̎g�����ł͖��ɂȂ�Ȃ��Ǝv���܂��B
�ǂ����Ă��O�҂̂悤�ȃR�[�h�������K�v���L��Ȃ�΁Avolatile���g���Ίm���ɐ������R�[�h�𐶐��ł��܂��B

��	int test2(char* buf) { SEH_throw(1); return 0; }
��	void test1() {
��		char* volatile buf;		//volatile�������ʒu�ɒ��ӂ���B'volatile char*'�ł͂Ȃ��B
��		SEH_try {			//(A)
��			buf = malloc(16);	//(B)
��			test2(buf);
��		} SEH_catch_all {
��			free(buf);		//������buf�́A(B)�Ŏ擾�����l��ێ����Ă���B
��			SEH_return;
��		} SEH_end
��	}
/*--------------------------------------------------------------------------*/
#endif
