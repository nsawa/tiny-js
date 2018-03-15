/*
 *	clipstl.c
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
 *	* Sun Sep 04 21:41:17 JST 2016 Naoyuki Sawa
 *	- std_rotate()���A(int)�ŃA�h���X���Z���s���Ă����̂��A(intptr_t)�ōs���悤�ɏC�����܂����B
 *	  64�r�b�g�Ή��̂��߂̏����ł����A32�r�b�g�ł�(intptr_t)�ōs�������{���͐���������ł��B
 *	  �Ƃ͌����A32�r�b�g�ł͂ǂ���ł��������ʂɂȂ�̂ŁA������32�r�b�g�R�[�h�ɂ͉e���L��܂���B
 */
#include "clip.h"
/****************************************************************************
 *	
 ****************************************************************************/
// * Sat Mar 05 21:53:08 JST 2016 Naoyuki Sawa
// - lower_bound()�֐�,upper_bound()�֐���ǉ����܂����B
//   �ړI�́A'�l�͈͕̔t��bsearch()'���s�����������L��������ł��B
// - lower_bound,upper_bound�̊֐����́ASTL��std::lower_bound(),std::upper_bound()������܂����B
//   ����ǉ�����lower_bound()�֐�,upper_bound()�֐��́Absearch()�̈����d�l/�߂�l�d�l�ŁASTL��std::lower_bound(),std::upper_bound()�Ɠ����������s�����̂ł��B
// - ���Q�l����
//   ���uPrograming Place�v(http://ppp-lab.sakura.ne.jp/)����́wC++��(�W�����C�u����) ��20�� �\�[�g�ς݂͈̔͂������A���S���Y���x(http://ppp-lab.sakura.ne.jp/cpp/library/020.html)�̋L��
//   ���uStack Overflow�v�́wFind the first element in an array that is greater than the target�x(http://stackoverflow.com/questions/6553970/find-the-first-element-in-an-array-that-is-greater-than-the-target)�̋L��
// - �X���b�h�Z�[�t�ł�lower_bound_r()�֐�,upper_bound_r()���ǉ����܂����B
//   qsort()�ɑ΂���Aqsort_r()�ɑ������܂��B
//   ���Ȃ݂ɁAbsearch()�ɑ΂���bsearch_r()�͑��݂��܂���B
//   bsearch_r,lfind_r,lsearch_r,�����݂��Ȃ��̂́AC�W�����C�u�����d�l�������Ă�����̂悤�ł��B
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
//(key��val)�ƂȂ�A�ŏ���val�ւ̃|�C���^��Ԃ��B
//�S�Ă�val��(key��val)�Ȃ�΁A�Ō��val�̎��ւ̃|�C���^��Ԃ��B	����������std::lower_bound()�̎d�l�ł��BNULL��Ԃ��̂ł͂Ȃ����ɒ��ӂ��ĉ������B
//[in]
//		�����d�l�́Absearch()�Ɠ����ł��B
//		�����Ɏw�肷��compar�֐��́Absearch()�Ɠ������̂����p�o���܂��B
//[out]
//		�߂�l�̎d�l�́Astd::lower_bound()�Ɠ����ł��B
//		���֐����ANULL��Ԃ����͗L��܂���B
//[note]
//		- �����𖞂����ʒu�����������ꍇ�Absearch()��NULL��Ԃ��܂����A���֐��͍Ō��val�̎��ւ̃|�C���^��Ԃ����ɒ��ӂ��ĉ������B
//		  std::lower_bound()�̎d�l�ɕ��������ł��B
//		- �����𖞂����ʒu�����������ꍇ�Absearch()�Ɠ��l��NULL��Ԃ��d�l�ɕύX���悤���Ƃ��v�����̂ł����A
//		  ���ۂɎg���Č���ƁA�Ăяo�����̊ϓ_�ł��A�Ō��val�̎��ւ̃|�C���^���Ԃ��ꂽ�����g���Ղ��P�[�X�����������킩��܂����B
//		  �]���āA�����𖞂����ʒu�����������ꍇ�́Astd::lower_bound()�Ɠ��l�ɁA�Ō��val�̎��ւ̃|�C���^��Ԃ����ɂ��܂����B
void* lower_bound(const void* key, const void* _base, size_t nmemb, size_t size, int (*compar)(const void*, const void*)) {
	return lower_bound_r(key, _base, nmemb, size, (int (*)(const void*, const void*, void*))compar, NULL);	//lower_bound()��lower_bound_r()�́Acompar�֐��̌`�����قȂ�܂����A'cdecl�ďo�K��'�Ȃ�΁Alower_bound()�`����compar�֐��ɂ����āA�]���Ȉ��������������̂Ŗ�肠��܂���B
}
void* lower_bound_r(const void* key, const void* _base, size_t nmemb, size_t size, int (*compar)(const void*, const void*, void*), void* arg) {
	unsigned char* base = (unsigned char*)_base;
	unsigned lo = 0, hi = nmemb;
	while(lo != hi) {
		unsigned mid = (lo + hi) >> 1;
		if((*compar)(key, base + (size * mid), arg) >  0) {	//���̈ʒu�̒l���L�[�����Ȃ�΁A���̈ʒu�͏����𖞂����Ă��Ȃ��̂Łc	����������lower_bound()��upper_bound()�̈Ⴂ�͂��̍s�����ł��B
			lo = mid + 1;					//���ʂ̈ʒu�́A���Ȃ��Ƃ��A���̈ʒu�̎��ȍ~�ƂȂ�B
		} else {						//���̈ʒu�̒l���L�[�ȏ�Ȃ�΁A���̈ʒu�͏����𖞂����Ă���̂Łc
			hi = mid;					//���ʂ̈ʒu�́A���Ȃ��Ƃ��A���̈ʒu�ȑO�ƂȂ�B
		}
	}
	return base + (size * lo);	//�����𖞂����ʒu���L�����ꍇ�́A(lo==hi��nmemb)�ƂȂ��Ă���B�����𖞂����ʒu�����������ꍇ�́A(lo==hi==nmemb)�ƂȂ��Ă���B
}
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
//(key��val)�ƂȂ�A�ŏ���val�ւ̃|�C���^��Ԃ��B
//�S�Ă�val��(key��val)�Ȃ�΁A�Ō��val�̎��ւ̃|�C���^��Ԃ��B	����������std::upper_bound()�̎d�l�ł��BNULL��Ԃ��̂ł͂Ȃ����ɒ��ӂ��ĉ������B
//[note]
//	- lower_bound()�̐������Q�Ƃ��ĉ������B
void* upper_bound(const void* key, const void* _base, size_t nmemb, size_t size, int (*compar)(const void*, const void*)) {
	return upper_bound_r(key, _base, nmemb, size, (int (*)(const void*, const void*, void*))compar, NULL);	//upper_bound()��upper_bound_r()�́Acompar�֐��̌`�����قȂ�܂����A'cdecl�ďo�K��'�Ȃ�΁Aupper_bound()�`����compar�֐��ɂ����āA�]���Ȉ��������������̂Ŗ�肠��܂���B
}
void* upper_bound_r(const void* key, const void* _base, size_t nmemb, size_t size, int (*compar)(const void*, const void*, void*), void* arg) {
	unsigned char* base = (unsigned char*)_base;
	unsigned lo = 0, hi = nmemb;
	while(lo != hi) {
		unsigned mid = (lo + hi) >> 1;
		if((*compar)(key, base + (size * mid), arg) >= 0) {	//���̈ʒu�̒l���L�[�ȉ��Ȃ�΁A���̈ʒu�͏����𖞂����Ă��Ȃ��̂Łc	����������lower_bound()��upper_bound()�̈Ⴂ�͂��̍s�����ł��B
			lo = mid + 1;					//���ʂ̈ʒu�́A���Ȃ��Ƃ��A���̈ʒu�̎��ȍ~�ƂȂ�B
		} else {						//���̈ʒu�̒l���L�[���߂Ȃ�΁A���̈ʒu�͏����𖞂����Ă���̂Łc
			hi = mid;					//���ʂ̈ʒu�́A���Ȃ��Ƃ��A���̈ʒu�ȑO�ƂȂ�B
		}
	}
	return base + (size * lo);	//�����𖞂����ʒu���L�����ꍇ�́A(lo==hi��nmemb)�ƂȂ��Ă���B�����𖞂����ʒu�����������ꍇ�́A(lo==hi==nmemb)�ƂȂ��Ă���B
}
/****************************************************************************
 *	
 ****************************************************************************/
