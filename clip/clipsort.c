/*	
 *	clipsort.c
 *
 *	CLiP - Common Library for P/ECE
 *	Copyright (C) 2001-2016 Naoyuki Sawa
 *
 *	* Fri Apr 13 00:10:09 JST 2007 Naoyuki Sawa
 *	- �V�K�쐬�B
 *	* Thu Apr 19 11:44:36 JST 2007 Naoyuki Sawa
 *	- ���ɐ[���ċA�Ăяo������������ꍇ���������̂�h�����߂̕ύX���s���܂����B
 *	* Thu Apr 19 14:58:48 JST 2007 Naoyuki Sawa
 *	- ���[�v���ꂩ���Awhile(){...}����do{...}while()�ɕύX���A�������������܂����B
 *	* Sun May 13 14:21:21 JST 2007 Naoyuki Sawa
 *	- �q�[�v�\�[�g�̎������ǉ����܂����B
 *	  �R�����g�A�E�g���Ă���܂��̂ŁA���C�u�����v���O�������̂ɂ͕ω�����܂���B
 *	* Mon Oct 18 21:09:33 JST 2010 Naoyuki Sawa
 *	- msort()��ǉ����܂����B
 *	* Wed Nov 19 20:53:11 JST 2014 Naoyuki Sawa
 *	- qsort_r()��ǉ����Aqsort()��qsort_r()�ւ̌Ăяo���ɕύX���܂����B
 *	  qsort_r()�̐錾�́Ainclude/stdlib.h�ɗL��܂��B
 *	- qsort_r()�́Aglibc�o�[�W����2.8�Œǉ����ꂽ�֐��ł��B
 *	  �ڍׂ́A�uSection: Linux Programmer's Manual (3) - QSORT�v(http://linuxjm.sourceforge.jp/html/LDP_man-pages/man3/qsort.3.html)���Q�Ƃ��Ă��������B
 *	* Thu Jan 08 21:00:51 JST 2015 Naoyuki Sawa
 *	- qsort()��qsort_r()���A�Z���u�������܂����B�R�[�h�T�C�Y��24�o�C�g�������Ȃ�A�������x����4.5%�����Ȃ�܂����B
 *	- Visual C++ 6.0�Ńr���h���鎞�̂��߂ɁAqsort_r()�̐錾��ǉ����܂����B
 *	  P/ECE�Ńr���h���鎞�́Ainclude/stdlib.h��qsort_r()���錾����Ă���̂Ŗ��L��܂��񂪁A
 *	  Visual C++ 6.0�Ńr���h���鎞�́AVisual C++ 6.0��stdlib.h�ɂ�qsort_r()���܂܂�Ă��Ȃ��̂ŁAqsort()����qsort_r()�ւ̑O���Q�Ƃ��x���ɂȂ邽�߂ł��B
 *	  ������qsort_r()��錾���邱�Ƃɂ��AVisual C++ 6.0�Ńr���h���鎞���x�����o�Ȃ��Ȃ�܂����B
 *	  ���AP/ECE�Ńr���h���鎞�������Ő錾����Ă��Ă��Q�͖����̂ł����A�ŏ����Ƃ��邽�߂ɁAP/ECE�Ńr���h���鎞�͂����ł͐錾���Ȃ����Ƃɂ��܂����B
 *	* Fri Jan 09 00:00:00 JST 2015 Naoyuki Sawa
 *	- Asm��qsort()���œK�����܂����B�R�[�h�T�C�Y�������8�o�C�g�������Ȃ�܂����B
 *	* Fri Jun 10 21:42:30 JST 2016 Naoyuki Sawa
 *	- ������hsort()��msort()���Aheapsort()��mergesort()�ɖ��O�ύX���A/clip/include/stdlib.h�Ő錾����悤�ɂ��܂����B
 *	  �ڍׂ́Aclipsort.c�́u�q�[�v�\�[�g�ƃ}�[�W�\�[�g�ɂ���(2016/06/10)�v���Q�Ƃ��ĉ������B
 *	- clipsort.c�̈�ԉ��ɁA�u�e�X�g�X�C�[�g�v��ǋL���܂����B
 *	* Sat Jun 11 20:47:59 JST 2016 Naoyuki Sawa
 *	- clipsort.c�̈�ԉ��ɁA�u���x�v������(2016/06/11)�v��ǋL���܂����B
 *	  Win10(x64)��P/ECE�ŁA�e�֐��̑��x���v���������ʂƁA�l�@�ł��B
 */
#include "clip.h"

/*****************************************************************************
 *	�N�B�b�N�\�[�g
 *****************************************************************************/

//{{2016/06/10�폜:clipstdw.h�Ő錾�����̂ŁA�����ł̐錾�͕s�v�ɂȂ��Ă��܂����B
//#ifdef  _MSC_VER
//// * Thu Jan 08 21:00:51 JST 2015 Naoyuki Sawa
//// - Visual C++ 6.0�Ńr���h���鎞�̂��߂ɁAqsort_r()�̐錾��ǉ����܂����B
////   P/ECE�Ńr���h���鎞�́Ainclude/stdlib.h��qsort_r()���錾����Ă���̂Ŗ��L��܂��񂪁A
////   Visual C++ 6.0�Ńr���h���鎞�́AVisual C++ 6.0��stdlib.h�ɂ�qsort_r()���܂܂�Ă��Ȃ��̂ŁAqsort()����qsort_r()�ւ̑O���Q�Ƃ��x���ɂȂ邽�߂ł��B
////   ������qsort_r()��錾���邱�Ƃɂ��AVisual C++ 6.0�Ńr���h���鎞���x�����o�Ȃ��Ȃ�܂����B
////   ���AP/ECE�Ńr���h���鎞�������Ő錾����Ă��Ă��Q�͖����̂ł����A�ŏ����Ƃ��邽�߂ɁAP/ECE�Ńr���h���鎞�͂����ł͐錾���Ȃ����Ƃɂ��܂����B
//void qsort_r(void* base, size_t num, size_t width, int (*compare)(const void*, const void*, void*), void* arg);
//#endif//_MSC_VER
//}}2016/06/10�폜:clipstdw.h�Ő錾�����̂ŁA�����ł̐錾�͕s�v�ɂȂ��Ă��܂����B

/* * �W��C���C�u������EPSON��qsort()��u�������܂��B
 * - EPSON��qsort()�ɕs�������킯�ł͖����̂ŁA���̂܂܂ł����������삵�܂��B
 *   �������AEPSON��qsort()�ɂ͏������ʂȏ������܂܂�Ă��āA�T�C�Y�Ǝ��s���x�ɖ��ʂ�����܂��B
 *   �N�B�b�N�\�[�g�A���S���Y���̕׋������˂āA������CLiP�ł��������Ă݂邱�Ƃɂ��܂����B
 *   �قƂ��EPSON��qsort()�Ɠ����ŁA�ꕔ�̖��ʂȏ������폜(�֐����̃R�����g�Q��)���������ł��B
 * - EPSON�Ł�CLiP�łւ̒u�������ɂ��A�T�C�Y�ߖ�ƍ������̌��ʂ́A�ȉ��̂Ƃ���ł��B
 *	qsort()+xchg()�S�̂̃��W���[���T�C�Y
 *		EPSON��: 196�o�C�g
 *		CLiP�� : 128�o�C�g (68�o�C�g�ߖ�)
 *	10000�v�f�̃����_����int�z����\�[�g���鎞��
 *		EPSON��: 1852�~���b
 *		CLiP�� : 1682�~���b (��10%������)
 * * Thu Apr 19 11:44:36 JST 2007 Naoyuki Sawa
 * - ���ɐ[���ċA�Ăяo������������ꍇ���������̂�h�����߂̕ύX���s�������Ƃɂ��A
 *   �T�C�Y�Ƒ��x�̓����������ς��܂����B
 *	qsort()+xchg()�S�̂̃��W���[���T�C�Y
 *		EPSON��: 196�o�C�g
 *		CLiP�� : 140�o�C�g (56�o�C�g�ߖ�)
 *	10000�v�f�̃����_����int�z����\�[�g���鎞��
 *		EPSON��: 1852�~���b
 *		CLiP�� : 1645�~���b (��11%������)
 * * Thu Apr 19 14:58:48 JST 2007 Naoyuki Sawa
 * - ���[�v���ꂩ���Awhile(){...}����do{...}while()�ɕύX���A�������������܂����B
 *   �����킸���ɑ����Ȃ�������ǁA�T�C�Y�͌���܂���ł����B
 *	qsort()+xchg()�S�̂̃��W���[���T�C�Y
 *		EPSON��: 196�o�C�g
 *		CLiP�� : 140�o�C�g (56�o�C�g�ߖ�)
 *	10000�v�f�̃����_����int�z����\�[�g���鎞��
 *		EPSON��: 1852�~���b
 *		CLiP�� : 1642�~���b (��11%������)
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
		sub		%r14, 1			;//%psr(Z) := size--	����
		 ld.b		%r10, [%r12]		;//			�@��
		 ld.b		%r11, [%r13]		;//			�@��
		 ld.b		[%r13]+, %r10		;//			�@��	*anti-interlock*
		 ld.b		[%r12]+, %r11		;//			�@��
		jrne.d		-4			;//			����
		sub		%r14, 1			;//%psr(Z) := size--	����	*delay
		ret
");
#endif /*PIECE*/

