//
//	TinyJS - A single-file Javascript-alike engine
//
//	Authored By Gordon Williams <gw@pur3.co.uk>
//	Copyright (C) 2009 Pur3 Ltd
//
//	Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//	The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
//
//	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
//	IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//-----------------------------------------------------------------------------
//
//	TinyJS(�ڐA��)
//
//	Ported By Naoyuki Sawa <nsawa@piece-me.org>
//	Copyright (C) 2018 Piece Lab.
//
//	* Fri Mar 30 23:59:59 JST 2018 Naoyuki Sawa
//	- 1st �����[�X�B
//	- �uTinyJS�v(https://github.com/gfwilliams/tiny-js)���AP/ECE�œ��삷��悤�ɈڐA�������̂ł��B
//	  �I���W�i���ł�TinyJS��C++�ŏ�����Ă��܂����AP/ECE�J�����ɂ�GCC 2.7.2������C�R���p�C�����������̂ŁAC++����C����ɈڐA���܂����B
//	  C++��C����͂����Ԃ�Ⴄ���߁A�R�[�h�͂قڑS�ď��������Ă��܂��܂������A�A���S���Y���̓I���W�i���ł�TinyJS�Ƃ������������܂܂ł��B
//	  ���A���C���^�[�Q�b�g��P/ECE�ł����A(���Ȃ��Ƃ������_�ł�)Visual Studio 2017�ƁAVisual C++ 6.0�ł��r���h�o���Ă��܂��B
//	- �I���W�i���ł�TinyJS�͖����I�ȃ������Ǘ����s���Ă��܂����A�ڐA�łł̓K�[�x�[�W�R���N�^�[�̎g�p��O��Ƃ��ă������Ǘ����Ȃ��܂����B
//	  P/ECE�̏ꍇ��clipbmgc.c,����,clipgc.c���W���[�����CWindows�̏ꍇ��Boehm GC���g�p���ĉ������B
//	- �I���W�i���ł�TinyJS���A/clip/keep/tiny-js-master.7z �ɕۑ����Ă����܂����B
//	  (Latest commit 56a0c6d on Mar 24 2015�B2018/03/30���_�̍ŐV���r�W�����ł��B)
//
#include "clip.h"
//-----------------------------------------------------------------------------
//TinyJS.c���W���[������������o�͂�����@���A�v���P�[�V�������ɕύX�o����悤�ɂ��邽�߂ɁATinyJS_TRACE()�֐�������ʃ��W���[��(TinyJS_TRACE.c)�ɕ������܂����B
//�I���W�i���ł�TinyJS�ł́ATRACE�}�N���Ƃ��ă��C�u�����̃r���h���ɕύX�o����悤�ɂȂ��Ă��܂������A���C�u�����̃r���h���ł͂Ȃ��A�v���P�[�V�����̃r���h���ɕύX�������Ǝv��������ł��B
//�f�t�H���g��TinyJS_TRACE()�́ATinyJS_TRACE.c�ɂāAstdout�ɕ�������o�͂��邾���̊֐��Ƃ��Ē�`���Ă���܂��B
//�A�v���P�[�V��������TinyJS_TRACE()���Ē�`����ƁATinyJS_TRACE.c�̓����N���ꂸ�A�A�v���P�[�V�������Œ�`����TinyJS_TRACE()���Ăяo����܂��B
//TinyJS_TRACE()���Ē�`�o����悤�ɂ������R�́A��ɁAP/ECE���̂��߂ł��B(P/ECE���ł́Astdout��USB�ɑ��o����āA��ʂɂ͕\������Ȃ�����B)
//Windows���̏ꍇ�́A�f�t�H���g�̓���̂܂܂œ��ɕs�s���͖����̂ŁA������TinyJS_TRACE()���Ē�`����K�v�͂��܂薳���Ǝv���܂��B
void TinyJS_TRACE(const char* msg) {
	fputs(msg, stdout);
}
