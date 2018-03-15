/*	
 *	clipsort.h
 *
 *	ユーティリティ：ソート
 *
 *	CLiP - Common Library for P/ECE
 *	Copyright (C) 2016 Naoyuki Sawa
 *
 *	* Tue Jan 06 21:26:41 JST 2015 Naoyuki Sawa
 *	- 新規作成。
 *	- 「ユーティリティ：ソート」の宣言を、clipmisc.hからclipsort.hへ分離しました。
 *	  「ユーティリティ：ソート」の実装は既に、clipsort.cで行っていたので、今回分離したのは宣言のみです。
 *	  宣言も分離することにした理由は、宣言と実装のファイルの対応付けを、分り易くするためです。
 *	* Wed Nov 18 21:37:53 JST 2015 Naoyuki Sawa
 *	- 'extern "C" {'〜'}'で囲みました。.cppを含むWin32プロジェクトからもインクルード出来るようにするためです。
 *	* Fri Jun 10 21:42:30 JST 2016 Naoyuki Sawa
 *	- 既存のhsort()とmsort()を、heapsort()とmergesort()に名前変更し、/clip/include/stdlib.hで宣言するようにしました。
 *	  詳細は、clipsort.cの「ヒープソートとマージソートについて(2016/06/10)」を参照して下さい。
 *	- この変更に伴い、clipsort.hは不要になりました。
 *	  ひとまず、全体をコメントアウトして残してありますが、折を見てファイルを削除する予定です。
 */
#ifndef __CLIP_SORT_H__
#define __CLIP_SORT_H__
#ifdef  __cplusplus
extern "C" {
#endif//__cplusplus
//{{2016/06/10削除:既存のhsort()とmsort()を、heapsort()とmergesort()に名前変更し、/clip/include/stdlib.hで宣言するようにしました。この変更に伴い、clipsort.hは不要になりました。ひとまず、全体をコメントアウトして残してありますが、折を見てファイルを削除する予定です。
///*****************************************************************************
// *	ユーティリティ：ソート
// *****************************************************************************/
//
///* マージソートを実行します。
// * [in]
// *	base		ソートする配列の先頭
// *	num		配列の要素数
// *	width		配列要素のサイズ(バイト数)
// *	compare		比較関数
// * [note]
// *	- 引数の仕様は、qsort()関数と同じです。
// *	- クイックソートに対する、マージソートの利点：
// *	  ・安定ソートである。
// *	  ・あらかじめほとんど整列されている配列に対しては、クイックソートよりも高速である。
// *	- クイックソートに対する、マージソートの欠点：
// *	  ・ソートする配列と同じサイズの、一時的な作業用メモリが必要である。
// *	  ・あまり整列されていないばらばらの配列に対しては、クイックソートよりも低速である。
// *	- 以上の利点と欠点を勘案して、ソート方法を選択してください。
// *	  通常はクイックソートを使い、安定ソートが必要な場合のみマージソートを使うのが適切です。
// */
//void msort(void* base, size_t num, size_t width, int (*compare)(const void*, const void*)); /* -> clipsort.c */
//}}2016/06/10削除:既存のhsort()とmsort()を、heapsort()とmergesort()に名前変更し、/clip/include/stdlib.hで宣言するようにしました。この変更に伴い、clipsort.hは不要になりました。ひとまず、全体をコメントアウトして残してありますが、折を見てファイルを削除する予定です。
#ifdef  __cplusplus
}//extern "C"
#endif//__cplusplus
#endif//__CLIP_SORT_H__
