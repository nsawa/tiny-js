/*	
 *	clipsort.h
 *
 *	���[�e�B���e�B�F�\�[�g
 *
 *	CLiP - Common Library for P/ECE
 *	Copyright (C) 2016 Naoyuki Sawa
 *
 *	* Tue Jan 06 21:26:41 JST 2015 Naoyuki Sawa
 *	- �V�K�쐬�B
 *	- �u���[�e�B���e�B�F�\�[�g�v�̐錾���Aclipmisc.h����clipsort.h�֕������܂����B
 *	  �u���[�e�B���e�B�F�\�[�g�v�̎����͊��ɁAclipsort.c�ōs���Ă����̂ŁA���񕪗������̂͐錾�݂̂ł��B
 *	  �錾���������邱�Ƃɂ������R�́A�錾�Ǝ����̃t�@�C���̑Ή��t�����A����Ղ����邽�߂ł��B
 *	* Wed Nov 18 21:37:53 JST 2015 Naoyuki Sawa
 *	- 'extern "C" {'�`'}'�ň݂͂܂����B.cpp���܂�Win32�v���W�F�N�g������C���N���[�h�o����悤�ɂ��邽�߂ł��B
 *	* Fri Jun 10 21:42:30 JST 2016 Naoyuki Sawa
 *	- ������hsort()��msort()���Aheapsort()��mergesort()�ɖ��O�ύX���A/clip/include/stdlib.h�Ő錾����悤�ɂ��܂����B
 *	  �ڍׂ́Aclipsort.c�́u�q�[�v�\�[�g�ƃ}�[�W�\�[�g�ɂ���(2016/06/10)�v���Q�Ƃ��ĉ������B
 *	- ���̕ύX�ɔ����Aclipsort.h�͕s�v�ɂȂ�܂����B
 *	  �ЂƂ܂��A�S�̂��R�����g�A�E�g���Ďc���Ă���܂����A�܂����ăt�@�C�����폜����\��ł��B
 */
#ifndef __CLIP_SORT_H__
#define __CLIP_SORT_H__
#ifdef  __cplusplus
extern "C" {
#endif//__cplusplus
//{{2016/06/10�폜:������hsort()��msort()���Aheapsort()��mergesort()�ɖ��O�ύX���A/clip/include/stdlib.h�Ő錾����悤�ɂ��܂����B���̕ύX�ɔ����Aclipsort.h�͕s�v�ɂȂ�܂����B�ЂƂ܂��A�S�̂��R�����g�A�E�g���Ďc���Ă���܂����A�܂����ăt�@�C�����폜����\��ł��B
///*****************************************************************************
// *	���[�e�B���e�B�F�\�[�g
// *****************************************************************************/
//
///* �}�[�W�\�[�g�����s���܂��B
// * [in]
// *	base		�\�[�g����z��̐擪
// *	num		�z��̗v�f��
// *	width		�z��v�f�̃T�C�Y(�o�C�g��)
// *	compare		��r�֐�
// * [note]
// *	- �����̎d�l�́Aqsort()�֐��Ɠ����ł��B
// *	- �N�C�b�N�\�[�g�ɑ΂���A�}�[�W�\�[�g�̗��_�F
// *	  �E����\�[�g�ł���B
// *	  �E���炩���߂قƂ�ǐ��񂳂�Ă���z��ɑ΂��ẮA�N�C�b�N�\�[�g���������ł���B
// *	- �N�C�b�N�\�[�g�ɑ΂���A�}�[�W�\�[�g�̌��_�F
// *	  �E�\�[�g����z��Ɠ����T�C�Y�́A�ꎞ�I�ȍ�Ɨp���������K�v�ł���B
// *	  �E���܂萮�񂳂�Ă��Ȃ��΂�΂�̔z��ɑ΂��ẮA�N�C�b�N�\�[�g�����ᑬ�ł���B
// *	- �ȏ�̗��_�ƌ��_�����Ă��āA�\�[�g���@��I�����Ă��������B
// *	  �ʏ�̓N�C�b�N�\�[�g���g���A����\�[�g���K�v�ȏꍇ�̂݃}�[�W�\�[�g���g���̂��K�؂ł��B
// */
//void msort(void* base, size_t num, size_t width, int (*compare)(const void*, const void*)); /* -> clipsort.c */
//}}2016/06/10�폜:������hsort()��msort()���Aheapsort()��mergesort()�ɖ��O�ύX���A/clip/include/stdlib.h�Ő錾����悤�ɂ��܂����B���̕ύX�ɔ����Aclipsort.h�͕s�v�ɂȂ�܂����B�ЂƂ܂��A�S�̂��R�����g�A�E�g���Ďc���Ă���܂����A�܂����ăt�@�C�����폜����\��ł��B
#ifdef  __cplusplus
}//extern "C"
#endif//__cplusplus
#endif//__CLIP_SORT_H__
