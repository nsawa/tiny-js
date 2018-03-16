/*	
 *	clipdie.c
 *
 *	�G���[��~�����A�g���[�X�o�͏���
 *
 *	CLiP - Common Library for P/ECE
 *	Copyright (C) 2001-2016 Naoyuki Sawa
 *
 *	* Tue Aug 16 21:54:11 JST 2016 Naoyuki Sawa
 *	- 1st �����[�X�B
 *	  �G���[��~�����ƃg���[�X�o�͏������Aclipmisc.h,clipmisc.c����Aclipdie.h,clipdie.c�֕������܂����B
 *	* Thu Dec 01 22:25:12 JST 2016 Naoyuki Sawa
 *	- vdie()�ɂ����āAabort()�Ŏ~�߂邩__asm int 3�Ŏ~�߂邩�̑I�����A���C�u�����̃r���h���ɐÓI�ɑI������̂łȂ��A���s���Ƀf�o�b�K������s����Ă��邩�𔻒f���āA���I�ɑI������悤�ɕύX���܂����B
 *	  �ύX�������R�́A�ȉ��̒ʂ�ł��B
 *	- �f�o�b�K�Ŏ��s�����ꍇ�ɑ΂��ẮAabort()����__asm int 3�̕����ǂ����ɂ́A���ɋC�t���Ă����̂ł����A����܂ł́A�f�o�b�K�ȊO�Ŏ��s�����ꍇ�̃G���[���b�Z�[�W����Ղ���D�悵�āAabort()��I�����Ă��܂����B
 *	  �f�o�b�K�Ŏ��s�����ꍇ�ł��Aabort()�ł��Ăяo���K�w�̕\�����[���Ȃ邾���ŁA���قǑ傫�Ȗ��ł͂Ȃ���������ł��B
 *	  �������A���(2016/11/31)�Apcl.c���W���[�������������ۂɁA���[�U�[�R���e�L�X�g��؂�ւ��ăf�o�b�O�u���[�N�����ꍇ�Aabort()�ł̓X�^�b�N�t���[�����\�����ꂸ�A__asm int 3�Ȃ�Ε\������鎖�ɋC�t���܂����B(�ڍׂ́Apcl.c��'Wed Nov 30 21:38:22 JST 2016'�̃R�����g���Q�Ƃ��ĉ������B)
 *	  ���̂��߁A__asm int 3�ɕύX����K�v���������̂ł����A���__asm int 3�ɕύX���Ă��܂��ƁA�f�o�b�K�ȊO�Ŏ��s�������ɁA�G���[�_�C�A���O������h���Ȃ��肪�����܂��B
 *	  �����ŁA���s���Ƀf�o�b�K������s����Ă��邩�𔻒f���āA���I�ɑI������悤�ɕύX���܂����B
 *	- ����ύX�������������́A��L�̒ʂ�A���[�U�[�R���e�L�X�g��؂�ւ��ăf�o�b�O�u���[�N�����ꍇ�̑΍�ł����A���[�U�[�R���e�L�X�g���g�p���Ă��Ȃ��ꍇ�ɑ΂��Ă��A����̕ύX�͗L�p�ł���Ǝv���܂��B
 *	  �f�o�b�O�u���[�N�������ɁA�Ăяo���K�w�̕\��������܂ł�abort()�����󂭂Ȃ�A�҂�����__asm int 3�̈ʒu�Ŏ~�܂�悤�ɂȂ邩��ł��B
 */
#include "clip.h"
/*****************************************************************************
 *	�G���[��~����
 *****************************************************************************/
//{{2015/02/08�R�����g�ǉ���������������������������������������������������������������������������������������������������������������
// * Sun Feb 08 19:33:38 JST 2015 Naoyuki Sawa
// - VC++6.0�ŃR���p�C���������A'PIECE'�V���{�����`���Ă��Ȃ��̂ɁA�ȉ���'#ifdef PIECE'�u���b�N���ŃG���[���o�鎖���L��܂��B
//   �w�v���R���p�C���ς݃w�b�_�[�������I�Ɏg�p�x�̐ݒ�ɂȂ��Ă���ƁA��L�̃G���[���o��悤�ł��B
//   �w�v���R���p�C���ς݃w�b�_�[���g�p���Ȃ��x�̐ݒ�ɕύX����ƁA��L�̃G���[���o�Ȃ��Ȃ�܂����B
//   �ڍׂȌ����͔���Ȃ��̂ł����A�Ƃ�������Ɂw�v���R���p�C���ς݃w�b�_�[���g�p���Ȃ��x�̐ݒ�ɂ��Ă����������S���Ǝv���܂��B
//}}2015/02/08�R�����g�ǉ���������������������������������������������������������������������������������������������������������������
#ifdef  PIECE
/*---------------------------------------------------------------------------*/
void die(const char* fmt, ...) /*__attribute__((noreturn))*/;
asm("
		.code
		.align		1
		.global		die
die:
		;//as33�A�Z���u���́A����`�V���{����extern�ƌ��Ȃ��̂ŁA
		;//�����I��extern�錾�͕K�v����܂���B(_def_vbuff�̂���)
;//�s�v		xcall		pceLCDDispStop			;//pceLCDDispStop(); {{2005/12/13 Naoyuki Sawa �s�v�Ȃ̂ō폜���܂���}}
		xld.w		%r12, _def_vbuff		;//pceLCDSetBuffer(_def_vbuff);
		xcall		pceLCDSetBuffer
;//�s�v		xcall		pceLCDDispStart			;//pceLCDDispStart();		//{{2014/12/31�폜:cstart.c��pceAppInit00()��pceLCDDispStart()���Ăяo���A�t����ʂ̕\�����J�n����悤�ɂ��܂����B}}
;//{{2014/12/10�ύX:die()���g�p����t�H���g��؂�ւ�����悤�ɂ��܂����B�ڍׂ́Aclip.mk��'Wed Dec 10 00:39:11 JST 2014'�̃R�����g���Q�Ƃ��Ă��������B
;//{{		xld.w		%r12, 0				;//pceFontSetType(0);
;//��2014/12/10�ύX:die()���g�p����t�H���g��؂�ւ�����悤�ɂ��܂����B�ڍׂ́Aclip.mk��'Wed Dec 10 00:39:11 JST 2014'�̃R�����g���Q�Ƃ��Ă��������B
  ;//{{2014/12/23�ύX:Asm��die()��2���ߐߖ񂵂܂����B_diefont��0�`2�Ȃ̂�xld.w�łȂ�ld.w�ŏ[���ł����B
  ;//		xld.w		%r12, _diefont			;//pceFontSetType(_diefont);
  ;//��2014/12/23�ύX:Asm��die()��2���ߐߖ񂵂܂����B_diefont��0�`2�Ȃ̂�xld.w�łȂ�ld.w�ŏ[���ł����B
		 ld.w		%r12, _diefont@l		;//pceFontSetType(_diefont);
  ;//}}2014/12/23�ύX:Asm��die()��2���ߐߖ񂵂܂����B_diefont��0�`2�Ȃ̂�xld.w�łȂ�ld.w�ŏ[���ł����B
;//}}2014/12/10�ύX:die()���g�p����t�H���g��؂�ւ�����悤�ɂ��܂����B�ڍׂ́Aclip.mk��'Wed Dec 10 00:39:11 JST 2014'�̃R�����g���Q�Ƃ��Ă��������B
		xcall		pceFontSetType
		xld.w		%r12, 3				;//pceFontSetTxColor(3);
		xcall		pceFontSetTxColor
		xld.w		%r12, 0				;//pceFontSetBkColor(0);
		xcall		pceFontSetBkColor
		xld.w		%r12, 0				;//pceFontSetPos(0, 0);
		xld.w		%r13, 0
		xcall		pceFontSetPos
		xld.w		%r4, die_L10			;//pceFontPrintf(fmt, ...);
		xld.w		[%sp], %r4
		xjp		pceFontPrintf
die_L10:
		xcall		pceLCDTrans			;//pceLCDTrans();
die_L20:							;//for(;;) {
		xcall		yield				;//  yield();
		xcall		pcePadGet			;//  if(pcePadGet() & TRG_SELECT) {
		xand		%r10, %r10, 0x4000		;//    
		xjreq		die_L30				;//    
		xld.w		%r12, 1				;//    pceAppReqExit(1);
		xcall		pceAppReqExit			;//    
die_L30:							;//  }
		xjp		die_L20				;//}
");
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
//{{2016/04/15�ǉ�:vdie()��ǉ����܂����B
// * Fri Apr 15 21:07:28 JST 2016 Naoyuki Sawa
// - vdie()��ǉ����܂����B
// - P/ECE�ł̎����́A������die()�͂��̂܂܂ɂ��āA�V����vdie()��ǉ����܂����B
//   P/ECE�ł̊�����die()�́AP/ECE API��pceFontPrintf()���g�p���Ă���̂ŁAdie()��vdie()�ɕ������鎖�͏o���Ȃ�����ł��B
// - P/ECE�ł̎����́Adief()�Ɠ��l�ɁA_def_vbuff[]�̉��[80�`87���C���ɁA�����t���������W�J���܂��B
//   �ڍׂ́Aclipflt1.c�́Adief()�̃R�����g���Q�Ƃ��ĉ������B
// - Windows�ł̎����́A������die()���Adie()��vdie()�ɕ������Adie()����vdie()���Ăяo���悤�ɕύX���܂����B
//   ������die()�𗘗p���Ă���A�v���P�[�V�����̊ϓ_����́Adie()�̓���͕ς���Ă��Ȃ��̂ŁA�A�v���P�[�V�����̕ύX�͕s�v�ł��B
void vdie(const char* fmt, va_list ap) /*__attribute__((noreturn))*/ {
	vsnprintf(&_def_vbuff[DISP_X * (DISP_Y - 8)], DISP_X * 8, fmt, ap);
	die("%s", &_def_vbuff[DISP_X * (DISP_Y - 8)]);
}
//}}2016/04/15�ǉ�:vdie()��ǉ����܂����B
/*---------------------------------------------------------------------------*/
#else /*PIECE*/
/*---------------------------------------------------------------------------*/
// * __declspec(noreturn)�́A.h�t�@�C���̊֐��錾�ɕt�����邾���ŗǂ��悤�ł��B
//   .c�t�@�C���̊֐���`�ɂ́A�t�����Ă����Ȃ��Ă������݂����ł��B(�������)
//   GCC��__attribute__((noreturn))���A�֐���`�ɂ͕t���ł��Ȃ����Ƃɍ��킹�āA
//   __declspec(noreturn)���A�֐���`�ɂ͕t�����Ȃ����Ƃɂ��܂����B
// * GCC�́A__attribute__((noreturn))��t�������֐����A����ď�����Ԃ����ꍇ�A
//   �R���p�C�����Ɍx����\�����܂��B
//   Visual C++�́A__declspec(noreturn)��t�������֐����A����ď�����Ԃ��Ă��A
//   �R���p�C�����Ɍx����\�����Ȃ��̂ŁA���ӂ��K�v�ł��B
// * Fri Apr 15 21:07:28 JST 2016 Naoyuki Sawa
// - vdie()��ǉ����܂����B
// - P/ECE�ł̎����́A������die()�͂��̂܂܂ɂ��āA�V����vdie()��ǉ����܂����B
//   P/ECE�ł̊�����die()�́AP/ECE API��pceFontPrintf()���g�p���Ă���̂ŁAdie()��vdie()�ɕ������鎖�͏o���Ȃ�����ł��B
// - P/ECE�ł̎����́Adief()�Ɠ��l�ɁA_def_vbuff[]�̉��[80�`87���C���ɁA�����t���������W�J���܂��B
//   �ڍׂ́Aclipflt1.c�́Adief()�̃R�����g���Q�Ƃ��ĉ������B
// - Windows�ł̎����́A������die()���Adie()��vdie()�ɕ������Adie()����vdie()���Ăяo���悤�ɕύX���܂����B
//   ������die()�𗘗p���Ă���A�v���P�[�V�����̊ϓ_����́Adie()�̓���͕ς���Ă��Ȃ��̂ŁA�A�v���P�[�V�����̕ύX�͕s�v�ł��B
// * Fri Apr 15 23:25:13 JST 2016 Naoyuki Sawa
// - Windows�ł�vdie()�̓���������ύX���܂����B
//   ����܂ł́A��ɁA�G���[���b�Z�[�W���R���\�[���ɏo�͂��Ă��܂����B
//   �ύX��́A�W���G���[�o�͂��R���\�[���Ȃ�΁A����܂Œʂ�G���[���b�Z�[�W���R���\�[���ɏo�͂��܂����A
//   �W���G���[�o�͂��R���\�[���łȂ���΁A�G���[���b�Z�[�W�����b�Z�[�W�{�b�N�X�ŕ\������悤�ɂ��܂����B
// - �W���G���[�o�͂��R���\�[���łȂ��P�[�X�Ƃ��ẮA�ȉ��̂悤�ȃP�[�X���l������B
//   �@�����I�ɕW���G���[�o�͂��t�@�C�����փ��_�C���N�g���Ď��s�����ꍇ
//   �AVC++�̃A�v���P�[�V�����̐ݒ�ŁA�E�C���h�E�A�v���P�[�V�����Ƃ��ăr���h�����ꍇ
//   �BVC++�̃A�v���P�[�V�����̐ݒ�̓R���\�[���A�v���P�[�V���������A�̒���'#pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")'���w�肵���ꍇ
//   �����l������B
//   ���񂱂̑Ή����s�������R�́A��ɁA�A�B�̃P�[�X�ł��f�o�b�O���Ղ��悤�ɂ��邽�߂ł��B
/*__declspec(noreturn)*/ void die(const char* fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
//{{2016/04/15�ύX:vdie()��ǉ����܂����B
//	vfprintf(stderr, fmt, ap);
//	va_end(ap);
//��2016/04/15�ύX:vdie()��ǉ����܂����B
	vdie(fmt, ap);
//�s�v	va_end(ap);
}
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*__declspec(noreturn)*/ void vdie(const char* fmt, va_list ap) {
	//�W���G���[�o�͂��R���\�[���Ȃ�΁A�G���[���b�Z�[�W���R���\�[���ɏo�͂���B
	if(isatty(fileno(stderr))) {
		vfprintf(stderr, fmt, ap);
	//�W���G���[�o�͂��R���\�[���łȂ���΁A�G���[���b�Z�[�W�����b�Z�[�W�{�b�N�X�ŕ\������B
	// - �W���G���[�o�͂��R���\�[���łȂ��P�[�X�Ƃ��ẮA�ȉ��̂悤�ȃP�[�X���l������B
	//   �@�����I�ɕW���G���[�o�͂��t�@�C�����փ��_�C���N�g���Ď��s�����ꍇ
	//   �AVC++�̃A�v���P�[�V�����̐ݒ�ŁA�E�C���h�E�A�v���P�[�V�����Ƃ��ăr���h�����ꍇ
	//   �BVC++�̃A�v���P�[�V�����̐ݒ�̓R���\�[���A�v���P�[�V���������A�̒���'#pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")'���w�肵���ꍇ
	//   �����l������B
	//   ���񂱂̑Ή����s�������R�́A��ɁA�A�B�̃P�[�X�ł��f�o�b�O���Ղ��悤�ɂ��邽�߂ł��B
	} else {
		char* str = strdup_vprintf(fmt, ap);	//�G���[�����̒���strdup_vprintf()���g���̂͏����|�����A���͑��v���낤�B
		MessageBox(NULL, str, program_invocation_short_name, MB_OK|MB_ICONERROR|MB_TASKMODAL);	//MB_SYSTEMMODAL�ł͂Ȃ�MB_TASKMODAL���K�؂ł��B���O�Ƃ͗����ɃA�v���P�[�V�����̃X���b�h�ɑ�����S�ẴE�C���h�E�Ƀt�H�[�J�X���ڂ��Ȃ�����̂�MB_SYSTEMMODAL�ł͂Ȃ�MB_TASKMODAL�̕��ł��B
	//�s�v	free(str);	�˂��̌シ���ɏI������̂Ń���������͕s�v�ł��B��d����̊댯�������ł����炷���߂Ƀ�����������Ȃ����ɂ��܂����B
	}
//}}2016/04/15�ύX:vdie()��ǉ����܂����B
    //{{2015/07/14�ǉ�:Windows�ł�die()�̒�~�O�ɁAfflush(NULL)��ǉ����܂����B
	//abort()���o�̓X�g���[�����t���b�V�����邩���Ȃ����͊��ˑ��ł���AVisual C++�ł̓t���b�V�����Ȃ��悤�ł��B
	//�]���āA�G���[���O���m���ɕۑ����邽�߂ɁA���Ȃ��Ƃ�fflush(stderr)���K�v�ł��B
	//�G���[���O�ȊO�̏o�̓X�g���[�����m���ɕۑ��ł���悤�Afflush(NULL)�Ƃ��鎖�ɂ��܂����B
	fflush(NULL);
    //}}2015/07/14�ǉ�:Windows�ł�die()�̒�~�O�ɁAfflush(NULL)��ǉ����܂����B
//{{2016/12/01�ύX:vdie()�ɂ����āAabort()�Ŏ~�߂邩__asm int 3�Ŏ~�߂邩�̑I�����A���C�u�����̃r���h���ɐÓI�ɑI������̂łȂ��A���s���Ƀf�o�b�K������s����Ă��邩�𔻒f���āA���I�ɑI������悤�ɕύX���܂����B
//#if 1
//	abort(); /* DebugBreak()�����G���[�_�C�A���O������Ղ��ł� */
//#else
//	//DebugBreak(); /* abort()�����Ăяo���K�w�����Ȃ��čς݂܂� */
//	//��* Tue Oct 03 01:05:53 JST 2006 Naoyuki Sawa
//	//��- �Ăяo���K�w�̏��Ȃ��f�o�b�O�u���[�N���uDebugBreak()�v���u__asm int 3�v�ɕύX���܂����B
//	//��  2006/10/3���݂́A�Ăяo���K�w�̏��Ȃ������G���[�_�C�A���O�̔���Ղ���D�悵�āA
//	//��  �uDebugBreak()�v�ł��u__asm int 3�v�ł��Ȃ��uabort()�v���̗p���Ă���̂ł����A
//	//��  ����̂��߂̊o�������Ƃ��āA�R�����g�Ƃ��Ďc���Ă������Ƃɂ��܂����B
//	__asm int 3; /* DebugBreak()��������ɌĂяo���K�w�����Ȃ��čς݂܂� */
//#endif
//��2016/12/01�ύX:vdie()�ɂ����āAabort()�Ŏ~�߂邩__asm int 3�Ŏ~�߂邩�̑I�����A���C�u�����̃r���h���ɐÓI�ɑI������̂łȂ��A���s���Ƀf�o�b�K������s����Ă��邩�𔻒f���āA���I�ɑI������悤�ɕύX���܂����B
	//���݂̃v���Z�X���f�o�b�K�̃R���e�L�X�g�Ŏ��s����Ă��Ȃ��ꍇ�́c
	if(!IsDebuggerPresent()) {
		abort();	//DebugBreak()�����G���[�_�C�A���O������Ղ��ł��B
	//���݂̃v���Z�X���f�o�b�K�̃R���e�L�X�g�Ŏ��s����Ă���ꍇ�́c
	} else {
#ifdef  _WIN64
		DebugBreak();	//abort()�����Ăяo���K�w�����Ȃ��čς݂܂��B			//��DebugBreak()�Ŏ~�߂�ƁAVC++6.0�̃f�o�b�K���Avdie()�������̃X�^�b�N�t���[���܂ł�����͏o���Ȃ��悤�ł��B
#else //_WIN64											//��__asm int 3 �Ŏ~�߂�΁AVC++6.0�̃f�o�b�K���Avdie()�̃X�^�b�N�t���[������͏o����̂ŁA__asm int 3 �̕����]�܂����ł��B
		__asm int 3;	//DebugBreak()��������ɌĂяo���K�w�����Ȃ��čς݂܂��B	//���������AVS2015��x64�^�[�Q�b�g�ł̓C�����C���A�Z���u�����g���Ȃ�(ucontext.c��'Tue Nov 29 23:01:43 JST 2016'�̃R�����g�Q��)�̂ŁAx64�̏ꍇ��DebugBreak()�Ŏ~�߂鎖�ɂ��܂����B
#endif//_WIN64
	}
//}}2016/12/01�ύX:vdie()�ɂ����āAabort()�Ŏ~�߂邩__asm int 3�Ŏ~�߂邩�̑I�����A���C�u�����̃r���h���ɐÓI�ɑI������̂łȂ��A���s���Ƀf�o�b�K������s����Ă��邩�𔻒f���āA���I�ɑI������悤�ɕύX���܂����B
}
#endif//PIECE
/*---------------------------------------------------------------------------*/
//{{2012/01/14:�ǉ�
/*__declspec(noreturn)*/ void die2(const char* file, int line) /*__attribute__((noreturn))*/ {
	die("%s(%d)", file, line);
}
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*__declspec(noreturn)*/ void die3(const char* file, int line, const char* f) /*__attribute__((noreturn))*/ {
	die("%s(%d): %s", file, line, f);
}
//}}2012/01/14:�ǉ�
/*****************************************************************************
 *	�g���[�X�o��
 *****************************************************************************/
void trace(const char* fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
}