// * Tue Jun 07 22:13:11 JST 2016 Naoyuki Sawa
// - std_rotate()�֐���ǉ����܂����B
//   �ړI�́A���������[�e�[�g�����̍������̂��߂ł��B
//   �ڍׂ́A�ȉ��̒ʂ�ł��B
// - ����Aclipstr.c���W���[����memrotate()�֐���ǉ������̂ł����A�f���ȃA���S���Y���ł͂���̂ł����A���܂����ᑬ�ł����B
//   memrotate()���Ăяo���Ă���memmove()�����������铙���čH�v�����̂ł����A���܂���ʂ͗L��܂���ł����B(memmove()���������o�������͂���P�̂ł��Ӌ`�͗L�����̂ł����c)
//   �������@��T���āAWeb�Ń��������[�e�[�g�����̃A���S���Y���ɂ��Č������Č������A���ɁA�W��C++���C�u�����ɁAstd::rotate()�Ƃ����֐����L�鎖������܂����B
//   �ucppreference.com - std::rotate�v(http://en.cppreference.com/w/cpp/algorithm/rotate)�ɁA���̎������@��������Ă���A�ƂĂ������̗ǂ��A���S���Y���̂悤�ł��B
//   �����ŁA���̃A���S���Y�����AC����łƂ��Ď������鎖�ɂ��܂����B
//   ���ځAclipstr.c���W���[����memrotate()��ύX���Ă��ǂ��̂ł����A�����ĕʊ֐�std_rotate()�Ƃ��Ď������Amemrotate()��std_rotate()���Ăяo���悤�ɕύX���鎖�ɂ��܂����B
//   �����������R�́ASTL��W��C++���C�u���������ɂ��č쐬�����R�[�h�́A�����W���[��(clipstl.c)�ɏW�߂���j������ł��B
// - ���g�p��
//   ��void test() {
//   ��    int a[] = { 0,1,2,3,4,5,6,7,8,9 }, i;
//   ��    std_rotate(a, &a[3], &a[10]);
//   ��    for(i = 0; i < 10; i++) { printf("%d ", a[i]); }
//   ��    //����:     3 4 5 6 7 8 9 0 1 2
//   ��}
// - �����W���[���ɒǉ�����֐�����'std_'�̃v���t�B�N�X��t������j�ł͂Ȃ������̂ł����A'rotate'�����ł͂��܂�ɒP���߂��Ė��O�̏Փ˂��S�z�Ȃ̂ŁA���ʂ�'std_'�̃v���t�B�N�X��t���鎖�ɂ��܂����B
//   �ȑO�ɒǉ�����lower_bound()�֐����ɂ�'std_'�̃v���t�B�N�X�͕t���Ă��Ȃ������̂œ��ꂪ���Ă��܂��񂪁A���p��͖�薳���Ǝv���܂��B
//   �����W���[���ɒǉ�����֐��̊֐����ɂ��ẮA���܂茵���ɍl���Ȃ����ɂ��܂����B
//   ������A�P�[�X�o�C�P�[�X�ŁA'std_'�̃v���t�B�N�X��t���邩�ǂ������f���ĉ������B
//
//------------------------------------------------------------------------------------------------------------------------------
//
//	���ucppreference.com - std::rotate�v(http://en.cppreference.com/w/cpp/algorithm/rotate)�����p:
//	��template<class ForwardIt> void rotate(ForwardIt first, ForwardIt n_first, ForwardIt last);
//	��
//	��Parameters
//	��	first		the beginning of the original range
//	��	n_first		the element that should appear at the beginning of the rotated range
//	��	last		the end of the original range
//	��
//	��Possible implementation
//	��
//	��	Possible implementation
//	��	template<class ForwardIt> void rotate(ForwardIt first, ForwardIt n_first, ForwardIt last) {
//	��		ForwardIt next = n_first;
//	��		while(first != next) {
//	��			std::iter_swap(first++, next++);
//	��			if(next == last) {
//	��				next = n_first;
//	��			} else if(first == n_first) {
//	��				n_first = next;
//	��			}
//	��		}
//	��	}
//
//	��Ƃ��āA
//		std::vector<int> v = {1,2,3,4,5};	//C++11�Ȃ��std::vector<int> v{1,2,3,4,5};�Ə����Ă��ǂ��B(Uniform Initialization)
//	�ɑ΂��āA
//		std::rotate(v.begin(), v.begin() + 2, v.end());
//	���s�������̓����}�����܂��B
//
//	��	�e: first
//	��	NF: n_first
//	��	�k: last
//	��	�m: next
//	��
//	��	�@�������
//	��
//	��		�@�@�@�@�@NF�@�@�@�@�@�k
//	��		�����������������������@
//	��		���P���Q���R���S���T���@
//	��		�����������������������@
//	��		�@�e�@�@�@�m�@�@�@�@�@�@
//	��
//	��	�A�e�̈ʒu�̒l�ƁA�m�̈ʒu�̒l���������A�e�Ƃm��i�߂�B
//	��
//	��		�@�@�@�@�@NF�@�@�@�@�@�k
//	��		�����������������������@
//	��		���R���Q���P���S���T���@
//	��		�����������������������@
//	��		�@�@�@�e�@�@�@�m�@�@�@�@
//	��
//	��	�B�e�̈ʒu�̒l�ƁA�m�̈ʒu�̒l���������A�e�Ƃm��i�߂�B
//	��
//	��		�@�@�@�@�@NF�@�@�@�@�@�k
//	��		�����������������������@
//	��		���R���S���P���Q���T���@
//	��		�����������������������@
//	��		�@�@�@�@�@�e�@�@�@�m�@�@
//	��
//	��	�C�e==NF�ɂȂ����̂ŁANF���m�̈ʒu�ֈړ�����B
//	��
//	��		�@�@�@�@�@�@�@�@�@NF�@�k
//	��		�����������������������@
//	��		���R���S���P���Q���T���@
//	��		�����������������������@
//	��		�@�@�@�@�@�e�@�@�@�m�@�@
//	��
//	��	�D�e�̈ʒu�̒l�ƁA�m�̈ʒu�̒l���������A�e�Ƃm��i�߂�B
//	��
//	��		�@�@�@�@�@�@�@�@�@NF�@�k
//	��		�����������������������@
//	��		���R���S���T���Q���P���@
//	��		�����������������������@
//	��		�@�@�@�@�@�@�@�e�@�@�@�m
//	��
//	��	�E�m==�k�ɂȂ����̂ŁA�m��NF�̈ʒu�ֈړ�����B
//	��
//	��		�@�@�@�@�@�@�@�@�@NF�@�k
//	��		�����������������������@
//	��		���R���S���T���Q���P���@
//	��		�����������������������@
//	��		�@�@�@�@�@�@�@�e�@�m�@�@
//	��
//	��	�F�e�̈ʒu�̒l�ƁA�m�̈ʒu�̒l���������A�e�Ƃm��i�߂�B
//	��
//	��		�@�@�@�@�@�@�@�@�@NF�@�k
//	��		�����������������������@
//	��		���R���S���T���P���Q���@
//	��		�����������������������@
//	��		�@�@�@�@�@�@�@�@�@�e�@�m
//	��
//	��	�G�m==�k�ɂȂ����̂ŁA�m��NF�̈ʒu�ֈړ�����B
//	��
//	��		�@�@�@�@�@�@�@�@�@NF�@�k
//	��		�����������������������@
//	��		���R���S���T���P���Q���@
//	��		�����������������������@
//	��		�@�@�@�@�@�@�@�@�@�e�@�@
//	��		�@�@�@�@�@�@�@�@�@�m�@�@
//	��
//	��	�H�e==�m�ɂȂ����̂ŏI������B
//
//	���r���̕����́A�l�����肵���ӏ��������܂��B
//	�ŏ��̗v�f����l�����肵�čs���A��x���肵���v�f�͕����񏑂������鎖�͖����̂ŁA�����I�ł��B
//	�������A�F�`�G�̕������A�܂����ϓI�ɗ����o���Ă��܂���B
//	����̗v�f�́A�ꎞ�I�ɓ���ւ����Č��̕��тƈ���Ă���̂ɁA��L�̃A���S���Y���łȂ����������тɖ߂���̂ł��傤���c
//	�Ƃ͌����A�W��C++���C�u�����ł�����A���S���Y�������������͊ԈႢ�Ȃ��̂ŁA���̂܂܎������鎖�ɂ��܂����B
//	�ǁX�A�������悤�Ǝv���܂��B	��{{2016/06/08�ǋL:std::rotate()�̃A���S���Y���������o�����̂ŁA�R�����g��ǋL���܂����B���̂��������Q�Ƃ��ĉ������B}}
//
// * Wed Jun 08 21:50:02 JST 2016 Naoyuki Sawa
// - std::rotate()�̃A���S���Y���������o���܂����B
//   �y������Ԃ��`���a�ł���ꍇ�z�y������Ԃ��`���a�ł���ꍇ�z�y������Ԃ��`���a�ł���ꍇ�z�ɕ����čl���Ĉ�菇�����s���A���̌�A�c�����͈͂ɑ΂���std::rotate()�̏������ċA���s����ƍl����Δ���Ղ��ł��B
//   �ڍׂ́A���̉��̐}���Q�Ƃ��ĉ������B
//
//	��	�y������Ԃ��`���a�ł���ꍇ�z
//	��
//	��		�u������ԁv
//	��
//	��			�@�@�@�@NF�@�@�@�k
//	��			������������������
//	��			���P���Q���R���S��
//	��			������������������
//	��			�e�@�@�@�m�@�@�@�@
//	��			�����`�������a����
//	��
//	��		�u�ŏI��ԁv
//	��
//	��			������������������
//	��			���R���S���P���Q��
//	��			������������������
//	��			�����a�������`����
//	��
//	��			�u������ԁv�̂a��擪�ֈړ����A���̕��`�����ւ��炷�ƁA�u�ŏI��ԁv�ɂȂ�܂��B
//	��
//	��
//	��		�`�`�`�菇�`�`�`
//	��
//	��			�@�@�@�@NF�@�@�@�k
//	��			������������������
//	��			���P���Q���R���S��
//	��			������������������
//	��			�e�@�@�@�m�@�@�@�@
//	��			�����`�������a����
//	��
//	��			�@�@�@�@NF�@�@�@�k
//	��			������������������
//	��			���R���Q���P���S��
//	��			������������������
//	��			�@�@�e�@�@�@�m�@�@
//	��
//	��			�@�@�@�@NF�@�@�@�k
//	��			������������������
//	��			���R���S���P���Q��
//	��			������������������
//	��			�@�@�@�@�e�@�@�@�m
//	��
//	��			�@�@�@�@NF�@�@�@�k
//	��			������������������
//	��			���R���S���P���Q��
//	��			������������������
//	��			�@�@�@�@�e�@�@�@�@
//	��			�@�@�@�@�m�@�@�@�@
//	��			�����a�������`����
//	��
//	��			�`���a�����������̂ŁA�����ł��B
//	��
//	��	�y������Ԃ��`���a�ł���ꍇ�z
//	��
//	��		�u������ԁv
//	��
//	��			�@�@�@�@�@�@NF�@�@�@�k
//	��			����������������������
//	��			���P���Q���R���S���T��
//	��			����������������������
//	��			�e�@�@�@�@�@�m�@�@�@�@
//	��			�������`���������a����
//	��
//	��		�u�ŏI��ԁv
//	��
//	��			����������������������
//	��			���S���T���P���Q���R��
//	��			����������������������
//	��			�����a���������`������
//	��
//	��			�u������ԁv�̂a��擪�ֈړ����A���̕��`�����ւ��炷�ƁA�u�ŏI��ԁv�ɂȂ�܂��B
//	��
//	��
//	��		�`�`�`�菇�`�`�`
//	��
//	��			�@�@�@�@�@�@NF�@�@�@�k
//	��			����������������������
//	��			���P���Q���R���S���T��
//	��			����������������������
//	��			�e�@�@�@�@�@�m�@�@�@�@
//	��			�������`���������a����
//	��
//	��			�@�@�@�@�@�@NF�@�@�@�k
//	��			����������������������
//	��			���S���Q���R���P���T��
//	��			����������������������
//	��			�@�@�e�@�@�@�@�@�m�@�@
//	��
//	��			�@�@�@�@�@�@NF�@�@�@�k
//	��			����������������������
//	��			���S���T���R���P���Q��
//	��			����������������������
//	��			�@�@�@�@�e�@�@�@�@�@�m
//	��
//	��			�@�@�@�@�@�@NF�@�@�@�k
//	��			����������������������
//	��			���S���T���R���P���Q��
//	��			����������������������
//	��			�@�@�@�@�e�@�m�@�@�@�@
//	��			�����a��������A'������
//	��
//	��			���̎��_�ŁA�a�͊������Ă���BA'�ɑ΂��āAstd::rotate()�̏������ċA���s����Ηǂ��B
//	��
//	��	�y������Ԃ��`���a�ł���ꍇ�z
//	��
//	��		�u������ԁv
//	��
//	��			�@�@�@�@NF�@�@�@�@�@�k
//	��			����������������������
//	��			���P���Q���R���S���T��
//	��			����������������������
//	��			�e�@�@�@�m�@�@�@�@�@�@
//	��			�����`���������a������
//	��
//	��		�u�ŏI��ԁv
//	��
//	��			����������������������
//	��			���R���S���T���P���Q��
//	��			����������������������
//	��			�������a���������`����
//	��
//	��			�u������ԁv�̂a��擪�ֈړ����A���̕��`�����ւ��炷�ƁA�u�ŏI��ԁv�ɂȂ�܂��B
//	��
//	��
//	��		�`�`�`�菇�`�`�`
//	��
//	��			�@�@�@�@NF�@�@�@�@�@�k
//	��			����������������������
//	��			���P���Q���R���S���T��
//	��			����������������������
//	��			�e�@�@�@�m�@�@�@�@�@�@
//	��			�����`���������a������
//	��
//	��			�@�@�@�@NF�@�@�@�@�@�k
//	��			����������������������
//	��			���R���Q���P���S���T��
//	��			����������������������
//	��			�@�@�e�@�@�@�m�@�@�@�@
//	��
//	��			�@�@�@�@NF�@�@�@�@�@�k
//	��			����������������������
//	��			���R���S���P���Q���T��
//	��			����������������������
//	��			�@�@�@�@�e�@�@�@�m�@�@
//	��
//	��			�@�@�@�@�@�@�@�@NF�@�k
//	��			����������������������
//	��			���R���S���P���Q���T��
//	��			����������������������
//	��			�@�@�@�@�e�@�@�@�m�@�@
//	��			����B'��������A'������
//	��
//	��			���̎��_�ŁAB'�͊������Ă���BA'�ɑ΂��āAstd::rotate()�̏������ċA���s����Ηǂ��B
//	��
//------------------------------------------------------------------------------------------------------------------------------
#ifndef PIECE
static void std_rotate_word(int* first, int* n_first, int* last) {
	int* next = n_first;
    //{{���͈̔͂�C����\�[�X�́Astd_rotate_byte()/std_rotate_half()/std_rotate_word()�ŋ��ʂł��B
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
    //}}���͈̔͂�C����\�[�X�́Astd_rotate_byte()/std_rotate_half()/std_rotate_word()�ŋ��ʂł��B
}
static void std_rotate_half(short* first, short* n_first, short* last) {
	short* next = n_first;
    //{{���͈̔͂�C����\�[�X�́Astd_rotate_byte()/std_rotate_half()/std_rotate_word()�ŋ��ʂł��B
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
    //}}���͈̔͂�C����\�[�X�́Astd_rotate_byte()/std_rotate_half()/std_rotate_word()�ŋ��ʂł��B
}
static void std_rotate_byte(char* first, char* n_first, char* last) {
	char* next = n_first;
    //{{���͈̔͂�C����\�[�X�́Astd_rotate_byte()/std_rotate_half()/std_rotate_word()�ŋ��ʂł��B
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
    //}}���͈̔͂�C����\�[�X�́Astd_rotate_byte()/std_rotate_half()/std_rotate_word()�ŋ��ʂł��B
}
void std_rotate(void* first, void* n_first, void* last) {
//{{2016/09/04�ύX:std_rotate()���A(int)�ŃA�h���X���Z���s���Ă����̂��A(intptr_t)�ōs���悤�ɏC�����܂����B
//	int x = (int)first | (int)n_first | (int)last;
//��2016/09/04�ύX:std_rotate()���A(int)�ŃA�h���X���Z���s���Ă����̂��A(intptr_t)�ōs���悤�ɏC�����܂����B
	intptr_t x = (intptr_t)first | (intptr_t)n_first | (intptr_t)last;
//}}2016/09/04�ύX:std_rotate()���A(int)�ŃA�h���X���Z���s���Ă����̂��A(intptr_t)�ōs���悤�ɏC�����܂����B
	if(!(x & 3)) {
		std_rotate_word(first, n_first, last);	//first,n_first,last���S�ă��[�h���񂵂Ă���ꍇ
	} else if(!(x & 1)) {
		std_rotate_half(first, n_first, last);	//first,n_first,last���S�ăn�[�t���[�h���񂵂Ă���ꍇ
	} else {
		std_rotate_byte(first, n_first, last);	//first,n_first,last����ł���A�h���X�̏ꍇ
	}
}
#else //PIECE
void std_rotate(void* first, void* n_first, void* last);
//�����������؃v���O�����B�e�o�[�W�����ɋL�ڂ������Ԃ́A���̃v���O�����Ōv���������Ԃł��B
//��int test() {
//��  static int buf[16384];
//��  int i, t1, t2, t3;
//��  t1 = pceTimerGetCount();
//��  for(i = 0; i < 100; i++) { std_rotate(&buf[0], &buf[1], &buf[16384]); }		//�ǂ̃o�[�W�����ł��������x�ł��B
//��  t1 = pceTimerGetCount() - t1;
//��  t2 = pceTimerGetCount();
//��  for(i = 0; i < 100; i++) { std_rotate(&buf[0], &buf[16384/2], &buf[16384]); }	//'�������O�̃o�[�W����'���ᑬ�ŁA'�������o�[�W����1'��'�������o�[�W����2'�������ł��B
//��  t2 = pceTimerGetCount() - t2;
//��  t3 = pceTimerGetCount();
//��  for(i = 0; i < 100; i++) { std_rotate(&buf[0], &buf[16384-1], &buf[16384]); }	//'�������O�̃o�[�W����'��'�������o�[�W����1'���ᑬ�ŁA'�������o�[�W����2'�������ł��B
//��  t3 = pceTimerGetCount() - t3;
//��  die("%d[ms]\n%d[ms]\n%d[ms]", t1, t2, t3);
//��}
//�ȏ�̌��،��ʂ��A'�������o�[�W����2'���ő��ł��鎖������A'�������o�[�W����2'���̗p���鎖�ɂ��܂����B
//���A�R�[�h�T�C�Y�́A'�������O�̃o�[�W����'��3���ߏ��Ȃ��A'�������o�[�W����1'��'�������o�[�W����2'�͓����R�[�h�T�C�Y�ł��B
//3���ߒ��x�̑����Ȃ�΋��e�͈͂��Ǝv���̂ŁA'�������o�[�W����2'���̗p���鎖�ɖ��͗L��܂���B
#if 0
//���������O�̃o�[�W����	2830[ms] 1347[ms] 2554[ms]
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
//���������o�[�W����1		2830[ms] 1278[ms] 2554[ms]
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
		cmp		%r12, %r15			;//  	������������������������������������������
