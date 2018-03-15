/*	
 *	cliphash.c
 *
 *	���[�e�B���e�B�F�n�b�V���֐�
 *
 *	CLiP - Common Library for P/ECE
 *	Copyright (C) 2001-2015 Naoyuki Sawa
 *
 *	* Sat Apr 18 12:02:38 JST 2015 Naoyuki Sawa
 *	- 1st �����[�X�B
 *	- �l�X�ȃn�b�V���֐����������邽�߂ɁA�����W���[�����쐬���܂����B
 *	  ���݂̏�One-at-a-Time Hash�����L��܂��񂪁A���㑼�̃n�b�V���֐���ǉ����鎞�͓����W���[���Ɋ܂߂�\��ł��B
 *	- ������clipcode.h���W���[����MD5,SHA-1,��A������clipcrc.h���W���[����CRC8,CRC16,CRC32���A�n�b�V���֐��ł͂���̂ł����A
 *	  �����̌v�Z���@�͌X�̋K�i�Ō����Ɍ��߂�ꂽ���̂ł���̂ŁA�����W���[���ֈړ����鎖�͂��Ȃ����ɂ��܂����B
 *	  �����W���[���Ɋ܂߂�n�b�V���֐��́A���܂茵���ɋK�i������Ă��Ȃ��AWeb��Ō��J����Ă���A���S���Y�����ɂ���\��ł��B
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
		ld.w		%r9, %r10			;//%r9  := tmp = %r10		*delay*		����́uxor %r10,%r9�v��(%r10=0)�ɂ���e�N�j�b�N�ł��B�uld.w %r10,0; ld.w %r9,0�v����1���ߏ��Ȃ��čς݂܂��B
one_at_a_time_hash_LOOP:					;//do {						��
		ld.ub		%r9, [%r12]+			;//  %r9  := tmp = *key++			��
		add		%r10, %r9			;//  %r10 := hash += tmp			��
		ld.w		%r9, %r10			;//  %r9  := tmp = hash				��
		xsll		%r9, 10				;//  %r9  := tmp <<= 10				��
		add		%r10, %r9			;//  %r10 := hash += tmp			��
		ld.w		%r9, %r10			;//  %r9  := tmp = hash				��
		xsrl		%r9,  6				;//  %r9  := tmp >>=  6				��
one_at_a_time_hash_START:					;//L_START:					��
		sub		%r13, 1				;//  %r13 := len--				��
		jrge.d		one_at_a_time_hash_LOOP		;//} while(len >= 0)				��
		xor		%r10, %r9			;//  %r10 := hash ^= tmp	*delay*		�����(%r10=0)�ɂȂ�܂��B
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
