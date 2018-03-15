/*	
 *	cliphash.h
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
 *	* Wed Nov 18 21:37:53 JST 2015 Naoyuki Sawa
 *	- 'extern "C" {'�`'}'�ň݂͂܂����B.cpp���܂�Win32�v���W�F�N�g������C���N���[�h�o����悤�ɂ��邽�߂ł��B
 */
#ifndef __CLIP_HASH_H__
#define __CLIP_HASH_H__
#ifdef  __cplusplus
extern "C" {
#endif//__cplusplus
/*****************************************************************************
 *	One-at-a-Time Hash
 *****************************************************************************/
/* One-at-a-Time Hash���v�Z���܂��B
 * [in]
 *	key		�o�C�i���f�[�^�B(����)
 *	len		�o�C�i���f�[�^�̃o�C�g���B(>=0)
 * [out]
 *	�߂�l		�o�C�i���f�[�^�̃n�b�V���B
 * [note]
 *	- One-at-a-Time Hash�́A�����ŁC�V���v���ŁC���i���ȃn�b�V���A���S���Y���ł��B
 *	  Perl�̊���̃n�b�V���A���S���Y���Ƃ��Ă��g���Ă��邻���ŁA�[���Ȏ��т��L��܂��B
 *	  ���Q�l����
 *	  ���@�uWikipedia - Jenkins hash function - one-at-a-time�v(http://en.wikipedia.org/wiki/Jenkins_hash_function#one-at-a-time)
 *	  ���A�uBob Jenkins' Web Site - A Hash Function for Hash Table Lookup�v(http://www.burtleburtle.net/bob/hash/doobs.html)
 *	  ���B�uStack Overflow - What hashing function/algorithm does Perl use ?�v(http://stackoverflow.com/questions/11214270/what-hashing-function-algorithm-does-perl-use)
 *	- �@�ƇA�̓f�[�^��char�^�Ƃ��Čv�Z���Ă���̂ɑ΂��āA�B�̓f�[�^��unsigned char�^�Ƃ��Čv�Z���Ă���Ⴂ���L��܂��B
 *	  �ǂ���̕��@�Ōv�Z���Ă��A�n�b�V���̕i���ɂ͑卷����(?)�݂����ł��B
 *	  �������Achar�^�͏����n�ɂ����signed�ƌ��Ȃ��ꂽ��unsigned�ƌ��Ȃ��ꂽ�肷��̂ŁA�ǂ��炩�𖾎�����������S�ł��B
 *	  �������́A�B�̕��@�ɕ키���ɂ��܂����B
 */
int one_at_a_time_hash(const void* _key, int len);
#ifdef  __cplusplus
}//extern "C"
#endif//__cplusplus
#endif//__CLIP_HASH_H__
