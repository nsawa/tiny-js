/*	
 *	clipdlst.h
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
 *	* Wed Nov 18 21:37:53 JST 2015 Naoyuki Sawa
 *	- 'extern "C" {'～'}'で囲みました。.cppを含むWin32プロジェクトからもインクルード出来るようにするためです。
 */
#ifndef __CLIP_DLST_H__
#define __CLIP_DLST_H__

#ifdef  __cplusplus
extern "C" {
#endif//__cplusplus

/*****************************************************************************
 *	ユーティリティ：ダブルリンクリスト
 *****************************************************************************/

//Windowsヘッダファイル(WINNT.H)との衝突回避
#ifdef  WIN32
#define _LIST_ENTRY _xLIST_ENTRY
#define  LIST_ENTRY  xLIST_ENTRY
#endif//WIN32

/* Windows DDK互換の、ダブルリンクリスト操作関数群です。
 * CONTAINING_RECORD()マクロと併せて、使用してください。 */
typedef struct _LIST_ENTRY {
	struct _LIST_ENTRY *Flink;	/* 前方リンク */
	struct _LIST_ENTRY *Blink;	/* 後方リンク */
} LIST_ENTRY;

/* ダブルリンクリストを初期化します。 */
void InitializeListHead(LIST_ENTRY* ListHead);

/* ダブルリンクリストが空か否か、判定します。 */
int IsListEmpty(LIST_ENTRY* ListHead);

/* ダブルリンクリストから、先頭の要素を取り除きます。 */
LIST_ENTRY* RemoveHeadList(LIST_ENTRY* ListHead);

/* ダブルリンクリストから、末尾の要素を取り除きます。 */
LIST_ENTRY* RemoveTailList(LIST_ENTRY* ListHead);

/* ダブルリンクリストから、指定した要素を取り除きます。 */
void RemoveEntryList(LIST_ENTRY* Entry);

/* ダブルリンクリストの先頭に、指定した要素を追加します。 */
void InsertHeadList(LIST_ENTRY* ListHead, LIST_ENTRY* Entry);

/* ダブルリンクリストの末尾に、指定した要素を追加します。 */
void InsertTailList(LIST_ENTRY* ListHead, LIST_ENTRY* Entry);

/*****************************************************************************
 *	独自拡張
 *****************************************************************************/

//ダブルリンクリストをソートします。
// - 使用方法は概ね、qsort(),qsort_r()と同じです。
//   この関数はWindows DDKには無く、当モジュールの独自拡張です。
// - アルゴリズムは、マージソートの応用です。
//   本来のマージソートは安定ソートですが、当関数は安定ソートではありません。
//   当関数が安定ソートではない理由は、SortList_r()の実装を参照してください。
void SortList(  LIST_ENTRY* ListHead, int (*Compare)(LIST_ENTRY*, LIST_ENTRY*       )             );
void SortList_r(LIST_ENTRY* ListHead, int (*Compare)(LIST_ENTRY*, LIST_ENTRY*, void*), void* Param);

#ifdef  __cplusplus
}//extern "C"
#endif//__cplusplus

#endif//__CLIP_DLST_H__
