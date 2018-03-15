/*	
 *	clipstr.c
 *
 *	ユーティリティ：文字列操作
 *
 *	CLiP - Common Library for P/ECE
 *	Copyright (C) 2001-2016 Naoyuki Sawa
 *
 *	* Sat Oct 11 12:29:17 JST 2014 Naoyuki Sawa
 *	- 作成開始。
 *	- strtrim(),strescape(),strcompress()を、clipmisc.h,cからclipstr.h,cへ移動しました。
 *	- strfreev(),strv_length(),strdup_printf(),strdup_vprintf(),strjoin(),strjoinv(),strsplit(),strsplit_set()を追加しました。
 *	* Mon Oct 13 02:54:55 JST 2014 Naoyuki Sawa
 *	- str_has_prefix(),str_has_suffix()を追加しました。
 *	* Sat Oct 18 18:55:59 JST 2014 Naoyuki Sawa
 *	- string_match()を追加しました。
 *	* Thu Nov 06 00:58:25 JST 2014 Naoyuki Sawa
 *	- strreverse()を追加しました。
 *	* Sat Nov 08 16:50:24 JST 2014 Naoyuki Sawa
 *	- strdupv()を追加しました。
 *	* Tue Nov 11 23:26:40 JST 2014 Naoyuki Sawa
 *	- strconcat()を追加しました。
 *	* Sat Nov 15 20:39:46 JST 2014 Naoyuki Sawa
 *	- str_has_prefix_suffixのバグを修正しました。
 *	  str_has_prefix_suffixの中で%r3を使用しているのに、%r0～%r2しか退避していないバグでした。
 *	  そのために、str_has_prefix_suffixを呼び出すモジュールが、不正な動作を起こしていました。
 *	  具体的には、cliptcl.cのTcl_GetIndex()が、アドレスエラーで不正終了してしまっていました。
 *	* Sat Mar 07 16:39:40 JST 2015 Naoyuki Sawa
 *	- strstrip(),strchug(),strchomp()を追加しました。
 *	* Sat Mar 07 23:27:25 JST 2015 Naoyuki Sawa
 *	- textwrap()を追加しました。
 *	* Sun Mar 08 10:55:52 JST 2015 Naoyuki Sawa
 *	- strsplit_set()とビットマップGCを併用し、ある程度大きなファイルを分割しようとすると、ビットマップGCがエラー停止する事が有ります。
 *	  この問題について調査した結果を、clipstr.cのstrsplit_set()関数の上に追記しました。
 *	* Mon Mar 09 23:08:36 JST 2015 Naoyuki Sawa
 *	- textwrap()をアセンブラ化しました。
 *	* Sun Jul 19 12:59:05 JST 2015 Naoyuki Sawa
 *	- quark_from_string(),quark_to_string(),intern_string()を追加しました。
 *	* Thu Jul 23 21:03:39 JST 2015 Naoyuki Sawa
 *	- strreplace()を追加しました。
 *	* Wed Aug 12 21:46:27 JST 2015 Naoyuki Sawa
 *	- memdup()を追加しました。
 *	* Thu Oct 15 00:58:32 JST 2015 Naoyuki Sawa
 *	- strnatcmp(),strnatcasecmp()を追加しました。
 *	* Thu Oct 15 22:15:08 JST 2015 Naoyuki Sawa
 *	- strnatcmp(),strnatcasecmp()をアセンブラ化しました。コードサイズが80バイト減りました。
 *	  とりあえず、C言語版,アセンブラ版の両方で、テストスイートをパスする事は確認したのですが、まだアセンブラ版の動作確認が不十分です。
 *	* Fri Oct 16 21:16:43 JST 2015 Naoyuki Sawa
 *	- strnatcmp(),strnatcasecmp()の動作を変更しました。
 *	  詳細は、clipstr.hのstrnatcmp(),strnatcasecmp()のコメントを参照して下さい。
 *	* Tue Jan 19 21:44:45 JST 2016 Naoyuki Sawa
 *	- StrConv()を追加しました。
 *	* Sat Apr 09 21:06:53 JST 2016 Naoyuki Sawa
 *	- strdup_splitpath()とstrdup_makepath()を追加しました。
 *	  splitpath()とmakepath()も、strdup_printf()と同様に使えると便利なので、自分の判断で'strdup_*()'バージョンを追加する事にしました。
 *	* Fri May 06 21:58:10 JST 2016 Naoyuki Sawa
 *	- intern_string()が、キーと値が同じメモリを指すようにして、メモリの無駄を低減しました。
 *	* Sat May 21 22:36:02 JST 2016 Naoyuki Sawa
 *	- strsplit()とstrsplit_set()をマルチバイト対応しました。
 *	  古いコードでは配列のメモリ確保を関数内で行っていましたが、新しいコードではstrv.cの機能を使うように変更し、コードが簡潔になりました。
 *	  コードが簡潔になったことによって、マルチバイト対応を追加したにもかかわらず、当モジュールのコードサイズが約24バイト小さくなりました。
 *	  ただし、strv.oもリンクされるようになった分コードサイズが増加するので、全体としてはコードサイズは増加していると思います。
 *	- 新しいコードで実装を大幅に変更しましたが、基本的なアルゴリズムは古いコードと変わっていません。
 *	  新しいコードには詳細なコメントを付けていないので、アルゴリズムについては古いコードのコメントを参照して下さい。
 *	- マルチバイト非対応の古いコードも、これまで長く安定して動作していた実績が有るので、コードを残しておく事にしました。
 *	  USE_STRSPLIT_MBCSシンボルを定義してライブラリをビルドすると、新しいマルチバイト対応版のstrsplit()とstrsplit_set()を使用します。
 *	  USE_STRSPLIT_MBCSシンボルを定義せずにライブラリをビルドすると、古いマルチバイト非対応版のstrsplit()とstrsplit_set()を使用します。
 *	  基本的には、新しいマルチバイト対応版のstrsplit()とstrsplit_set()を使う予定ですが、もし今後マルチバイト対応が不要になった場合は、古いコードに戻して下さい。
 *	- ■検証プログラム
 *	  │void test1() {
 *	  │  char** s = strsplit("表現\\十回\\予備", "\\", 0);
 *	  │  char** t = s;
 *	  │  while(*t) { printf("'%s'\n", *t++); }
 *	  │  strfreev(s);
 *	  │}
 *	  │//□結果(マルチバイト非対応版)…誤り
 *	  │//│'\x95'  「表」の2バイト目に'\'がマッチしてしまっている。
 *	  │//│'現'
 *	  │//│'\x8F'  「十」の2バイト目に'\'がマッチしてしまっている。
 *	  │//│'回'
 *	  │//│'\x97'  「予」の2バイト目に'\'がマッチしてしまっている。
 *	  │//│'備'
 *	  │//□結果(マルチバイト対応版)…正しい
 *	  │//│'表現'
 *	  │//│'十回'
 *	  │//│'予備'
 *	  │void test2() {
 *	  │  char** s = strsplit_set("表現\\十回\\予備", "表十予", 0);
 *	  │  char** t = s;
 *	  │  while(*t) { printf("'%s'\n", *t++); }
 *	  │  strfreev(s);
 *	  │}
 *	  │//□結果(マルチバイト非対応版)…誤り
 *	  │//│''      「表」の1バイト目が個別に「表」の1バイト目にマッチしてしまっている。
 *	  │//│''      「表」の2バイト目が個別に「表」の2バイト目にマッチしてしまっている。
 *	  │//│'現'    「表」又は「十」又は「予」の2バイト目が'\'にマッチしてしまっている。
 *	  │//│''      「十」の1バイト目が個別に「十」の1バイト目にマッチしてしまっている。
 *	  │//│''      「十」の2バイト目が個別に「十」の2バイト目にマッチしてしまっている。
 *	  │//│'回'    「表」又は「十」又は「予」の2バイト目が'\'にマッチしてしまっている。
 *	  │//│''      「予」の1バイト目が個別に「予」の1バイト目にマッチしてしまっている。
 *	  │//│''      「予」の2バイト目が個別に「予」の2バイト目にマッチしてしまっている。
 *	  │//│'備'    「表」又は「十」又は「予」の2バイト目が'\'にマッチしてしまっている。
 *	  │//□結果(マルチバイト対応版)…正しい
 *	  │//│''
 *	  │//│'現\'
 *	  │//│'回\'
 *	  │//│'備'
 *	* Sun May 22 21:58:55 JST 2016 Naoyuki Sawa
 *	- textwrap()の実装を変更しました。結果は変わりません。
 *	  変更点は以下の通りです。
 *	- 変更前は、textwrap()の中で文字列配列のメモリ確保を行っていました。
 *	  変更後は、strv.cモジュールの機能を使って文字列配列のメモリ確保を行うようにしました。
 *	  この変更によって、関数の実装が単純になり、コードサイズが約44バイト小さくなりました。
 *	  strv.oをリンクするためのコードサイズ増加は有りますが、既にstrstrip()がstrv.oを使用しているので、新たな増加は有りません。
 *	- 変更前のコードも、これまで安定して(…という程頻繁には使っていませんでしたが…)動作していたので、#if 0～で切り分けて残してあります。
 *	  変更後のコードには、変更前のコードほど詳細なコメントは付けていないので、アルゴリズムについては変更前のコードのコメントを参照して下さい。
 *	  変更後のコードも、変更前のコードと同じアルゴリズムで、文字列配列のメモリを確保する方法が変わっただけです。
 *	* Wed May 25 10:22:02 JST 2016 Naoyuki Sawa
 *	- C言語版のstrcompress()をマルチバイト対応しました。
 *	  strcompress()をマルチバイト対応した目的は、clipyaml.cモジュールのyaml_path_get_node(),yaml_path_set_string()で使用するためです。
 *	  詳細は、clipyaml.cモジュールの同日のコメントを参照して下さい。
 *	- アセンブラ版のstrcompress()はまだ、マルチバイト対応していません。
 *	  アセンブラ版のstrcompress()も、マルチバイト対応する必要が有ります。	⇒{{2016/05/26コメント追記:アセンブラ版のstrcompress()もマルチバイト対応しました。}}
 *	- strcompress()はstrescape()と対を成す関数ですが、今回strcompress()だけをマルチバイト対応した事により、strcompress()とstrescape()の非対称性が生じました。
 *	  半角文字だけを使う限りは問題は生じませんが、全角文字を使用すると、strescape()でエスケープした文字列をstrcompress()で復元した時に、元の文字列にならない可能性が有ります。
 *	  対称性を保つために、strescape()もマルチバイト対応する必要が有ります。	⇒{{2016/05/26コメント追記:strescape()をマルチバイト対応しました。}}
 *	* Thu May 26 20:37:02 JST 2016 Naoyuki Sawa
 *	- アセンブラ版のstrcompress()もマルチバイト対応しました。
 *	  以下の動作確認を行いました。
 *	  │//下記のテスト関数から利用するサブルーチン
 *	  │void test_subr(const char* s, const char* delim) {
 *	  │  do {
 *	  │    char* t = strcompress(s, (char**)&s, delim);
 *	  │    printf("'%s'\n", t);
 *	  │    free(t);
 *	  │    s = mbsninc(s, 1);  //sが既に'\0'を指していた場合に進まないようにするためmbsninc()を使う必要が有る事に注意せよ。mbsinc()ではsが既に'\0'を指していても進んでしまう。
 *	  │  } while(*s);
 *	  │}
 *	  │//■マルチバイト対応になった事を確認するテスト
 *	  │void test1() {
 *	  │  test_subr("表現\\十回\\予備", "\\");
 *	  │  //□変更前
 *	  │  //│'\x95'	←変更前は、全角文字の2バイト目の'\'を間違えてエスケープの'\'と見なしてしまっていた。
 *	  │  //│'現'
 *	  │  //│'\x8F'
 *	  │  //│'回'
 *	  │  //│'\x97'
 *	  │  //│'備'
 *	  │  //□変更前
 *	  │  //│'表現'	←変更後は、正しくなった。
 *	  │  //│'十回'
 *	  │  //│'予備'
 *	  │}
 *	  │//■マルチバイト対応版が様々なケースで正しく動作している事を確認するテスト(以下は全て変更後の出力です)
 *	  │void test2() {
 *	  │  test_subr("", NULL);
 *	  │  //│''
 *	  │  test_subr("ﾊﾝｶｸ", NULL);
 *	  │  //│'ﾊﾝｶｸ'
 *	  │  test_subr("全角", NULL);
 *	  │  //│'全角'
 *	  │  test_subr("\\x30\\x31\\x32", NULL);
 *	  │  //│'012'
 *	  │  test_subr("\\x30\\x31\\x32", "\\");
 *	  │  //│''
 *	  │  //│'x30'
 *	  │  //│'x31'
 *	  │  //│'x32'
 *	  │  test_subr("あいう.えお[かき]", ".[]");
 *	  │  //│'あいう'
 *	  │  //│'えお'
 *	  │  //│'かき'
 *	  │  test_subr("あいう．えお［かき］", "．［］");		//全角文字のデリミタも利用出来るようになった。
 *	  │  //│'あいう'
 *	  │  //│'えお'
 *	  │  //│'かき'
 *	  │  test_subr("あいう\\.えお\\[かき\\]", ".[]");
 *	  │  //│'あいう.えお[かき]'
 *	  │  test_subr("あいう\\．えお\\［かき\\］", "．［］");	//全角文字のデリミタも'\'でエスケープ出来ます。
 *	  │  //│'あいう．えお［かき］'
 *	  │}
 *	* Thu May 26 23:31:18 JST 2016 Naoyuki Sawa
 *	- strescape()をマルチバイト対応しました。
 *	  この対応により、strescape()とstrcompress()の対称性が復活しました。
 *	  詳細は、'Wed May 25 10:22:02 JST 2016'のコメントを参照して下さい。
 *	  以下の動作確認を行いました。
 *	  │//下記のテスト関数から利用するサブルーチン
 *	  │void test_subr(const char* s, const char* need_escape) {
 *	  │  char *t, *u;
 *	  │  t = strescape(s, need_escape);	//┬エスケープした結果を表示する。
 *	  │  printf("'%s'\n", t);		//┘
 *	  │  u = strcompress(t, NULL, NULL);	//┬エスケープを解除して元の文字列とバイナリ一致する事を確認する。
 *	  │  if(strcmp(s, u)) { DIE(); }	//┘
 *	  │  free(t);
 *	  │  free(u);
 *	  │}
 *	  │//■マルチバイト対応版が様々なケースで正しく動作している事を確認するテスト
 *	  │void test() {
 *	  │  test_subr("\1\2\3\4\5\6\7\10\11\12\13\14\15\16\17", NULL);
 *	  │  //│'\001\002\003\004\005\006\a\b\t\n\v\f\r\016\017'
 *	  │  test_subr("C:\\usr\\PIECE\\docs\\PIECE ポート解説.htm", NULL);
 *	  │  //│'C:\\usr\\PIECE\\docs\\PIECE ポート解説.htm'
 *	  │  test_subr("ABC.DEF[123]", NULL);
 *	  │  //│'ABC.DEF[123]'
 *	  │  test_subr("ABC.DEF[123]", ".[]");
 *	  │  //│'ABC\.DEF\[123\]'
 *	  │  test_subr("表現.十回[予備]", NULL);
 *	  │  //│'表現.十回[予備]'				//全角文字はエスケープせずにそのまま表示するようになった。
 *	  │  test_subr("表現.十回[予備]", ".[]");
 *	  │  //│'表現\.十回\[予備\]'
 *	  │  test_subr("表現．十回［予備］", "．［］");	//全角文字もエスケープ対象に出来ます。
 *	  │  //│'表現\．十回\［予備\］'
 *	  │}
 *	* Mon May 30 21:45:50 JST 2016 Naoyuki Sawa
 *	- intern_string()自身で排他制御を行うようにしました。
 *	  これまでは、アプリケーションが注意してintern_string()を割り込みルーチンやマルチスレッド環境で使用しないようにしていましたが、
 *	  intern_string()は様々なモジュールからも間接的に利用するので、アプリケーションが意図して割り込みルーチンやマルチスレッド環境で使用しないように注意するのは難しいと思ったからです。
 *	  今後は、quark_from_string()/quark_to_string()/intern_string()や、これらを間接的に呼び出す各モジュールの関数を、割り込みルーチンやマルチスレッド環境で安全に使用出来るようになりました。
 *	* Mon Jun 06 21:45:00 JST 2016 Naoyuki Sawa
 *	- memrotate()を追加しました。
 *	* Tue Jun 07 22:13:11 JST 2016 Naoyuki Sawa
 *	- memrotate()の実装を変更しました。
 *	  詳細は、clipstl.cモジュールの、std_rotate()関数のコメントを参照して下さい。
 *	  memrotate()の関数仕様は変わっていません。
 *	* Wed Sep 07 21:52:10 JST 2016 Naoyuki Sawa
 *	- strescape(),及び,strcompress()が、(int)でアドレス演算を行っていたのを、(intptr_t)で行うように修正しました。
 *	  64ビット対応のための準備ですが、32ビットでも(intptr_t)で行う方が本当は正しいからです。
 *	  とは言え、32ビットではどちらでも同じ結果になるので、既存の32ビットコードには影響有りません。
 *	- quark_from_string(),quark_to_string()を、32ビット版と64ビット版とで、別の実装にしました。
 *	  32ビット版はこれまで通り、単一化表現の文字列ポインタをそのまま(int)として返せば良いのですが、
 *	  64ビット版はポインタが64ビットのため、文字列ポインタをそのまま(int)として返せなくなったからです。
 *	  64ビット版は、単一化表現の文字列ポインタ⇔(int)を相互変換するための、新たな辞書を設ける事にしました。
 *	* Fri Sep 30 21:41:38 JST 2016 Naoyuki Sawa
 *	- strreverse()のC言語版の実装で、'goto L_START'が変数'c'の宣言をまたいでいた問題を修正しました。
 *	  動作上はそのままでも問題ありませんし、P/ECE開発環境(GCC)やWindows(VC++6.0,VS2015)では警告も出ないのですが、Hewで「C5546 (W) Transfer of control bypasses initialization of:」という警告が出たからです。
 *	  今回の変更は、Hewで上記の警告を消すためだけの修正であり、今回の修正による動作の変化は有りません。
 *	  尚、今回の変更を行っても、Hewではなお「C5128 (I) Loop is not reachable from preceding code」という警告が出るのですが、C言語仕様的に問題無く、明らかに誤った警告なので、その警告は無視する事にしました。
 */
#include "clip.h"

/*****************************************************************************
 *	トリム
 *****************************************************************************/

