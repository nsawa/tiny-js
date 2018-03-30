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
//
#ifndef __TINYJS_FUNCTIONS_H__
#define __TINYJS_FUNCTIONS_H__
#ifdef  __cplusplus
extern "C" {
#endif//__cplusplus
void TinyJS_registerFunctions(ST_TinyJS* tinyJS);	//Register useful functions with the TinyJS interpreter.
#ifdef  __cplusplus
}//extern "C"
#endif//__cplusplus
#endif//__TINYJS_FUNCTIONS_H__
