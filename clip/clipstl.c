/*
 *	clipstl.c
 *
 *	STLを参考にして作成した関数
 *
 *	CLiP - Common Library for P/ECE
 *	Copyright (C) 2016 Naoyuki Sawa
 *
 *	* Sat Mar 05 21:53:08 JST 2016 Naoyuki Sawa
 *	- 1st リリース。
 *	- lower_bound()関数,upper_bound()関数を作成しました。
 *	- スレッドセーフ版のlower_bound_r()関数,upper_bound_r()も追加しました。
 *	* Tue Jun 07 22:13:11 JST 2016 Naoyuki Sawa
 *	- std_rotate()関数を追加しました。
 *	  目的は、メモリローテート処理の高速化のためです。
 *	  詳細は、以下の通りです。
 *	- 昨日、clipstr.cモジュールにmemrotate()関数を追加したのですが、素直なアルゴリズムではあるのですが、いまいち低速でした。
 *	  memrotate()が呼び出しているmemmove()も高速化する等して工夫したのですが、あまり効果は有りませんでした。(memmove()を高速化出来た事はそれ単体でも意義は有ったのですが…)
 *	  解決方法を探して、Webでメモリローテート処理のアルゴリズムについて検索して見た処、既に、標準C++ライブラリに、std::rotate()という関数が有る事が判りました。
 *	  「cppreference.com - std::rotate」(http://en.cppreference.com/w/cpp/algorithm/rotate)に、その実装方法が書かれており、とても効率の良いアルゴリズムのようです。
 *	  そこで、このアルゴリズムを、C言語版として実装する事にしました。
 *	  直接、clipstr.cモジュールのmemrotate()を変更しても良いのですが、敢えて別関数std_rotate()として実装し、memrotate()はstd_rotate()を呼び出すように変更する事にしました。
 *	  そうした理由は、STLや標準C++ライブラリを元にして作成したコードは、当モジュール(clipstl.c)に集める方針だからです。
 *	- □使用例
 *	  │void test() {
 *	  │    int a[] = { 0,1,2,3,4,5,6,7,8,9 }, i;
 *	  │    std_rotate(a, &a[3], &a[10]);
 *	  │    for(i = 0; i < 10; i++) { printf("%d ", a[i]); }
 *	  │    //結果:     3 4 5 6 7 8 9 0 1 2
 *	  │}
 *	- 当モジュールに追加する関数名に'std_'のプレフィクスを付ける方針ではなかったのですが、'rotate'だけではあまりに単純過ぎて名前の衝突が心配なので、特別に'std_'のプレフィクスを付ける事にしました。
 *	  以前に追加したlower_bound()関数等には'std_'のプレフィクスは付けていなかったので統一が取れていませんが、実用上は問題無いと思います。
 *	  当モジュールに追加する関数の関数名については、あまり厳密に考えない事にしました。
 *	  今後も、ケースバイケースで、'std_'のプレフィクスを付けるかどうか判断して下さい。
 *	* Wed Jun 08 21:50:02 JST 2016 Naoyuki Sawa
 *	- std::rotate()のアルゴリズムが理解出来たので、コメントを追記しました。
 *	* Sun Sep 04 21:41:17 JST 2016 Naoyuki Sawa
 *	- std_rotate()が、(int)でアドレス演算を行っていたのを、(intptr_t)で行うように修正しました。
 *	  64ビット対応のための準備ですが、32ビットでも(intptr_t)で行う方が本当は正しいからです。
 *	  とは言え、32ビットではどちらでも同じ結果になるので、既存の32ビットコードには影響有りません。
 */
#include "clip.h"
/****************************************************************************
 *	
 ****************************************************************************/
