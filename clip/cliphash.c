/*	
 *	cliphash.c
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
 */
#include "clip.h"
/*****************************************************************************
 *	One-at-a-Time Hash
 *****************************************************************************/
#ifndef PIECE
int one_at_a_time_hash(const void* _key, int len) {
	const unsigned char* key = _key;
	unsigned hash = 0;
	goto L_START;
	do {
		hash += *key++;
		hash += hash << 10;
		hash ^= hash >>  6;
L_START:	;
	} while(--len >= 0);
	hash += hash <<  3;
	hash ^= hash >> 11;
	hash += hash << 15;
	return  hash;
}
#else //PIECE
int one_at_a_time_hash(const void* _key, int len);
asm("
		.code
		.align		1
		.global		one_at_a_time_hash
one_at_a_time_hash:
		jp.d		one_at_a_time_hash_START	;//goto L_START
		ld.w		%r9, %r10			;//%r9  := tmp = %r10		*delay*		初回の「xor %r10,%r9」で(%r10=0)にするテクニックです。「ld.w %r10,0; ld.w %r9,0」よりも1命令少なくて済みます。
one_at_a_time_hash_LOOP:					;//do {						│
		ld.ub		%r9, [%r12]+			;//  %r9  := tmp = *key++			│
		add		%r10, %r9			;//  %r10 := hash += tmp			│
		ld.w		%r9, %r10			;//  %r9  := tmp = hash				│
		xsll		%r9, 10				;//  %r9  := tmp <<= 10				│
		add		%r10, %r9			;//  %r10 := hash += tmp			│
		ld.w		%r9, %r10			;//  %r9  := tmp = hash				│
		xsrl		%r9,  6				;//  %r9  := tmp >>=  6				│
one_at_a_time_hash_START:					;//L_START:					│
		sub		%r13, 1				;//  %r13 := len--				│
		jrge.d		one_at_a_time_hash_LOOP		;//} while(len >= 0)				↓
		xor		%r10, %r9			;//  %r10 := hash ^= tmp	*delay*		初回は(%r10=0)になります。
		ld.w		%r9, %r10			;//%r9  := tmp = hash
		xsll		%r9,  3				;//%r9  := tmp <<=  3
		add		%r10, %r9			;//%r10 := hash += tmp
		ld.w		%r9, %r10			;//%r9  := tmp = hash
		xsrl		%r9, 11				;//%r9  := tmp >>= 11
		xor		%r10, %r9			;//%r10 := hash ^= tmp
		ld.w		%r9, %r10			;//%r9  := tmp = hash
		xsll		%r9, 15				;//%r9  := tmp <<= 15
		ret.d						;//return  hash
		add		%r10, %r9			;//%r10 := hash += tmp		*delay*
");
#endif//PIECE