std_rotate_byte_LOOP:						;//for(;;) {					��
	;//	cmp		%r12, %r15			;//  if(first == next) { return }	����������	���[�v���̒x���X���b�g�����p���邽�߂ɂ��̍s�̖��߂�񂩏��ɕ������B�����R�[�h�T�C�Y�������邪���x��D�悷�鎖�ɂ����B
		jreq		std_rotate_RET			;//  						��
		ld.b		%r4, [%r12]			;//  %r4  := tmp1 = *first			��
		ld.b		%r5, [%r15]			;//  %r5  := tmp2 = *next			��
		ld.b		[%r15]+, %r4			;//  *next++  = tmp1				��		*anti-interlock*
		ld.b		[%r12]+, %r5			;//  *first++ = tmp2				��
		cmp		%r15, %r14			;//  if(next == last) { next = n_first }	��
		jreq		std_rotate_byte			;//  						��
		cmp		%r12, %r13			;//  if(first != n_first) { continue }		��
		jrne.d		std_rotate_byte_LOOP		;//  						��
		cmp		%r12, %r15			;//  	������������������������������������������		*delay*
		jp.d		std_rotate_byte_LOOP		;//}
		ld.w		%r13, %r15			;//  %r13 := n_first = next					*delay*
		;//---------------------------------------------;//
