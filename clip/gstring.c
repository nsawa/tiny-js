/*
 *	gstring.c
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
#include "clip.h"
/*****************************************************************************
 *	�O���[�o���֐�
 *****************************************************************************/
//Creates a new GString, initialized with the given string.
//Parameters
//		init		The initial text to copy into the string, or NULL to start with an empty string.	[nullable]
//Returns
//		The new GString.
GString* g_string_new(const char* init) {
	return g_string_new_len(init/*NULL��*/, -1);
}
/*--------------------------------------------------------------------------*/
//Creates a new GString with len bytes of the init buffer.
//Because a length is provided, init need not be nul-terminated, and can contain embedded nul bytes.								�˓������͖��ߍ��݃k���o�C�g�ɑΉ����Ă��܂���B
//Since this function does not stop at nul bytes, it is the caller's responsibility to ensure that init has at least len addressable bytes.
//Parameters
//		init		Initial contents of the string.
//		len		Length of init to use.
//Returns
//		A new GString.
GString* g_string_new_len(const char* init, int len) {
	GArray* array = (GArray*)g_string_sized_new(0);
	if(init/*NULL��*/) {
		len = strnlen(init, len);			//�������͖��ߍ��݃k���o�C�g�ɑΉ����Ă��܂���Blen�ɑ傫�Ȓl(����,�}�C�i�X�l)���w�肵�Ă��A���ߍ��݃k���o�C�g���L��΂��̒����܂łƉ��߂��܂��B
		g_array_append_vals(array, init, len);
	}
	return (GString*)array;
}
/*--------------------------------------------------------------------------*/
//Creates a new GString, with enough space for dfl_size bytes.
//This is useful if you are going to add a lot of text to the string and don't want it to be reallocated too often.
//Parameters
//		dfl_size	The default size of the space allocated to hold the string.
//Returns
//		The new GString.
GString* g_string_sized_new(int dfl_size) {
	GArray* array = g_array_sized_new(1/*zero_terminated*/, 1/*clear*/, sizeof(char), dfl_size);
	return (GString*)array;
}
/*--------------------------------------------------------------------------*/
//Copies the bytes from a string into a GString, destroying any previous contents.
//It is rather like the standard strcpy() function, except that you do not have to worry about having enough space to copy the string.
//Parameters
//		string		The destination GString. Its current contents are destroyed.
//		rval		The string to copy into string.		string->str���g,����,string->str�̓r�����w���Ă��Ă����S�ł��B
//Returns
//		The new GString.	[transfer none]
GString* g_string_assign(GString* string, const char* rval) {
	//����rval��string->str�Ɠ���,����,string->str�̓r�����w���Ă����ꍇ�A�����Ȃ�g_string_truncate()���s���ƁAg_string_append()���s���O��rval���j�󂳂�Ă��܂���肪������B
	//��L�̖�������邽�߂ɁAg_string_truncate()���s���O��rval�𕡐����Ă����A��������rval���g����g_string_append()���s���B
	//�I���W�i���ł�gstring.c�ł��A(�S�������ł͂Ȃ���)���l�̑΍􂪎���Ă���B
	//�{���A���̖���g_string_assign()�����łȂ��Ag_array_insert_vals()���ł��N���蓾��̂����Ag_array_insert_vals()�ł͑΍􂳂�Ă��Ȃ��B(�I���W�i���łł������C�u�����ł�)
	//g_array_insert_vals()�ł���Ȏg���������鎖��(�܂�)�������Ag_string_assign()�ł͎��ȑ�����p�ɂɋN���蓾�邩�炾�Ǝv���B
	char* tmp = strdup(rval);	//������𕡐�����B
	if(!tmp) { DIE(); }
	g_string_truncate(string, 0);
	g_string_append(string, tmp);	//����������������g����g_string_append()���s���B
	free(tmp);			//����������������J������B
	return string;
}
/*--------------------------------------------------------------------------*/
//Writes a formatted string into a GString.
//This function is similar to g_string_printf() except that the arguments to the format string are passed as a va_list.
//Parameters
//		string		A GString.
//		format		The string format. See the printf() documentation.
//		args		The parameters to insert into the format string.
void g_string_vprintf(GString* string, const char* format, va_list args) {
	char* tmp = strdup_vprintf(format, args);
	g_string_assign(string, tmp);
	free(tmp);
}
/*--------------------------------------------------------------------------*/
//Appends a formatted string onto the end of a GString.
//This function is similar to g_string_append_printf() except that the arguments to the format string are passed as a va_list.
//Parameters
//		string		A GString.
//		format		The string format. See the printf() documentation.
//		args		The list of arguments to insert in the output.
void g_string_append_vprintf(GString* string, const char* format, va_list args) {
	char* tmp = strdup_vprintf(format, args);
	g_string_append(string, tmp);
	free(tmp);
}
/*--------------------------------------------------------------------------*/
//Writes a formatted string into a GString.
//This is similar to the standard sprintf() function, except that the GString buffer automatically expands to contain the results.
//The previous contents of the GString are destroyed.
//Parameters
//		string		A GString.
//		format		The string format. See the printf() documentation.
//		...		The parameters to insert into the format string.
void g_string_printf(GString* string, const char* format, ...) {
	va_list args;
	va_start(args, format);
	g_string_vprintf(string, format, args);
	va_end(args);
}
/*--------------------------------------------------------------------------*/
//Appends a formatted string onto the end of a GString.
//This function is similar to g_string_printf() except that the text is appended to the GString.
//Parameters
//		string		A GString.
//		format		The string format. See the printf() documentation.
//		...		The parameters to insert into the format string.
void g_string_append_printf(GString* string, const char* format, ...) {
	va_list args;
	va_start(args, format);
	g_string_append_vprintf(string, format, args);
	va_end(args);
}
/*--------------------------------------------------------------------------*/
//Adds a string onto the end of a GString, expanding it if necessary.
//Parameters
//		string		A GString.
//		val		The string to append onto the end of string.
//Returns
//		String.		[transfer none]
GString* g_string_append(GString* string, const char* val) {
	return g_string_insert(string, -1, val);
}
/*--------------------------------------------------------------------------*/
//Adds a byte onto the end of a GString, expanding it if necessary.
//Parameters
//		string		A GString.
//		c		The byte to append onto the end of string.
//Returns
//		String.		[transfer none]
GString* g_string_append_c(GString* string, char c) {
	return g_string_insert_c(string, -1, c);
}
/*--------------------------------------------------------------------------*/
//GString* g_string_append_unichar(GString* string, wchar_t wc);												//���Ή�
/*--------------------------------------------------------------------------*/
//Appends len bytes of val to string.
//Because len is provided, val may contain embedded nuls and need not be nul-terminated.									�˓������͖��ߍ��݃k���o�C�g�ɑΉ����Ă��܂���B
//Since this function does not stop at nul bytes, it is the caller's responsibility to ensure that val has at least len addressable bytes.
//Parameters
//		string		A GString.
//		val		Bytes to append.
//		len		Number of bytes of val to use.
//Returns
//		String.		[transfer none]
GString* g_string_append_len(GString* string, const char* val, int len) {
	return g_string_insert_len(string, -1, val, len);
}
/*--------------------------------------------------------------------------*/
//GString* g_string_append_uri_escaped(GString* string, const char* unescaped, const char* reserved_chars_allowed, int allow_utf8);				//���Ή�
/*--------------------------------------------------------------------------*/
//Adds a string on to the start of a GString, expanding it if necessary.
//Parameters
//		string		A GString.
//		val		The string to prepend on the start of string.
//Returns
//		String.		[transfer none]
GString* g_string_prepend(GString* string, const char* val) {
	return g_string_insert(string, 0, val);
}
/*--------------------------------------------------------------------------*/
//Adds a byte onto the start of a GString, expanding it if necessary.
//Parameters
//		string		A GString.
//		c		The byte to prepend on the start of the GString.
//Returns
//		String.		[transfer none]
GString* g_string_prepend_c(GString* string, char c) {
	return g_string_insert_c(string, 0, c);
}
/*--------------------------------------------------------------------------*/
//GString* g_string_prepend_unichar(GString* string, wchar_t wc);												//���Ή�
/*--------------------------------------------------------------------------*/
//Prepends len bytes of val to string.
//Because len is provided, val may contain embedded nuls and need not be nul-terminated.									�˓������͖��ߍ��݃k���o�C�g�ɑΉ����Ă��܂���B
//Since this function does not stop at nul bytes, it is the caller's responsibility to ensure that val has at least len addressable bytes.
//Parameters
//		string		A GString.
//		val		Bytes to prepend.
//		len		Number of bytes in val to prepend.
//Returns
//		String.		[transfer none]
GString* g_string_prepend_len(GString* string, const char* val, int len) {
	return g_string_insert_len(string, 0, val, len);
}
/*--------------------------------------------------------------------------*/
//Inserts a copy of a string into a GString, expanding it if necessary.
//Parameters
//		string		A GString.
//		pos		The position to insert the copy of the string.
//		val		The string to insert.
//Returns
//		string.		[transfer none]
GString* g_string_insert(GString* string, int pos, const char* val) {
	return g_string_insert_len(string, pos, val, -1);
}
/*--------------------------------------------------------------------------*/
//Inserts a byte into a GString, expanding it if necessary.
//Parameters
//		string		A GString.
//		pos		The position to insert the byte.
//		c		The byte to insert.
//Returns
//		String.		[transfer none]
GString* g_string_insert_c(GString* string, int pos, char c) {
	return g_string_insert_len(string, pos, &c, 1);
}
/*--------------------------------------------------------------------------*/
//GString* g_string_insert_unichar(GString* string, int pos, wchar_t wc);											//���Ή�
/*--------------------------------------------------------------------------*/
//Inserts len bytes of val into string at pos.
//Because len is provided, val may contain embedded nuls and need not be nul-terminated.									�˓������͖��ߍ��݃k���o�C�g�ɑΉ����Ă��܂���B
//If pos is -1, bytes are inserted at the end of the string.
//Since this function does not stop at nul bytes, it is the caller's responsibility to ensure that val has at least len addressable bytes.
//Parameters
//		string		A GString.
//		pos		Position in string where insertion should happen, or -1 for at the end.
//		val		Bytes to insert.
//		len		Number of bytes of val to insert.
//Returns
//		String.		[transfer none]
GString* g_string_insert_len(GString* string, int pos, const char* val, int len) {
	GArray* array = (GArray*)string;
	//����val��string->str�Ɠ���,����,string->str�̓r�����w���Ă����ꍇ�Ag_array_insert_vals()�̏�������val���j�󂳂�Ă��܂���肪������B
	//��L�̖�������邽�߂ɁAg_array_insert_vals()���s���O��val�𕡐����Ă����A��������val���g����g_array_insert_vals()���s���B
	//�{���A���̖���g_string_insert_len()�����łȂ��Ag_array_insert_vals()���̂ł��N���蓾��̂����Ag_array_insert_vals()�ł͑΍􂳂�Ă��Ȃ��B(�I���W�i���łł������C�u�����ł�)
	//g_array_insert_vals()���܂�garray.c�̓v���~�e�B�u�ȋ@�\��񋟂��郂�W���[���ł���A��L�̂悤�ȃP�[�X�̑Ώ��̓A�v���P�[�V�������ɔC����Ă���̂��Ǝv���B
	//����Agstring.c�͕����񑀍�ɓ����������W���[���ł��邩��A��ʓI�Ȏg�p�ŋN����Ղ����ɑ΂��ẮA���W���[�����őΏ�������j�ł���B
	char* tmp = strndup(val, len);	//������𕡐�����B	//�������͖��ߍ��݃k���o�C�g�ɑΉ����Ă��܂���Blen�ɑ傫�Ȓl(����,�}�C�i�X�l)���w�肵�Ă��A���ߍ��݃k���o�C�g���L��΂��̒����܂łƉ��߂��܂��B
	if(!tmp) { DIE(); }
	len = strlen(tmp);
	if(pos < 0) { pos = string->len; }	//g_array_insert_vals()�ɂ�(index<0)���w��o���Ȃ��̂ł����Œu�������܂��B
	g_array_insert_vals(array, pos, tmp, len);
	free(tmp);			//����������������J������B
	return string;
}
/*--------------------------------------------------------------------------*/
//Overwrites part of a string, lengthening it if necessary.
//Parameters
//		string		A GString.
//		pos		The position at which to start overwriting.
//		val		The string that will overwrite the string starting at pos.
//Returns
//		String.		[transfer none]
GString* g_string_overwrite(GString* string, int pos, const char* val) {
	return g_string_overwrite_len(string, pos, val, -1);
}
/*--------------------------------------------------------------------------*/
//Overwrites part of a string, lengthening it if necessary.
//This function will work with embedded nuls.
//Parameters
//		string		A GString.
//		pos		The position at which to start overwriting.
//		val		The string that will overwrite the string starting at pos.
//		len		The number of bytes to write from val.
//Returns
//		String.		[transfer none]
GString* g_string_overwrite_len(GString* string, int pos, const char* val, int len) {
	GArray* array = (GArray*)string;
	//����val��string->str�Ɠ���,����,string->str�̓r�����w���Ă����ꍇ�Ag_array_set_size(),����,memcpy()�̏�������val���j�󂳂�Ă��܂���肪������B
	//��L�̖�������邽�߂ɁAg_array_set_size(),�y��,memcpy()���s���O��val�𕡐����Ă����A��������val���g����memcpy()���s���B
	//�I���W�i���ł̓��֐��̎����͂��̑΍􂪎���Ă��Ȃ��悤�����A�������ł͑΍������Ă������ɂ����B
	char* tmp = strndup(val, len);	//������𕡐�����B
	if(!tmp) { DIE(); }
	len = strlen(tmp);					//�������͖��ߍ��݃k���o�C�g�ɑΉ����Ă��܂���Blen�ɑ傫�Ȓl(����,�}�C�i�X�l)���w�肵�Ă��A���ߍ��݃k���o�C�g���L��΂��̒����܂łƉ��߂��܂��B
	if((pos + len) > string->len) {
		g_array_set_size(array, pos + len);
	}
	memcpy(string->str + pos, tmp, len);
	free(tmp);			//����������������J������B
	return string;
}
/*--------------------------------------------------------------------------*/
//Removes len bytes from a GString, starting at position pos.
//The rest of the GString is shifted down to fill the gap.
//Parameters
//		string		A GString.
//		pos		The position of the content to remove.
//		len		The number of bytes to remove, or -1 to remove all following bytes.
//Returns
//		String.		[transfer none]
GString* g_string_erase(GString* string, int pos, int len) {
	GArray* array = (GArray*)string;
	if(len < 0) { len = string->len - pos; }	//g_array_remove_range()�ɂ�(length<0)���w��o���Ȃ��̂ł����Œu�������܂��B
	g_array_remove_range(array, pos, len);
	return string;
}
/*--------------------------------------------------------------------------*/
//Cuts off the end of the GString, leaving the first len bytes.
//Parameters
//		string		A GString.
//		len		The new size of string.
//Returns
//		String.		[transfer none]
GString* g_string_truncate(GString* string, int len) {
	if(len < string->len) {
		GArray* array = (GArray*)string;
		g_array_set_size(array, len);
	}
	return string;
}
/*--------------------------------------------------------------------------*/
//Sets the length of a GString.
//If the length is less than the current length, the string will be truncated.
//If the length is greater than the current length, the contents of the newly added area are undefined.
//(However, as always, string->str[string->len] will be a nul byte.)
//Parameters
//		string		A GString.
//		len		The new length.
//Returns
//		String.		[transfer none]
GString* g_string_set_size(GString* string, int len) {
	GArray* array = (GArray*)string;
	g_array_set_size(array, len);
	return string;
}
/*--------------------------------------------------------------------------*/
//Frees the memory allocated for the GString.
//If free_segment is TRUE it also frees the character data.
//If it's FALSE, the caller gains ownership of the buffer and must free it after use with g_free().
//Parameters
//		string		A GString.						[transfer full]
//		free_segment	If TRUE, the actual character data is freed as well.
//Returns
//		The character data of string (i.e. NULL if free_segment is TRUE).	[nullable]
char* g_string_free(GString* string, int free_segment) {
	GArray* array = (GArray*)string;
	return g_array_free(array, free_segment);
}
/*--------------------------------------------------------------------------*/
//GBytes* g_string_free_to_bytes(GString* string);														//���Ή�
/*--------------------------------------------------------------------------*/
//Creates a hash code for str ; for use with GHashTable.
//Parameters
//		str		A string to hash.
//Returns
//		Hash code for str.
int g_string_hash(const GString* str) {
	return one_at_a_time_hash(str->str, str->len);
}
/*--------------------------------------------------------------------------*/
//Compares two strings for equality, returning TRUE if they are equal.
//For use with GHashTable.
//Parameters
//		v		A GString.
//		v2		Another GString.
//Returns
//		TRUE if the strings are the same length and contain the same bytes.
int g_string_equal(const GString* v, const GString* v2) {
	return !strcmp(v->str, v2->str);
}
/*--------------------------------------------------------------------------*/
//GString* g_string_ascii_down(GString* string);														//���Ή�
//GString* g_string_ascii_up(GString* string);															//���Ή�
/*--------------------------------------------------------------------------*/
