/*
 *	strv.h
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
#ifndef __STRV_H__
#define __STRV_H__
#ifdef  __cplusplus
extern "C" {
#endif//__cplusplus
/*****************************************************************************
 *	
 *****************************************************************************/
char** strv_new(const char* s, ...);
char** strv_new_ap(const char* s, va_list ap);
int strv_push(char*** pv, char* s);
int strv_extend(char*** pv, const char* s);
char** strv_append(char** v, const char* s);
char** strv_merge(char** v1, char** v2);
char** strv_remove(char** v, const char* s);
char** strv_uniq(char** v);
char* strv_find(char** v, const char* s);
int strv_overlap(char** v1, char** v2);
char** strv_sort(char** v);
void strv_print(char** v);
//-----------------------------------------------------------------------------
//�����̊֐��Ɠ����@�\�ł���֐��́A�}�N���Ƃ��Ď������鎖�ɂ��܂����B
#define strv_free(v)		strfreev(v)			//��clipstr.c
#define strv_copy(v)		strdupv(v)			//��clipstr.c
//#define strv_length(v)	�������O			//��clipstr.c
#define strv_split(s,delimiter)	strsplit((s),(delimiter),0)	//��clipstr.c
#define strv_join(v,separator)	strjoinv((separator),(v))	//��clipstr.c		�����������t�ł��鎖�ɒ��ӂ���B
#define strv_split_quoted(s)	parse_cmdline(s)		//��clipmisc.c		�����ɂ͈Ⴄ�����m��Ȃ�����̓������삾�낤�B
//-----------------------------------------------------------------------------
//�I���W�i���łɑ��݂���ȉ��̊֐��́A�������ɂ͊܂߂Ȃ����ɂ��܂����B
//char* strv_find_prefix(char** v, const char* s);	//�ˎg�����������Ǝv���̂Ŏ������Ȃ����ɂ����B
//char** strv_remove_prefix(char** v, const char* s);	//�ˎg�����������Ǝv���̂Ŏ������Ȃ����ɂ����B
//char** strv_parse_nulstr(const char* s, size_t len);	//�ˎg�����������Ǝv���̂Ŏ������Ȃ����ɂ����B����ɋ߂�������argz_extract()�ōs����B
//char** strv_split_nulstr(const char* s);		//�ˎg�����������Ǝv���̂Ŏ������Ȃ����ɂ����B����ɋ߂�������argz_extract()�ōs����B
//char** strv_merge_concat(char** a, char** b, const char* suffix);	//Like strv_merge(), but appends suffix to all strings in b, before adding.	�ˎg�����������Ǝv���̂Ŏ������Ȃ����ɂ����B
//char** strv_split_newlines(const char* s);	//Special version of strv_split() that splits on newlines and suppresses an empty string at the end.	�ˎg�����������Ǝv���̂Ŏ������Ȃ����ɂ����B
#ifdef  __cplusplus
}//extern "C"
#endif//__cplusplus
#endif//__STRV_H__