std_rotate_half:
		ld.w		%r15, %r13			;//%r15 := next = n_first
		cmp		%r12, %r15			;//  	������������������������������������������
std_rotate_half_LOOP:						;//for(;;) {					��
	;//	cmp		%r12, %r15			;//  if(first == next) { return }	����������	���[�v���̒x���X���b�g�����p���邽�߂ɂ��̍s�̖��߂�񂩏��ɕ������B�����R�[�h�T�C�Y�������邪���x��D�悷�鎖�ɂ����B
		jreq		std_rotate_RET			;//  						��
		ld.h		%r4, [%r12]			;//  %r4  := tmp1 = *first			��
		ld.h		%r5, [%r15]			;//  %r5  := tmp2 = *next			��
		ld.h		[%r15]+, %r4			;//  *next++  = tmp1				��		*anti-interlock*
		ld.h		[%r12]+, %r5			;//  *first++ = tmp2				��
		cmp		%r15, %r14			;//  if(next == last) { next = n_first }	��
		jreq		std_rotate_half			;//  						��
		cmp		%r12, %r13			;//  if(first != n_first) { continue }		��
		jrne.d		std_rotate_half_LOOP		;//  						��
		cmp		%r12, %r15			;//  	������������������������������������������		*delay*
		jp.d		std_rotate_half_LOOP		;//}
		ld.w		%r13, %r15			;//  %r13 := n_first = next					*delay*
		;//---------------------------------------------;//
