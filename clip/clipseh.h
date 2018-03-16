/*
 *	clipseh.h
 *
 *	構造化例外処理 (Structured Exception Handling)
 *
 *	CLiP - Common Library for P/ECE
 *	Copyright (C) 2015-2016 Naoyuki Sawa
 *
 *	* Fri Jul 03 01:00:00 JST 2015 Naoyuki Sawa
 *	- 1st リリース。
 *	* Fri Jul 03 21:00:32 JST 2015 Naoyuki Sawa
 *	- clipseh.cに「使用例」を追記しました。
 *	* Sun Oct 11 10:36:38 JST 2015 Naoyuki Sawa
 *	- gccが余計な警告や無駄なコードを生成する事が有る問題を回避しました。
 *	  詳細はclipseh.cの「2015/10/11に行った修正内容についての説明」を参照して下さい。
 *	* Sun Oct 11 13:00:37 JST 2015 Naoyuki Sawa
 *	- 以下二点の機能を追加しました。
 *	  これらの追加機能は、P/ECE開発環境(GCC)でもWindows(VC++6.0)でも使用出来ます。
 *	  1. グローバル変数SEH_infoを追加しました。
 *	     例外処理ブロックの中で、例外を投入したソースコードの位置を参照出来るようになりました。
 *	  2. マクロSEH_DIE()を追加しました。
 *	     例外処理ブロックの中で回復出来ない時に、例外を投入したソースコードの位置を表示して停止出来るようになりました。
 *	* Sun Oct 11 16:11:49 JST 2015 Naoyuki Sawa
 *	- ユーティリティマクロSEH_VERIFY()を追加しました。
 *	* Tue Oct 13 22:12:29 JST 2015 Naoyuki Sawa
 *	- ユーティリティマクロSEH_use_shared,SEH_try_shared,SEH_VERIFY_SHARED()を追加しました。
 *	* Wed Oct 14 21:50:46 JST 2015 Naoyuki Sawa
 *	- SEH_info構造体に、'msg'フィールドを追加しました。
 *	  それに伴い、SEH_throw_msg()を追加しました。
 *	- SEH_die()が表示するファイル名を、これまではフルパスだったのを、ベース名だけに変更しました。
 *	  SEH_throw()で例外を投入するのは自作モジュールのソースファイルだけであり、フルパスにしなくても特定出来るからです。
 *	  フルパスで表示すると長過ぎて見辛いので、ベース名だけを表示する事にしました。
 *	- SEH_die()に、SEH_info.msgの表示を追加しました。
 *	* Wed Nov 18 21:37:53 JST 2015 Naoyuki Sawa
 *	- 'extern "C" {'～'}'で囲みました。.cppを含むWin32プロジェクトからもインクルード出来るようにするためです。
 *	* Sat Jan 16 21:34:44 JST 2016 Naoyuki Sawa
 *	- Win32でデバッガから実行された場合は、最上位の例外フレームを初期化しないようにしました。
 *	  変更前は、明示的な例外フレーム無しで例外が投入された時、最上位の例外フレームまで巻き戻って捕捉していましたが、
 *	  今後は、明示的な例外フレーム無しで例外が投入された時、その場でブレークするようになります。(デバッグ実行時のみ)
 *	  詳細は、SEH_init()のコメントを参照して下さい。
 *	- 今回の変更に伴い、マクロの構成を大幅に変更しました。
 *	  マクロの構成を大幅に変更しましたが、生成されるコードは上記の点以外変わっていないはずです。
 *	  特に、Win32以外のプラットフォームに対して、今回の変更による影響は一切無いはずです。
 *	  もし問題が生じた場合は、前回までのアーカイブのソースコードと比較して下さい。
 */
