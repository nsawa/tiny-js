/*	
 *	clipdie.h
 *
 *	エラー停止処理、トレース出力処理
 *
 *	CLiP - Common Library for P/ECE
 *	Copyright (C) 2001-2016 Naoyuki Sawa
 *
 *	* Tue Aug 16 21:54:11 JST 2016 Naoyuki Sawa
 *	- 1st リリース。
 *	  エラー停止処理とトレース出力処理を、clipmisc.h,clipmisc.cから、clipdie.h,clipdie.cへ分離しました。
 */
#ifndef __CLIP_DIE_H__
#define __CLIP_DIE_H__
#ifdef  __cplusplus
extern "C" {
#endif//__cplusplus
/*****************************************************************************
 *	エラー停止処理
 *****************************************************************************/
//エラーメッセージを表示し、プログラムを強制終了します。
//[in]
//	fmt,...		printf()の引数と同様です。
//[note]
//	- この関数は処理を返しません。
#ifdef  PIECE
void die(const char* fmt, ...) __attribute__((noreturn));
//{{2016/04/15追加:vdie()を追加しました。
void vdie(const char* fmt, va_list ap) __attribute__((noreturn));
//}}2016/04/15追加:vdie()を追加しました。
//{{2012/01/14:追加
void die2(const char* file, int line) __attribute__((noreturn));
void die3(const char* file, int line, const char* f) __attribute__((noreturn));
//}}2012/01/14:追加
#else //PIECE
__declspec(noreturn) void die(const char* fmt, ...);
//{{2016/04/15追加:vdie()を追加しました。
__declspec(noreturn) void vdie(const char* fmt, va_list ap);
//}}2016/04/15追加:vdie()を追加しました。
//{{2012/01/14:追加
__declspec(noreturn) void die2(const char* file, int line);
__declspec(noreturn) void die3(const char* file, int line, const char* f);
//}}2012/01/14:追加
#endif//PIECE
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
//{{2012/01/14:変更
//#define DIE()		die(__FILE__ "(%d)", __LINE__)
//↓2012/01/14:変更
#define DIE()		die2(__FILE__, __LINE__)
//}}2012/01/14:変更
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
// * Sat Aug 24 00:14:18 JST 2013 Naoyuki Sawa
// - GCC拡張機能「__FUNCTION__」と「__PRETTY_FUNCTION__」を利用して、関数名と行番号を表示してエラー停止するマクロを作成しました。
//   C言語においては、「__FUNCTION__」と「__PRETTY_FUNCTION__」は同一なので、下記「DIEFN()」と「DIEPF()」は同じ結果が得られます。
// - 作成しては見たものの、多分、実際にこれらを利用することは無いと思います。なぜならば、既存の「DIE()」に比べて、「DIEFN()」と「DIEPF()」は以下の欠点があるからです：
//   ・DIE()よりも、却ってエラー箇所を特定しづらくなる場合があります。複数のファイルに同じ名前のローカル関数があった場合に、同じエラーメッセージが生成される可能性があるからです。
//   ・DIE()よりも、コードサイズが大きくなります。同一ファイル内で複数回使用した時に、DIE()ならばファイル名を共有できますが、DIEFN()は個々に関数名を展開するからです。
//   ・VisualC++ 6.0では使用できません。新しいVisualC++ならばC99標準の__func__を使って作成できそうですが、自分はVisualC++ 6.0使いなので考慮していません。
// - そんなわけで、DIEFN()とDIEPF()を実際に利用することは無いと思います。
//   今回、DIEFN()とDIEPF()を作成した理由は、P/ECE開発環境のgcc33でも、__FUNCTION__や__PRETTY_FUNCTION__が利用できることの確認と、備忘録代わりです。
#ifdef  __GNUC__
#define DIEFN()		die2(__FUNCTION__, __LINE__)
#define DIEPF()		die2(__PRETTY_FUNCTION__, __LINE__)
#endif//__GNUC__
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef  DEBUG
//{{2012/01/14:変更
//#define ASSERT(f)	((void)((f) || (die(__FILE__ "(%d): %s", __LINE__, #f), 0)))
//↓2012/01/14:変更
#define ASSERT(f)	((void)((f) || (die3(__FILE__, __LINE__, #f), 0)))
//}}2012/01/14:変更
#define VERIFY(f)	ASSERT(f)
#else //DEBUG
#define ASSERT(f)	((void)0)
#define VERIFY(f)	((void)(f))
#endif//DEBUG
/*****************************************************************************
 *	トレース出力処理
 *****************************************************************************/
//標準エラー出力へのfprintfです。
//[in]
//	fmt,...		printf()の引数と同様です。
//[note]
//	- TRACEマクロ経由で利用します。
void trace(const char* fmt, ...);
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
// !!要注意!! TRACE()マクロはアプリケーション専用です。CLiPライブラリの中で使わないように!!
//            (TRACE()マクロはTRACE_ONシンボルの定義によって振る舞いが変わるので、
//             あらかじめコンパイルされるライブラリの中ではTRACE_ONによる振る舞いの切り替えが効かないから)
// - トレース出力を利用する場合は、  clip.hをインクルードする前に「#define TRACE_ON 1」を定義してください。
//   トレース出力を利用しない場合は、clip.hをインクルードする前に「#define TRACE_ON 0」を定義するか、
//   またはTRACE_ONシンボルを定義しないでください。
// - トレース出力を利用しない場合は、トレース文字列はCコンパイラの最適化によって削除されます。
//   TRACE()呼び出しを削除しなくても、トレース文字列がメモリを圧迫することはありません。
#if     (defined(TRACE_ON) && (TRACE_ON))
#define TRACE		trace
#else //(defined(TRACE_ON) && (TRACE_ON))
#define TRACE		1 ? (void)0 : trace
#endif//(defined(TRACE_ON) && (TRACE_ON))
/*---------------------------------------------------------------------------*/
#ifdef  __cplusplus
}//extern "C"
#endif//__cplusplus
#endif//__CLIP_DIE_H__