std_rotate_word:
		ld.w		%r15, %r13			;//%r15 := next = n_first
		cmp		%r12, %r15			;//  	������������������������������������������
std_rotate_word_LOOP:						;//for(;;) {					��
	;//	cmp		%r12, %r15			;//  if(first == next) { return }	����������	���[�v���̒x���X���b�g�����p���邽�߂ɂ��̍s�̖��߂�񂩏��ɕ������B�����R�[�h�T�C�Y�������邪���x��D�悷�鎖�ɂ����B
		jreq		std_rotate_RET			;//  						��
		ld.w		%r4, [%r12]			;//  %r4  := tmp1 = *first			��
		ld.w		%r5, [%r15]			;//  %r5  := tmp2 = *next			��
		ld.w		[%r15]+, %r4			;//  *next++  = tmp1				��		*anti-interlock*
		ld.w		[%r12]+, %r5			;//  *first++ = tmp2				��
		cmp		%r15, %r14			;//  if(next == last) { next = n_first }	��
		jreq		std_rotate_word			;//  						��
		cmp		%r12, %r13			;//  if(first != n_first) { continue }		��
		jrne.d		std_rotate_word_LOOP		;//  						��
		cmp		%r12, %r15			;//  	������������������������������������������		*delay*
		jp.d		std_rotate_word_LOOP		;//}
		ld.w		%r13, %r15			;//  %r13 := n_first = next					*delay*
		;//---------------------------------------------;//