static const char strtrim_reject[]="\t\n\v\f\r ";//isspace()と同等	//asmブロックだけから参照する'定数'は、__attribute__((unused))を宣言しなくても警告が出ないようです。一方、asmブロックだけから参照する'変数'と'関数'は、__attribute__((unused))を宣言しなければ警告が出ます。
#ifndef PIECE
char* strtrim(const char* s, int mode, const char* reject) {
	int c;
	char *t, *u;
	if(!reject) { reject = strtrim_reject; }
	switch(mode) {
	case 0://両端にある指定文字を除去する
		t = strtrim(s, 1, reject);					//左端にある指定文字を除去して、途中の文字列を作成する。
		u = strtrim(t, 2, reject);					//右端にある指定文字を除去して、結果の文字列を作成する。
		free(t);							//左端にある指定文字を除去した、途中の文字列を削除する。
		break;
	case 1://左端にある指定文字を除去する
		while((c = *s++) && strchr(reject, c)) { /** no job **/ }	//終端でなく、指定文字以外が見つかるまで、次の文字へ進める。
		if(!(u = strdup(s - 1))) { DIE(); }				//結果の文字列を作成する。sは1文字分先へ進み過ぎていることに注意せよ。
		break;
	case 2://右端にある指定文字を除去する
		t = strchr(s, '\0');						//終端のnulの位置へ進める。
		do { t--; } while((t >= s) && strchr(reject, *t));		//前の文字へ戻す。先頭を超えた,又は,指定文字以外が見つかったら抜ける。
		if(!(u = strndup(s, t - s + 1))) { DIE(); }			//結果の文字列を作成する。tは1文字分前へ戻り過ぎていることに注意せよ。
		break;
	case 3://文字列内にある全ての指定文字を除去する
		if(!(t = u = strdup(s))) { DIE(); }				//文字列を複製する。
		while((c = *t++ = *s++)) { if(strchr(reject, c)) { t--; } }	//指定文字以外を前詰めでコピーする。このcaseのみ、結果の文字列より大きなメモリのままになるが、使用頻度が低いと思うので許容することにした。
		break;
	default:
		DIE();
	}
	return u;
}
#else //PIECE
//アセンブラ化した目的は、コードサイズの低減です。
//コードサイズが、(C言語版=276バイト)⇒(Asm版=192バイト)に小さくなりました。
//処理速度は、メモリ確保の処理に要する時間が大きいので、元々期待できません。
char* strtrim(const char* s, int mode, const char* reject);
asm("
		.code
		.align		1
		.global		strtrim
strtrim:
		pushn		%r3
		;//%r12 := s
		;//%r13 := mode
		;//%r14 := reject
		cmp		%r14, 0				;//if(!reject) { reject = strtrim_reject }
		jrne.d		5				;//
		 cmp		%r13, 3				;//%psr := ((unsigned)mode <=> (unsigned)3)		─┐	*delay*
		 ext		      strtrim_reject@h		;//							　│
		 ext		      strtrim_reject@m		;//							　│
		 ld.w		%r14, strtrim_reject@l		;//							　│
		jrugt		strtrim_DIE			;//if((unsigned)mode >  (unsigned)3) { goto DIE() }	←┘
		ld.w		%r0, %r12			;//%r0  := s
		jreq.d		strtrim_case_3			;//if((unsigned)mode == (unsigned)3) { goto case_3 }
		ld.w		%r1, %r14			;//%r1  := reject						*delay*
		cmp		%r13, 1				;//%psr := ((unsigned)mode <=> (unsigned)1)
		jrugt		strtrim_case_2			;//if((unsigned)mode >  (unsigned)1) { goto case_2 }
		jreq		strtrim_case_1			;//if((unsigned)mode == (unsigned)1) { goto case_1 }
		;//---------------------------------------------;//
;//strtrim_case_0:
		;//%r0  := s			←使わない
		;//%r1  := reject
		;//%r12 := s
		;//%r14 := reject
		call.d		strtrim				;//%r10 := t = strtrim(s, 1, reject)
		ld.w		%r13, 1				;//%r13 :=                1					*delay*
		ld.w		%r2, %r10			;//%r2  := t
		ld.w		%r12, %r2			;//%r12 :=             t
		ld.w		%r13, 2				;//%r13 :=                2
		call.d		strtrim				;//%r10 := u = strtrim(t, 2, reject)
		ld.w		%r14, %r1			;//%r14 :=                   reject				*delay*
		ld.w		%r3, %r10			;//%r3  := u
		xcall.d		free				;//   free(t)
		ld.w		%r12, %r2			;//%r12 := t							*delay*
		jp		strtrim_return_u		;//return  u
		;//---------------------------------------------;//
strtrim_case_1:	
		;//%r0  := s
		;//%r1  := reject
		;//%r12 := s			←使わない
		;//%r14 := reject		←使わない
strtrim_case_1_L10:						;//do {
		ld.b		%r13, [%r0]+			;//%r13 := c = *s++
		cmp		%r13, 0				;//if(!c) { break }
		jreq		strtrim_case_1_L20		;//
		xcall.d		strchr				;//%r10 := strchr(reject, c)
		ld.w		%r12, %r1			;//%r12 :=        reject					*delay*
		cmp		%r10, 0				;//} while(strchr(reject, c))
		jrne		strtrim_case_1_L10		;//
strtrim_case_1_L20:
		sub		%r0, 1				;//%r0  := s--
		jp.d		strtrim_case_2_L30		;//goto case_2_L30
		ld.w		%r13, -1			;//%r13 := n = SIZE_MAX						*delay*
		;//---------------------------------------------;//
strtrim_case_2:
		;//%r0  := s
		;//%r1  := reject
		;//%r12 := s
		;//%r14 := reject		←使わない
		xcall.d		strchr				;//%r10 := t = strchr(s, '￥0')
		ld.w		%r13, 0				;//%r13 :=               '￥0'					*delay*
		ld.w		%r2, %r10			;//%r2  := t
strtrim_case_2_L10:						;//do {
		cmp		%r2, %r0			;//     if(t-- <= s) { break }
		jrule.d		strtrim_case_2_L20		;//
		sub		%r2, 1				;//%r2  := t--							*delay*
		ld.b		%r13, [%r2]			;//
		xcall.d		strchr				;//%r10 := strchr(reject, *t)
		ld.w		%r12, %r1			;//%r12 :=        reject					*delay*
		cmp		%r10, 0				;//} while(strchr(reject, *t))
		jrne		strtrim_case_2_L10		;//
strtrim_case_2_L20:						;//
		ld.w		%r13, %r2			;//%r13 :=     t
		sub		%r13, %r0			;//%r13 :=     t - s
		add		%r13, 1				;//%r13 := n = t - s + 1
strtrim_case_2_L30:						;//
		xcall.d		strndup				;//%r10 := u = strndup(s, n)
		ld.w		%r12, %r0			;//%r12 :=             s					*delay*
		cmp		%r10, 0				;//    if(!u) { DIE() }
		jreq		strtrim_DIE			;//
		jp.d		strtrim_return_u		;//return  u
		ld.w		%r3, %r10			;//%r3  := u							*delay*
		;//---------------------------------------------;//
strtrim_case_3:
		;//%r0  := s
		;//%r1  := reject
		;//%r12 := s
		;//%r14 := reject		←使わない
		xcall		strdup				;//%r10 := u = strdup(s, n)
		cmp		%r10, 0				;//    if(!u) { DIE() }
		jreq		strtrim_DIE			;//
		ld.w		%r2, %r10			;//%r2  := t
		ld.w		%r3, %r10			;//%r2  := u
strtrim_case_3_L10:						;//for(;;) {
		ld.b		%r13, [%r0]+			;//%r13 := c = *s++
		ld.b		[%r2]+, %r13			;//*t++  = c
		cmp		%r13, 0				;//    if(!c) { return  u }
		jreq		strtrim_return_u		;//
		xcall.d		strchr				;//%r10 := p = strchr(reject, c)
		ld.w		%r12, %r1			;//%r12 :=            reject					*delay*
		cmp		%r8, %r10			;//%psr(C) :=   !!p								┐
		jp.d		strtrim_case_3_L10		;//										├「if(strchr(reject, c)) { t--; }」を条件分岐無しで実装する工夫
		sbc		%r2, %r8			;//%r2  := t -= !!p						*delay*		┘
		;//---------------------------------------------;//
strtrim_return_u:
		;//%r3  := u
		ld.w		%r10, %r3			;//%r10 := u
		popn		%r3				;//
		ret						;//return  u
");
static void __attribute__((noreturn,unused))/*asmブロックから参照*/ strtrim_DIE() { DIE(); }
#endif//PIECE

#ifndef PIECE
static char* strstrip_strchug_strchomp(char* s, int mode) {
	char* t = strtrim(s, mode, NULL);	//空白文字を除去し一時的な文字列を作成する。
	strcpy(s, t);	//一時的な文字列を元の文字列にコピーする。
	free(t);	//一時的な文字列を開放する。
	return s;	//元の文字列を返す。
}
char* strstrip(char* s) { return strstrip_strchug_strchomp(s, 0); }
char* strchug( char* s) { return strstrip_strchug_strchomp(s, 1); }
char* strchomp(char* s) { return strstrip_strchug_strchomp(s, 2); }
#else //PIECE
char* strstrip(char* s);
char* strchug( char* s);
char* strchomp(char* s);
asm("
		.code
		.align		1
		.global		strstrip
		.global		strchug
		.global		strchomp
strstrip:
		jp.d		strstrip_strchug_strchomp
		ld.w		%r13, 0				;//%r13 :=                mode = 0	*delay*
strchug:
		jp.d		strstrip_strchug_strchomp
		ld.w		%r13, 1				;//%r13 :=                mode = 1	*delay*
strchomp:
		ld.w		%r13, 2				;//%r13 :=                mode = 2
strstrip_strchug_strchomp:
		pushn		%r1
		ld.w		%r0, %r12			;//%r0  := s
		xcall.d		strtrim				;//%r10 := t = strtrim(s, mode, NULL)
		ld.w		%r14, 0				;//%r14 :=                      NULL	*delay*
		ld.w		%r1, %r10			;//%r0  := t
		ld.w		%r12, %r0			;//%r12 :=        s
		xcall.d		strcpy				;//%r10 := strcpy(s, t)
		ld.w		%r13, %r1			;//%r13 :=           t			*delay*
		xcall.d		free				;//        free(t)
		ld.w		%r12, %r1			;//%r12 :=      t			*delay*
		ld.w		%r10, %r0			;//%r10 := s
		popn		%r1				;//
		ret						;//return  2
");
#endif//PIECE

/*****************************************************************************
 *	エスケープ
 *****************************************************************************/

//アセンブラ化した目的は、コードサイズの低減です。
//コードサイズが、(C言語版=528バイト)⇒(Asm版=456バイト)に小さくなりました。(TBL_strescape[],strescape(),strcompress()の合計サイズ)	⇒{{2016/05/26コメント追記:strescape()とstrcompress()をマルチバイト対応した事により、コードサイズが左記よりも増加しました。}}
//処理速度は、メモリ確保の処理に要する時間が大きいので、元々期待できません。
#ifdef  __GNUC__
//GCCは「\e」(非標準のエスケープシーケンス)に対応している。
#define SIZE_strescape 13//─────────────────┐
static const char TBL_strescape[]={//                        ↓		//asmブロックだけから参照する'定数'は、__attribute__((unused))を宣言しなくても警告が出ないようです。一方、asmブロックだけから参照する'変数'と'関数'は、__attribute__((unused))を宣言しなければ警告が出ます。
//1    2    3    4    5    6    7    8    9   10   11   12   13
'\a','\b','\e','\f','\n','\r','\t','\v','\?','\'','\"','\\','\0',	//末尾の'\0'はstrescape()  で行うstrchr()の終端用です。エスケープ対象文字ではありません。
 'a', 'b', 'e', 'f', 'n', 'r', 't', 'v','\?','\'','\"','\\','\0'};	//末尾の'\0'はstrcompress()で行うstrchr()の終端用です。エスケープ対象文字ではありません。
#else //__GNUC__
//VC++6.0は「\e」(非標準のエスケープシーケンス)に対応していない。
#define SIZE_strescape 12//─────────────────┐
static const char TBL_strescape[]={//                        ↓
//1    2         3    4    5    6    7    8    9   10   11   12
'\a','\b',     '\f','\n','\r','\t','\v','\?','\'','\"','\\','\0',	//末尾の'\0'はstrescape()  で行うstrchr()の終端用です。エスケープ対象文字ではありません。
 'a', 'b',      'f', 'n', 'r', 't', 'v','\?','\'','\"','\\','\0'};	//末尾の'\0'はstrcompress()で行うstrchr()の終端用です。エスケープ対象文字ではありません。
#endif//__GNUC__
// * Thu May 26 23:31:18 JST 2016 Naoyuki Sawa
// - strescape()をマルチバイト対応しました。
//   この対応により、strescape()とstrcompress()の対称性が復活しました。
//   詳細は、'Wed May 25 10:22:02 JST 2016'のコメントを参照して下さい。
//   以下の動作確認を行いました。
//   │//下記のテスト関数から利用するサブルーチン
//   │void test_subr(const char* s, const char* need_escape) {
//   │  char *t, *u;
//   │  t = strescape(s, need_escape);	//┬エスケープした結果を表示する。
//   │  printf("'%s'\n", t);		//┘
//   │  u = strcompress(t, NULL, NULL);	//┬エスケープを解除して元の文字列とバイナリ一致する事を確認する。
//   │  if(strcmp(s, u)) { DIE(); }	//┘
//   │  free(t);
//   │  free(u);
//   │}
//   │//■マルチバイト対応版が様々なケースで正しく動作している事を確認するテスト
//   │void test() {
//   │  test_subr("\1\2\3\4\5\6\7\10\11\12\13\14\15\16\17", NULL);
//   │  //│'\001\002\003\004\005\006\a\b\t\n\v\f\r\016\017'
//   │  test_subr("C:\\usr\\PIECE\\docs\\PIECE ポート解説.htm", NULL);
//   │  //│'C:\\usr\\PIECE\\docs\\PIECE ポート解説.htm'
//   │  test_subr("ABC.DEF[123]", NULL);
//   │  //│'ABC.DEF[123]'
//   │  test_subr("ABC.DEF[123]", ".[]");
//   │  //│'ABC\.DEF\[123\]'
//   │  test_subr("表現.十回[予備]", NULL);
//   │  //│'表現.十回[予備]'				//全角文字はエスケープせずにそのまま表示するようになった。
//   │  test_subr("表現.十回[予備]", ".[]");
//   │  //│'表現\.十回\[予備\]'
//   │  test_subr("表現．十回［予備］", "．［］");	//全角文字もエスケープ対象に出来ます。
//   │  //│'表現\．十回\［予備\］'
//   │}
#ifndef PIECE
char* strescape(const char* s, const char* need_escape) {
	char* r = NULL;
	for(;;) {//1周目で文字数を数え、2周目で文字を格納する
		const char *t = s, *p;
		      char *u = r;
		int c;
	//{{2016/05/26変更:strescape()をマルチバイト対応しました。
	//	while((c = *t++)) {
	//↓2016/05/26変更:strescape()をマルチバイト対応しました。
		for(;;) {
			c = mbsnextc(t);
			t = mbsinc(t);
			if(!c) { break; }
	//}}2016/05/26変更:strescape()をマルチバイト対応しました。
			/* C言語のエスケープシーケンス */
	//{{2016/05/26変更:strescape()をマルチバイト対応しました。
	//		if((p = strchr(TBL_strescape, c))) {		//テーブルの前半から検索
	//↓2016/05/26変更:strescape()をマルチバイト対応しました。
			if((p = mbschr(TBL_strescape, c))) {		//テーブルの前半から検索
	//}}2016/05/26変更:strescape()をマルチバイト対応しました。
				if(r/*2周目*/) {
					u[0] = '\\';
					u[1] = *(p + SIZE_strescape);	//テーブルの後半へシフト
				}
				u += 2;
			/* アプリケーション定義のエスケープが必要な文字 */
	//{{2016/05/26変更:strescape()をマルチバイト対応しました。
	//		} else if(need_escape && strchr(need_escape, c)) {
	//			if(r/*2周目*/) {
	//				u[0] = '\\';
	//				u[1] = c;
	//			}
	//			u += 2;
	//↓2016/05/26変更:strescape()をマルチバイト対応しました。
			} else if(need_escape && mbschr(need_escape, c)) {
				if(c > UCHAR_MAX) {
					if(r/*2周目*/) {
						u[0] = '\\';
						u[1] = (c >> 8);
						u[2] = c;
					}
					u += 3;
				} else {
					if(r/*2周目*/) {
						u[0] = '\\';
						u[1] = c;
					}
					u += 2;
				}
	//}}2016/05/26変更:strescape()をマルチバイト対応しました。
	//{{2016/05/26追加:strescape()をマルチバイト対応しました。
			/* 全角文字 */
			} else if(c > UCHAR_MAX) {
				if(r/*2周目*/) {
					u[0] = (c >> 8);
					u[1] = c;
				}
				u += 2;
	//}}2016/05/26追加:strescape()をマルチバイト対応しました。
			/* 印字可能でない文字 */
			} else if(!isprint(c)) {
				if(r/*2周目*/) {
					u[0] = '\\';
					u[1] = '0' + ((c >> 6) & 3);	//┐
					u[2] = '0' + ((c >> 3) & 7);	//├この後に'0'～'7'が続く場合のために常に最大の3桁を出力しておく。
					u[3] = '0' + ((c >> 0) & 7);	//┘
				}
				u += 4;
			/* それ以外 */
			} else {
				if(r/*2周目*/) {
					u[0] = c;
				}
				u += 1;
			}
		}
		if(r/*2周目*/) {
			u[0] = '\0';
			return r;
		}
//{{2016/09/07変更:strescape(),及び,strcompress()が、(int)でアドレス演算を行っていたのを、(intptr_t)で行うように修正しました。
//		if(!(r = malloc((int)u + 1/*nul*/))) { DIE(); }
//↓2016/09/07変更:strescape(),及び,strcompress()が、(int)でアドレス演算を行っていたのを、(intptr_t)で行うように修正しました。
		if(!(r = malloc((intptr_t)u + 1/*nul*/))) { DIE(); }
//}}2016/09/07変更:strescape(),及び,strcompress()が、(int)でアドレス演算を行っていたのを、(intptr_t)で行うように修正しました。
	}
}
#else //PIECE
char* strescape(const char* s, const char* need_escape);
asm("
		.code
		.align		1
		.global		strescape
strescape:
		pushn		%r3
		xsub		%sp, %sp, 8
		xld.w		[%sp+4], %r12			;//[%sp+4] := s
		xld.w		[%sp+0], %r13			;//[%sp+0] := need_escape
		ld.w		%r0, 0				;//%r0  := r = NULL
		;//---------------------------------------------;//
strescape_L10:							;//for(;;) {
		xld.w		%r1, [%sp+4]			;//  %r1  := t = s
		ld.w		%r2, %r0			;//  %r2  := u = r
		;//---------------------------------------------;//
strescape_L20:							;//
	;//{{2016/05/26変更:strescape()をマルチバイト対応しました。
	;//	ld.b		%r3, [%r1]+			;//  %r3  := c = *t++
	;//	cmp		%r3, 0				;//  if(!c) {
	;//	jrne		strescape_L30
	;//↓2016/05/26変更:strescape()をマルチバイト対応しました。
		xcall.d		mbsnextc			;//  %r10 := c = mbsnextc(t)
		ld.w		%r12, %r1			;//  %r12 :=              t					*delay*
		ld.w		%r3, %r10			;//  %r3  := c
		xcall.d		mbsinc				;//  %r10 := t = mbsinc(t)
		ld.w		%r12, %r1			;//  %r12 :=            t					*delay*
		cmp		%r3, 0				;//  if(!c) {
		jrne.d		strescape_L30
		ld.w		%r1, %r10			;//  %r1  := t							*delay*
	;//}}2016/05/26変更:strescape()をマルチバイト対応しました。
		cmp		%r0, 0				;//    if(!r) {
		jrne		strescape_L25
		add		%r2, 1				;//      %r2  :=            u + 1
		xcall.d		malloc				;//      %r10 := r = malloc(u + 1)
		ld.w		%r12, %r2			;//      %r12 :=            u + 1				*delay*
		cmp		%r10, 0				;//      if(!r) { DIE() }
		jreq		strescape_DIE
		jp.d		strescape_L10
		ld.w		%r0, %r10			;//      %r0  := r						*delay*
strescape_L25:							;//    } else {
		ld.b		[%r2], %r8			;//      *u    = '￥0'
		ld.w		%r10, %r0			;//      return  r
		xadd		%sp, %sp, 8
		popn		%r3
		ret						;//    }
		;//---------------------------------------------;//  } else {
strescape_L30:
	;//{{2016/05/26変更:strescape()をマルチバイト対応しました。
	;//	xld.w		%r12, TBL_strescape		;//    %r12 :=            TBL_strescape
	;//	xcall.d		strchr				;//    %r10 := p = strchr(TBL_strescape, c))
	;//	ld.w		%r13, %r3			;//    %r13 :=                           c			*delay*
	;//	cmp		%r10, 0				;//    if(    (p = strchr(TBL_strescape, c)) {
	;//	jreq		strescape_L40
	;//↓2016/05/26変更:strescape()をマルチバイト対応しました。
		xld.w		%r12, TBL_strescape		;//    %r12 :=            TBL_strescape
		xcall.d		mbschr				;//    %r10 := p = mbschr(TBL_strescape, c))
		ld.w		%r13, %r3			;//    %r13 :=                           c			*delay*
		cmp		%r10, 0				;//    if(    (p = mbschr(TBL_strescape, c)) {
		jreq		strescape_L40
	;//}}2016/05/26変更:strescape()をマルチバイト対応しました。
		cmp		%r0, 0				;//      if(!r) { u += 2
		jreq		strescape_ADD_2			;//      } else {
		xld.w		%r9, 92				;//        %r9  := '￥'
		ld.b		[%r2]+, %r9			;//        *u++  = '￥'
		add		%r10, 13			;//        %r10 :=   p + SIZE_strescape
		ld.b		%r9, [%r10]			;//        %r9  := *(p + SIZE_strescape)
		ld.b		[%r2]+, %r9			;//        *u++  = *(p + SIZE_strescape)
		jp		strescape_L20			;//      }
		;//---------------------------------------------;//    } else
strescape_L40:
	;//{{2016/05/26変更:strescape()をマルチバイト対応しました。
	;//	xld.w		%r12, [%sp+0]			;//    %r12 := need_escape
	;//	cmp		%r12, 0				;//    if((    need_escape) &&
	;//	jreq		strescape_L50
	;//	xcall.d		strchr				;//    strchr( need_escape, c)) {
	;//	ld.w		%r13, %r3			;//    %r13 :=              c					*delay*
	;//	cmp		%r10, 0
	;//	jreq		strescape_L50
	;//	cmp		%r0, 0				;//      if(!r) { u += 2
	;//	jreq		strescape_ADD_2			;//      } else {
	;//	xld.w		%r9, 92				;//        %r9  := '￥'
	;//	ld.b		[%r2]+, %r9			;//        *u++  = '￥'
	;//	ld.b		[%r2]+, %r3			;//        *u++  = c
	;//	jp		strescape_L20			;//      }
	;//↓2016/05/26変更:strescape()をマルチバイト対応しました。
		xld.w		%r12, [%sp+0]			;//    %r12 := need_escape
		cmp		%r12, 0				;//    if((    need_escape) &&
		jreq		strescape_L50
		xcall.d		mbschr				;//    mbschr( need_escape, c)) {
		ld.w		%r13, %r3			;//    %r13 :=              c					*delay*
		cmp		%r10, 0
		jreq		strescape_L50
		ld.w		%r4, %r3			;//      %r4  :=   c
		sra		%r4, 8				;//      %r4  :=  (c >> 8)
		xld.w		%r5, 92				;//                 %r5  := '￥'	←───┐	(!r)の時は無駄処理となるが害は無い。コードサイズ低減を優先する事にした。
		jreq.d		strescape_L45			;//      if(      (c >> 8)) {			│
		cmp		%r0, 0				;//      %psr(Z) := !r			──┐	│		*delay*
		jreq		strescape_ADD_3			;//        if(!r) { u += 3		←─┤	│
	  ;//	xld.w		%r5, 92				;//        } else { %r5  := '￥'	──│─┤
		ld.b		[%r2]+, %r5			;//                 *u++  = '￥'	　　│	│
		ld.b		[%r2]+, %r4			;//                 *u++  = (c >> 8)	　　│	│
		ld.b		[%r2]+, %r3			;//                 *u++  =  c		　　│	│
		jp		strescape_L20			;//        }				　　│	│
strescape_L45:							;//      } else {			　　│	│
		jreq		strescape_ADD_2			;//        if(!r) { u += 2		←─┘	│
	  ;//	xld.w		%r5, 92				;//        } else { %r5  := '￥'	────┘
		ld.b		[%r2]+, %r5			;//                 *u++  = '￥'
		ld.b		[%r2]+, %r3			;//                 *u++  =  c
		jp		strescape_L20			;//      } }
	;//}}2016/05/26変更:strescape()をマルチバイト対応しました。
		;//---------------------------------------------;//    } else
strescape_L50:
	;//{{2016/05/26追加:strescape()をマルチバイト対応しました。
		ld.w		%r9, %r3			;//    %r9  :=     c
		sra		%r9, 8				;//    %r9  :=    (c >> 8)
		jreq		strescape_L55			;//    if(        (c >> 8)) {
		cmp		%r0, 0				;//      if(!r) { u += 2
		jreq		strescape_ADD_2			;//      } else {
		ld.b		[%r2]+, %r9			;//        *u++ = (c >> 8)
		ld.b		[%r2]+, %r3			;//        *u++ =  c
		jp		strescape_L20			;//      }
		;//---------------------------------------------;//    } else
strescape_L55:
	;//}}2016/05/26追加:strescape()をマルチバイト対応しました。
		xcall.d		isprint				;//    if(isprint(c)) {
		ld.w		%r12, %r3			;//    %r12 :=    c						*delay*
		cmp		%r10, 0
		jrne		strescape_L60
		cmp		%r0, 0				;//      if(!r) { u += 4
		jreq		strescape_ADD_4			;//      } else {
		xld.w		%r9, 92				;//        %r9  := '￥'
		ld.b		[%r2]+, %r9			;//        *u++  = '￥'
		xand		%r5, %r3, 7			;//        %r5  :=        (c >> 0) & 7
		add		%r5, 48				;//        %r5  := '0' + ((c >> 0) & 7)
		sra		%r3, 3				;//        %r3  :=         c >> 3
		xand		%r4, %r3, 7			;//        %r5  :=        (c >> 3) & 7
		add		%r4, 48				;//        %r5  := '0' + ((c >> 3) & 7
		sra		%r3, 3				;//        %r3  :=         c >> 6
		xand		%r3, %r3, 3			;//        %r3  :=        (c >> 6) & 3
		add		%r3, 48				;//        %r3  := '0' + ((c >> 6) & 3
		ld.b		[%r2]+, %r3			;//        *u++  = '0' + ((c >> 6) & 3
		ld.b		[%r2]+, %r4			;//        *u++  = '0' + ((c >> 3) & 7
		ld.b		[%r2]+, %r5			;//        *u++  = '0' + ((c >> 0) & 7
		jp		strescape_L20			;//      }
		;//---------------------------------------------;//    } else {
strescape_L60:
		cmp		%r0, 0				;//      if(!r) { u += 1
		jreq		strescape_ADD_1			;//      } else {
		ld.b		[%r2]+, %r3			;//        *u++  = c
		jp		strescape_L20			;//      }
		;//---------------------------------------------;//} } }
strescape_ADD_4:
	;//{{2016/05/26変更:strescape()をマルチバイト対応しました。
	;//	add		%r2, 2				;//%r2  := u += 2 + 1 + 1
	;//↓2016/05/26変更:strescape()をマルチバイト対応しました。
		add		%r2, 1				;//%r2  := u += 1+1+1 + 1
strescape_ADD_3:
		add		%r2, 1				;//%r2  := u +=   1+1 + 1
	;//}}2016/05/26変更:strescape()をマルチバイト対応しました。
strescape_ADD_2:
		add		%r2, 1				;//%r2  := u +=     1 + 1
strescape_ADD_1:
		jp.d		strescape_L20
		add		%r2, 1				;//%r2  := u +=         1					*delay*
");
static void __attribute__((noreturn,unused))/*asmブロックから参照*/ strescape_DIE() { DIE(); }
#endif//PIECE

// * Wed May 25 10:22:02 JST 2016 Naoyuki Sawa
// - C言語版のstrcompress()をマルチバイト対応しました。
//   strcompress()をマルチバイト対応した目的は、clipyaml.cモジュールのyaml_path_get_node(),yaml_path_set_string()で使用するためです。
//   詳細は、clipyaml.cモジュールの同日のコメントを参照して下さい。
// - アセンブラ版のstrcompress()はまだ、マルチバイト対応していません。
//   アセンブラ版のstrcompress()も、マルチバイト対応する必要が有ります。	⇒{{2016/05/26コメント追記:アセンブラ版のstrcompress()もマルチバイト対応しました。}}
// - strcompress()はstrescape()と対を成す関数ですが、今回strcompress()だけをマルチバイト対応した事により、strcompress()とstrescape()の非対称性が生じました。
//   半角文字だけを使う限りは問題は生じませんが、全角文字を使用すると、strescape()でエスケープした文字列をstrcompress()で復元した時に、元の文字列にならない可能性が有ります。
//   対称性を保つために、strescape()もマルチバイト対応する必要が有ります。	⇒{{2016/05/26コメント追記:strescape()をマルチバイト対応しました。}}
// * Thu May 26 20:37:02 JST 2016 Naoyuki Sawa
// - アセンブラ版のstrcompress()もマルチバイト対応しました。
//   以下の動作確認を行いました。
//   │//下記のテスト関数から利用するサブルーチン
//   │void test_subr(const char* s, const char* delim) {
//   │  do {
//   │    char* t = strcompress(s, (char**)&s, delim);
//   │    printf("'%s'\n", t);
//   │    free(t);
//   │    s = mbsninc(s, 1);  //sが既に'\0'を指していた場合に進まないようにするためmbsninc()を使う必要が有る事に注意せよ。mbsinc()ではsが既に'\0'を指していても進んでしまう。
//   │  } while(*s);
//   │}
//   │//■マルチバイト対応になった事を確認するテスト
//   │void test1() {
//   │  test_subr("表現\\十回\\予備", "\\");
//   │  //□変更前
//   │  //│'\x95'	←変更前は、全角文字の2バイト目の'\'を間違えてエスケープの'\'と見なしてしまっていた。
//   │  //│'現'
//   │  //│'\x8F'
//   │  //│'回'
//   │  //│'\x97'
//   │  //│'備'
//   │  //□変更前
//   │  //│'表現'	←変更後は、正しくなった。
//   │  //│'十回'
//   │  //│'予備'
//   │}
//   │//■マルチバイト対応版が様々なケースで正しく動作している事を確認するテスト(以下は全て変更後の出力です)
//   │void test2() {
//   │  test_subr("", NULL);
//   │  //│''
//   │  test_subr("ﾊﾝｶｸ", NULL);
//   │  //│'ﾊﾝｶｸ'
//   │  test_subr("全角", NULL);
//   │  //│'全角'
//   │  test_subr("\\x30\\x31\\x32", NULL);
//   │  //│'012'
//   │  test_subr("\\x30\\x31\\x32", "\\");
//   │  //│''
//   │  //│'x30'
//   │  //│'x31'
//   │  //│'x32'
//   │  test_subr("あいう.えお[かき]", ".[]");
//   │  //│'あいう'
//   │  //│'えお'
//   │  //│'かき'
//   │  test_subr("あいう．えお［かき］", "．［］");		//全角文字のデリミタも利用出来るようになった。
//   │  //│'あいう'
//   │  //│'えお'
//   │  //│'かき'
//   │  test_subr("あいう\\.えお\\[かき\\]", ".[]");
//   │  //│'あいう.えお[かき]'
//   │  test_subr("あいう\\．えお\\［かき\\］", "．［］");	//全角文字のデリミタも'\'でエスケープ出来ます。
//   │  //│'あいう．えお［かき］'
//   │}
#ifndef PIECE
char* strcompress(const char* s, char** endptr, const char* delim) {
	char* r = NULL;
	for(;;) {//1周目で文字数を数え、2周目で文字を格納する
		const char *t = s, *p;
		      char *u = r;
		int c;
	//{{2016/05/25変更:strcompress()をマルチバイト対応しました。
	//	while((c = *t++) && !(delim && strchr(delim, c))) {
	//↓2016/05/25変更:strcompress()をマルチバイト対応しました。
		for(;;) {
			c = mbsnextc(t);
			t = mbsinc(t);
			if(!c) { break; }
			if(delim && mbschr(delim, c)) { break; }
	//}}2016/05/25変更:strcompress()をマルチバイト対応しました。
			if(c != '\\') { goto L_COPY; }
			/* '\'にnulが続く場合、'\'を破棄して処理2.を行い終了する。('\'にnulが続くのは'エスケープされた文字列'として不正だが、文字列の終端を超えないように最低限の対策を行うことにした)
			 * 「strchr(～,'\0')」は必ず非NULLの結果になるので、strchr()を用いる判定よりも先に行わなければならないことに注意せよ。 */
	//{{2016/05/25変更:strcompress()をマルチバイト対応しました。
	//		if(!(c = *t++)) { break; }
	//↓2016/05/25変更:strcompress()をマルチバイト対応しました。
			c = mbsnextc(t);
			t = mbsinc(t);
			if(!c) { break; }
	//}}2016/05/25変更:strcompress()をマルチバイト対応しました。
			/* '\'に続くC言語のエスケープシーケンス文字を、1文字に変換して出力する。 */
	//{{2016/05/25変更:strcompress()をマルチバイト対応しました。
	//		if((p = strchr(TBL_strescape + SIZE_strescape, c))) {	//テーブルの後半から検索
	//↓2016/05/25変更:strcompress()をマルチバイト対応しました。
			if((p = mbschr(TBL_strescape + SIZE_strescape, c))) {	//テーブルの後半から検索
	//}}2016/05/25変更:strcompress()をマルチバイト対応しました。
				if(r/*2周目*/) { *u = *(p - SIZE_strescape); }	//テーブルの前半へシフト
				u++;
			/* '\'に続く8進表記(1～3桁)を、1文字に変換して出力する。 */
			} else if((c >= '0') && (c <= '7')) {
				int x = (c - '0');
				if(((c = *t) >= '0') && (c <= '7')) {
					x = (x << 3) | (c - '0'), t++;
					if(((c = *t) >= '0') && (c <= '7')) {
						x = (x << 3) | (c - '0'), t++;
					}
				}
				if(r/*2周目*/) { *u = x; }
				u++;
			/* '\'に続く16進表記(1～∞桁)を、1文字に変換して出力する。 */
			} else if((c == 'x') || (c == 'X')) {
				int x = strtoul(t, (char**)&t, 16);
				if(r/*2周目*/) { *u = x; }
				u++;
			/* 行の継続 */
			} else if(c == '\n') {
				/** no job **/
			/* '\'に続くそれ以外の文字を、'\'を削除して文字のみに変換して出力する。('アプリケーション定義のエスケープが必要な文字'も含む) */
			} else {
L_COPY:
	//{{2016/05/25追加:strcompress()をマルチバイト対応しました。
				if(c > UCHAR_MAX) {
					if(r/*2周目*/) { *u = (c >> 8); }
					u++;
				}
	//}}2016/05/25追加:strcompress()をマルチバイト対応しました。
				if(r/*2周目*/) { *u = c; }
				u++;
			}
		}
		if(r/*2周目*/) {
	//{{2016/05/25変更:strcompress()をマルチバイト対応しました。
	//		if(endptr) { *endptr = (char*)(t - 1); }
	//↓2016/05/25変更:strcompress()をマルチバイト対応しました。
			if(endptr) { *endptr = mbsdec(s, t); }
	//}}2016/05/25変更:strcompress()をマルチバイト対応しました。
			*u = '\0';
			return r;
		}
//{{2016/09/07変更:strescape(),及び,strcompress()が、(int)でアドレス演算を行っていたのを、(intptr_t)で行うように修正しました。
//		if(!(r = malloc((int)u + 1/*nul*/))) { DIE(); }
//↓2016/09/07変更:strescape(),及び,strcompress()が、(int)でアドレス演算を行っていたのを、(intptr_t)で行うように修正しました。
		if(!(r = malloc((intptr_t)u + 1/*nul*/))) { DIE(); }
//}}2016/09/07変更:strescape(),及び,strcompress()が、(int)でアドレス演算を行っていたのを、(intptr_t)で行うように修正しました。
	}
}
#else //PIECE
char* strcompress(const char* s, char** endptr, const char* delim);
asm("
		.code
		.align		1
		.global		strcompress
strcompress:
		pushn		%r3
		xsub		%sp, %sp, 16
		xld.w		[%sp+4], %r12			;//[%sp+4]  := s
		xld.w		[%sp+8], %r13			;//[%sp+8]  := endptr
		xld.w		[%sp+12], %r14			;//[%sp+12] := delim
		ld.w		%r0, 0				;//%r0  := r = NULL
		;//---------------------------------------------;//
strcompress_L10:						;//for(;;) {
		xld.w		%r1, [%sp+4]			;//  %r1  := t = s
		ld.w		%r2, %r0			;//  %r2  := u = r
		;//---------------------------------------------;//
strcompress_L20:						;//
	;//{{2016/05/26変更:アセンブラ版のstrcompress()もマルチバイト対応しました。
	;//	ld.b		%r3, [%r1]+			;//  %r3  := c = *t++
	;//	cmp		%r3, 0				;//  if(!c || (delim && strchr(delim, c))) {
	;//	jreq		strcompress_BREAK
	;//	xld.w		%r12, [%sp+12]			;//  %r12 :=                   delim
	;//	cmp		%r12, 0
	;//	jreq		strcompress_L30
	;//	xcall.d		strchr				;//  %r10 :=            strchr(delim, c)
	;//	ld.w		%r13, %r3			;//  %r13 :=                          c				*delay*
	;//	cmp		%r10, 0
	;//	jreq		strcompress_L30
	;//↓2016/05/26変更:アセンブラ版のstrcompress()もマルチバイト対応しました。
		xcall.d		mbsnextc			;//  %r10 := c = mbsnextc(t)
		ld.w		%r12, %r1			;//  %r12 :=              t					*delay*
		ld.w		%r3, %r10			;//  %r3  := c
		xcall.d		mbsinc				;//  %r10 := t = mbsinc(t)
		ld.w		%r12, %r1			;//  %r12 :=            t					*delay*
		cmp		%r3, 0				;//  if(!c) { break }
		jreq.d		strcompress_BREAK		;//  
		ld.w		%r1, %r10			;//  %r1  := t							*delay*
		xld.w		%r12, [%sp+12]			;//  %r12 :=         delim
		cmp		%r12, 0				;//              if(!delim) { goto L30 }			!INTERLOCK!
		jreq		strcompress_L30			;//  
		xcall.d		mbschr				;//  %r10 :=  mbschr(delim, c)
		ld.w		%r13, %r3			;//  %r13 :=                c					*delay*
		cmp		%r10, 0				;//       if(!mbschr(delim, c)) { goto L30 }
		jreq		strcompress_L30			;//  
	;//}}2016/05/26変更:アセンブラ版のstrcompress()もマルチバイト対応しました。
strcompress_BREAK:
		cmp		%r0, 0				;//    if(!r) {
		jrne		strcompress_L25
		add		%r2, 1				;//      %r2  :=            u + 1
		xcall.d		malloc				;//      %r10 := r = malloc(u + 1)
		ld.w		%r12, %r2			;//      %r12 :=            u + 1				*delay*
		cmp		%r10, 0				;//      if(!r) { DIE() }
		jreq		strcompress_DIE
		jp.d		strcompress_L10
		ld.w		%r0, %r10			;//      %r0  := r						*delay*
strcompress_L25:						;//    } else {
	;//{{2016/05/26変更:アセンブラ版のstrcompress()もマルチバイト対応しました。
	;//	xld.w		%r9, [%sp+8]			;//      %r9  := endptr
	;//	cmp		%r9, 0				;//      if(endptr) {
	;//	jreq		3
	;//	 sub		%r1, 1				;//        *endptr = t - 1
	;//	 ld.w		[%r9], %r1			;//      }
	;//↓2016/05/26変更:アセンブラ版のstrcompress()もマルチバイト対応しました。
		xld.w		%r3, [%sp+8]			;//      %r3  := endptr
		cmp		%r3, 0				;//      if(endptr) {						!INTERLOCK!
		jreq		strcompress_L26
		xld.w		%r12, [%sp+4]			;//        %r12 :=          s
		xcall.d		mbsdec				;//        %r10 :=   mbsdec(s, t)
		ld.w		%r13, %r1			;//        %r13 :=             t				*delay*
		ld.w		[%r3], %r10			;//        *endptr = mbsdec(s, t)
strcompress_L26:						;//      }
	;//}}2016/05/26変更:アセンブラ版のstrcompress()もマルチバイト対応しました。
		ld.b		[%r2], %r8			;//      *u    = '￥0'
		ld.w		%r10, %r0			;//      return  r
		xadd		%sp, %sp, 16
		popn		%r3
		ret						;//    }
		;//---------------------------------------------;//  } else {
strcompress_L30:
		xcmp		%r3, 92				;//    if(c != '￥') { goto COPY }
		jrne		strcompress_COPY
	;//{{2016/05/26変更:アセンブラ版のstrcompress()もマルチバイト対応しました。
	;//	ld.b		%r3, [%r1]+			;//    %r3  := c = *t++
	;//	cmp		%r3, 0				;//    if(!c) { goto BREAK }
	;//	jreq		strcompress_BREAK
	;//↓2016/05/26変更:アセンブラ版のstrcompress()もマルチバイト対応しました。
		xcall.d		mbsnextc			;//    %r10 := c = mbsnextc(t)
		ld.w		%r12, %r1			;//    %r12 :=              t					*delay*
		ld.w		%r3, %r10			;//    %r3  := c
		xcall.d		mbsinc				;//    %r10 := t = mbsinc(t)
		ld.w		%r12, %r1			;//    %r12 :=            t					*delay*
		cmp		%r3, 0				;//    if(!c) { break }
		jreq.d		strcompress_BREAK		;//    
		ld.w		%r1, %r10			;//    %r1  := t						*delay*
	;//}}2016/05/26変更:アセンブラ版のstrcompress()もマルチバイト対応しました。
	;//{{2016/05/26変更:アセンブラ版のstrcompress()もマルチバイト対応しました。
	;//	xld.w		%r12, TBL_strescape+13		;//    %r12 :=            TBL_strescape + SIZE_strescape
	;//	xcall.d		strchr				;//    %r10 := p = strchr(TBL_strescape + SIZE_strescape, c))
	;//	ld.w		%r13, %r3			;//    %r13 :=                                            c	*delay*
	;//	cmp		%r10, 0				;//    if(    (p = strchr(TBL_strescape + SIZE_strescape, c)) {
	;//	jreq		strcompress_L40
	;//↓2016/05/26変更:アセンブラ版のstrcompress()もマルチバイト対応しました。
		xld.w		%r12, TBL_strescape+13		;//    %r12 :=            TBL_strescape + SIZE_strescape
		xcall.d		mbschr				;//    %r10 := p = mbschr(TBL_strescape + SIZE_strescape, c))
		ld.w		%r13, %r3			;//    %r13 :=                                            c	*delay*
		cmp		%r10, 0				;//    if(    (p = mbschr(TBL_strescape + SIZE_strescape, c)) {
		jreq		strcompress_L40
	;//}}2016/05/26変更:アセンブラ版のstrcompress()もマルチバイト対応しました。
		cmp		%r0, 0				;//      if(!r) { u += 1
		jreq		strcompress_ADD_1		;//      } else {
		sub		%r10, 13			;//        %r10 :=   p - SIZE_strescape
		ld.b		%r9, [%r10]			;//        %r9  := *(p - SIZE_strescape)
		ld.b		[%r2]+, %r9			;//        *u++  = *(p - SIZE_strescape)
		jp		strcompress_L20			;//      }
		;//---------------------------------------------;//    } else
strcompress_L40:
		ld.w		%r4, %r3			;//    %r4  := x = c
		sub		%r4, 48				;//    %r4  := x -= '0'
		cmp		%r4, 7				;//    if((unsigned)x <= (unsigned)7) {
		jrugt		strcompress_L50
		ld.b		%r9, [%r1]			;//      %r9  := y = *t
		sub		%r9, 48				;//      %r9  := y -= '0'
		cmp		%r9, 7				;//      if((unsigned)y <= (unsigned)7) {
		jrugt		strcompress_L45
		 sla		%r4, 3				;//        %r4  := x <<= 3
		 or		%r4, %r9			;//        %r4  := x  |= y
		 add		%r1, 1				;//        %r1  := t++
		 ld.b		%r9, [%r1]			;//        %r9  := y = *t
		 sub		%r9, 48				;//        %r9  := y -= '0'
		 cmp		%r9, 7				;//        if((unsigned)y <= (unsigned)7) {
		 jrugt		strcompress_L45
		  sla		%r4, 3				;//          %r4  := x <<= 3
		  or		%r4, %r9			;//          %r4  := x  |= y
		  add		%r1, 1				;//          %r1  := t++
strcompress_L45:						;//      } }
		cmp		%r0, 0				;//      if(!r) { u += 1
		jreq		strcompress_ADD_1		;//      } else {
		ld.b		[%r2]+, %r4			;//        *u++  = x
		jp		strcompress_L20			;//      }
		;//---------------------------------------------;//    } else
strcompress_L50:
		xand		%r9, %r3, -33			;//    if((c == 'x') || (c == 'X')) {
		xcmp		%r9, 88
		jrne		strcompress_L60
		cmp		%r0, 0				;//      if(!r) { u += 1
		jreq		strcompress_ADD_1		;//      } else {
		ld.w		%r12, %r1			;//        %r12 :=             t
		ld.w		%r13, %sp			;//        %r13 :=                &t
		xld.w		[%sp+0], %r1
		xcall.d		strtoul				;//        %r10 := x = strtoul(t, &t, 16)
		ld.w		%r14, 16			;//        %r14 :=                    16			*delay*
		xld.w		%r1, [%sp+0]			;//        %r1  :=                 t
		ld.b		[%r2]+, %r10			;//        *u++  = x
		jp		strcompress_L20			;//      }
		;//---------------------------------------------;//    } else
strcompress_L60:
		cmp		%r3, 10				;//    if(c == '￥n') {
		jreq		strcompress_L20			;//      /** no job **/
		;//---------------------------------------------;//    } else {
strcompress_COPY:
	;//{{2016/05/26追加:アセンブラ版のstrcompress()もマルチバイト対応しました。
		ld.w		%r9, %r3			;//      %r9  :=   c
		sra		%r9, 8				;//      %r9  :=  (c >> 8)
		jreq		5				;//      if(      (c >> 8)) {
		 cmp		%r0, 0				;//        if(r) {
		 jreq		2				;//          *u = (c >> 8)
		  ld.b		[%r2], %r9			;//        }
		 add		%r2, 1				;//        %r2  := u += 1   }
	;//}}2016/05/26追加:アセンブラ版のstrcompress()もマルチバイト対応しました。
		cmp		%r0, 0				;//      if(!r) { u += 1
		jreq		strcompress_ADD_1		;//      } else {
		ld.b		[%r2]+, %r3			;//        *u++  = c
		jp		strcompress_L20			;//      }
		;//---------------------------------------------;//} } }
strcompress_ADD_1:
		jp.d		strcompress_L20
		add		%r2, 1				;//%r2  := u += 1						*delay*
");
static void __attribute__((noreturn,unused))/*asmブロックから参照*/ strcompress_DIE() { DIE(); }
#endif//PIECE

/*****************************************************************************
 *	文字列ポインタ配列
 *****************************************************************************/

#ifndef PIECE
void strfreev(char** str_array) {
	/* str_array自体がNULLポインタでなければ… */
	if(str_array) {
		char **p = str_array, *str;
		/* NULLポインタ要素に到達するまで、文字列を開放する。 */
		while((str = *p++)) { free(str); }
		/* str_array自体を開放する。 */
		free(str_array);
	}
}
#else /*PIECE*/
void strfreev(char** str_array);
//素直なバージョン(16命令)と、4命令節約バージョン(12命令)を作成しました。サイズを優先して後者を使用することにしました。
//strfreev()を使用するような処理は、速度があまり重要でないことが多く、少しでも小さい方が良いと思ったからです。
#if 0
//ほぼC言語どおり素直に実装したバージョンです。
asm("
		.code
		.align		1
		.global		strfreev
strfreev:
		cmp		%r12, 0			;//if(!str_array) { goto L30 }
		jreq		strfreev_L30
		pushn		%r1
		ld.w		%r0, %r12		;//%r0  :=     str_array
		ld.w		%r1, %r12		;//%r1  := p = str_array
strfreev_L10:
		ld.w		%r12, [%r1]+		;//while((str = *p++)) {
		cmp		%r12, 0
		jreq		strfreev_L20
		xcall		free			;// free(str)
		jp		strfreev_L10		;//}
strfreev_L20:
		ld.w		%r12, %r0		;//%r12 := str_array
		xcall		free			;//   free(str_array)
		popn		%r1
strfreev_L30:
		ret
");
#else
//4命令節約したバージョン。ただし少し低速です。
asm("
		.code
		.align		1
		.global		strfreev
strfreev:
		cmp		%r12, 0			;//if(!str_array) { goto L30 }
		jreq		strfreev_L30
		pushn		%r12
		ld.w		%r0, %r12		;//%r0  := p = str_array
strfreev_L10:
		ld.w		%r12, [%r0]+		;//while((str = *p++)) {
		cmp		%r12, 0
		jreq		strfreev_L20
		call		strfreev_L30		;// free(str)		「xcall free」と書くよりも、この方がext命令1個節約できる。
		jp		strfreev_L10		;//}
strfreev_L20:
		popn		%r12
strfreev_L30:
		xjp		free			;//free(str_array)	(!str_array)の場合も実行されるが、free(NULL)は安全である。
");
#endif
#endif/*PIECE*/

#ifndef PIECE
size_t strv_length(char** str_array) {
	/* NULLポインタ要素に対する(+1)を相殺するため、初期値を(-1)としておく。 */
	int n = -1;
	/* NULLポインタ要素も含めて、要素数を(+1)する。NULLポインタ要素に対する(+1)は、初期値の(-1)と相殺される。 */
	do { n++; } while(*str_array++);
	/* 要素数を返す。 */
	return n;
}
#else /*PIECE*/
size_t strv_length(char** str_array);
asm("
		.code
		.align		1
		.global		strv_length
strv_length:
		ld.w		%r10, -1		;//%r10 := n = -1
		ld.w		%r9, [%r12]+		;//%r9  := str = *str_array++
		 cmp		%r9, 0			;//    if(!str) { break }
		jrne.d		-2
		add		%r10, 1			;//%r10 := n++			*delay*
		ret
");
#endif/*PIECE*/

#ifndef PIECE
char** strdupv(char** str_array) {
	char **p, **q, *r;
	/* 文字列ポインタ配列の(要素数+1(終端のNULL))の、メモリを確保する。
	 * calloc()を使用して確保するので、終端のNULLは既に設定されている。 */
	p = q = calloc(strv_length(str_array) + 1/*終端のNULL*/, sizeof(char*));
	if(!p) { DIE(); }
	goto L_START;
	do {
		/* 文字列を複製する。 */
		r = strdup(r);
		if(!r) { DIE(); }
		/* 複製した文字列を格納する。 */
		*q++ = r;
		/* 文字列を取得する。終端ならば、抜ける。 */
L_START:	r = *str_array++;
	} while(r);
	/* 複製した文字列ポインタ配列を返す。 */
	return p;
}
#else //PIECE
asm("
		.code
		.align		1
		.global		strdupv
strdupv:
		pushn		%r2
		xcall.d		strv_length		;//%r10 := n = strv_length(str_array)
		ld.w		%r0, %r12		;//%r0  := str_array			*delay*
		add		%r10, 1			;//%r12 := n++
		ld.w		%r12, %r10		;//%r12 :=            n
		xcall.d		calloc			;//%r10 := p = calloc(n, sizeof(char*))
		ld.w		%r13, 4			;//%r13 :=               sizeof(char*)	*delay*
		cmp		%r10, 0			;//if(!p) { DIE() }
		jreq		strdupv_DIE		;//
		ld.w		%r1, %r10		;//%r1  := p
		jp.d		strdupv_START		;//goto START
		ld.w		%r2, %r10		;//%r2  := q = p			*delay*
strdupv_LOOP:						;//do {
		xcall		strdup			;//  %r10 := r = strdup(r)
		cmp		%r10, 0			;//  if(!r) { DIE() }
		jreq		strdupv_DIE		;//  
		ld.w		[%r2]+, %r10		;//  *q++  = r
strdupv_START:						;//  
		ld.w		%r12, [%r0]+		;//  %r12 := r = *str_array++
		cmp		%r12, 0			;//  
		jrne		strdupv_LOOP		;//} while(r)
		ld.w		%r10, %r1		;//%r10 := p
		popn		%r2
		ret
");
static void __attribute__((noreturn,unused))/*asmブロックから参照*/ strdupv_DIE() { DIE(); }
#endif//PIECE

/****************************************************************************
 *	書式化文字列
 ****************************************************************************/

#ifndef PIECE
char* strdup_printf(const char* fmt, ...) {
	char* str;
	va_list ap;
	va_start(ap, fmt);
	str = strdup_vprintf(fmt, ap);
	va_end(ap);
	return str;
}
char* strdup_vprintf(const char* fmt, va_list ap) {
	char* str;
	if(vasprintf(&str, fmt, ap) == -1) { DIE(); }
	return str;
}
#else /*PIECE*/
asm("
		.code
		.align		1
		.global		strdup_printf
		.global		strdup_vprintf
		;//[%sp+0] := retp
		;//[%sp+4] := fmt
		;//[%sp+8] := ...
strdup_printf:
		xld.w		%r12, [%sp+4]		;//%r12 := fmt
		xadd		%r13, %sp, 8		;//%r13 := ap
strdup_vprintf:
		xsub		%sp, %sp, 4
		ld.w		%r14, %r13		;//%r14 :=                               ap
		ld.w		%r13, %r12		;//%r13 :=                          fmt
		ld.w		%r12, %sp		;//%r12 :=                    &str
		xcall		vasprintf		;//%r10 := retval = vasprintf(&str, fmt, ap)
		cmp		%r10, -1		;//if(retval == -1) { DIE() }
		jreq		strdup_vprintf_DIE
		xld.w		%r10, [%sp+0]		;//%r10 := str
		xadd		%sp, %sp, 4
		ret					;//return  str
");
static void __attribute__((noreturn,unused))/*asmブロックから参照*/ strdup_vprintf_DIE() { DIE(); }
#endif//PIECE

/*****************************************************************************
 *	連結
 *****************************************************************************/

#ifndef PIECE
char* strconcat(const char* s, ...) {
	va_list ap;
	char *newStr, *oldStr;
	/* 文字列の初期値を、""とする。 */
	if(!(newStr = strdup(""))) { DIE(); }
	/* 引数s,及び,可変引数が、NULLポインタ要素に到達するまで… */
	va_start(ap, s);
	while(s) {	//引数sがNULLだった場合は、ループに入らずに抜ける。
		/* 古い文字列と、要素の文字列を連結し、新しい文字列とする。 */
		newStr = strjoin(NULL, (oldStr = newStr), s, NULL);
		/* 古い文字列を開放する。 */
		free(oldStr);
		/* 次の可変引数を取得する。 */
		s = va_arg(ap, char*);
	}
	va_end(ap);
	/* 連結した文字列を返す。 */
	return newStr;
}
char* strjoin(const char* separator, ...) {
	va_list ap;
	char *newStr, *oldStr, *str;
	/* 一要素目に先行するセパレータは""固定とする。 */
	const char* sep = "";
	/* 文字列の初期値を、""とする。 */
	if(!(newStr = strdup(""))) { DIE(); }
	/* 可変引数が、NULLポインタ要素に到達するまで… */
	va_start(ap, separator);
	while((str = va_arg(ap, char*))) {
		/* 古い文字列と、セパレータと、要素の文字列を連結し、新しい文字列とする。 */
		newStr = strdup_printf("%s%s%s", (oldStr = newStr), sep, str);
		/* 古い文字列を開放する。 */
		free(oldStr);
		/* セパレータが指定されていたら、二要素目以降に先行するセパレータとする。 */
		if(separator) { sep = separator; }
	}
	va_end(ap);
	/* 連結した文字列を返す。 */
	return newStr;
}
char* strjoinv(const char* separator, char** str_array) {
	char *newStr, *oldStr, *str;
	/* 一要素目に先行するセパレータは""固定とする。 */
	const char* sep = "";
	/* 文字列の初期値を、""とする。 */
	if(!(newStr = strdup(""))) { DIE(); }
	/* 文字列ポインタ配列が、NULLポインタ要素に到達するまで… */
	while((str = *str_array++)) {
		/* 古い文字列と、セパレータと、要素の文字列を連結し、新しい文字列とする。 */
		newStr = strdup_printf("%s%s%s", (oldStr = newStr), sep, str);
		/* 古い文字列を開放する。 */
		free(oldStr);
		/* セパレータが指定されていたら、二要素目以降に先行するセパレータとする。 */
		if(separator) { sep = separator; }
	}
	/* 連結した文字列を返す。 */
	return newStr;
}
#else /*PIECE*/
char* strconcat(const char* s, ...);
char* strjoin(const char* separator, ...);
char* strjoinv(const char* separator, char** str_array);
asm("
		.code
		.align		1
		.global		strconcat
		.global		strjoin
		.global		strjoinv
strconcat:
		;//[%sp+ 0] := retp
		;//[%sp+ 4] := s
		;//[%sp+ 8] := ...
		xadd		%r13, %sp, 4			;//%r13 := str_array = &{s,...}		//sをstr_arrayの先頭要素とすることに注意せよ。
		jp.d		strjoinv
		ld.w		%r12, 0				;//%r12 := separator = NULL		*delay*
strjoin:
		;//[%sp+ 0] := retp
		;//[%sp+ 4] := separator
		;//[%sp+ 8] := ...
		xld.w		%r12, [%sp+4]			;//%r12 := separator
		xadd		%r13, %sp, 8			;//%r13 := str_array = &...
strjoinv:
		pushn		%r1
		xsub		%sp, %sp, 16
		ld.w		%r0, %r12			;//%r0      := separator
		ld.w		%r1, %r13			;//%r1      := str_array
		xld.w		%r12, strjoinv_FMT		;//%r12     := “%s%s%s”	【注意】半角の二重引用符を使うとasmブロックの文字列終端と見なされるので使用不可。
		xld.w		[%sp+ 0], %r12			;//[%sp+ 0] := “%s%s%s”
		add		%r12, 6				;//%r12     :=       “”
		xld.w		[%sp+ 8], %r12			;//[%sp+ 8] := sep = “”
		xcall		strdup				;//%r10     := newStr = strdup(“”)
		cmp		%r10, 0				;//if(!newStr) { DIE() }
		jreq		strjoinv_DIE
		xld.w		[%sp+ 4], %r10			;//[%sp+ 4] := oldStr = newStr			最初のループのoldStr
strjoinv_L10:	;//---------------------------------------------;//for(;;) {
		;//%r0      := separator
		;//%r1      := str_array
		;//%r10     := newStr
		;//[%sp+ 0] := “%s%s%s”	strdup_printfへの引数
		;//[%sp+ 4] := oldStr		strdup_printfへの引数
		;//[%sp+ 8] := sep		strdup_printfへの引数
		;//[%sp+12] := str		strdup_printfへの引数
		ld.w		%r9, [%r1]+			;//  %r9      := str = *str_array++
		cmp		%r9, 0
		jreq		strjoinv_L20
		xld.w		[%sp+12], %r9			;//  [%sp+12] := str
		xcall		strdup_printf			;//  %r10     := newStr = strdup_printf(“%s%s%s”, oldStr, sep, str)
		xld.w		%r12, [%sp+ 4]			;//  %r12     := oldStr		────┐
		xld.w		[%sp+ 4], %r10			;//  [%sp+ 4] := oldStr = newStr	│	次回のループのoldStr
		xcall		free				;//         free(oldStr)	←───┘
		cmp		%r0, 0				;//  if(separator) { sep = separator }
		jreq		strjoinv_L10
		xld.w		[%sp+ 8], %r0
		jp		strjoinv_L10
strjoinv_L20:	;//---------------------------------------------;//}
		xld.w		%r10, [%sp+ 4]			;//%r10     := newStr
		xadd		%sp, %sp, 16
		popn		%r1
		ret
");
static void __attribute__((noreturn,unused))/*asmブロックから参照*/ strjoinv_DIE() { DIE(); }
static const char strjoinv_FMT[] = "%s%s%s";
#endif/*PIECE*/

/****************************************************************************
 *	分割
 ****************************************************************************/

//───────────────────────────────────────────────────────────────────────────────────────────────────
//* Sun Mar 08 10:55:52 JST 2015 Naoyuki Sawa
//- strsplit_set()とビットマップGCを併用し、ある程度大きなファイルを分割しようとすると、ビットマップGCがエラー停止する事が有ります。
//  この問題について調査した結果は、以下の通りです。
//□再現方法
//│//予め｢isd.exe =w \Home\Share\Piece\clip\getline.c｣等としてP/ECEに何かﾃｷｽﾄを転送しておく｡ある程度大きなﾌｧｲﾙで､改行文字がCR+LFだと問題が再現し易い｡
//│FILE* fp = fopen("getline.c","r");
//│char* s = NULL;
//│int len = 0;
//│getdelim(&s,&len,0,fp);
//│//ﾋﾞｯﾄﾏｯﾌﾟGCを起動する｡もっと早く起動しておいても同じ結果｡ﾕﾆｯﾄは関係無い｡
//│BitmapGC_Init(3);
//│//CR,又は,LFを分割文字としてｽﾌﾟﾘｯﾄすると､clipbmgc.cのnew_pceHeapAlloc()内でｴﾗｰ停止する｡
//│strsplit_set(s,"\r\n",0);
//□原因
//│これはﾊﾞｸﾞではなく､strsplit_set()のﾒﾓﾘ確保ﾊﾟﾀｰﾝと､ﾋﾞｯﾄﾏｯﾌﾟGCの特性のために､ﾒﾓﾘﾌﾗｸﾞﾒﾝﾃｰｼｮﾝが発生して､大きな空きﾒﾓﾘﾌﾞﾛｯｸが確保できなくなってしまう事が有るのが原因です｡
//│具体的には､strsplit_set()から呼び出される､strsplit_sub()の中で…
//│①｢文字列ﾎﾟｲﾝﾀ配列を拡張する｡｣処理で､1要素のargvを確保する｡⇒ﾒﾓﾘﾌﾞﾛｯｸ割り当て
//│②argv[0]に､最初の'\r'までの文字列を複製して格納する｡⇒ﾒﾓﾘﾌﾞﾛｯｸ割り当て
//│③｢文字列ﾎﾟｲﾝﾀ配列を拡張する｡｣処理で､2要素のargvに拡張する｡この時､①の後ろに②が割り当て済みなのでその場でreallocできずfree+mallocになる｡ﾋﾞｯﾄﾏｯﾌﾟGCはfreeを保留してmallocのみ行う｡⇒ﾒﾓﾘﾌﾞﾛｯｸ割り当て
//│④argv[1]に､'\r'と'\n'の間の文字列,即ち,空文字列を複製して格納する｡⇒ﾒﾓﾘﾌﾞﾛｯｸ割り当て
//│⑤各行について①～④の処理を繰り返す｡
//│⑥④のために実際の行数の二倍の細かな分割が行われて､かなり早く空きﾒﾓﾘが無くなる｡すると､ﾋﾞｯﾄﾏｯﾌﾟGCが起動する｡
//│⑦②と④で複製した文字列は開放できない｡①と③で確保したargvのうち､最後に確保したもの以外が開放される｡
//│⑧しかし､それによって開いたﾒﾓﾘは､現時点のargv[argc]よりも少ないよう素数のﾒﾓﾘﾌﾞﾛｯｸなので､新しくargv[argc]を確保するには小さ過ぎる｡
//│⑨結果として､clipbmgc.cのnew_pceHeapAlloc()の2nd tryが失敗して､ｴﾗｰ停止する｡
//□考察
//│この問題を回避するには､argvをまとめて拡張するとか､時々ﾋﾞｯﾄﾏｯﾌﾟGCを起動して早めに不要なargvのﾒﾓﾘを開放して､そのｽﾍﾟｰｽに文字列が複製されるようにするとか､対策方法が無くはないが､対策は行わない事にした｡
//│前述のとおり､strsplit_set()のﾒﾓﾘ確保ﾊﾟﾀｰﾝと､ﾋﾞｯﾄﾏｯﾌﾟGCの特性による'非効率性'が原因であり､ﾊﾞｸﾞではないからです｡
//│特定の用途に合わせて対策を行うと､strsplit_set()やﾋﾞｯﾄﾏｯﾌﾟGCの単純さを失う事になり､その方が害が大きいと思ったからです｡
//│ﾗｲﾌﾞﾗﾘ側での対策は行いません｡ｱﾌﾟﾘｹｰｼｮﾝが､下記の暫定対策を行って回避してください｡
//□暫定対策
//│大きなﾃｷｽﾄ(特に､分割すると空文字列がたくさん生じるような分割ﾊﾟﾀｰﾝ)の分割には､strsplit_set()を使用せず､strtok()やstrsep()等を使用してください｡
//───────────────────────────────────────────────────────────────────────────────────────────────────
// * Sat May 21 22:36:02 JST 2016 Naoyuki Sawa
// - strsplit()とstrsplit_set()をマルチバイト対応しました。
//   古いコードでは配列のメモリ確保を関数内で行っていましたが、新しいコードではstrv.cの機能を使うように変更し、コードが簡潔になりました。
//   コードが簡潔になったことによって、マルチバイト対応を追加したにもかかわらず、当モジュールのコードサイズが約24バイト小さくなりました。
//   ただし、strv.oもリンクされるようになった分コードサイズが増加するので、全体としてはコードサイズは増加していると思います。
// - 新しいコードで実装を大幅に変更しましたが、基本的なアルゴリズムは古いコードと変わっていません。
//   新しいコードには詳細なコメントを付けていないので、アルゴリズムについては古いコードのコメントを参照して下さい。
// - マルチバイト非対応の古いコードも、これまで長く安定して動作していた実績が有るので、コードを残しておく事にしました。
//   USE_STRSPLIT_MBCSシンボルを定義してライブラリをビルドすると、新しいマルチバイト対応版のstrsplit()とstrsplit_set()を使用します。
//   USE_STRSPLIT_MBCSシンボルを定義せずにライブラリをビルドすると、古いマルチバイト非対応版のstrsplit()とstrsplit_set()を使用します。
//   基本的には、新しいマルチバイト対応版のstrsplit()とstrsplit_set()を使う予定ですが、もし今後マルチバイト対応が不要になった場合は、古いコードに戻して下さい。
// - ■検証プログラム
//   │void test1() {
//   │  char** s = strsplit("表現\\十回\\予備", "\\", 0);
//   │  char** t = s;
//   │  while(*t) { printf("'%s'\n", *t++); }
//   │  strfreev(s);
//   │}
//   │//□結果(マルチバイト非対応版)…誤り
//   │//│'\x95'  「表」の2バイト目に'\'がマッチしてしまっている。
//   │//│'現'
//   │//│'\x8F'  「十」の2バイト目に'\'がマッチしてしまっている。
//   │//│'回'
//   │//│'\x97'  「予」の2バイト目に'\'がマッチしてしまっている。
//   │//│'備'
//   │//□結果(マルチバイト対応版)…正しい
//   │//│'表現'
//   │//│'十回'
//   │//│'予備'
//   │void test2() {
//   │  char** s = strsplit_set("表現\\十回\\予備", "表十予", 0);
//   │  char** t = s;
//   │  while(*t) { printf("'%s'\n", *t++); }
//   │  strfreev(s);
//   │}
//   │//□結果(マルチバイト非対応版)…誤り
//   │//│''      「表」の1バイト目が個別に「表」の1バイト目にマッチしてしまっている。
//   │//│''      「表」の2バイト目が個別に「表」の2バイト目にマッチしてしまっている。
//   │//│'現'    「表」又は「十」又は「予」の2バイト目が'\'にマッチしてしまっている。
//   │//│''      「十」の1バイト目が個別に「十」の1バイト目にマッチしてしまっている。
//   │//│''      「十」の2バイト目が個別に「十」の2バイト目にマッチしてしまっている。
//   │//│'回'    「表」又は「十」又は「予」の2バイト目が'\'にマッチしてしまっている。
//   │//│''      「予」の1バイト目が個別に「予」の1バイト目にマッチしてしまっている。
//   │//│''      「予」の2バイト目が個別に「予」の2バイト目にマッチしてしまっている。
//   │//│'備'    「表」又は「十」又は「予」の2バイト目が'\'にマッチしてしまっている。
//   │//□結果(マルチバイト対応版)…正しい
//   │//│''
//   │//│'現\'
//   │//│'回\'
//   │//│'備'
#define USE_STRSPLIT_MBCS	//このシンボルを定義するとマルチバイト対応版のstrsplit()とstrsplit_set()を使用します。このシンボルを定義しないとマルチバイト非対応版のstrsplit()とstrsplit_set()を使用します。
//‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥
//■マルチバイト非対応版のstrsplit()とstrsplit_set()	2014/10～2016/05まで、安定して動作していたコードです。
#ifndef USE_STRSPLIT_MBCS
#ifndef PIECE
static char** strsplit_sub(const char* s, const char* delimiter,  int max_tokens, int delimiter_len);
       char** strsplit(    const char* s, const char* delimiter,  int max_tokens) { return strsplit_sub(s, delimiter,  max_tokens, 0); }
       char** strsplit_set(const char* s, const char* delimiters, int max_tokens) { return strsplit_sub(s, delimiters, max_tokens, 1); }
static char** strsplit_sub(const char* s, const char* delimiter,  int max_tokens, int delimiter_len) {
	int    argc = 0;
	char **argv = NULL, *p;
	if(*s) {
		char* (*fn)(const char*, const char*) = strpbrk; /* デリミタ検索関数 */
		/* strsplit()    から呼び出された時、(delimiter_len=0)である。ここでデリミタ文字列の実際の文字数を数える。(delimiter=""は不可)
		 * strsplit_set()から呼び出された時、(delimiter_len=1)である。個々のデリミタは一文字と見なすのでそのまま。(delimiter=""も可能) */
		if(!delimiter_len) {
			if(!(delimiter_len = strlen(delimiter))) { DIE(); } /* 「strsplit(s,"",max_tokens)」は不可。「strsplit_set(s,"",max_tokens)」は可能。 */
			fn = strstr; /* デリミタ検索関数 */
		}
		/* 空文字列が指定された場合の結果は、終端のNULLポインタのみから成る空の文字列ポインタ配列とする。
		 * - As a special case, the result of splitting the empty string "" is an empty vector, not a vector containing a single string.
		 *   The reason for this special case is that being able to represent a empty vector is typically more useful than consistent handling of empty elements.
		 *   If you do need to represent empty elements, you'll need to check for the empty string before calling g_strsplit(). */
		for(;;) {
			/* 文字列ポインタ配列を拡張する。 */
			if(!(argv = realloc(argv, sizeof(char*) * (argc + 1)))) { DIE(); }
			/* 分割数の最終ならば、デリミタが見つからなかったのと同じとする。
			 * - The maximum number of pieces to split string into. If this is less than 1, the string is split completely. */
			if((max_tokens >= 1) && (--max_tokens == 0)) { break; }
			/* デリミタが見つからなければ、抜ける。 */
			if(!(p = (*fn)(s, delimiter))) { break; }
			/* 現在位置からデリミタの直前までの文字列を、複製して文字列ポインタ配列に格納する。
			 * - 文字列の先頭にデリミタが有った場合、最初に格納する要素は空文字列となる。 */
			if(!(argv[argc++] = strndup(s, p - s))) { DIE(); }
			/* 現在位置を、デリミタの直後へ進める。 */
			s = p + delimiter_len;
		}
		/* 現在位置から文字列の終端までの文字列を、複製して文字列ポインタ配列に格納する。
		 * - 文字列の末尾にデリミタが有った場合、最後に格納する要素は空文字列となる。 */
		if(!(argv[argc++] = strdup(s))) { DIE(); }
	}
	/* 文字列ポインタ配列の終端として、NULLポインタを格納する。 */
	if(!(argv = realloc(argv, sizeof(char*) * (argc + 1)))) { DIE(); }
	argv[argc] = NULL;
	return argv;
}
#else /*PIECE*/
asm("
		.code
		.align		1
		.global		strsplit
		.global		strsplit_set
strsplit:
		jp.d		strsplit_sub
		ld.w		%r15, 0			;//%r15    := delimiter_len = 0					*delay*
strsplit_set:
		ld.w		%r15, 1			;//%r15    := delimiter_len = 1
strsplit_sub:
		pushn		%r3
		xsub		%sp, %sp, 12
		ld.w		%r0, 0			;//%r0     := argv = NULL
		ld.w		%r1, 0			;//%r1     := size = 0
		ld.b		%r9, [%r12]		;//%r9     := *s
		cmp		%r9, 0			;//if(!(*s)) { goto strsplit_sub_L40 }
		jreq		strsplit_sub_L40
		ld.w		%r2, %r12		;//%r2     := s
		ld.w		%r3, %r14		;//%r3     := max_tokens
		xld.w		[%sp+0], %r13		;//[%sp+0] := delimiter
		xld.w		%r9, strpbrk		;//%r9     := fn = strpbrk
		cmp		%r15, 0			;//if(!delimiter_len) {
		jrne		strsplit_sub_L10
		ld.w		%r12, %r13		;//  %r12  :=                        delimiter
		xcall		strlen			;//  %r10  := delimiter_len = strlen(delimiter)
		cmp		%r10, 0			;//  if(!delimiter_len) { DIE() }
		jreq		strsplit_sub_DIE
		ld.w		%r15, %r10		;//  %r15  := delimiter_len
		xld.w		%r9, strstr		;//  %r9   := fn = strstr
strsplit_sub_L10:					;//}
		xld.w		[%sp+4], %r15		;//[%sp+4] := delimiter_len
		xld.w		[%sp+8], %r9		;//[%sp+8] := fn
strsplit_sub_L20:					;//for(;;) {
		ld.w		%r12, %r0		;//  %r12    :=                argv
		ld.w		%r13, %r1		;//  %r13    :=                      size
		xcall.d		realloc			;//  %r10    := argv = realloc(argv, size + sizeof(char*))
		add		%r13, 4			;//  %r13    :=                      size + sizeof(char*)	*delay*
		cmp		%r10, 0			;//  if(!argv) { DIE() }
		jreq		strsplit_sub_DIE
		ld.w		%r0, %r10		;//  %r0     := argv
		cmp		%r3, 1			;//  if((max_tokens >= 1) &&
		jrlt		3
		 sub		%r3, 1			;//   (--max_tokens == 0)) { break }
		 jreq		strsplit_sub_L30
		xld.w		%r9, [%sp+8]		;//  %r9    :=       fn
		xld.w		%r13, [%sp+0]		;//  %r13   :=              delimiter
		call.d		%r9			;//  %r10   := p = (*fn)(s, delimiter)
		ld.w		%r12, %r2		;//  %r12   :=           s					*delay*
		cmp		%r10, 0			;//  if(!p) { break }
		jreq		strsplit_sub_L30
		ld.w		%r12, %r2		;//  %r12   :=             s
		ld.w		%r13, %r10		;//  %r13   :=                p
		sub		%r13, %r2		;//  %r13   :=                p - s
		xld.w		%r2, [%sp+4]		;//  %r2    :=         delimiter_len
		xcall.d		strndup			;//  %r10   := t = strndup(s, p - s)
		add		%r2, %r10		;//  %r2    := s = p + delimiter_len				*delay*
		cmp		%r10, 0			;//  if(!t) { DIE() }
		jreq		strsplit_sub_DIE
		ld.w		%r9, %r0		;//  %r9    := argv
		add		%r9, %r1		;//  %r9    := argv[argc]
		ld.w		[%r9], %r10		;//  argv[argc] = t
		jp.d		strsplit_sub_L20	;//}
		add		%r1, 4			;//  %r1    := size += sizeof(char*)				*delay*
strsplit_sub_L30:
		xcall.d		strdup			;//%r10     := strdup(s)
		ld.w		%r12, %r2		;//%r12     :=        s						*delay*
		ld.w		%r9, %r0		;//%r9      := argv
		add		%r9, %r1		;//%r9      := argv[argc]
		ld.w		[%r9], %r10		;//argv[argc] = t
		add		%r1, 4			;//%r1      := size += sizeof(char*)
strsplit_sub_L40:
		ld.w		%r12, %r0		;//%r12      :=                argv
		ld.w		%r13, %r1		;//%r13      :=                      size
		xcall.d		realloc			;//%r10      := argv = realloc(argv, size + sizeof(char*))
		add		%r13, 4			;//%r13      :=                      size + sizeof(char*)	*delay*
		cmp		%r10, 0			;//if(!argv) { DIE() }
		jreq		strsplit_sub_DIE
		ld.w		%r9, %r10		;//%r9      := argv
		add		%r9, %r1		;//%r9      := argv[argc]
		ld.w		[%r9], %r8		;//argv[argc] = NULL
		xadd		%sp, %sp, 12
		popn		%r3
		ret
");
static void __attribute__((noreturn,unused))/*asmブロックから参照*/ strsplit_sub_DIE() { DIE(); }
#endif/*PIECE*/
//‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥
#else //USE_STRSPLIT_MBCS
//‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥
//■マルチバイト対応版のstrsplit()とstrsplit_set()	2016/05～使用開始するコードです。
#ifndef PIECE
char** strsplit(const char* s, const char* delimiter,  int max_tokens) {
	char **v, *p, *t;
	if(!*delimiter) { DIE(); }	//「strsplit(s,"",max_tokens)」は不可。
	v = strv_new(NULL);
	if(*s) {
		while(--max_tokens) {	//厳密には「while((max_tokens<1)||(--max_tokens)){～」だが、トークン数が(2^31)個以上になる事はまず無いので左記のように簡略化して問題無いだろう。
			if(!(p = mbsstr(s, delimiter))) { break; }	//←strsplit()とstrsplit_set()とで異なるのはこの行です。アセンブラ版では(fn1)に相当します。
			if(!(t = strndup(s, p - s))) { DIE(); }
			strv_push(&v, t);
			s = p + strlen(delimiter);			//←strsplit()とstrsplit_set()とで異なるのはこの行です。アセンブラ版では(fn2)に相当します。
		}
		strv_extend(&v, s);
	}
	return v;
}
char** strsplit_set(const char* s, const char* delimiters, int max_tokens) {
	char **v, *p, *t;
//不要	if(!*delimiters) { DIE(); }	//「strsplit_set(s,"",max_tokens)」は可能。
	v = strv_new(NULL);
	if(*s) {
		while(--max_tokens) {	//厳密には「while((max_tokens<1)||(--max_tokens)){～」だが、トークン数が(2^31)個以上になる事はまず無いので左記のように簡略化して問題無いだろう。
			if(!(p = mbspbrk(s, delimiters))) { break; }	//←strsplit()とstrsplit_set()とで異なるのはこの行です。アセンブラ版では(fn1)に相当します。
			if(!(t = strndup(s, p - s))) { DIE(); }
			strv_push(&v, t);
			s = mbsinc(p);					//←strsplit()とstrsplit_set()とで異なるのはこの行です。アセンブラ版では(fn2)に相当します。
		}
		strv_extend(&v, s);
	}
	return v;
}
#else //PIECE
char** strsplit(const char* s, const char* delimiter,  int max_tokens);
char** strsplit_set(const char* s, const char* delimiters, int max_tokens);
asm("
		.code
		.align		1
		.global		strsplit
		.global		strsplit_set
strsplit:
		ld.b		%r9, [%r13]			;//%r9     := *delimiter
		cmp		%r9, 0				;//if(       !*delimiter) { DIE() }
		jreq		strsplit_DIE			;//
		;//- - - - - - - - - - - - - - - - - - - - - - -;//
		xld.w		%r9, strsplit_vtbl		;//%r9     := vtbl = strsplit_vtbl
		xjp		strsplit_START			;//goto START
		;//---------------------------------------------;//
strsplit_set:
		xld.w		%r9, strsplit_set_vtbl		;//%r9     := vtbl = strsplit_set_vtbl
		;//- - - - - - - - - - - - - - - - - - - - - - -;//
strsplit_START:
		pushn		%r3				;//
		xsub		%sp, %sp, 8			;//
		xld.w		[%sp+4], %r9			;//[%sp+4] := vtbl
		ld.w		%r0, %r12			;//%r0     := s
		ld.w		%r1, %r13			;//%r1     := delimiter
	;//	ld.w		%r2, %r14			;//%r2     := max_tokens	────┐
		xld.w		[%sp+0], %r8			;//[%sp+0] :=              NULL		│
		xcall.d		strv_new			;//%r10    := v = strv_new(NULL)	│
		ld.w		%r2, %r14			;//%r2     := max_tokens	←───┘	*delay*
		xld.w		[%sp+0], %r10			;//[%sp+0] := v
		;//%r0     := s
		;//%r1     := delimiter
		;//%r2     := max_tokens
		;//[%sp+0] := v
		;//[%sp+4] := vtbl
		ld.b		%r9, [%r0]			;//%r9     := *s
		cmp		%r9, 0				;//if(        *s) {				!INTERLOCK!
		jreq		strsplit_RET			;//  
strsplit_LOOP:							;//  while(--max_tokens) {
		sub		%r2, 1				;//    
		jreq		strsplit_BREAK			;//    
		;//- - - - - - - - - - - - - - - - - - - - - - -;//
		xld.w		%r9, [%sp+4]			;//    %r9     :=       fn1
		ld.w		%r12, %r0			;//    %r12    :=            s
		call.d		%r9				;//    %r10    := p = (*fn1)(s, delimiter)
		ld.w		%r13, %r1			;//    %r13    :=               delimiter	*delay*
		cmp		%r10, 0				;//    if(       !p) { break }
		jreq		strsplit_BREAK			;//    
		ld.w		%r3, %r10			;//    %r3     := p
		;//- - - - - - - - - - - - - - - - - - - - - - -;//
		ld.w		%r12, %r0			;//    %r12    :=             s
		ld.w		%r13, %r3			;//    %r13    :=                p
		xcall.d		strndup				;//    %r10    := t = strndup(s, p - s)
		sub		%r13, %r0			;//    %r13    :=                p - s		*delay*
		cmp		%r10, 0				;//    if(       !t) { DIE() }
		jreq		strsplit_DIE			;//    
		ld.w		%r12, %sp			;//    %r12    := &v
		xcall.d		strv_push			;//    strv_push( &v, t)
		ld.w		%r13, %r10			;//    %r13    :=     t				*delay*
		;//- - - - - - - - - - - - - - - - - - - - - - -;//
		;//%r0     := s
		;//%r1     := delimiter
		;//%r2     := max_tokens
		;//%r3     := p
		;//[%sp+0] := v
		;//[%sp+4] := vtbl
		xld.w		%r9, [%sp+4]			;//    %r9     :=   fn1
		add		%r9, 4				;//    %r9     :=   fn2				!INTERLOCK!
		call		%r9				;//    %r10    := (*fn2)()
		;//%r0     := s
		;//%r1     := delimiter
		;//%r2     := max_tokens
		;//[%sp+0] := v
		;//[%sp+4] := vtbl
		jp.d		strsplit_LOOP			;//  }
		ld.w		%r0, %r10			;//    %r0     := s = p + strlen(delimiter)	*delay*
strsplit_BREAK:							;//  
		ld.w		%r12, %sp			;//  %r12    :=  &v
		xcall.d		strv_extend			;//  strv_extend(&v, s)
		ld.w		%r13, %r0			;//  %r13    :=      s				*delay*
strsplit_RET:							;//}
		xld.w		%r10, [%sp+0]			;//%r10    := v
		xadd		%sp, %sp, 8			;//
		popn		%r3				;//
		ret						;//return     v
		;//---------------------------------------------;//
strsplit_vtbl:
;//strsplit_fn1:
		ext		mbsstr@rm			;//xjp mbsstr	┬(fn1)は2命令固定
		jp		mbsstr@rl			;//		┘
;//strsplit_fn2:
		xcall.d		strlen				;//%r10    :=     strlen(delimiter)
		ld.w		%r12, %r1			;//%r12    :=            delimiter		*delay*
		ret.d
		add		%r10, %r3			;//%r10    := p + strlen(delimiter)		*delay*
		;//- - - - - - - - - - - - - - - - - - - - - - -;//
strsplit_set_vtbl:
;//strsplit_set_fn1:
		ext		mbspbrk@rm			;//xjp mbspbrk	┬(fn1)は2命令固定
		jp		mbspbrk@rl			;//		┘
;//strsplit_set_fn2:
		xjp.d		mbsinc				;//%r10    := mbsinc(p)
		ld.w		%r12, %r3			;//%r12    :=        p				*delay*
");
static void __attribute__((noreturn,unused))/*asmブロックから参照*/ strsplit_DIE() { DIE(); }
#endif//PIECE
#endif//USE_STRSPLIT_MBCS
//───────────────────────────────────────────────────────────────────────────────────────────────────

//───────────────────────────────────────────────────────────────────────────────────────────────────
// * Sun May 22 21:58:55 JST 2016 Naoyuki Sawa
// - textwrap()の実装を変更しました。結果は変わりません。
//   変更点は以下の通りです。
// - 変更前は、textwrap()の中で文字列配列のメモリ確保を行っていました。
//   変更後は、strv.cモジュールの機能を使って文字列配列のメモリ確保を行うようにしました。
//   この変更によって、関数の実装が単純になり、コードサイズが約44バイト小さくなりました。
//   strv.oをリンクするためのコードサイズ増加は有りますが、既にstrstrip()がstrv.oを使用しているので、新たな増加は有りません。
// - 変更前のコードも、これまで安定して(…という程頻繁には使っていませんでしたが…)動作していたので、#if 0～で切り分けて残してあります。
//   変更後のコードには、変更前のコードほど詳細なコメントは付けていないので、アルゴリズムについては変更前のコードのコメントを参照して下さい。
//   変更後のコードも、変更前のコードと同じアルゴリズムで、文字列配列のメモリを確保する方法が変わっただけです。
//‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥
//■変更前のコード。文字列配列のメモリ確保を関数内で行います。strv.cモジュールに依存しませんが、当関数のコードサイズは少し大きいです。	2015/03～2016/05まで動作していたコードです。
#if 0
#ifndef PIECE
char** textwrap(const char* s, int w) {
	char** argv = NULL;
	int    argc = 0, n = 0;
	/* 文字列が残っている間繰り返す。 */
	while(*s) {
		/* 現在の文字が終端文字で無ければ… */
		if(s[n + 0]) {
			/* 現在の文字の半角(c=1)/全角(c=2)を判定する。 */
			int c = 1;
			if(ismbblead(s[n + 0]) && ismbbtrail(s[n + 1])) {
				c = 2;
			}
			/* 現在の文字を含めた現在の文字列の長さを求める。 */
			n += c;
			/* 現在の文字が現在の文字列の1文字目か,又は,行幅以下ならば継続する。
			 * 前者の条件は(w≦1)が指定されても無限ループに陥らない対策です。 */
			if((n == c) || (n <= w)) { continue; }
			/* 行幅を超えたら現在の文字を含めないで現在の文字列を切り出す。 */
			n -= c;
		}
		/* 文字列ポインタ配列を拡張し文字列を複製して格納する。
		 * どちらかが失敗したらメモリ不足なのでエラー停止する。 */
		if(!(argv = realloc(argv, sizeof(char*) * (argc + 1))) ||
		   !(argv[argc++] = strndup(s, n))) { DIE(); }
		/* 切り出した文字列を飛ばし文字列の長さをリセットする。 */
		s += n, n = 0;
	}
	/* 文字列ポインタ配列の終端としてNULLポインタを格納する。 */
	if(!(argv = realloc(argv, sizeof(char*) * (argc + 1)))) { DIE(); }
	argv[argc] = NULL;
	return argv;
}
#else //PIECE
//アセンブラ化した目的は、コードサイズの低減です。
//コードサイズが、(C言語版=200バイト)⇒(Asm版=156バイト)に小さくなりました。
//文字列分割を行うプログラムは速度が重要でないと思うので、重視していません。
asm("
		.code
		.align		1
		.global		textwrap
textwrap:
		pushn		%r3
		xsub		%sp, %sp, 4
		xld.w		[%sp+0], %r13		;//[%sp+0] := w
		ld.w		%r0, 0			;//%r0     :=  argv = NULL
		ld.w		%r1, 0			;//%r1     := (argc*4) = 0
		jp.d		textwrap_START		;//goto START
		ld.w		%r2, %r12		;//%r2     := s					*delay*
textwrap_LOOP:	;//-------------------------------------;//do {
		;//%r0     :=  argv
		;//%r1     := (argc*4)
		;//%r2     :=  s
		;//%r3     :=  n
		;//[%sp+0] :=  w
		ld.w		%r12, %r2		;//  %r12 :=             s
		add		%r12, %r3		;//  %r12 :=            &s[n+0]
		ld.b		%r12, [%r12]		;//  %r12 :=             s[n+0]
		cmp		%r12, 0			;//  if(                 s[n+0]) {		!INTERLOCK!
		jreq		textwrap_NUL		;//    
		xcall		ismbblead		;//    %r10 := ismbblead(s[n+0])
		cmp		%r10, 0			;//    if(    !ismbblead(s[n+0]) {
		jreq		textwrap_ANK		;//    goto ANK } else {
		ld.w		%r12, %r2		;//      %r12 :=            s
		add		%r12, %r3		;//      %r12 :=           &s[n+0]
		xld.b		%r12, [%r12+1]		;//      %r12 :=            s[n+1]
		xcall		ismbbtrail		;//      %r10 := ismbbtrail(s[n+1])
		cmp		%r10, 0			;//      if(     ismbbtrail(s[n+1]) {
		jrne.d		textwrap_KNJ		;//        
		ld.w		%r4, 2			;//        %r4  := c = 2			*delay*
textwrap_ANK:						;//      } else {
		ld.w		%r4, 1			;//        %r4  := c = 1
textwrap_KNJ:						;//    } }
		add		%r3, %r4		;//    %r3  := n += c
		cmp		%r3, %r4		;//    if(n == c) { goto LOOP }					sは変化していないので(*s)の検査は不要でありLOOPへ戻って良い。C言語版はcontinueを使うのでwhile(*s)に戻ってしまうが実際のところ(*s)の検査は無駄である。
		jreq		textwrap_LOOP		;//    
		xld.w		%r5, [%sp+0]		;//    %r5  := w
		cmp		%r3, %r5		;//    if(n <= w) { goto LOOP }			!INTERLOCK!	sは変化していないので(*s)の検査は不要でありLOOPへ戻って良い。C言語版はcontinueを使うのでwhile(*s)に戻ってしまうが実際のところ(*s)の検査は無駄である。
		jrle		textwrap_LOOP		;//    
		sub		%r3, %r4		;//    %r3  := n -= c
textwrap_NUL:						;//  }
		ld.w		%r12, %r0		;//  %r12 :=                argv
		ld.w		%r13, %r1		;//  %r13 :=                      (argc*4)
		xcall.d		realloc			;//  %r10 := argv = realloc(argv, (argc*4)+4)
		add		%r13, 4			;//  %r13 :=                      (argc*4)+4	*delay*
		cmp		%r10, 0			;//  if(!argv) { DIE() }
		jreq		textwrap_DIE		;//  
		ld.w		%r0, %r10		;//  %r0  := argv
		ld.w		%r12, %r2		;//  %r12 :=             s
		xcall.d		strndup			;//  %r10 := t = strndup(s, n)
		ld.w		%r13, %r3		;//  %r13 :=                n			*delay*
		add		%r1, %r0		;//  %r1  := argv[argc  ]
		ld.w		[%r1]+, %r10		;//          argv[argc++] = t
		sub		%r1, %r0		;//  %r1  :=     (argc*4)
		add		%r2, %r3		;//  %r2  :=  s += n
textwrap_START:
		ld.b		%r4, [%r2]		;//  %r4  := *s
		cmp		%r4, 0			;//  						!INTERLOCK!
		jrne.d		textwrap_LOOP		;;//} while( *s)
		ld.w		%r3, 0			;//  %r3  :=       n = 0			*delay*		初回の'n=0'と文字列を切り出した後の'n=0'を兼ねる。
		;//-------------------------------------;//
		ld.w		%r12, %r0		;//  %r12 :=                argv
		ld.w		%r13, %r1		;//  %r13 :=                      (argc*4)
		xcall.d		realloc			;//  %r10 := argv = realloc(argv, (argc*4)+4)
		add		%r13, 4			;//  %r13 :=                      (argc*4)+4	*delay*
		cmp		%r10, 0			;//  if(!argv) { DIE() }
		jreq		textwrap_DIE		;//  
		add		%r1, %r10		;//  %r1  := argv[argc]
		ld.w		[%r1], %r8		;//          argv[argc] = NULL
		xadd		%sp, %sp, 4
		popn		%r3
		ret
");
static void __attribute__((noreturn,unused))/*asmブロックから参照*/ textwrap_DIE() { DIE(); }
#endif//PIECE
//‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥
#else
//‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥
//■変更後のコード。strv.cモジュールの機能を使って文字列配列のメモリ確保を行います。関数の実装が単純になりコードサイズが小さくなりました。	2016/05～使用開始するコードです。
#ifndef PIECE
char** textwrap(const char* s, int w) {
	char** v = strv_new(NULL);
	while(*s) {
		int n = 0, c;
		while((c = mbsnextc(&s[n]))) {
			c = (c >> 8) ? 2 : 1;
			n += c;
			if((n != c) && (n > w)) { break; }	//「if((n != c) && (n >= w))～」としてはいけません。この下で「n -= c」を行うので、文字数がオーバーしてからループを抜ける必要が有ります。
		}
		n -= c;						//「while((c = mbsnextc(&s[n]))) {」で抜けた場合は、(c==0)なのでダミー処理となります。
		{
			char* t = strndup(s, n);
			if(!t) { DIE(); }
			strv_push(&v, t);
		}
		s += n;
	}
	return v;
}
#else //PIECE
asm("
		.code
		.align		1
		.global		textwrap
textwrap:
		pushn		%r2			;//
		xsub		%sp, %sp, 4		;//
		ld.w		%r0, %r12		;//%r0     := s
	;//	ld.w		%r1, %r13		;//%r1     := w		────────┐
		xld.w		[%sp+0], %r8		;//[%sp+0] :=              NULL		│
		xcall.d		strv_new		;//%r10    :=     strv_new(NULL)	│
		ld.w		%r1, %r13		;//%r1     := w		←───────┘	*delay*
		xld.w		[%sp+0], %r10		;//[%sp+0] := v = strv_new(NULL)
		;//%r0     := s
		;//%r1     := w
		;//[%sp+0] := v
textwrap_LOOP1:						;//for(;;) {
		ld.b		%r9, [%r0]		;//  %r9     := *s
		cmp		%r9, 0			;//  if(    !*s) { break }			!INTERLOCK!
		jreq		textwrap_RET		;//  
		ld.w		%r2, 0			;//  %r2     := n = 0
		;//%r0     := s
		;//%r1     := w
		;//%r2     := n
		;//[%sp+0] := v
textwrap_LOOP2:						;//  for(;;) {
		ld.w		%r12, %r0		;//    %r12    :=              s
		xcall.d		mbsnextc		;//    %r10    := c = mbsnextc(s + n)
		add		%r12, %r2		;//    %r12    :=              s + n		*delay*
		cmp		%r10, 0			;//    if(!c) { break }
		jreq		textwrap_BREAK		;//    
		sra		%r10, 8			;//    %r10    :=     (c >> 8)
		jreq.d		3			;//    %r10    := c = (c >> 8) ? 2 : 1
		 ld.w		%r10, 1			;//    						*delay*
		 ld.w		%r10, 2			;//    
		add		%r2, %r10		;//    %r2     := n += c
		cmp		%r2, %r10		;//    if(n == c) { continue }
		jreq		textwrap_LOOP2		;//    
		cmp		%r2, %r1		;//    if(n <= w) { continue }
		jrle		textwrap_LOOP2		;//    
textwrap_BREAK:						;//    break }
		sub		%r2, %r10		;//  %r2     := n -= c
		ld.w		%r12, %r0		;//  %r12    :=             s
		xcall.d		strndup			;//  %r10    := t = strndup(s, n)
		ld.w		%r13, %r2		;//  %r13    :=                n		*delay*
		cmp		%r10, 0			;//  if(!t) { DIE() }
		jreq		textwrap_DIE		;//  
		ld.w		%r12, %sp		;//  %r12    : &v
		xcall.d		strv_push		;//  strv_push(&v, t)
		ld.w		%r13, %r10		;//  %r13    :=    t				*delay*
		jp.d		textwrap_LOOP1		;//}
		add		%r0, %r2		;//  %r0     := s += n				*delay*
textwrap_RET:						;//
		xld.w		%r10, [%sp+0]		;//%r10    := v
		xadd		%sp, %sp, 4		;//
		popn		%r2			;//return     v
		ret
");
static void __attribute__((noreturn,unused))/*asmブロックから参照*/ textwrap_DIE() { DIE(); }
#endif//PIECE
#endif
//───────────────────────────────────────────────────────────────────────────────────────────────────

// * Sat Apr 09 21:06:53 JST 2016 Naoyuki Sawa
// - strdup_splitpath()とstrdup_makepath()を追加しました。
//   splitpath()とmakepath()も、strdup_printf()と同様に使えると便利なので、自分の判断で'strdup_*()'バージョンを追加する事にしました。
// - □使用例
//   │void test(const char* path) {
//   │  char *fname, *ext, *fullpath;
//   │  strdup_splitpath(path, NULL, NULL, &fname, &ext);
//   │  fullpath = strdup_makepath("C:", "\\Home", fname, ext);
//   │  printf("%s\n", fullpath);
//   │  free(fname);    //┐
//   │  free(ext);      //├ガーベージコレクターを使用している場合は解放しなくても構いません。
//   │  free(fullpath); //┘
//   │}
void strdup_splitpath(const char* path, char** drive/*NULL可*/, char** dir/*NULL可*/, char** fname/*NULL可*/, char** ext/*NULL可*/) {
	char _drive[MAX_DRIVE], _dir[MAX_DIR], _fname[MAX_FNAME], _ext[MAX_EXT];
	splitpath(path, _drive, _dir, _fname, _ext);
	if(drive) { if(!(*drive = strdup(_drive))) { DIE(); } }
	if(dir  ) { if(!(*dir   = strdup(_dir  ))) { DIE(); } }
	if(fname) { if(!(*fname = strdup(_fname))) { DIE(); } }
	if(ext  ) { if(!(*ext   = strdup(_ext  ))) { DIE(); } }
}
char* strdup_makepath(const char* drive/*NULL可*/, const char* dir/*NULL可*/, const char* fname/*NULL可*/, const char* ext/*NULL可*/) {
	char _path[MAX_PATH], *path;
	makepath(_path, drive, dir, fname, ext);
	if(!(path = strdup(_path))) { DIE(); }
	return path;
}

/*****************************************************************************
 *	比較
 *****************************************************************************/

#ifndef PIECE
#if 0
//素直に実装したバージョン。
int str_has_prefix(const char* s1, const char* s2) {
	unsigned n1 = strlen(s1);
	unsigned n2 = strlen(s2);
	if(n1 < n2) { return 0; }
	return !strncmp(s1, s2, n2);
}
int str_has_suffix(const char* s1, const char* s2) {
	unsigned n1 = strlen(s1);
	unsigned n2 = strlen(s2);
	if(n1 < n2) { return 0; }
	return !strcmp(s1 + n1 - n2, s2);
}
#else
//共通処理をまとめて容量節約したバージョン。
static int str_has_prefix_suffix(const char* s1, const char* s2, int mask);
       int str_has_prefix(const char* s1, const char* s2) { return str_has_prefix_suffix(s1, s2,  0); }
       int str_has_suffix(const char* s1, const char* s2) { return str_has_prefix_suffix(s1, s2, -1); }
static int str_has_prefix_suffix(const char* s1, const char* s2, int mask) {
	unsigned n1 = strlen(s1);
	unsigned n2 = strlen(s2);
	if(n1 < n2) { return 0; }
	return !strncmp(s1 + ((n1-n2)&mask)/*0,or,(n1-n2)*/, s2, (n2|mask)/*n2,or,-1*/);
}
#endif
#else /*PIECE*/
//共通処理をまとめて容量節約したバージョンを元に、アセンブラ化しました。
int str_has_prefix(const char* s1, const char* s2);
int str_has_suffix(const char* s1, const char* s2);
asm("
		.code
		.align		1
		.global		str_has_prefix
		.global		str_has_suffix
str_has_prefix:
		jp.d		str_has_prefix_suffix
		ld.w		%r14,  0			;//%r14 := mask =  0									*delay*
str_has_suffix:
		ld.w		%r14, -1			;//%r14 := mask = -1
str_has_prefix_suffix:
		pushn		%r3
		ld.w		%r0, %r12			;//%r0  := s1
		ld.w		%r1, %r13			;//%r1  := s2
		xcall.d		strlen				;//%r10 := n1 = strlen(s1)
		ld.w		%r2, %r14			;//%r2  := mask										*delay*
		ld.w		%r3, %r10			;//%r3  := n1
		xcall.d		strlen				;//%r10 := n2 = strlen(s2)
		ld.w		%r12, %r1			;//%r12 :=             s2								*delay*
		sub		%r3, %r10			;//%r3  :=      n1 -         n2		%psr(C) := ((unsigned)n1<(unsigned)n2) ? 1 : 0
		jrult		str_has_prefix_suffix_L10	;//if((unsigned)n1<(unsigned)n2) { return 0 }					┬
		and		%r3, %r2			;//%r3  :=                        (n1-n2)&mask					│
		or		%r10, %r2			;//%r10 :=                                            n2|mask			│
		ld.w		%r12, %r0			;//%r12 :=                  s1							│
		add		%r12, %r3			;//%r12 :=                  s1 + ((n1-n2)&mask)					│
		ld.w		%r13, %r1			;//%r13 :=                                       s2				│
		xcall.d		strncmp				;//%r10 := retval = strncmp(s1 + ((n1-n2)&mask), s2, (n2|mask))			│
		ld.w		%r14, %r10			;//%r14 :=                                            n2|mask			│	*delay*
		cmp		%r8, %r10			;//%psr(C) := retval  ?  1 : 0							│
str_has_prefix_suffix_L10:					;//			┬　┬							│
		ld.w		%r10, 1				;//%r10    := 1		└─┴─────┐					│
		popn		%r3				;//					│					│
		ret.d						;//					│					│
		sbc		%r10, %r8			;//%r10    := %psr(C) ?  0 : 1	←───┘←──────────────────┘	*delay*
");
#endif/*PIECE*/

#ifndef PIECE
int string_match(const char* pat, const char* str) {
	int p, q, s;
	for(;;) {
		//パターンから一文字取得し、次の位置へ進める。
		p = *pat++;
		//文字列から一文字取得し、次の位置へ進める。
		s = *str++;
		//パターンが終端だったならば…
		if(!p) {
			//文字列が終端だったならば、マッチしたと決定する。
			//文字列が終端でなかったならば、マッチしなかったと決定する。
			return !s;
		}
		//文字列が終端だったならば…
		if(!s) {
			//パターンの文字が'*'以外だったならば、マッチしなかったと決定する。
			if(p != '*') { return 0; }
			//パターンの文字が'*'だったならば、以下のブロックにて処理を行う。
		}								//│
		//パターンの文字が'*'だったならば…				//│
		if(p == '*') {							//│
			//パターンが終端ならば、マッチしたと決定する。		//│
			if(!*pat) { return 1; }					//↓
			//文字列の位置を、文字sの位置へ戻す。文字sは'\0'であることも有り得る。
			str--;
			for(;;) {
				//現在位置以降の文字列とパターンがマッチしたら、マッチしたと決定する。
				if(string_match(pat, str)) { return 1; }
				//'*'が消費できる文字が残っていなければ、マッチしなかったと決定する。
				if(!*str++) { return 0; }
				//'*'が一文字消費したと見なして、比較を継続する。
			}
		}
		//パターンの文字が'?'だったならば、一文字一致したと見なして、次の文字の比較処理へ移行する。
		if(p == '?') { continue; }
		//パターンの文字が'['だったならば…
		if(p == '[') {
			int match = 0;
			for(;;) {
				//パターンから一文字取得し、次の位置へ進める。
				p = q = *pat++;
				//パターンが終端だったならば、閉じていない文字クラスであり、不正なパターンである。
				if(!p) { DIE(); }
				//パターンの文字が']'だったならば、ループを抜ける。
				//□Tcl6.7のstring matchと、一般的な正規表現の相違点として、以下に注意せよ:
				//│一般的な正規表現では、'['の直後の']'は文字クラスを閉じず、文字クラスの一文字と見なされるが、Tcl6.7のstring matchにはこの特例処理が無い。
				//│一般的な正規表現では、文字クラスの中でも'\'による特殊文字のエスケープやエスケープシーケンスが使えるが、Tcl6.7のstring matchでは使えない。
				if(p == ']') { break; }
				//パターンの文字が'-'ならば…
				if(*pat == '-') {
					//パターンの位置を、'-'の次の位置へ進める。
					pat++;
					//パターンから一文字取得し、次の位置へ進める。
					q = *pat++;
					//パターンが終端だったならば、閉じていない文字クラスであり、不正なパターンである。
					if(!q) { DIE(); }
				}
				//文字が文字クラスの範囲内ならば、文字が文字クラスに一致したことをマークする。
				//文字クラスの残りを読み捨てるために、ループを継続する。
				if((s >= p) && (s <= q)) { match = 1; }
				if((s >= q) && (s <= p)) { match = 1; }	//逆範囲は正規表現の仕様外だが、Tcl6.7の実装では許容している。
			}
			//文字が文字クラスに一致していなければ、マッチしなかったと決定する。
			if(!match) { return 0; }
			//一文字一致したと見なして、次の文字の比較処理へ移行する。
			continue;
		}
		//パターンの文字が'\'だったならば…
		if(p == '\\') {
			//パターンから一文字取得し、次の位置へ進める。
			p = *pat++;
			//パターンが終端だったならば、不正なエスケープであり、不正なパターンである。
			if(!p) { DIE(); }
		}
		//パターンの文字と文字列の文字が一致しなければ、マッチしなかったと決定する。
		if(p != s) { return 0; }
	}
}
#else //PIECE
//アセンブラ化した目的は、コードサイズの低減です。
//コードサイズが、(C言語版=224バイト)⇒(Asm版=172バイト)に小さくなりました。
//文字列比較を行うプログラムは速度が重要でないと思うので、重視していません。
int string_match(const char* pat, const char* str);
asm("
		.code
		.align		1
		.global		string_match
string_match:							;//for(;;) {
		ld.b		%r4, [%r12]+			;//  %r4  := p = *pat++
		ld.b		%r5, [%r13]+			;//  %r5  := s = *str++
		cmp		%r4, 0				;//  if(!p) {
		jrne.d		4
		 cmp		%r5, 0				;//  %psr(Z) := !s			─┐	*delay*
		 jreq		string_match_RET_1		;//    return !s			←┤
		 jp		string_match_RET_0		;//  }					　│
		jrne.d		3				;//  if(!s) {				←┘
		 sub		%r4, 42				;//  %psr(Z) := (p == '*')		─┐	*delay*		←┬「cmp %r4,42」は「cmp %rd,sign6」の範囲外なので不可。
		 jrne		string_match_RET_0		;//    if(p != '*') { return 0 } }	←┤			　│範囲外であるにもかかわらずアセンブラエラーが出ないので注意せよ。
		jrne.d		string_match_L20		;//  if(p == '*') {			←┘			　│「xcmp %r4,42」にすると2か所に書く必要が生じるが、「sub %r4,42」で判断して「add %r4,42」で戻すと1か所で済む。
		add		%r4, 42				;//  						*delay*		←┴ここでは「sub %r4,42」～「add %r4,42」の間で%r4の値が変化しても問題無いことを利用した工夫である。
		ld.b		%r4, [%r12]			;//    %r4  := p = *pat
		cmp		%r4, 0				;//    if(!p) { return 1 }
		jreq		string_match_RET_1
		sub		%r13, 1				;//    %r13 := str--
string_match_L10:						;//    for(;;) {
	;//{{どちらか一方を選択してください。
	;//□スタック消費量が少ないバージョン
	;//	xsub		%sp, %sp, 8
	;//	xld.w		[%sp+0], %r12			;//      [%sp+0] := pat
	;//	xld.w		[%sp+4], %r13			;//      [%sp+4] := str
	;//	call		string_match			;//      %r10 := retval = string_match(pat,str)
	;//	xld.w		%r12, [%sp+0]			;//      %r12 := pat
	;//	xld.w		%r13, [%sp+4]			;//      %r13 := str
	;//	xadd		%sp, %sp, 8
	;//	cmp		%r10, 0				;//      if(retval) { return 1 }
	;//||どちらか一方を選択してください。
	;//□スタック消費量が多いが4命令節約したバージョン
		pushn		%r13
		call		string_match			;//      %r10 := retval = string_match(pat,str)
		cmp		%r10, 0				;//      if(retval) { return 1 }
		popn		%r13
	;//}}どちらか一方を選択してください。
		jrne		string_match_RET_1
		ld.b		%r10, [%r13]+			;//      %r10 := s = *str++
		cmp		%r10, 0				;//      if(!s) { return 0 }
		jrne		string_match_L10
		jp		string_match_RET_0		;//    }
string_match_L20:						;//  }
		xcmp		%r4, 63
		jreq		string_match
		xcmp		%r4, 91				;//  if(p == '[') {
		jrne		string_match_L40
		ld.w		%r6, 0				;//    %r6  := match = 0
string_match_L30:						;//    for(;;) {
		ld.b		%r4, [%r12]+			;//      %r4  := p = *pat++
		cmp		%r4, 0				;//      if(!p) { DIE() }
		jreq		string_match_DIE
		xcmp		%r4, 93				;//      if(p == ']') { break }
		jreq		string_match_L35
		ld.b		%r7, [%r12]			;//      %r7  := q = *pat
		xcmp		%r7, 45				;//      if(q == '-') {
		jrne.d		6
		 cmp		%r7, %r4			;//      %r7  := q = p				*delay*
		 add		%r12, 1				;//        %r12 := pat++
		 ld.b		%r7, [%r12]+			;//        %r7  := q = *pat++
		 cmp		%r7, 0				;//        if(!q) { DIE() }
		 jreq		string_match_DIE		;//      }
		cmp		%r4, %r7			;//      if(p > q) {
		jrle		4
		 xor		%r4, %r7			;//        %r4  := p ^= q	┐
		 xor		%r7, %r4			;//        %r7  := q ^= p	├pとqを交換
		 xor		%r4, %r7			;//        %r4  := p ^= q }	┘
		sub		%r5, %r4			;//      %r5  :=       s-p
		sub		%r7, %r4			;//      %r7  :=                          q-p
		cmp		%r5, %r7			;//      if((unsigned)(s-p) <= (unsigned)(q-p)) { match = 1 }
		jrugt		string_match_L30
		jp.d		string_match_L30
		ld.w		%r6, 1				;//						*delay*
string_match_L35:						;//    }
		cmp		%r6, 0				;//    if(!match) { return 0 }
		jrne		string_match			;//    continue
		jp		string_match_RET_0
string_match_L40:						;//  }
		xcmp		%r4, 92				;//  if(p == '￥') {
		jrne		4
		 ld.b		%r4, [%r12]+			;//    %r4  := p = *pat++
		 cmp		%r4, 0				;//    if(!p) { DIE() }
		 jreq		string_match_DIE		;//  }
		cmp		%r4, %r5			;//  if(p != s) { return 0 }
		jreq		string_match			;//}
string_match_RET_0:
		ret.d						;//return 0
		ld.w		%r10, 0				;//						*delay*
string_match_RET_1:
		ret.d						;//return 1
		ld.w		%r10, 1				;//						*delay*
");
static void __attribute__((noreturn,unused))/*asmブロックから参照*/ string_match_DIE() { DIE(); }
#endif//PIECE

#ifndef PIECE
static int strnatcmp_subr(const unsigned char* s1, const unsigned char* s2, int (*conv)(int)) {
	int c1, c2;
	//行頭の空白を読み飛ばす。
	// - 行頭の空白は比較対象に含めません。
	while(isspace(c1 = *s1++)) { /** no job **/ }
	while(isspace(c2 = *s2++)) { /** no job **/ }
	for(;;) {
		//両方が数字ならば…
		if(isdigit(c1) && isdigit(c2)) {
			//少なくとも一方が小数部ならば…
			// - '0'から始まる数字列は暗黙の'.'が先行していると見なす。
			if((c1 == '0') || (c2 == '0')) {
				//Leading zeros are not ignored, which tends to give more reasonable results on decimal fractions.
				//1.001 < 1.002 < 1.010 < 1.02 < 1.1 < 1.3
				for(;;) {
					if(isdigit(c1)) {
						//両方とも数字の場合…
						if(isdigit(c2)) {
							if((c1 -= c2)) { return c1; }
						//左の桁数の方が多い場合…
						} else {
							return  1;
						}
					} else {
						//右の桁数の方が多い場合…
						if(isdigit(c2)) {
							return -1;
						//両方とも数字でない場合…
						} else {
							break;	//連続した空白文字を一つのスペース(' ')に置き換える処理へ移行する。
						}
					}
					//次の文字を取得する。
					c1 = *s1++;
					c2 = *s2++;
				}
			//両方とも整数部ならば…
			} else {
				int bias = 0;
				for(;;) {
					if(isdigit(c1)) {
						//両方とも数字の場合…
						if(isdigit(c2)) {
							if(!bias) { bias = c1 - c2; }
						//左の桁数の方が多い場合…
						} else {
							return  1;
						}
					} else {
						//右の桁数の方が多い場合…
						if(isdigit(c2)) {
							return -1;
						//両方とも数字でない場合…
						} else {
							if(bias) { return bias; }
							break;	//連続した空白文字を一つのスペース(' ')に置き換える処理へ移行する。
						}
					}
					//次の文字を取得する。
					c1 = *s1++;
					c2 = *s2++;
				}
			}
		//少なくとも一方が数字でなければ…
		} else {
			//片方の文字列が終端ならば、もう片方の文字列の空白文字を読み飛ばす。
			// - その結果、もう片方の文字列も終端に達したら、一致と見なすためです。
			//   行末の空白は比較対象に含めません。
			if(!c1) { while(isspace(c2)) { c2 = *s2++; } }
			if(!c2) { while(isspace(c1)) { c1 = *s1++; } }
			//必要に応じて大文字に変換する。
			c1 = (*conv)(c1);
			c2 = (*conv)(c2);
			//c1,c2は(unsigned char*)から読み出した値なので、Overflowの心配は無く減算で比較出来る。
			c1 -= c2;
			//c1,c2が異なるか,又は,両方が終端に達したら、比較結果を返す。
			// - 後者の条件判断は、前者の条件判断が否定されたの後なので、片方だけを検査すれば良い。
			if(c1 || !c2) { return c1; }
			//次の文字を取得する。
			c1 = *s1++;
			c2 = *s2++;
		}
		//単語間の連続した空白文字を一つのスペース(' ')に置き換える。
		// - 単語間の空白文字は一つのスペース(' ')と見なして比較します。
		if(isspace(c1)) {
			   c1 = ' ';
			while(isspace(*s1++)) { /** no job **/ }
			               s1--;
		}
		if(isspace(c2)) {
			   c2 = ' ';
			while(isspace(*s2++)) { /** no job **/ }
			               s2--;
		}
	}
}
static int strnatcmp_noconv(int c) { return c; }
int strnatcmp(    const char* s1, const char* s2) { return strnatcmp_subr((unsigned char*)s1, (unsigned char*)s2, strnatcmp_noconv); }
int strnatcasecmp(const char* s1, const char* s2) { return strnatcmp_subr((unsigned char*)s1, (unsigned char*)s2,          toupper); }
#else //PIECE
int strnatcmp(    const char* s1, const char* s2);
int strnatcasecmp(const char* s1, const char* s2);
asm("
		.code
		.align		1
		.global		strnatcmp
		.global		strnatcasecmp
strnatcasecmp:
		xld.w		%r14, toupper			;//%r14 := conv = toupper
		jp		strnatcmp_subr
		;//---------------------------------------------;//
strnatcmp:
		xld.w		%r14, strnatcmp_noconv		;//%r14 := conv = strnatcmp_noconv
		;//---------------------------------------------;//
strnatcmp_subr:
		;//%r12 := s1
		;//%r13 := s2
		;//%r14 := conv
		pushn		%r3
		xsub		%sp, %sp, 4
		ld.w		%r0, %r12			;//%r0     := s1
		ld.w		%r1, %r13			;//%r1     := s2
		xld.w		[%sp+0], %r14			;//[%sp+0] := conv
		;//行頭の空白を読み飛ばす。
strnatcmp_subr_LOOP1:						;//for(;;) {
		ld.ub		%r2, [%r0]+			;//  %r2  := c1 = *s1++
		call.d		strnatcmp_isspace		;//  if(!isspace(c1)) { break }
		ld.w		%r12, %r2			;//  													*delay*
		jrne		strnatcmp_subr_LOOP1		;//}
strnatcmp_subr_LOOP2:						;//for(;;) {
		ld.ub		%r3, [%r1]+			;//  %r3  := c2 = *s2++
		call.d		strnatcmp_isspace		;//  if(!isspace(c2)) { break }
		ld.w		%r12, %r3			;//  													*delay*
		jrne		strnatcmp_subr_LOOP2		;//}
		;//- - - - - - - - - - - - - - - - - - - - - - -;//
strnatcmp_subr_LOOP3:						;//for(;;) {
		call		strnatcmp_isdigit		;//  %r4[0] := isdigit(c2), %r4[1] := isdigit(c1), %r4[31:2] := 0, %alr := (c1 - '0') * (c2 - '0')
		cmp		%r4, 3				;//  if(isdigit(c1) && isdigit(c2)) {
		jrne		strnatcmp_subr_L10		;//    
		;//両方が数字ならば…				;//    
		;//両方とも整数部ならば…			;//    
		ld.w		%r9, %alr			;//    %r9  := (c1 - '0') * (c2 - '0')
		cmp		%r9, 0				;//    if(     (c1 - '0') * (c2 - '0')) {
		jreq		strnatcmp_subr_LOOP5		;//      
		ld.w		%r10, 0				;//      %r10 := bias = 0
strnatcmp_subr_LOOP4:						;//      for(;;) {
		sub		%r4, 1				;//        if(!isdigit(c1) && !isdigit(c2)) {
		jrge		4				;//          
		 cmp		%r10, 0				;//          if(bias) { return bias }
		 jrne		strnatcmp_subr_RET		;//          
		 jp		strnatcmp_subr_LOOP3_NEXT	;//          break }
		sub		%r4, 1				;//        if(!isdigit(c1) &&  isdigit(c2)) { return -1 }
		jrlt		strnatcmp_subr_RETm1		;//        
		sub		%r4, 1				;//        if( isdigit(c1) && !isdigit(c2)) { return  1 }
		jrlt		strnatcmp_subr_RET1		;//        
		cmp		%r10, 0				;//        if(!bias) {
		jrne		3				;//          
		 ld.w		%r10, %r2			;//          %r10 :=        c1
		 sub		%r10, %r3			;//          %r10 := bias = c1 - c2 }
		ld.ub		%r2, [%r0]+			;//        %r2  := c1 = *s1++
		ld.ub		%r3, [%r1]+			;//        %r3  := c2 = *s2++
		call		strnatcmp_isdigit		;//        %r4[0] := isdigit(c2), %r4[1] := isdigit(c1), %r4[31:2] := 0, %alr := (c1 - '0') * (c2 - '0')
		jp		strnatcmp_subr_LOOP4		;//      }
		;//少なくとも一方が小数部ならば…		;//    } else {
strnatcmp_subr_LOOP5:						;//      for(;;) {
		sub		%r4, 1				;//        if(!isdigit(c1) && !isdigit(c2)) { break }
		jrlt		strnatcmp_subr_LOOP3_NEXT	;//        
		sub		%r4, 1				;//        if(!isdigit(c1) &&  isdigit(c2)) { return -1 }
		jrlt		strnatcmp_subr_RETm1		;//        
		sub		%r4, 1				;//        if( isdigit(c1) && !isdigit(c2)) { return  1 }
		jrlt		strnatcmp_subr_RET1		;//        
		ld.w		%r10, %r2			;//        %r10 := c1
		sub		%r10, %r3			;//        %r10 := c1 - c2
		jrne		strnatcmp_subr_RET		;//        if(     c1 - c2) { return c1 - c2 }
		ld.ub		%r2, [%r0]+			;//        %r2  := c1 = *s1++
		ld.ub		%r3, [%r1]+			;//        %r3  := c2 = *s2++
		call		strnatcmp_isdigit		;//        %r4[0] := isdigit(c2), %r4[1] := isdigit(c1), %r4[31:2] := 0, %alr := (c1 - '0') * (c2 - '0')
		jp		strnatcmp_subr_LOOP5		;//    } }
strnatcmp_subr_L10:						;//  } else {
		;//少なくとも一方が数字でなければ…		;//    
		;//片方の文字列が終端ならば、もう片方の文字列の空白文字を読み飛ばす。
		cmp		%r2, 0				;//    if(!c1) {
		jrne		strnatcmp_subr_LOOP6_SKIP	;//      
strnatcmp_subr_LOOP6:						;//      for(;;) {
		call.d		strnatcmp_isspace		;//        if(!isspace(c2)) { break }
		ld.w		%r12, %r3			;//        												*delay*
		jreq		strnatcmp_subr_LOOP6_SKIP	;//        
		ld.ub		%r3, [%r1]+			;//        c2 = *s2++
		jp		strnatcmp_subr_LOOP6		;//      }
strnatcmp_subr_LOOP6_SKIP:					;//    }
		cmp		%r3, 0				;//    if(!c2) {
		jrne		strnatcmp_subr_LOOP7_SKIP	;//      
strnatcmp_subr_LOOP7:						;//      for(;;) {
		call.d		strnatcmp_isspace		;//        if(!isspace(c1)) { break }
		ld.w		%r12, %r2			;//        												*delay*
		jreq		strnatcmp_subr_LOOP7_SKIP	;//        
		ld.ub		%r2, [%r0]+			;//        c1 = *s1++
		jp		strnatcmp_subr_LOOP7		;//      }
strnatcmp_subr_LOOP7_SKIP:					;//    }
		;//必要に応じて大文字に変換する。		;//    
		xld.w		%r9, [%sp+0]			;//    %r9  :=        conv
		call.d		%r9				;//    %r10 := c2 = (*conv)(c2)
		ld.w		%r12, %r3			;//    %r12 :=              c2										*delay*
		ld.w		%r3, %r10			;//    %r3  := c2
		xld.w		%r9, [%sp+0]			;//    %r9  :=        conv
		call.d		%r9				;//    %r10 := c1 = (*conv)(c1)
		ld.w		%r12, %r2			;//    %r12 :=              c1										*delay*
		;//c1,c2が異なるか,又は,両方が終端に達したら、比較結果を返す。
		sub		%r10, %r3			;//    %r10 := c1 -= c2
		jrne		strnatcmp_subr_RET		;//    if( c1) { return c1 }
		cmp		%r3, 0				;//    if(!c2) { return c1 }
		jreq		strnatcmp_subr_RET		;//    
		;//次の文字を取得する。				;//    
		ld.ub		%r2, [%r0]+			;//    %r2  := c1 = *s1++
		ld.ub		%r3, [%r1]+			;//    %r3  := c2 = *s2++
strnatcmp_subr_LOOP3_NEXT:					;//  }
		;//連続した空白文字を一つのスペース(' ')に置き換える。
		call.d		strnatcmp_isspace		;//  if(isspace(c1)) {
		ld.w		%r12, %r2			;//    													*delay*
		jreq		strnatcmp_subr_LOOP8_SKIP	;//    
		xld.w		%r2, 32				;//    %r2  := c1 = ' '
strnatcmp_subr_LOOP8:						;//    
		ld.ub		%r12, [%r0]+			;//    while(isspace(*s1++)) { /** no job **/ }
		call		strnatcmp_isspace		;//    
		jrne		strnatcmp_subr_LOOP8		;//    
		sub		%r0, 1				;//    %r0  := s1--
strnatcmp_subr_LOOP8_SKIP:					;//  }
		call.d		strnatcmp_isspace		;//  if(isspace(c2)) {
		ld.w		%r12, %r3			;//    													*delay*
		jreq		strnatcmp_subr_LOOP9_SKIP	;//    
		xld.w		%r3, 32				;//    %r3  := c2 = ' '
strnatcmp_subr_LOOP9:						;//    
		ld.ub		%r12, [%r1]+			;//    while(isspace(*s2++)) { /** no job **/ }
		call		strnatcmp_isspace		;//    
		jrne		strnatcmp_subr_LOOP9		;//    
		sub		%r1, 1				;//    %r1  := s2--
strnatcmp_subr_LOOP9_SKIP:					;//  }
		jp		strnatcmp_subr_LOOP3		;//}
		;//- - - - - - - - - - - - - - - - - - - - - - -;//
strnatcmp_subr_RET:
		xadd		%sp, %sp, 4
		popn		%r3
		ret
strnatcmp_subr_RET1:						;//
		jp.d		strnatcmp_subr_RET		;//return   1
		ld.w		%r10, 1				;//%r10 :=  1												*delay*
strnatcmp_subr_RETm1:						;//
		jp.d		strnatcmp_subr_RET		;//return  -1
		ld.w		%r10, -1			;//%r10 := -1												*delay*
		;//---------------------------------------------;//
strnatcmp_isspace:
		xcall		isspace				;//%r10    :=  isspace(c)
		ret.d						;//%psr(Z) := !isspace(c)
		cmp		%r10, 0				;//													*delay*
		;//---------------------------------------------;//
strnatcmp_isdigit:
		sub		%r2, 48				;//%r2     := c1 -= '0'
		cmp		%r2, 10				;//%psr(C) := (c1 <= 9)
		adc		%r4, %r4			;//%r4[0]  := isdigit(c1),                         %r4[31:1] := (不定値)
		sub		%r3, 48				;//%r3     := c2 -= '0'
		cmp		%r3, 10				;//%psr(C) := (c2 <= 9)
		adc		%r4, %r4			;//%r4[0]  := isdigit(c2), %r4[1]  := isdigit(c1), %r4[31:2] := (不定値)
		mlt.h		%r2, %r3			;//%alr    := (c1 - '0') * (c2 - '0')
		add		%r2, 48				;//%r2     := c1 += '0'
		add		%r3, 48				;//%r3     := c2 += '0'
		ret.d						;//
		and		%r4, 3				;//%r4[0]  := isdigit(c2), %r4[1]  := isdigit(c1), %r4[31:2] := 0					*delay*
		;//---------------------------------------------;//
strnatcmp_noconv:
		ret.d						;//return  c
		ld.w		%r10, %r12			;//%r10 := c												*delay*
");
#endif//PIECE
//□テストスイート
//	static int compar_strcmp(   const char** a, const char** b) { return strcmp(   *a, *b); }
//	static int compar_strnatcmp(const char** a, const char** b) { return strnatcmp(*a, *b); }
//	static void test1() {
//		static const char* const org[9]={"0","00","000","01","010","09","1","9","10"};
//		       const char*       tmp[9];
//		memcpy((void*)tmp, org, sizeof org);
//		qsort((void*)tmp, 9, sizeof(char*), compar_strcmp);
//		assert(memcmp(tmp, org, sizeof org) != 0);
//		qsort((void*)tmp, 9, sizeof(char*), compar_strnatcmp);
//		assert(memcmp(tmp, org, sizeof org) == 0);
//		//
//		assert(strnatcmp("", "") == 0);
//		assert(strnatcmp("a", "a") == 0);
//		assert(strnatcmp("a", "b") < 0);
//		assert(strnatcmp("b", "a") > 0);
//		assert(strnatcmp("000", "00") > 0);	//ここがstrverscmp()と違います。strnatcmp()の方が直観に近いです。
//		assert(strnatcmp("00", "000") < 0);	//ここがstrverscmp()と違います。strnatcmp()の方が直観に近いです。
//		assert(strnatcmp("a0", "a") > 0);
//		assert(strnatcmp("00", "01") < 0);
//		assert(strnatcmp("01", "010") < 0);
//		assert(strnatcmp("010", "09") < 0);
//		assert(strnatcmp("09", "0") > 0);	//ここがstrverscmp()と違います。strnatcmp()の方が直観に近いです。
//		assert(strnatcmp("9", "10") < 0);
//		assert(strnatcmp("0a", "0") > 0);
//	}

/*****************************************************************************
 *	反転
 *****************************************************************************/

/* 文字列を反転する。
 * [in]
 *	s		反転する文字列。
 * [out]
 *	戻り値		引数sをそのまま返す。
 * [note]
 *	- GLibのg_strreverse()に互換です。
 */
#ifndef PIECE
char* strreverse(char* s) {
	char* p1 = s;
	char* p2 = strchr(s, '\0');
//{{2016/09/30追加:strreverse()のC言語版の実装で、'goto L_START'が変数'c'の宣言をまたいでいた問題を修正しました。
	int c;
//}}2016/09/30追加:strreverse()のC言語版の実装で、'goto L_START'が変数'c'の宣言をまたいでいた問題を修正しました。
	goto L_START;
	do {
//{{2016/09/30変更:strreverse()のC言語版の実装で、'goto L_START'が変数'c'の宣言をまたいでいた問題を修正しました。
//		int c = *p1;
//↓2016/09/30変更:strreverse()のC言語版の実装で、'goto L_START'が変数'c'の宣言をまたいでいた問題を修正しました。
		    c = *p1;
//}}2016/09/30変更:strreverse()のC言語版の実装で、'goto L_START'が変数'c'の宣言をまたいでいた問題を修正しました。
		        *p1 = *p2;
		              *p2 = c;
		p1++;
L_START:	p2--;
	} while(p1 < p2);
	return s;
}
#else /*PIECE*/
char* strreverse(char* s);
asm("
		.code
		.align		1
		.global		strreverse
strreverse:
		pushn		%r0
		ld.w		%r0, %r12		;//%r0  := s
		xcall.d		strchr			;//%r10 := p2 = strchr(s, '￥０')
		ld.w		%r13, 0			;//%r13 :=                '￥０'	*delay*
		jp.d		strreverse_START	;//goto START
		ld.w		%r4, %r0		;//%r4 := p1 = s			*delay*
strreverse_LOOP:					;//do {
		ld.b		%r5, [%r4]		;//  %r5  := c1 = *p1
		ld.b		%r6, [%r10]		;//  %r6  := c2 = *p2
		ld.b		[%r10], %r5		;//  *p2   = c1
		ld.b		[%r4]+, %r6		;//  *p1++ = c2
strreverse_START:					;//  
		sub		%r10, 1			;//  %r10 := p2--
		cmp		%r4, %r10		;//} while(p1 < p2)
		jrult		strreverse_LOOP		;//
		ld.w		%r10, %r0		;//%r10 := s
		popn		%r0
		ret
");
#endif/*PIECE*/

/*****************************************************************************
 *	単一化
 *****************************************************************************/

// * Wed Sep 07 21:52:10 JST 2016 Naoyuki Sawa
// - quark_from_string(),quark_to_string()を、32ビット版と64ビット版とで、別の実装にしました。
//   32ビット版はこれまで通り、単一化表現の文字列ポインタをそのまま(int)として返せば良いのですが、
//   64ビット版はポインタが64ビットのため、文字列ポインタをそのまま(int)として返せなくなったからです。
//   64ビット版は、単一化表現の文字列ポインタ⇔(int)を相互変換するための、新たな辞書を設ける事にしました。
#ifndef _WIN64
//32ビット版
int quark_from_string(const char* s) {
	return (int)intern_string(s);
}
const char* quark_to_string(int q) {
	return (const char*)q;
}
#else //_WIN64
//64ビット版
static ght_hash_table_t* ht_quark_from_string;	//Key=単一化表現の文字列ポインタ(const char*),Data=文字列の単一化表現の数値(int)	┬初回の登録時にquark_from_string()にて作成する。片方だけが作成される事は無い。
static ght_hash_table_t* ht_quark_to_string;	//Key=文字列の単一化表現の数値(int),Data=単一化表現の文字列ポインタ(const char*)	┘
int quark_from_string(const char* s) {
	//NULLポインタに対しては数値0を返す仕様です。
	int q = 0, n;
	if(s) {	//NULLポインタ以外ならば…
ENTER_CS;
		//ハッシュテーブルが作成されていなければ…
		if(!ht_quark_from_string) {
			//ハッシュテーブルを作成する。
			ght_set_rehash((ht_quark_from_string = ght_create(0)), 1);
			ght_set_rehash((ht_quark_to_string   = ght_create(0)), 1);
		}
		//単一化表現の文字列ポインタを取得する。
		s = intern_string(s);
		//文字列の長さを取得する。キーの末尾にヌル文字は不要なので、(+1)は不要です。
		n = strlen(s);
		//既に登録されていたら、単一化表現の数値を取得する。
		q = (intptr_t)ght_get(ht_quark_from_string, n, s);
		//まだ登録されていなければ…
		if(!q) {
			//単一化表現の数値を作成する。登録順に1,2,3,...とする事にした。(必須ではありません。)
			q = ght_size(ht_quark_from_string) + 1;
			//ハッシュテーブルに登録する。
			if(ght_insert(ht_quark_from_string, (void*)(intptr_t)q, n, s)) { DIE(); }	//Key=単一化表現の文字列ポインタ(const char*),Data=文字列の単一化表現の数値(int)
			if(ght_insert(ht_quark_to_string, (char*)s, sizeof q, &q)) { DIE(); }		//Key=文字列の単一化表現の数値(int),Data=単一化表現の文字列ポインタ(const char*)
		}	//                                ~~~~~~~このキャストは、const警告抑制のためです。
LEAVE_CS;
	}
	return q;
}
const char* quark_to_string(int q) {
	//数値0に対してはNULLポインタを返す仕様です。
	const char* s = NULL;
	if(q) {	//数値0以外ならば…
ENTER_CS;
		//ハッシュテーブルが作成されていなければ…
		if(!ht_quark_to_string) { DIE(); }	//単一化表現の数値が登録されていない状態で、数値0以外に対してquark_to_string()を呼び出してはいけない。ここで停止した場合は呼び出し側のバグです。
		//単一化表現の文字列ポインタを取得する。
		s = ght_get(ht_quark_to_string, sizeof q, &q);
		if(!s) { DIE(); }	//未登録の単一化表現の数値に対してquark_to_string()を呼び出してはいけない。ここで停止した場合は呼び出し側のバグです。
LEAVE_CS;
	}
	return s;
}
#endif//_WIN64
/*---------------------------------------------------------------------------*/
const char* intern_string(const char* s) {
//{{2016/05/06変更:intern_string()が、キーと値が同じメモリを指すようにして、メモリの無駄を低減しました。
//	//重複しない文字列を登録するハッシュテーブル。
//	// - キーと値に同じ文字列が格納されておりメモリの無駄だが、現在は処理の単純さを優先してこのままとしてある。	//┐
//	//   もし改善するには、新規文字列を登録した後にハッシュテーブルを走査してエントリのキーのアドレスを取得し、	//├→{{2016/05/06追記:左記の問題を解決しました。よく考えたらハッシュテーブルを走査しなくても、新しいエントリがイテレーション用のリストの末尾に追加される特性を利用すれば良い事に気付いたからです。詳細は、変更後の実装を参照して下さい。
//	//   データポインタがキーのアドレスを指すようにght_replace()で書き換えれば可能ではある。(かなり遅くなるが)	//│
//	//   もしそうする場合は、キーの文字列にヌル文字を含める必要が有るので、キーの長さを(+1)するように注意せよ。	//┘
//	static ght_hash_table_t* ht;
//	//ハッシュテーブルが未作成ならば…
//	if(!ht) {
//		//ハッシュテーブルを作成する。
//		ht = ght_create(0);	//初期サイズを無指定とする。
//		ght_set_rehash(ht, 1);	//自動リサイズを有効にする。
//	}
//	//文字列ポインタがNULLの場合は、そのままNULLを返す。そうでなければ…
//	if(s) {
//		//文字列の長さを取得する。キーの末尾にヌル文字は不要なので、(+1)は不要です。
//		int n = strlen(s);
//		//ハッシュテーブルから検索する。
//		char* t = ght_get(ht, n, s);
//		//登録されていたら、そのデータを返す。登録されていなければ…
//		if(!t) {
//			//文字列を複製する。
//			t = strdup(s);
//			//複製した文字列を、文字列自身をキーとして登録する。
//			if(ght_insert(ht, t, n, t)) { DIE(); }	//バグ
//		}
//		//ハッシュテーブル内に保持した文字列のデータを戻り値とする。
//		s = t;
//	}
//	return s;
//↓2016/05/06変更:intern_string()が、キーと値が同じメモリを指すようにして、メモリの無駄を低減しました。
	//重複しない文字列を登録するハッシュテーブル。
	// - intern_string()が、キーと値が同じメモリを指すようにして、メモリの無駄を低減しました。
	//   当関数の処理は、ght_hash_table.cの実装の、以下の特性に依存しています。			//┐
	//   │・ght_insert()で追加した新しいエントリは、イテレーション用のリストの末尾に追加される。	//│
	//   │・ght_hash_entry_tの実体はリハッシュが発生してもそのまま使用され、開放/再作成されない。	//├■■■注意：ght_hash_table_tの実装依存の処理です。■■■
	//   │・従って、ght_hash_entry_t.key_dataのアドレスは、ght_remove()を呼び出すまで不変である。	//│
	//   上記の特性を利用して、メモリ上に同じ内容の文字列を二重に保持してしまう無駄を省きました。	//┘
	static ght_hash_table_t* ht;
    //{{2016/05/30追加:intern_string()自身で排他制御を行うようにしました。これまでは、アプリケーションが注意してintern_string()を割り込みルーチンやマルチスレッド環境で使用しないようにしていましたが、intern_string()は様々なモジュールからも間接的に利用するので、アプリケーションが意図して割り込みルーチンやマルチスレッド環境で使用しないように注意するのは難しいと思ったからです。今後は、quark_from_string()/quark_to_string()/intern_string()や、これらを間接的に呼び出す各モジュールの関数を、割り込みルーチンやマルチスレッド環境で安全に使用出来るようになりました。
    ENTER_CS;
    //}}2016/05/30追加:intern_string()自身で排他制御を行うようにしました。これまでは、アプリケーションが注意してintern_string()を割り込みルーチンやマルチスレッド環境で使用しないようにしていましたが、intern_string()は様々なモジュールからも間接的に利用するので、アプリケーションが意図して割り込みルーチンやマルチスレッド環境で使用しないように注意するのは難しいと思ったからです。今後は、quark_from_string()/quark_to_string()/intern_string()や、これらを間接的に呼び出す各モジュールの関数を、割り込みルーチンやマルチスレッド環境で安全に使用出来るようになりました。
	//ハッシュテーブルが未作成ならば…
	if(!ht) {
		//ハッシュテーブルを作成する。
		ht = ght_create(0);	//初期サイズを無指定とする。
		ght_set_rehash(ht, 1);	//自動リサイズを有効にする。
	}
	//文字列ポインタがNULLの場合は、そのままNULLを返す。そうでなければ…
	if(s) {
		//文字列の長さを取得する。キーを値としても使用するのでキーの末尾にヌル文字が必要であり、(+1)が必要です。
		int n = strlen(s) + 1/*nul*/;
		//ハッシュテーブルから検索する。
		char* t = ght_get(ht, n, s);
		//登録されていたら、そのデータを返す。登録されていなければ…
		if(!t) {
			ght_hash_entry_t* e;
			//複製した文字列を、文字列自身をキーとして登録する。
			if(ght_insert(ht, (void*)s/*ダミー*/, n, s)) { DIE(); }	//バグ
			//新しいエントリは、イテレーション用のリストの末尾に追加されているはずである。	//┐
			// - ght_hash_table.cのght_insert()の実装,及び,2016/05/06コメントを参照せよ。	//│
			e = CONTAINING_RECORD(ht->le_iterator.Blink, ght_hash_entry_t, le_iterator);	//│
#ifdef  DEBUG												//│
			if(e->p_entry_data != s/*ダミー*/) { DIE(); }	//バグ				//│
#endif//DEBUG												//│
			//エントリ自身のキーを、ハッシュテーブル内に保持した文字列として取得する。	//│
			t = e->key_data;								//│
#ifdef  DEBUG												//├■■■注意：ght_hash_table_tの実装依存の処理です。■■■
			if(strcmp(s, t)) { DIE(); }	//バグ						//│
#endif//DEBUG												//│
			//エントリのデータを、エントリ自身のキーを指すように置き換える。		//│
		    //{{どちらでも同じ結果になります。後者の方が早いので、後者を使う事にしました。	//│
		    //	if(ght_replace(ht, t, n, s) != s/*ダミー*/) { DIE(); }	//バグ			//│
		    //│どちらでも同じ結果になります。後者の方が早いので、後者を使う事にしました。	//│
			e->p_entry_data = t;								//│
		    //}}どちらでも同じ結果になります。後者の方が早いので、後者を使う事にしました。	//┘
		}
		//ハッシュテーブル内に保持した文字列のデータを戻り値とする。
		s = t;
	}
    //{{2016/05/30追加:intern_string()自身で排他制御を行うようにしました。これまでは、アプリケーションが注意してintern_string()を割り込みルーチンやマルチスレッド環境で使用しないようにしていましたが、intern_string()は様々なモジュールからも間接的に利用するので、アプリケーションが意図して割り込みルーチンやマルチスレッド環境で使用しないように注意するのは難しいと思ったからです。今後は、quark_from_string()/quark_to_string()/intern_string()や、これらを間接的に呼び出す各モジュールの関数を、割り込みルーチンやマルチスレッド環境で安全に使用出来るようになりました。
    LEAVE_CS;
    //}}2016/05/30追加:intern_string()自身で排他制御を行うようにしました。これまでは、アプリケーションが注意してintern_string()を割り込みルーチンやマルチスレッド環境で使用しないようにしていましたが、intern_string()は様々なモジュールからも間接的に利用するので、アプリケーションが意図して割り込みルーチンやマルチスレッド環境で使用しないように注意するのは難しいと思ったからです。今後は、quark_from_string()/quark_to_string()/intern_string()や、これらを間接的に呼び出す各モジュールの関数を、割り込みルーチンやマルチスレッド環境で安全に使用出来るようになりました。
	return s;
//}}2016/05/06変更:intern_string()が、キーと値が同じメモリを指すようにして、メモリの無駄を低減しました。
}

/*****************************************************************************
 *	置換
 *****************************************************************************/

#ifndef PIECE
char* strreplace(const char* s, const char* oldstr, const char* newstr) {
	//元の文字列を、検索する文字列で分割する。
	char** argv = strsplit(s, oldstr, 0);
	//分割した文字列を、置換する文字列を挟んで連結する。
	char* t = strjoinv(newstr, argv);
	//分割した文字列を開放する。
	strfreev(argv);
	//連結した文字列を返す。
	return t;
}
#else //PIECE
char* strreplace(const char* s, const char* oldstr, const char* newstr);
asm("
		.code
		.align		1
		.global		strreplace
strreplace:
		pushn		%r0			;//
		ld.w		%r0, %r14		;//%r0  := newstr
		xcall.d		strsplit		;//%r10 := argv = strsplit(s, oldstr, 0)
		ld.w		%r14, 0			;//%r14 :=                            0		*delay*
		ld.w		%r12, %r0		;//%r12 :=              newstr
		ld.w		%r0, %r10		;//%r0  := argv
		xcall.d		strjoinv		;//%r10 := t = strjoinv(newstr, argv)
		ld.w		%r13, %r0		;//%r13 :=                      argv		*delay*
		ld.w		%r12, %r0		;//%r12 :=          argv
		xcall.d		strfreev		;//        strfreev(argv)
		ld.w		%r0, %r10		;//%r0  := t					*delay*
		ld.w		%r10, %r0		;//%r10 := t
		popn		%r0			;//
		ret					;//return  t
");
#endif//PIECE

/*****************************************************************************
 *	メモリ複製
 *****************************************************************************/

#ifndef PIECE
void* memdup(const void* src, size_t n) {
	void* dest = malloc(n);
	if(dest) { dest = memcpy(dest, src, n); }
	return dest;
}
#else //PIECE
void* memdup(const void* src, size_t n);
asm("
		.code
		.align		1
		.global		memdup
memdup:
		;//%r12 := src
		;//%r13 := n
		pushn		%r1			;//
		ld.w		%r0, %r12		;//%r0  := src
		ld.w		%r1, %r13		;//%r1  := n
		xcall.d		malloc			;//%r10 := dest = malloc(n)
		ld.w		%r12, %r13		;//%r12 :=               n			*delay*
		cmp		%r10, 0			;//if(dest) {
		jreq		memdup_RET		;//  
		ld.w		%r13, %r0		;//  %r13 :=                     src
		ld.w		%r14, %r1		;//  %r14 :=                          n
		xcall.d		memcpy			;//  %r10 := dest = memcpy(dest, src, n)
		ld.w		%r12, %r10		;//  %r12 :=               dest			*delay*
memdup_RET:						;//}
		popn		%r1			;//
		ret					;//return dest
");
#endif//PIECE

/*****************************************************************************
 *	メモリローテート
 *****************************************************************************/
// * Mon Jun 06 21:45:00 JST 2016 Naoyuki Sawa
// - memrotate()を追加しました。
// * Tue Jun 07 22:13:11 JST 2016 Naoyuki Sawa
// - memrotate()の実装を変更しました。
//   詳細は、clipstl.cモジュールの、std_rotate()関数のコメントを参照して下さい。
//   memrotate()の関数仕様は変わっていません。
#define USE_MEMROTATE_STD_ROTATE	//このシンボルを定義すると、std_rotate()を呼び出すバージョンのmemrotate()を使用します。このシンボルを定義しないと、独自アルゴリズムの素直な実装バージョンを使用します。		2016/06/07現在は、std_rotate()を呼び出すバージョンのmemrotate()を使用するようにしています。今後、変更する事は無いと思います。
#ifndef USE_MEMROTATE_STD_ROTATE
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
//□独自アルゴリズムの素直な実装。2016/06/06に作成し、2016/06/06の一日間しか使用しませんでしたが、C言語版・アセンブラ版共に(clipmemc.cモジュールのMemCpt_Resize()を使って)十分検証したので、動作は正しいと思います。2016/06/07にmemrotate()をstd_rotate()を使うように変更して、もうこちらのバージョンを使う事は無いと思いますが、一応残しておきます。
#ifndef PIECE
void* memrotate(void* _base, int n, size_t size) {	//直観的には'memrotate(base,size,n)'の引数順の方が自然な気もするが、memset()等の引数仕様に合わせてsize引数を最後にする事にした。
	char* const base = _base;
	int i, m;
	//一時バッファ。このサイズを増やすほど高速になります。実行環境によってスタックが溢れない程度で、なるべく大きなサイズにするよう調整して下さい。
#ifdef  WIN32
	char tmp[4096/*調整可*/];	//Windowsの場合
#else //WIN32
	char tmp[256/*調整可*/];	//P/ECEやその他の組み込み環境の場合
#endif//WIN32
	//メモリブロックのサイズが0ならば、何もしない。そうでなければ…
	if(size) {
		//ローテート数を、-(メモリブロックのサイズ-1)～+(メモリブロックのサイズ-1) に補正する。
		n %= (int)size;
		//ローテート数が0になったら、何もしない。そうでなければ…
		if(n) {
			//後方ローテートする場合のローテート数 1～(メモリブロックのサイズ-1) を求める。
			if(n < 0) { n += size; }
			//前方ローテートする場合のローテート数 1～(メモリブロックのサイズ-1) を求める。
			m = size - n;
			//後方ローテートの方がローテート数が少なければ、後方ローテートを行う。
			if(n < m) {
				do {
					//今回のローテート数を求める。
					i = (n <= sizeof tmp) ? n : sizeof tmp;
					//メモリブロックの末尾の、今回のローテート数分のデータを、一時バッファに退避する。
					//
					//	base
					//	┌───────────────┐
					//	│			│	│
					//	└───────────────┘
					//				　↓↓↓
					//				┌───┐
					//				│tmp	│
					//				└───┘
					//
					memcpy(tmp, &base[size - i], i);
					//メモリブロックを、今回のローテート数分、後方へシフトする。
					//
					//	base
					//	┌───────────────┐
					//	│→→→│			│
					//	└───────────────┘
					//
					//				┌───┐
					//				│tmp	│
					//				└───┘
					//
					memmove(&base[i], &base[0], size - i);
					//一時バッファに退避したデータを、メモリブロックの先頭に復帰する。
					//
					//	base
					//	┌───────────────┐
					//	│	│			│
					//	└───────────────┘
					//	　↑↑↑
					//	　│││		┌───┐
					//	　└┴┴────────│tmp	│
					//				└───┘
					//
					memcpy(&base[0], tmp, i);
				} while(n -= i);	//残りローテート数を減らし、0になるまで繰り返す。
			//前方ローテートの方がローテート数が少なければ、前方ローテートを行う。		後方ローテートと前方ローテートのローテート数が同じ場合はどちらでも良いが、前方ローテートの方がmemmove()がmemcpy()呼び出しになって高速なので、同じ場合は前方ローテートを使うことにした。(cliplibc.cのアセンブラ版memmove()の実装を参照せよ。)
			} else {
				do {
					//今回のローテート数を求める。
					i = (m <= sizeof tmp) ? m : sizeof tmp;
					//メモリブロックの先頭の、今回のローテート数分のデータを、一時バッファに退避する。
					//
					//	base
					//	┌───────────────┐
					//	│	│		│	│
					//	└───────────────┘
					//	　↓↓↓
					//	┌───┐
					//	│tmp	│
					//	└───┘
					//
					memcpy(tmp, &base[0], i);
					//メモリブロックを、今回のローテート数分、前方へシフトする。
					//
					//	base
					//	┌───────────────┐
					//	│			│←←←│
					//	└───────────────┘
					//
					//	┌───┐
					//	│tmp	│
					//	└───┘
					//
					memmove(&base[0], &base[i], size - i);
					//一時バッファに退避したデータを、メモリブロックの末尾に復帰する。
					//
					//	base
					//	┌───────────────┐
					//	│			│	│
					//	└───────────────┘
					//				　↑↑↑
					//	┌───┐		　│││
					//	│tmp	│────────┴┴┘
					//	└───┘
					//
					memcpy(&base[size - i], tmp, i);
				} while(m -= i);	//残りローテート数を減らし、0になるまで繰り返す。
			}
		}
	}
	return base;	//メモリブロックの先頭アドレスをそのまま返す。特に意味は無いが、memset()等の戻り値仕様に合わせる事にした。
}
#else //PIECE
void* memrotate(void* _base, int n, size_t size);
asm("
		.code
		.align		1
		.global		memrotate
memrotate:
		;//%r12 := base
		;//%r13 := n
		;//%r14 := size
		pushn		%r3
		cmp		%r14, 0				;//if(!size) { goto NOJOB }
		jreq.d		memrotate_NOJOB			;//
		ld.w		%r0, %r12			;//%r0  := base					*delay*
		ld.w		%r1, %r14			;//%r1  := size
		ld.w		%r12, %r13			;//%r12 := n
		xcall.d		__modsi3			;//%r10 := n %= (int)size
		ld.w		%r13, %r1			;//%r13 :=           size			*delay*
		cmp		%r10, 0				;//%psr := n <=> 0
		jrgt.d		4				;//if(n <= 0) {
		 ld.w		%r2, %r1			;//%r2  :=     size				*delay*
		 jreq		memrotate_NOJOB			;//  if(!n) { goto NOJOB }
		 add		%r10, %r1			;//  %r10 := n += size }
		sub		%r2, %r10			;//%r2  := m = size - n
		cmp		%r10, %r2			;//if(n < m) {
		jrge.d		memrotate_LOOP2			;//
		sub		%sp, 256/4			;//%sp  := tmp					*delay*		↓↓↓↓↓スタック上にtmp[256]を確保する区間:ここから↓↓↓↓↓
		;//---------------------------------------------;//
		ld.w		%r2, %r10			;//  %r2  := n
		;//%r0  := base
		;//%r1  := size
		;//%r2  := n
		;//%sp  := tmp
memrotate_LOOP1:						;//  do {
		xcmp		%r2, 256			;//    if(n > sizeof tmp) {
		jrle.d		4				;//      
		 ld.w		%r3, %r2			;//    %r3  := i = n				*delay*
		 ext		     256^M			;//      %r3  := i = sizeof tmp
		 ld.w		%r3, 256^L			;//    }
		ld.w		%r12, %sp			;//    %r12 := tmp
		ld.w		%r13, %r0			;//    %r13 :=       base
		add		%r13, %r1			;//    %r13 :=      &base[size    ]
		sub		%r13, %r3			;//    %r13 :=      &base[size - i]
		xcall.d		memcpy				;//    memcpy( tmp, &base[size - i], i)
		ld.w		%r14, %r3			;//    %r14 :=                       i		*delay*
		ld.w		%r12, %r0			;//    %r12 :=  base
		ld.w		%r13, %r12			;//    %r13 :=           &base[0]
		add		%r12, %r3			;//    %r12 := &base[i]
		ld.w		%r14, %r1			;//    %r14 :=                     size
		xcall.d		memmove				;//    memmove(&base[i], &base[0], size - i)
		sub		%r14, %r3			;//    %r14 :=                     size - i	*delay*
		ld.w		%r12, %r0			;//    %r12 := &base[0]
		ld.w		%r13, %sp			;//    %r13 :=           tmp
		xcall.d		memcpy				;//    memcpy( &base[0], tmp, i)
		ld.w		%r14, %r3			;//    %r14 :=                i			*delay*
		sub		%r2, %r3			;//    %r2  := n -= i
		jrne		memrotate_LOOP1			;//  } while(n)
		jp		memrotate_RET			;//  goto RET
		;//---------------------------------------------;//} else {
		;//%r0  := base
		;//%r1  := size
		;//%r2  := m
		;//%sp  := tmp
memrotate_LOOP2:						;//  do {
		xcmp		%r2, 256			;//    if(m > sizeof tmp) {
		jrle.d		4				;//      
		 ld.w		%r3, %r2			;//    %r3  := i = m				*delay*
		 ext		     256^M			;//      %r3  := i = sizeof tmp
		 ld.w		%r3, 256^L			;//    }
		ld.w		%r12, %sp			;//    %r12 := tmp
		ld.w		%r13, %r0			;//    %r13 :=     &base[0]
		xcall.d		memcpy				;//    memcpy(tmp, &base[0], i)
		ld.w		%r14, %r3			;//    %r14 :=               i			*delay*
		ld.w		%r12, %r0			;//    %r12 := &base[0]
		ld.w		%r13, %r12			;//    %r13 :=            base
		add		%r13, %r3			;//    %r12 :=           &base[i]
		ld.w		%r14, %r1			;//    %r14 :=                     size
		xcall.d		memmove				;//    memmove(&base[0], &base[i], size - i)
		sub		%r14, %r3			;//    %r14 :=                     size - i	*delay*
		ld.w		%r12, %r0			;//    %r12 :=  base
		add		%r12, %r1			;//    %r12 := &base[size    ]
		sub		%r12, %r3			;//    %r12 := &base[size - i]
		ld.w		%r13, %sp			;//    %r13 :=                  tmp
		xcall.d		memcpy				;//    memcpy( &base[size - i], tmp, i)
		ld.w		%r14, %r3			;//    %r14 :=                       i		*delay*
		sub		%r2, %r3			;//    %r2  := m -= i
		jrne		memrotate_LOOP2			;//  } while(m)
		;//---------------------------------------------;//}
memrotate_RET:							;//
		xadd		%sp, %sp, 256			;//								↑↑↑↑↑スタック上にtmp[256]を確保する区間:ここまで↑↑↑↑↑
memrotate_NOJOB:						;//
		ld.w		%r10, %r0			;//%r10 := base
		popn		%r3				;//
		ret						;//return  base
");
#endif//PIECE
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#else //USE_MEMROTATE_STD_ROTATE
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
//□std_rotate()を呼び出すバージョン	2016/06/07～使用開始
void* memrotate(void* _base, int n, size_t size) {	//直観的には'memrotate(base,size,n)'の引数順の方が自然な気もするが、memset()等の引数仕様に合わせてsize引数を最後にする事にした。
	char* const base = _base;
	//メモリブロックのサイズが0ならば、何もしない。そうでなければ…
	if(size) {
		//ローテート数を、-(メモリブロックのサイズ-1)～+(メモリブロックのサイズ-1) に補正する。
		n %= (int)size;
		//ローテート数が0になったら、何もしない。そうでなければ…
		if(n) {
			//後方ローテートする場合のローテート数 1～(メモリブロックのサイズ-1) を求める。
			if(n < 0) { n += size; }
			//前方ローテートする場合のローテート数 1～(メモリブロックのサイズ-1) を求める。
			n = size - n;
			//std_rotate()への第2引数はローテート後に先頭になる要素へのポインタを指定する，即ち，(前方ローテート数)番目の要素へのポインタです。
			//
			//	base	&base[n]		&base[size]
			//	┌───┬───────────┐
			//	│	│			│
			//	└───┴───────────┘
			//	│←── n
			//
			std_rotate(base, &base[n], &base[size]);
		}
	}
	return base;	//メモリブロックの先頭アドレスをそのまま返す。特に意味は無いが、memset()等の戻り値仕様に合わせる事にした。
}
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#endif//USE_MEMROTATE_STD_ROTATE

/*****************************************************************************
 *	変換
 *****************************************************************************/

#ifdef  WIN32
char* StrConv(const char* str, int conv) {
	char* buf;
	int size, flag = 0;
	//VBAのStrConv()互換のフラグを、Win32のLCMapString()互換のフラグに変換する。
	if(conv & vbUpperCase) { flag |= LCMAP_UPPERCASE; }	//Use uppercase.
	if(conv & vbLowerCase) { flag |= LCMAP_LOWERCASE; }	//Use lowercase.
	if(conv & vbWide     ) { flag |= LCMAP_FULLWIDTH; }	//Wide characters (where applicable).
	if(conv & vbNarrow   ) { flag |= LCMAP_HALFWIDTH; }	//Narrow characters (where applicable).
	if(conv & vbKatakana ) { flag |= LCMAP_KATAKANA;  }	//Katakana.
	if(conv & vbHiragana ) { flag |= LCMAP_HIRAGANA;  }	//Hiragana.
	//この文字列の変換に必要なサイズを取得する。
	// - 変換に必要なサイズを取得する場合は、最後の引数に0を指定する必要がある。
	//   暫定の出力バッファサイズを指定して、足りるならば一度で済ませようとすると、エラー(0)が返され、必要なサイズを取得できない。
	size = LCMapString(GetSystemDefaultLCID(), flag, str, -1, NULL, 0);
	if(!size) { DIE(); }
	//この文字列の変換に必要なサイズの、出力バッファを割り当てる。
	// - LCMapString APIが返す、変換に必要なサイズは、終端のnulを含んでいる。
	//   アプリケーションプログラム側で、割り当てサイズを(+1)する必要は無い。
	buf = malloc(size);
	if(!buf) { DIE(); }
	//この文字列を変換する。
	size = LCMapString(GetSystemDefaultLCID(), flag, str, -1, buf, size);
	if(!size) { DIE(); }
	//変換した文字列を返す。
	// - 呼び出し側でfree()して開放せよ。
	return buf;
}
#endif//WIN32
