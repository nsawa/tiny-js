#include "clip.h"
//*****************************************************************************
//	
//*****************************************************************************
//clip/clipprnt.c
int asprintf(char** strp, const char* fmt, ...) {
	int n;
	va_list ap;
	va_start(ap, fmt);
	n = vasprintf(strp, fmt, ap);
	va_end(ap);
	return n;
}
int vasprintf(char** strp, const char* fmt, va_list ap) {
	va_list ap2;
	va_copy(ap2, ap);					//apをap2にコピーする。
	*strp = malloc(vscprintf(fmt, ap2) + 1/*nul*/);		//ap2はvscprintf()によって変更されるので、このあとvsprintf()に渡してはいけない。	P/ECE開発環境(GCC)とWindows(VC++6.0)では値渡しなので問題無いのだが、処理系依存であり厳密にはこのように処理する必要が有る。
	va_end(ap2);						//ap2の使用を終了する。
	if(!*strp) { return -1; }
	return vsprintf(*strp, fmt, ap);			//変更していない元のapをvsprintf()に渡す。
}
