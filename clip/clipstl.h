/*
 *	clipstl.h
 *
 *	STL���Q�l�ɂ��č쐬�����֐�
 *
 *	CLiP - Common Library for P/ECE
 *	Copyright (C) 2016 Naoyuki Sawa
 *
 *	* Sat Mar 05 21:53:08 JST 2016 Naoyuki Sawa
 *	- 1st �����[�X�B
 *	- lower_bound()�֐�,upper_bound()�֐����쐬���܂����B
 *	- �X���b�h�Z�[�t�ł�lower_bound_r()�֐�,upper_bound_r()���ǉ����܂����B
 *	* Tue Jun 07 22:13:11 JST 2016 Naoyuki Sawa
 *	- std_rotate()�֐���ǉ����܂����B
 *	  �ړI�́A���������[�e�[�g�����̍������̂��߂ł��B
 *	  �ڍׂ́A�ȉ��̒ʂ�ł��B
 *	- ����Aclipstr.c���W���[����memrotate()�֐���ǉ������̂ł����A�f���ȃA���S���Y���ł͂���̂ł����A���܂����ᑬ�ł����B
 *	  memrotate()���Ăяo���Ă���memmove()�����������铙���čH�v�����̂ł����A���܂���ʂ͗L��܂���ł����B(memmove()���������o�������͂���P�̂ł��Ӌ`�͗L�����̂ł����c)
 *	  �������@��T���āAWeb�Ń��������[�e�[�g�����̃A���S���Y���ɂ��Č������Č������A���ɁA�W��C++���C�u�����ɁAstd::rotate()�Ƃ����֐����L�鎖������܂����B
 *	  �ucppreference.com - std::rotate�v(http://en.cppreference.com/w/cpp/algorithm/rotate)�ɁA���̎������@��������Ă���A�ƂĂ������̗ǂ��A���S���Y���̂悤�ł��B
 *	  �����ŁA���̃A���S���Y�����AC����łƂ��Ď������鎖�ɂ��܂����B
 *	  ���ځAclipstr.c���W���[����memrotate()��ύX���Ă��ǂ��̂ł����A�����ĕʊ֐�std_rotate()�Ƃ��Ď������Amemrotate()��std_rotate()���Ăяo���悤�ɕύX���鎖�ɂ��܂����B
 *	  �����������R�́ASTL��W��C++���C�u���������ɂ��č쐬�����R�[�h�́A�����W���[��(clipstl.c)�ɏW�߂���j������ł��B
 *	- ���g�p��
 *	  ��void test() {
 *	  ��    int a[] = { 0,1,2,3,4,5,6,7,8,9 }, i;
 *	  ��    std_rotate(a, &a[3], &a[10]);
 *	  ��    for(i = 0; i < 10; i++) { printf("%d ", a[i]); }
 *	  ��    //����:     3 4 5 6 7 8 9 0 1 2
 *	  ��}
 *	- �����W���[���ɒǉ�����֐�����'std_'�̃v���t�B�N�X��t������j�ł͂Ȃ������̂ł����A'rotate'�����ł͂��܂�ɒP���߂��Ė��O�̏Փ˂��S�z�Ȃ̂ŁA���ʂ�'std_'�̃v���t�B�N�X��t���鎖�ɂ��܂����B
 *	  �ȑO�ɒǉ�����lower_bound()�֐����ɂ�'std_'�̃v���t�B�N�X�͕t���Ă��Ȃ������̂œ��ꂪ���Ă��܂��񂪁A���p��͖�薳���Ǝv���܂��B
 *	  �����W���[���ɒǉ�����֐��̊֐����ɂ��ẮA���܂茵���ɍl���Ȃ����ɂ��܂����B
 *	  ������A�P�[�X�o�C�P�[�X�ŁA'std_'�̃v���t�B�N�X��t���邩�ǂ������f���ĉ������B
 *	* Wed Jun 08 21:50:02 JST 2016 Naoyuki Sawa
 *	- std::rotate()�̃A���S���Y���������o�����̂ŁA�R�����g��ǋL���܂����B
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
