/*
 *	gstring.h
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
#ifndef __G_STRING_H__
#define __G_STRING_H__
#ifdef	__cplusplus
extern "C" {
#endif//__cplusplus
/*****************************************************************************
 *	構造体
 *****************************************************************************/
typedef struct _GString {
	char*		str;			//Points to the character data. It may move as text is added. The str field is null-terminated and so can be used as an ordinary C string.	⇔ GRealArray.data
	int		len;			//Contains the length of the string, not including the terminating nul byte.									⇔ GRealArray.len
/*!!*/	int		allocated_len;		//The number of bytes that can be stored in the string before it needs to be reallocated. May be larger than len.				⇔ GRealArray.alloc_bytes
} GString;
// * Wed May 25 10:22:02 JST 2016 Naoyuki Sawa
// - GString.allocated_lenフィールドについて、注意書きを追記しました。
// - allocated_lenは実質プライベートフィールドであり、アプリケーションが直接参照してはいけません。
//   'str[len]～str[allocated_len-1]の範囲に、アプリケーションが直接、追加の文字を格納して良い'という意味 *ではない* 事に注意して下さい。
//   2016/05/25現在、GStringの実装はGArrayを'zero_terminated=1'の設定で使用しており、上記の範囲にアプリケーションが直接、追加の文字を格納すると、GArrayが0だと想定している箇所を破壊してしまう可能性が有ります。
// - 'アプリケーションが直接、追加の文字を格納して良い領域を確保'するには、g_string_set_size()を呼び出す必要が有り、そのサイズはallocated_lenではなくlenに反映されます。
//   例として、MFCのCString::GetBuffer()/CString::ReleaseBuffer()と同様の処理を行う場合について、allocated_lenに依存した間違った方法と、allocated_lenに依存しない正しい方法を示します。
//   □allocated_lenに依存した間違った方法
//   │GString* s = g_string_sized_new(100);	//len=0,allocated_len>=100																						┐
//   │strcpy(s->str, "foo");			//s->str[1],s->str[2]が0以外になる事はGArrayが想定しておらず、以後の処理でヌル終端が必要になった時にその位置に改めて0を格納するとは限らない。偶然動作する可能性も有るが、動作しない可能性も有る。	├誤り。だいたい、allocated_lenを明示的に設定する方法がg_string_sized_new()しか無い事から考えても、allocated_lenに依存するのは誤りである事は容易に想像が付くだろう。(作成済みの文字列に対してサイズを指定してGetBuffer()出来ないという事になってしまうから)
//   │s = g_string_assign(s, s->str);		//len=3,allocated_len>=100																						┘
//   □allocated_lenに依存しない正しい方法
//   │GString* s = g_string_sized_new(NULL);	//len=0,allocated_len=?
//   │g_string_set_size(s, 100);		//len=100,allocated_len>=100	CString::GetBuffer(100)に相当する。	┐
//   │strcpy(s->str, "foo");			//									├正しい。
//   │s = g_string_assign(s, s->str);		//len=3,allocated_len>=100	CString::ReleaseBuffer(-1)に相当する。	┘
// - 上記の理由により、allocated_lenをアプリケーションが直接参照してはいけません。
//   それにもかかわらず、allocated_lenを、アプリケーションから見えるGString構造体内に定義した訳は、オリジナル版がそうだからという理由だけです。
//   当実装では、GArrayを使ってGStringを実装していますが、オリジナル版はGString単独で実装されているので、内部だけで使用するallocated_lenもGString内に定義されているのだと思います。
//   GArrayのように、公開する構造体と非公開の構造体に分ける方法も有りますが、GStringではプライベートフィールドがallocated_lenだけなので、分けられていないのだと思います。
//   当実装では、GString構造体内にallocated_lenを定義せずに、プライベートフィールドとしても良かったのですが、一応、オリジナル版に倣って公開しておく事にしました。
//   GArrayのプライベートフィールド(GRealArray.alloc_bytes)を公開している形となります。
//   実際にアプリケーションから参照する事は、まず、無いと思います。
//   それどころか、gstring.cモジュール内の処理からも参照していません。
//   当実装の実際の処理は、ほとんど全てGArrayの関数を呼び出すだけであり、GString.allocated_len(=GRealArray.alloc_bytes)はGArrayの関数内でしか参照しないからです。
/*****************************************************************************
 *	グローバル関数
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
//GString* g_string_append_unichar(GString* string, wchar_t wc);		//未対応
GString* g_string_append_len(GString* string, const char* val, int len);
//GString* g_string_append_uri_escaped(GString* string, const char* unescaped, const char* reserved_chars_allowed, int allow_utf8);	//未対応
GString* g_string_prepend(GString* string, const char* val);
GString* g_string_prepend_c(GString* string, char c);
//GString* g_string_prepend_unichar(GString* string, wchar_t wc);		//未対応
GString* g_string_prepend_len(GString* string, const char* val, int len);
GString* g_string_insert(GString* string, int pos, const char* val);
GString* g_string_insert_c(GString* string, int pos, char c);
//GString* g_string_insert_unichar(GString* string, int pos, wchar_t wc);	//未対応
GString* g_string_insert_len(GString* string, int pos, const char* val, int len);
GString* g_string_overwrite(GString* string, int pos, const char* val);
GString* g_string_overwrite_len(GString* string, int pos, const char* val, int len);
GString* g_string_erase(GString* string, int pos, int len);
GString* g_string_truncate(GString* string, int len);
GString* g_string_set_size(GString* string, int len);
char* g_string_free(GString* string, int free_segment);
//GBytes* g_string_free_to_bytes(GString* string);				//未対応
int g_string_hash(const GString* str);
int g_string_equal(const GString* v, const GString* v2);
//GString* g_string_ascii_down(GString* string);				//未対応
//GString* g_string_ascii_up(GString* string);					//未対応
#ifdef	__cplusplus
}//extern "C"
#endif//__cplusplus
#endif//__G_STRING_H__
