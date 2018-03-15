/*	
 *	clipdlst.c
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
 */
#include "clip.h"

/*****************************************************************************
 *	���[�e�B���e�B�F�_�u�������N���X�g
 *****************************************************************************/

#ifndef PIECE
void InitializeListHead(LIST_ENTRY* ListHead) {
	ListHead->Flink = ListHead->Blink = ListHead;
}
int IsListEmpty(LIST_ENTRY* ListHead) {
	return ListHead->Flink == ListHead;
}
LIST_ENTRY* RemoveHeadList(LIST_ENTRY* ListHead) {
	LIST_ENTRY* Head = ListHead->Flink;
	RemoveEntryList(Head);
	return Head;
}
LIST_ENTRY* RemoveTailList(LIST_ENTRY* ListHead) {
	LIST_ENTRY* Tail = ListHead->Blink;
	RemoveEntryList(Tail);
	return Tail;
}
void RemoveEntryList(LIST_ENTRY* Entry) {
	LIST_ENTRY* Flink = Entry->Flink;
	LIST_ENTRY* Blink = Entry->Blink;
	Blink->Flink = Flink;
	Flink->Blink = Blink;
}
void InsertHeadList(LIST_ENTRY* ListHead, LIST_ENTRY* Entry) {
	LIST_ENTRY* Head = ListHead->Flink;
	Entry->Flink = Head;
	Entry->Blink = ListHead;
	ListHead->Flink = Head->Blink = Entry;
}
void InsertTailList(LIST_ENTRY* ListHead, LIST_ENTRY* Entry) {
	LIST_ENTRY* Tail = ListHead->Blink;
	Entry->Flink = ListHead;
	Entry->Blink = Tail;
	Tail->Flink = ListHead->Blink = Entry;
}
#else //PIECE
void InitializeListHead(LIST_ENTRY* ListHead);
int IsListEmpty(LIST_ENTRY* ListHead);
LIST_ENTRY* RemoveHeadList(LIST_ENTRY* ListHead);
LIST_ENTRY* RemoveTailList(LIST_ENTRY* ListHead);
void RemoveEntryList(LIST_ENTRY* Entry);
void InsertHeadList(LIST_ENTRY* ListHead, LIST_ENTRY* Entry);
void InsertTailList(LIST_ENTRY* ListHead, LIST_ENTRY* Entry);
asm("
		.code
		.align		1
		.global		InitializeListHead
		.global		IsListEmpty
		.global		RemoveHeadList
		.global		RemoveTailList
		.global		RemoveEntryList
		.global		InsertHeadList
		.global		InsertTailList
		;//-------------------------------------;//
InitializeListHead:
		xld.w		[%r12+4], %r12		;//ListHead->Blink = ListHead
		ld.w		[%r12], %r12		;//ListHead->Flink = ListHead
		ret
		;//-------------------------------------;//
IsListEmpty:
		ld.w		%r10, [%r12]		;//%r10    :=  ListHead->Flink
		cmp		%r10, %r12		;//%psr(Z) := (ListHead->Flink == ListHead)	!INTERLOCK!
		ld.w		%r10, %psr		;//%r10[1] := (ListHead->Flink == ListHead)
		ret.d					;//
		and		%r10, 2			;//return     (ListHead->Flink == ListHead)	*delay*		//C�ł͔�0�l�Ƃ���1��Ԃ����AAsm�ł͔�0�l�Ƃ���2��Ԃ��B��0�l�Ɉˑ����Ȃ��悤���ӂ���B
		;//-------------------------------------;//
RemoveHeadList:
		ld.w		%r12, [%r12]		;//%r12 := Head = ListHead->Flink
		jp		RemoveEntryList		;//return RemoveEntryList(Head)					//Asm��RemoveEntryList()�̉B��߂�l�Ɉˑ�
		;//-------------------------------------;//
RemoveTailList:
		xld.w		%r12, [%r12+4]		;//%r12 := Tail = ListHead->Blink
	;//�s�v	jp		RemoveEntryList		;//return RemoveEntryList(Tail)					//Asm��RemoveEntryList()�̉B��߂�l�Ɉˑ�
		;//-------------------------------------;//
RemoveEntryList:
	;//{{�C���^�[���b�N���
	;//	ld.w		%r10, %r12		;//%r10 := Entry
	;//	ld.w		%r11, [%r12]+		;//%r11 := Flink = Entry->Flink
	;//	ld.w		%r12, [%r12]		;//%r12 := Blink = Entry->Blink
	;//	ld.w		[%r12] %r11		;// Blink->Flink = Flink			!INTERLOCK!
	;//	xld.w		[%r11+4], %r12		;// Flink->Blink = Blink
	;//	ret					;//return  Entry						//Asm��RemoveEntryList()�̉B��߂�l %r10 := Entry
	;//���C���^�[���b�N���
		ld.w		%r10, %r12		;//%r10 := Entry
		ld.w		%r11, [%r12]+		;//%r11 := Flink = Entry->Flink
		ld.w		%r12, [%r12]		;//%r12 := Blink = Entry->Blink
		xld.w		[%r11+4], %r12		;// Flink->Blink = Blink
		ld.w		[%r12] %r11		;// Blink->Flink = Flink
		ret					;//return  Entry						//Asm��RemoveEntryList()�̉B��߂�l %r10 := Entry
	;//}}�C���^�[���b�N���
		;//-------------------------------------;//
InsertHeadList:
	;//{{�C���^�[���b�N���,1���ߐߖ�
	;//	ld.w		%r9, [%r12]		;//%r9  :=    Head = ListHead->Flink
	;//	ld.w		[%r13], %r9		;//   Entry->Flink = Head			!INTERLOCK!
	;//	xld.w		[%r13+4], %r12		;//   Entry->Blink = ListHead
	;//	xld.w		[%r9+4], %r13		;//    Head->Blink = Entry
	;//	ld.w		[%r12], %r13		;//ListHead->Flink = Entry
	;//	ret
	;//���C���^�[���b�N���,1���ߐߖ�
		ld.w		%r9, [%r12]		;//%r9  :=    Head = ListHead->Flink
		xld.w		[%r9+4], %r13		;//    Head->Blink = Entry
		ld.w		[%r12], %r13		;//ListHead->Flink = Entry
		ld.w		[%r13]+, %r9		;//   Entry->Flink = Head, %r13 := &Entry->Blink
		ld.w		[%r13], %r12		;//   Entry->Blink = ListHead
		ret
	;//}}�C���^�[���b�N���,1���ߐߖ�
		;//-------------------------------------;//
InsertTailList:
	;//{{1���ߐߖ�
	;//	xld.w		%r9, [%r12+4]		;//%r9  :=    Tail = ListHead->Blink
	;//	ld.w		[%r13], %r12		;//   Entry->Flink = ListHead
	;//	xld.w		[%r13+4], %r9		;//   Entry->Blink = Tail
	;//	xld.w		[%r12+4], %r13		;//ListHead->Blink = Entry
	;//	ld.w		[%r9], %r13		;//    Tail->Flink = Entry
	;//	ret
	;//��1���ߐߖ�
		ld.w		[%r13], %r12		;//   Entry->Flink = ListHead
		add		%r12, 4			;//%r12 :=          &ListHead->Blink
		ld.w		%r9, [%r12]		;//%r9  :=    Tail = ListHead->Blink
		xld.w		[%r13+4], %r9		;//   Entry->Blink = Tail
		ld.w		[%r12], %r13		;//ListHead->Blink = Entry
		ld.w		[%r9], %r13		;//    Tail->Flink = Entry
		ret
	;//}}1���ߐߖ�
");
#endif//PIECE

/*****************************************************************************
 *	�Ǝ��g��
 *****************************************************************************/

void SortList(LIST_ENTRY* ListHead, int (*Compare)(LIST_ENTRY*, LIST_ENTRY*)) {
	//SortList()��SortList_r()�́ACompare�֐��̌`�����قȂ�܂����A
	//'cdecl�ďo�K��'�Ȃ�΁ASortList()�`����Compare�֐��ɂ����āA�]���Ȉ��������������̂Ŗ��L��܂���B
	SortList_r(ListHead, (int (*)(LIST_ENTRY*, LIST_ENTRY*, void*))Compare, NULL);
}
#if 0
//�~����ŃN�C�b�N�\�[�g�̃A���S���Y���ɂ������B
//���\�߃\�[�g���ꂽ���X�g�ɑ΂��Ď��s����ƁA�ċA�ďo�����[���Ȃ�߂��A�v���O��������~�����肪�L��B
//���Œ�z��̃N�C�b�N�\�[�g�Ȃ�΁A���������Е��̔z����ċA���Ȃ����@�ŉ���o���邪�A���X�g�ł͓���B
void SortList_r(LIST_ENTRY* ListHead, int (*Compare)(LIST_ENTRY*, LIST_ENTRY*, void*), void* Param) {
	LIST_ENTRY ListLT;	//�s�{�b�g�����̗v�f���A���т��ێ������܂܂ŁA�ǉ����čs�����X�g�B
	LIST_ENTRY ListGE;	//�s�{�b�g�ȏ�̗v�f���A���т��ێ������܂܂ŁA�ǉ����čs�����X�g�B
	LIST_ENTRY *Pivot, *Entry;
	//���̃��X�g����Ȃ�΁A�������Ȃ��B
	if(IsListEmpty(ListHead)) { return; }	//�����܂�
	//���̃��X�g������������B
	InitializeListHead(&ListLT);
	//�E�̃��X�g������������B
	InitializeListHead(&ListGE);
	//���̃��X�g�̐擪�v�f�����O���A�s�{�b�g�Ƃ���B
	Pivot = RemoveHeadList(ListHead);
	//���̃��X�g����ɂȂ�܂Łc
	while(!IsListEmpty(ListHead)) {
		//���̃��X�g�̐擪�v�f�����O���B
		Entry = RemoveHeadList(ListHead);
		//�s�{�b�g�����Ȃ�΁A���т��ێ������܂܂ŁA���̃��X�g�ɒǉ�����B
		if((*Compare)(Entry, Pivot, Param) < 0) {
			InsertTailList(&ListLT, Entry);
		//�s�{�b�g�ȏ�Ȃ�΁A���т��ێ������܂܂ŁA�E�̃��X�g�ɒǉ�����B
		} else {
			InsertTailList(&ListGE, Entry);
		}
	}
	//���̃��X�g���\�[�g����B�s�{�b�g�����̗v�f���A����\�[�g�������ʂƂȂ�B
	SortList_r(&ListLT, Compare, Param);
	//�E�̃��X�g���\�[�g����B�s�{�b�g�ȏ�̗v�f���A����\�[�g�������ʂƂȂ�B
	SortList_r(&ListGE, Compare, Param);
	//���̃��X�g���A���т��ێ������܂܂ŁA���̃��X�g�ɏ����߂��B
	while(!IsListEmpty(&ListLT)) {
		Entry = RemoveHeadList(&ListLT);
		InsertTailList(ListHead, Entry);
	}
	//�s�{�b�g���A���̃��X�g�ɏ����߂��B�E�̃��X�g�̐擪�Ƀs�{�b�g�Ɠ����l���L���Ă��A���̎菇�ň���\�[�g�ƂȂ�B
	InsertTailList(ListHead, Pivot);
	//�E�̃��X�g���A���т��ێ������܂܂ŁA���̃��X�g�ɏ����߂��B
	while(!IsListEmpty(&ListGE)) {
		Entry = RemoveHeadList(&ListGE);
		InsertTailList(ListHead, Entry);
	}
}
#else
//���}�[�W�\�[�g�̃A���S���Y���ɂ������B
//�����X�g��񓙕����čċA����̂ŁA�ň��ł�30����x�̍ċA�ďo���ɗ}������̂ŁA�N�C�b�N�\�[�g�ł̖�肪����o����B
//���������A�{���̃}�[�W�\�[�g�͈���\�[�g�����A���֐��̓���͕s����\�[�g�ł��鎖�ɒ��ӂ���B
void SortList_r(LIST_ENTRY* ListHead, int (*Compare)(LIST_ENTRY*, LIST_ENTRY*, void*), void* Param) {
	LIST_ENTRY List1, List2, *Entry1, *Entry2;
	//���E�̃��X�g������������B
	InitializeListHead(&List1);
	InitializeListHead(&List2);
	//���̃��X�g����ɂȂ�܂Łc
	for(;;) {
		//���̃��X�g����Ȃ�΁A������B				//��
		if(IsListEmpty(ListHead)) { break; }				//��
		//���̃��X�g�̐擪�v�f�����O���A���̃��X�g�ɒǉ�����B	//��
		Entry1 = RemoveHeadList(ListHead);				//��
		InsertTailList(&List1, Entry1);					//���Œ�z��̃}�[�W�\�[�g�ƈ���čŏ��ɑS�̗̂v�f��������Ȃ��̂ŁA�񓙕����邽�߂Ɉ�v�f�ÂU�蕪���鎖�ɂ����B
		//���̃��X�g����Ȃ�΁A������B				//���Œ�z��̃}�[�W�\�[�g�͈���\�[�g�ł��邪�A���֐��̎����͂��̓�����s�����߂Ɉ���\�[�g�ł͂Ȃ����ɒ��ӂ���B
		if(IsListEmpty(ListHead)) { break; }				//��
		//���̃��X�g�̐擪�v�f�����O���A�E�̃��X�g�ɒǉ�����B	//��
		Entry2 = RemoveHeadList(ListHead);				//��
		InsertTailList(&List2, Entry2);					//��
	}
	//�E�̃��X�g����łȂ���΁c
	// - ���̃��X�g�̗v�f����0,����,1�ɂȂ�����A�ċA���~���邽�߂̔��f�ł���B
	//   ���̔��f���s��Ȃ��ƁA0,����,1�v�f�̃��X�g�ɑ΂��Ė����ɍċA���Ă��܂��B
	if(!IsListEmpty(&List2)) {
		//���E�̃��X�g���\�[�g����B
		SortList_r(&List1, Compare, Param);
		SortList_r(&List2, Compare, Param);
	}
	//���E�̃��X�g����ɂȂ�܂Łc
	for(;;) {
		//���E�̃��X�g�̐擪�v�f�����O���B��Ȃ��NULL�Ƃ���B
		Entry1 = IsListEmpty(&List1) ? NULL : RemoveHeadList(&List1);
		Entry2 = IsListEmpty(&List2) ? NULL : RemoveHeadList(&List2);
		//���̗v�f���L��΁c
		if(Entry1) {
			//�E�̗v�f���L��΁c
			if(Entry2) {
				//���̗v�f���E�̗v�f�����������������Ȃ�΁c
				if((*Compare)(Entry1, Entry2, Param) <= 0) {
					//���̗v�f�����̃��X�g�̖����ɒǉ�����B
					InsertTailList(ListHead, Entry1);
					//�E�̗v�f���E�̃��X�g�̐擪�ɏ����߂��B
					InsertHeadList(&List2, Entry2);
				//�E�̗v�f�����̗v�f������������΁c
				} else {
					//�E�̗v�f�����̃��X�g�̖����ɒǉ�����B
					InsertTailList(ListHead, Entry2);
					//���̗v�f�����̃��X�g�̐擪�ɏ����߂��B
					InsertHeadList(&List1, Entry1);
				}
			//�E�̗v�f��������΁c
			} else {
				//���̗v�f�����̃��X�g�̖����ɒǉ�����B
				InsertTailList(ListHead, Entry1);
			}
		//���̗v�f��������΁c
		} else {
			//�E�̗v�f���L��΁c
			if(Entry2) {
				//�E�̗v�f�����̃��X�g�̖����ɒǉ�����B
				InsertTailList(ListHead, Entry2);
			//�E�̗v�f��������΁c
			} else {
				//���[�v�𔲂���B
				break;
			}
		}
	}
}
#endif
