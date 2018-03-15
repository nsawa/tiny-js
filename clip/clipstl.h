/*
 *	clipstl.h
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
 */
#ifndef __CLIP_STL_H__
#define __CLIP_STL_H__
#ifdef  __cplusplus
extern "C" {
#endif//__cplusplus
/****************************************************************************
 *	
 ****************************************************************************/
void* lower_bound(const void* key, const void* _base, size_t nmemb, size_t size, int (*compar)(const void*, const void*));
void* lower_bound_r(const void* key, const void* _base, size_t nmemb, size_t size, int (*compar)(const void*, const void*, void*), void* arg);
void* upper_bound(const void* key, const void* _base, size_t nmemb, size_t size, int (*compar)(const void*, const void*));
void* upper_bound_r(const void* key, const void* _base, size_t nmemb, size_t size, int (*compar)(const void*, const void*, void*), void* arg);
/****************************************************************************
 *	
 ****************************************************************************/
void std_rotate(void* first, void* n_first, void* last);
#ifdef  __cplusplus
}//extern "C"
#endif//__cplusplus
#endif//__CLIP_STL_H__
