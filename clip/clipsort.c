/*	
 *	clipsort.c
 *
 *	CLiP - Common Library for P/ECE
 *	Copyright (C) 2001-2016 Naoyuki Sawa
 *
 *	* Fri Apr 13 00:10:09 JST 2007 Naoyuki Sawa
 *	- 新規作成。
 *	* Thu Apr 19 11:44:36 JST 2007 Naoyuki Sawa
 *	- 非常に深い再帰呼び出しが発生する場合があったのを防ぐための変更を行いました。
 *	* Thu Apr 19 14:58:48 JST 2007 Naoyuki Sawa
 *	- ループを一か所、while(){...}からdo{...}while()に変更し、少し効率化しました。
 *	* Sun May 13 14:21:21 JST 2007 Naoyuki Sawa
 *	- ヒープソートの実装例を追加しました。
 *	  コメントアウトしてありますので、ライブラリプログラム自体には変化ありません。
 *	* Mon Oct 18 21:09:33 JST 2010 Naoyuki Sawa
 *	- msort()を追加しました。
 *	* Wed Nov 19 20:53:11 JST 2014 Naoyuki Sawa
 *	- qsort_r()を追加し、qsort()はqsort_r()への呼び出しに変更しました。
 *	  qsort_r()の宣言は、include/stdlib.hに有ります。
 *	- qsort_r()は、glibcバージョン2.8で追加された関数です。
 *	  詳細は、「Section: Linux Programmer's Manual (3) - QSORT」(http://linuxjm.sourceforge.jp/html/LDP_man-pages/man3/qsort.3.html)を参照してください。
 *	* Thu Jan 08 21:00:51 JST 2015 Naoyuki Sawa
 *	- qsort()とqsort_r()をアセンブラ化しました。コードサイズが24バイト小さくなり、処理速度が約4.5%早くなりました。
 *	- Visual C++ 6.0でビルドする時のために、qsort_r()の宣言を追加しました。
 *	  P/ECEでビルドする時は、include/stdlib.hでqsort_r()が宣言されているので問題有りませんが、
 *	  Visual C++ 6.0でビルドする時は、Visual C++ 6.0のstdlib.hにはqsort_r()が含まれていないので、qsort()からqsort_r()への前方参照が警告になるためです。
 *	  ここでqsort_r()を宣言することにより、Visual C++ 6.0でビルドする時も警告が出なくなりました。
 *	  尚、P/ECEでビルドする時もここで宣言されていても害は無いのですが、最小限とするために、P/ECEでビルドする時はここでは宣言しないことにしました。
 *	* Fri Jan 09 00:00:00 JST 2015 Naoyuki Sawa
 *	- Asm版qsort()を最適化しました。コードサイズがさらに8バイト小さくなりました。
 *	* Fri Jun 10 21:42:30 JST 2016 Naoyuki Sawa
 *	- 既存のhsort()とmsort()を、heapsort()とmergesort()に名前変更し、/clip/include/stdlib.hで宣言するようにしました。
 *	  詳細は、clipsort.cの「ヒープソートとマージソートについて(2016/06/10)」を参照して下さい。
 *	- clipsort.cの一番下に、「テストスイート」を追記しました。
 *	* Sat Jun 11 20:47:59 JST 2016 Naoyuki Sawa
 *	- clipsort.cの一番下に、「速度計測結果(2016/06/11)」を追記しました。
 *	  Win10(x64)とP/ECEで、各関数の速度を計測した結果と、考察です。
 */
#include "clip.h"

/*****************************************************************************
 *	クィックソート
 *****************************************************************************/

//{{2016/06/10削除:clipstdw.hで宣言したので、ここでの宣言は不要になっていました。
//#ifdef  _MSC_VER
//// * Thu Jan 08 21:00:51 JST 2015 Naoyuki Sawa
//// - Visual C++ 6.0でビルドする時のために、qsort_r()の宣言を追加しました。
////   P/ECEでビルドする時は、include/stdlib.hでqsort_r()が宣言されているので問題有りませんが、
////   Visual C++ 6.0でビルドする時は、Visual C++ 6.0のstdlib.hにはqsort_r()が含まれていないので、qsort()からqsort_r()への前方参照が警告になるためです。
////   ここでqsort_r()を宣言することにより、Visual C++ 6.0でビルドする時も警告が出なくなりました。
////   尚、P/ECEでビルドする時もここで宣言されていても害は無いのですが、最小限とするために、P/ECEでビルドする時はここでは宣言しないことにしました。
//void qsort_r(void* base, size_t num, size_t width, int (*compare)(const void*, const void*, void*), void* arg);
//#endif//_MSC_VER
//}}2016/06/10削除:clipstdw.hで宣言したので、ここでの宣言は不要になっていました。

/* * 標準CライブラリのEPSON版qsort()を置き換えます。
 * - EPSON版qsort()に不具合があるわけでは無いので、そのままでも正しく動作します。
 *   しかし、EPSON版qsort()には少し無駄な処理が含まれていて、サイズと実行速度に無駄ががります。
 *   クィックソートアルゴリズムの勉強も兼ねて、自分でCLiP版を実装してみることにしました。
 *   ほとんどEPSON版qsort()と同じで、一部の無駄な処理を削除(関数内のコメント参照)しただけです。
 * - EPSON版→CLiP版への置き換えによる、サイズ節約と高速化の効果は、以下のとおりです。
 *	qsort()+xchg()全体のモジュールサイズ
 *		EPSON版: 196バイト
 *		CLiP版 : 128バイト (68バイト節約)
 *	10000要素のランダムなint配列をソートする時間
 *		EPSON版: 1852ミリ秒
 *		CLiP版 : 1682ミリ秒 (約10%高速化)
 * * Thu Apr 19 11:44:36 JST 2007 Naoyuki Sawa
 * - 非常に深い再帰呼び出しが発生する場合があったのを防ぐための変更を行ったことにより、
 *   サイズと速度の特性が少し変わりました。
 *	qsort()+xchg()全体のモジュールサイズ
 *		EPSON版: 196バイト
 *		CLiP版 : 140バイト (56バイト節約)
 *	10000要素のランダムなint配列をソートする時間
 *		EPSON版: 1852ミリ秒
 *		CLiP版 : 1645ミリ秒 (約11%高速化)
 * * Thu Apr 19 14:58:48 JST 2007 Naoyuki Sawa
 * - ループを一か所、while(){...}からdo{...}while()に変更し、少し効率化しました。
 *   ごくわずかに速くなったけれど、サイズは減りませんでした。
 *	qsort()+xchg()全体のモジュールサイズ
 *		EPSON版: 196バイト
 *		CLiP版 : 140バイト (56バイト節約)
 *	10000要素のランダムなint配列をソートする時間
 *		EPSON版: 1852ミリ秒
 *		CLiP版 : 1642ミリ秒 (約11%高速化)
 */