// * Sat Mar 05 21:53:08 JST 2016 Naoyuki Sawa
// - lower_bound()関数,upper_bound()関数を追加しました。
//   目的は、'値の範囲付きbsearch()'を行いたい事が有ったからです。
// - lower_bound,upper_boundの関数名は、STLのstd::lower_bound(),std::upper_bound()から取りました。
//   今回追加したlower_bound()関数,upper_bound()関数は、bsearch()の引数仕様/戻り値仕様で、STLのstd::lower_bound(),std::upper_bound()と同じ処理を行うものです。
// - □参考資料
//   │「Programing Place」(http://ppp-lab.sakura.ne.jp/)さんの『C++編(標準ライブラリ) 第20章 ソート済みの範囲を扱うアルゴリズム』(http://ppp-lab.sakura.ne.jp/cpp/library/020.html)の記事
//   │「Stack Overflow」の『Find the first element in an array that is greater than the target』(http://stackoverflow.com/questions/6553970/find-the-first-element-in-an-array-that-is-greater-than-the-target)の記事
// - スレッドセーフ版のlower_bound_r()関数,upper_bound_r()も追加しました。
//   qsort()に対する、qsort_r()に相当します。
//   ちなみに、bsearch()に対するbsearch_r()は存在しません。
//   bsearch_r,lfind_r,lsearch_r,が存在しないのは、C標準ライブラリ仕様が抜けている問題のようです。
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
//(key≦val)となる、最初のvalへのポインタを返す。
//全てのvalが(key＞val)ならば、最後のvalの次へのポインタを返す。	←←←←←std::lower_bound()の仕様です。NULLを返すのではない事に注意して下さい。
//[in]
//		引数仕様は、bsearch()と同じです。
//		引数に指定するcompar関数は、bsearch()と同じものが利用出来ます。
//[out]
//		戻り値の仕様は、std::lower_bound()と同じです。
//		当関数が、NULLを返す事は有りません。
//[note]
//		- 条件を満たす位置が無かった場合、bsearch()はNULLを返しますが、当関数は最後のvalの次へのポインタを返す事に注意して下さい。
//		  std::lower_bound()の仕様に倣ったからです。
//		- 条件を満たす位置が無かった場合、bsearch()と同様にNULLを返す仕様に変更しようかとも思ったのですが、
//		  実際に使って見ると、呼び出し側の観点でも、最後のvalの次へのポインタが返された方が使い易いケースが多い事がわかりました。
//		  従って、条件を満たす位置が無かった場合は、std::lower_bound()と同様に、最後のvalの次へのポインタを返す事にしました。
void* lower_bound(const void* key, const void* _base, size_t nmemb, size_t size, int (*compar)(const void*, const void*)) {
	return lower_bound_r(key, _base, nmemb, size, (int (*)(const void*, const void*, void*))compar, NULL);	//lower_bound()とlower_bound_r()は、compar関数の形式が異なりますが、'cdecl呼出規約'ならば、lower_bound()形式のcompar関数において、余分な引数が無視されるので問題ありません。
}
void* lower_bound_r(const void* key, const void* _base, size_t nmemb, size_t size, int (*compar)(const void*, const void*, void*), void* arg) {
	unsigned char* base = (unsigned char*)_base;
	unsigned lo = 0, hi = nmemb;
	while(lo != hi) {
		unsigned mid = (lo + hi) >> 1;
		if((*compar)(key, base + (size * mid), arg) >  0) {	//この位置の値がキー未満ならば、この位置は条件を満たしていないので…	←←←←←lower_bound()とupper_bound()の違いはこの行だけです。
			lo = mid + 1;					//結果の位置は、少なくとも、この位置の次以降となる。
		} else {						//この位置の値がキー以上ならば、この位置は条件を満たしているので…
			hi = mid;					//結果の位置は、少なくとも、この位置以前となる。
		}
	}
	return base + (size * lo);	//条件を満たす位置が有った場合は、(lo==hi＜nmemb)となっている。条件を満たす位置が無かった場合は、(lo==hi==nmemb)となっている。
}
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
//(key＜val)となる、最初のvalへのポインタを返す。
//全てのvalが(key≧val)ならば、最後のvalの次へのポインタを返す。	←←←←←std::upper_bound()の仕様です。NULLを返すのではない事に注意して下さい。
//[note]
//	- lower_bound()の説明を参照して下さい。
void* upper_bound(const void* key, const void* _base, size_t nmemb, size_t size, int (*compar)(const void*, const void*)) {
	return upper_bound_r(key, _base, nmemb, size, (int (*)(const void*, const void*, void*))compar, NULL);	//upper_bound()とupper_bound_r()は、compar関数の形式が異なりますが、'cdecl呼出規約'ならば、upper_bound()形式のcompar関数において、余分な引数が無視されるので問題ありません。
}
void* upper_bound_r(const void* key, const void* _base, size_t nmemb, size_t size, int (*compar)(const void*, const void*, void*), void* arg) {
	unsigned char* base = (unsigned char*)_base;
	unsigned lo = 0, hi = nmemb;
	while(lo != hi) {
		unsigned mid = (lo + hi) >> 1;
		if((*compar)(key, base + (size * mid), arg) >= 0) {	//この位置の値がキー以下ならば、この位置は条件を満たしていないので…	←←←←←lower_bound()とupper_bound()の違いはこの行だけです。
			lo = mid + 1;					//結果の位置は、少なくとも、この位置の次以降となる。
		} else {						//この位置の値がキー超過ならば、この位置は条件を満たしているので…
			hi = mid;					//結果の位置は、少なくとも、この位置以前となる。
		}
	}
	return base + (size * lo);	//条件を満たす位置が有った場合は、(lo==hi＜nmemb)となっている。条件を満たす位置が無かった場合は、(lo==hi==nmemb)となっている。
}
/****************************************************************************
 *	
 ****************************************************************************/
