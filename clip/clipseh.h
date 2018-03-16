/*
 *	clipseh.h
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
 *	* Wed Nov 18 21:37:53 JST 2015 Naoyuki Sawa
 *	- 'extern "C" {'�`'}'�ň݂͂܂����B.cpp���܂�Win32�v���W�F�N�g������C���N���[�h�o����悤�ɂ��邽�߂ł��B
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
#ifndef __CLIP_SEH_H__
#define __CLIP_SEH_H__
#ifdef  __cplusplus
extern "C" {
#endif//__cplusplus
/****************************************************************************
 *	�O���[�o���ϐ�
 ****************************************************************************/
//�A�v���P�[�V���������ڎQ�Ƃ��Ȃ��ŉ������B
extern jmp_buf* SEH_root;
//�Ō�ɗ�O�𓊓������\�[�X�R�[�h�̈ʒu�B
//��O�����u���b�N�̒��ŃA�v���P�[�V���������ڎQ�Əo���܂��B
extern struct SEH_info { const char* file; int line; char* msg; } SEH_info;
/****************************************************************************
 *	���������p�֐�,���������p�}�N��
 ****************************************************************************/
//�A�v���P�[�V���������ڎg�p���Ȃ��ŉ������B
#if     defined(PIECE)
  //P/ECE
  void SEH_die(const char* file, int line, int code) __attribute__((noreturn));
  int SEH_setjmp(jmp_buf node);
  void SEH_longjmp(int code) __attribute__((noreturn));
#elif   defined(WIN32)
  //Win32
  __declspec(noreturn) void SEH_die(const char* file, int line, int code);
  #define SEH_setjmp(__node__)						\
    setjmp(*(SEH_root = &(__node__)))
  #define SEH_longjmp(__code__)						\
    do { if(!SEH_root) { SEH_die(__FILE__, __LINE__, (__code__)); }	\
         longjmp((*SEH_root), (__code__)); } while(0)
#else //!defined(PIECE) && !defined(WIN32)
  //Generic
  void SEH_die(const char* file, int line, int code);
  #define SEH_setjmp(__node__)						\
    setjmp(*(SEH_root = &(__node__)))
  #define SEH_longjmp(__code__)						\
    longjmp((*SEH_root), (__code__))
#endif//!defined(PIECE) && !defined(WIN32)
/****************************************************************************
 *	�A�v���P�[�V�����p�}�N��
 ****************************************************************************/
//��O�����u���b�N�̒��ŉ񕜏o���Ȃ����ɌĂяo���ĉ������B
#define SEH_DIE()							\
  SEH_die(__FILE__, __LINE__, SEH_code)
/*--------------------------------------------------------------------------*/
//�ߑ�����Ȃ�������O��ߑ����ăG���[���b�Z�[�W��\�����邽�߂́A��O�t���[�������������܂��B
//�K�{�ł͗L��܂��񂪁A���}�N�������s�����ɗ�O���ߑ�����Ȃ������ꍇ�́A�ُ퓮��ƂȂ�܂��B
//�A�v���P�[�V�����̎��s���ɔj������Ȃ��R���e�L�X�g��ŁA���}�N�������s���Ă��������B
//��ʓI�ɂ́A�A�v���P�[�V�����G���g���|�C���g(main�֐�)�̐擪�Ŏ��s����̂��K�؂ł��B
// - Win32�Ńf�o�b�K������s�����ꍇ�́A��O�t���[��������������SEH_root��NULL�̂܂܂ɂ��܂��B
//   SEH_throw,����,SEH_throw_last�ŗ�O���������ꂽ���ɁASEH_root��NULL�Ȃ��(=�ߑ������O�t���[�����������)���̏�Ńu���[�N���܂��B
//   �����������R�́A�ŏ�ʂ̗�O�t���[���܂Ŋ����߂��ău���[�N��������A��O�𓊓������ӏ��Ńu���[�N��������f�o�b�O���Ղ�����ł��B
// - ��L�̓���́AWin32�Ńf�o�b�K������s�����ꍇ�̂ݓK�p���܂��B�f�o�b�O�r���h�������[�X�r���h���͊֌W�L��܂���B
//   Win32�ȊO�̃v���b�g�t�H�[����AWin32�ł�exe�𒼐ڎ��s�����ꍇ�́A����܂Œʂ�ŏ�ʂ̗�O�t���[���ŕߑ����܂��B
#ifdef  WIN32
  //Win32
  #define SEH_init()							\
    if(!IsDebuggerPresent()) { jmp_buf __SEH_node__;			\
         const int SEH_code = SEH_setjmp(__SEH_node__);			\
         if(SEH_code) { SEH_DIE(); } }