std_rotate_RET:
		ret
");
#elif 1
//���������o�[�W����2		2830[ms] 1278[ms] 2416[ms]	����������������̗p���鎖�Ɍ��肵�܂����B'�������o�[�W����2'��'�������o�[�W����1'�����ɂ���ɒx���X���b�g�����p�������̂ł����A'�������o�[�W����2'�̃R�[�h������ǂނƗ������h���Ǝv���܂��B'�������o�[�W����1'��'�������o�[�W����2'���r���Č���΁A'�������o�[�W����2'�̃R�[�h�������o����Ǝv���܂��B
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
		cmp		%r12, %r13			;//%psr := first <=> n_first	������������������
std_rotate_byte_NEXT:						;//						��
		ld.w		%r15, %r13			;//%r15 := next = n_first			��
std_rotate_byte_LOOP:						;//for(;;) {					��
	;//	cmp		%r12, %r15			;//  if(first == next) { return }	����������	���[�v���̒x���X���b�g�����p���邽�߂ɂ��̍s�̖��߂�񂩏��ɕ������B�����R�[�h�T�C�Y�������邪���x��D�悷�鎖�ɂ����B
		jreq		std_rotate_RET			;//  						��
		ld.b		%r4, [%r12]			;//  %r4  := tmp1 = *first			��
		ld.b		%r5, [%r15]			;//  %r5  := tmp2 = *next			��
		ld.b		[%r15]+, %r4			;//  *next++  = tmp1				��		*anti-interlock*
		ld.b		[%r12]+, %r5			;//  *first++ = tmp2				��
		cmp		%r15, %r14			;//  if(next == last) { next = n_first }	��
		jreq.d		std_rotate_byte_NEXT		;//  						��
		cmp		%r12, %r13			;//  if(first != n_first) { continue }		��		*delay*		����'cmp %r12,%r13'���A�W�����v��ōs���ׂ�'ld.w %r15,%r13; cmp %r12,%r15'�̓��́A'cmp %r12,%r15'�̑�p�ƂȂ�܂��B
		jrne.d		std_rotate_byte_LOOP		;//  						��
		cmp		%r12, %r15			;//  	������������������������������������������		*delay*
		jp.d		std_rotate_byte_LOOP		;//}
		ld.w		%r13, %r15			;//  %r13 := n_first = next					*delay*
		;//---------------------------------------------;//