#ifndef PIECE
static void xchg(unsigned char* p1, unsigned char* p2, int size) {
	int tmp;
	do {
		tmp = *p1;
		      *p1++ = *p2;
		              *p2++ = tmp;
	} while(--size);
}
#else /*PIECE*/
/*static*/ void xchg(unsigned char* p1, unsigned char* p2, int size);
asm("
		.code
		.align		1
xchg:
		sub		%r14, 1			;//%psr(Z) := size--	─┐
		 ld.b		%r10, [%r12]		;//			　│
		 ld.b		%r11, [%r13]		;//			　│
		 ld.b		[%r13]+, %r10		;//			　│	*anti-interlock*
		 ld.b		[%r12]+, %r11		;//			　│
		jrne.d		-4			;//			←┤
		sub		%r14, 1			;//%psr(Z) := size--	─┘	*delay
		ret
");
#endif /*PIECE*/

//-------------------------------------------------------------------------------------------------------------
#ifndef PIECE
//void
//qsort(void* base, size_t num, size_t width, int (*compare)(const void*, const void*))
//{
//	unsigned char* pv;	/* ピボットへのポインタ */
//	unsigned char* lo;	/* 低位ポインタ (次に低位要素(ピボット以下の要素)を格納する位置へのポインタ) */
//	unsigned char* hi;	/* 高位ポインタ (次に高位要素(ピボット超過の要素)を格納する位置へのポインタ) */
//	int lo_cnt;		/* 低位要素数 */
//	int hi_cnt;		/* 高位要素数 */
//
//	/* 要素数が0または1ならば、何もせずに帰ります。
//	 * - EPSON版は、2要素の場合も特別扱いして、単純な比較と入れ替えを行っていますが、その必要はありません。
//	 *   2要素の場合も、それ以上の要素数の場合と同じ、通常の処理で対応できます。
//	 */
//	if(num <= 1) {
//		return;
//	}
//
//	/* ポインタと要素数を初期化します。
//	 * - EPSON版は、配列の中央付近と先頭の要素を入れ替え、もともと中央付近にあった値をピボットとしています。
//	 *   元からある程度昇順に並んでいると仮定して、中央付近に中央値があることを期待しているのだと思います。
//	 *   しかし実際にはあまり効果が無く、むしろ余計な入れ替えの回数が増える分、性能を落としているようです。
//	 *   そこでCLiP版では、単純に先頭の要素をピボットとして採用することにしました。
//	 */
//	pv = base;			/* ピボット     = 先頭 */
//	lo = pv + width;		/* 低位ポインタ = ピボットの次 */
//	hi = pv + width * (num - 1);	/* 高位ポインタ = 末尾 */
//	lo_cnt = 0;			/* 低位要素数   = 0 */
//	hi_cnt = 0;			/* 高位要素数   = 0 */
//
//	/* 低位ポインタと高位ポインタが交差するまで...(即ち、ピボット以外の全要素の低位or高位が確定するまで)
//	 * - (num-1)回のカウントダウンでループする方法でも同じ結果を得られますが、(lo<=hi)でループする方が、
//	 *   num変数の生存期間を短くし、自動変数のレジスタ割り当て最適化がより有効に働くことが期待できます。
//	 */
//	while(lo <= hi) { /* 注意!!(lo<hi)ではありません!! */
//		/* 低位ポインタの位置に有る要素を、ピボットと比較します。 */
//		if((*compare)(lo, pv) <= 0) {
//			/* 低位要素ならば、低位ポインタの位置に格納します。
//			 * 低位ポインタの次の位置に有る要素が、次回の比較対象となります。
//			 */
//			lo += width;
//			lo_cnt++;
//		} else {
//			/* 高位要素ならば、高位ポインタの位置に格納します。
//			 * 高位ポインタの位置に有った要素が、次回の比較位置に移動します。
//			 */
//			xchg(lo, hi, width);
//			hi -= width;
//			hi_cnt++;
//		}
//	}
//
//	/* 以上の処理が終わった時点で[ピボット,[低位要素の並び],[高位要素の並び]]となっています。
//	 * [[低位要素の並び],ピボット,[高位要素の並び]]と入れ替えて、ピボットの位置を確定します。
//	 * - 入れ替え時に低位要素の並び順が変化しますが、まだ並び順に意味はないので、大丈夫です。
//	 * - もし低位要素が一つも無い場合、pv==hiとなっていて、このxchg()処理はダミーとなります。
//	 */
//	xchg(pv, hi, width);
//
//	/* [低位要素の並び]をソートします。 */
//	qsort(pv, lo_cnt, width, compare); /* 注意!![低位要素の並び]の先頭を指しているのは、loでなくpvです!! */
//
//	/* [高位要素の並び]をソートします。 */
//	qsort(lo, hi_cnt, width, compare); /* 注意!![高位要素の並び]の先頭を指しているのは、hiでなくloです!! */
//}
//-------------------------------------------------------------------------------------------------------------
//↓* Thu Apr 19 11:44:36 JST 2007 Naoyuki Sawa
//↓- 非常に深い再帰呼び出しが発生する場合があったのを防ぐための変更を行いました。
//↓- 上のコメントアウトした実装は、クイックソートのいちばん素直な実装です。
//↓  アルゴリズム的にはこのままでも間違いではありませんが、非常に深い再帰呼び出しが発生する場合があるという問題があります。
//↓- どのようなケースで非常に深い再帰呼び出しが発生するかというと、あらかじめソートされた数列をソートしようとした場合です。
//↓
//↓	<例>	次のような呼び出しを行った場合を考えてみます。
//↓
//↓			int array[10000] = { 1,2,3,...,10000 };
//↓			qsort(array, 10000, sizeof(int), compare);
//↓
//↓		最初の呼び出しレベルで、[[低位要素の並び],ピボット,[高位要素の並び]]は、以下のようになります。
//↓
//↓			[[(空)],1,[2,3,4,...,10000]]	(呼び出しの深さ=1)
//↓
//↓		[高位要素の並び]に対する再帰呼び出しに注目すると、次のレベルで、[[低位要素の並び],ピボット,[高位要素の並び]]は、以下のようになります。
//↓
//↓			[[(空)],2,[3,4,5,...,10000]]	(呼び出しの深さ=2)
//↓
//↓		さらに次のレベルでは、[[低位要素の並び],ピボット,[高位要素の並び]]は、以下のようになります。
//↓
//↓			[[(空)],3,[4,5,6,...,10000]]	(呼び出しの深さ=3)
//↓
//↓		最後には、こうなります。
//↓
//↓			[[(空)],10000,[(空)]]	(呼び出しの深さ=10000)
//↓
//↓		以上の例のように、あらかじめソートされた数列をソートしようとすると、非常に深い再帰呼び出しが発生する場合があります。
//↓		数列の長さをNとすると、最大Nレベルの再帰呼び出しが発生します。(あらかじめ完全にソートされている数列をソートしようとした場合)
//↓		あらかじめ完全にソートされている数列に限らずとも、ある程度ソートされていればいるほど、再帰呼び出しが深くなる可能性があります。
//↓
//↓		実際にどのぐらい深くなるかは数列の初期状態に依存し、qsort()を実行してみるまでわからないので、危険です。
//↓		特に、P/ECEのようなスタック領域の小さな実行環境で利用する場合に危険です。
//↓
//↓- 上述の問題を改善する方法は、以下のとおりです。
//↓	・[低位要素の並び]と[高位要素の並び]のうち、要素が少ない方を再帰呼び出しでソートします。
//↓	・要素が多い方は、現在の呼び出しレベルのまま、最初から処理を繰り返すことでソートします。
//↓  このように変更することで、再帰呼び出しの最大深さが、最悪でも log2(N) に抑えられます。
//↓- たとえば、10000要素の数列をソートする場合、改善前は最悪10000レベルの再帰呼び出しが発生していましたが、改善後は最悪でも14レベルに抑えられます。
//↓  ソート対象の現実的な長さを考慮すると、約20レベル以上の再帰呼び出しが発生する状況はほぼ有り得ないと考えられるので、P/ECEでも充分に安全です。
//↓- ただし、上述の改善はあくまでもメモリ効率(=再帰呼び出しレベル)に関するものであり、時間効率は改善されないことに注意してください。
//↓  10000要素のint配列をソートする時間を計ってみると、ランダム数列ならば約1600ミリ秒ですが、あらかじめソートされていると約15300ミリ秒かかりました。
//↓
//↓	[参考文献：P.J.プラウガー著「標準Ｃライブラリ ANSI/ISO/JIS C規格」（トッパン）初版 411～412ページ 本文]
void qsort(void* base, size_t num, size_t width, int (*compare)(const void*, const void*)) {
//{{2014/11/19追加:qsort_r()を追加し、qsort()はqsort_r()への呼び出しに変更しました。qsort_r()の宣言は、include/stdlib.hに有ります。
	qsort_r(base, num, width, (int (*)(const void*, const void*, void*))compare, NULL);	//qsort()とqsort_r()は、compare関数の形式が異なりますが、'cdecl呼出規約'ならば、qsort()形式のcompare関数において、余分な引数が無視されるので問題ありません。
}
void qsort_r(void* base, size_t num, size_t width, int (*compare)(const void*, const void*, void*), void* arg) {
//}}2014/11/19追加:qsort_r()を追加し、qsort()はqsort_r()への呼び出しに変更しました。qsort_r()の宣言は、include/stdlib.hに有ります。
	unsigned char* pv;	/* ピボットへのポインタ */
	unsigned char* lo;	/* 低位ポインタ (次に低位要素(ピボット以下の要素)を格納する位置へのポインタ) */
	unsigned char* hi;	/* 高位ポインタ (次に高位要素(ピボット超過の要素)を格納する位置へのポインタ) */
	unsigned lo_cnt;	/* 低位要素数 */
	unsigned hi_cnt;	/* 高位要素数 */

	/* 要素数が0または1ならば、何もせずに帰ります。
	 * - EPSON版は、2要素の場合も特別扱いして、単純な比較と入れ替えを行っていますが、その必要はありません。
	 *   2要素の場合も、それ以上の要素数の場合と同じ、通常の処理で対応できます。
	 */
	while(num >= 2) {
		/* ポインタと要素数を初期化します。
		 * - EPSON版は、配列の中央付近と先頭の要素を入れ替え、もともと中央付近にあった値をピボットとしています。
		 *   元からある程度昇順に並んでいると仮定して、中央付近に中央値があることを期待しているのだと思います。
		 *   しかし実際にはあまり効果が無く、むしろ余計な入れ替えの回数が増える分、性能を落としているようです。
		 *   そこでCLiP版では、単純に先頭の要素をピボットとして採用することにしました。
		 */
		pv = base;			/* ピボット     = 先頭 */
		lo = pv + width;		/* 低位ポインタ = ピボットの次 */
		hi = pv + width * (num - 1);	/* 高位ポインタ = 末尾 */
		lo_cnt = 0;			/* 低位要素数   = 0 */
		hi_cnt = 0;			/* 高位要素数   = 0 */

		/* 低位ポインタと高位ポインタが交差するまで...(即ち、ピボット以外の全要素の低位or高位が確定するまで)
		 * - (num-1)回のカウントダウンでループする方法でも同じ結果を得られますが、(lo<=hi)でループする方が、
		 *   num変数の生存期間を短くし、自動変数のレジスタ割り当て最適化がより有効に働くことが期待できます。
		 */
		//while(lo <= hi) { /* 注意!!(lo<hi)ではありません!! */
		//↓* Thu Apr 19 14:58:48 JST 2007 Naoyuki Sawa
		//↓- (num>=2)の条件より初回は必ず(lo<=hi)となるので、while(){...}よりも効率的なdo{...}while()に替えられます。
		do {
			/* 低位ポインタの位置に有る要素を、ピボットと比較します。 */
//{{2014/11/19変更:qsort_r()を追加し、qsort()はqsort_r()への呼び出しに変更しました。qsort_r()の宣言は、include/stdlib.hに有ります。
//			if((*compare)(lo, pv) <= 0) {
//↓2014/11/19変更:qsort_r()を追加し、qsort()はqsort_r()への呼び出しに変更しました。qsort_r()の宣言は、include/stdlib.hに有ります。
			if((*compare)(lo, pv, arg) <= 0) {
//}}2014/11/19変更:qsort_r()を追加し、qsort()はqsort_r()への呼び出しに変更しました。qsort_r()の宣言は、include/stdlib.hに有ります。
				/* 低位要素ならば、低位ポインタの位置に格納します。
				 * 低位ポインタの次の位置に有る要素が、次回の比較対象となります。
				 */
				lo += width;
				lo_cnt++;
			} else {
				/* 高位要素ならば、高位ポインタの位置に格納します。
				 * 高位ポインタの位置に有った要素が、次回の比較位置に移動します。
				 */
				xchg(lo, hi, width);
				hi -= width;
				hi_cnt++;
			}
		//}
		//↓* Thu Apr 19 14:58:48 JST 2007 Naoyuki Sawa
		//↓- (num>=2)の条件より初回は必ず(lo<=hi)となるので、while(){...}よりも効率的なdo{...}while()に替えられます。
		} while(lo <= hi); /* 注意!!(lo<hi)ではありません!! */

		/* 以上の処理が終わった時点で[ピボット,[低位要素の並び],[高位要素の並び]]となっています。
		 * [[低位要素の並び],ピボット,[高位要素の並び]]と入れ替えて、ピボットの位置を確定します。
		 * - 入れ替え時に低位要素の並び順が変化しますが、まだ並び順に意味はないので、大丈夫です。
		 * - もし低位要素が一つも無い場合、pv==hiとなっていて、このxchg()処理はダミーとなります。
		 */
		xchg(pv, hi, width);

		/* [低位要素の並び]と[高位要素の並び]のうち、要素が少ない方を再帰呼び出しでソートします。
		 * 要素が多い方は、現在の呼び出しレベルのまま、最初から処理を繰り返すことでソートします。
		 * - 両方を再帰呼び出しでソートすると、非常に深い再帰が発生してしまう場合を防ぐためです。
		 *   具体的には、この関数の上に書いたコメントを参照してください。
		 */
		if(lo_cnt <= hi_cnt) {
			/* [低位要素の並び]をソートします。 */
//{{2014/11/19変更:qsort_r()を追加し、qsort()はqsort_r()への呼び出しに変更しました。qsort_r()の宣言は、include/stdlib.hに有ります。
//			qsort(pv, lo_cnt, width, compare); /* 注意!![低位要素の並び]の先頭を指しているのは、loでなくpvです!! */
//↓2014/11/19変更:qsort_r()を追加し、qsort()はqsort_r()への呼び出しに変更しました。qsort_r()の宣言は、include/stdlib.hに有ります。
			qsort_r(pv, lo_cnt, width, compare, arg); /* 注意!![低位要素の並び]の先頭を指しているのは、loでなくpvです!! */
//}}2014/11/19変更:qsort_r()を追加し、qsort()はqsort_r()への呼び出しに変更しました。qsort_r()の宣言は、include/stdlib.hに有ります。
			/* [高位要素の並び]をソートします。 */
			base = lo, num = hi_cnt;           /* 注意!![高位要素の並び]の先頭を指しているのは、hiでなくloです!! */
		} else {
			/* [高位要素の並び]をソートします。 */
//{{2014/11/19変更:qsort_r()を追加し、qsort()はqsort_r()への呼び出しに変更しました。qsort_r()の宣言は、include/stdlib.hに有ります。
//			qsort(lo, hi_cnt, width, compare); /* 注意!![高位要素の並び]の先頭を指しているのは、hiでなくloです!! */
//↓2014/11/19変更:qsort_r()を追加し、qsort()はqsort_r()への呼び出しに変更しました。qsort_r()の宣言は、include/stdlib.hに有ります。
			qsort_r(lo, hi_cnt, width, compare, arg); /* 注意!![高位要素の並び]の先頭を指しているのは、hiでなくloです!! */
//}}2014/11/19変更:qsort_r()を追加し、qsort()はqsort_r()への呼び出しに変更しました。qsort_r()の宣言は、include/stdlib.hに有ります。
			/* [低位要素の並び]をソートします。 */
			base = pv, num = lo_cnt;           /* 注意!![低位要素の並び]の先頭を指しているのは、loでなくpvです!! */
		}
	}
}
#else //PIECE
// * Thu Jan 08 21:00:51 JST 2015 Naoyuki Sawa
// - qsort()とqsort_r()をアセンブラ化しました。コードサイズが24バイト小さくなり、処理速度が約4.5%早くなりました。
void qsort(void* base, size_t num, size_t width, int (*compare)(const void*, const void*));
void qsort_r(void* base, size_t num, size_t width, int (*compare)(const void*, const void*, void*), void* arg);
asm("
		.code
		.align		1
		.global		qsort
		.global		qsort_r
qsort:
	;//{{2015/01/09変更:Asm版qsort()を最適化しました。コードサイズがさらに8バイト小さくなりました。
	;//	xsub		%sp, %sp, 4		;//
	;//	xld.w		[%sp+ 0], %r8		;//[%sp+ 0] :=                        NULL
	;//	call		qsort_r			;//qsort_r(base, num, width, compare, NULL)
	;//	xadd		%sp, %sp, 4		;//
	;//	ret
	;//↓2015/01/09変更:Asm版qsort()を最適化しました。コードサイズがさらに8バイト小さくなりました。
	;// * Fri Jan 09 00:00:00 JST 2015 Naoyuki Sawa
	;// - Asm版qsort()を最適化しました。コードサイズがさらに8バイト小さくなりました。
	;//   qsort()の処理は不要で、そのままqsort_r()を実行すればokでした。理由は以下の通りです。
	;// - qsort()が呼び出された時のスタック構造は、以下の通りです:
	;//	[%sp + 0] := %pc
	;//	[%sp + 4] := (不定値)
	;//   このままqsort_r()を実行すると、(*compare)()にargとして、(不定値)が渡されることになります。
	;//   しかし、qsort()から呼び出される(*compare)()はargを受け取らないので、(不定値)で構いません。
	;//   従って、qsort()の処理は不要で、そのままqsort_r()を実行すればokです。
	;// - 厳密には、(不定値)の部分がアクセス可能なRAMだとは保証できないのですが、現実的には問題ありません。
	;//   スタックの深さが0の状態でqsort()を呼び出すような場合でもなければ、(不定値)はスタック上のRAMを指しているからです。
	;//   万一上記のようなことを行ったとしても、P/ECE環境ならば無効アドレスに対する読み出しで一般保護例外が発生するようなことは無いので、問題有りません。
		;//** no job **//
	;//}}2015/01/09変更:Asm版qsort()を最適化しました。コードサイズがさらに8バイト小さくなりました。
qsort_r:
		;//[%sp + 0] := %pc
		;//[%sp + 4] := arg
		xld.w		%r9, [%sp + 4]		;//%r9      := arg
		pushn		%r3			;//
		xsub		%sp, %sp, 16		;//
		xld.w		[%sp+ 0], %r9		;//[%sp+ 0] := arg
		xld.w		[%sp+ 4], %r12		;//[%sp+ 4] := pv = base
		xld.w		[%sp+ 8], %r14		;//[%sp+ 8] := width
		xld.w		[%sp+12], %r15		;//[%sp+12] := compare
		;//%r13      := num
		;//[%sp + 0] := arg	←┐
		;//[%sp + 4] := pv	　│
		;//[%sp + 8] := width	　│
		;//[%sp +12] := compare	　│
		;//[%sp +16] := %r0	　│
		;//[%sp +20] := %r1	　│
		;//[%sp +24] := %r2	　│
		;//[%sp +28] := %r3	　│
		;//[%sp +32] := %pc	　│
		;//[%sp +36] := arg	─┘
qsort_r_L10:						;//
		sub		%r13, 2			;//%r13 := (num-2)						┐
		jrult		qsort_r_L60		;//while(  (num-2) >= 0) {					│
		xld.w		%r0, [%sp+ 4]		;//  %r0  :=      pv						│
		xld.w		%r1, [%sp+ 8]		;//  %r1  :=           width					│
		add		%r0, %r1		;//  %r0  := lo = pv + width			!INTERLOCK!	│
		mltu.w		%r1, %r13		;//  %alr :=           width * (num-2)				│
		ld.w		%r1, %alr		;//  %r1  :=           width * (num-2)				│
		add		%r1, %r0		;//  %r1  := hi = pv + width * (num-1)				┴hiの計算方法を工夫した。
		ld.w		%r2, 0			;//  %r2  := lo_cnt = 0
		ld.w		%r3, 0			;//  %r3  := hi_cnt = 0
qsort_r_L20:						;//  do {
		xld.w		%r9, [%sp+12]		;//    %r9  :=            compare
		xld.w		%r13, [%sp+ 4]		;//    %r13 :=                         pv
		xld.w		%r14, [%sp+ 0]		;//    %r14 :=                             arg
		call.d		%r9			;//    %r10 := retval = (*compare)(lo, pv, arg)
		ld.w		%r12, %r0		;//    %r12 :=                  lo		*delay*
		xld.w		%r14, [%sp+ 8]		;//    %r14 :=           width
		cmp		%r10, 0			;//    if(retval <= 0) {
		jrgt		qsort_r_L30		;//      
		add		%r0, %r14		;//      %r0  := lo +=   width
		jp.d		qsort_r_L40		;//      
		add		%r2, 1			;//      %r2  := lo_cnt++			*delay*
qsort_r_L30:						;//    } else
		ld.w		%r12, %r0		;//      %r12 := lo
		ld.w		%r13, %r1		;//      %r13 :=     hi
		xcall.d		xchg			;//      xchg(   lo, hi, width)
		sub		%r1, %r14		;//      %r1  := hi -=   width			*delay*
		add		%r3, 1			;//      %r3  := hi_cnt++
qsort_r_L40:						;//    }
		cmp		%r0, %r1		;//  } while(lo <= hi)
		jrule		qsort_r_L20		;//  
		xld.w		%r12, [%sp+ 4]		;//  %r12 := pv
		xld.w		%r14, [%sp+ 8]		;//  %r14 :=         width
		xcall.d		xchg			;//  xchg(   pv, hi, width)
		ld.w		%r13, %r1		;//  %r13 :=     hi				*delay*
		xld.w		%r14, [%sp+ 8]		;//  %r14 :=               width
		xld.w		%r15, [%sp+12]		;//  %r15 :=                      compare
		cmp		%r2, %r3		;//  if(lo_cnt <= hi_cnt) {
		jrugt		qsort_r_L50		;//    
		xld.w		%r12, [%sp+ 4]		;//    %r12 := pv
		call.d		qsort_r			;//    qsort_r(pv, lo_cnt, width, compare, arg)
		ld.w		%r13, %r2		;//    %r13 :=     lo_cnt			*delay*
		xld.w		[%sp+ 4], %r0		;//    [%sp+ 4] := pv = lo
		jp.d		qsort_r_L10		;//    
		ld.w		%r13, %r3		;//    %r13 := num = hi_cnt			*delay*
qsort_r_L50:						;//  } else {
		ld.w		%r12, %r0		;//    %r12 := lo
		call.d		qsort_r			;//    qsort_r(lo, hi_cnt, width, compare, arg)
		ld.w		%r13, %r3		;//    %r13 :=     hi_cnt			*delay*
		jp.d		qsort_r_L10		;//    
		ld.w		%r13, %r2		;//    %r13 := num = lo_cnt			*delay*
qsort_r_L60:						;//} }
		xadd		%sp, %sp, 16		;//
		popn		%r3			;//
		ret					;//
");
#endif//PIECE

/*****************************************************************************
 *	ヒープソートとマージソートについて(2016/06/10)
 *****************************************************************************/

// * Fri Jun 10 21:42:30 JST 2016 Naoyuki Sawa
// - 既存のhsort()とmsort()を、heapsort()とmergesort()に名前変更し、/clip/include/stdlib.hで宣言するようにしました。
//   ヒープソートとマージソートは標準的なC関数ではないので、これまではclipsort.hで宣言していたのですが、
//   (ANSI/ISOやPOSIXではないものの)、FreeBSDのlibcでは、ヒープソートとマージソートが定義されている事が判ったからです。
//   FreeBSDのlibcの関数名に倣って、heapsort()とmergesort()に名前変更し、/clip/include/stdlib.hで宣言する事にしました。
//
//	□参照資料
//	「FreeBSD Library Functions Manual - QSORT」(http://www.freebsd.org/cgi/man.cgi?query=qsort&sektion=3&apropos=0&manpath=FreeBSD+10.3-RELEASE+and+Ports)
//	「FreeBSD ライブラリ関数マニュアル QSORT(3)」(http://kaworu.jpn.org/doc/FreeBSD/jman/man3/mergesort.3.php)より引用:
//	│
//	│	名称
//	│			qsort,heapsort,mergesort - ソート関数
//	│	ライブラリ
//	│			標準Cライブラリ(libc,lc)
//	│	書式
//	│			#include <stdlib.h>
//	│			void qsort(void* base, size_t nmemb, size_t size, int (*compar)(const void*, const void*));
//	│			int heapsort(void* base, size_t nmemb, size_t size, int (*compar)(const void*, const void*));
//	│			int mergesort(void* base, size_t nmemb, size_t size, int (*compar)(const void*, const void*));
//	│	解説
//	│			qsort()関数は、パーティション交換ソートの修正版、ようするにクイックソートです。
//	│			heapsort()関数は、選択ソートの修正版です。
//	│			mergesort()関数は、既に並んでいる箇所のあるデータをソートする、指数検索によるマージソートの修正版です。
//	│
//	│			qsort()関数とheapsort()関数は、baseによって初期メンバが指されているnmembオブジェクトの配列をソートします。
//	│			各オブジェクトのサイズは、sizeで指定します。
//	│			mergesort()も同じように動作しますが、sizeは``sizeof(void*)/2''より大きくなければなりません。	←←←←←当ライブラリの実装ではsize引数の制約は有りません。
//	│
//	│			配列baseの内容は、comparが指す比較関数に従って昇順でソートされます。
//	│			この関数では、比較するオブジェクトを指す、2つの引数が必要です。
//	│
//	│			比較関数は、最初の引数が次の引数より小さい場合は0より小さい整数、等しい場合は0、大きい場合は0より大きい整数を戻す必要があります。
//	│
//	│			qsort()関数とheapsort()関数は不安定です。
//	│			つまり2つのメンバが等しい場合、ソート済み配列内での順序は不定になります。
//	│			mergesort()関数は安定です。
//	│
//	│			qsort()関数は、パーティション交換ソートの一種である、C.A.R. Hoareの``クイックソート''アルゴリズムを実装しています。
//	│			とくにD.E. KnuthのアルゴリズムQを参照してください。
//	│			qsort()には、平均でO N lg Nの時間がかかります。
//	│			この実装では、メジアン選択を使用して、ON**2という最悪なケースの動作を回避します。
//	│
//	│			heapsort()関数は、選択ソートの一種である、J.W.J. Williamの``ヒープソート''アルゴリズムを実装しています。
//	│			とくにD.E. KnuthのアルゴリズムHを参照してください。
//	│			heapsort()には、最悪のケースでO N lg Nの時間がかかります。
//	│			メモリをほとんど余分に使用しないという点のみがqsort()より優れています。
//	│			一方、qsort()はメモリを割り当てませんが、再帰を使用して実装されています。
//	│
//	│			mergesort()関数では、nmemb*sizeバイトのメモリが余分に必要となります。
//	│			スペースに余裕がある場合のみに使用してください。
//	│			mergesort()は、既に並んでいる箇所のあるデータを扱うよう最適化されています。
//	│			最悪のケースの時間はO N lg Nで、最善のケースはO Nです。
//	│
//	│			通常は、heapsort()よりmergesort()の方が速く、mergesort()よりqsort()の方が高速です。
//	│			使用できるメモリ量や既に並んでいるデータ量により、この状況は変化します。
//	│	戻り値
//	│			qsort()関数は値を戻しません。
//	│
//	│			関数heapsort()およびmergesort()は、処理が成功すると値0を返します。
//	│			そうでない場合、値-1が返され、グローバル変数errnoが設定されてエラーを示します。
//	│	エラー
//	│			heapsort()関数とmergesort()関数は、以下のような場合にエラーとなります。													
//	│			[EINVAL]
//	│				size引数が0であるか、mergesort()のsize引数が``sizeof(void*)/2''より小さい場合。		←←←←←当ライブラリの実装ではsize引数の制約は有りません。		┐
//	│			[ENOMEM]																				├要するに当実装は常に0(成功)を返します。
//	│				heapsort()かmergesort()がメモリを割り当てられなかった場合。				←←←←←当ライブラリの実装では処理を返さずにエラー終了します。	┘
//	│	互換性
//	│			旧バージョンのqsort()では、比較ルーチンがqsort(3)を呼び出すことはできませんでした。
//	│			現在は呼び出せます。
//	│	関連項目
//	│			sort(1),radixsort(3)
//	│	規格
//	│			qsort()関数は、ISO/IEC 9899:1990(``ISO C89'')に適合しています。
//	│			FreeBSD 10.0 June 4, 1993 FreeBSD 10.0
//	│
//
// - 独自拡張として、heapsort_r()とmergesort_r()も追加しました。
//   これらはFreeBSDのlibcで定義されていませんが、必要だと思うからです。
//   独自拡張のbsearch_r()を追加した時と同様です。
//   詳細は、cliplibc.cの'Wed May 04 21:28:21 JST 2016'のコメントを参照して下さい。

/*****************************************************************************
 *	ヒープソート
 *****************************************************************************/

#ifndef PIECE	//{{2016/06/10:P/ECE環境ではコードサイズ節約のためにヒープソートを無効化しています。有効化する事も可能です。}}

// * Sun May 13 14:21:21 JST 2007 Naoyuki Sawa
// - 「ヒープソート - Wikipedia」の擬似コードを参考に、最適化して作成しました。
//   (URL: http://ja.wikipedia.org/w/index.php?title=%E3%83%92%E3%83%BC%E3%83%97%E3%82%BD%E3%83%BC%E3%83%88&oldid=12248591)
//   最適化した主な点は、親や子の要素を指し示すためのインデクスを使わずに、すべてポインタで処理して高速化を図ったことです。
// - ヒープソートがクィックソートよりも優れている点は、データのかたよりが速度にあまり影響しないことです。
//   クィックソートは、一様にランダムな数列のソートがいちばん早く、元々順列のソートは遅く、逆順のソートがもっとも遅いです。
//   ヒープソートは、どのケースに対してもおおよそ同じぐらいの速度で完了します。
// - しかし実際には、クィックソートよりもヒープソートを選択する場面は無いと思います。
//   クィックソートに不利な逆順のソートであっても、クィックソートの方がヒープソートよりも二倍ぐらい早く完了できるようです。
//   一様にランダムな数列のソートの場合は、50倍以上の速度差が付いてしまいます。
//   (Windowsで実験した場合はここまでの速度差は付かなかったのですが、P/ECEではなぜか上述のような大差になってしまいました。)
// - 以下のコードは、正しく動くことを確認済みですが、実際に使うことは無いと思います。
//   リンクサイズが増えないようにコメントアウトしてありますので、もし使う場合はアプリケーションにコピーして使ってください。
// * Fri Jun 10 21:42:30 JST 2016 Naoyuki Sawa
// - 既存のhsort()とmsort()を、heapsort()とmergesort()に名前変更し、/clip/include/stdlib.hで宣言するようにしました。
//   これまでは、ヒープソートの実装例はコメントアウトしていましたが、Win32環境では有効化しました。
//   ただし、実際に必要になる事は無いと思うので、P/ECE環境ではコードサイズ節約のために無効化したままとしました。
//   今回、ヒープソートを有効化した理由は、あくまでもWin32環境でのテストのためだけです。

//{{2016/06/10変更:既存のhsort()とmsort()を、heapsort()とmergesort()に名前変更し、/clip/include/stdlib.hで宣言するようにしました。
//static void make_heap(unsigned char* base, unsigned char* last, size_t width, int (*compare)(const void*, const void*), unsigned char* parent) {
//↓2016/06/10変更:既存のhsort()とmsort()を、heapsort()とmergesort()に名前変更し、/clip/include/stdlib.hで宣言するようにしました。
static void make_heap(unsigned char* base, unsigned char* last, size_t width, int (*compare)(const void*, const void*, void*), unsigned char* parent, void* arg) {
//}}2016/06/10変更:既存のhsort()とmsort()を、heapsort()とmergesort()に名前変更し、/clip/include/stdlib.hで宣言するようにしました。
	unsigned char *child1, *child2;
	/* 左の子が有れば... */
	child1 = (base + (parent - base) * 2) + width;
	if(child1 <= last) {
		/* 左の子のツリーをソートします。 */
//{{2016/06/10変更:既存のhsort()とmsort()を、heapsort()とmergesort()に名前変更し、/clip/include/stdlib.hで宣言するようにしました。
//		make_heap(base, last, width, compare, child1);
//↓2016/06/10変更:既存のhsort()とmsort()を、heapsort()とmergesort()に名前変更し、/clip/include/stdlib.hで宣言するようにしました。
		make_heap(base, last, width, compare, child1, arg);
//}}2016/06/10変更:既存のhsort()とmsort()を、heapsort()とmergesort()に名前変更し、/clip/include/stdlib.hで宣言するようにしました。
		/* 右の子が有れば... */
		child2 = child1 + width;
		if(child2 <= last) {
			/* 右の子のツリーをソートします。 */
//{{2016/06/10変更:既存のhsort()とmsort()を、heapsort()とmergesort()に名前変更し、/clip/include/stdlib.hで宣言するようにしました。
//			make_heap(base, last, width, compare, child2);
//↓2016/06/10変更:既存のhsort()とmsort()を、heapsort()とmergesort()に名前変更し、/clip/include/stdlib.hで宣言するようにしました。
			make_heap(base, last, width, compare, child2, arg);
//}}2016/06/10変更:既存のhsort()とmsort()を、heapsort()とmergesort()に名前変更し、/clip/include/stdlib.hで宣言するようにしました。
			/* child1が大きい方の子を指すようにします。 */
//{{2016/06/10変更:既存のhsort()とmsort()を、heapsort()とmergesort()に名前変更し、/clip/include/stdlib.hで宣言するようにしました。
//			if((*compare)(child1, child2) < 0) {
//↓2016/06/10変更:既存のhsort()とmsort()を、heapsort()とmergesort()に名前変更し、/clip/include/stdlib.hで宣言するようにしました。
			if((*compare)(child1, child2, arg) < 0) {
//}}2016/06/10変更:既存のhsort()とmsort()を、heapsort()とmergesort()に名前変更し、/clip/include/stdlib.hで宣言するようにしました。
				child1 = child2;
			}
		}
		/* 大きい方の子が親よりも大きければ... */
//{{2016/06/10変更:既存のhsort()とmsort()を、heapsort()とmergesort()に名前変更し、/clip/include/stdlib.hで宣言するようにしました。
//		if((*compare)(parent, child1) < 0) {
//↓2016/06/10変更:既存のhsort()とmsort()を、heapsort()とmergesort()に名前変更し、/clip/include/stdlib.hで宣言するようにしました。
		if((*compare)(parent, child1, arg) < 0) {
//}}2016/06/10変更:既存のhsort()とmsort()を、heapsort()とmergesort()に名前変更し、/clip/include/stdlib.hで宣言するようにしました。
			/* 大きい方の子と親を入れ替えます。 */
			xchg(parent, child1, width);
			//make_heap(base, last, width, compare, child1);
			//→子のツリーのソートは次回に行うので不要です。
		}
	}
}

//{{2016/06/10変更:既存のhsort()とmsort()を、heapsort()とmergesort()に名前変更し、/clip/include/stdlib.hで宣言するようにしました。
//void hsort(void* _base, size_t num, size_t width, int (*compare)(const void*, const void*)) {
//↓2016/06/10変更:既存のhsort()とmsort()を、heapsort()とmergesort()に名前変更し、/clip/include/stdlib.hで宣言するようにしました。
int heapsort(void* _base, size_t num, size_t width, int (*compare)(const void*, const void*)) {	//FreeBSD 10.0 June 4, 1993 FreeBSD 10.0
	return heapsort_r(_base, num, width, (int (*)(const void*, const void*, void*))compare, NULL);
}
int heapsort_r(void* _base, size_t num, size_t width, int (*compare)(const void*, const void*, void*), void* arg) {	//独自拡張
//}}2016/06/10変更:既存のhsort()とmsort()を、heapsort()とmergesort()に名前変更し、/clip/include/stdlib.hで宣言するようにしました。
	unsigned char* base = _base;
	unsigned char* last = base + (width * (num - 1));
	while(base < last) {
		/* ツリー全体を降順にソートします。 */
//{{2016/06/10変更:既存のhsort()とmsort()を、heapsort()とmergesort()に名前変更し、/clip/include/stdlib.hで宣言するようにしました。
//		make_heap(base, last, width, compare, base);
//↓2016/06/10変更:既存のhsort()とmsort()を、heapsort()とmergesort()に名前変更し、/clip/include/stdlib.hで宣言するようにしました。
		make_heap(base, last, width, compare, base, arg);
//}}2016/06/10変更:既存のhsort()とmsort()を、heapsort()とmergesort()に名前変更し、/clip/include/stdlib.hで宣言するようにしました。
		/* 最大要素をツリーの末尾に切り離します。 */
		xchg(base, last, width);
		last -= width;
	}
//{{2016/06/10追加:既存のhsort()とmsort()を、heapsort()とmergesort()に名前変更し、/clip/include/stdlib.hで宣言するようにしました。
	return 0;
//}}2016/06/10追加:既存のhsort()とmsort()を、heapsort()とmergesort()に名前変更し、/clip/include/stdlib.hで宣言するようにしました。
}

#endif//PIECE	//{{2016/06/10:P/ECE環境ではコードサイズ節約のためにヒープソートを無効化しています。有効化する事も可能です。}}

/*****************************************************************************
 *	マージソート
 *****************************************************************************/

/* * Mon Oct 18 15:12:52 JST 2010 Naoyuki Sawa
 * - msort()関数を実装する際に参考にしたWebサイトを、テキスト化して記録しておくことにします。
 *   『（ＭＡＸ上における）アルゴリズム的問題におけるユーザーインターフェースの改良』の中の、
 *   　(http://www.th.cs.meiji.ac.jp/researches/2005/omoto/index.html)
 *   「8. マージソート」のページから、引用させていただきました。
 *   　(http://www.th.cs.meiji.ac.jp/researches/2005/omoto/mergesort.html)
 * - 一般的にマージソートは、再帰を用いて実装するのですけれど、上記サイトのサンプルソースは、再帰を用いないで実装しています。
 *   また、メモリ確保の仕組みも判りやすく、リソースの限られたマイコン向きの実装だと思います。
 * 
 * 	┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
 * 	┃マージソート														┃
 * 	┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛
 * 
 * 	┌───────────────────────────────────────────────────────────┐
 * 	│マージソートとは？													│
 * 	└───────────────────────────────────────────────────────────┘
 * 
 * 	マージソートは、データ列を二つに分割し、それらを再びマージする（併合する）時に大きさの順にデータを取り出して並べる。
 * 	分割されたそれぞれが大きさの順に並んでいればマージしたものも大きさの順に並ぶので、分割されたものに対しても
 * 	マージソート（や、その他のソート…例えば分割後のデータ数が２個とか３個の場合は単純な比較で済む）を再帰的に適用すれば良い。
 * 	ソートが比較的高速で、同位のものの順序が保存されるという性質がある。
 * 
 * 	┌───────────────────────────────────────────────────────────┐
 * 	│考え方														│
 * 	└───────────────────────────────────────────────────────────┘
 * 
 * 	感覚としては、「２本の配列を用意し、分割されたノードずつソートを行う」といった感じである。
 * 
 * 	今、次のような配列が存在するとする。
 * 
 * 		┌─┬─┬─┬─┬─┬─┬─┬─┐
 * 		│４│１│２│６│９│７│３│８│
 * 		└─┴─┴─┴─┴─┴─┴─┴─┘
 * 		┌─┬─┬─┬─┬─┬─┬─┬─┐
 * 		│　│　│　│　│　│　│　│　│
 * 		└─┴─┴─┴─┴─┴─┴─┴─┘
 * 
 * 	これを細かく分割していくのだが、まず、２で分割する。
 * 
 * 		┏━┯━┳━┯━┳━┯━┳━┯━┓
 * 		┃４│１┃２│６┃９│７┃３│８┃
 * 		┗━┷━┻━┷━┻━┷━┻━┷━┛
 * 		┌─┬─┬─┬─┬─┬─┬─┬─┐
 * 		│　│　│　│　│　│　│　│　│
 * 		└─┴─┴─┴─┴─┴─┴─┴─┘
 * 
 * 	分割された範囲内で、もう１つ用意された配列にソートした値を入れていく。
 * 
 * 		┏━┯━┳━┯━┳━┯━┳━┯━┓
 * 		┃４│１┃２│６┃９│７┃３│８┃
 * 		┗━┷━┻━┷━┻━┷━┻━┷━┛
 * 		↓　　　↓　　　↓　　　↓　　　↓
 * 		┌─┬─┬─┬─┬─┬─┬─┬─┐
 * 		│１│４│２│６│７│９│３│８│
 * 		└─┴─┴─┴─┴─┴─┴─┴─┘
 * 
 * 	次に新しく値を保持した配列を分割するのだが、２×２＝４で分割する。
 * 
 * 		┌─┬─┬─┬─┬─┬─┬─┬─┐
 * 		│４│１│２│６│９│７│３│８│
 * 		└─┴─┴─┴─┴─┴─┴─┴─┘
 * 		┏━┯━┯━┯━┳━┯━┯━┯━┓
 * 		┃１│４│２│６┃７│９│３│８┃
 * 		┗━┷━┷━┷━┻━┷━┷━┷━┛
 * 
 * 	分割された範囲内で、元の配列にソートした値を入れていく。
 * 
 * 		┌─┬─┬─┬─┬─┬─┬─┬─┐
 * 		│１│２│４│６│３│７│８│９│
 * 		└─┴─┴─┴─┴─┴─┴─┴─┘
 * 		↑　　　　　　　↑　　　　　　　↑
 * 		┏━┯━┯━┯━┳━┯━┯━┯━┓
 * 		┃１│４│２│６┃７│９│３│８┃
 * 		┗━┷━┷━┷━┻━┷━┷━┷━┛
 * 
 * 	次に新しく値を保持した元の配列を分割する。４×２＝８で分割する。
 * 
 * 		┏━┯━┯━┯━┯━┯━┯━┯━┓
 * 		┃１│２│４│６│３│７│８│９┃
 * 		┗━┷━┷━┷━┷━┷━┷━┷━┛
 * 		┌─┬─┬─┬─┬─┬─┬─┬─┐
 * 		│１│４│２│６│７│９│３│８│
 * 		└─┴─┴─┴─┴─┴─┴─┴─┘
 * 
 * 	分割された範囲内で、もう１つ用意された配列にソートした値を入れていく。
 * 
 * 		┏━┯━┯━┯━┯━┯━┯━┯━┓
 * 		┃１│２│４│６│３│７│８│９┃
 * 		┗━┷━┷━┷━┷━┷━┷━┷━┛
 * 		↓　　　　　　　　　　　　　　　↓
 * 		┌─┬─┬─┬─┬─┬─┬─┬─┐
 * 		│１│２│３│４│６│７│８│９│
 * 		└─┴─┴─┴─┴─┴─┴─┴─┘
 * 
 * 	次に新しく値を保持した配列を分割するのだが、要素数が８であるので８で分割する。
 * 
 * 		┌─┬─┬─┬─┬─┬─┬─┬─┐
 * 		│１│２│４│６│３│７│８│９│
 * 		└─┴─┴─┴─┴─┴─┴─┴─┘
 * 		┏━┯━┯━┯━┯━┯━┯━┯━┓
 * 		┃１│２│３│４│６│７│８│９┃
 * 		┗━┷━┷━┷━┷━┷━┷━┷━┛
 * 
 * 	分割された範囲内で、元の配列にソートした値を入れていく。
 * 
 * 		┌─┬─┬─┬─┬─┬─┬─┬─┐
 * 		│１│２│３│４│６│７│８│９│
 * 		└─┴─┴─┴─┴─┴─┴─┴─┘
 * 		↑　　　　　　　　　　　　　　　↑
 * 		┏━┯━┯━┯━┯━┯━┯━┯━┓
 * 		┃１│２│３│４│６│７│８│９┃
 * 		┗━┷━┷━┷━┷━┷━┷━┷━┛
 * 
 * 	元の配列を分割する際に、分割の範囲が要素数を超えている場合、ソート終了となる。
 * 
 * 	以上がマージソートの流れである。
 * 
 * 	マージソートのアルゴリズムは以下のように記述できる。
 * 
 * 		void sort() {							// マージソート(昇順)
 * 			int seqsize = 1;					// 大きさの初期値を1とする
 * 			while (seqsize < length) {				// ソートする大きさがデータよりも小さければ，
 * 				mergeseqs(seqsize, a, b);			// 指定された大きさで配列aから配列bへマージ
 * 				mergeseqs(2 * seqsize, b, a);			// 指定された2倍の大きさで配列bから配列aへマージ
 * 				seqsize = 4 * seqsize;				// 大きさを4倍にする．
 * 			}
 * 		}
 * 
 * 		void mergeseqs(int size, int from [], int to []) {		// from配列からマージでto配列を作る
 * 			int i, j, k, iend, jend;
 * 			int start = 0;
 * 			while (start < length) {
 * 				i = start;					// i: マージされる列の始まり
 * 				j = start + size;				// j: マージされるもう一つの列の始まり
 * 				k = start;					// k: マージした結果の列の始まり
 * 				iend = Math.min(start + size, length);		// 対象とする配列要素の最後を計算
 * 				jend = Math.min(start + 2 * size, length);	// 対象とする配列要素の最後を計算
 * 				while (i < iend && j < jend) {			// マージする列が2つある場合
 * 					if (from[i] <= from[j]) {
 * 						to[k] = from[i];		// 小さい値の方をコピー
 * 						i++;
 * 						k++;
 * 					} else {
 * 						to[k] = from[j];		// 小さい値の方をコピー
 * 						j++;
 * 						k++;
 * 					}
 * 				}
 * 				while (i < iend) {				// マージする列が1つだけの場合
 * 					to[k] = from[i];			// 値のコピー
 * 					i++;
 * 					k++;
 * 				}
 * 				while (j < jend) {				// マージする列が1つだけの場合
 * 					to[k] = from[j];			// 値のコピー
 * 					j++;
 * 					k++;
 * 				}
 * 				start = start + 2 * size;			// 新たな開始点を設定
 * 			}
 * 		}
 * 
 * 	※プログラム引用
 * 	Lecture of Computer Programming I by Hiroshi Ichiji 
 * 	(http://lecture.ecc.u-tokyo.ac.jp/~cichiji/cp-01/cp-01.html)
 */

#if 0
//{{2016/06/10変更:既存のhsort()とmsort()を、heapsort()とmergesort()に名前変更し、/clip/include/stdlib.hで宣言するようにしました。
//static void mergeseqs(size_t size, const unsigned char* from, unsigned char* to, size_t num, size_t width, int (*compare)(const void*, const void*)) {
//↓2016/06/10変更:既存のhsort()とmsort()を、heapsort()とmergesort()に名前変更し、/clip/include/stdlib.hで宣言するようにしました。
//□素直な実装
static void mergeseqs(size_t size, const unsigned char* from, unsigned char* to, size_t num, size_t width, int (*compare)(const void*, const void*, void*), void* arg) {
//}}2016/06/10変更:既存のhsort()とmsort()を、heapsort()とmergesort()に名前変更し、/clip/include/stdlib.hで宣言するようにしました。
	size_t i, j, k, iend, jend;
	for(jend = 0; jend < num; /** no job **/) {
		i    = k = jend;
		iend = j = i + size;
		jend     = j + size;
		if(iend > num) {
			iend = num;
		}
		if(jend > num) {
			jend = num;
		}
		while((i < iend) && (j < jend)) {
//{{2016/06/10変更:既存のhsort()とmsort()を、heapsort()とmergesort()に名前変更し、/clip/include/stdlib.hで宣言するようにしました。
//			if((*compare)(&from[i * width], &from[j * width]) <= 0) {
//↓2016/06/10変更:既存のhsort()とmsort()を、heapsort()とmergesort()に名前変更し、/clip/include/stdlib.hで宣言するようにしました。
			if((*compare)(&from[i * width], &from[j * width], arg) <= 0) {
//}}2016/06/10変更:既存のhsort()とmsort()を、heapsort()とmergesort()に名前変更し、/clip/include/stdlib.hで宣言するようにしました。
				memcpy(&to[k++ * width], &from[i++ * width], width);
			} else {
				memcpy(&to[k++ * width], &from[j++ * width], width);
			}
		}
		if(i < iend) {
			memcpy(&to[k * width], &from[i * width], (iend - i) * width);
		}
		if(j < jend) {
			memcpy(&to[k * width], &from[j * width], (jend - j) * width);
		}
	}
}
//{{2016/06/10変更:既存のhsort()とmsort()を、heapsort()とmergesort()に名前変更し、/clip/include/stdlib.hで宣言するようにしました。
//void msort(void* base, size_t num, size_t width, int (*compare)(const void*, const void*)) {
//↓2016/06/10変更:既存のhsort()とmsort()を、heapsort()とmergesort()に名前変更し、/clip/include/stdlib.hで宣言するようにしました。
int mergesort(void* base, size_t num, size_t width, int (*compare)(const void*, const void*)) {	//FreeBSD 10.0 June 4, 1993 FreeBSD 10.0
	return mergesort_r(base, num, width, (int (*)(const void*, const void*, void*))compare, NULL);
}
int mergesort_r(void* base, size_t num, size_t width, int (*compare)(const void*, const void*, void*), void* arg) {	//独自拡張
//}}2016/06/10変更:既存のhsort()とmsort()を、heapsort()とmergesort()に名前変更し、/clip/include/stdlib.hで宣言するようにしました。
	void *a = base, *b;
	size_t seqsize;
	b = malloc(num * width);
	if(!b) { DIE(); }
	for(seqsize = 1; seqsize < num; /** no job **/) {
//{{2016/06/10変更:既存のhsort()とmsort()を、heapsort()とmergesort()に名前変更し、/clip/include/stdlib.hで宣言するようにしました。
//		mergeseqs(seqsize, a, b, num, width, compare);
//↓2016/06/10変更:既存のhsort()とmsort()を、heapsort()とmergesort()に名前変更し、/clip/include/stdlib.hで宣言するようにしました。
		mergeseqs(seqsize, a, b, num, width, compare, arg);
//}}2016/06/10変更:既存のhsort()とmsort()を、heapsort()とmergesort()に名前変更し、/clip/include/stdlib.hで宣言するようにしました。
		seqsize *= 2;
//↓2016/06/10変更:既存のhsort()とmsort()を、heapsort()とmergesort()に名前変更し、/clip/include/stdlib.hで宣言するようにしました。
//		mergeseqs(seqsize, b, a, num, width, compare);
//↓2016/06/10変更:既存のhsort()とmsort()を、heapsort()とmergesort()に名前変更し、/clip/include/stdlib.hで宣言するようにしました。
		mergeseqs(seqsize, b, a, num, width, compare, arg);
//↓2016/06/10変更:既存のhsort()とmsort()を、heapsort()とmergesort()に名前変更し、/clip/include/stdlib.hで宣言するようにしました。
		seqsize *= 2;
	}
	free(b);
//{{2016/06/10追加:既存のhsort()とmsort()を、heapsort()とmergesort()に名前変更し、/clip/include/stdlib.hで宣言するようにしました。
	return 0;
//}}2016/06/10追加:既存のhsort()とmsort()を、heapsort()とmergesort()に名前変更し、/clip/include/stdlib.hで宣言するようにしました。
}
#else	//↓{{2016/06/10変更:アセンブラ化しました。コードサイズが72バイト小さくなりました。速度も多少早くなったと思いますが、元々malloc/freeを使っているので全体としての速度はあまり変わらないと思います。}}
//□アセンブラ化するための準備として変更した実装。
// - '素直な実装'ではインデクスベースであった処理を、'アセンブラ化するための準備として変更した実装'ではポインタベースの処理に変更しました。
//   変更した理由は、アセンブラ化する時に、必要なレジスタ数を減らすための準備です。
// - 'アセンブラ化するための準備として変更した実装'も、アルゴリズム自体は'素直な実装'と同じです。
//   'アセンブラ化するための準備として変更した実装'のコードは、読んで判りづらいと思うので、アルゴリズム自体は'素直な実装'を参照して下さい。
#ifndef PIECE
static void mergeseqs(size_t size, const unsigned char* from, unsigned char* to, size_t num, size_t width, int (*compare)(const void*, const void*, void*), void* arg) {
	const int from_to = to - from;							//これ以降、toは不要。		これ以降、from_toは変化しない。		from_toはメモリに置け。
	const unsigned char *from_num, *from_i, *from_iend, *from_j, *from_jend;
	unsigned char* to_k;
	size *= width;									//				これ以降、sizeは変化しない。		sizeはメモリに置け。
	num  *= width;									//				これ以降、numは変化しない。
	from_num = &from[num];								//これ以降、numは不要。		これ以降、from_numは変化しない。	from_numはメモリに置いても良いが、レジスタが一つ余ったので、from_numをレジスタに置く事にした。メモリに置いても良い変数のうち、from_numをレジスタに置くのが一番効果が高いと思うからです。
	for(from_jend = from; from_jend < from_num; /** no job **/) {			//これ以降、fromは不要。
		from_i    = from_jend;							//													from_iはレジスタに置け。
		from_iend = from_i + size;
		from_j    = from_iend;							//													from_jはレジスタに置け。
		from_jend = from_j + size;						//これ以降、sizeは不要。
		if(from_iend > from_num) {
			from_iend = from_num;						//				これ以降、from_iendは変化しない。	from_iendはメモリに置け。
		}
		if(from_jend > from_num) {
			from_jend = from_num;						//				これ以降、from_jendは変化しない。	from_jendはメモリに置け。
		}
		to_k = (unsigned char*)(from_i + from_to);				//													to_kはレジスタに置け。
		while((from_i < from_iend) && (from_j < from_jend)) {
			if((*compare)(from_i, from_j, arg) <= 0) {
				memcpy(to_k, from_i, width);
				from_i += width;
			} else {
				memcpy(to_k, from_j, width);
				from_j += width;
			}
			to_k += width;
		}
		if(from_iend > from_i) {
			memcpy(to_k, from_i, from_iend - from_i);
		}
		if(from_jend > from_j) {
			memcpy(to_k, from_j, from_jend - from_j);
		}
	}
}
int mergesort(void* base, size_t num, size_t width, int (*compare)(const void*, const void*)) {	//FreeBSD 10.0 June 4, 1993 FreeBSD 10.0
	return mergesort_r(base, num, width, (int (*)(const void*, const void*, void*))compare, NULL);
}
int mergesort_r(void* base, size_t num, size_t width, int (*compare)(const void*, const void*, void*), void* arg) {	//独自拡張
	void *a = base, *b;
	size_t seqsize;
	b = malloc(num * width);
	if(!b) { DIE(); }
	for(seqsize = 1; seqsize < num; /** no job **/) {
		mergeseqs(seqsize, a, b, num, width, compare, arg);
		seqsize *= 2;
		mergeseqs(seqsize, b, a, num, width, compare, arg);
		seqsize *= 2;
	}
	free(b);
	return 0;
}
#else //PIECE
/*static*/ void mergeseqs(size_t size, const unsigned char* from, unsigned char* to, size_t num, size_t width, int (*compare)(const void*, const void*, void*), void* arg);
int mergesort(void* base, size_t num, size_t width, int (*compare)(const void*, const void*));	//FreeBSD 10.0 June 4, 1993 FreeBSD 10.0
int mergesort_r(void* base, size_t num, size_t width, int (*compare)(const void*, const void*, void*), void* arg);	//独自拡張
asm("
		.global		mergesort
		.global		mergesort_r
mergesort:
		;//[%sp+0] := retp
		;//[%sp+4] := (arg = 不定値)
mergesort_r:
		pushn		%r3
		xsub		%sp, %sp, 12
		;//%r0      := (a)
		;//%r1      := (num)
		;//%r2      := (b)
		;//%r3      := (seqsize)
		;//%r12     := base
		;//%r13     := num
		;//%r14     := width
		;//%r15     := compare
		;//[%sp+0]  := (width)		┐
		;//[%sp+4]  := (compare)	├mergeseqs()への第5,第6,第7引数になります。
		;//[%sp+8]  := (arg)		┘
		;//[%sp+12] := %r0
		;//[%sp+16] := %r1
		;//[%sp+20] := %r2
		;//[%sp+24] := %r3
		;//[%sp+28] := retp
		;//[%sp+32] := arg
		ld.w		%r0, %r12			;//%r0     := a = base
		ld.w		%r1, %r13			;//%r1     := num
		xld.w		%r9, [%sp+32]			;//%r9     := arg
		xld.w		[%sp+0], %r14			;//[%sp+0] := width
		xld.w		[%sp+4], %r15			;//[%sp+4] := compare
		xld.w		[%sp+8], %r9			;//[%sp+8] := arg
		mlt.w		%r13, %r14			;//%alr    :=            num * width
		xcall.d		malloc				;//%r10    := b = malloc(num * width)
		ld.w		%r12, %alr			;//%r12    :=            num * width				*delay*
		cmp		%r10, 0				;//if(       !b) { DIE() }
		jreq		mergesort_r_DIE			;//
		ld.w		%r2, %r10			;//%r2     := b
		ld.w		%r3, 1				;//%r3     :=  seqsize = 1
		;//- - - - - - - - - - - - - - - - - - - - - - -;//
mergesort_r_LOOP:						;//for(;;) {
		cmp		%r3, %r1			;//  if(seqsize >= num) { goto EXIT }
		jrge		mergesort_r_EXIT		;//  
		ld.w		%r12, %r3			;//  %r12 :=   seqsize
		ld.w		%r13, %r0			;//  %r13 :=            a
		ld.w		%r14, %r2			;//  %r14 :=               b
		call.d		mergeseqs			;//  mergeseqs(seqsize, a, b, num, width, compare, arg)
		ld.w		%r15, %r1			;//  %r15 :=                  num				*delay*
		add		%r3, %r3			;//  %r3  :=   seqsize *= 2
		ld.w		%r12, %r3			;//  %r12 :=   seqsize
		ld.w		%r13, %r2			;//  %r13 :=            b
		ld.w		%r14, %r0			;//  %r14 :=               a
		call.d		mergeseqs			;//  mergeseqs(seqsize, b, a, num, width, compare, arg)
		ld.w		%r15, %r1			;//  %r15 :=                  num				*delay*
		jp.d		mergesort_r_LOOP		;//}
		add		%r3, %r3			;//  %r3  :=   seqsize *= 2					*delay*
mergesort_r_EXIT:						;//
		;//- - - - - - - - - - - - - - - - - - - - - - -;//
		xcall.d		free				;//free(     b)
		ld.w		%r12, %r2			;//%r12   := b							*delay*
		xadd		%sp, %sp, 12			;//
		popn		%r3				;//
		ret.d						;//return    0
		ld.w		%r10, 0				;//%r10   := 0							*delay*
		;//---------------------------------------------;//
mergeseqs:
		pushn		%r3
		xsub		%sp, %sp, 16
		;//%r0      := (from_num)
		;//%r1      := (from_i)
		;//%r2      := (from_j)
		;//%r3      := (to_k)
		;//%r12     := size
		;//%r13     := from	────────────────────────────────────────────────┐
		;//%r14     := to													│
		;//%r15     := num													│
		;//[%sp+0]  := (from_to)												│
		;//[%sp+4]  := (size)													│
		;//[%sp+8]  := (from_iend)												│
		;//[%sp+12] := (from_jend)												│
		;//[%sp+16] := %r0													│
		;//[%sp+20] := %r1													│
		;//[%sp+24] := %r2													│
		;//[%sp+28] := %r3													│
		;//[%sp+32] := retp													│
		;//[%sp+36] := width													│
		;//[%sp+40] := compare													│
		;//[%sp+44] := arg													│
		xld.w		%r9, [%sp+36]			;//%r9      :=         width						│
		sub		%r14, %r13			;//%r14     := from_to = to - from					│
		xld.w		[%sp+0], %r14			;//[%sp+0]  := from_to							│
		mlt.w		%r12, %r9			;//%alr     := size *= width						│
		ld.w		%r12, %alr			;//%r12     := size							│
		xld.w		[%sp+4], %r12			;//[%sp+4]  := size							│
		mlt.w		%r15, %r9			;//%alr     :=                  num *= width				│
		ld.w		%r0, %alr			;//%r0      :=                  num					│
		add		%r0, %r13			;//%r0      := from_num = &from[num]					│
		;//- - - - - - - - - - - - - - - - - - - - - - -;//%r13     := from_jend = from						│
mergeseqs_LOOP1:						;//for(;;) {								│
		cmp		%r13, %r0			;//  if(from_jend >= from_num) { goto EXIT1 }	←───────────┤
		jruge		mergeseqs_EXIT1			;//  									│
		xld.w		%r3, [%sp+0]			;//  %r3      :=                 from_to				│
		xld.w		%r4, [%sp+4]			;//  %r4      :=                      size				│
		ld.w		%r1, %r13			;//  %r1      := from_i    = from_jend					│
		ld.w		%r5, %r1			;//  %r5      :=             from_i					│
		add		%r5, %r4			;//  %r5      := from_iend = from_i + size				│
		ld.w		%r2, %r5			;//  %r2      := from_j    = from_iend					│
		ld.w		%r6, %r2			;//  %r6      :=             from_j					│
		add		%r6, %r4			;//  %r6      := from_jend = from_j + size				│
		cmp		%r5, %r0			;//  if(from_iend > from_num) {						│
		jrule.d		3				;//    									│
		 cmp		%r6, %r0			;//   ←────────────────────┐			│	*delay*
		 ld.w		%r5, %r0			;//    %r5      := from_iend = from_num }	│			│
	;//	cmp		%r6, %r0			;//  if(from_jend > from_num) {		────┘			│
		jrule.d		3				;//    									│
		 add		%r3, %r1			;//  %r3      := to_k = from_i + from_to				│	*delay*
		 ld.w		%r6, %r0			;//    %r6      := from_jend = from_num }				│
		xld.w		[%sp+8], %r5			;//  [%sp+8]  := from_iend						│
		xld.w		[%sp+12], %r6			;//  [%sp+12] := from_jend						│
		;//- - - - - - - - - - - - - - - - - - - - - - -;//									│
mergeseqs_LOOP2:						;//  for(;;) {								│
		xld.w		%r14, [%sp+8]			;//    %r14     := from_iend	────────────────┐	│
		xld.w		%r9, [%sp+12]			;//    %r9      := from_jend					│	│	*anti-interlock*
		cmp		%r1, %r14			;//    if(from_i >= from_iend) { goto EXIT2 }			│	│
		jruge		mergeseqs_EXIT2			;//    								│	│
		cmp		%r2, %r9			;//    if(from_j >= from_jend) { goto EXIT2 }			│	│
		jruge		mergeseqs_EXIT2			;//    								│	│
		xld.w		%r9, [%sp+40]			;//    %r9      :=   compare					│	│
		xld.w		%r14, [%sp+44]			;//    %r14     :=                            arg		│	│
		ld.w		%r12, %r1			;//    %r12     :=            from_i				│	│
		call.d		%r9				;//    %r10     := (*compare)(from_i, from_j, arg)		│	│
		ld.w		%r13, %r2			;//    %r13     :=                    from_j			│	│	*delay*
		xld.w		%r14, [%sp+36]			;//    %r14     :=                 width			│	│
		cmp		%r10, 0				;//    if(         (*compare)(from_i, from_j, arg) <= 0) {	│	│
		jrgt.d		5		;//─┐		;//      							│	│
		 ld.w		%r12, %r3	;//　│		;//    %r12     := to_k						│	│	*delay*
		 ld.w		%r13, %r1	;//　│		;//      %r13     :=     from_i		────────┐	│	│
		 jp.d		4		;//　│	;//─┐	;//    } else {						│	│	│
		 add		%r1, %r14	;//　│	;//　│	;//      %r1      :=     from_i += width		│	│	│	*delay*
		ld.w		%r13, %r2	;//←┘	;//　│	;//      %r13     :=     from_j		────────┤	│	│
		add		%r2, %r14		;//　│	;//      %r2      :=     from_j += width		│	│	│
		call.d		mergeseqs_MEMCPY	;//←┘	;//      memcpy(   to_k, from_?,   width)	←───┘	│	│
		add		%r3, %r14			;//    %r3      := to_k         += width			│	│	*delay*
		jp		mergeseqs_LOOP2			;//  }								│	│	*anti-interlock*
mergeseqs_EXIT2:						;//  								│	│
		;//- - - - - - - - - - - - - - - - - - - - - - -;//								│	│
	;//	xld.w		%r14, [%sp+8]			;//  %r14     :=                 from_iend	←───────┘	│
		sub		%r14, %r1			;//  %r14     :=                 from_iend - from_i			│
		jrule		4				;//  if(                         from_iend > from_i) {			│
		 ld.w		%r12, %r3			;//    %r12     := to_k							│
		 call.d		mergeseqs_MEMCPY		;//    memcpy(     to_k, from_i, from_iend - from_i) }			│
		 ld.w		%r13, %r1			;//    %r13     :=       from_i						│	*delay*
		xld.w		%r14, [%sp+12]			;//  %r14     :=                 from_jend				│
		sub		%r14, %r2			;//  if(                         from_jend - from_j) {			│	!INTERLOCK!
		jrule		4				;//  if(                         from_jend > from_j) {			│
		 ld.w		%r12, %r3			;//    %r12     := to_k							│
		 call.d		mergeseqs_MEMCPY		;//    memcpy(     to_k, from_j, from_jend - from_j) }			│
		 ld.w		%r13, %r2			;//    %r13     :=       from_j						│	*delay*
		xld.w		%r13, [%sp+12]			;//  %r13     := from_jend	────────────────────┘
		jp		mergeseqs_LOOP1			;//}
mergeseqs_EXIT1:						;//
		;//- - - - - - - - - - - - - - - - - - - - - - -;//
		xadd		%sp, %sp, 16			;//
		popn		%r3				;//
		ret						;//
		;//- - - - - - - - - - - - - - - - - - - - - - -;//
mergeseqs_MEMCPY:						;//
		xjp		memcpy				;//当関数にはmemcpy呼び出しが3か所有るので、それらの3か所を1命令(jp)にして、ここに追加の2命令(xjp)を設ける方が、全体としては1命令の節約になる。
");
static void __attribute__((noreturn,unused))/*asmブロックから参照*/ mergesort_r_DIE() { DIE(); }
#endif//PIECE
#endif

/*****************************************************************************
 *	テストスイート
 *****************************************************************************/
#if 0
static void test_subr(void (*sort_r)(void*, size_t, size_t, int (*)(const void*,const void*, void*), void*));	//前方参照
//2016/06/10現在、Win32環境とP/ECE環境で正常に動作する事を確認しました。
void test() {
	int i;
	for(i = 0; i < 100; i++) { test_subr(qsort_r); }											//qsort_rのテスト
  #ifndef PIECE	//{{2016/06/10:P/ECE環境ではコードサイズ節約のためにヒープソートを無効化しています。有効化する事も可能です。}}
	for(i = 0; i < 100; i++) { test_subr((void (*)(void*, size_t, size_t, int (*)(const void*,const void*, void*), void*))heapsort_r); }	//heapsort_rのテスト
  #endif//PIECE	//{{2016/06/10:P/ECE環境ではコードサイズ節約のためにヒープソートを無効化しています。有効化する事も可能です。}}
	for(i = 0; i < 100; i++) { test_subr((void (*)(void*, size_t, size_t, int (*)(const void*,const void*, void*), void*))mergesort_r); }	//mergesort_rのテスト
	puts("ok");
}
static int compar(const void* _x, const void* _y, void* arg) {
	const char* x = _x;
	const char* y = _y;
	int offset = (int)arg;
	return x[offset] - y[offset];
}
static void test_subr(void (*sort_r)(void*, size_t, size_t, int (*)(const void*,const void*, void*), void*)) {
    #define N 127
	char data[N][2+1/*nul*/];
	int i;
	//ランダムな文字列("0A"～"9Z")を作成する。
	for(i = 0; i < N; i++) {
		data[i][0] = RND32_RANGE(seed, '0', '9' + 1);
		data[i][1] = RND32_RANGE(seed, 'A', 'Z' + 1);
		data[i][2] = '\0';
	}
	//先に、文字列の2文字目でソートする。
	(*sort_r)(&data[0][0], N, 2+1/*nul*/, compar, (void*)1);
	//次に、文字列の1文字目でソートする。
	(*sort_r)(&data[0][0], N, 2+1/*nul*/, compar, (void*)0);
	//結果を確認する。
	for(i = 0; i < N; i++) {
	    //	printf("%s\n", data[i]);
		if(i && (strncmp(data[i - 1], data[i], 1) > 0)) { DIE(); }										//どのソートでも、後でソートした方(1文字目)は昇順になっているはず。
		if(i && ( strcmp(data[i - 1], data[i]   ) > 0)) { if((void*)sort_r == (void*)mergesort_r) { DIE(); } /*else { printf("*"); }*/ }	//mergesort_rは安定ソートなので先にソートした方(2文字目)は維持されているはず。qsort_r,heapsort_rは不安定ソートなので先にソートした方(2文字目)は不定である。
	}
    #undef  N
}
#endif

/****************************************************************************
 *	速度計測結果(2016/06/11)
 ****************************************************************************/
// * Sat Jun 11 20:47:59 JST 2016 Naoyuki Sawa
// - clipsort.cの一番下に、「速度計測結果(2016/06/11)」を追記しました。
//   Win10(x64)とP/ECEで、各関数の速度を計測した結果と、考察です。
//   {{計測プログラム:ここから
#if 0
#define N 32768
static uint8_t data[N][2];
static int compar(const void* _x, const void* _y, void* arg) {
	int offset = (int)arg;
	int x = ((uint8_t*)_x)[offset];
	int y = ((uint8_t*)_y)[offset];
	if(x < y) { return -1; }
	if(x > y) { return  1; }
	return 0;
}
static void test(void (*sort)(void*,size_t,size_t,int (*)(const void*,const void*,void*), void*)) {
	int i, t, seed = 0;
	for(i = 0; i < N; i++) {
		data[i][0] = RND8(seed);
		data[i][1] = RND8(seed);
	}
    #ifdef  PIECE
	t = pceTimerGetCount();
    #else //PIECE
	t = GetTickCount();
    #endif//PIECE
	for(i = 0; i < 3; i++) {
		(*sort)(data, N, 2, compar, (void*)0);
		(*sort)(data, N, 2, compar, (void*)1);
	}
    #ifdef  PIECE
	t = pceTimerGetCount() - t;
	die("%d", t);
    #else //PIECE
	t = GetTickCount() - t;
	printf("%d\n", t);
	getch();
    #endif//PIECE
}
int app_main(int argc, char* argv[]) {
	test(qsort_r);
   //	test(heapsort_r);
   //	test(qsort_r);
	return 0;
}
#endif
//   }}計測プログラム:ここまで
// - □結果
//   test(qsort_r)の結果:
//   ・Core i7-4500U	   78[ms]	VC++6.0,リリースピルド
//   ・P/ECE		43407[ms]	C言語版qsort_r()
//   ・P/ECE		41404[ms]	アセンブラ版qsort_r()
//   test(heapsort_r)の結果:
//   ・Core i7-4500U	45953[ms]	VC++6.0,リリースピルド
//   ・P/ECE		(未対応)
//   test(mergesort_r)の結果:
//   ・Core i7-4500U	   63[ms]	VC++6.0,リリースピルド
//   ・P/ECE		12119[ms]	C言語版qsort_r()
//   ・P/ECE		11377[ms]	アセンブラ版qsort_r()
// - □考察
//   ・意外にもqsort_r()よりもmergesort_r()の方が早い。
//     上記の結果はmergesort_r()内のmalloc/freeの時間を含んでいるが、それでもmergesort_r()の方が早い。
//     ただし、今回はデータが大きいのでmalloc/freeが占める割合が小さいだけで、データが小さいとmalloc/freeが占める割合が大きくなって結果が逆転する可能性は有る。
//   ・動的なメモリ確保を行っているという欠点を除けば、mergesort_r()が安定ソートだし結構良いみたいだ。
//     これまでは無条件にqsort_r()を使っていたが、今後はmergesort_r()の使用も積極的に検討しよう。
//     特に、動的メモリ確保のコストが小さいPC環境では、mergesort_r()を積極的に使っても良さそうだ。
//   ・現状ではheapsort_r()が極端に遅い。
//     現在のheapsort_r()は日本語版ウィキペディアのヒープソートのページを見て作ったのだが、どうやらウィキペディアのサンプルはかなり遅い実装であるようだ。
//     参照資料:「DreamHope」(http://www.dreamhope.net/)さんの『ヒープソートでなぜ再帰？～日本語版ウィキペディアの問題点と最強最速のヒープソート～』(http://www.dreamhope.net/soliloquies/HeapSort/)
//     いずれ、上記の記事を参考にして、heapsort_r()を作り直そう。	←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←※TODO:
