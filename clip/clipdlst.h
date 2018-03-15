/*	
 *	clipdlst.h
 *
 *	���[�e�B���e�B�F�_�u�������N���X�g
 *
 *	CLiP - Common Library for P/ECE
 *	Copyright (C) 2015 Naoyuki Sawa
 *
 *	* Tue Jan 06 21:26:41 JST 2015 Naoyuki Sawa
 *	- �V�K�쐬�B
 *	- �u���[�e�B���e�B�F�_�u�������N���X�g�v���Aclipmisc.*��clipdlst.*�֕������܂����B
 *	  �g�p�p�x�͍����̂ŁA���ʂȃ����N��h�����߂ƌ��������A���W���[���������ĕ���Ղ����邽�߂ł��B
 *	* Wed Jan 07 00:30:50 JST 2015 Naoyuki Sawa
 *	- InitializeListHead(),IsListEmpty(), RemoveHeadList(), RemoveTailList(),RemoveEntryList(),InsertHeadList(),InsertTailList()��Asm�����܂����B
 *	  C��:100�o�C�g��Asm��:72�o�C�g �ɂȂ�܂����B28�o�C�g�ߖ�ł��܂����B
 *	* Thu Jul 16 22:21:18 JST 2015 Naoyuki Sawa
 *	- SortList(),SortList_r()��ǉ����܂����B
 *	* Wed Nov 18 21:37:53 JST 2015 Naoyuki Sawa
 *	- 'extern "C" {'�`'}'�ň݂͂܂����B.cpp���܂�Win32�v���W�F�N�g������C���N���[�h�o����悤�ɂ��邽�߂ł��B
 */
#ifndef __CLIP_DLST_H__
#define __CLIP_DLST_H__

#ifdef  __cplusplus
extern "C" {
#endif//__cplusplus

/*****************************************************************************
 *	���[�e�B���e�B�F�_�u�������N���X�g
 *****************************************************************************/

//Windows�w�b�_�t�@�C��(WINNT.H)�Ƃ̏Փˉ��
#ifdef  WIN32
#define _LIST_ENTRY _xLIST_ENTRY
#define  LIST_ENTRY  xLIST_ENTRY
#endif//WIN32

/* Windows DDK�݊��́A�_�u�������N���X�g����֐��Q�ł��B
 * CONTAINING_RECORD()�}�N���ƕ����āA�g�p���Ă��������B */
typedef struct _LIST_ENTRY {
	struct _LIST_ENTRY *Flink;	/* �O�������N */
	struct _LIST_ENTRY *Blink;	/* ��������N */
} LIST_ENTRY;

/* �_�u�������N���X�g�����������܂��B */
void InitializeListHead(LIST_ENTRY* ListHead);

/* �_�u�������N���X�g���󂩔ۂ��A���肵�܂��B */
int IsListEmpty(LIST_ENTRY* ListHead);

/* �_�u�������N���X�g����A�擪�̗v�f����菜���܂��B */
LIST_ENTRY* RemoveHeadList(LIST_ENTRY* ListHead);

/* �_�u�������N���X�g����A�����̗v�f����菜���܂��B */
LIST_ENTRY* RemoveTailList(LIST_ENTRY* ListHead);

/* �_�u�������N���X�g����A�w�肵���v�f����菜���܂��B */
void RemoveEntryList(LIST_ENTRY* Entry);

/* �_�u�������N���X�g�̐擪�ɁA�w�肵���v�f��ǉ����܂��B */
void InsertHeadList(LIST_ENTRY* ListHead, LIST_ENTRY* Entry);

/* �_�u�������N���X�g�̖����ɁA�w�肵���v�f��ǉ����܂��B */
void InsertTailList(LIST_ENTRY* ListHead, LIST_ENTRY* Entry);

/*****************************************************************************
 *	�Ǝ��g��
 *****************************************************************************/

//�_�u�������N���X�g���\�[�g���܂��B
// - �g�p���@�͊T�ˁAqsort(),qsort_r()�Ɠ����ł��B
//   ���̊֐���Windows DDK�ɂ͖����A�����W���[���̓Ǝ��g���ł��B
// - �A���S���Y���́A�}�[�W�\�[�g�̉��p�ł��B
//   �{���̃}�[�W�\�[�g�͈���\�[�g�ł����A���֐��͈���\�[�g�ł͂���܂���B
//   ���֐�������\�[�g�ł͂Ȃ����R�́ASortList_r()�̎������Q�Ƃ��Ă��������B
void SortList(  LIST_ENTRY* ListHead, int (*Compare)(LIST_ENTRY*, LIST_ENTRY*       )             );
void SortList_r(LIST_ENTRY* ListHead, int (*Compare)(LIST_ENTRY*, LIST_ENTRY*, void*), void* Param);

#ifdef  __cplusplus
}//extern "C"
#endif//__cplusplus

#endif//__CLIP_DLST_H__