#ifndef __CLIP_SEH_H__
#define __CLIP_SEH_H__
#ifdef  __cplusplus
extern "C" {
#endif//__cplusplus
/****************************************************************************
 *	グローバル変数
 ****************************************************************************/
//アプリケーションが直接参照しないで下さい。
extern jmp_buf* SEH_root;
//最後に例外を投入したソースコードの位置。
//例外処理ブロックの中でアプリケーションが直接参照出来ます。
extern struct SEH_info { const char* file; int line; char* msg; } SEH_info;
/****************************************************************************
 *	内部処理用関数,内部処理用マクロ
 ****************************************************************************/
//アプリケーションが直接使用しないで下さい。
#if     defined(PIECE)
  //P/ECE
  void SEH_die(const char* file, int line, int code) __attribute__((noreturn));
  int SEH_setjmp(jmp_buf node);
  void SEH_longjmp(int code) __attribute__((noreturn));
#elif   defined(WIN32)
  //Win32
  __declspec(noreturn) void SEH_die(const char* file, int line, int code);
  #define SEH_setjmp(__node__)						\
    setjmp(*(SEH_root = &(__node__)))
  #define SEH_longjmp(__code__)						\
    do { if(!SEH_root) { SEH_die(__FILE__, __LINE__, (__code__)); }	\
         longjmp((*SEH_root), (__code__)); } while(0)
#else //!defined(PIECE) && !defined(WIN32)
  //Generic
  void SEH_die(const char* file, int line, int code);
  #define SEH_setjmp(__node__)						\
    setjmp(*(SEH_root = &(__node__)))
  #define SEH_longjmp(__code__)						\
    longjmp((*SEH_root), (__code__))
#endif//!defined(PIECE) && !defined(WIN32)
/****************************************************************************
 *	アプリケーション用マクロ
 ****************************************************************************/
//例外処理ブロックの中で回復出来ない時に呼び出して下さい。
#define SEH_DIE()							\
  SEH_die(__FILE__, __LINE__, SEH_code)
/*--------------------------------------------------------------------------*/
//捕捉されなかった例外を捕捉してエラーメッセージを表示するための、例外フレームを初期化します。
//必須では有りませんが、当マクロを実行せずに例外が捕捉されなかった場合は、異常動作となります。
//アプリケーションの実行中に破棄されないコンテキスト上で、当マクロを実行してください。
//一般的には、アプリケーションエントリポイント(main関数)の先頭で実行するのが適切です。
// - Win32でデバッガから実行した場合は、例外フレームを初期化せずSEH_rootをNULLのままにします。
//   SEH_throw,又は,SEH_throw_lastで例外が投入された時に、SEH_rootがNULLならば(=捕捉する例外フレームが無ければ)その場でブレークします。
//   そうした理由は、最上位の例外フレームまで巻き戻ってブレークするよりも、例外を投入した箇所でブレークする方がデバッグし易いからです。
// - 上記の特例は、Win32でデバッガから実行した場合のみ適用します。デバッグビルドかリリースビルドかは関係有りません。
//   Win32以外のプラットフォームや、Win32でもexeを直接実行した場合は、これまで通り最上位の例外フレームで捕捉します。
#ifdef  WIN32
  //Win32
  #define SEH_init()							\
    if(!IsDebuggerPresent()) { jmp_buf __SEH_node__;			\
         const int SEH_code = SEH_setjmp(__SEH_node__);			\
         if(SEH_code) { SEH_DIE(); } }
#else //WIN32
  //P/ECE,Generic
  #define SEH_init()							\
    do { jmp_buf __SEH_node__;						\
         const int SEH_code = SEH_setjmp(__SEH_node__);			\
         if(SEH_code) { SEH_DIE(); } } while(0)
#endif//WIN32
/*--------------------------------------------------------------------------*/
//例外ブロックを開始します。
#define SEH_try								\
  { jmp_buf __SEH_node__, * const __SEH_save__ = SEH_root;		\
    const int SEH_code = SEH_setjmp(__SEH_node__);			\
    if(!SEH_code) {
/*--------------------------------------------------------------------------*/
//引数で指定した例外コードのみを捕捉します。
#define SEH_catch(__code__)						\
      SEH_root = __SEH_save__;						\
    } else if(SEH_code == (int)(__code__)) {				\
      SEH_root = __SEH_save__;
/*--------------------------------------------------------------------------*/
//全ての例外コードを捕捉します。
#define SEH_catch_all							\
    SEH_catch(SEH_code)
/*--------------------------------------------------------------------------*/
//例外ブロックを終了します。
#define SEH_end								\
      SEH_root = __SEH_save__;						\
    } else {								\
      SEH_root = __SEH_save__;						\
      SEH_throw_last;							\
  } }
/*--------------------------------------------------------------------------*/
//例外を投入します。
//この時、最後に例外を投入したソースコードの位置(SEH_info)を設定します。
// - SEH_throw()は、メッセージ無しを示すために、SEH_info.msgにNULLを格納します。
//   SEH_throw_msg()は、指定されたメッセージ文字列を、SEH_info.msgに格納します。
// - SEH_info.msgに格納する文字列は、基本的には、定数文字列です。
//   動的に生成した文字列を格納する事も可能ですが、そうする場合、アプリケーションがSEH_info.msgのメモリ管理を行って下さい。
//   Garbage collectorが動作している環境ならば、特に意識する事無く、strdup_printf()等で動的に生成した文字列を格納出来ます。
#define SEH_throw(__code__)						\
  SEH_throw_msg(__code__, NULL)
#define SEH_throw_msg(__code__, __msg__)				\
  do { SEH_info.file = __FILE__;					\
       SEH_info.line = __LINE__;					\
       SEH_info.msg  = (char*)(__msg__);				\
       SEH_longjmp((int)(__code__)); } while(0)
/*--------------------------------------------------------------------------*/
//catchブロックの中で使用し、捕捉した例外コードを上位の例外フレームへ再投入します。
//この時、最後に例外を投入したソースコードの位置(SEH_info)を変更しません。
#define SEH_throw_last							\
  SEH_longjmp(SEH_code)
/*--------------------------------------------------------------------------*/
//tryブロックの中から関数を抜ける場合は、returnの代りにSEH_returnを使ってください。
//関数の戻り値も指定出来ます。
#define SEH_return							\
  SEH_root = __SEH_save__; return
/****************************************************************************
 *	ユーティリティマクロ
 ****************************************************************************/
//一つの関数の中でSEH_tryをたくさん使用する場合は、SEH_tryの代りに下記のマクロを使用するとスタックを節約出来ます。
// - 一つの関数の中でSEH_tryを複数回使用すると、P/ECE開発環境のGCCは各ブロックの__SEH_node__のメモリを個別に確保してしまうようです。
//   SEH_tryを数回使用する程度ならば特に問題は有りませんが、数十回使用する場合はスタック領域がたくさん確保されて、スタックがあふれるおそれがあります。
//   △
//   │SEH_try {		←①	┐
//   │    ～				│
//   │} SEH_end			│①のブロックを抜けたら①のブロックで定義した__SEH_node__はもう不要である。
//   │SEH_try {		←②	│②のブロックを抜けたら②のブロックで定義した__SEH_node__はもう不要である。
//   │    ～				│従って、①と②と③の__SEH_node__はスタック上で同じ領域に確保出来るのだが、コンパイラはそれがわからずに個別に確保してしまう。
//   │} SEH_end			│
//   │SEH_try {		←③	┘
//   │    ～
//   │} SEH_end
//   以下のようなコードが生成されます。
//   △
//   │pushn %r0
//   │xsub  %sp,%sp,72		←①②③が個別に確保されてしまっている。無駄。本当は'xsub %sp,%sp,24'で良い。
//   │    ～
//   │ld.w  %r12,%sp		←①
//   │xcall SEH_setjmp
//   │    ～
//   │xadd  %r12,%sp,24	←②					無駄。本当は'ld.w %r12,%sp'で良い。
//   │xcall SEH_setjmp
//   │    ～
//   │xadd  %r12,%sp,48	←③					無駄。本当は'ld.w %r12,%sp'で良い。
//   │xcall SEH_setjmp
//   │    ～
//   │xadd  %sp,%sp,72
//   │popn  %r0
//   │ret
//   NOTE:P/ECE開発環境のGCC(ver2.7.2)と、VC++6.0では、上記のとおり無駄なスタック領域を確保するコードが生成されました。最大限の最適化を指定しても同じ結果でした。
//        最新のx86用GCC(ver4.8.1)ならば、最適化を指定しなくても、①②③で同じスタック領域を使用するコードが生成されました。最新のGCCでは改善されているようです。
// - 上記の問題を回避するために、関数の先頭で__SEH_node_shared__を定義しておき、関数内ではそれを再利用するマクロを用意しました。
//   以下のように使用して下さい。
//   ○
//   │SEH_use_shared;
//   │SEH_try_shared {
//   │    ～
//   │} SEH_end
//   │SEH_try_shared {
//   │    ～
//   │} SEH_end
//   │SEH_try_shared {
//   │    ～
//   │} SEH_end
//   以下のようなコードが生成されます。
//   ○
//   │pushn %r0
//   │xsub  %sp,%sp,24
//   │    ～
//   │ld.w  %r12,%sp
//   │xcall SEH_setjmp
//   │    ～
//   │ld.w  %r12,%sp
//   │xcall SEH_setjmp
//   │    ～
//   │ld.w  %r12,%sp
//   │xcall SEH_setjmp
//   │    ～
//   │xadd  %sp,%sp,24
//   │popn  %r0
//   │ret
// - !!注意点!!
//   例外処理ブロックがネストしている場合は、このユーティリティマクロを使用してはいけません。
//   以下の使用方法は誤りです。
//   ×
//   │SEH_use_shared;
//   │SEH_try_shared {		←①
//   │    ～
//   │  SEH_try_shared {	←誤り。__SEH_node_shared__は①のブロックがまだ使用中なのに、ここで再利用してしまっている。
//   │      ～
//   │  } SEH_end
//   │    ～
//   │} SEH_end
#define SEH_use_shared							\
  jmp_buf __SEH_node_shared__
#define SEH_try_shared							\
  { jmp_buf* const __SEH_save__ = SEH_root;				\
    const int SEH_code = SEH_setjmp(__SEH_node_shared__);		\
    if(!SEH_code) {
/*--------------------------------------------------------------------------*/
//'action'の中で任意の例外が投入されたら、例外が投入された位置と、呼び出し側(このマクロを記述した位置)を表示して、エラー停止します。
//このユーティリティマクロの使用は必須では有りません。アプリケーションが明示的に例外を捕捉してエラー停止する事と同じ結果になります。
// - <使用例>
//   △
//   │SEH_VERIFY(dVal=PrpHdr_fget_l(phApp,PrpVar_x,-1));
//   │SEH_VERIFY(PrpHdr_fset_l(phApp,1.23,PrpVar_y,-1);
//   │           PrpHdr_fset_l(phApp,4.56,PrpVar_z,-1));
// - !!注意点!!
//   一つの関数内でSEH_VERIFY()をたくさん使用すると、スタックがあふれるおそれがあります。
//   このマクロの利用目的から考えて、一つの関数内でSEH_VERIFY()をたくさん使用する事は有り得ると思うので、注意してください。
//   スタックがあふれないようにするには、SEH_VERIFY()の代りにSEH_VERIFY_SHARED()を使用して下さい。
#define SEH_VERIFY(action)						\
  SEH_try {								\
    action;								\
  } SEH_catch_all {							\
    SEH_DIE();								\
  } SEH_end
//SEH_VERIFY()のスタック節約版です。
//詳細は、SEH_use_shared,SEH_try_sharedの説明を参照して下さい。
// - <使用例>
//  ○
//  │SEH_use_shared;
//  │SEH_VERIFY_SHARED(dVal=PrpHdr_fget_l(phApp,PrpVar_x,-1));
//  │SEH_VERIFY_SHARED(dVal=PrpHdr_fget_l(phApp,PrpVar_x,-1));
//  │SEH_VERIFY_SHARED(PrpHdr_fset_l(phApp,1.23,PrpVar_y,-1);
//  │                  PrpHdr_fset_l(phApp,4.56,PrpVar_z,-1));
#define SEH_VERIFY_SHARED(action)					\
  SEH_try_shared {							\
    action;								\
  } SEH_catch_all {							\
    SEH_DIE();								\
  } SEH_end
#ifdef  __cplusplus
}//extern "C"
#endif//__cplusplus
#endif//__CLIP_SEH_H__
