/*	
 *	clipdie.c
 *
 *	エラー停止処理、トレース出力処理
 *
 *	CLiP - Common Library for P/ECE
 *	Copyright (C) 2001-2016 Naoyuki Sawa
 *
 *	* Tue Aug 16 21:54:11 JST 2016 Naoyuki Sawa
 *	- 1st リリース。
 *	  エラー停止処理とトレース出力処理を、clipmisc.h,clipmisc.cから、clipdie.h,clipdie.cへ分離しました。
 *	* Thu Dec 01 22:25:12 JST 2016 Naoyuki Sawa
 *	- vdie()において、abort()で止めるか__asm int 3で止めるかの選択を、ライブラリのビルド時に静的に選択するのでなく、実行時にデバッガから実行されているかを判断して、動的に選択するように変更しました。
 *	  変更した理由は、以下の通りです。
 *	- デバッガで実行した場合に対しては、abort()よりも__asm int 3の方が良い事には、既に気付いていたのですが、これまでは、デバッガ以外で実行した場合のエラーメッセージ判り易さを優先して、abort()を選択していました。
 *	  デバッガで実行した場合でも、abort()でも呼び出し階層の表示が深くなるだけで、さほど大きな問題ではなかったからです。
 *	  しかし、昨日(2016/11/31)、pcl.cモジュールを実装した際に、ユーザーコンテキストを切り替えてデバッグブレークした場合、abort()ではスタックフレームが表示されず、__asm int 3ならば表示される事に気付きました。(詳細は、pcl.cの'Wed Nov 30 21:38:22 JST 2016'のコメントを参照して下さい。)
 *	  そのため、__asm int 3に変更する必要が生じたのですが、常に__asm int 3に変更してしまうと、デバッガ以外で実行した時に、エラーダイアログが判り辛くなる問題が生じます。
 *	  そこで、実行時にデバッガから実行されているかを判断して、動的に選択するように変更しました。
 *	- 今回変更したきっかけは、上記の通り、ユーザーコンテキストを切り替えてデバッグブレークした場合の対策ですが、ユーザーコンテキストを使用していない場合に対しても、今回の変更は有用であると思います。
 *	  デバッグブレークした時に、呼び出し階層の表示がこれまでのabort()よりも浅くなり、ぴったり__asm int 3の位置で止まるようになるからです。
 */
#include "clip.h"
/*****************************************************************************
 *	エラー停止処理
 *****************************************************************************/