// * Tue Jun 07 22:13:11 JST 2016 Naoyuki Sawa
// - std_rotate()関数を追加しました。
//   目的は、メモリローテート処理の高速化のためです。
//   詳細は、以下の通りです。
// - 昨日、clipstr.cモジュールにmemrotate()関数を追加したのですが、素直なアルゴリズムではあるのですが、いまいち低速でした。
//   memrotate()が呼び出しているmemmove()も高速化する等して工夫したのですが、あまり効果は有りませんでした。(memmove()を高速化出来た事はそれ単体でも意義は有ったのですが…)
//   解決方法を探して、Webでメモリローテート処理のアルゴリズムについて検索して見た処、既に、標準C++ライブラリに、std::rotate()という関数が有る事が判りました。
//   「cppreference.com - std::rotate」(http://en.cppreference.com/w/cpp/algorithm/rotate)に、その実装方法が書かれており、とても効率の良いアルゴリズムのようです。
//   そこで、このアルゴリズムを、C言語版として実装する事にしました。
//   直接、clipstr.cモジュールのmemrotate()を変更しても良いのですが、敢えて別関数std_rotate()として実装し、memrotate()はstd_rotate()を呼び出すように変更する事にしました。
//   そうした理由は、STLや標準C++ライブラリを元にして作成したコードは、当モジュール(clipstl.c)に集める方針だからです。
// - □使用例
//   │void test() {
//   │    int a[] = { 0,1,2,3,4,5,6,7,8,9 }, i;
//   │    std_rotate(a, &a[3], &a[10]);
//   │    for(i = 0; i < 10; i++) { printf("%d ", a[i]); }
//   │    //結果:     3 4 5 6 7 8 9 0 1 2
//   │}
// - 当モジュールに追加する関数名に'std_'のプレフィクスを付ける方針ではなかったのですが、'rotate'だけではあまりに単純過ぎて名前の衝突が心配なので、特別に'std_'のプレフィクスを付ける事にしました。
//   以前に追加したlower_bound()関数等には'std_'のプレフィクスは付けていなかったので統一が取れていませんが、実用上は問題無いと思います。
//   当モジュールに追加する関数の関数名については、あまり厳密に考えない事にしました。
//   今後も、ケースバイケースで、'std_'のプレフィクスを付けるかどうか判断して下さい。
//
//------------------------------------------------------------------------------------------------------------------------------
//
//	□「cppreference.com - std::rotate」(http://en.cppreference.com/w/cpp/algorithm/rotate)より引用:
//	│template<class ForwardIt> void rotate(ForwardIt first, ForwardIt n_first, ForwardIt last);
//	│
//	│Parameters
//	│	first		the beginning of the original range
//	│	n_first		the element that should appear at the beginning of the rotated range
//	│	last		the end of the original range
//	│
//	│Possible implementation
//	│
//	│	Possible implementation
//	│	template<class ForwardIt> void rotate(ForwardIt first, ForwardIt n_first, ForwardIt last) {
//	│		ForwardIt next = n_first;
//	│		while(first != next) {
//	│			std::iter_swap(first++, next++);
//	│			if(next == last) {
//	│				next = n_first;
//	│			} else if(first == n_first) {
//	│				n_first = next;
//	│			}
//	│		}
//	│	}
//
//	例として、
//		std::vector<int> v = {1,2,3,4,5};	//C++11ならばstd::vector<int> v{1,2,3,4,5};と書いても良い。(Uniform Initialization)
//	に対して、
//		std::rotate(v.begin(), v.begin() + 2, v.end());
//	を行った時の動作を図示します。
//
//	│	Ｆ: first
//	│	NF: n_first
//	│	Ｌ: last
//	│	Ｎ: next
//	│
//	│	①初期状態
//	│
//	│		　　　　　NF　　　　　Ｌ
//	│		┌─┬─┬─┬─┬─┐　
//	│		│１│２│３│４│５│　
//	│		└─┴─┴─┴─┴─┘　
//	│		　Ｆ　　　Ｎ　　　　　　
//	│
//	│	②Ｆの位置の値と、Ｎの位置の値を交換し、ＦとＮを進める。
//	│
//	│		　　　　　NF　　　　　Ｌ
//	│		┏━┳─┬─┬─┬─┐　
//	│		┃３┃２│１│４│５│　
//	│		┗━┻─┴─┴─┴─┘　
//	│		　　　Ｆ　　　Ｎ　　　　
//	│
//	│	③Ｆの位置の値と、Ｎの位置の値を交換し、ＦとＮを進める。
//	│
//	│		　　　　　NF　　　　　Ｌ
//	│		┏━┳━┳─┬─┬─┐　
//	│		┃３┃４┃１│２│５│　
//	│		┗━┻━┻─┴─┴─┘　
//	│		　　　　　Ｆ　　　Ｎ　　
//	│
//	│	④Ｆ==NFになったので、NFをＮの位置へ移動する。
//	│
//	│		　　　　　　　　　NF　Ｌ
//	│		┏━┳━┳─┬─┬─┐　
//	│		┃３┃４┃１│２│５│　
//	│		┗━┻━┻─┴─┴─┘　
//	│		　　　　　Ｆ　　　Ｎ　　
//	│
//	│	⑤Ｆの位置の値と、Ｎの位置の値を交換し、ＦとＮを進める。
//	│
//	│		　　　　　　　　　NF　Ｌ
//	│		┏━┳━┳━┳─┬─┐　
//	│		┃３┃４┃５┃２│１│　
//	│		┗━┻━┻━┻─┴─┘　
//	│		　　　　　　　Ｆ　　　Ｎ
//	│
//	│	⑥Ｎ==Ｌになったので、ＮをNFの位置へ移動する。
//	│
//	│		　　　　　　　　　NF　Ｌ
//	│		┏━┳━┳━┳─┬─┐　
//	│		┃３┃４┃５┃２│１│　
//	│		┗━┻━┻━┻─┴─┘　
//	│		　　　　　　　Ｆ　Ｎ　　
//	│
//	│	⑦Ｆの位置の値と、Ｎの位置の値を交換し、ＦとＮを進める。
//	│
//	│		　　　　　　　　　NF　Ｌ
//	│		┏━┳━┳━┳━┳━┓　
//	│		┃３┃４┃５┃１┃２┃　
//	│		┗━┻━┻━┻━┻━┛　
//	│		　　　　　　　　　Ｆ　Ｎ
//	│
//	│	⑧Ｎ==Ｌになったので、ＮをNFの位置へ移動する。
//	│
//	│		　　　　　　　　　NF　Ｌ
//	│		┏━┳━┳━┳━┳━┓　
//	│		┃３┃４┃５┃１┃２┃　
//	│		┗━┻━┻━┻━┻━┛　
//	│		　　　　　　　　　Ｆ　　
//	│		　　　　　　　　　Ｎ　　
//	│
//	│	⑨Ｆ==Ｎになったので終了する。
//
//	太罫線の部分は、値が決定した箇所を示します。
//	最初の要素から値が決定して行き、一度決定した要素は複数回書き換える事は無いので、効率的です。
//	しかし、⑦～⑧の部分が、まだ直観的に理解出来ていません。
//	後ろ二つの要素は、一時的に入れ替えられて元の並びと違っているのに、上記のアルゴリズムでなぜ正しい並びに戻せるのでしょうか…
//	とは言え、標準C++ライブラリですからアルゴリズムが正しい事は間違いないので、そのまま実装する事にしました。
//	追々、理解しようと思います。	⇒{{2016/06/08追記:std::rotate()のアルゴリズムが理解出来たので、コメントを追記しました。このすぐ下を参照して下さい。}}
//
// * Wed Jun 08 21:50:02 JST 2016 Naoyuki Sawa
// - std::rotate()のアルゴリズムが理解出来ました。
//   【初期状態がＡ＝Ｂである場合】【初期状態がＡ＞Ｂである場合】【初期状態がＡ＜Ｂである場合】に分けて考えて一手順を実行し、その後、残った範囲に対してstd::rotate()の処理を再帰実行すると考えれば判り易いです。
//   詳細は、この下の図を参照して下さい。
//
//	│	【初期状態がＡ＝Ｂである場合】
//	│
//	│		「初期状態」
//	│
//	│			　　　　NF　　　Ｌ
//	│			┌─┬─┬─┬─┐
//	│			│１│２│３│４│
//	│			└─┴─┴─┴─┘
//	│			Ｆ　　　Ｎ　　　　
//	│			│←Ａ→│←Ｂ→│
//	│
//	│		「最終状態」
//	│
//	│			┏━┳━┳━┳━┓
//	│			┃３┃４┃１┃２┃
//	│			┗━┻━┻━┻━┛
//	│			│←Ｂ→│←Ａ→│
//	│
//	│			「初期状態」のＢを先頭へ移動し、その分Ａを後ろへずらすと、「最終状態」になります。
//	│
//	│
//	│		～～～手順～～～
//	│
//	│			　　　　NF　　　Ｌ
//	│			┌─┬─┬─┬─┐
//	│			│１│２│３│４│
//	│			└─┴─┴─┴─┘
//	│			Ｆ　　　Ｎ　　　　
//	│			│←Ａ→│←Ｂ→│
//	│
//	│			　　　　NF　　　Ｌ
//	│			┏━┳─┬─┬─┐
//	│			┃３┃２│１│４│
//	│			┗━┻─┴─┴─┘
//	│			　　Ｆ　　　Ｎ　　
//	│
//	│			　　　　NF　　　Ｌ
//	│			┏━┳━┳─┬─┐
//	│			┃３┃４┃１│２│
//	│			┗━┻━┻─┴─┘
//	│			　　　　Ｆ　　　Ｎ
//	│
//	│			　　　　NF　　　Ｌ
//	│			┏━┳━┳━┳━┓
//	│			┃３┃４┃１┃２┃
//	│			┗━┻━┻━┻━┛
//	│			　　　　Ｆ　　　　
//	│			　　　　Ｎ　　　　
//	│			│←Ｂ→│←Ａ→│
//	│
//	│			ＡもＢも完成したので、完了です。
//	│
//	│	【初期状態がＡ＞Ｂである場合】
//	│
//	│		「初期状態」
//	│
//	│			　　　　　　NF　　　Ｌ
//	│			┌─┬─┬─┬─┬─┐
//	│			│１│２│３│４│５│
//	│			└─┴─┴─┴─┴─┘
//	│			Ｆ　　　　　Ｎ　　　　
//	│			│←─Ａ─→│←Ｂ→│
//	│
//	│		「最終状態」
//	│
//	│			┏━┳━┳━┳━┳━┓
//	│			┃４┃５┃１┃２┃３┃
//	│			┗━┻━┻━┻━┻━┛
//	│			│←Ｂ→│←─Ａ─→│
//	│
//	│			「初期状態」のＢを先頭へ移動し、その分Ａを後ろへずらすと、「最終状態」になります。
//	│
//	│
//	│		～～～手順～～～
//	│
//	│			　　　　　　NF　　　Ｌ
//	│			┌─┬─┬─┬─┬─┐
//	│			│１│２│３│４│５│
//	│			└─┴─┴─┴─┴─┘
//	│			Ｆ　　　　　Ｎ　　　　
//	│			│←─Ａ─→│←Ｂ→│
//	│
//	│			　　　　　　NF　　　Ｌ
//	│			┏━┳─┬─┬─┬─┐
//	│			┃４┃２│３│１│５│
//	│			┗━┻─┴─┴─┴─┘
//	│			　　Ｆ　　　　　Ｎ　　
//	│
//	│			　　　　　　NF　　　Ｌ
//	│			┏━┳━┳─┬─┬─┐
//	│			┃４┃５┃３│１│２│
//	│			┗━┻━┻─┴─┴─┘
//	│			　　　　Ｆ　　　　　Ｎ
//	│
//	│			　　　　　　NF　　　Ｌ
//	│			┏━┳━┳─┬─┬─┐
//	│			┃４┃５┃３│１│２│
//	│			┗━┻━┻─┴─┴─┘
//	│			　　　　Ｆ　Ｎ　　　　
//	│			│←Ｂ→│←─A'─→│
//	│
//	│			この時点で、Ｂは完成している。A'に対して、std::rotate()の処理を再帰実行すれば良い。
//	│
//	│	【初期状態がＡ＜Ｂである場合】
//	│
//	│		「初期状態」
//	│
//	│			　　　　NF　　　　　Ｌ
//	│			┌─┬─┬─┬─┬─┐
//	│			│１│２│３│４│５│
//	│			└─┴─┴─┴─┴─┘
//	│			Ｆ　　　Ｎ　　　　　　
//	│			│←Ａ→│←─Ｂ─→│
//	│
//	│		「最終状態」
//	│
//	│			┏━┳━┳━┳━┳━┓
//	│			┃３┃４┃５┃１┃２┃
//	│			┗━┻━┻━┻━┻━┛
//	│			│←─Ｂ─→│←Ａ→│
//	│
//	│			「初期状態」のＢを先頭へ移動し、その分Ａを後ろへずらすと、「最終状態」になります。
//	│
//	│
//	│		～～～手順～～～
//	│
//	│			　　　　NF　　　　　Ｌ
//	│			┌─┬─┬─┬─┬─┐
//	│			│１│２│３│４│５│
//	│			└─┴─┴─┴─┴─┘
//	│			Ｆ　　　Ｎ　　　　　　
//	│			│←Ａ→│←─Ｂ─→│
//	│
//	│			　　　　NF　　　　　Ｌ
//	│			┏━┳─┬─┬─┬─┐
//	│			┃３┃２│１│４│５│
//	│			┗━┻─┴─┴─┴─┘
//	│			　　Ｆ　　　Ｎ　　　　
//	│
//	│			　　　　NF　　　　　Ｌ
//	│			┏━┳━┳─┬─┬─┐
//	│			┃３┃４┃１│２│５│
//	│			┗━┻━┻─┴─┴─┘
//	│			　　　　Ｆ　　　Ｎ　　
//	│
//	│			　　　　　　　　NF　Ｌ
//	│			┏━┳━┳─┬─┬─┐
//	│			┃３┃４┃１│２│５│
//	│			┗━┻━┻─┴─┴─┘
//	│			　　　　Ｆ　　　Ｎ　　
//	│			│←B'→│←─A'─→│
//	│
//	│			この時点で、B'は完成している。A'に対して、std::rotate()の処理を再帰実行すれば良い。
//	│
//------------------------------------------------------------------------------------------------------------------------------
#ifndef PIECE
static void std_rotate_word(int* first, int* n_first, int* last) {
	int* next = n_first;
    //{{この範囲のC言語ソースは、std_rotate_byte()/std_rotate_half()/std_rotate_word()で共通です。
	while(first != next) {
		int tmp = *first;
		          *first++ = *next;
		                     *next++ = tmp;
		if(next == last) {
			next = n_first;
		} else if(first == n_first) {
			n_first = next;
		}
	}
    //}}この範囲のC言語ソースは、std_rotate_byte()/std_rotate_half()/std_rotate_word()で共通です。
}
static void std_rotate_half(short* first, short* n_first, short* last) {
	short* next = n_first;
    //{{この範囲のC言語ソースは、std_rotate_byte()/std_rotate_half()/std_rotate_word()で共通です。
	while(first != next) {
		int tmp = *first;
		          *first++ = *next;
		                     *next++ = tmp;
		if(next == last) {
			next = n_first;
		} else if(first == n_first) {
			n_first = next;
		}
	}
    //}}この範囲のC言語ソースは、std_rotate_byte()/std_rotate_half()/std_rotate_word()で共通です。
}
static void std_rotate_byte(char* first, char* n_first, char* last) {
	char* next = n_first;
    //{{この範囲のC言語ソースは、std_rotate_byte()/std_rotate_half()/std_rotate_word()で共通です。
	while(first != next) {
		int tmp = *first;
		          *first++ = *next;
		                     *next++ = tmp;
		if(next == last) {
			next = n_first;
		} else if(first == n_first) {
			n_first = next;
		}
	}
    //}}この範囲のC言語ソースは、std_rotate_byte()/std_rotate_half()/std_rotate_word()で共通です。
}
void std_rotate(void* first, void* n_first, void* last) {
//{{2016/09/04変更:std_rotate()が、(int)でアドレス演算を行っていたのを、(intptr_t)で行うように修正しました。
//	int x = (int)first | (int)n_first | (int)last;
//↓2016/09/04変更:std_rotate()が、(int)でアドレス演算を行っていたのを、(intptr_t)で行うように修正しました。
	intptr_t x = (intptr_t)first | (intptr_t)n_first | (intptr_t)last;
//}}2016/09/04変更:std_rotate()が、(int)でアドレス演算を行っていたのを、(intptr_t)で行うように修正しました。
	if(!(x & 3)) {
		std_rotate_word(first, n_first, last);	//first,n_first,lastが全てワード整列している場合
	} else if(!(x & 1)) {
		std_rotate_half(first, n_first, last);	//first,n_first,lastが全てハーフワード整列している場合
	} else {
		std_rotate_byte(first, n_first, last);	//first,n_first,lastが一つでも奇数アドレスの場合
	}
}
#else //PIECE
void std_rotate(void* first, void* n_first, void* last);
//□高速化検証プログラム。各バージョンに記載した時間は、このプログラムで計測した時間です。
//│int test() {
//│  static int buf[16384];
//│  int i, t1, t2, t3;
//│  t1 = pceTimerGetCount();
//│  for(i = 0; i < 100; i++) { std_rotate(&buf[0], &buf[1], &buf[16384]); }		//どのバージョンでも同じ速度です。
//│  t1 = pceTimerGetCount() - t1;
//│  t2 = pceTimerGetCount();
//│  for(i = 0; i < 100; i++) { std_rotate(&buf[0], &buf[16384/2], &buf[16384]); }	//'高速化前のバージョン'が低速で、'高速化バージョン1'と'高速化バージョン2'が高速です。
//│  t2 = pceTimerGetCount() - t2;
//│  t3 = pceTimerGetCount();
//│  for(i = 0; i < 100; i++) { std_rotate(&buf[0], &buf[16384-1], &buf[16384]); }	//'高速化前のバージョン'と'高速化バージョン1'が低速で、'高速化バージョン2'が高速です。
//│  t3 = pceTimerGetCount() - t3;
//│  die("%d[ms]\n%d[ms]\n%d[ms]", t1, t2, t3);
//│}
//以上の検証結果より、'高速化バージョン2'が最速である事が判り、'高速化バージョン2'を採用する事にしました。
//尚、コードサイズは、'高速化前のバージョン'が3命令少なく、'高速化バージョン1'と'高速化バージョン2'は同じコードサイズです。
//3命令程度の増加ならば許容範囲だと思うので、'高速化バージョン2'を採用する事に問題は有りません。
#if 0
//□高速化前のバージョン	2830[ms] 1347[ms] 2554[ms]
asm("
		.code
		.align		1
		.global		std_rotate
std_rotate:
		;//%r12 := first
		;//%r13 := n_first
		;//%r14 := last
		ld.w		%r9, %r12			;//%r9  :=  first
		or		%r9, %r13			;//%r9  :=  first|n_first
		or		%r9, %r14			;//%r9  :=  first|n_first|last
		and		%r9, 3				;//%r9  := (first|n_first|last)&3
		jreq		std_rotate_word			;//if(    !(first|n_first|last)&3) { goto std_rotate_word }
		and		%r9, 1				;//%r9  := (first|n_first|last)&1
		jreq		std_rotate_half			;//if(    !(first|n_first|last)&1) { goto std_rotate_half }
		;/* FALLTHRU */					;//                                { goto std_rotate_byte }
		;//---------------------------------------------;//
std_rotate_byte:
		ld.w		%r15, %r13			;//%r15 := next = n_first
std_rotate_byte_LOOP:						;//for(;;) {
		cmp		%r12, %r15			;//  if(first == next) { return }
		jreq		std_rotate_RET			;//  
		ld.b		%r4, [%r12]			;//  %r4  := tmp1 = *first
		ld.b		%r5, [%r15]			;//  %r5  := tmp2 = *next
		ld.b		[%r15]+, %r4			;//  *next++  = tmp1						*anti-interlock*
		ld.b		[%r12]+, %r5			;//  *first++ = tmp2
		cmp		%r15, %r14			;//  if(next == last) { next = n_first }
		jreq		std_rotate_byte			;//  
		cmp		%r12, %r13			;//  if(first != n_first) { continue }
		jrne		std_rotate_byte_LOOP		;//  
		jp.d		std_rotate_byte_LOOP		;//}
		ld.w		%r13, %r15			;//  %r13 := n_first = next					*delay*
		;//---------------------------------------------;//
std_rotate_half:
		ld.w		%r15, %r13			;//%r15 := next = n_first
std_rotate_half_LOOP:						;//for(;;) {
		cmp		%r12, %r15			;//  if(first == next) { return }
		jreq		std_rotate_RET			;//  
		ld.h		%r4, [%r12]			;//  %r4  := tmp1 = *first
		ld.h		%r5, [%r15]			;//  %r5  := tmp2 = *next
		ld.h		[%r15]+, %r4			;//  *next++  = tmp1						*anti-interlock*
		ld.h		[%r12]+, %r5			;//  *first++ = tmp2
		cmp		%r15, %r14			;//  if(next == last) { next = n_first }
		jreq		std_rotate_half			;//  
		cmp		%r12, %r13			;//  if(first != n_first) { continue }
		jrne		std_rotate_half_LOOP		;//  
		jp.d		std_rotate_half_LOOP		;//}
		ld.w		%r13, %r15			;//  %r13 := n_first = next					*delay*
		;//---------------------------------------------;//
std_rotate_word:
		ld.w		%r15, %r13			;//%r15 := next = n_first
std_rotate_word_LOOP:						;//for(;;) {
		cmp		%r12, %r15			;//  if(first == next) { return }
		jreq		std_rotate_RET			;//  
		ld.w		%r4, [%r12]			;//  %r4  := tmp1 = *first
		ld.w		%r5, [%r15]			;//  %r5  := tmp2 = *next
		ld.w		[%r15]+, %r4			;//  *next++  = tmp1						*anti-interlock*
		ld.w		[%r12]+, %r5			;//  *first++ = tmp2
		cmp		%r15, %r14			;//  if(next == last) { next = n_first }
		jreq		std_rotate_word			;//  
		cmp		%r12, %r13			;//  if(first != n_first) { continue }
		jrne		std_rotate_word_LOOP		;//  
		jp.d		std_rotate_word_LOOP		;//}
		ld.w		%r13, %r15			;//  %r13 := n_first = next					*delay*
		;//---------------------------------------------;//
std_rotate_RET:
		ret
");
#elif 0
//□高速化バージョン1		2830[ms] 1278[ms] 2554[ms]
asm("
		.code
		.align		1
		.global		std_rotate
std_rotate:
		;//%r12 := first
		;//%r13 := n_first
		;//%r14 := last
		ld.w		%r9, %r12			;//%r9  :=  first
		or		%r9, %r13			;//%r9  :=  first|n_first
		or		%r9, %r14			;//%r9  :=  first|n_first|last
		and		%r9, 3				;//%r9  := (first|n_first|last)&3
		jreq		std_rotate_word			;//if(    !(first|n_first|last)&3) { goto std_rotate_word }
		and		%r9, 1				;//%r9  := (first|n_first|last)&1
		jreq		std_rotate_half			;//if(    !(first|n_first|last)&1) { goto std_rotate_half }
		;/* FALLTHRU */					;//                                { goto std_rotate_byte }
		;//---------------------------------------------;//
std_rotate_byte:
		ld.w		%r15, %r13			;//%r15 := next = n_first
		cmp		%r12, %r15			;//  	←───────────────────┐
std_rotate_byte_LOOP:						;//for(;;) {					│
	;//	cmp		%r12, %r15			;//  if(first == next) { return }	────┤	ループ時の遅延スロットを活用するためにこの行の命令を二か所に分けた。少しコードサイズが増えるが速度を優先する事にした。
		jreq		std_rotate_RET			;//  						│
		ld.b		%r4, [%r12]			;//  %r4  := tmp1 = *first			│
		ld.b		%r5, [%r15]			;//  %r5  := tmp2 = *next			│
		ld.b		[%r15]+, %r4			;//  *next++  = tmp1				│		*anti-interlock*
		ld.b		[%r12]+, %r5			;//  *first++ = tmp2				│
		cmp		%r15, %r14			;//  if(next == last) { next = n_first }	│
		jreq		std_rotate_byte			;//  						│
		cmp		%r12, %r13			;//  if(first != n_first) { continue }		│
		jrne.d		std_rotate_byte_LOOP		;//  						│
		cmp		%r12, %r15			;//  	←───────────────────┘		*delay*
		jp.d		std_rotate_byte_LOOP		;//}
		ld.w		%r13, %r15			;//  %r13 := n_first = next					*delay*
		;//---------------------------------------------;//
std_rotate_half:
		ld.w		%r15, %r13			;//%r15 := next = n_first
		cmp		%r12, %r15			;//  	←───────────────────┐
std_rotate_half_LOOP:						;//for(;;) {					│
	;//	cmp		%r12, %r15			;//  if(first == next) { return }	────┤	ループ時の遅延スロットを活用するためにこの行の命令を二か所に分けた。少しコードサイズが増えるが速度を優先する事にした。
		jreq		std_rotate_RET			;//  						│
		ld.h		%r4, [%r12]			;//  %r4  := tmp1 = *first			│
		ld.h		%r5, [%r15]			;//  %r5  := tmp2 = *next			│
		ld.h		[%r15]+, %r4			;//  *next++  = tmp1				│		*anti-interlock*
		ld.h		[%r12]+, %r5			;//  *first++ = tmp2				│
		cmp		%r15, %r14			;//  if(next == last) { next = n_first }	│
		jreq		std_rotate_half			;//  						│
		cmp		%r12, %r13			;//  if(first != n_first) { continue }		│
		jrne.d		std_rotate_half_LOOP		;//  						│
		cmp		%r12, %r15			;//  	←───────────────────┘		*delay*
		jp.d		std_rotate_half_LOOP		;//}
		ld.w		%r13, %r15			;//  %r13 := n_first = next					*delay*
		;//---------------------------------------------;//
std_rotate_word:
		ld.w		%r15, %r13			;//%r15 := next = n_first
		cmp		%r12, %r15			;//  	←───────────────────┐
std_rotate_word_LOOP:						;//for(;;) {					│
	;//	cmp		%r12, %r15			;//  if(first == next) { return }	────┤	ループ時の遅延スロットを活用するためにこの行の命令を二か所に分けた。少しコードサイズが増えるが速度を優先する事にした。
		jreq		std_rotate_RET			;//  						│
		ld.w		%r4, [%r12]			;//  %r4  := tmp1 = *first			│
		ld.w		%r5, [%r15]			;//  %r5  := tmp2 = *next			│
		ld.w		[%r15]+, %r4			;//  *next++  = tmp1				│		*anti-interlock*
		ld.w		[%r12]+, %r5			;//  *first++ = tmp2				│
		cmp		%r15, %r14			;//  if(next == last) { next = n_first }	│
		jreq		std_rotate_word			;//  						│
		cmp		%r12, %r13			;//  if(first != n_first) { continue }		│
		jrne.d		std_rotate_word_LOOP		;//  						│
		cmp		%r12, %r15			;//  	←───────────────────┘		*delay*
		jp.d		std_rotate_word_LOOP		;//}
		ld.w		%r13, %r15			;//  %r13 := n_first = next					*delay*
		;//---------------------------------------------;//
std_rotate_RET:
		ret
");
#elif 1
//□高速化バージョン2		2830[ms] 1278[ms] 2416[ms]	←←←←←これを採用する事に決定しました。'高速化バージョン2'は'高速化バージョン1'を元にさらに遅延スロットを活用したものですが、'高速化バージョン2'のコードだけを読むと理解し辛いと思います。'高速化バージョン1'と'高速化バージョン2'を比較して見れば、'高速化バージョン2'のコードも理解出来ると思います。
asm("
		.code
		.align		1
		.global		std_rotate
std_rotate:
		;//%r12 := first
		;//%r13 := n_first
		;//%r14 := last
		ld.w		%r9, %r12			;//%r9  :=  first
		or		%r9, %r13			;//%r9  :=  first|n_first
		or		%r9, %r14			;//%r9  :=  first|n_first|last
		and		%r9, 3				;//%r9  := (first|n_first|last)&3
		jreq		std_rotate_word			;//if(    !(first|n_first|last)&3) { goto std_rotate_word }
		and		%r9, 1				;//%r9  := (first|n_first|last)&1
		jreq		std_rotate_half			;//if(    !(first|n_first|last)&1) { goto std_rotate_half }
		;/* FALLTHRU */					;//                                { goto std_rotate_byte }
		;//---------------------------------------------;//
std_rotate_byte:
		cmp		%r12, %r13			;//%psr := first <=> n_first	←───────┐
std_rotate_byte_NEXT:						;//						│
		ld.w		%r15, %r13			;//%r15 := next = n_first			│
std_rotate_byte_LOOP:						;//for(;;) {					│
	;//	cmp		%r12, %r15			;//  if(first == next) { return }	────┤	ループ時の遅延スロットを活用するためにこの行の命令を二か所に分けた。少しコードサイズが増えるが速度を優先する事にした。
		jreq		std_rotate_RET			;//  						│
		ld.b		%r4, [%r12]			;//  %r4  := tmp1 = *first			│
		ld.b		%r5, [%r15]			;//  %r5  := tmp2 = *next			│
		ld.b		[%r15]+, %r4			;//  *next++  = tmp1				│		*anti-interlock*
		ld.b		[%r12]+, %r5			;//  *first++ = tmp2				│
		cmp		%r15, %r14			;//  if(next == last) { next = n_first }	│
		jreq.d		std_rotate_byte_NEXT		;//  						│
		cmp		%r12, %r13			;//  if(first != n_first) { continue }		│		*delay*		この'cmp %r12,%r13'が、ジャンプ先で行うべき'ld.w %r15,%r13; cmp %r12,%r15'の内の、'cmp %r12,%r15'の代用となります。
		jrne.d		std_rotate_byte_LOOP		;//  						│
		cmp		%r12, %r15			;//  	←───────────────────┘		*delay*
		jp.d		std_rotate_byte_LOOP		;//}
		ld.w		%r13, %r15			;//  %r13 := n_first = next					*delay*
		;//---------------------------------------------;//
std_rotate_half:
		cmp		%r12, %r13			;//%psr := first <=> n_first	←───────┐
std_rotate_half_NEXT:						;//						│
		ld.w		%r15, %r13			;//%r15 := next = n_first			│
std_rotate_half_LOOP:						;//for(;;) {					│
	;//	cmp		%r12, %r15			;//  if(first == next) { return }	────┤	ループ時の遅延スロットを活用するためにこの行の命令を二か所に分けた。少しコードサイズが増えるが速度を優先する事にした。
		jreq		std_rotate_RET			;//  						│
		ld.h		%r4, [%r12]			;//  %r4  := tmp1 = *first			│
		ld.h		%r5, [%r15]			;//  %r5  := tmp2 = *next			│
		ld.h		[%r15]+, %r4			;//  *next++  = tmp1				│		*anti-interlock*
		ld.h		[%r12]+, %r5			;//  *first++ = tmp2				│
		cmp		%r15, %r14			;//  if(next == last) { next = n_first }	│
		jreq.d		std_rotate_half_NEXT		;//  						│
		cmp		%r12, %r13			;//  if(first != n_first) { continue }		│		*delay*		この'cmp %r12,%r13'が、ジャンプ先で行うべき'ld.w %r15,%r13; cmp %r12,%r15'の内の、'cmp %r12,%r15'の代用となります。
		jrne.d		std_rotate_half_LOOP		;//  						│
		cmp		%r12, %r15			;//  	←───────────────────┘		*delay*
		jp.d		std_rotate_half_LOOP		;//}
		ld.w		%r13, %r15			;//  %r13 := n_first = next					*delay*
		;//---------------------------------------------;//
std_rotate_word:
		cmp		%r12, %r13			;//%psr := first <=> n_first	←───────┐
std_rotate_word_NEXT:						;//						│
		ld.w		%r15, %r13			;//%r15 := next = n_first			│
std_rotate_word_LOOP:						;//for(;;) {					│
	;//	cmp		%r12, %r15			;//  if(first == next) { return }	────┤	ループ時の遅延スロットを活用するためにこの行の命令を二か所に分けた。少しコードサイズが増えるが速度を優先する事にした。
		jreq		std_rotate_RET			;//  						│
		ld.w		%r4, [%r12]			;//  %r4  := tmp1 = *first			│
		ld.w		%r5, [%r15]			;//  %r5  := tmp2 = *next			│
		ld.w		[%r15]+, %r4			;//  *next++  = tmp1				│		*anti-interlock*
		ld.w		[%r12]+, %r5			;//  *first++ = tmp2				│
		cmp		%r15, %r14			;//  if(next == last) { next = n_first }	│
		jreq.d		std_rotate_word_NEXT		;//  						│
		cmp		%r12, %r13			;//  if(first != n_first) { continue }		│		*delay*		この'cmp %r12,%r13'が、ジャンプ先で行うべき'ld.w %r15,%r13; cmp %r12,%r15'の内の、'cmp %r12,%r15'の代用となります。
		jrne.d		std_rotate_word_LOOP		;//  						│
		cmp		%r12, %r15			;//  	←───────────────────┘		*delay*
		jp.d		std_rotate_word_LOOP		;//}
		ld.w		%r13, %r15			;//  %r13 := n_first = next					*delay*
		;//---------------------------------------------;//
std_rotate_RET:
		ret
");
#endif
#endif//PIECE