#else //WIN32
  //P/ECE,Generic
  #define SEH_init()							\
    do { jmp_buf __SEH_node__;						\
         const int SEH_code = SEH_setjmp(__SEH_node__);			\
         if(SEH_code) { SEH_DIE(); } } while(0)
#endif//WIN32
/*--------------------------------------------------------------------------*/
//��O�u���b�N���J�n���܂��B
#define SEH_try								\
  { jmp_buf __SEH_node__, * const __SEH_save__ = SEH_root;		\
    const int SEH_code = SEH_setjmp(__SEH_node__);			\
    if(!SEH_code) {
/*--------------------------------------------------------------------------*/
//�����Ŏw�肵����O�R�[�h�݂̂�ߑ����܂��B
#define SEH_catch(__code__)						\
      SEH_root = __SEH_save__;						\
    } else if(SEH_code == (int)(__code__)) {				\
      SEH_root = __SEH_save__;
/*--------------------------------------------------------------------------*/
//�S�Ă̗�O�R�[�h��ߑ����܂��B
#define SEH_catch_all							\
    SEH_catch(SEH_code)
/*--------------------------------------------------------------------------*/
//��O�u���b�N���I�����܂��B
#define SEH_end								\
      SEH_root = __SEH_save__;						\
    } else {								\
      SEH_root = __SEH_save__;						\
      SEH_throw_last;							\
  } }
/*--------------------------------------------------------------------------*/
//��O�𓊓����܂��B
//���̎��A�Ō�ɗ�O�𓊓������\�[�X�R�[�h�̈ʒu(SEH_info)��ݒ肵�܂��B
// - SEH_throw()�́A���b�Z�[�W�������������߂ɁASEH_info.msg��NULL���i�[���܂��B
//   SEH_throw_msg()�́A�w�肳�ꂽ���b�Z�[�W��������ASEH_info.msg�Ɋi�[���܂��B
// - SEH_info.msg�Ɋi�[���镶����́A��{�I�ɂ́A�萔������ł��B
//   ���I�ɐ���������������i�[���鎖���\�ł����A��������ꍇ�A�A�v���P�[�V������SEH_info.msg�̃������Ǘ����s���ĉ������B
//   Garbage collector�����삵�Ă�����Ȃ�΁A���Ɉӎ����鎖�����Astrdup_printf()���œ��I�ɐ���������������i�[�o���܂��B
#define SEH_throw(__code__)						\
  SEH_throw_msg(__code__, NULL)
#define SEH_throw_msg(__code__, __msg__)				\
  do { SEH_info.file = __FILE__;					\
       SEH_info.line = __LINE__;					\
       SEH_info.msg  = (char*)(__msg__);				\
       SEH_longjmp((int)(__code__)); } while(0)
/*--------------------------------------------------------------------------*/
//catch�u���b�N�̒��Ŏg�p���A�ߑ�������O�R�[�h����ʂ̗�O�t���[���֍ē������܂��B
//���̎��A�Ō�ɗ�O�𓊓������\�[�X�R�[�h�̈ʒu(SEH_info)��ύX���܂���B
#define SEH_throw_last							\
  SEH_longjmp(SEH_code)
/*--------------------------------------------------------------------------*/
//try�u���b�N�̒�����֐��𔲂���ꍇ�́Areturn�̑���SEH_return���g���Ă��������B
//�֐��̖߂�l���w��o���܂��B
#define SEH_return							\
  SEH_root = __SEH_save__; return
