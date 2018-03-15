/*
 *	gstring.c
 *
 *	Strings - text buffers which grow automatically as text is added
 *
 *	* Tue May 24 23:59:14 JST 2016 Naoyuki Sawa
 *	- 1st リリース。
 *	- GLibのStrings(http://developer.gnome.org/glib/stable/glib-Strings.html)を参考にしました。
 *	  関数仕様はオリジナル版と同じですが、実装は独自で、処理速度よりも容量削減を優先しました。
 *	- オリジナル版はgstring.cモジュール単独で実装されていますが、当実装ではgarray.cモジュールを利用して実装しました。
 *	  GStringの機能はGArrayの機能に近いので、内部でGArrayを使用して実装すると、新規コード作成の手間が省けるからです。
 *	* Wed May 25 10:22:02 JST 2016 Naoyuki Sawa
 *	- GString.allocated_lenフィールドについて、注意書きを追記しました。
 */
#include "clip.h"
/*****************************************************************************
 *	グローバル関数
 *****************************************************************************/
//Creates a new GString, initialized with the given string.
//Parameters
//		init		The initial text to copy into the string, or NULL to start with an empty string.	[nullable]
//Returns
//		The new GString.
GString* g_string_new(const char* init) {
	return g_string_new_len(init/*NULL可*/, -1);
}
/*--------------------------------------------------------------------------*/
//Creates a new GString with len bytes of the init buffer.
//Because a length is provided, init need not be nul-terminated, and can contain embedded nul bytes.								⇒当実装は埋め込みヌルバイトに対応していません。
//Since this function does not stop at nul bytes, it is the caller's responsibility to ensure that init has at least len addressable bytes.
//Parameters
//		init		Initial contents of the string.
//		len		Length of init to use.
//Returns
//		A new GString.
GString* g_string_new_len(const char* init, int len) {
	GArray* array = (GArray*)g_string_sized_new(0);
	if(init/*NULL可*/) {
		len = strnlen(init, len);			//当実装は埋め込みヌルバイトに対応していません。lenに大きな値(又は,マイナス値)を指定しても、埋め込みヌルバイトが有ればその長さまでと解釈します。
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
//		rval		The string to copy into string.		string->str自身,又は,string->strの途中を指していても安全です。
//Returns
//		The new GString.	[transfer none]
GString* g_string_assign(GString* string, const char* rval) {
	//もしrvalがstring->strと同じ,又は,string->strの途中を指していた場合、いきなりg_string_truncate()を行うと、g_string_append()を行う前にrvalが破壊されてしまう問題が生じる。
	//上記の問題を避けるために、g_string_truncate()を行う前にrvalを複製しておき、複製したrvalを使ってg_string_append()を行う。
	//オリジナル版のgstring.cでも、(全く同じではないが)同様の対策が取られている。
	//本来、この問題はg_string_assign()だけでなく、g_array_insert_vals()等でも起こり得るのだが、g_array_insert_vals()では対策されていない。(オリジナル版でも当ライブラリでも)
	//g_array_insert_vals()でそんな使い方をする事は(まず)無いが、g_string_assign()では自己代入が頻繁に起こり得るからだと思う。
	char* tmp = strdup(rval);	//文字列を複製する。
	if(!tmp) { DIE(); }
	g_string_truncate(string, 0);
	g_string_append(string, tmp);	//複製した文字列を使ってg_string_append()を行う。
	free(tmp);			//複製した文字列を開放する。
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
//GString* g_string_append_unichar(GString* string, wchar_t wc);												//未対応
/*--------------------------------------------------------------------------*/
//Appends len bytes of val to string.
//Because len is provided, val may contain embedded nuls and need not be nul-terminated.									⇒当実装は埋め込みヌルバイトに対応していません。
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
//GString* g_string_append_uri_escaped(GString* string, const char* unescaped, const char* reserved_chars_allowed, int allow_utf8);				//未対応
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
//GString* g_string_prepend_unichar(GString* string, wchar_t wc);												//未対応
/*--------------------------------------------------------------------------*/
//Prepends len bytes of val to string.
//Because len is provided, val may contain embedded nuls and need not be nul-terminated.									⇒当実装は埋め込みヌルバイトに対応していません。
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
//GString* g_string_insert_unichar(GString* string, int pos, wchar_t wc);											//未対応
/*--------------------------------------------------------------------------*/
//Inserts len bytes of val into string at pos.
//Because len is provided, val may contain embedded nuls and need not be nul-terminated.									⇒当実装は埋め込みヌルバイトに対応していません。
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
	//もしvalがstring->strと同じ,又は,string->strの途中を指していた場合、g_array_insert_vals()の処理中にvalが破壊されてしまう問題が生じる。
	//上記の問題を避けるために、g_array_insert_vals()を行う前にvalを複製しておき、複製したvalを使ってg_array_insert_vals()を行う。
	//本来、この問題はg_string_insert_len()だけでなく、g_array_insert_vals()自体でも起こり得るのだが、g_array_insert_vals()では対策されていない。(オリジナル版でも当ライブラリでも)
	//g_array_insert_vals()を含むgarray.cはプリミティブな機能を提供するモジュールであり、上記のようなケースの対処はアプリケーション側に任されているのだと思う。
	//一方、gstring.cは文字列操作に特化したモジュールであるから、一般的な使用で起こり易い問題に対しては、モジュール側で対処する方針である。
	char* tmp = strndup(val, len);	//文字列を複製する。	//当実装は埋め込みヌルバイトに対応していません。lenに大きな値(又は,マイナス値)を指定しても、埋め込みヌルバイトが有ればその長さまでと解釈します。
	if(!tmp) { DIE(); }
	len = strlen(tmp);
	if(pos < 0) { pos = string->len; }	//g_array_insert_vals()には(index<0)を指定出来ないのでここで置き換えます。
	g_array_insert_vals(array, pos, tmp, len);
	free(tmp);			//複製した文字列を開放する。
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
	//もしvalがstring->strと同じ,又は,string->strの途中を指していた場合、g_array_set_size(),又は,memcpy()の処理中にvalが破壊されてしまう問題が生じる。
	//上記の問題を避けるために、g_array_set_size(),及び,memcpy()を行う前にvalを複製しておき、複製したvalを使ってmemcpy()を行う。
	//オリジナル版の当関数の実装はこの対策が取られていないようだが、当実装では対策を取っておく事にした。
	char* tmp = strndup(val, len);	//文字列を複製する。
	if(!tmp) { DIE(); }
	len = strlen(tmp);					//当実装は埋め込みヌルバイトに対応していません。lenに大きな値(又は,マイナス値)を指定しても、埋め込みヌルバイトが有ればその長さまでと解釈します。
	if((pos + len) > string->len) {
		g_array_set_size(array, pos + len);
	}
	memcpy(string->str + pos, tmp, len);
	free(tmp);			//複製した文字列を開放する。
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
	if(len < 0) { len = string->len - pos; }	//g_array_remove_range()には(length<0)を指定出来ないのでここで置き換えます。
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
//GBytes* g_string_free_to_bytes(GString* string);														//未対応
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
//GString* g_string_ascii_down(GString* string);														//未対応
//GString* g_string_ascii_up(GString* string);															//未対応
/*--------------------------------------------------------------------------*/
