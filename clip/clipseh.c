/*
 *	clipseh.c
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
#include "clip.h"
/****************************************************************************
 *	グローバル変数
 ****************************************************************************/
jmp_buf* SEH_root;		//アプリケーションが直接参照しないで下さい。
struct SEH_info SEH_info;	//最後に例外を投入したソースコードの位置。例外処理ブロックの中でアプリケーションが直接参照出来ます。
/****************************************************************************
 *	内部処理用関数
 ****************************************************************************/
/*__declspec(noreturn)*/ void SEH_die(const char* file, int line, int code) /*__attribute__((noreturn))*/ {	//アプリケーションが直接呼び出さないで下さい。
	die("%s(%d): Uncaught exception 0x%X from %s(%d) '%s'",
		basename((char*)file), line, code,
		basename((char*)SEH_info.file), SEH_info.line,
		SEH_info.msg ? SEH_info.msg : "");
}
/*--------------------------------------------------------------------------*/
#ifdef  PIECE
//SEH_setjmp()とSEH_longjmp()をC言語で実装してはいけません。
//呼び出し階層が増えるとsetjmp()とlongjmp()が意図通りに動作しないからです。アセンブラで実装する必要が有るため2015/10/11の対策はP/ECE専用です。
//もっとも、VC++6.0では後述の余計な警告は出ないし多少無駄なコードが増えても構わないので、VC++6.0では対策前のマクロをそのまま使う事にしました。
int SEH_setjmp(jmp_buf node);				//アプリケーションが直接呼び出さないで下さい。
void SEH_longjmp(int code) __attribute__((noreturn));	//アプリケーションが直接呼び出さないで下さい。
asm("
		.code
		.align		1
		.global		SEH_setjmp
		.global		SEH_longjmp
SEH_setjmp:
		;//%r12 := node
		xld.w		[SEH_root], %r12	;//               (SEH_root = &node)
		xjp		setjmp			;//return setjmp(*(SEH_root = &node))
SEH_longjmp:
		;//%r12 := code
		ld.w		%r13, %r12		;//%r13 :=                     code
		xld.w		%r12, [SEH_root]	;//%r12 :=        (*SEH_root)
		xjp		longjmp			;//return longjmp((*SEH_root), code)
");
#endif//PIECE
/****************************************************************************
 *	使用例									{{2015/10/11コメント追加:2015/10/11の修正により展開結果が少し変わりましたが、下記の「使用例」の展開結果はまだ書き換えていません。従って下記の説明は現在の展開結果と少し違っています。処理の流れは変わっていないので、「使用例」としてはこのまま参照して問題ありません。}}
 ****************************************************************************/
#if 0
/*--------------------------------------------------------------------------*/
//最小限の書き方の例。
//最小限過ぎてあまり意味がありません。(例外ブロックを書かないのと同じだから。)
static void Sample1() {
	SEH_init();
	printf("<1>\n");
	SEH_try {
		printf("<2>\n");
	} SEH_end
	printf("<3>\n");
}
//↓↓↓展開結果↓↓↓
static void Sample1() {
	{ jmp_buf __SEH_node__;
	  const int SEH_code = setjmp(*(SEH_root = &__SEH_node__));
	  if(SEH_code) { die("Uncaught exception: 0x%08X", SEH_code); } }
	printf("<1>\n");
	{ jmp_buf __SEH_node__, * const __SEH_save__ = SEH_root;
	  const int SEH_code = setjmp(*(SEH_root = &__SEH_node__));
	  if(!SEH_code) {
		printf("<2>\n");
	    SEH_root = __SEH_save__;				←←←例外が発生しなかった場合に例外フレームを巻き戻す処理
	  } else {
	    SEH_root = __SEH_save__;				←←←例外が捕捉されなかった場合に例外フレームを巻き戻す処理
	    SEH_throw(SEH_code);				←←←捕捉されなかった例外を上位の例外フレームへ再投入する処理
	} }
	printf("<3>\n");
}
/*--------------------------------------------------------------------------*/
//基本的な例。
const char AppException1[1];
const char AppException2[1];
static void Sample2() {
	SEH_init();
	printf("<1>\n");
	SEH_try {
		printf("<2>\n");
		SEH_throw(AppException2);			←←←例外コードには、'0'以外の任意の値を指定できます。アプリケーション全体で一意とするために、変数アドレスを例外コードとするのが良いです。
	} SEH_catch(AppException1) {
		printf("<3>\n");
	} SEH_catch(AppException2) {
		printf("<4>\n");
	} SEH_catch_all {
		printf("<5> 0x%08X\n", SEH_code);		←←←例外処理ブロックの中では、発生した例外コードを'SEH_code'で参照出来る。
	} SEH_end
	printf("<6>\n");
}
//↓↓↓展開結果↓↓↓
static void Sample2() {
	{ jmp_buf __SEH_node__;
	  const int SEH_code = setjmp(*(SEH_root = &__SEH_node__));
	  if(SEH_code) { die("Uncaught exception: 0x%08X", SEH_code); } }
	printf("<1>\n");
	{ jmp_buf __SEH_node__, * const __SEH_save__ = SEH_root;
	  const int SEH_code = setjmp(*(SEH_root = &__SEH_node__));
	  if(!SEH_code) {
		printf("<2>\n");
		longjmp((*SEH_root), (int)(AppException2));
	  SEH_root = __SEH_save__;				←←←例外が発生しなかった場合に例外フレームを巻き戻す処理
	} else if(SEH_code == (int)(AppException1)) {
	  SEH_root = __SEH_save__;				←←←例外を捕捉した場合に例外フレームを巻き戻す処理
		printf("<3>\n");
	  SEH_root = __SEH_save__;				←←←無駄処理となるが動作上問題は無い。
	} else if(SEH_code == (int)(AppException2)) {
	  SEH_root = __SEH_save__;				←←←例外を捕捉した場合に例外フレームを巻き戻す処理
		printf("<4>\n");
	  SEH_root = __SEH_save__;				←←←無駄処理となるが動作上問題は無い。
	} else if(SEH_code == (int)(SEH_code)) {
	  SEH_root = __SEH_save__;				←←←例外を捕捉した場合に例外フレームを巻き戻す処理
		printf("<5> 0x%08X\n", SEH_code);
	    SEH_root = __SEH_save__;				←←←無駄処理となるが動作上問題は無い。
	  } else {
	    SEH_root = __SEH_save__;				←←←例外が捕捉されなかった場合に例外フレームを巻き戻す処理
	    longjmp((*SEH_root), (int)(SEH_code));		←←←捕捉されなかった例外を上位の例外フレームへ再投入する処理
	} }
	printf("<6>\n");
}
/*--------------------------------------------------------------------------*/
//例外処理ブロックの中からreturnで抜ける例。
const char AppException1[1];
const char AppException2[1];
int Sample3() {
	SEH_init();
	printf("<1>\n");
	SEH_try {
		printf("<2>\n");
		if(now & 1) { SEH_return 1; }			←←←tryブロックの中から「return」で例外ブロックを抜けてはいけない。代りに「SEH_return」を使って下さい。「SEH_return;」「SEH_return 戻り値;」のどちらにも使用可能です。
								　　　「SEH_return」をifブロックに入れる場合は'{～}'で囲って下さい。「if(now & 1) SEH_return 1;」と書いてはいけません。
								　　　tryブロックの中から「goto」「break」「continue」等で例外ブロックを抜ける事も禁止です。これらは必要なケースが少ないと思ったので、代替手段は用意しない事にしました。
	} SEH_catch(AppException1) {
		printf("<3>\n");
		return 2;					←←←catchブロックの中では、「return」でも「SEH_return」でも構いません。
	} SEH_catch(AppException2) {
		printf("<4>\n");
		SEH_return 3;					←←←catchブロックの中では、「return」でも「SEH_return」でも構いません。
	} SEH_end
	printf("<6>\n");
	return 0;
}
//↓↓↓展開結果↓↓↓
static void Sample1() {
	{ jmp_buf __SEH_node__;
	  const int SEH_code = setjmp(*(SEH_root = &__SEH_node__));
	  if(SEH_code) { die("Uncaught exception: 0x%08X", SEH_code); } }
	printf("<1>\n");
	{ jmp_buf __SEH_node__, * const __SEH_save__ = SEH_root;
	  const int SEH_code = setjmp(*(SEH_root = &__SEH_node__));
	  if(!SEH_code) {
		printf("<2>\n");
		if(now & 1) { SEH_root = __SEH_save__; return 1; }
	  SEH_root = __SEH_save__;
	} else if(SEH_code == (int)(AppException1)) {
	  SEH_root = __SEH_save__;
		printf("<3>\n");
		return 2;
	  SEH_root = __SEH_save__;
	} else if(SEH_code == (int)(AppException2)) {
	  SEH_root = __SEH_save__;
		printf("<4>\n");
		SEH_root = __SEH_save__; return 3;		←←←「SEH_root = __SEH_save__;」は無駄処理となるが動作上問題は無い。
	    SEH_root = __SEH_save__;
	  } else {
	    SEH_root = __SEH_save__;
	    SEH_throw(SEH_code);
	} }
	printf("<5>\n");
	return 0;
}
/*--------------------------------------------------------------------------*/
//catchブロックの中から例外を再投入する例。
const char AppException1[1];
const char AppException2[1];
static void Sample2() {
	SEH_init();
	printf("<1>\n");
	SEH_try {
		printf("<2>\n");
		SEH_throw(AppException1);
	} SEH_catch(AppException1) {
		printf("<3>\n");
		SEH_throw(AppException2);			←←←catchブロックの中から、上位の例外フレームへ別の例外を投入出来ます。
	} SEH_catch(AppException2) {
		printf("<4>\n");
		SEH_throw_last;					←←←catchブロックの中から、上位の例外フレームへ同じ例外を投入出来ます。
	} SEH_end
	printf("<6>\n");
}
//↓↓↓展開結果↓↓↓
static void Sample2() {
	{ jmp_buf __SEH_node__;
	  const int SEH_code = setjmp(*(SEH_root = &__SEH_node__));
	  if(SEH_code) { die("Uncaught exception: 0x%08X", SEH_code); } }
	printf("<1>\n");
	{ jmp_buf __SEH_node__, * const __SEH_save__ = SEH_root;
	  const int SEH_code = setjmp(*(SEH_root = &__SEH_node__));
	  if(!SEH_code) {
		printf("<2>\n");
		longjmp((*SEH_root), (int)(AppException1));
	  SEH_root = __SEH_save__;
	} else if(SEH_code == (int)(AppException1)) {
	  SEH_root = __SEH_save__;
		printf("<3>\n");
		longjmp((*SEH_root), (int)(AppException2));	←←←catchブロックの中から、上位の例外フレームへ別の例外を投入する処理
	  SEH_root = __SEH_save__;
	} else if(SEH_code == (int)(AppException2)) {
	  SEH_root = __SEH_save__;
		printf("<4>\n");
		longjmp((*SEH_root), (int)(SEH_code));		←←←catchブロックの中から、上位の例外フレームへ同じ例外を投入する処理
	    SEH_root = __SEH_save__;
	  } else {
	    SEH_root = __SEH_save__;
	    longjmp((*SEH_root), (int)(SEH_code));
	} }
	printf("<5>\n");
}
/*--------------------------------------------------------------------------*/
#endif
/****************************************************************************
 *	2015/10/11に行った修正内容についての説明
 ****************************************************************************/
#if 0
/*--------------------------------------------------------------------------*/
以下の対策はP/ECE専用です。VC++6.0では対策前のマクロをそのまま使う事にしました。その理由はSEH_setjmp()とSEH_longjmp()の所のコメントを参照して下さい。

①--- 問題の根源 ---

setjmp(),longjmp()で、グローバル変数のjmp_bufを使った場合に、以下のような問題が生じる事が有ります:

│	jmp_buf buf;
│	int test(int x) {	//warning: argument `x' might be clobbered by `longjmp' or `vfork'	←←←意図どおりであるにもかかわらず、余計な警告が出ます。	■問題点■
│		if(setjmp(buf)) { x++; }
│		return x;
│	}

↓以下のようなコードが生成されます。

│	test:
│			xsub		%sp, %sp, 4		;//　　　┌gccがsetjmp()を認識して、setjmp()で戻って来た場合のために、xをレジスタに保持せずスタック上に格納しています。
│			xld.w		[%sp], %r12		;//←←←┴確かにこの場合は適切なのですが、setjmp()で戻って来ない事が判っている場合は、無駄なコードとなってしまいます。	■問題点■
│			xld.w		%r12, buf
│			xcall		setjmp
│			cmp		%r10, 0
│			jreq		test_L10
│			xld.w		%r11, [%sp]
│			add		%r11, 1
│			xld.w		[%sp], %r11
│	test_L10:
│			xld.w		%r10, [%sp]
│			xadd		%sp, %sp, 4
│			ret

上記の問題の原因は、gccがsetjmp()を認識して、安全なコードを生成しようとしているからです。
それは良いのですが、setjmp()で戻って来ない事が判っている場合には、余計な警告や無駄なコードとなってしまいます。

②--- 問題が構造化例外処理に与える影響 ---

たとえば、当モジュール(構造化例外処理)においては、setjmp()で戻って来て同じコードを二度通る事は無い事が判っています。
しかし、gccにはそれが判らないので、余計な警告や無駄なコードが生成されてしまいます。

│	int test(int x) {	//warning: argument `x' might be clobbered by `longjmp' or `vfork'	←←←意図どおりであるにもかかわらず、余計な警告が出ます。	■問題点■
│		SEH_try {
│			x++;
│		} SEH_catch_all {
│		} SEH_end
│		return x;
│	}

↓以下のようなコードが生成されます。

│	test:
│			xsub		%sp, %sp, 32		;//　　　┌gccがsetjmp()を認識して、setjmp()で戻って来た場合のために、xをレジスタに保持せずスタック上に格納しています。
│			xld.w		[%sp+24], %r12		;//←←←┴確かにこの場合は適切なのですが、setjmp()で戻って来ない事が判っている場合は、無駄なコードとなってしまいます。	■問題点■
│			xld.w		%r11, [SEH_root]
│			xld.w		[%sp+28], %r11
│			xld.w		[SEH_root], %sp
│			ld.w		%r12, %sp
│			xcall		setjmp
│			cmp		%r10, 0
│			xjrne		test_L10
│			xld.w		%r11, [%sp+24]
│			add		%r11, 1
│			xld.w		[%sp+24], %r11
│	test_L10:
│			xld.w		%r11, [%sp+28]
│			xld.w		[SEH_root], %r11
│			xld.w		%r10, [%sp+24]
│			xadd		%sp, %sp, 32
│			ret

③--- 対策方法 ---

問題の根源は、gccがsetjmp()を認識して、(その必要が無いケースでも)、安全なコードを生成しようとする事です。
そこで、setjmp()を別の名前の関数でラップして、gccに認識させないようにして回避しました。
longjmp()はそのままでも良かったのですが、longjmp()もラップする方が効率が良くなるので、longjmp()もラップする事にしました。
そうして出来た関数が、SEH_setjmp(),SEH_longjmp()です。

ちなみにgccは、'setjmp'という関数名だけでなく、'_setjmp'や'__setjmp'という関数名も、setjmp()と認識するようです。
_setjmp()や__setjmp()という名前でラップ関数を作成すると、問題は回避出来ませんでした。
'SEH_setjmp'という関数名にすれば、大丈夫でした。

③--- 修正後の検証 ---

│	int test(int x) {	//←←←警告が出なくなりました。	■解決■
│		SEH_try {
│			x++;
│		} SEH_catch_all {
│		} SEH_end
│		return x;
│	}

↓以下のようなコードが生成されます。

│	test:
│			pushn		%r1
│			xsub		%sp, %sp, 24
│			ld.w		%r0, %r12		;//←←←xがレジスタに保持されるようになりました。	■解決■
│			xld.w		%r1, [SEH_root]
│			ld.w		%r12, %sp
│			xcall		SEH_setjmp
│			cmp		%r10, 0
│			xjrne		test_L10
│			xadd		%r0, %r0, 1
│	test_L10:
│			xld.w		[SEH_root], %r1
│			ld.w		%r10, %r0
│			xadd		%sp, %sp, 24
│			popn		%r1
│			ret

④--- 修正後の注意点 ---

厳密には、今回行った修正は、正しくない方向への修正です。
たとえば、以下のようなケースで、却って問題を生じる事が有ります。

│	int test2(char* buf) { SEH_throw(1); return 0; }
│	void test1() {
│		char* buf;
│		SEH_try {			//(A)
│			buf = malloc(16);	//(B)
│			test2(buf);
│		} SEH_catch_all {
│			free(buf);		//ここでbufは、(B)で取得した値ではなく、(A)の時点で保持した値(=不定値)になってしまう。	warning: `buf' might be used uninitialized in this function
│			SEH_return;
│		} SEH_end
│	}

修正前ならば、buf変数がスタック上に確保されているので、問題は生じません。
つまり、今回行った修正によって、却って問題を生じてしまいます。

しかし、上記の問題は、構造化例外処理の普通の使い方においては、問題にならないでしょう。
構造化例外処理の普通の使い方では、以下のように書くと思うからです。

│	int test2(char* buf) { SEH_throw(1); return 0; }
│	void test1() {
│		char* buf = malloc(16);		//┬明示的にリソースの確保失敗を返す関数の呼び出しは、tryブロックの前に書くのが普通だと思うから。
│		if(buf) {			//┘
│			SEH_try {
│				test2(buf);
│			} SEH_catch_all {
│				free(buf);	//ここでbufは、正しく、malloc()で取得した値を保持している。
│				SEH_return;
│			} SEH_end
│		}
│	}

そんなわけで、構造化例外処理の普通の使い方では問題にならないと思います。
どうしても前者のようなコードを書く必要が有るならば、volatileを使えば確実に正しいコードを生成できます。

│	int test2(char* buf) { SEH_throw(1); return 0; }
│	void test1() {
│		char* volatile buf;		//volatileを書く位置に注意せよ。'volatile char*'ではない。
│		SEH_try {			//(A)
│			buf = malloc(16);	//(B)
│			test2(buf);
│		} SEH_catch_all {
│			free(buf);		//ここでbufは、(B)で取得した値を保持している。
│			SEH_return;
│		} SEH_end
│	}
/*--------------------------------------------------------------------------*/
#endif
