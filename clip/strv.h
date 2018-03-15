/*
 *	strv.h
 *
 *	strv形式のユーティリティ関数
 *
 *	* Wed Dec 16 00:31:23 JST 2015 Naoyuki Sawa
 *	- 1st リリース。
 *	- 下記のリポジトリに含まれている、strv形式のユーティリティ関数に互換な関数を実装しました。
 *	  ⇒https://github.com/haraldh/dracut/blob/master/install/strv.c
 *	  関数仕様は上記のオリジナル版と同じで、実装は独自です。
 *	- strv形式のユーティリティ関数は、既存のclipstr.c,clipmisc.c等の中にもいくつか存在します。
 *	  既存の関数と同じ機能である関数は、マクロとして実装する事にしました。
 *	* Tue May 24 21:23:01 JST 2016 Naoyuki Sawa
 *	- 全関数をアセンブラ化しました。
 *	  コードサイズが484バイト⇒408バイトに減り、76バイト小さくなりました。
 *	- 2016/05/21以前は、アプリケーションが明示的に使用しない限りstrv.oはリンクされなかったので、strv.oを最適化していなかったのですが、
 *	  2016/05/21以降は、clipstr.cのstrsplit(),strsplit_set()等がstrv.oを利用するようになり、strv.oが常にリンクされるようになりました。
 *	  従って、今回行ったstrv.oのコードサイズ低減は、意義のある最適化だと思います。
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
//既存の関数と同じ機能である関数は、マクロとして実装する事にしました。
#define strv_free(v)		strfreev(v)			//⇒clipstr.c
#define strv_copy(v)		strdupv(v)			//⇒clipstr.c
//#define strv_length(v)	同じ名前			//⇒clipstr.c
#define strv_split(s,delimiter)	strsplit((s),(delimiter),0)	//⇒clipstr.c
#define strv_join(v,separator)	strjoinv((separator),(v))	//⇒clipstr.c		引数順序が逆である事に注意せよ。
#define strv_split_quoted(s)	parse_cmdline(s)		//⇒clipmisc.c		厳密には違うかも知れないが大体同じ動作だろう。
//-----------------------------------------------------------------------------
//オリジナル版に存在する以下の関数は、当実装には含めない事にしました。
//char* strv_find_prefix(char** v, const char* s);	//⇒使い道が無いと思うので実装しない事にした。
//char** strv_remove_prefix(char** v, const char* s);	//⇒使い道が無いと思うので実装しない事にした。
//char** strv_parse_nulstr(const char* s, size_t len);	//⇒使い道が無いと思うので実装しない事にした。これに近い処理はargz_extract()で行える。
//char** strv_split_nulstr(const char* s);		//⇒使い道が無いと思うので実装しない事にした。これに近い処理はargz_extract()で行える。
//char** strv_merge_concat(char** a, char** b, const char* suffix);	//Like strv_merge(), but appends suffix to all strings in b, before adding.	⇒使い道が無いと思うので実装しない事にした。
//char** strv_split_newlines(const char* s);	//Special version of strv_split() that splits on newlines and suppresses an empty string at the end.	⇒使い道が無いと思うので実装しない事にした。
#ifdef  __cplusplus
}//extern "C"
#endif//__cplusplus
#endif//__STRV_H__
