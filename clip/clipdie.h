/*	
 *	clipdie.h
 *
 *	�G���[��~�����A�g���[�X�o�͏���
 *
 *	CLiP - Common Library for P/ECE
 *	Copyright (C) 2001-2016 Naoyuki Sawa
 *
 *	* Tue Aug 16 21:54:11 JST 2016 Naoyuki Sawa
 *	- 1st �����[�X�B
 *	  �G���[��~�����ƃg���[�X�o�͏������Aclipmisc.h,clipmisc.c����Aclipdie.h,clipdie.c�֕������܂����B
 */
#ifndef __CLIP_DIE_H__
#define __CLIP_DIE_H__
#ifdef  __cplusplus
extern "C" {
#endif//__cplusplus
/*****************************************************************************
 *	�G���[��~����
 *****************************************************************************/
//�G���[���b�Z�[�W��\�����A�v���O�����������I�����܂��B
//[in]
//	fmt,...		printf()�̈����Ɠ��l�ł��B
//[note]
//	- ���̊֐��͏�����Ԃ��܂���B
#ifdef  PIECE
void die(const char* fmt, ...) __attribute__((noreturn));
//{{2016/04/15�ǉ�:vdie()��ǉ����܂����B
void vdie(const char* fmt, va_list ap) __attribute__((noreturn));
//}}2016/04/15�ǉ�:vdie()��ǉ����܂����B
//{{2012/01/14:�ǉ�
void die2(const char* file, int line) __attribute__((noreturn));
void die3(const char* file, int line, const char* f) __attribute__((noreturn));
//}}2012/01/14:�ǉ�
#else //PIECE
__declspec(noreturn) void die(const char* fmt, ...);
//{{2016/04/15�ǉ�:vdie()��ǉ����܂����B
__declspec(noreturn) void vdie(const char* fmt, va_list ap);
//}}2016/04/15�ǉ�:vdie()��ǉ����܂����B
//{{2012/01/14:�ǉ�
__declspec(noreturn) void die2(const char* file, int line);
__declspec(noreturn) void die3(const char* file, int line, const char* f);
//}}2012/01/14:�ǉ�
#endif//PIECE
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
//{{2012/01/14:�ύX
//#define DIE()		die(__FILE__ "(%d)", __LINE__)
//��2012/01/14:�ύX
#define DIE()		die2(__FILE__, __LINE__)
//}}2012/01/14:�ύX
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
// * Sat Aug 24 00:14:18 JST 2013 Naoyuki Sawa
// - GCC�g���@�\�u__FUNCTION__�v�Ɓu__PRETTY_FUNCTION__�v�𗘗p���āA�֐����ƍs�ԍ���\�����ăG���[��~����}�N�����쐬���܂����B
//   C����ɂ����ẮA�u__FUNCTION__�v�Ɓu__PRETTY_FUNCTION__�v�͓���Ȃ̂ŁA���L�uDIEFN()�v�ƁuDIEPF()�v�͓������ʂ������܂��B
// - �쐬���Ă͌������̂́A�����A���ۂɂ����𗘗p���邱�Ƃ͖����Ǝv���܂��B�Ȃ��Ȃ�΁A�����́uDIE()�v�ɔ�ׂāA�uDIEFN()�v�ƁuDIEPF()�v�͈ȉ��̌��_�����邩��ł��F
//   �EDIE()�����A�p���ăG���[�ӏ�����肵�Â炭�Ȃ�ꍇ������܂��B�����̃t�@�C���ɓ������O�̃��[�J���֐����������ꍇ�ɁA�����G���[���b�Z�[�W�����������\�������邩��ł��B
//   �EDIE()�����A�R�[�h�T�C�Y���傫���Ȃ�܂��B����t�@�C�����ŕ�����g�p�������ɁADIE()�Ȃ�΃t�@�C���������L�ł��܂����ADIEFN()�͌X�Ɋ֐�����W�J���邩��ł��B
//   �EVisualC++ 6.0�ł͎g�p�ł��܂���B�V����VisualC++�Ȃ��C99�W����__func__���g���č쐬�ł������ł����A������VisualC++ 6.0�g���Ȃ̂ōl�����Ă��܂���B
// - ����Ȃ킯�ŁADIEFN()��DIEPF()�����ۂɗ��p���邱�Ƃ͖����Ǝv���܂��B
//   ����ADIEFN()��DIEPF()���쐬�������R�́AP/ECE�J������gcc33�ł��A__FUNCTION__��__PRETTY_FUNCTION__�����p�ł��邱�Ƃ̊m�F�ƁA���Y�^����ł��B
#ifdef  __GNUC__
#define DIEFN()		die2(__FUNCTION__, __LINE__)
#define DIEPF()		die2(__PRETTY_FUNCTION__, __LINE__)
#endif//__GNUC__
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef  DEBUG
//{{2012/01/14:�ύX
//#define ASSERT(f)	((void)((f) || (die(__FILE__ "(%d): %s", __LINE__, #f), 0)))
//��2012/01/14:�ύX
#define ASSERT(f)	((void)((f) || (die3(__FILE__, __LINE__, #f), 0)))
//}}2012/01/14:�ύX
#define VERIFY(f)	ASSERT(f)
#else //DEBUG
#define ASSERT(f)	((void)0)
#define VERIFY(f)	((void)(f))
#endif//DEBUG
/*****************************************************************************
 *	�g���[�X�o�͏���
 *****************************************************************************/
//�W���G���[�o�͂ւ�fprintf�ł��B
//[in]
//	fmt,...		printf()�̈����Ɠ��l�ł��B
//[note]
//	- TRACE�}�N���o�R�ŗ��p���܂��B
void trace(const char* fmt, ...);
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
// !!�v����!! TRACE()�}�N���̓A�v���P�[�V������p�ł��BCLiP���C�u�����̒��Ŏg��Ȃ��悤��!!
//            (TRACE()�}�N����TRACE_ON�V���{���̒�`�ɂ���ĐU�镑�����ς��̂ŁA
//             ���炩���߃R���p�C������郉�C�u�����̒��ł�TRACE_ON�ɂ��U�镑���̐؂�ւ��������Ȃ�����)
// - �g���[�X�o�͂𗘗p����ꍇ�́A  clip.h���C���N���[�h����O�Ɂu#define TRACE_ON 1�v���`���Ă��������B
//   �g���[�X�o�͂𗘗p���Ȃ��ꍇ�́Aclip.h���C���N���[�h����O�Ɂu#define TRACE_ON 0�v���`���邩�A
//   �܂���TRACE_ON�V���{�����`���Ȃ��ł��������B
// - �g���[�X�o�͂𗘗p���Ȃ��ꍇ�́A�g���[�X�������C�R���p�C���̍œK���ɂ���č폜����܂��B
//   TRACE()�Ăяo�����폜���Ȃ��Ă��A�g���[�X�����񂪃��������������邱�Ƃ͂���܂���B
#if     (defined(TRACE_ON) && (TRACE_ON))
#define TRACE		trace
#else //(defined(TRACE_ON) && (TRACE_ON))
#define TRACE		1 ? (void)0 : trace
#endif//(defined(TRACE_ON) && (TRACE_ON))
/*---------------------------------------------------------------------------*/
#ifdef  __cplusplus
}//extern "C"
#endif//__cplusplus
#endif//__CLIP_DIE_H__