std_rotate_half:
		cmp		%r12, %r13			;//%psr := first <=> n_first	������������������
std_rotate_half_NEXT:						;//						��
		ld.w		%r15, %r13			;//%r15 := next = n_first			��
std_rotate_half_LOOP:						;//for(;;) {					��
	;//	cmp		%r12, %r15			;//  if(first == next) { return }	����������	���[�v���̒x���X���b�g�����p���邽�߂ɂ��̍s�̖��߂�񂩏��ɕ������B�����R�[�h�T�C�Y�������邪���x��D�悷�鎖�ɂ����B
		jreq		std_rotate_RET			;//  						��
		ld.h		%r4, [%r12]			;//  %r4  := tmp1 = *first			��
		ld.h		%r5, [%r15]			;//  %r5  := tmp2 = *next			��
		ld.h		[%r15]+, %r4			;//  *next++  = tmp1				��		*anti-interlock*
		ld.h		[%r12]+, %r5			;//  *first++ = tmp2				��
		cmp		%r15, %r14			;//  if(next == last) { next = n_first }	��
		jreq.d		std_rotate_half_NEXT		;//  						��
		cmp		%r12, %r13			;//  if(first != n_first) { continue }		��		*delay*		����'cmp %r12,%r13'���A�W�����v��ōs���ׂ�'ld.w %r15,%r13; cmp %r12,%r15'�̓��́A'cmp %r12,%r15'�̑�p�ƂȂ�܂��B
		jrne.d		std_rotate_half_LOOP		;//  						��
		cmp		%r12, %r15			;//  	������������������������������������������		*delay*
		jp.d		std_rotate_half_LOOP		;//}
		ld.w		%r13, %r15			;//  %r13 := n_first = next					*delay*
		;//---------------------------------------------;//