/****************************************************************************
 *	���[�e�B���e�B�}�N��
 ****************************************************************************/
//��̊֐��̒���SEH_try����������g�p����ꍇ�́ASEH_try�̑��ɉ��L�̃}�N�����g�p����ƃX�^�b�N��ߖ�o���܂��B
// - ��̊֐��̒���SEH_try�𕡐���g�p����ƁAP/ECE�J������GCC�͊e�u���b�N��__SEH_node__�̃��������ʂɊm�ۂ��Ă��܂��悤�ł��B
//   SEH_try�𐔉�g�p������x�Ȃ�Γ��ɖ��͗L��܂��񂪁A���\��g�p����ꍇ�̓X�^�b�N�̈悪��������m�ۂ���āA�X�^�b�N�����ӂ�邨���ꂪ����܂��B
//   ��
//   ��SEH_try {		���@	��
//   ��    �`				��
//   ��} SEH_end			���@�̃u���b�N�𔲂�����@�̃u���b�N�Œ�`����__SEH_node__�͂����s�v�ł���B
//   ��SEH_try {		���A	���A�̃u���b�N�𔲂�����A�̃u���b�N�Œ�`����__SEH_node__�͂����s�v�ł���B
//   ��    �`				���]���āA�@�ƇA�ƇB��__SEH_node__�̓X�^�b�N��œ����̈�Ɋm�ۏo����̂����A�R���p�C���͂��ꂪ�킩�炸�ɌʂɊm�ۂ��Ă��܂��B
//   ��} SEH_end			��
//   ��SEH_try {		���B	��
//   ��    �`
//   ��} SEH_end
//   �ȉ��̂悤�ȃR�[�h����������܂��B
//   ��
//   ��pushn %r0
//   ��xsub  %sp,%sp,72		���@�A�B���ʂɊm�ۂ���Ă��܂��Ă���B���ʁB�{����'xsub %sp,%sp,24'�ŗǂ��B
//   ��    �`
//   ��ld.w  %r12,%sp		���@
//   ��xcall SEH_setjmp
//   ��    �`
//   ��xadd  %r12,%sp,24	���A					���ʁB�{����'ld.w %r12,%sp'�ŗǂ��B
//   ��xcall SEH_setjmp
//   ��    �`
//   ��xadd  %r12,%sp,48	���B					���ʁB�{����'ld.w %r12,%sp'�ŗǂ��B
//   ��xcall SEH_setjmp
//   ��    �`
//   ��xadd  %sp,%sp,72
//   ��popn  %r0
//   ��ret
//   NOTE:P/ECE�J������GCC(ver2.7.2)�ƁAVC++6.0�ł́A��L�̂Ƃ��薳�ʂȃX�^�b�N�̈���m�ۂ���R�[�h����������܂����B�ő���̍œK�����w�肵�Ă��������ʂł����B
//        �ŐV��x86�pGCC(ver4.8.1)�Ȃ�΁A�œK�����w�肵�Ȃ��Ă��A�@�A�B�œ����X�^�b�N�̈���g�p����R�[�h����������܂����B�ŐV��GCC�ł͉��P����Ă���悤�ł��B
// - ��L�̖���������邽�߂ɁA�֐��̐擪��__SEH_node_shared__���`���Ă����A�֐����ł͂�����ė��p����}�N����p�ӂ��܂����B
//   �ȉ��̂悤�Ɏg�p���ĉ������B
//   ��
//   ��SEH_use_shared;
//   ��SEH_try_shared {
//   ��    �`
//   ��} SEH_end
//   ��SEH_try_shared {
//   ��    �`
//   ��} SEH_end
//   ��SEH_try_shared {
//   ��    �`
//   ��} SEH_end
//   �ȉ��̂悤�ȃR�[�h����������܂��B
//   ��
//   ��pushn %r0
//   ��xsub  %sp,%sp,24
//   ��    �`
//   ��ld.w  %r12,%sp
//   ��xcall SEH_setjmp
//   ��    �`
//   ��ld.w  %r12,%sp
//   ��xcall SEH_setjmp
//   ��    �`
//   ��ld.w  %r12,%sp
//   ��xcall SEH_setjmp
//   ��    �`
//   ��xadd  %sp,%sp,24
//   ��popn  %r0
//   ��ret
// - !!���ӓ_!!
//   ��O�����u���b�N���l�X�g���Ă���ꍇ�́A���̃��[�e�B���e�B�}�N�����g�p���Ă͂����܂���B
//   �ȉ��̎g�p���@�͌��ł��B
//   �~
//   ��SEH_use_shared;
//   ��SEH_try_shared {		���@
//   ��    �`
//   ��  SEH_try_shared {	�����B__SEH_node_shared__�͇@�̃u���b�N���܂��g�p���Ȃ̂ɁA�����ōė��p���Ă��܂��Ă���B
//   ��      �`
//   ��  } SEH_end
//   ��    �`
//   ��} SEH_end
#define SEH_use_shared							\
  jmp_buf __SEH_node_shared__
