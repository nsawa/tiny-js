/*
 *	gstring.h
 *
 *	Strings - text buffers which grow automatically as text is added
 *
 *	* Tue May 24 23:59:14 JST 2016 Naoyuki Sawa
 *	- 1st �����[�X�B
 *	- GLib��Strings(http://developer.gnome.org/glib/stable/glib-Strings.html)���Q�l�ɂ��܂����B
 *	  �֐��d�l�̓I���W�i���łƓ����ł����A�����͓Ǝ��ŁA�������x�����e�ʍ팸��D�悵�܂����B
 *	- �I���W�i���ł�gstring.c���W���[���P�ƂŎ�������Ă��܂����A�������ł�garray.c���W���[���𗘗p���Ď������܂����B
 *	  GString�̋@�\��GArray�̋@�\�ɋ߂��̂ŁA������GArray���g�p���Ď�������ƁA�V�K�R�[�h�쐬�̎�Ԃ��Ȃ��邩��ł��B
 *	* Wed May 25 10:22:02 JST 2016 Naoyuki Sawa
 *	- GString.allocated_len�t�B�[���h�ɂ��āA���ӏ�����ǋL���܂����B
 */
#ifndef __G_STRING_H__
#define __G_STRING_H__
#ifdef	__cplusplus
extern "C" {
#endif//__cplusplus
/*****************************************************************************
 *	�\����
 *****************************************************************************/
typedef struct _GString {
	char*		str;			//Points to the character data. It may move as text is added. The str field is null-terminated and so can be used as an ordinary C string.	�� GRealArray.data
	int		len;			//Contains the length of the string, not including the terminating nul byte.									�� GRealArray.len
/*!!*/	int		allocated_len;		//The number of bytes that can be stored in the string before it needs to be reallocated. May be larger than len.				�� GRealArray.alloc_bytes
} GString;
// * Wed May 25 10:22:02 JST 2016 Naoyuki Sawa
// - GString.allocated_len�t�B�[���h�ɂ��āA���ӏ�����ǋL���܂����B
// - allocated_len�͎����v���C�x�[�g�t�B�[���h�ł���A�A�v���P�[�V���������ڎQ�Ƃ��Ă͂����܂���B
//   'str[len]�`str[allocated_len-1]�͈̔͂ɁA�A�v���P�[�V���������ځA�ǉ��̕������i�[���ėǂ�'�Ƃ����Ӗ� *�ł͂Ȃ�* ���ɒ��ӂ��ĉ������B
//   2016/05/25���݁AGString�̎�����GArray��'zero_terminated=1'�̐ݒ�Ŏg�p���Ă���A��L�͈̔͂ɃA�v���P�[�V���������ځA�ǉ��̕������i�[����ƁAGArray��0���Ƒz�肵�Ă���ӏ���j�󂵂Ă��܂��\�����L��܂��B
// - '�A�v���P�[�V���������ځA�ǉ��̕������i�[���ėǂ��̈���m��'����ɂ́Ag_string_set_size()���Ăяo���K�v���L��A���̃T�C�Y��allocated_len�ł͂Ȃ�len�ɔ��f����܂��B
//   ��Ƃ��āAMFC��CString::GetBuffer()/CString::ReleaseBuffer()�Ɠ��l�̏������s���ꍇ�ɂ��āAallocated_len�Ɉˑ������Ԉ�������@�ƁAallocated_len�Ɉˑ����Ȃ����������@�������܂��B
//   ��allocated_len�Ɉˑ������Ԉ�������@
//   ��GString* s = g_string_sized_new(100);	//len=0,allocated_len>=100																						��
//   ��strcpy(s->str, "foo");			//s->str[1],s->str[2]��0�ȊO�ɂȂ鎖��GArray���z�肵�Ă��炸�A�Ȍ�̏����Ńk���I�[���K�v�ɂȂ������ɂ��̈ʒu�ɉ��߂�0���i�[����Ƃ͌���Ȃ��B���R���삷��\�����L�邪�A���삵�Ȃ��\�����L��B	�����B���������Aallocated_len�𖾎��I�ɐݒ肷����@��g_string_sized_new()��������������l���Ă��Aallocated_len�Ɉˑ�����̂͌��ł��鎖�͗e�Ղɑz�����t�����낤�B(�쐬�ς݂̕�����ɑ΂��ăT�C�Y���w�肵��GetBuffer()�o���Ȃ��Ƃ������ɂȂ��Ă��܂�����)
//   ��s = g_string_assign(s, s->str);		//len=3,allocated_len>=100																						��
//   ��allocated_len�Ɉˑ����Ȃ����������@
//   ��GString* s = g_string_sized_new(NULL);	//len=0,allocated_len=?
//   ��g_string_set_size(s, 100);		//len=100,allocated_len>=100	CString::GetBuffer(100)�ɑ�������B	��
//   ��strcpy(s->str, "foo");			//									���������B
//   ��s = g_string_assign(s, s->str);		//len=3,allocated_len>=100	CString::ReleaseBuffer(-1)�ɑ�������B	��
// - ��L�̗��R�ɂ��Aallocated_len���A�v���P�[�V���������ڎQ�Ƃ��Ă͂����܂���B
//   ����ɂ�������炸�Aallocated_len���A�A�v���P�[�V�������猩����GString�\���̓��ɒ�`������́A�I���W�i���ł�����������Ƃ������R�����ł��B
//   �������ł́AGArray���g����GString���������Ă��܂����A�I���W�i���ł�GString�P�ƂŎ�������Ă���̂ŁA���������Ŏg�p����allocated_len��GString���ɒ�`����Ă���̂��Ǝv���܂��B
//   GArray�̂悤�ɁA���J����\���̂Ɣ���J�̍\���̂ɕ�������@���L��܂����AGString�ł̓v���C�x�[�g�t�B�[���h��allocated_len�����Ȃ̂ŁA�������Ă��Ȃ��̂��Ǝv���܂��B
//   �������ł́AGString�\���̓���allocated_len���`�����ɁA�v���C�x�[�g�t�B�[���h�Ƃ��Ă��ǂ������̂ł����A�ꉞ�A�I���W�i���łɕ���Č��J���Ă������ɂ��܂����B
//   GArray�̃v���C�x�[�g�t�B�[���h(GRealArray.alloc_bytes)�����J���Ă���`�ƂȂ�܂��B
//   ���ۂɃA�v���P�[�V��������Q�Ƃ��鎖�́A�܂��A�����Ǝv���܂��B
//   ����ǂ��납�Agstring.c���W���[�����̏���������Q�Ƃ��Ă��܂���B
//   �������̎��ۂ̏����́A�قƂ�ǑS��GArray�̊֐����Ăяo�������ł���AGString.allocated_len(=GRealArray.alloc_bytes)��GArray�̊֐����ł����Q�Ƃ��Ȃ�����ł��B
/*****************************************************************************
 *	�O���[�o���֐�
 *****************************************************************************/
GString* g_string_new(const char* init);
GString* g_string_new_len(const char* init, int len);
GString* g_string_sized_new(int dfl_size);
GString* g_string_assign(GString* string, const char* rval);
void g_string_vprintf(GString* string, const char* format, va_list args);
void g_string_append_vprintf(GString* string, const char* format, va_list args);
void g_string_printf(GString* string, const char* format, ...);
void g_string_append_printf(GString* string, const char* format, ...);
GString* g_string_append(GString* string, const char* val);
GString* g_string_append_c(GString* string, char c);
//GString* g_string_append_unichar(GString* string, wchar_t wc);		//���Ή�
GString* g_string_append_len(GString* string, const char* val, int len);
//GString* g_string_append_uri_escaped(GString* string, const char* unescaped, const char* reserved_chars_allowed, int allow_utf8);	//���Ή�
GString* g_string_prepend(GString* string, const char* val);
GString* g_string_prepend_c(GString* string, char c);
//GString* g_string_prepend_unichar(GString* string, wchar_t wc);		//���Ή�
GString* g_string_prepend_len(GString* string, const char* val, int len);
GString* g_string_insert(GString* string, int pos, const char* val);
GString* g_string_insert_c(GString* string, int pos, char c);
//GString* g_string_insert_unichar(GString* string, int pos, wchar_t wc);	//���Ή�
GString* g_string_insert_len(GString* string, int pos, const char* val, int len);
GString* g_string_overwrite(GString* string, int pos, const char* val);
GString* g_string_overwrite_len(GString* string, int pos, const char* val, int len);
GString* g_string_erase(GString* string, int pos, int len);
GString* g_string_truncate(GString* string, int len);
GString* g_string_set_size(GString* string, int len);
char* g_string_free(GString* string, int free_segment);
//GBytes* g_string_free_to_bytes(GString* string);				//���Ή�
int g_string_hash(const GString* str);
int g_string_equal(const GString* v, const GString* v2);
//GString* g_string_ascii_down(GString* string);				//���Ή�
//GString* g_string_ascii_up(GString* string);					//���Ή�
#ifdef	__cplusplus
}//extern "C"
#endif//__cplusplus
#endif//__G_STRING_H__