std_rotate_word:
		cmp		%r12, %r13			;//%psr := first <=> n_first	������������������
std_rotate_word_NEXT:						;//						��
		ld.w		%r15, %r13			;//%r15 := next = n_first			��
std_rotate_word_LOOP:						;//for(;;) {					��
	;//	cmp		%r12, %r15			;//  if(first == next) { return }	����������	���[�v���̒x���X���b�g�����p���邽�߂ɂ��̍s�̖��߂�񂩏��ɕ������B�����R�[�h�T�C�Y�������邪���x��D�悷�鎖�ɂ����B
		jreq		std_rotate_RET			;//  						��
		ld.w		%r4, [%r12]			;//  %r4  := tmp1 = *first			��
		ld.w		%r5, [%r15]			;//  %r5  := tmp2 = *next			��
		ld.w		[%r15]+, %r4			;//  *next++  = tmp1				��		*anti-interlock*
		ld.w		[%r12]+, %r5			;//  *first++ = tmp2				��
		cmp		%r15, %r14			;//  if(next == last) { next = n_first }	��
		jreq.d		std_rotate_word_NEXT		;//  						��
		cmp		%r12, %r13			;//  if(first != n_first) { continue }		��		*delay*		����'cmp %r12,%r13'���A�W�����v��ōs���ׂ�'ld.w %r15,%r13; cmp %r12,%r15'�̓��́A'cmp %r12,%r15'�̑�p�ƂȂ�܂��B
		jrne.d		std_rotate_word_LOOP		;//  						��
		cmp		%r12, %r15			;//  	������������������������������������������		*delay*
		jp.d		std_rotate_word_LOOP		;//}
		ld.w		%r13, %r15			;//  %r13 := n_first = next					*delay*
		;//---------------------------------------------;//
std_rotate_RET:
		ret
");
#endif
#endif//PIECE