//-------------------------------------------------------------------------------------------------------------
#ifndef PIECE
//void
//qsort(void* base, size_t num, size_t width, int (*compare)(const void*, const void*))
//{
//	unsigned char* pv;	/* �s�{�b�g�ւ̃|�C���^ */
//	unsigned char* lo;	/* ��ʃ|�C���^ (���ɒ�ʗv�f(�s�{�b�g�ȉ��̗v�f)���i�[����ʒu�ւ̃|�C���^) */
//	unsigned char* hi;	/* ���ʃ|�C���^ (���ɍ��ʗv�f(�s�{�b�g���߂̗v�f)���i�[����ʒu�ւ̃|�C���^) */
//	int lo_cnt;		/* ��ʗv�f�� */
//	int hi_cnt;		/* ���ʗv�f�� */
//
//	/* �v�f����0�܂���1�Ȃ�΁A���������ɋA��܂��B
//	 * - EPSON�ł́A2�v�f�̏ꍇ�����ʈ������āA�P���Ȕ�r�Ɠ���ւ����s���Ă��܂����A���̕K�v�͂���܂���B
//	 *   2�v�f�̏ꍇ���A����ȏ�̗v�f���̏ꍇ�Ɠ����A�ʏ�̏����őΉ��ł��܂��B
//	 */
//	if(num <= 1) {
//		return;
//	}
//
//	/* �|�C���^�Ɨv�f�������������܂��B
//	 * - EPSON�ł́A�z��̒����t�߂Ɛ擪�̗v�f�����ւ��A���Ƃ��ƒ����t�߂ɂ������l���s�{�b�g�Ƃ��Ă��܂��B
//	 *   �����炠����x�����ɕ���ł���Ɖ��肵�āA�����t�߂ɒ����l�����邱�Ƃ����҂��Ă���̂��Ǝv���܂��B
//	 *   ���������ۂɂ͂��܂���ʂ������A�ނ���]�v�ȓ���ւ��̉񐔂������镪�A���\�𗎂Ƃ��Ă���悤�ł��B
//	 *   ������CLiP�łł́A�P���ɐ擪�̗v�f���s�{�b�g�Ƃ��č̗p���邱�Ƃɂ��܂����B
//	 */
//	pv = base;			/* �s�{�b�g     = �擪 */
//	lo = pv + width;		/* ��ʃ|�C���^ = �s�{�b�g�̎� */
//	hi = pv + width * (num - 1);	/* ���ʃ|�C���^ = ���� */
//	lo_cnt = 0;			/* ��ʗv�f��   = 0 */
//	hi_cnt = 0;			/* ���ʗv�f��   = 0 */
//
//	/* ��ʃ|�C���^�ƍ��ʃ|�C���^����������܂�...(�����A�s�{�b�g�ȊO�̑S�v�f�̒��or���ʂ��m�肷��܂�)
//	 * - (num-1)��̃J�E���g�_�E���Ń��[�v������@�ł��������ʂ𓾂��܂����A(lo<=hi)�Ń��[�v��������A
//	 *   num�ϐ��̐������Ԃ�Z�����A�����ϐ��̃��W�X�^���蓖�čœK�������L���ɓ������Ƃ����҂ł��܂��B
//	 */
//	while(lo <= hi) { /* ����!!(lo<hi)�ł͂���܂���!! */
//		/* ��ʃ|�C���^�̈ʒu�ɗL��v�f���A�s�{�b�g�Ɣ�r���܂��B */
//		if((*compare)(lo, pv) <= 0) {
//			/* ��ʗv�f�Ȃ�΁A��ʃ|�C���^�̈ʒu�Ɋi�[���܂��B
//			 * ��ʃ|�C���^�̎��̈ʒu�ɗL��v�f���A����̔�r�ΏۂƂȂ�܂��B
//			 */
//			lo += width;
//			lo_cnt++;
//		} else {
//			/* ���ʗv�f�Ȃ�΁A���ʃ|�C���^�̈ʒu�Ɋi�[���܂��B
//			 * ���ʃ|�C���^�̈ʒu�ɗL�����v�f���A����̔�r�ʒu�Ɉړ����܂��B
//			 */
//			xchg(lo, hi, width);
//			hi -= width;
//			hi_cnt++;
//		}
//	}
//
//	/* �ȏ�̏������I��������_��[�s�{�b�g,[��ʗv�f�̕���],[���ʗv�f�̕���]]�ƂȂ��Ă��܂��B
//	 * [[��ʗv�f�̕���],�s�{�b�g,[���ʗv�f�̕���]]�Ɠ���ւ��āA�s�{�b�g�̈ʒu���m�肵�܂��B
//	 * - ����ւ����ɒ�ʗv�f�̕��я����ω����܂����A�܂����я��ɈӖ��͂Ȃ��̂ŁA���v�ł��B
//	 * - ������ʗv�f����������ꍇ�Apv==hi�ƂȂ��Ă��āA����xchg()�����̓_�~�[�ƂȂ�܂��B
//	 */
//	xchg(pv, hi, width);
//
//	/* [��ʗv�f�̕���]���\�[�g���܂��B */
//	qsort(pv, lo_cnt, width, compare); /* ����!![��ʗv�f�̕���]�̐擪���w���Ă���̂́Alo�łȂ�pv�ł�!! */
//
//	/* [���ʗv�f�̕���]���\�[�g���܂��B */
//	qsort(lo, hi_cnt, width, compare); /* ����!![���ʗv�f�̕���]�̐擪���w���Ă���̂́Ahi�łȂ�lo�ł�!! */
//}
//-------------------------------------------------------------------------------------------------------------
//��* Thu Apr 19 11:44:36 JST 2007 Naoyuki Sawa
//��- ���ɐ[���ċA�Ăяo������������ꍇ���������̂�h�����߂̕ύX���s���܂����B
//��- ��̃R�����g�A�E�g���������́A�N�C�b�N�\�[�g�̂����΂�f���Ȏ����ł��B
//��  �A���S���Y���I�ɂ͂��̂܂܂ł��ԈႢ�ł͂���܂��񂪁A���ɐ[���ċA�Ăяo������������ꍇ������Ƃ�����肪����܂��B
//��- �ǂ̂悤�ȃP�[�X�Ŕ��ɐ[���ċA�Ăяo�����������邩�Ƃ����ƁA���炩���߃\�[�g���ꂽ������\�[�g���悤�Ƃ����ꍇ�ł��B
//��
//��	<��>	���̂悤�ȌĂяo�����s�����ꍇ���l���Ă݂܂��B
//��
//��			int array[10000] = { 1,2,3,...,10000 };
//��			qsort(array, 10000, sizeof(int), compare);
//��
//��		�ŏ��̌Ăяo�����x���ŁA[[��ʗv�f�̕���],�s�{�b�g,[���ʗv�f�̕���]]�́A�ȉ��̂悤�ɂȂ�܂��B
//��
//��			[[(��)],1,[2,3,4,...,10000]]	(�Ăяo���̐[��=1)
//��
//��		[���ʗv�f�̕���]�ɑ΂���ċA�Ăяo���ɒ��ڂ���ƁA���̃��x���ŁA[[��ʗv�f�̕���],�s�{�b�g,[���ʗv�f�̕���]]�́A�ȉ��̂悤�ɂȂ�܂��B
//��
//��			[[(��)],2,[3,4,5,...,10000]]	(�Ăяo���̐[��=2)
//��
//��		����Ɏ��̃��x���ł́A[[��ʗv�f�̕���],�s�{�b�g,[���ʗv�f�̕���]]�́A�ȉ��̂悤�ɂȂ�܂��B
//��
//��			[[(��)],3,[4,5,6,...,10000]]	(�Ăяo���̐[��=3)
//��
//��		�Ō�ɂ́A�����Ȃ�܂��B
//��
//��			[[(��)],10000,[(��)]]	(�Ăяo���̐[��=10000)
//��
//��		�ȏ�̗�̂悤�ɁA���炩���߃\�[�g���ꂽ������\�[�g���悤�Ƃ���ƁA���ɐ[���ċA�Ăяo������������ꍇ������܂��B
//��		����̒�����N�Ƃ���ƁA�ő�N���x���̍ċA�Ăяo�����������܂��B(���炩���ߊ��S�Ƀ\�[�g����Ă��鐔����\�[�g���悤�Ƃ����ꍇ)
//��		���炩���ߊ��S�Ƀ\�[�g����Ă��鐔��Ɍ��炸�Ƃ��A������x�\�[�g����Ă���΂���قǁA�ċA�Ăяo�����[���Ȃ�\��������܂��B
//��
//��		���ۂɂǂ̂��炢�[���Ȃ邩�͐���̏�����ԂɈˑ����Aqsort()�����s���Ă݂�܂ł킩��Ȃ��̂ŁA�댯�ł��B
//��		���ɁAP/ECE�̂悤�ȃX�^�b�N�̈�̏����Ȏ��s���ŗ��p����ꍇ�Ɋ댯�ł��B
//��
//��- ��q�̖������P������@�́A�ȉ��̂Ƃ���ł��B
//��	�E[��ʗv�f�̕���]��[���ʗv�f�̕���]�̂����A�v�f�����Ȃ������ċA�Ăяo���Ń\�[�g���܂��B
//��	�E�v�f���������́A���݂̌Ăяo�����x���̂܂܁A�ŏ����珈�����J��Ԃ����ƂŃ\�[�g���܂��B
//��  ���̂悤�ɕύX���邱�ƂŁA�ċA�Ăяo���̍ő�[�����A�ň��ł� log2(N) �ɗ}�����܂��B
//��- ���Ƃ��΁A10000�v�f�̐�����\�[�g����ꍇ�A���P�O�͍ň�10000���x���̍ċA�Ăяo�����������Ă��܂������A���P��͍ň��ł�14���x���ɗ}�����܂��B
//��  �\�[�g�Ώۂ̌����I�Ȓ������l������ƁA��20���x���ȏ�̍ċA�Ăяo������������󋵂͂قڗL�蓾�Ȃ��ƍl������̂ŁAP/ECE�ł��[���Ɉ��S�ł��B
//��- �������A��q�̉��P�͂����܂ł�����������(=�ċA�Ăяo�����x��)�Ɋւ�����̂ł���A���Ԍ����͉��P����Ȃ����Ƃɒ��ӂ��Ă��������B
//��  10000�v�f��int�z����\�[�g���鎞�Ԃ��v���Ă݂�ƁA�����_������Ȃ�Ζ�1600�~���b�ł����A���炩���߃\�[�g����Ă���Ɩ�15300�~���b������܂����B
//��
//��	[�Q�l�����FP.J.�v���E�K�[���u�W���b���C�u���� ANSI/ISO/JIS C�K�i�v�i�g�b�p���j���� 411�`412�y�[�W �{��]
void qsort(void* base, size_t num, size_t width, int (*compare)(const void*, const void*)) {
//{{2014/11/19�ǉ�:qsort_r()��ǉ����Aqsort()��qsort_r()�ւ̌Ăяo���ɕύX���܂����Bqsort_r()�̐錾�́Ainclude/stdlib.h�ɗL��܂��B
	qsort_r(base, num, width, (int (*)(const void*, const void*, void*))compare, NULL);	//qsort()��qsort_r()�́Acompare�֐��̌`�����قȂ�܂����A'cdecl�ďo�K��'�Ȃ�΁Aqsort()�`����compare�֐��ɂ����āA�]���Ȉ��������������̂Ŗ�肠��܂���B
}
void qsort_r(void* base, size_t num, size_t width, int (*compare)(const void*, const void*, void*), void* arg) {
//}}2014/11/19�ǉ�:qsort_r()��ǉ����Aqsort()��qsort_r()�ւ̌Ăяo���ɕύX���܂����Bqsort_r()�̐錾�́Ainclude/stdlib.h�ɗL��܂��B
	unsigned char* pv;	/* �s�{�b�g�ւ̃|�C���^ */
	unsigned char* lo;	/* ��ʃ|�C���^ (���ɒ�ʗv�f(�s�{�b�g�ȉ��̗v�f)���i�[����ʒu�ւ̃|�C���^) */
	unsigned char* hi;	/* ���ʃ|�C���^ (���ɍ��ʗv�f(�s�{�b�g���߂̗v�f)���i�[����ʒu�ւ̃|�C���^) */
	unsigned lo_cnt;	/* ��ʗv�f�� */
	unsigned hi_cnt;	/* ���ʗv�f�� */

	/* �v�f����0�܂���1�Ȃ�΁A���������ɋA��܂��B
	 * - EPSON�ł́A2�v�f�̏ꍇ�����ʈ������āA�P���Ȕ�r�Ɠ���ւ����s���Ă��܂����A���̕K�v�͂���܂���B
	 *   2�v�f�̏ꍇ���A����ȏ�̗v�f���̏ꍇ�Ɠ����A�ʏ�̏����őΉ��ł��܂��B
	 */
	while(num >= 2) {
		/* �|�C���^�Ɨv�f�������������܂��B
		 * - EPSON�ł́A�z��̒����t�߂Ɛ擪�̗v�f�����ւ��A���Ƃ��ƒ����t�߂ɂ������l���s�{�b�g�Ƃ��Ă��܂��B
		 *   �����炠����x�����ɕ���ł���Ɖ��肵�āA�����t�߂ɒ����l�����邱�Ƃ����҂��Ă���̂��Ǝv���܂��B
		 *   ���������ۂɂ͂��܂���ʂ������A�ނ���]�v�ȓ���ւ��̉񐔂������镪�A���\�𗎂Ƃ��Ă���悤�ł��B
		 *   ������CLiP�łł́A�P���ɐ擪�̗v�f���s�{�b�g�Ƃ��č̗p���邱�Ƃɂ��܂����B
		 */
		pv = base;			/* �s�{�b�g     = �擪 */
		lo = pv + width;		/* ��ʃ|�C���^ = �s�{�b�g�̎� */
		hi = pv + width * (num - 1);	/* ���ʃ|�C���^ = ���� */
		lo_cnt = 0;			/* ��ʗv�f��   = 0 */
		hi_cnt = 0;			/* ���ʗv�f��   = 0 */

		/* ��ʃ|�C���^�ƍ��ʃ|�C���^����������܂�...(�����A�s�{�b�g�ȊO�̑S�v�f�̒��or���ʂ��m�肷��܂�)
		 * - (num-1)��̃J�E���g�_�E���Ń��[�v������@�ł��������ʂ𓾂��܂����A(lo<=hi)�Ń��[�v��������A
		 *   num�ϐ��̐������Ԃ�Z�����A�����ϐ��̃��W�X�^���蓖�čœK�������L���ɓ������Ƃ����҂ł��܂��B
		 */
		//while(lo <= hi) { /* ����!!(lo<hi)�ł͂���܂���!! */
		//��* Thu Apr 19 14:58:48 JST 2007 Naoyuki Sawa
		//��- (num>=2)�̏�����菉��͕K��(lo<=hi)�ƂȂ�̂ŁAwhile(){...}���������I��do{...}while()�ɑւ����܂��B
		do {
			/* ��ʃ|�C���^�̈ʒu�ɗL��v�f���A�s�{�b�g�Ɣ�r���܂��B */
//{{2014/11/19�ύX:qsort_r()��ǉ����Aqsort()��qsort_r()�ւ̌Ăяo���ɕύX���܂����Bqsort_r()�̐錾�́Ainclude/stdlib.h�ɗL��܂��B
//			if((*compare)(lo, pv) <= 0) {
//��2014/11/19�ύX:qsort_r()��ǉ����Aqsort()��qsort_r()�ւ̌Ăяo���ɕύX���܂����Bqsort_r()�̐錾�́Ainclude/stdlib.h�ɗL��܂��B
			if((*compare)(lo, pv, arg) <= 0) {
//}}2014/11/19�ύX:qsort_r()��ǉ����Aqsort()��qsort_r()�ւ̌Ăяo���ɕύX���܂����Bqsort_r()�̐錾�́Ainclude/stdlib.h�ɗL��܂��B
				/* ��ʗv�f�Ȃ�΁A��ʃ|�C���^�̈ʒu�Ɋi�[���܂��B
				 * ��ʃ|�C���^�̎��̈ʒu�ɗL��v�f���A����̔�r�ΏۂƂȂ�܂��B
				 */
				lo += width;
				lo_cnt++;
			} else {
				/* ���ʗv�f�Ȃ�΁A���ʃ|�C���^�̈ʒu�Ɋi�[���܂��B
				 * ���ʃ|�C���^�̈ʒu�ɗL�����v�f���A����̔�r�ʒu�Ɉړ����܂��B
				 */
				xchg(lo, hi, width);
				hi -= width;
				hi_cnt++;
			}
		//}
		//��* Thu Apr 19 14:58:48 JST 2007 Naoyuki Sawa
		//��- (num>=2)�̏�����菉��͕K��(lo<=hi)�ƂȂ�̂ŁAwhile(){...}���������I��do{...}while()�ɑւ����܂��B
		} while(lo <= hi); /* ����!!(lo<hi)�ł͂���܂���!! */

		/* �ȏ�̏������I��������_��[�s�{�b�g,[��ʗv�f�̕���],[���ʗv�f�̕���]]�ƂȂ��Ă��܂��B
		 * [[��ʗv�f�̕���],�s�{�b�g,[���ʗv�f�̕���]]�Ɠ���ւ��āA�s�{�b�g�̈ʒu���m�肵�܂��B
		 * - ����ւ����ɒ�ʗv�f�̕��я����ω����܂����A�܂����я��ɈӖ��͂Ȃ��̂ŁA���v�ł��B
		 * - ������ʗv�f����������ꍇ�Apv==hi�ƂȂ��Ă��āA����xchg()�����̓_�~�[�ƂȂ�܂��B
		 */
		xchg(pv, hi, width);

		/* [��ʗv�f�̕���]��[���ʗv�f�̕���]�̂����A�v�f�����Ȃ������ċA�Ăяo���Ń\�[�g���܂��B
		 * �v�f���������́A���݂̌Ăяo�����x���̂܂܁A�ŏ����珈�����J��Ԃ����ƂŃ\�[�g���܂��B
		 * - �������ċA�Ăяo���Ń\�[�g����ƁA���ɐ[���ċA���������Ă��܂��ꍇ��h�����߂ł��B
		 *   ��̓I�ɂ́A���̊֐��̏�ɏ������R�����g���Q�Ƃ��Ă��������B
		 */
		if(lo_cnt <= hi_cnt) {
			/* [��ʗv�f�̕���]���\�[�g���܂��B */
//{{2014/11/19�ύX:qsort_r()��ǉ����Aqsort()��qsort_r()�ւ̌Ăяo���ɕύX���܂����Bqsort_r()�̐錾�́Ainclude/stdlib.h�ɗL��܂��B
//			qsort(pv, lo_cnt, width, compare); /* ����!![��ʗv�f�̕���]�̐擪���w���Ă���̂́Alo�łȂ�pv�ł�!! */
//��2014/11/19�ύX:qsort_r()��ǉ����Aqsort()��qsort_r()�ւ̌Ăяo���ɕύX���܂����Bqsort_r()�̐錾�́Ainclude/stdlib.h�ɗL��܂��B
			qsort_r(pv, lo_cnt, width, compare, arg); /* ����!![��ʗv�f�̕���]�̐擪���w���Ă���̂́Alo�łȂ�pv�ł�!! */
//}}2014/11/19�ύX:qsort_r()��ǉ����Aqsort()��qsort_r()�ւ̌Ăяo���ɕύX���܂����Bqsort_r()�̐錾�́Ainclude/stdlib.h�ɗL��܂��B
			/* [���ʗv�f�̕���]���\�[�g���܂��B */
			base = lo, num = hi_cnt;           /* ����!![���ʗv�f�̕���]�̐擪���w���Ă���̂́Ahi�łȂ�lo�ł�!! */
		} else {
			/* [���ʗv�f�̕���]���\�[�g���܂��B */
//{{2014/11/19�ύX:qsort_r()��ǉ����Aqsort()��qsort_r()�ւ̌Ăяo���ɕύX���܂����Bqsort_r()�̐錾�́Ainclude/stdlib.h�ɗL��܂��B
//			qsort(lo, hi_cnt, width, compare); /* ����!![���ʗv�f�̕���]�̐擪���w���Ă���̂́Ahi�łȂ�lo�ł�!! */
//��2014/11/19�ύX:qsort_r()��ǉ����Aqsort()��qsort_r()�ւ̌Ăяo���ɕύX���܂����Bqsort_r()�̐錾�́Ainclude/stdlib.h�ɗL��܂��B
			qsort_r(lo, hi_cnt, width, compare, arg); /* ����!![���ʗv�f�̕���]�̐擪���w���Ă���̂́Ahi�łȂ�lo�ł�!! */
//}}2014/11/19�ύX:qsort_r()��ǉ����Aqsort()��qsort_r()�ւ̌Ăяo���ɕύX���܂����Bqsort_r()�̐錾�́Ainclude/stdlib.h�ɗL��܂��B
			/* [��ʗv�f�̕���]���\�[�g���܂��B */
			base = pv, num = lo_cnt;           /* ����!![��ʗv�f�̕���]�̐擪���w���Ă���̂́Alo�łȂ�pv�ł�!! */
		}
	}
}
#else //PIECE
// * Thu Jan 08 21:00:51 JST 2015 Naoyuki Sawa
// - qsort()��qsort_r()���A�Z���u�������܂����B�R�[�h�T�C�Y��24�o�C�g�������Ȃ�A�������x����4.5%�����Ȃ�܂����B
void qsort(void* base, size_t num, size_t width, int (*compare)(const void*, const void*));
void qsort_r(void* base, size_t num, size_t width, int (*compare)(const void*, const void*, void*), void* arg);
asm("
		.code
		.align		1
		.global		qsort
		.global		qsort_r
qsort:
	;//{{2015/01/09�ύX:Asm��qsort()���œK�����܂����B�R�[�h�T�C�Y�������8�o�C�g�������Ȃ�܂����B
	;//	xsub		%sp, %sp, 4		;//
	;//	xld.w		[%sp+ 0], %r8		;//[%sp+ 0] :=                        NULL
	;//	call		qsort_r			;//qsort_r(base, num, width, compare, NULL)
	;//	xadd		%sp, %sp, 4		;//
	;//	ret
	;//��2015/01/09�ύX:Asm��qsort()���œK�����܂����B�R�[�h�T�C�Y�������8�o�C�g�������Ȃ�܂����B
	;// * Fri Jan 09 00:00:00 JST 2015 Naoyuki Sawa
	;// - Asm��qsort()���œK�����܂����B�R�[�h�T�C�Y�������8�o�C�g�������Ȃ�܂����B
	;//   qsort()�̏����͕s�v�ŁA���̂܂�qsort_r()�����s�����ok�ł����B���R�͈ȉ��̒ʂ�ł��B
	;// - qsort()���Ăяo���ꂽ���̃X�^�b�N�\���́A�ȉ��̒ʂ�ł�:
	;//	[%sp + 0] := %pc
	;//	[%sp + 4] := (�s��l)
	;//   ���̂܂�qsort_r()�����s����ƁA(*compare)()��arg�Ƃ��āA(�s��l)���n����邱�ƂɂȂ�܂��B
	;//   �������Aqsort()����Ăяo�����(*compare)()��arg���󂯎��Ȃ��̂ŁA(�s��l)�ō\���܂���B
	;//   �]���āAqsort()�̏����͕s�v�ŁA���̂܂�qsort_r()�����s�����ok�ł��B
	;// - �����ɂ́A(�s��l)�̕������A�N�Z�X�\��RAM���Ƃ͕ۏ؂ł��Ȃ��̂ł����A�����I�ɂ͖�肠��܂���B
	;//   �X�^�b�N�̐[����0�̏�Ԃ�qsort()���Ăяo���悤�ȏꍇ�ł��Ȃ���΁A(�s��l)�̓X�^�b�N���RAM���w���Ă��邩��ł��B
	;//   �����L�̂悤�Ȃ��Ƃ��s�����Ƃ��Ă��AP/ECE���Ȃ�Ζ����A�h���X�ɑ΂���ǂݏo���ň�ʕی��O����������悤�Ȃ��Ƃ͖����̂ŁA���L��܂���B
		;//** no job **//
	;//}}2015/01/09�ύX:Asm��qsort()���œK�����܂����B�R�[�h�T�C�Y�������8�o�C�g�������Ȃ�܂����B
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
		;//[%sp + 0] := arg	����
		;//[%sp + 4] := pv	�@��
		;//[%sp + 8] := width	�@��
		;//[%sp +12] := compare	�@��
		;//[%sp +16] := %r0	�@��
		;//[%sp +20] := %r1	�@��
		;//[%sp +24] := %r2	�@��
		;//[%sp +28] := %r3	�@��
		;//[%sp +32] := %pc	�@��
		;//[%sp +36] := arg	����
qsort_r_L10:						;//
		sub		%r13, 2			;//%r13 := (num-2)						��
		jrult		qsort_r_L60		;//while(  (num-2) >= 0) {					��
		xld.w		%r0, [%sp+ 4]		;//  %r0  :=      pv						��
		xld.w		%r1, [%sp+ 8]		;//  %r1  :=           width					��
		add		%r0, %r1		;//  %r0  := lo = pv + width			!INTERLOCK!	��
		mltu.w		%r1, %r13		;//  %alr :=           width * (num-2)				��
		ld.w		%r1, %alr		;//  %r1  :=           width * (num-2)				��
		add		%r1, %r0		;//  %r1  := hi = pv + width * (num-1)				��hi�̌v�Z���@���H�v�����B
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
 *	�q�[�v�\�[�g�ƃ}�[�W�\�[�g�ɂ���(2016/06/10)
 *****************************************************************************/

// * Fri Jun 10 21:42:30 JST 2016 Naoyuki Sawa
// - ������hsort()��msort()���Aheapsort()��mergesort()�ɖ��O�ύX���A/clip/include/stdlib.h�Ő錾����悤�ɂ��܂����B
//   �q�[�v�\�[�g�ƃ}�[�W�\�[�g�͕W���I��C�֐��ł͂Ȃ��̂ŁA����܂ł�clipsort.h�Ő錾���Ă����̂ł����A
//   (ANSI/ISO��POSIX�ł͂Ȃ����̂�)�AFreeBSD��libc�ł́A�q�[�v�\�[�g�ƃ}�[�W�\�[�g����`����Ă��鎖������������ł��B
//   FreeBSD��libc�̊֐����ɕ���āAheapsort()��mergesort()�ɖ��O�ύX���A/clip/include/stdlib.h�Ő錾���鎖�ɂ��܂����B
//
//	���Q�Ǝ���
//	�uFreeBSD Library Functions Manual - QSORT�v(http://www.freebsd.org/cgi/man.cgi?query=qsort&sektion=3&apropos=0&manpath=FreeBSD+10.3-RELEASE+and+Ports)
//	�uFreeBSD ���C�u�����֐��}�j���A�� QSORT(3)�v(http://kaworu.jpn.org/doc/FreeBSD/jman/man3/mergesort.3.php)�����p:
//	��
//	��	����
//	��			qsort,heapsort,mergesort - �\�[�g�֐�
//	��	���C�u����
//	��			�W��C���C�u����(libc,lc)
//	��	����
//	��			#include <stdlib.h>
//	��			void qsort(void* base, size_t nmemb, size_t size, int (*compar)(const void*, const void*));
//	��			int heapsort(void* base, size_t nmemb, size_t size, int (*compar)(const void*, const void*));
//	��			int mergesort(void* base, size_t nmemb, size_t size, int (*compar)(const void*, const void*));
//	��	���
//	��			qsort()�֐��́A�p�[�e�B�V���������\�[�g�̏C���ŁA�悤����ɃN�C�b�N�\�[�g�ł��B
//	��			heapsort()�֐��́A�I���\�[�g�̏C���łł��B
//	��			mergesort()�֐��́A���ɕ���ł���ӏ��̂���f�[�^���\�[�g����A�w�������ɂ��}�[�W�\�[�g�̏C���łł��B
//	��
//	��			qsort()�֐���heapsort()�֐��́Abase�ɂ���ď��������o���w����Ă���nmemb�I�u�W�F�N�g�̔z����\�[�g���܂��B
//	��			�e�I�u�W�F�N�g�̃T�C�Y�́Asize�Ŏw�肵�܂��B
//	��			mergesort()�������悤�ɓ��삵�܂����Asize��``sizeof(void*)/2''���傫���Ȃ���΂Ȃ�܂���B	���������������C�u�����̎����ł�size�����̐���͗L��܂���B
//	��
//	��			�z��base�̓��e�́Acompar���w����r�֐��ɏ]���ď����Ń\�[�g����܂��B
//	��			���̊֐��ł́A��r����I�u�W�F�N�g���w���A2�̈������K�v�ł��B
//	��
//	��			��r�֐��́A�ŏ��̈��������̈�����菬�����ꍇ��0��菬���������A�������ꍇ��0�A�傫���ꍇ��0���傫��������߂��K�v������܂��B
//	��
//	��			qsort()�֐���heapsort()�֐��͕s����ł��B
//	��			�܂�2�̃����o���������ꍇ�A�\�[�g�ςݔz����ł̏����͕s��ɂȂ�܂��B
//	��			mergesort()�֐��͈���ł��B
//	��
//	��			qsort()�֐��́A�p�[�e�B�V���������\�[�g�̈��ł���AC.A.R. Hoare��``�N�C�b�N�\�[�g''�A���S���Y�����������Ă��܂��B
//	��			�Ƃ���D.E. Knuth�̃A���S���Y��Q���Q�Ƃ��Ă��������B
//	��			qsort()�ɂ́A���ς�O N lg N�̎��Ԃ�������܂��B
//	��			���̎����ł́A���W�A���I�����g�p���āAON**2�Ƃ����ň��ȃP�[�X�̓����������܂��B
//	��
//	��			heapsort()�֐��́A�I���\�[�g�̈��ł���AJ.W.J. William��``�q�[�v�\�[�g''�A���S���Y�����������Ă��܂��B
//	��			�Ƃ���D.E. Knuth�̃A���S���Y��H���Q�Ƃ��Ă��������B
//	��			heapsort()�ɂ́A�ň��̃P�[�X��O N lg N�̎��Ԃ�������܂��B
//	��			���������قƂ�Ǘ]���Ɏg�p���Ȃ��Ƃ����_�݂̂�qsort()���D��Ă��܂��B
//	��			����Aqsort()�̓����������蓖�Ă܂��񂪁A�ċA���g�p���Ď�������Ă��܂��B
//	��
//	��			mergesort()�֐��ł́Anmemb*size�o�C�g�̃��������]���ɕK�v�ƂȂ�܂��B
//	��			�X�y�[�X�ɗ]�T������ꍇ�݂̂Ɏg�p���Ă��������B
//	��			mergesort()�́A���ɕ���ł���ӏ��̂���f�[�^�������悤�œK������Ă��܂��B
//	��			�ň��̃P�[�X�̎��Ԃ�O N lg N�ŁA�őP�̃P�[�X��O N�ł��B
//	��
//	��			�ʏ�́Aheapsort()���mergesort()�̕��������Amergesort()���qsort()�̕��������ł��B
//	��			�g�p�ł��郁�����ʂ���ɕ���ł���f�[�^�ʂɂ��A���̏󋵂͕ω����܂��B
//	��	�߂�l
//	��			qsort()�֐��͒l��߂��܂���B
//	��
//	��			�֐�heapsort()�����mergesort()�́A��������������ƒl0��Ԃ��܂��B
//	��			�����łȂ��ꍇ�A�l-1���Ԃ���A�O���[�o���ϐ�errno���ݒ肳��ăG���[�������܂��B
//	��	�G���[
//	��			heapsort()�֐���mergesort()�֐��́A�ȉ��̂悤�ȏꍇ�ɃG���[�ƂȂ�܂��B													
//	��			[EINVAL]
//	��				size������0�ł��邩�Amergesort()��size������``sizeof(void*)/2''��菬�����ꍇ�B		���������������C�u�����̎����ł�size�����̐���͗L��܂���B		��
//	��			[ENOMEM]																				���v����ɓ������͏��0(����)��Ԃ��܂��B
//	��				heapsort()��mergesort()�������������蓖�Ă��Ȃ������ꍇ�B				���������������C�u�����̎����ł͏�����Ԃ����ɃG���[�I�����܂��B	��
//	��	�݊���
//	��			���o�[�W������qsort()�ł́A��r���[�`����qsort(3)���Ăяo�����Ƃ͂ł��܂���ł����B
//	��			���݂͌Ăяo���܂��B
//	��	�֘A����
//	��			sort(1),radixsort(3)
//	��	�K�i
//	��			qsort()�֐��́AISO/IEC 9899:1990(``ISO C89'')�ɓK�����Ă��܂��B
//	��			FreeBSD 10.0 June 4, 1993 FreeBSD 10.0
//	��
//
// - �Ǝ��g���Ƃ��āAheapsort_r()��mergesort_r()���ǉ����܂����B
//   ������FreeBSD��libc�Œ�`����Ă��܂��񂪁A�K�v���Ǝv������ł��B
//   �Ǝ��g����bsearch_r()��ǉ��������Ɠ��l�ł��B
//   �ڍׂ́Acliplibc.c��'Wed May 04 21:28:21 JST 2016'�̃R�����g���Q�Ƃ��ĉ������B

/*****************************************************************************
 *	�q�[�v�\�[�g
 *****************************************************************************/

#ifndef PIECE	//{{2016/06/10:P/ECE���ł̓R�[�h�T�C�Y�ߖ�̂��߂Ƀq�[�v�\�[�g�𖳌������Ă��܂��B�L�������鎖���\�ł��B}}

// * Sun May 13 14:21:21 JST 2007 Naoyuki Sawa
// - �u�q�[�v�\�[�g - Wikipedia�v�̋[���R�[�h���Q�l�ɁA�œK�����č쐬���܂����B
//   (URL: http://ja.wikipedia.org/w/index.php?title=%E3%83%92%E3%83%BC%E3%83%97%E3%82%BD%E3%83%BC%E3%83%88&oldid=12248591)
//   �œK��������ȓ_�́A�e��q�̗v�f���w���������߂̃C���f�N�X���g�킸�ɁA���ׂă|�C���^�ŏ������č�������}�������Ƃł��B
// - �q�[�v�\�[�g���N�B�b�N�\�[�g�����D��Ă���_�́A�f�[�^�̂�����肪���x�ɂ��܂�e�����Ȃ����Ƃł��B
//   �N�B�b�N�\�[�g�́A��l�Ƀ����_���Ȑ���̃\�[�g�������΂񑁂��A���X����̃\�[�g�͒x���A�t���̃\�[�g�������Ƃ��x���ł��B
//   �q�[�v�\�[�g�́A�ǂ̃P�[�X�ɑ΂��Ă������悻�������炢�̑��x�Ŋ������܂��B
// - ���������ۂɂ́A�N�B�b�N�\�[�g�����q�[�v�\�[�g��I�������ʂ͖����Ǝv���܂��B
//   �N�B�b�N�\�[�g�ɕs���ȋt���̃\�[�g�ł����Ă��A�N�B�b�N�\�[�g�̕����q�[�v�\�[�g������{���炢���������ł���悤�ł��B
//   ��l�Ƀ����_���Ȑ���̃\�[�g�̏ꍇ�́A50�{�ȏ�̑��x�����t���Ă��܂��܂��B
//   (Windows�Ŏ��������ꍇ�͂����܂ł̑��x���͕t���Ȃ������̂ł����AP/ECE�ł͂Ȃ�����q�̂悤�ȑ卷�ɂȂ��Ă��܂��܂����B)
// - �ȉ��̃R�[�h�́A�������������Ƃ��m�F�ς݂ł����A���ۂɎg�����Ƃ͖����Ǝv���܂��B
//   �����N�T�C�Y�������Ȃ��悤�ɃR�����g�A�E�g���Ă���܂��̂ŁA�����g���ꍇ�̓A�v���P�[�V�����ɃR�s�[���Ďg���Ă��������B
// * Fri Jun 10 21:42:30 JST 2016 Naoyuki Sawa
// - ������hsort()��msort()���Aheapsort()��mergesort()�ɖ��O�ύX���A/clip/include/stdlib.h�Ő錾����悤�ɂ��܂����B
//   ����܂ł́A�q�[�v�\�[�g�̎�����̓R�����g�A�E�g���Ă��܂������AWin32���ł͗L�������܂����B
//   �������A���ۂɕK�v�ɂȂ鎖�͖����Ǝv���̂ŁAP/ECE���ł̓R�[�h�T�C�Y�ߖ�̂��߂ɖ����������܂܂Ƃ��܂����B
//   ����A�q�[�v�\�[�g��L�����������R�́A�����܂ł�Win32���ł̃e�X�g�̂��߂����ł��B

//{{2016/06/10�ύX:������hsort()��msort()���Aheapsort()��mergesort()�ɖ��O�ύX���A/clip/include/stdlib.h�Ő錾����悤�ɂ��܂����B
//static void make_heap(unsigned char* base, unsigned char* last, size_t width, int (*compare)(const void*, const void*), unsigned char* parent) {
//��2016/06/10�ύX:������hsort()��msort()���Aheapsort()��mergesort()�ɖ��O�ύX���A/clip/include/stdlib.h�Ő錾����悤�ɂ��܂����B
static void make_heap(unsigned char* base, unsigned char* last, size_t width, int (*compare)(const void*, const void*, void*), unsigned char* parent, void* arg) {
//}}2016/06/10�ύX:������hsort()��msort()���Aheapsort()��mergesort()�ɖ��O�ύX���A/clip/include/stdlib.h�Ő錾����悤�ɂ��܂����B
	unsigned char *child1, *child2;
	/* ���̎q���L���... */
	child1 = (base + (parent - base) * 2) + width;
	if(child1 <= last) {
		/* ���̎q�̃c���[���\�[�g���܂��B */
//{{2016/06/10�ύX:������hsort()��msort()���Aheapsort()��mergesort()�ɖ��O�ύX���A/clip/include/stdlib.h�Ő錾����悤�ɂ��܂����B
//		make_heap(base, last, width, compare, child1);
//��2016/06/10�ύX:������hsort()��msort()���Aheapsort()��mergesort()�ɖ��O�ύX���A/clip/include/stdlib.h�Ő錾����悤�ɂ��܂����B
		make_heap(base, last, width, compare, child1, arg);
//}}2016/06/10�ύX:������hsort()��msort()���Aheapsort()��mergesort()�ɖ��O�ύX���A/clip/include/stdlib.h�Ő錾����悤�ɂ��܂����B
		/* �E�̎q���L���... */
		child2 = child1 + width;
		if(child2 <= last) {
			/* �E�̎q�̃c���[���\�[�g���܂��B */
//{{2016/06/10�ύX:������hsort()��msort()���Aheapsort()��mergesort()�ɖ��O�ύX���A/clip/include/stdlib.h�Ő錾����悤�ɂ��܂����B
//			make_heap(base, last, width, compare, child2);
//��2016/06/10�ύX:������hsort()��msort()���Aheapsort()��mergesort()�ɖ��O�ύX���A/clip/include/stdlib.h�Ő錾����悤�ɂ��܂����B
			make_heap(base, last, width, compare, child2, arg);
//}}2016/06/10�ύX:������hsort()��msort()���Aheapsort()��mergesort()�ɖ��O�ύX���A/clip/include/stdlib.h�Ő錾����悤�ɂ��܂����B
			/* child1���傫�����̎q���w���悤�ɂ��܂��B */
//{{2016/06/10�ύX:������hsort()��msort()���Aheapsort()��mergesort()�ɖ��O�ύX���A/clip/include/stdlib.h�Ő錾����悤�ɂ��܂����B
//			if((*compare)(child1, child2) < 0) {
//��2016/06/10�ύX:������hsort()��msort()���Aheapsort()��mergesort()�ɖ��O�ύX���A/clip/include/stdlib.h�Ő錾����悤�ɂ��܂����B
			if((*compare)(child1, child2, arg) < 0) {
//}}2016/06/10�ύX:������hsort()��msort()���Aheapsort()��mergesort()�ɖ��O�ύX���A/clip/include/stdlib.h�Ő錾����悤�ɂ��܂����B
				child1 = child2;
			}
		}
		/* �傫�����̎q���e�����傫�����... */
//{{2016/06/10�ύX:������hsort()��msort()���Aheapsort()��mergesort()�ɖ��O�ύX���A/clip/include/stdlib.h�Ő錾����悤�ɂ��܂����B
//		if((*compare)(parent, child1) < 0) {
//��2016/06/10�ύX:������hsort()��msort()���Aheapsort()��mergesort()�ɖ��O�ύX���A/clip/include/stdlib.h�Ő錾����悤�ɂ��܂����B
		if((*compare)(parent, child1, arg) < 0) {
//}}2016/06/10�ύX:������hsort()��msort()���Aheapsort()��mergesort()�ɖ��O�ύX���A/clip/include/stdlib.h�Ő錾����悤�ɂ��܂����B
			/* �傫�����̎q�Ɛe�����ւ��܂��B */
			xchg(parent, child1, width);
			//make_heap(base, last, width, compare, child1);
			//���q�̃c���[�̃\�[�g�͎���ɍs���̂ŕs�v�ł��B
		}
	}
}

//{{2016/06/10�ύX:������hsort()��msort()���Aheapsort()��mergesort()�ɖ��O�ύX���A/clip/include/stdlib.h�Ő錾����悤�ɂ��܂����B
//void hsort(void* _base, size_t num, size_t width, int (*compare)(const void*, const void*)) {
//��2016/06/10�ύX:������hsort()��msort()���Aheapsort()��mergesort()�ɖ��O�ύX���A/clip/include/stdlib.h�Ő錾����悤�ɂ��܂����B
int heapsort(void* _base, size_t num, size_t width, int (*compare)(const void*, const void*)) {	//FreeBSD 10.0 June 4, 1993 FreeBSD 10.0
	return heapsort_r(_base, num, width, (int (*)(const void*, const void*, void*))compare, NULL);
}
int heapsort_r(void* _base, size_t num, size_t width, int (*compare)(const void*, const void*, void*), void* arg) {	//�Ǝ��g��
//}}2016/06/10�ύX:������hsort()��msort()���Aheapsort()��mergesort()�ɖ��O�ύX���A/clip/include/stdlib.h�Ő錾����悤�ɂ��܂����B
	unsigned char* base = _base;
	unsigned char* last = base + (width * (num - 1));
	while(base < last) {
		/* �c���[�S�̂��~���Ƀ\�[�g���܂��B */
//{{2016/06/10�ύX:������hsort()��msort()���Aheapsort()��mergesort()�ɖ��O�ύX���A/clip/include/stdlib.h�Ő錾����悤�ɂ��܂����B
//		make_heap(base, last, width, compare, base);
//��2016/06/10�ύX:������hsort()��msort()���Aheapsort()��mergesort()�ɖ��O�ύX���A/clip/include/stdlib.h�Ő錾����悤�ɂ��܂����B
		make_heap(base, last, width, compare, base, arg);
//}}2016/06/10�ύX:������hsort()��msort()���Aheapsort()��mergesort()�ɖ��O�ύX���A/clip/include/stdlib.h�Ő錾����悤�ɂ��܂����B
		/* �ő�v�f���c���[�̖����ɐ؂藣���܂��B */
		xchg(base, last, width);
		last -= width;
	}
//{{2016/06/10�ǉ�:������hsort()��msort()���Aheapsort()��mergesort()�ɖ��O�ύX���A/clip/include/stdlib.h�Ő錾����悤�ɂ��܂����B
	return 0;
//}}2016/06/10�ǉ�:������hsort()��msort()���Aheapsort()��mergesort()�ɖ��O�ύX���A/clip/include/stdlib.h�Ő錾����悤�ɂ��܂����B
}

#endif//PIECE	//{{2016/06/10:P/ECE���ł̓R�[�h�T�C�Y�ߖ�̂��߂Ƀq�[�v�\�[�g�𖳌������Ă��܂��B�L�������鎖���\�ł��B}}

/*****************************************************************************
 *	�}�[�W�\�[�g
 *****************************************************************************/

/* * Mon Oct 18 15:12:52 JST 2010 Naoyuki Sawa
 * - msort()�֐�����������ۂɎQ�l�ɂ���Web�T�C�g���A�e�L�X�g�����ċL�^���Ă������Ƃɂ��܂��B
 *   �w�i�l�`�w��ɂ�����j�A���S���Y���I���ɂ����郆�[�U�[�C���^�[�t�F�[�X�̉��ǁx�̒��́A
 *   �@(http://www.th.cs.meiji.ac.jp/researches/2005/omoto/index.html)
 *   �u8. �}�[�W�\�[�g�v�̃y�[�W����A���p�����Ă��������܂����B
 *   �@(http://www.th.cs.meiji.ac.jp/researches/2005/omoto/mergesort.html)
 * - ��ʓI�Ƀ}�[�W�\�[�g�́A�ċA��p���Ď�������̂ł�����ǁA��L�T�C�g�̃T���v���\�[�X�́A�ċA��p���Ȃ��Ŏ������Ă��܂��B
 *   �܂��A�������m�ۂ̎d�g�݂�����₷���A���\�[�X�̌���ꂽ�}�C�R�������̎������Ǝv���܂��B
 * 
 * 	��������������������������������������������������������������������������������������������������������������������������
 * 	���}�[�W�\�[�g														��
 * 	��������������������������������������������������������������������������������������������������������������������������
 * 
 * 	��������������������������������������������������������������������������������������������������������������������������
 * 	���}�[�W�\�[�g�Ƃ́H													��
 * 	��������������������������������������������������������������������������������������������������������������������������
 * 
 * 	�}�[�W�\�[�g�́A�f�[�^����ɕ������A�������Ăу}�[�W����i��������j���ɑ傫���̏��Ƀf�[�^�����o���ĕ��ׂ�B
 * 	�������ꂽ���ꂼ�ꂪ�傫���̏��ɕ���ł���΃}�[�W�������̂��傫���̏��ɕ��Ԃ̂ŁA�������ꂽ���̂ɑ΂��Ă�
 * 	�}�[�W�\�[�g�i��A���̑��̃\�[�g�c�Ⴆ�Ε�����̃f�[�^�����Q�Ƃ��R�̏ꍇ�͒P���Ȕ�r�ōςށj���ċA�I�ɓK�p����Ηǂ��B
 * 	�\�[�g����r�I�����ŁA���ʂ̂��̂̏������ۑ������Ƃ�������������B
 * 
 * 	��������������������������������������������������������������������������������������������������������������������������
 * 	���l����														��
 * 	��������������������������������������������������������������������������������������������������������������������������
 * 
 * 	���o�Ƃ��ẮA�u�Q�{�̔z���p�ӂ��A�������ꂽ�m�[�h���\�[�g���s���v�Ƃ����������ł���B
 * 
 * 	���A���̂悤�Ȕz�񂪑��݂���Ƃ���B
 * 
 * 		����������������������������������
 * 		���S���P���Q���U���X���V���R���W��
 * 		����������������������������������
 * 		����������������������������������
 * 		���@���@���@���@���@���@���@���@��
 * 		����������������������������������
 * 
 * 	������ׂ����������Ă����̂����A�܂��A�Q�ŕ�������B
 * 
 * 		����������������������������������
 * 		���S���P���Q���U���X���V���R���W��
 * 		����������������������������������
 * 		����������������������������������
 * 		���@���@���@���@���@���@���@���@��
 * 		����������������������������������
 * 
 * 	�������ꂽ�͈͓��ŁA�����P�p�ӂ��ꂽ�z��Ƀ\�[�g�����l�����Ă����B
 * 
 * 		����������������������������������
 * 		���S���P���Q���U���X���V���R���W��
 * 		����������������������������������
 * 		���@�@�@���@�@�@���@�@�@���@�@�@��
 * 		����������������������������������
 * 		���P���S���Q���U���V���X���R���W��
 * 		����������������������������������
 * 
 * 	���ɐV�����l��ێ������z��𕪊�����̂����A�Q�~�Q���S�ŕ�������B
 * 
 * 		����������������������������������
 * 		���S���P���Q���U���X���V���R���W��
 * 		����������������������������������
 * 		����������������������������������
 * 		���P���S���Q���U���V���X���R���W��
 * 		����������������������������������
 * 
 * 	�������ꂽ�͈͓��ŁA���̔z��Ƀ\�[�g�����l�����Ă����B
 * 
 * 		����������������������������������
 * 		���P���Q���S���U���R���V���W���X��
 * 		����������������������������������
 * 		���@�@�@�@�@�@�@���@�@�@�@�@�@�@��
 * 		����������������������������������
 * 		���P���S���Q���U���V���X���R���W��
 * 		����������������������������������
 * 
 * 	���ɐV�����l��ێ��������̔z��𕪊�����B�S�~�Q���W�ŕ�������B
 * 
 * 		����������������������������������
 * 		���P���Q���S���U���R���V���W���X��
 * 		����������������������������������
 * 		����������������������������������
 * 		���P���S���Q���U���V���X���R���W��
 * 		����������������������������������
 * 
 * 	�������ꂽ�͈͓��ŁA�����P�p�ӂ��ꂽ�z��Ƀ\�[�g�����l�����Ă����B
 * 
 * 		����������������������������������
 * 		���P���Q���S���U���R���V���W���X��
 * 		����������������������������������
 * 		���@�@�@�@�@�@�@�@�@�@�@�@�@�@�@��
 * 		����������������������������������
 * 		���P���Q���R���S���U���V���W���X��
 * 		����������������������������������
 * 
 * 	���ɐV�����l��ێ������z��𕪊�����̂����A�v�f�����W�ł���̂łW�ŕ�������B
 * 
 * 		����������������������������������
 * 		���P���Q���S���U���R���V���W���X��
 * 		����������������������������������
 * 		����������������������������������
 * 		���P���Q���R���S���U���V���W���X��
 * 		����������������������������������
 * 
 * 	�������ꂽ�͈͓��ŁA���̔z��Ƀ\�[�g�����l�����Ă����B
 * 
 * 		����������������������������������
 * 		���P���Q���R���S���U���V���W���X��
 * 		����������������������������������
 * 		���@�@�@�@�@�@�@�@�@�@�@�@�@�@�@��
 * 		����������������������������������
 * 		���P���Q���R���S���U���V���W���X��
 * 		����������������������������������
 * 
 * 	���̔z��𕪊�����ۂɁA�����͈̔͂��v�f���𒴂��Ă���ꍇ�A�\�[�g�I���ƂȂ�B
 * 
 * 	�ȏオ�}�[�W�\�[�g�̗���ł���B
 * 
 * 	�}�[�W�\�[�g�̃A���S���Y���͈ȉ��̂悤�ɋL�q�ł���B
 * 
 * 		void sort() {							// �}�[�W�\�[�g(����)
 * 			int seqsize = 1;					// �傫���̏����l��1�Ƃ���
 * 			while (seqsize < length) {				// �\�[�g����傫�����f�[�^������������΁C
 * 				mergeseqs(seqsize, a, b);			// �w�肳�ꂽ�傫���Ŕz��a����z��b�փ}�[�W
 * 				mergeseqs(2 * seqsize, b, a);			// �w�肳�ꂽ2�{�̑傫���Ŕz��b����z��a�փ}�[�W
 * 				seqsize = 4 * seqsize;				// �傫����4�{�ɂ���D
 * 			}
 * 		}
 * 
 * 		void mergeseqs(int size, int from [], int to []) {		// from�z�񂩂�}�[�W��to�z������
 * 			int i, j, k, iend, jend;
 * 			int start = 0;
 * 			while (start < length) {
 * 				i = start;					// i: �}�[�W������̎n�܂�
 * 				j = start + size;				// j: �}�[�W����������̗�̎n�܂�
 * 				k = start;					// k: �}�[�W�������ʂ̗�̎n�܂�
 * 				iend = Math.min(start + size, length);		// �ΏۂƂ���z��v�f�̍Ō���v�Z
 * 				jend = Math.min(start + 2 * size, length);	// �ΏۂƂ���z��v�f�̍Ō���v�Z
 * 				while (i < iend && j < jend) {			// �}�[�W�����2����ꍇ
 * 					if (from[i] <= from[j]) {
 * 						to[k] = from[i];		// �������l�̕����R�s�[
 * 						i++;
 * 						k++;
 * 					} else {
 * 						to[k] = from[j];		// �������l�̕����R�s�[
 * 						j++;
 * 						k++;
 * 					}
 * 				}
 * 				while (i < iend) {				// �}�[�W�����1�����̏ꍇ
 * 					to[k] = from[i];			// �l�̃R�s�[
 * 					i++;
 * 					k++;
 * 				}
 * 				while (j < jend) {				// �}�[�W�����1�����̏ꍇ
 * 					to[k] = from[j];			// �l�̃R�s�[
 * 					j++;
 * 					k++;
 * 				}
 * 				start = start + 2 * size;			// �V���ȊJ�n�_��ݒ�
 * 			}
 * 		}
 * 
 * 	���v���O�������p
 * 	Lecture of Computer Programming I by Hiroshi Ichiji 
 * 	(http://lecture.ecc.u-tokyo.ac.jp/~cichiji/cp-01/cp-01.html)
 */

#if 0
//{{2016/06/10�ύX:������hsort()��msort()���Aheapsort()��mergesort()�ɖ��O�ύX���A/clip/include/stdlib.h�Ő錾����悤�ɂ��܂����B
//static void mergeseqs(size_t size, const unsigned char* from, unsigned char* to, size_t num, size_t width, int (*compare)(const void*, const void*)) {
//��2016/06/10�ύX:������hsort()��msort()���Aheapsort()��mergesort()�ɖ��O�ύX���A/clip/include/stdlib.h�Ő錾����悤�ɂ��܂����B
//���f���Ȏ���
static void mergeseqs(size_t size, const unsigned char* from, unsigned char* to, size_t num, size_t width, int (*compare)(const void*, const void*, void*), void* arg) {
//}}2016/06/10�ύX:������hsort()��msort()���Aheapsort()��mergesort()�ɖ��O�ύX���A/clip/include/stdlib.h�Ő錾����悤�ɂ��܂����B
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
//{{2016/06/10�ύX:������hsort()��msort()���Aheapsort()��mergesort()�ɖ��O�ύX���A/clip/include/stdlib.h�Ő錾����悤�ɂ��܂����B
//			if((*compare)(&from[i * width], &from[j * width]) <= 0) {
//��2016/06/10�ύX:������hsort()��msort()���Aheapsort()��mergesort()�ɖ��O�ύX���A/clip/include/stdlib.h�Ő錾����悤�ɂ��܂����B
			if((*compare)(&from[i * width], &from[j * width], arg) <= 0) {
//}}2016/06/10�ύX:������hsort()��msort()���Aheapsort()��mergesort()�ɖ��O�ύX���A/clip/include/stdlib.h�Ő錾����悤�ɂ��܂����B
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
//{{2016/06/10�ύX:������hsort()��msort()���Aheapsort()��mergesort()�ɖ��O�ύX���A/clip/include/stdlib.h�Ő錾����悤�ɂ��܂����B
//void msort(void* base, size_t num, size_t width, int (*compare)(const void*, const void*)) {
//��2016/06/10�ύX:������hsort()��msort()���Aheapsort()��mergesort()�ɖ��O�ύX���A/clip/include/stdlib.h�Ő錾����悤�ɂ��܂����B
int mergesort(void* base, size_t num, size_t width, int (*compare)(const void*, const void*)) {	//FreeBSD 10.0 June 4, 1993 FreeBSD 10.0
	return mergesort_r(base, num, width, (int (*)(const void*, const void*, void*))compare, NULL);
}
int mergesort_r(void* base, size_t num, size_t width, int (*compare)(const void*, const void*, void*), void* arg) {	//�Ǝ��g��
//}}2016/06/10�ύX:������hsort()��msort()���Aheapsort()��mergesort()�ɖ��O�ύX���A/clip/include/stdlib.h�Ő錾����悤�ɂ��܂����B
	void *a = base, *b;
	size_t seqsize;
	b = malloc(num * width);
	if(!b) { DIE(); }
	for(seqsize = 1; seqsize < num; /** no job **/) {
//{{2016/06/10�ύX:������hsort()��msort()���Aheapsort()��mergesort()�ɖ��O�ύX���A/clip/include/stdlib.h�Ő錾����悤�ɂ��܂����B
//		mergeseqs(seqsize, a, b, num, width, compare);
//��2016/06/10�ύX:������hsort()��msort()���Aheapsort()��mergesort()�ɖ��O�ύX���A/clip/include/stdlib.h�Ő錾����悤�ɂ��܂����B
		mergeseqs(seqsize, a, b, num, width, compare, arg);
//}}2016/06/10�ύX:������hsort()��msort()���Aheapsort()��mergesort()�ɖ��O�ύX���A/clip/include/stdlib.h�Ő錾����悤�ɂ��܂����B
		seqsize *= 2;
//��2016/06/10�ύX:������hsort()��msort()���Aheapsort()��mergesort()�ɖ��O�ύX���A/clip/include/stdlib.h�Ő錾����悤�ɂ��܂����B
//		mergeseqs(seqsize, b, a, num, width, compare);
//��2016/06/10�ύX:������hsort()��msort()���Aheapsort()��mergesort()�ɖ��O�ύX���A/clip/include/stdlib.h�Ő錾����悤�ɂ��܂����B
		mergeseqs(seqsize, b, a, num, width, compare, arg);
//��2016/06/10�ύX:������hsort()��msort()���Aheapsort()��mergesort()�ɖ��O�ύX���A/clip/include/stdlib.h�Ő錾����悤�ɂ��܂����B
		seqsize *= 2;
	}
	free(b);
//{{2016/06/10�ǉ�:������hsort()��msort()���Aheapsort()��mergesort()�ɖ��O�ύX���A/clip/include/stdlib.h�Ő錾����悤�ɂ��܂����B
	return 0;
//}}2016/06/10�ǉ�:������hsort()��msort()���Aheapsort()��mergesort()�ɖ��O�ύX���A/clip/include/stdlib.h�Ő錾����悤�ɂ��܂����B
}
#else	//��{{2016/06/10�ύX:�A�Z���u�������܂����B�R�[�h�T�C�Y��72�o�C�g�������Ȃ�܂����B���x�����������Ȃ����Ǝv���܂����A���Xmalloc/free���g���Ă���̂őS�̂Ƃ��Ă̑��x�͂��܂�ς��Ȃ��Ǝv���܂��B}}
//���A�Z���u�������邽�߂̏����Ƃ��ĕύX���������B
// - '�f���Ȏ���'�ł̓C���f�N�X�x�[�X�ł������������A'�A�Z���u�������邽�߂̏����Ƃ��ĕύX��������'�ł̓|�C���^�x�[�X�̏����ɕύX���܂����B
//   �ύX�������R�́A�A�Z���u�������鎞�ɁA�K�v�ȃ��W�X�^�������炷���߂̏����ł��B
// - '�A�Z���u�������邽�߂̏����Ƃ��ĕύX��������'���A�A���S���Y�����̂�'�f���Ȏ���'�Ɠ����ł��B
//   '�A�Z���u�������邽�߂̏����Ƃ��ĕύX��������'�̃R�[�h�́A�ǂ�Ŕ���Â炢�Ǝv���̂ŁA�A���S���Y�����̂�'�f���Ȏ���'���Q�Ƃ��ĉ������B
#ifndef PIECE
static void mergeseqs(size_t size, const unsigned char* from, unsigned char* to, size_t num, size_t width, int (*compare)(const void*, const void*, void*), void* arg) {
	const int from_to = to - from;							//����ȍ~�Ato�͕s�v�B		����ȍ~�Afrom_to�͕ω����Ȃ��B		from_to�̓������ɒu���B
	const unsigned char *from_num, *from_i, *from_iend, *from_j, *from_jend;
	unsigned char* to_k;
	size *= width;									//				����ȍ~�Asize�͕ω����Ȃ��B		size�̓������ɒu���B
	num  *= width;									//				����ȍ~�Anum�͕ω����Ȃ��B
	from_num = &from[num];								//����ȍ~�Anum�͕s�v�B		����ȍ~�Afrom_num�͕ω����Ȃ��B	from_num�̓������ɒu���Ă��ǂ����A���W�X�^����]�����̂ŁAfrom_num�����W�X�^�ɒu�����ɂ����B�������ɒu���Ă��ǂ��ϐ��̂����Afrom_num�����W�X�^�ɒu���̂���Ԍ��ʂ������Ǝv������ł��B
	for(from_jend = from; from_jend < from_num; /** no job **/) {			//����ȍ~�Afrom�͕s�v�B
		from_i    = from_jend;							//													from_i�̓��W�X�^�ɒu���B
		from_iend = from_i + size;
		from_j    = from_iend;							//													from_j�̓��W�X�^�ɒu���B
		from_jend = from_j + size;						//����ȍ~�Asize�͕s�v�B
		if(from_iend > from_num) {
			from_iend = from_num;						//				����ȍ~�Afrom_iend�͕ω����Ȃ��B	from_iend�̓������ɒu���B
		}
		if(from_jend > from_num) {
			from_jend = from_num;						//				����ȍ~�Afrom_jend�͕ω����Ȃ��B	from_jend�̓������ɒu���B
		}
		to_k = (unsigned char*)(from_i + from_to);				//													to_k�̓��W�X�^�ɒu���B
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
int mergesort_r(void* base, size_t num, size_t width, int (*compare)(const void*, const void*, void*), void* arg) {	//�Ǝ��g��
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
int mergesort_r(void* base, size_t num, size_t width, int (*compare)(const void*, const void*, void*), void* arg);	//�Ǝ��g��
asm("
		.global		mergesort
		.global		mergesort_r
mergesort:
		;//[%sp+0] := retp
		;//[%sp+4] := (arg = �s��l)
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
		;//[%sp+0]  := (width)		��
		;//[%sp+4]  := (compare)	��mergeseqs()�ւ̑�5,��6,��7�����ɂȂ�܂��B
		;//[%sp+8]  := (arg)		��
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
		;//%r13     := from	��������������������������������������������������������������������������������������������������
		;//%r14     := to													��
		;//%r15     := num													��
		;//[%sp+0]  := (from_to)												��
		;//[%sp+4]  := (size)													��
		;//[%sp+8]  := (from_iend)												��
		;//[%sp+12] := (from_jend)												��
		;//[%sp+16] := %r0													��
		;//[%sp+20] := %r1													��
		;//[%sp+24] := %r2													��
		;//[%sp+28] := %r3													��
		;//[%sp+32] := retp													��
		;//[%sp+36] := width													��
		;//[%sp+40] := compare													��
		;//[%sp+44] := arg													��
		xld.w		%r9, [%sp+36]			;//%r9      :=         width						��
		sub		%r14, %r13			;//%r14     := from_to = to - from					��
		xld.w		[%sp+0], %r14			;//[%sp+0]  := from_to							��
		mlt.w		%r12, %r9			;//%alr     := size *= width						��
		ld.w		%r12, %alr			;//%r12     := size							��
		xld.w		[%sp+4], %r12			;//[%sp+4]  := size							��
		mlt.w		%r15, %r9			;//%alr     :=                  num *= width				��
		ld.w		%r0, %alr			;//%r0      :=                  num					��
		add		%r0, %r13			;//%r0      := from_num = &from[num]					��
		;//- - - - - - - - - - - - - - - - - - - - - - -;//%r13     := from_jend = from						��
mergeseqs_LOOP1:						;//for(;;) {								��
		cmp		%r13, %r0			;//  if(from_jend >= from_num) { goto EXIT1 }	��������������������������
		jruge		mergeseqs_EXIT1			;//  									��
		xld.w		%r3, [%sp+0]			;//  %r3      :=                 from_to				��
		xld.w		%r4, [%sp+4]			;//  %r4      :=                      size				��
		ld.w		%r1, %r13			;//  %r1      := from_i    = from_jend					��
		ld.w		%r5, %r1			;//  %r5      :=             from_i					��
		add		%r5, %r4			;//  %r5      := from_iend = from_i + size				��
		ld.w		%r2, %r5			;//  %r2      := from_j    = from_iend					��
		ld.w		%r6, %r2			;//  %r6      :=             from_j					��
		add		%r6, %r4			;//  %r6      := from_jend = from_j + size				��
		cmp		%r5, %r0			;//  if(from_iend > from_num) {						��
		jrule.d		3				;//    									��
		 cmp		%r6, %r0			;//   ��������������������������������������������			��	*delay*
		 ld.w		%r5, %r0			;//    %r5      := from_iend = from_num }	��			��
	;//	cmp		%r6, %r0			;//  if(from_jend > from_num) {		����������			��
		jrule.d		3				;//    									��
		 add		%r3, %r1			;//  %r3      := to_k = from_i + from_to				��	*delay*
		 ld.w		%r6, %r0			;//    %r6      := from_jend = from_num }				��
		xld.w		[%sp+8], %r5			;//  [%sp+8]  := from_iend						��
		xld.w		[%sp+12], %r6			;//  [%sp+12] := from_jend						��
		;//- - - - - - - - - - - - - - - - - - - - - - -;//									��
mergeseqs_LOOP2:						;//  for(;;) {								��
		xld.w		%r14, [%sp+8]			;//    %r14     := from_iend	����������������������������������	��
		xld.w		%r9, [%sp+12]			;//    %r9      := from_jend					��	��	*anti-interlock*
		cmp		%r1, %r14			;//    if(from_i >= from_iend) { goto EXIT2 }			��	��
		jruge		mergeseqs_EXIT2			;//    								��	��
		cmp		%r2, %r9			;//    if(from_j >= from_jend) { goto EXIT2 }			��	��
		jruge		mergeseqs_EXIT2			;//    								��	��
		xld.w		%r9, [%sp+40]			;//    %r9      :=   compare					��	��
		xld.w		%r14, [%sp+44]			;//    %r14     :=                            arg		��	��
		ld.w		%r12, %r1			;//    %r12     :=            from_i				��	��
		call.d		%r9				;//    %r10     := (*compare)(from_i, from_j, arg)		��	��
		ld.w		%r13, %r2			;//    %r13     :=                    from_j			��	��	*delay*
		xld.w		%r14, [%sp+36]			;//    %r14     :=                 width			��	��
		cmp		%r10, 0				;//    if(         (*compare)(from_i, from_j, arg) <= 0) {	��	��
		jrgt.d		5		;//����		;//      							��	��
		 ld.w		%r12, %r3	;//�@��		;//    %r12     := to_k						��	��	*delay*
		 ld.w		%r13, %r1	;//�@��		;//      %r13     :=     from_i		������������������	��	��
		 jp.d		4		;//�@��	;//����	;//    } else {						��	��	��
		 add		%r1, %r14	;//�@��	;//�@��	;//      %r1      :=     from_i += width		��	��	��	*delay*
		ld.w		%r13, %r2	;//����	;//�@��	;//      %r13     :=     from_j		������������������	��	��
		add		%r2, %r14		;//�@��	;//      %r2      :=     from_j += width		��	��	��
		call.d		mergeseqs_MEMCPY	;//����	;//      memcpy(   to_k, from_?,   width)	����������	��	��
		add		%r3, %r14			;//    %r3      := to_k         += width			��	��	*delay*
		jp		mergeseqs_LOOP2			;//  }								��	��	*anti-interlock*
mergeseqs_EXIT2:						;//  								��	��
		;//- - - - - - - - - - - - - - - - - - - - - - -;//								��	��
	;//	xld.w		%r14, [%sp+8]			;//  %r14     :=                 from_iend	������������������	��
		sub		%r14, %r1			;//  %r14     :=                 from_iend - from_i			��
		jrule		4				;//  if(                         from_iend > from_i) {			��
		 ld.w		%r12, %r3			;//    %r12     := to_k							��
		 call.d		mergeseqs_MEMCPY		;//    memcpy(     to_k, from_i, from_iend - from_i) }			��
		 ld.w		%r13, %r1			;//    %r13     :=       from_i						��	*delay*
		xld.w		%r14, [%sp+12]			;//  %r14     :=                 from_jend				��
		sub		%r14, %r2			;//  if(                         from_jend - from_j) {			��	!INTERLOCK!
		jrule		4				;//  if(                         from_jend > from_j) {			��
		 ld.w		%r12, %r3			;//    %r12     := to_k							��
		 call.d		mergeseqs_MEMCPY		;//    memcpy(     to_k, from_j, from_jend - from_j) }			��
		 ld.w		%r13, %r2			;//    %r13     :=       from_j						��	*delay*
		xld.w		%r13, [%sp+12]			;//  %r13     := from_jend	������������������������������������������
		jp		mergeseqs_LOOP1			;//}
mergeseqs_EXIT1:						;//
		;//- - - - - - - - - - - - - - - - - - - - - - -;//
		xadd		%sp, %sp, 16			;//
		popn		%r3				;//
		ret						;//
		;//- - - - - - - - - - - - - - - - - - - - - - -;//
mergeseqs_MEMCPY:						;//
		xjp		memcpy				;//���֐��ɂ�memcpy�Ăяo����3�����L��̂ŁA������3������1����(jp)�ɂ��āA�����ɒǉ���2����(xjp)��݂�������A�S�̂Ƃ��Ă�1���߂̐ߖ�ɂȂ�B
");
static void __attribute__((noreturn,unused))/*asm�u���b�N����Q��*/ mergesort_r_DIE() { DIE(); }
#endif//PIECE
#endif

/*****************************************************************************
 *	�e�X�g�X�C�[�g
 *****************************************************************************/
#if 0
static void test_subr(void (*sort_r)(void*, size_t, size_t, int (*)(const void*,const void*, void*), void*));	//�O���Q��
//2016/06/10���݁AWin32����P/ECE���Ő���ɓ��삷�鎖���m�F���܂����B
void test() {
	int i;
	for(i = 0; i < 100; i++) { test_subr(qsort_r); }											//qsort_r�̃e�X�g
  #ifndef PIECE	//{{2016/06/10:P/ECE���ł̓R�[�h�T�C�Y�ߖ�̂��߂Ƀq�[�v�\�[�g�𖳌������Ă��܂��B�L�������鎖���\�ł��B}}
	for(i = 0; i < 100; i++) { test_subr((void (*)(void*, size_t, size_t, int (*)(const void*,const void*, void*), void*))heapsort_r); }	//heapsort_r�̃e�X�g
  #endif//PIECE	//{{2016/06/10:P/ECE���ł̓R�[�h�T�C�Y�ߖ�̂��߂Ƀq�[�v�\�[�g�𖳌������Ă��܂��B�L�������鎖���\�ł��B}}
	for(i = 0; i < 100; i++) { test_subr((void (*)(void*, size_t, size_t, int (*)(const void*,const void*, void*), void*))mergesort_r); }	//mergesort_r�̃e�X�g
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
	//�����_���ȕ�����("0A"�`"9Z")���쐬����B
	for(i = 0; i < N; i++) {
		data[i][0] = RND32_RANGE(seed, '0', '9' + 1);
		data[i][1] = RND32_RANGE(seed, 'A', 'Z' + 1);
		data[i][2] = '\0';
	}
	//��ɁA�������2�����ڂŃ\�[�g����B
	(*sort_r)(&data[0][0], N, 2+1/*nul*/, compar, (void*)1);
	//���ɁA�������1�����ڂŃ\�[�g����B
	(*sort_r)(&data[0][0], N, 2+1/*nul*/, compar, (void*)0);
	//���ʂ��m�F����B
	for(i = 0; i < N; i++) {
	    //	printf("%s\n", data[i]);
		if(i && (strncmp(data[i - 1], data[i], 1) > 0)) { DIE(); }										//�ǂ̃\�[�g�ł��A��Ń\�[�g������(1������)�͏����ɂȂ��Ă���͂��B
		if(i && ( strcmp(data[i - 1], data[i]   ) > 0)) { if((void*)sort_r == (void*)mergesort_r) { DIE(); } /*else { printf("*"); }*/ }	//mergesort_r�͈���\�[�g�Ȃ̂Ő�Ƀ\�[�g������(2������)�͈ێ�����Ă���͂��Bqsort_r,heapsort_r�͕s����\�[�g�Ȃ̂Ő�Ƀ\�[�g������(2������)�͕s��ł���B
	}
    #undef  N
}
#endif

/****************************************************************************
 *	���x�v������(2016/06/11)
 ****************************************************************************/
// * Sat Jun 11 20:47:59 JST 2016 Naoyuki Sawa
// - clipsort.c�̈�ԉ��ɁA�u���x�v������(2016/06/11)�v��ǋL���܂����B
//   Win10(x64)��P/ECE�ŁA�e�֐��̑��x���v���������ʂƁA�l�@�ł��B
//   {{�v���v���O����:��������
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
//   }}�v���v���O����:�����܂�
// - ������
//   test(qsort_r)�̌���:
//   �ECore i7-4500U	   78[ms]	VC++6.0,�����[�X�s���h
//   �EP/ECE		43407[ms]	C�����qsort_r()
//   �EP/ECE		41404[ms]	�A�Z���u����qsort_r()
//   test(heapsort_r)�̌���:
//   �ECore i7-4500U	45953[ms]	VC++6.0,�����[�X�s���h
//   �EP/ECE		(���Ή�)
//   test(mergesort_r)�̌���:
//   �ECore i7-4500U	   63[ms]	VC++6.0,�����[�X�s���h
//   �EP/ECE		12119[ms]	C�����qsort_r()
//   �EP/ECE		11377[ms]	�A�Z���u����qsort_r()
// - ���l�@
//   �E�ӊO�ɂ�qsort_r()����mergesort_r()�̕��������B
//     ��L�̌��ʂ�mergesort_r()����malloc/free�̎��Ԃ��܂�ł��邪�A����ł�mergesort_r()�̕��������B
//     �������A����̓f�[�^���傫���̂�malloc/free����߂銄���������������ŁA�f�[�^����������malloc/free����߂銄�����傫���Ȃ��Č��ʂ��t�]����\���͗L��B
//   �E���I�ȃ������m�ۂ��s���Ă���Ƃ������_�������΁Amergesort_r()������\�[�g�������\�ǂ��݂������B
//     ����܂ł͖�������qsort_r()���g���Ă������A�����mergesort_r()�̎g�p���ϋɓI�Ɍ������悤�B
//     ���ɁA���I�������m�ۂ̃R�X�g��������PC���ł́Amergesort_r()��ϋɓI�Ɏg���Ă��ǂ��������B
//   �E����ł�heapsort_r()���ɒ[�ɒx���B
//     ���݂�heapsort_r()�͓��{��ŃE�B�L�y�f�B�A�̃q�[�v�\�[�g�̃y�[�W�����č�����̂����A�ǂ����E�B�L�y�f�B�A�̃T���v���͂��Ȃ�x�������ł���悤���B
//     �Q�Ǝ���:�uDreamHope�v(http://www.dreamhope.net/)����́w�q�[�v�\�[�g�łȂ��ċA�H�`���{��ŃE�B�L�y�f�B�A�̖��_�ƍŋ��ő��̃q�[�v�\�[�g�`�x(http://www.dreamhope.net/soliloquies/HeapSort/)
//     ������A��L�̋L�����Q�l�ɂ��āAheapsort_r()����蒼�����B	������������������������������������������������������������������������������������������������������������������������������������������������������������������TODO:
