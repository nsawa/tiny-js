/*
 *	strv.c
 *
 *	strv�`���̃��[�e�B���e�B�֐�
 *
 *	* Wed Dec 16 00:31:23 JST 2015 Naoyuki Sawa
 *	- 1st �����[�X�B
 *	- ���L�̃��|�W�g���Ɋ܂܂�Ă���Astrv�`���̃��[�e�B���e�B�֐��Ɍ݊��Ȋ֐����������܂����B
 *	  ��https://github.com/haraldh/dracut/blob/master/install/strv.c
 *	  �֐��d�l�͏�L�̃I���W�i���łƓ����ŁA�����͓Ǝ��ł��B
 *	- strv�`���̃��[�e�B���e�B�֐��́A������clipstr.c,clipmisc.c���̒��ɂ����������݂��܂��B
 *	  �����̊֐��Ɠ����@�\�ł���֐��́A�}�N���Ƃ��Ď������鎖�ɂ��܂����B
 *	* Tue May 24 21:23:01 JST 2016 Naoyuki Sawa
 *	- �S�֐����A�Z���u�������܂����B
 *	  �R�[�h�T�C�Y��484�o�C�g��408�o�C�g�Ɍ���A76�o�C�g�������Ȃ�܂����B
 *	- 2016/05/21�ȑO�́A�A�v���P�[�V�����������I�Ɏg�p���Ȃ�����strv.o�̓����N����Ȃ������̂ŁAstrv.o���œK�����Ă��Ȃ������̂ł����A
 *	  2016/05/21�ȍ~�́Aclipstr.c��strsplit(),strsplit_set()����strv.o�𗘗p����悤�ɂȂ�Astrv.o����Ƀ����N�����悤�ɂȂ�܂����B
 *	  �]���āA����s����strv.o�̃R�[�h�T�C�Y�ጸ�́A�Ӌ`�̂���œK�����Ǝv���܂��B
 */
#include "clip.h"
/*****************************************************************************
 *	
 *****************************************************************************/
//�ψ����̊e������𕡐����āAstrv���쐬����B
//���g�p��
//��char** v = strv_new("123","456","789",NULL);
//��strv_print(v);//123 456 789
//��strv_free(v);
#ifndef PIECE
char** strv_new(const char* s, ...) {
	char** v;
	va_list ap;
	va_start(ap, s);
	v = strv_new_ap(s, ap);
	va_end(ap);
	return v;
}
//-----------------------------------------------------------------------------
//�ψ����̊e������𕡐����āAstrv���쐬����B
char** strv_new_ap(const char* s, va_list ap) {
	char** v = calloc(1, sizeof(char*));
	if(!v) { DIE(); }	//�������s��
	while(s) {
		strv_extend(&v, s);
		s = va_arg(ap, const char*);
	}
	return v;
}
#else //PIECE
char** strv_new(const char* s, ...);
char** strv_new_ap(const char* s, va_list ap);
asm("
		.code
		.align		1
		.global		strv_new
		.global		strv_new_ap
strv_new:
		;//[%sp+0] := retp
		;//[%sp+4] := s
		;//[%sp+8] := ...
		xld.w		%r12, [%sp+4]			;//%r12    := s
		xadd		%r13, %sp, 8			;//%r13    := ap = ...
		;/* FALLTHRU */					;//
		;//---------------------------------------------;//
strv_new_ap:
		;//%r12    := s
		;//%r13    := ap
		pushn		%r1				;//
		xsub		%sp, %sp, 4			;//
		ld.w		%r0, %r12			;//%r0     := s
		ld.w		%r1, %r13			;//%r1     := ap
		ld.w		%r12, 1				;//%r12    :=            1
		xcall.d		calloc				;//%r10    := v = calloc(1, sizeof(char*))
		ld.w		%r13, 4				;//%r13    :=               sizeof(char*)	*delay*
		cmp		%r10, 0				;//if(!v) { DIE() }
		jreq		strv_new_ap_DIE			;//
		xld.w		[%sp+0], %r10			;//[%sp+0] := v
strv_new_ap_LOOP:						;//
		cmp		%r0, 0				;//while(s) {
		jreq		strv_new_ap_RET			;//  
		ld.w		%r12, %sp			;//  %r12    :=  &v
		xcall.d		strv_extend			;//  strv_extend(&v, s)
		ld.w		%r13, %r0			;//  %r13    :=      s				*delay*
		ld.w		%r0, [%r1]+			;//  %r0     := s = va_arg(ap, const char*)
		jp		strv_new_ap_LOOP		;//}
strv_new_ap_RET:						;//
		xld.w		%r10, [%sp+0]			;//%r10    := v
		xadd		%sp, %sp, 4			;//
		popn		%r1				;//
		ret						;//return     v
");
static void __attribute__((noreturn,unused))/*asm�u���b�N����Q��*/ strv_new_ap_DIE() { DIE(); }
#endif//PIECE
//-----------------------------------------------------------------------------
//(*pv)�̖����ɕ�����s�𕡐����Ēǉ�����B
//�I���W�i���ł̎����̓������s��������������0�ȊO��Ԃ����A�������͕K������(0)��Ԃ��B
//���g�p��
//��char** v = strv_new("123","456","789",NULL);
//��strv_extend(&v,"ABC");
//��strv_print(v);//123 456 789 ABC
//��strv_free(v);
#ifndef PIECE
int strv_extend(char*** pv, const char* s) {
	char* t = strdup(s);
	if(!t) { DIE(); }
	return strv_push(pv, t);
}
//-----------------------------------------------------------------------------
//(*pv)�̖����Ƀ|�C���^s��ǉ�����B
//������s�͕�������Ȃ����ɒ��ӂ���B
//�I���W�i���ł̎����̓������s��������������0�ȊO��Ԃ����A�������͕K������(0)��Ԃ��B
//���g�p��
//��char** v = strv_new("123","456","789",NULL);
//��char* s = strdup("ABC");
//��strv_push(&v,s);
//��strv_print(v);//123 456 789 ABC
//��strv_free(v);
int strv_push(char*** pv, char* s) {
	char** v = *pv;
	int n = strv_length(v);
	v = realloc(v, sizeof(char*) * (n + 2));
	if(!v) { DIE(); }
	v[n + 0] = s;
	v[n + 1] = NULL;
	*pv = v;
	return 0;	//�������͕K������(0)��Ԃ��B
}
#else //PIECE
int strv_extend(char*** pv, const char* s);
int strv_push(char*** pv, char* s);
asm("
		.code
		.align		1
		.global		strv_extend
		.global		strv_push
strv_extend:
		;//%r12 := pv
		;//%r13 := s
		pushn		%r0				;//
		ld.w		%r0, %r12			;//%r0  := pv
		xcall.d		strdup				;//%r10 := t = strdup(s)
		ld.w		%r12, %r13			;//%r12 :=            s					*delay*
		cmp		%r10, 0				;//if(!t) { DIE() }
		jreq		strv_extend_push_DIE		;//
		ld.w		%r12, %r0			;//%r12 := pv
		ld.w		%r13, %r10			;//%r13 := t
		popn		%r0				;//
		;/* FALLTHRU */					;//
		;//---------------------------------------------;//
strv_push:
		;//%r12 := pv
		;//%r13 := s
		pushn		%r2				;//
		ld.w		%r0, %r12			;//%r0  := pv
		ld.w		%r1, %r13			;//%r1  := s
		ld.w		%r2, [%r0]			;//%r2  := v = *pv
		xcall.d		strv_length			;//%r10 := n = strv_length(v)				*anti-interlock*
		ld.w		%r12, %r2			;//%r12 :=                 v				*delay*
		ld.w		%r12, %r2			;//%r12 := v
		ld.w		%r2, %r10			;//%r2  := n
		sla		%r2, 2				;//%r2  := n * sizeof(char*)
		ld.w		%r13, %r2			;//%r13 :=                sizeof(char*) *  n
		xcall.d		realloc				;//%r10 := v = realloc(v, sizeof(char*) * (n + 2))
		add		%r13, 8				;//%r13 :=                sizeof(char*) * (n + 2)	*delay*
		cmp		%r10, 0				;//if(!v) { DIE() }
		jreq		strv_extend_push_DIE		;//
		add		%r2, %r10			;//%r2  := &v[n + 0]
		ld.w		[%r2]+, %r1			;//         v[n + 0] = s
		ld.w		[%r2], %r8			;//         v[n + 1] = NULL
		ld.w		[%r0], %r10			;//*pv = v
		popn		%r2				;//
		ret.d						;//return  0
		ld.w		%r10, 0				;//%r10 := 0						*delay*
");
static void __attribute__((noreturn,unused))/*asm�u���b�N����Q��*/ strv_extend_push_DIE() { DIE(); }
#endif//PIECE
//-----------------------------------------------------------------------------
//v�𕡐����A�����ɕ�����s�𕡐����Ēǉ�����B
//v�͕ύX����Ȃ��B
//���g�p��
//��char** v1 = strv_new("123","456","789",NULL);
//��char** v2 = strv_append(v1,"ABC");
//��strv_print(v1);//123 456 789
//��strv_print(v2);//123 456 789 ABC
//��strv_free(v1);
//��strv_free(v2);
#ifndef PIECE
char** strv_append(char** v, const char* s) {
	v = strv_copy(v);
	strv_extend(&v, s);
	return v;
}
#else //PIECE
char** strv_append(char** v, const char* s);
asm("
		.code
		.align		1
		.global		strv_append
strv_append:
		;//%r12 := v
		;//%r13 := s
		pushn		%r0				;//
		xsub		%sp, %sp, 4			;//
		xcall.d		strdupv				;//%r10    := v = strv_copy(v) = strdupv(v)
		ld.w		%r0, %r13			;//%r0     := s					*delay*
		xld.w		[%sp+0], %r10			;//[%sp+0] := v
		ld.w		%r12, %sp			;//%r12    :=             &v
		xcall.d		strv_extend			;//%r10    := strv_extend(&v, s)
		ld.w		%r13, %r0			;//%r13    :=                 s			*delay*
		xld.w		%r10, [%sp+0]			;//%r10    := v
		xadd		%sp, %sp, 4			;//
		popn		%r0				;//
		ret						;//return     v
");
#endif//PIECE
//-----------------------------------------------------------------------------
//v1��v2�̐[���R�s�[��A�������A�V����strv���쐬����B
//v1��v2���ύX����Ȃ��B
//���g�p��
//��char** v1 = strv_new("123","456","789",NULL);
//��char** v2 = strv_new("ABC","DEF","GHI",NULL);
//��char** v3 = strv_merge(v1,v2);
//��strv_print(v1);//123 456 789
//��strv_print(v2);//ABC DEF GHI
//��strv_print(v3);//123 456 789 ABC DEF GHI
//��strv_free(v1);
//��strv_free(v2);
//��strv_free(v3);
#ifndef PIECE
char** strv_merge(char** v1, char** v2) {
	char* s;
	v1 = strv_copy(v1);		//v1�̐[���R�s�[���쐬����B
	while((s = *v2++)) {		//v2�̊e������ɂ��āc
		strv_extend(&v1, s);	//������𕡐����Ēǉ�����B
	}
	return v1;
}
#else //PIECE
char** strv_merge(char** v1, char** v2);
asm("
		.code
		.align		1
		.global		strv_merge
strv_merge:
		;//%r12 := v1
		;//%r13 := v2
		pushn		%r0				;//
		xsub		%sp, %sp, 4			;//
		xcall.d		strdupv				;//%r10    := v1 = strv_copy(v1) = strdupv(v1)
		ld.w		%r0, %r13			;//%r0     := v2					*delay*
		xld.w		[%sp+0], %r10			;//[%sp+0] := v1
strv_merge_LOOP:						;//for(;;) {
		ld.w		%r13, [%r0]+			;//  %r13    := s = *v2++
		cmp		%r13, 0				;//  if(!s) { break }
		jreq		strv_merge_RET			;//
		ld.w		%r12, %sp			;//  %r12    :=  &v1
		xcall		strv_extend			;//  strv_extend(&v1, s)
		jp		strv_merge_LOOP			;//}
strv_merge_RET:							;//
		xld.w		%r10, [%sp+0]			;//%r10    := v1
		xadd		%sp, %sp, 4			;//
		popn		%r0				;//
		ret						;//return     v1
");
#endif//PIECE
//-----------------------------------------------------------------------------
//v�̒��́A������s�Ɠ������e�̕������S�ĊJ������B
//v��ύX���邪�A�k�������̕ύX�ł���A�������̍Ċ��蓖�Ă͍s��Ȃ��B
//�]���āA�|�C���^�͕ω������A�߂�l�͏�Ɉ���v�Ɠ����|�C���^�ł���B
//strv_uniq()�̎������Astrv_remove()���������̍Ċ��蓖�Ă��s��Ȃ����Ɉˑ����Ă���̂ŁAstrv_remove()�̋�����ύX���Ă͂����Ȃ��B
//���g�p��
//��char** v = strv_new("123","456","789","456",NULL);
//��strv_remove(v,"456");
//��strv_print(v);//123 789
//��strv_free(v);
#ifndef PIECE
char** strv_remove(char** v, const char* s) {
	char** i = v;
	char** j = v;
	char* t;
	while((t = *i++)) {
		if(!strcmp(s, t)) {
			free(t);
		} else {
			*j++ = t;
		}
	}
	*j = NULL;
	return v;	//����v�����̂܂ܕԂ��B
}
#else //PIECE
char** strv_remove(char** v, const char* s);
asm("
		.code
		.align		1
		.global		strv_remove
strv_remove:
		;//%r12 := v
		;//%r13 := s
		pushn		%r3				;//
		xsub		%sp, %sp, 4			;//
		xld.w		[%sp+0], %r12			;//[%sp+0] := v
		ld.w		%r0, %r12			;//%r0     := i = v
		ld.w		%r1, %r12			;//%r1     := j = v
		ld.w		%r2, %r13			;//%r2     := s
strv_remove_LOOP:						;//for(;;) {
		ld.w		%r3, [%r0]+			;//  %r3     := t = *i++
		cmp		%r3, 0				;//  if(!t) { break }
		jreq		strv_remove_RET			;//  
		ld.w		%r12, %r2			;//  %r12    :=        s
		xcall.d		strcmp				;//  %r10    := strcmp(s, t)
		ld.w		%r13, %r3			;//  %r13    :=           t	*delay*
		jreq		3				;//  if(strcmp(s, t)) {
		 ld.w		[%r1]+, %r3			;//    *j++  = t
		 jp		strv_remove_LOOP		;//  } else {
		xcall.d		free				;//    free(   t)
		ld.w		%r12, %r3			;//    %r12    := t		*delay*
		jp		strv_remove_LOOP		;//  }
strv_remove_RET:						;//}
		ld.w		[%r1], %r8			;//*j = NULL
		xld.w		%r10, [%sp+0]			;//%r10    := v
		xadd		%sp, %sp, 4			;//
		popn		%r3				;//
		ret						;//return     v
");
#endif//PIECE
//-----------------------------------------------------------------------------
//v�̒��́A�d�����镶�����S�ĊJ������B
//v��ύX���邪�A�k�������̕ύX�ł���A�������̍Ċ��蓖�Ă͍s��Ȃ��B
//�]���āA�|�C���^�͕ω������A�߂�l�͏�Ɉ���v�Ɠ����|�C���^�ł���B
//���g�p��
//��char** v = strv_new("123","456","789","456",NULL);
//��strv_uniq(v);
//��strv_print(v);//123 456 789
//��strv_free(v);
#ifndef PIECE
char** strv_uniq(char** v) {
	char** i = v;
	char* t;
	while((t = *i++)) {
		strv_remove(i, t);
	}
	return v;	//����v�����̂܂ܕԂ��B
}
#else //PIECE
char** strv_uniq(char** v);
asm("
		.code
		.align		1
		.global		strv_uniq
strv_uniq:
		;//%r12 := v
		pushn		%r1				;//
		ld.w		%r0, %r12			;//%r0  := v
		ld.w		%r1, %r0			;//%r1  := i = v
strv_uniq_LOOP:							;//for(;;) {
		ld.w		%r13, [%r1]+			;//  %r13 := t = *i++
		cmp		%r13, 0				;//  if(!t) { break }
		jreq		strv_uniq_RET			;//  
		xcall.d		strv_remove			;//  strv_remove(i, t)
		ld.w		%r12, %r1			;//  %r12 :=     i		*delay*
		jp		strv_uniq_LOOP			;//}
strv_uniq_RET:							;//
		ld.w		%r10, %r0			;//%r10 := v
		popn		%r1				;//
		ret						;//return  v
");
#endif//PIECE
//-----------------------------------------------------------------------------
//v�̒�����A������s�Ɠ������e�̕��������������B
//����������A���̕�����̃|�C���^��Ԃ��B
//������Ȃ���΁ANULL��Ԃ��B
//���g�p��
//��char** v = strv_new("123","456","789",NULL);
//��printf("%s\n",strv_find(v,"456"));//456
//��printf("%s\n",strv_find(v,"567"));//(null)
//��strv_free(v);
#ifndef PIECE
char* strv_find(char** v, const char* s) {
	char* t;
	while((t = *v++)) {
		if(!strcmp(s, t)) { break; }
	}
	return t;
}
#else //PIECE
char* strv_find(char** v, const char* s);
asm("
		.code
		.align		1
		.global		strv_find
strv_find:
		;//%r12 := v
		;//%r13 := s
		pushn		%r2				;//
		ld.w		%r0, %r12			;//%r0  := v
		ld.w		%r1, %r13			;//%r1  := s
strv_find_LOOP:							;//for(;;) {
		ld.w		%r2, [%r0]+			;//  %r2  := t = *v++
		cmp		%r2, 0				;//  if(!t) { break }
		jreq		strv_find_RET			;//  
		ld.w		%r12, %r1			;//  %r12 :=        s
		xcall.d		strcmp				;//  %r10 := strcmp(s, t)
		ld.w		%r13, %r2			;//  %r13 :=           t		*delay*
		cmp		%r10, 0				;//  if(!strcmp(s, t)) { break }
		jrne		strv_find_LOOP			;//}
strv_find_RET:							;//
		ld.w		%r10, %r2			;//%r10  := t
		popn		%r2				;//
		ret						;//return   t
");
#endif//PIECE
//-----------------------------------------------------------------------------
//v1��v2�̒��ɁA�������e�̕����񂪗L�邩����������B
//�L���1�A�������0��Ԃ��B
//���g�p��
//��char** v1 = strv_new("123","456","789",NULL);
//��char** v2 = strv_new("ABC","DEF","GHI",NULL);
//��char** v3 = strv_new("123","456","GHI",NULL);
//��printf("%d\n",strv_overlap(v1,v2));//0
//��printf("%d\n",strv_overlap(v1,v3));//1
//��strv_free(v1);
//��strv_free(v2);
//��strv_free(v3);
#ifndef PIECE
int strv_overlap(char** v1, char** v2) {
	char* s;
	while((s = *v1++)) {
		if(strv_find(v2, s)) { return 1; }
	}
	return 0;
}
#else //PIECE
int strv_overlap(char** v1, char** v2);
asm("
		.code
		.align		1
		.global		strv_overlap
strv_overlap:
		;//%r12 := v1
		;//%r13 := v2
		pushn		%r1				;//
		ld.w		%r0, %r12			;//%r0  := v1
		ld.w		%r1, %r13			;//%r1  := v2
strv_overlap_LOOP:						;//for(;;) {
		ld.w		%r10, [%r0]+			;//  %r10 := s = *v1++
		cmp		%r10, 0				;//  if(!s) { return 0 }	����������	�����ŕ��򂷂鎞�͊���%r10=0�ł��B
		jreq		strv_overlap_RET		;//  					��
		ld.w		%r12, %r1			;//  %r12 :=           v2		��
		xcall.d		strv_find			;//  %r10 := strv_find(v2, s)		��
		ld.w		%r13, %r10			;//  %r13 :=               s		��	*delay*
		cmp		%r10, 0				;//  if(strv_find(v2, s)) { return 1 }	��
		jreq		strv_overlap_LOOP		;//}					��
		ld.w		%r10, 1				;//%r10  := 1			����������
strv_overlap_RET:						;//					��
		popn		%r1				;//					��
		ret						;//return 0 or 1		����������
");
#endif//PIECE
//-----------------------------------------------------------------------------
//v���\�[�g����B
//v��ύX���邪�A�v�f���͕ω������A�������̍Ċ��蓖�Ă͍s��Ȃ��B
//�]���āA�|�C���^�͕ω������A�߂�l�͏�Ɉ���v�Ɠ����|�C���^�ł���B
//���g�p��
//��char** v = strv_new("ABC","123","DEF","456",NULL);
//��strv_sort(v);
//��strv_print(v);//123 456 ABC DEF
//��strv_free(v);
#ifndef PIECE
static int strv_sort_compar(const void* _x, const void* _y) {
	char* x = *(char**)_x;
	char* y = *(char**)_y;
	return strcmp(x, y);
}
char** strv_sort(char** v) {
	qsort(v, strv_length(v), sizeof(char*), strv_sort_compar);
	return v;
}
#else //PIECE
char** strv_sort(char** v);
asm("
		.code
		.align		1
		.global		strv_sort
strv_sort:
		;//%r12 := v
		pushn		%r0				;//
		xcall.d		strv_length			;//%r10 :=    strv_length(v)
		ld.w		%r0, %r12			;//%r0  := v								*delay*
		ld.w		%r12, %r0			;//%r12 := v
		ld.w		%r13, %r10			;//%r13 :=    strv_length(v)
	;//	ld.w		%r14, 4				;//%r14 :=                    sizeof(char*)	������������������
		xld.w		%r15, strv_sort_compar		;//%r15 :=                                   strv_sort_compar	��
		xcall.d		qsort				;//qsort(  v, strv_length(v), sizeof(char*), strv_sort_compar)	��
		ld.w		%r14, 4				;//%r14 :=                    sizeof(char*)	������������������	*delay*
		ld.w		%r10, %r0			;//%r10 := v
		popn		%r0				;//
		ret						;//return  v
		;//- - - - - - - - - - - - - - - - - - - - - - -;//
strv_sort_compar:
		ld.w		%r12, [%r12]			;//%r12 :=       x    = *(char**)_x
		ld.w		%r13, [%r13]			;//%r13 :=          y = *(char**)_y
		xjp		strcmp				;//return strcmp(x, y)
");
#endif//PIECE
//-----------------------------------------------------------------------------
//v�̊e��������A�W���o�͂Ɉ�s���\������B
#ifndef PIECE
void strv_print(char** v) {
	char* s;
	while((s = *v++)) {
		puts(s);
	}
}
#else //PIECE
void strv_print(char** v);
asm("
		.code
		.align		1
		.global		strv_print
strv_print:
		;//%r12 := v
		pushn		%r0				;//
		ld.w		%r0, %r12			;//%r0  := v
strv_print_LOOP:						;//for(;;) {
		ld.w		%r12, [%r0]+			;//  %r12 := s = *v++
		cmp		%r12, 0				;//  if(!s) { break }
		jreq		strv_print_RET			;//  
		xcall		puts				;//  puts(s)
		jp		strv_print_LOOP			;//}
strv_print_RET:							;//
		popn		%r0				;//
		ret						;//
");
#endif//PIECE