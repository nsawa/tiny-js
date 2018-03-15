/*	
 *	clipdlst.c
 *
 *	ユーティリティ：ダブルリンクリスト
 *
 *	CLiP - Common Library for P/ECE
 *	Copyright (C) 2015 Naoyuki Sawa
 *
 *	* Tue Jan 06 21:26:41 JST 2015 Naoyuki Sawa
 *	- 新規作成。
 *	- 「ユーティリティ：ダブルリンクリスト」を、clipmisc.*⇒clipdlst.*へ分離しました。
 *	  使用頻度は高いので、無駄なリンクを防ぐためと言うよりも、モジュール分けして分り易くするためです。
 *	* Wed Jan 07 00:30:50 JST 2015 Naoyuki Sawa
 *	- InitializeListHead(),IsListEmpty(), RemoveHeadList(), RemoveTailList(),RemoveEntryList(),InsertHeadList(),InsertTailList()をAsm化しました。
 *	  C版:100バイト⇒Asm版:72バイト になりました。28バイト節約できました。
 *	* Thu Jul 16 22:21:18 JST 2015 Naoyuki Sawa
 *	- SortList(),SortList_r()を追加しました。
 */
#include "clip.h"

/*****************************************************************************
 *	ユーティリティ：ダブルリンクリスト
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
		and		%r10, 2			;//return     (ListHead->Flink == ListHead)	*delay*		//C版は非0値として1を返すが、Asm版は非0値として2を返す。非0値に依存しないよう注意せよ。
		;//-------------------------------------;//
RemoveHeadList:
		ld.w		%r12, [%r12]		;//%r12 := Head = ListHead->Flink
		jp		RemoveEntryList		;//return RemoveEntryList(Head)					//Asm版RemoveEntryList()の隠れ戻り値に依存
		;//-------------------------------------;//
RemoveTailList:
		xld.w		%r12, [%r12+4]		;//%r12 := Tail = ListHead->Blink
	;//不要	jp		RemoveEntryList		;//return RemoveEntryList(Tail)					//Asm版RemoveEntryList()の隠れ戻り値に依存
		;//-------------------------------------;//
RemoveEntryList:
	;//{{インターロック回避
	;//	ld.w		%r10, %r12		;//%r10 := Entry
	;//	ld.w		%r11, [%r12]+		;//%r11 := Flink = Entry->Flink
	;//	ld.w		%r12, [%r12]		;//%r12 := Blink = Entry->Blink
	;//	ld.w		[%r12] %r11		;// Blink->Flink = Flink			!INTERLOCK!
	;//	xld.w		[%r11+4], %r12		;// Flink->Blink = Blink
	;//	ret					;//return  Entry						//Asm版RemoveEntryList()の隠れ戻り値 %r10 := Entry
	;//↓インターロック回避
		ld.w		%r10, %r12		;//%r10 := Entry
		ld.w		%r11, [%r12]+		;//%r11 := Flink = Entry->Flink
		ld.w		%r12, [%r12]		;//%r12 := Blink = Entry->Blink
		xld.w		[%r11+4], %r12		;// Flink->Blink = Blink
		ld.w		[%r12] %r11		;// Blink->Flink = Flink
		ret					;//return  Entry						//Asm版RemoveEntryList()の隠れ戻り値 %r10 := Entry
	;//}}インターロック回避
		;//-------------------------------------;//
InsertHeadList:
	;//{{インターロック回避,1命令節約
	;//	ld.w		%r9, [%r12]		;//%r9  :=    Head = ListHead->Flink
	;//	ld.w		[%r13], %r9		;//   Entry->Flink = Head			!INTERLOCK!
	;//	xld.w		[%r13+4], %r12		;//   Entry->Blink = ListHead
	;//	xld.w		[%r9+4], %r13		;//    Head->Blink = Entry
	;//	ld.w		[%r12], %r13		;//ListHead->Flink = Entry
	;//	ret
	;//↓インターロック回避,1命令節約
		ld.w		%r9, [%r12]		;//%r9  :=    Head = ListHead->Flink
		xld.w		[%r9+4], %r13		;//    Head->Blink = Entry
		ld.w		[%r12], %r13		;//ListHead->Flink = Entry
		ld.w		[%r13]+, %r9		;//   Entry->Flink = Head, %r13 := &Entry->Blink
		ld.w		[%r13], %r12		;//   Entry->Blink = ListHead
		ret
	;//}}インターロック回避,1命令節約
		;//-------------------------------------;//
InsertTailList:
	;//{{1命令節約
	;//	xld.w		%r9, [%r12+4]		;//%r9  :=    Tail = ListHead->Blink
	;//	ld.w		[%r13], %r12		;//   Entry->Flink = ListHead
	;//	xld.w		[%r13+4], %r9		;//   Entry->Blink = Tail
	;//	xld.w		[%r12+4], %r13		;//ListHead->Blink = Entry
	;//	ld.w		[%r9], %r13		;//    Tail->Flink = Entry
	;//	ret
	;//↓1命令節約
		ld.w		[%r13], %r12		;//   Entry->Flink = ListHead
		add		%r12, 4			;//%r12 :=          &ListHead->Blink
		ld.w		%r9, [%r12]		;//%r9  :=    Tail = ListHead->Blink
		xld.w		[%r13+4], %r9		;//   Entry->Blink = Tail
		ld.w		[%r12], %r13		;//ListHead->Blink = Entry
		ld.w		[%r9], %r13		;//    Tail->Flink = Entry
		ret
	;//}}1命令節約
");
#endif//PIECE

/*****************************************************************************
 *	独自拡張
 *****************************************************************************/

void SortList(LIST_ENTRY* ListHead, int (*Compare)(LIST_ENTRY*, LIST_ENTRY*)) {
	//SortList()とSortList_r()は、Compare関数の形式が異なりますが、
	//'cdecl呼出規約'ならば、SortList()形式のCompare関数において、余分な引数が無視されるので問題有りません。
	SortList_r(ListHead, (int (*)(LIST_ENTRY*, LIST_ENTRY*, void*))Compare, NULL);
}
#if 0
//×安定版クイックソートのアルゴリズムによる実装。
//│予めソートされたリストに対して実行すると、再帰呼出しが深くなり過ぎ、プログラムが停止する問題が有る。
//│固定配列のクイックソートならば、分割した片方の配列を再帰しない方法で回避出来るが、リストでは難しい。
void SortList_r(LIST_ENTRY* ListHead, int (*Compare)(LIST_ENTRY*, LIST_ENTRY*, void*), void* Param) {
	LIST_ENTRY ListLT;	//ピボット未満の要素を、並びを維持したままで、追加して行くリスト。
	LIST_ENTRY ListGE;	//ピボット以上の要素を、並びを維持したままで、追加して行くリスト。
	LIST_ENTRY *Pivot, *Entry;
	//元のリストが空ならば、何もしない。
	if(IsListEmpty(ListHead)) { return; }	//ここまで
	//左のリストを初期化する。
	InitializeListHead(&ListLT);
	//右のリストを初期化する。
	InitializeListHead(&ListGE);
	//元のリストの先頭要素を取り外し、ピボットとする。
	Pivot = RemoveHeadList(ListHead);
	//元のリストが空になるまで…
	while(!IsListEmpty(ListHead)) {
		//元のリストの先頭要素を取り外す。
		Entry = RemoveHeadList(ListHead);
		//ピボット未満ならば、並びを維持したままで、左のリストに追加する。
		if((*Compare)(Entry, Pivot, Param) < 0) {
			InsertTailList(&ListLT, Entry);
		//ピボット以上ならば、並びを維持したままで、右のリストに追加する。
		} else {
			InsertTailList(&ListGE, Entry);
		}
	}
	//左のリストをソートする。ピボット未満の要素を、安定ソートした結果となる。
	SortList_r(&ListLT, Compare, Param);
	//右のリストをソートする。ピボット以上の要素を、安定ソートした結果となる。
	SortList_r(&ListGE, Compare, Param);
	//左のリストを、並びを維持したままで、元のリストに書き戻す。
	while(!IsListEmpty(&ListLT)) {
		Entry = RemoveHeadList(&ListLT);
		InsertTailList(ListHead, Entry);
	}
	//ピボットを、元のリストに書き戻す。右のリストの先頭にピボットと同じ値が有っても、この手順で安定ソートとなる。
	InsertTailList(ListHead, Pivot);
	//右のリストを、並びを維持したままで、元のリストに書き戻す。
	while(!IsListEmpty(&ListGE)) {
		Entry = RemoveHeadList(&ListGE);
		InsertTailList(ListHead, Entry);
	}
}
#else
//○マージソートのアルゴリズムによる実装。
//│リストを二等分して再帰するので、最悪でも30回程度の再帰呼出しに抑えられるので、クイックソート版の問題が回避出来る。
//│ただし、本来のマージソートは安定ソートだが、当関数の動作は不安定ソートである事に注意せよ。
void SortList_r(LIST_ENTRY* ListHead, int (*Compare)(LIST_ENTRY*, LIST_ENTRY*, void*), void* Param) {
	LIST_ENTRY List1, List2, *Entry1, *Entry2;
	//左右のリストを初期化する。
	InitializeListHead(&List1);
	InitializeListHead(&List2);
	//元のリストが空になるまで…
	for(;;) {
		//元のリストが空ならば、抜ける。				//┐
		if(IsListEmpty(ListHead)) { break; }				//│
		//元のリストの先頭要素を取り外し、左のリストに追加する。	//│
		Entry1 = RemoveHeadList(ListHead);				//│
		InsertTailList(&List1, Entry1);					//│固定配列のマージソートと違って最初に全体の要素数が判らないので、二等分するために一要素づつ振り分ける事にした。
		//元のリストが空ならば、抜ける。				//│固定配列のマージソートは安定ソートであるが、当関数の実装はこの動作を行うために安定ソートではない事に注意せよ。
		if(IsListEmpty(ListHead)) { break; }				//│
		//元のリストの先頭要素を取り外し、右のリストに追加する。	//│
		Entry2 = RemoveHeadList(ListHead);				//│
		InsertTailList(&List2, Entry2);					//┘
	}
	//右のリストが空でなければ…
	// - 元のリストの要素数が0,又は,1になったら、再帰を停止するための判断である。
	//   この判断を行わないと、0,又は,1要素のリストに対して無限に再帰してしまう。
	if(!IsListEmpty(&List2)) {
		//左右のリストをソートする。
		SortList_r(&List1, Compare, Param);
		SortList_r(&List2, Compare, Param);
	}
	//左右のリストが空になるまで…
	for(;;) {
		//左右のリストの先頭要素を取り外す。空ならばNULLとする。
		Entry1 = IsListEmpty(&List1) ? NULL : RemoveHeadList(&List1);
		Entry2 = IsListEmpty(&List2) ? NULL : RemoveHeadList(&List2);
		//左の要素が有れば…
		if(Entry1) {
			//右の要素が有れば…
			if(Entry2) {
				//左の要素が右の要素よりも小さいか同じならば…
				if((*Compare)(Entry1, Entry2, Param) <= 0) {
					//左の要素を元のリストの末尾に追加する。
					InsertTailList(ListHead, Entry1);
					//右の要素を右のリストの先頭に書き戻す。
					InsertHeadList(&List2, Entry2);
				//右の要素が左の要素よりも小さければ…
				} else {
					//右の要素を元のリストの末尾に追加する。
					InsertTailList(ListHead, Entry2);
					//左の要素を左のリストの先頭に書き戻す。
					InsertHeadList(&List1, Entry1);
				}
			//右の要素が無ければ…
			} else {
				//左の要素を元のリストの末尾に追加する。
				InsertTailList(ListHead, Entry1);
			}
		//左の要素が無ければ…
		} else {
			//右の要素が有れば…
			if(Entry2) {
				//右の要素を元のリストの末尾に追加する。
				InsertTailList(ListHead, Entry2);
			//右の要素が無ければ…
			} else {
				//ループを抜ける。
				break;
			}
		}
	}
}
#endif