//{{2015/02/08コメント追加━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// * Sun Feb 08 19:33:38 JST 2015 Naoyuki Sawa
// - VC++6.0でコンパイルした時、'PIECE'シンボルを定義していないのに、以下の'#ifdef PIECE'ブロック内でエラーが出る事が有ります。
//   『プリコンパイル済みヘッダーを自動的に使用』の設定になっていると、上記のエラーが出るようです。
//   『プリコンパイル済みヘッダーを使用しない』の設定に変更すると、上記のエラーが出なくなりました。
//   詳細な原因は判らないのですが、ともかく常に『プリコンパイル済みヘッダーを使用しない』の設定にしておく方が安全だと思います。
//}}2015/02/08コメント追加━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
#ifdef  PIECE
/*---------------------------------------------------------------------------*/
void die(const char* fmt, ...) /*__attribute__((noreturn))*/;
asm("
		.code
		.align		1
		.global		die
die:
		;//as33アセンブラは、未定義シンボルをexternと見なすので、
		;//明示的なextern宣言は必要ありません。(_def_vbuffのこと)
;//不要		xcall		pceLCDDispStop			;//pceLCDDispStop(); {{2005/12/13 Naoyuki Sawa 不要なので削除しました}}
		xld.w		%r12, _def_vbuff		;//pceLCDSetBuffer(_def_vbuff);
		xcall		pceLCDSetBuffer
;//不要		xcall		pceLCDDispStart			;//pceLCDDispStart();		//{{2014/12/31削除:cstart.cのpceAppInit00()でpceLCDDispStart()を呼び出し、液晶画面の表示を開始するようにしました。}}
;//{{2014/12/10変更:die()が使用するフォントを切り替えられるようにしました。詳細は、clip.mkの'Wed Dec 10 00:39:11 JST 2014'のコメントを参照してください。
;//{{		xld.w		%r12, 0				;//pceFontSetType(0);
;//↓2014/12/10変更:die()が使用するフォントを切り替えられるようにしました。詳細は、clip.mkの'Wed Dec 10 00:39:11 JST 2014'のコメントを参照してください。
  ;//{{2014/12/23変更:Asm版die()を2命令節約しました。_diefontは0～2なのでxld.wでなくld.wで充分でした。
  ;//		xld.w		%r12, _diefont			;//pceFontSetType(_diefont);
  ;//↓2014/12/23変更:Asm版die()を2命令節約しました。_diefontは0～2なのでxld.wでなくld.wで充分でした。
		 ld.w		%r12, _diefont@l		;//pceFontSetType(_diefont);
  ;//}}2014/12/23変更:Asm版die()を2命令節約しました。_diefontは0～2なのでxld.wでなくld.wで充分でした。
;//}}2014/12/10変更:die()が使用するフォントを切り替えられるようにしました。詳細は、clip.mkの'Wed Dec 10 00:39:11 JST 2014'のコメントを参照してください。
		xcall		pceFontSetType
		xld.w		%r12, 3				;//pceFontSetTxColor(3);
		xcall		pceFontSetTxColor
		xld.w		%r12, 0				;//pceFontSetBkColor(0);
		xcall		pceFontSetBkColor
		xld.w		%r12, 0				;//pceFontSetPos(0, 0);
		xld.w		%r13, 0
		xcall		pceFontSetPos
		xld.w		%r4, die_L10			;//pceFontPrintf(fmt, ...);
		xld.w		[%sp], %r4
		xjp		pceFontPrintf
die_L10:
		xcall		pceLCDTrans			;//pceLCDTrans();
die_L20:							;//for(;;) {
		xcall		yield				;//  yield();
		xcall		pcePadGet			;//  if(pcePadGet() & TRG_SELECT) {
		xand		%r10, %r10, 0x4000		;//    
		xjreq		die_L30				;//    
		xld.w		%r12, 1				;//    pceAppReqExit(1);
		xcall		pceAppReqExit			;//    
die_L30:							;//  }
		xjp		die_L20				;//}
");
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
//{{2016/04/15追加:vdie()を追加しました。
// * Fri Apr 15 21:07:28 JST 2016 Naoyuki Sawa
// - vdie()を追加しました。
// - P/ECE版の実装は、既存のdie()はそのままにして、新しくvdie()を追加しました。
//   P/ECE版の既存のdie()は、P/ECE APIのpceFontPrintf()を使用しているので、die()とvdie()に分割する事は出来ないからです。
// - P/ECE版の実装は、dief()と同様に、_def_vbuff[]の下端80～87ラインに、書式付き文字列を展開します。
//   詳細は、clipflt1.cの、dief()のコメントを参照して下さい。
// - Windows版の実装は、既存のdie()を、die()とvdie()に分割し、die()からvdie()を呼び出すように変更しました。
//   既存のdie()を利用しているアプリケーションの観点からは、die()の動作は変わっていないので、アプリケーションの変更は不要です。
void vdie(const char* fmt, va_list ap) /*__attribute__((noreturn))*/ {
	vsnprintf(&_def_vbuff[DISP_X * (DISP_Y - 8)], DISP_X * 8, fmt, ap);
	die("%s", &_def_vbuff[DISP_X * (DISP_Y - 8)]);
}
//}}2016/04/15追加:vdie()を追加しました。
/*---------------------------------------------------------------------------*/
#else /*PIECE*/
/*---------------------------------------------------------------------------*/
// * __declspec(noreturn)は、.hファイルの関数宣言に付加するだけで良いようです。
//   .cファイルの関数定義には、付加してもしなくても同じみたいです。(実験より)
//   GCCの__attribute__((noreturn))が、関数定義には付加できないことに合わせて、
//   __declspec(noreturn)も、関数定義には付加しないことにしました。
// * GCCは、__attribute__((noreturn))を付加した関数が、誤って処理を返した場合、
//   コンパイル時に警告を表示します。
//   Visual C++は、__declspec(noreturn)を付加した関数が、誤って処理を返しても、
//   コンパイル時に警告を表示しないので、注意が必要です。
// * Fri Apr 15 21:07:28 JST 2016 Naoyuki Sawa
// - vdie()を追加しました。
// - P/ECE版の実装は、既存のdie()はそのままにして、新しくvdie()を追加しました。
//   P/ECE版の既存のdie()は、P/ECE APIのpceFontPrintf()を使用しているので、die()とvdie()に分割する事は出来ないからです。
// - P/ECE版の実装は、dief()と同様に、_def_vbuff[]の下端80～87ラインに、書式付き文字列を展開します。
//   詳細は、clipflt1.cの、dief()のコメントを参照して下さい。
// - Windows版の実装は、既存のdie()を、die()とvdie()に分割し、die()からvdie()を呼び出すように変更しました。
//   既存のdie()を利用しているアプリケーションの観点からは、die()の動作は変わっていないので、アプリケーションの変更は不要です。
// * Fri Apr 15 23:25:13 JST 2016 Naoyuki Sawa
// - Windows版のvdie()の動作を少し変更しました。
//   これまでは、常に、エラーメッセージをコンソールに出力していました。
//   変更後は、標準エラー出力がコンソールならば、これまで通りエラーメッセージをコンソールに出力しますが、
//   標準エラー出力がコンソールでなければ、エラーメッセージをメッセージボックスで表示するようにしました。
// - 標準エラー出力がコンソールでないケースとしては、以下のようなケースが考えられる。
//   ①明示的に標準エラー出力をファイル等へリダイレクトして実行した場合
//   ②VC++のアプリケーションの設定で、ウインドウアプリケーションとしてビルドした場合
//   ③VC++のアプリケーションの設定はコンソールアプリケーションだが、の中で'#pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")'を指定した場合
//   等が考えられる。
//   今回この対応を行った理由は、主に、②③のケースでもデバッグし易いようにするためです。
/*__declspec(noreturn)*/ void die(const char* fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
//{{2016/04/15変更:vdie()を追加しました。
//	vfprintf(stderr, fmt, ap);
//	va_end(ap);
//↓2016/04/15変更:vdie()を追加しました。
	vdie(fmt, ap);
//不要	va_end(ap);
}
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*__declspec(noreturn)*/ void vdie(const char* fmt, va_list ap) {
	//標準エラー出力がコンソールならば、エラーメッセージをコンソールに出力する。
	if(isatty(fileno(stderr))) {
		vfprintf(stderr, fmt, ap);
	//標準エラー出力がコンソールでなければ、エラーメッセージをメッセージボックスで表示する。
	// - 標準エラー出力がコンソールでないケースとしては、以下のようなケースが考えられる。
	//   ①明示的に標準エラー出力をファイル等へリダイレクトして実行した場合
	//   ②VC++のアプリケーションの設定で、ウインドウアプリケーションとしてビルドした場合
	//   ③VC++のアプリケーションの設定はコンソールアプリケーションだが、の中で'#pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")'を指定した場合
	//   等が考えられる。
	//   今回この対応を行った理由は、主に、②③のケースでもデバッグし易いようにするためです。
	} else {
		char* str = strdup_vprintf(fmt, ap);	//エラー処理の中でstrdup_vprintf()を使うのは少し怖いが、大抵は大丈夫だろう。
		MessageBox(NULL, str, program_invocation_short_name, MB_OK|MB_ICONERROR|MB_TASKMODAL);	//MB_SYSTEMMODALではなくMB_TASKMODALが適切です。名前とは裏腹にアプリケーションのスレッドに属する全てのウインドウにフォーカスを移せなくするのはMB_SYSTEMMODALではなくMB_TASKMODALの方です。
	//不要	free(str);	⇒この後すぐに終了するのでメモリ解放は不要です。二重遭難の危険を少しでも減らすためにメモリ解放しない事にしました。
	}
//}}2016/04/15変更:vdie()を追加しました。
    //{{2015/07/14追加:Windows版のdie()の停止前に、fflush(NULL)を追加しました。
	//abort()が出力ストリームをフラッシュするかしないかは環境依存であり、Visual C++ではフラッシュしないようです。
	//従って、エラーログを確実に保存するために、少なくともfflush(stderr)が必要です。
	//エラーログ以外の出力ストリームも確実に保存できるよう、fflush(NULL)とする事にしました。
	fflush(NULL);
    //}}2015/07/14追加:Windows版のdie()の停止前に、fflush(NULL)を追加しました。
//{{2016/12/01変更:vdie()において、abort()で止めるか__asm int 3で止めるかの選択を、ライブラリのビルド時に静的に選択するのでなく、実行時にデバッガから実行されているかを判断して、動的に選択するように変更しました。
//#if 1
//	abort(); /* DebugBreak()よりもエラーダイアログが判り易いです */
//#else
//	//DebugBreak(); /* abort()よりも呼び出し階層が少なくて済みます */
//	//↓* Tue Oct 03 01:05:53 JST 2006 Naoyuki Sawa
//	//↓- 呼び出し階層の少ないデバッグブレークを「DebugBreak()」→「__asm int 3」に変更しました。
//	//↓  2006/10/3現在は、呼び出し階層の少なさよりもエラーダイアログの判り易さを優先して、
//	//↓  「DebugBreak()」でも「__asm int 3」でもなく「abort()」を採用しているのですが、
//	//↓  今後のための覚え書きとして、コメントとして残しておくことにしました。
//	__asm int 3; /* DebugBreak()よりもさらに呼び出し階層が少なくて済みます */
//#endif
//↓2016/12/01変更:vdie()において、abort()で止めるか__asm int 3で止めるかの選択を、ライブラリのビルド時に静的に選択するのでなく、実行時にデバッガから実行されているかを判断して、動的に選択するように変更しました。
	//現在のプロセスがデバッガのコンテキストで実行されていない場合は…
	if(!IsDebuggerPresent()) {
		abort();	//DebugBreak()よりもエラーダイアログが判り易いです。
	//現在のプロセスがデバッガのコンテキストで実行されている場合は…
	} else {
#ifdef  _WIN64
		DebugBreak();	//abort()よりも呼び出し階層が少なくて済みます。			//┐DebugBreak()で止めると、VC++6.0のデバッガが、vdie()よりも一つ上のスタックフレームまでしか解析出来ないようです。
#else //_WIN64											//│__asm int 3 で止めれば、VC++6.0のデバッガが、vdie()のスタックフレームも解析出来るので、__asm int 3 の方が望ましいです。
		__asm int 3;	//DebugBreak()よりもさらに呼び出し階層が少なくて済みます。	//┘しかし、VS2015のx64ターゲットではインラインアセンブラが使えない(ucontext.cの'Tue Nov 29 23:01:43 JST 2016'のコメント参照)ので、x64の場合はDebugBreak()で止める事にしました。
#endif//_WIN64
	}
//}}2016/12/01変更:vdie()において、abort()で止めるか__asm int 3で止めるかの選択を、ライブラリのビルド時に静的に選択するのでなく、実行時にデバッガから実行されているかを判断して、動的に選択するように変更しました。
}
#endif//PIECE
/*---------------------------------------------------------------------------*/
//{{2012/01/14:追加
/*__declspec(noreturn)*/ void die2(const char* file, int line) /*__attribute__((noreturn))*/ {
	die("%s(%d)", file, line);
}
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*__declspec(noreturn)*/ void die3(const char* file, int line, const char* f) /*__attribute__((noreturn))*/ {
	die("%s(%d): %s", file, line, f);
}
//}}2012/01/14:追加
/*****************************************************************************
 *	トレース出力
 *****************************************************************************/
void trace(const char* fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
}