#define SEH_try_shared							\
  { jmp_buf* const __SEH_save__ = SEH_root;				\
    const int SEH_code = SEH_setjmp(__SEH_node_shared__);		\
    if(!SEH_code) {
/*--------------------------------------------------------------------------*/
//'action'�̒��ŔC�ӂ̗�O���������ꂽ��A��O���������ꂽ�ʒu�ƁA�Ăяo����(���̃}�N�����L�q�����ʒu)��\�����āA�G���[��~���܂��B
//���̃��[�e�B���e�B�}�N���̎g�p�͕K�{�ł͗L��܂���B�A�v���P�[�V�����������I�ɗ�O��ߑ����ăG���[��~���鎖�Ɠ������ʂɂȂ�܂��B
// - <�g�p��>
//   ��
//   ��SEH_VERIFY(dVal=PrpHdr_fget_l(phApp,PrpVar_x,-1));
//   ��SEH_VERIFY(PrpHdr_fset_l(phApp,1.23,PrpVar_y,-1);
//   ��           PrpHdr_fset_l(phApp,4.56,PrpVar_z,-1));
// - !!���ӓ_!!
//   ��̊֐�����SEH_VERIFY()����������g�p����ƁA�X�^�b�N�����ӂ�邨���ꂪ����܂��B
//   ���̃}�N���̗��p�ړI����l���āA��̊֐�����SEH_VERIFY()����������g�p���鎖�͗L�蓾��Ǝv���̂ŁA���ӂ��Ă��������B
//   �X�^�b�N�����ӂ�Ȃ��悤�ɂ���ɂ́ASEH_VERIFY()�̑���SEH_VERIFY_SHARED()���g�p���ĉ������B
#define SEH_VERIFY(action)						\
  SEH_try {								\
    action;								\
  } SEH_catch_all {							\
    SEH_DIE();								\
  } SEH_end
//SEH_VERIFY()�̃X�^�b�N�ߖ�łł��B
//�ڍׂ́ASEH_use_shared,SEH_try_shared�̐������Q�Ƃ��ĉ������B
// - <�g�p��>
//  ��
//  ��SEH_use_shared;
//  ��SEH_VERIFY_SHARED(dVal=PrpHdr_fget_l(phApp,PrpVar_x,-1));
//  ��SEH_VERIFY_SHARED(dVal=PrpHdr_fget_l(phApp,PrpVar_x,-1));
//  ��SEH_VERIFY_SHARED(PrpHdr_fset_l(phApp,1.23,PrpVar_y,-1);
//  ��                  PrpHdr_fset_l(phApp,4.56,PrpVar_z,-1));
#define SEH_VERIFY_SHARED(action)					\
  SEH_try_shared {							\
    action;								\
  } SEH_catch_all {							\
    SEH_DIE();								\
  } SEH_end
#ifdef  __cplusplus
}//extern "C"
#endif//__cplusplus
#endif//__CLIP_SEH_H__
