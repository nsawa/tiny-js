/*	
 *	cliphash.h
 *
 *	ユーティリティ：ハッシュ関数
 *
 *	CLiP - Common Library for P/ECE
 *	Copyright (C) 2001-2015 Naoyuki Sawa
 *
 *	* Sat Apr 18 12:02:38 JST 2015 Naoyuki Sawa
 *	- 1st リリース。
 *	- 様々なハッシュ関数を実装するために、当モジュールを作成しました。
 *	  現在の所One-at-a-Time Hashしか有りませんが、今後他のハッシュ関数を追加する時は当モジュールに含める予定です。
 *	- 既存のclipcode.hモジュールのMD5,SHA-1,や、既存のclipcrc.hモジュールのCRC8,CRC16,CRC32も、ハッシュ関数ではあるのですが、
 *	  それらの計算方法は個々の規格で厳密に決められたものであるので、当モジュールへ移動する事はしない事にしました。
 *	  当モジュールに含めるハッシュ関数は、あまり厳密に規格化されていない、Web上で公開されているアルゴリズム等にする予定です。
 *	* Wed Nov 18 21:37:53 JST 2015 Naoyuki Sawa
 *	- 'extern "C" {'～'}'で囲みました。.cppを含むWin32プロジェクトからもインクルード出来るようにするためです。
 */
#ifndef __CLIP_HASH_H__
#define __CLIP_HASH_H__
#ifdef  __cplusplus
extern "C" {
#endif//__cplusplus
/*****************************************************************************
 *	One-at-a-Time Hash
 *****************************************************************************/
/* One-at-a-Time Hashを計算します。
 * [in]
 *	key		バイナリデータ。(入力)
 *	len		バイナリデータのバイト数。(>=0)
 * [out]
 *	戻り値		バイナリデータのハッシュ。
 * [note]
 *	- One-at-a-Time Hashは、高速で，シンプルで，高品質なハッシュアルゴリズムです。
 *	  Perlの既定のハッシュアルゴリズムとしても使われているそうで、充分な実績が有ります。
 *	  □参考資料
 *	  │①「Wikipedia - Jenkins hash function - one-at-a-time」(http://en.wikipedia.org/wiki/Jenkins_hash_function#one-at-a-time)
 *	  │②「Bob Jenkins' Web Site - A Hash Function for Hash Table Lookup」(http://www.burtleburtle.net/bob/hash/doobs.html)
 *	  │③「Stack Overflow - What hashing function/algorithm does Perl use ?」(http://stackoverflow.com/questions/11214270/what-hashing-function-algorithm-does-perl-use)
 *	- ①と②はデータをchar型として計算しているのに対して、③はデータをunsigned char型として計算している違いが有ります。
 *	  どちらの方法で計算しても、ハッシュの品質には大差無い(?)みたいです。
 *	  ただし、char型は処理系によってsignedと見なされたりunsignedと見なされたりするので、どちらかを明示する方が安全です。
 *	  当実装は、③の方法に倣う事にしました。
 */
int one_at_a_time_hash(const void* _key, int len);
#ifdef  __cplusplus
}//extern "C"
#endif//__cplusplus
#endif//__CLIP_HASH_H__
