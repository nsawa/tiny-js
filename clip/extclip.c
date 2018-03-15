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
	va_copy(ap2, ap);					//ap��ap2�ɃR�s�[����B
	*strp = malloc(vscprintf(fmt, ap2) + 1/*nul*/);		//ap2��vscprintf()�ɂ���ĕύX�����̂ŁA���̂���vsprintf()�ɓn���Ă͂����Ȃ��B	P/ECE�J����(GCC)��Windows(VC++6.0)�ł͒l�n���Ȃ̂Ŗ�薳���̂����A�����n�ˑ��ł��茵���ɂ͂��̂悤�ɏ�������K�v���L��B
	va_end(ap2);						//ap2�̎g�p���I������B
	if(!*strp) { return -1; }
	return vsprintf(*strp, fmt, ap);			//�ύX���Ă��Ȃ�����ap��vsprintf()�ɓn���B
}
