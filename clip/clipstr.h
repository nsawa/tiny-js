/*	
 *	clipstr.h
 *
 *	���[�e�B���e�B�F�����񑀍�
 *
 *	CLiP - Common Library for P/ECE
 *	Copyright (C) 2001-2016 Naoyuki Sawa
 *
 *	* Sat Oct 11 12:29:17 JST 2014 Naoyuki Sawa
 *	- �쐬�J�n�B
 *	- strtrim(),strescape(),strcompress()���Aclipmisc.h,c����clipstr.h,c�ֈړ����܂����B
 *	- strfreev(),strv_length(),strdup_printf(),strdup_vprintf(),strjoin(),strjoinv(),strsplit(),strsplit_set()��ǉ����܂����B
 *	* Mon Oct 13 02:54:55 JST 2014 Naoyuki Sawa
 *	- str_has_prefix(),str_has_suffix()��ǉ����܂����B
 *	* Sat Oct 18 18:55:59 JST 2014 Naoyuki Sawa
 *	- string_match()��ǉ����܂����B
 *	* Thu Nov 06 00:58:25 JST 2014 Naoyuki Sawa
 *	- strreverse()��ǉ����܂����B
 *	* Sat Nov 08 16:50:24 JST 2014 Naoyuki Sawa
 *	- strdupv()��ǉ����܂����B
 *	* Tue Nov 11 23:26:40 JST 2014 Naoyuki Sawa
 *	- strconcat()��ǉ����܂����B
 *	* Sat Nov 15 20:39:46 JST 2014 Naoyuki Sawa
 *	- str_has_prefix_suffix�̃o�O���C�����܂����B
 *	  str_has_prefix_suffix�̒���%r3���g�p���Ă���̂ɁA%r0�`%r2�����ޔ����Ă��Ȃ��o�O�ł����B
 *	  ���̂��߂ɁAstr_has_prefix_suffix���Ăяo�����W���[�����A�s���ȓ�����N�����Ă��܂����B
 *	  ��̓I�ɂ́Acliptcl.c��Tcl_GetIndex()���A�A�h���X�G���[�ŕs���I�����Ă��܂��Ă��܂����B
 *	* Sat Mar 07 16:39:40 JST 2015 Naoyuki Sawa
 *	- strstrip(),strchug(),strchomp()��ǉ����܂����B
 *	* Sat Mar 07 23:27:25 JST 2015 Naoyuki Sawa
 *	- textwrap()��ǉ����܂����B
 *	* Sun Mar 08 10:55:52 JST 2015 Naoyuki Sawa
 *	- strsplit_set()�ƃr�b�g�}�b�vGC�𕹗p���A������x�傫�ȃt�@�C���𕪊����悤�Ƃ���ƁA�r�b�g�}�b�vGC���G���[��~���鎖���L��܂��B
 *	  ���̖��ɂ��Ē����������ʂ��Aclipstr.c��strsplit_set()�֐��̏�ɒǋL���܂����B
 *	* Mon Mar 09 23:08:36 JST 2015 Naoyuki Sawa
 *	- textwrap()���A�Z���u�������܂����B
 *	* Sun Jul 19 12:59:05 JST 2015 Naoyuki Sawa
 *	- quark_from_string(),quark_to_string(),intern_string()��ǉ����܂����B
 *	* Thu Jul 23 21:03:39 JST 2015 Naoyuki Sawa
 *	- strreplace()��ǉ����܂����B
 *	* Wed Aug 12 21:46:27 JST 2015 Naoyuki Sawa
 *	- memdup()��ǉ����܂����B
 *	* Thu Oct 15 00:58:32 JST 2015 Naoyuki Sawa
 *	- strnatcmp(),strnatcasecmp()��ǉ����܂����B
 *	* Thu Oct 15 22:15:08 JST 2015 Naoyuki Sawa
 *	- strnatcmp(),strnatcasecmp()���A�Z���u�������܂����B�R�[�h�T�C�Y��80�o�C�g����܂����B
 *	  �Ƃ肠�����AC�����,�A�Z���u���ł̗����ŁA�e�X�g�X�C�[�g���p�X���鎖�͊m�F�����̂ł����A�܂��A�Z���u���ł̓���m�F���s�\���ł��B
 *	* Fri Oct 16 21:16:43 JST 2015 Naoyuki Sawa
 *	- strnatcmp(),strnatcasecmp()�̓����ύX���܂����B
 *	  �ڍׂ́Aclipstr.h��strnatcmp(),strnatcasecmp()�̃R�����g���Q�Ƃ��ĉ������B
 *	* Wed Nov 18 21:37:53 JST 2015 Naoyuki Sawa
 *	- 'extern "C" {'�`'}'�ň݂͂܂����B.cpp���܂�Win32�v���W�F�N�g������C���N���[�h�o����悤�ɂ��邽�߂ł��B
 *	* Tue Jan 19 21:44:45 JST 2016 Naoyuki Sawa
 *	- StrConv()��ǉ����܂����B
 *	* Sat Apr 09 21:06:53 JST 2016 Naoyuki Sawa
 *	- strdup_splitpath()��strdup_makepath()��ǉ����܂����B
 *	  splitpath()��makepath()���Astrdup_printf()�Ɠ��l�Ɏg����ƕ֗��Ȃ̂ŁA�����̔��f��'strdup_*()'�o�[�W������ǉ����鎖�ɂ��܂����B
 *	* Fri May 06 21:58:10 JST 2016 Naoyuki Sawa
 *	- intern_string()���A�L�[�ƒl���������������w���悤�ɂ��āA�������̖��ʂ�ጸ���܂����B
 *	* Sat May 21 22:36:02 JST 2016 Naoyuki Sawa
 *	- strsplit()��strsplit_set()���}���`�o�C�g�Ή����܂����B
 *	  �Â��R�[�h�ł͔z��̃������m�ۂ��֐����ōs���Ă��܂������A�V�����R�[�h�ł�strv.c�̋@�\���g���悤�ɕύX���A�R�[�h���Ȍ��ɂȂ�܂����B
 *	  �R�[�h���Ȍ��ɂȂ������Ƃɂ���āA�}���`�o�C�g�Ή���ǉ������ɂ�������炸�A�����W���[���̃R�[�h�T�C�Y����24�o�C�g�������Ȃ�܂����B
 *	  �������Astrv.o�������N�����悤�ɂȂ������R�[�h�T�C�Y����������̂ŁA�S�̂Ƃ��Ă̓R�[�h�T�C�Y�͑������Ă���Ǝv���܂��B
 *	- �V�����R�[�h�Ŏ�����啝�ɕύX���܂������A��{�I�ȃA���S���Y���͌Â��R�[�h�ƕς���Ă��܂���B
 *	  �V�����R�[�h�ɂ͏ڍׂȃR�����g��t���Ă��Ȃ��̂ŁA�A���S���Y���ɂ��Ă͌Â��R�[�h�̃R�����g���Q�Ƃ��ĉ������B
 *	- �}���`�o�C�g��Ή��̌Â��R�[�h���A����܂Œ������肵�ē��삵�Ă������т��L��̂ŁA�R�[�h���c���Ă������ɂ��܂����B
 *	  USE_STRSPLIT_MBCS�V���{�����`���ă��C�u�������r���h����ƁA�V�����}���`�o�C�g�Ή��ł�strsplit()��strsplit_set()���g�p���܂��B
 *	  USE_STRSPLIT_MBCS�V���{�����`�����Ƀ��C�u�������r���h����ƁA�Â��}���`�o�C�g��Ή��ł�strsplit()��strsplit_set()���g�p���܂��B
 *	  ��{�I�ɂ́A�V�����}���`�o�C�g�Ή��ł�strsplit()��strsplit_set()���g���\��ł����A��������}���`�o�C�g�Ή����s�v�ɂȂ����ꍇ�́A�Â��R�[�h�ɖ߂��ĉ������B
 *	- �����؃v���O����
 *	  ��void test1() {
 *	  ��  char** s = strsplit("�\��\\�\��\\�\��", "\\", 0);
 *	  ��  char** t = s;
 *	  ��  while(*t) { printf("'%s'\n", *t++); }
 *	  ��  strfreev(s);
 *	  ��}
 *	  ��//������(�}���`�o�C�g��Ή���)�c���
 *	  ��//��'\x95'  �u�\�v��2�o�C�g�ڂ�'\'���}�b�`���Ă��܂��Ă���B
 *	  ��//��'��'
 *	  ��//��'\x8F'  �u�\�v��2�o�C�g�ڂ�'\'���}�b�`���Ă��܂��Ă���B
 *	  ��//��'��'
 *	  ��//��'\x97'  �u�\�v��2�o�C�g�ڂ�'\'���}�b�`���Ă��܂��Ă���B
 *	  ��//��'��'
 *	  ��//������(�}���`�o�C�g�Ή���)�c������
 *	  ��//��'�\��'
 *	  ��//��'�\��'
 *	  ��//��'�\��'
 *	  ��void test2() {
 *	  ��  char** s = strsplit_set("�\��\\�\��\\�\��", "�\�\�\", 0);
 *	  ��  char** t = s;
 *	  ��  while(*t) { printf("'%s'\n", *t++); }
 *	  ��  strfreev(s);
 *	  ��}
 *	  ��//������(�}���`�o�C�g��Ή���)�c���
 *	  ��//��''      �u�\�v��1�o�C�g�ڂ��ʂɁu�\�v��1�o�C�g�ڂɃ}�b�`���Ă��܂��Ă���B
 *	  ��//��''      �u�\�v��2�o�C�g�ڂ��ʂɁu�\�v��2�o�C�g�ڂɃ}�b�`���Ă��܂��Ă���B
 *	  ��//��'��'    �u�\�v���́u�\�v���́u�\�v��2�o�C�g�ڂ�'\'�Ƀ}�b�`���Ă��܂��Ă���B
 *	  ��//��''      �u�\�v��1�o�C�g�ڂ��ʂɁu�\�v��1�o�C�g�ڂɃ}�b�`���Ă��܂��Ă���B
 *	  ��//��''      �u�\�v��2�o�C�g�ڂ��ʂɁu�\�v��2�o�C�g�ڂɃ}�b�`���Ă��܂��Ă���B
 *	  ��//��'��'    �u�\�v���́u�\�v���́u�\�v��2�o�C�g�ڂ�'\'�Ƀ}�b�`���Ă��܂��Ă���B
 *	  ��//��''      �u�\�v��1�o�C�g�ڂ��ʂɁu�\�v��1�o�C�g�ڂɃ}�b�`���Ă��܂��Ă���B
 *	  ��//��''      �u�\�v��2�o�C�g�ڂ��ʂɁu�\�v��2�o�C�g�ڂɃ}�b�`���Ă��܂��Ă���B
 *	  ��//��'��'    �u�\�v���́u�\�v���́u�\�v��2�o�C�g�ڂ�'\'�Ƀ}�b�`���Ă��܂��Ă���B
 *	  ��//������(�}���`�o�C�g�Ή���)�c������
 *	  ��//��''
 *	  ��//��'��\'
 *	  ��//��'��\'
 *	  ��//��'��'
 *	* Sun May 22 21:58:55 JST 2016 Naoyuki Sawa
 *	- textwrap()�̎�����ύX���܂����B���ʂ͕ς��܂���B
 *	  �ύX�_�͈ȉ��̒ʂ�ł��B
 *	- �ύX�O�́Atextwrap()�̒��ŕ�����z��̃������m�ۂ��s���Ă��܂����B
 *	  �ύX��́Astrv.c���W���[���̋@�\���g���ĕ�����z��̃������m�ۂ��s���悤�ɂ��܂����B
 *	  ���̕ύX�ɂ���āA�֐��̎������P���ɂȂ�A�R�[�h�T�C�Y����44�o�C�g�������Ȃ�܂����B
 *	  strv.o�������N���邽�߂̃R�[�h�T�C�Y�����͗L��܂����A����strstrip()��strv.o���g�p���Ă���̂ŁA�V���ȑ����͗L��܂���B
 *	- �ύX�O�̃R�[�h���A����܂ň��肵��(�c�Ƃ������p�ɂɂ͎g���Ă��܂���ł������c)���삵�Ă����̂ŁA#if 0�`�Ő؂蕪���Ďc���Ă���܂��B
 *	  �ύX��̃R�[�h�ɂ́A�ύX�O�̃R�[�h�قǏڍׂȃR�����g�͕t���Ă��Ȃ��̂ŁA�A���S���Y���ɂ��Ă͕ύX�O�̃R�[�h�̃R�����g���Q�Ƃ��ĉ������B
 *	  �ύX��̃R�[�h���A�ύX�O�̃R�[�h�Ɠ����A���S���Y���ŁA������z��̃��������m�ۂ�����@���ς���������ł��B
 *	* Wed May 25 10:22:02 JST 2016 Naoyuki Sawa
 *	- C����ł�strcompress()���}���`�o�C�g�Ή����܂����B
 *	  strcompress()���}���`�o�C�g�Ή������ړI�́Aclipyaml.c���W���[����yaml_path_get_node(),yaml_path_set_string()�Ŏg�p���邽�߂ł��B
 *	  �ڍׂ́Aclipyaml.c���W���[���̓����̃R�����g���Q�Ƃ��ĉ������B
 *	- �A�Z���u���ł�strcompress()�͂܂��A�}���`�o�C�g�Ή����Ă��܂���B
 *	  �A�Z���u���ł�strcompress()���A�}���`�o�C�g�Ή�����K�v���L��܂��B	��{{2016/05/26�R�����g�ǋL:�A�Z���u���ł�strcompress()���}���`�o�C�g�Ή����܂����B}}
 *	- strcompress()��strescape()�Ƒ΂𐬂��֐��ł����A����strcompress()�������}���`�o�C�g�Ή��������ɂ��Astrcompress()��strescape()�̔�Ώ̐��������܂����B
 *	  ���p�����������g������͖��͐����܂��񂪁A�S�p�������g�p����ƁAstrescape()�ŃG�X�P�[�v�����������strcompress()�ŕ����������ɁA���̕�����ɂȂ�Ȃ��\�����L��܂��B
 *	  �Ώ̐���ۂ��߂ɁAstrescape()���}���`�o�C�g�Ή�����K�v���L��܂��B	��{{2016/05/26�R�����g�ǋL:strescape()���}���`�o�C�g�Ή����܂����B}}
 *	* Thu May 26 20:37:02 JST 2016 Naoyuki Sawa
 *	- �A�Z���u���ł�strcompress()���}���`�o�C�g�Ή����܂����B
 *	  �ȉ��̓���m�F���s���܂����B
 *	  ��//���L�̃e�X�g�֐����痘�p����T�u���[�`��
 *	  ��void test_subr(const char* s, const char* delim) {
 *	  ��  do {
 *	  ��    char* t = strcompress(s, (char**)&s, delim);
 *	  ��    printf("'%s'\n", t);
 *	  ��    free(t);
 *	  ��    s = mbsninc(s, 1);  //s������'\0'���w���Ă����ꍇ�ɐi�܂Ȃ��悤�ɂ��邽��mbsninc()���g���K�v���L�鎖�ɒ��ӂ���Bmbsinc()�ł�s������'\0'���w���Ă��Ă��i��ł��܂��B
 *	  ��  } while(*s);
 *	  ��}
 *	  ��//���}���`�o�C�g�Ή��ɂȂ��������m�F����e�X�g
 *	  ��void test1() {
 *	  ��  test_subr("�\��\\�\��\\�\��", "\\");
 *	  ��  //���ύX�O
 *	  ��  //��'\x95'	���ύX�O�́A�S�p������2�o�C�g�ڂ�'\'���ԈႦ�ăG�X�P�[�v��'\'�ƌ��Ȃ��Ă��܂��Ă����B
 *	  ��  //��'��'
 *	  ��  //��'\x8F'
 *	  ��  //��'��'
 *	  ��  //��'\x97'
 *	  ��  //��'��'
 *	  ��  //���ύX�O
 *	  ��  //��'�\��'	���ύX��́A�������Ȃ����B
 *	  ��  //��'�\��'
 *	  ��  //��'�\��'
 *	  ��}
 *	  ��//���}���`�o�C�g�Ή��ł��l�X�ȃP�[�X�Ő��������삵�Ă��鎖���m�F����e�X�g(�ȉ��͑S�ĕύX��̏o�͂ł�)
 *	  ��void test2() {
 *	  ��  test_subr("", NULL);
 *	  ��  //��''
 *	  ��  test_subr("�ݶ�", NULL);
 *	  ��  //��'�ݶ�'
 *	  ��  test_subr("�S�p", NULL);
 *	  ��  //��'�S�p'
 *	  ��  test_subr("\\x30\\x31\\x32", NULL);
 *	  ��  //��'012'
 *	  ��  test_subr("\\x30\\x31\\x32", "\\");
 *	  ��  //��''
 *	  ��  //��'x30'
 *	  ��  //��'x31'
 *	  ��  //��'x32'
 *	  ��  test_subr("������.����[����]", ".[]");
 *	  ��  //��'������'
 *	  ��  //��'����'
 *	  ��  //��'����'
 *	  ��  test_subr("�������D�����m�����n", "�D�m�n");		//�S�p�����̃f���~�^�����p�o����悤�ɂȂ����B
 *	  ��  //��'������'
 *	  ��  //��'����'
 *	  ��  //��'����'
 *	  ��  test_subr("������\\.����\\[����\\]", ".[]");
 *	  ��  //��'������.����[����]'
 *	  ��  test_subr("������\\�D����\\�m����\\�n", "�D�m�n");	//�S�p�����̃f���~�^��'\'�ŃG�X�P�[�v�o���܂��B
 *	  ��  //��'�������D�����m�����n'
 *	  ��}
 *	* Thu May 26 23:31:18 JST 2016 Naoyuki Sawa
 *	- strescape()���}���`�o�C�g�Ή����܂����B
 *	  ���̑Ή��ɂ��Astrescape()��strcompress()�̑Ώ̐����������܂����B
 *	  �ڍׂ́A'Wed May 25 10:22:02 JST 2016'�̃R�����g���Q�Ƃ��ĉ������B
 *	  �ȉ��̓���m�F���s���܂����B
 *	  ��//���L�̃e�X�g�֐����痘�p����T�u���[�`��
 *	  ��void test_subr(const char* s, const char* need_escape) {
 *	  ��  char *t, *u;
 *	  ��  t = strescape(s, need_escape);	//���G�X�P�[�v�������ʂ�\������B
 *	  ��  printf("'%s'\n", t);		//��
 *	  ��  u = strcompress(t, NULL, NULL);	//���G�X�P�[�v���������Č��̕�����ƃo�C�i����v���鎖���m�F����B
 *	  ��  if(strcmp(s, u)) { DIE(); }	//��
 *	  ��  free(t);
 *	  ��  free(u);
 *	  ��}
 *	  ��//���}���`�o�C�g�Ή��ł��l�X�ȃP�[�X�Ő��������삵�Ă��鎖���m�F����e�X�g
 *	  ��void test() {
 *	  ��  test_subr("\1\2\3\4\5\6\7\10\11\12\13\14\15\16\17", NULL);
 *	  ��  //��'\001\002\003\004\005\006\a\b\t\n\v\f\r\016\017'
 *	  ��  test_subr("C:\\usr\\PIECE\\docs\\PIECE �|�[�g���.htm", NULL);
 *	  ��  //��'C:\\usr\\PIECE\\docs\\PIECE �|�[�g���.htm'
 *	  ��  test_subr("ABC.DEF[123]", NULL);
 *	  ��  //��'ABC.DEF[123]'
 *	  ��  test_subr("ABC.DEF[123]", ".[]");
 *	  ��  //��'ABC\.DEF\[123\]'
 *	  ��  test_subr("�\��.�\��[�\��]", NULL);
 *	  ��  //��'�\��.�\��[�\��]'				//�S�p�����̓G�X�P�[�v�����ɂ��̂܂ܕ\������悤�ɂȂ����B
 *	  ��  test_subr("�\��.�\��[�\��]", ".[]");
 *	  ��  //��'�\��\.�\��\[�\��\]'
 *	  ��  test_subr("�\���D�\��m�\���n", "�D�m�n");	//�S�p�������G�X�P�[�v�Ώۂɏo���܂��B
 *	  ��  //��'�\��\�D�\��\�m�\��\�n'
 *	  ��}
 *	* Mon May 30 21:45:50 JST 2016 Naoyuki Sawa
 *	- intern_string()���g�Ŕr��������s���悤�ɂ��܂����B
 *	  ����܂ł́A�A�v���P�[�V���������ӂ���intern_string()�����荞�݃��[�`����}���`�X���b�h���Ŏg�p���Ȃ��悤�ɂ��Ă��܂������A
 *	  intern_string()�͗l�X�ȃ��W���[��������ԐړI�ɗ��p����̂ŁA�A�v���P�[�V�������Ӑ}���Ċ��荞�݃��[�`����}���`�X���b�h���Ŏg�p���Ȃ��悤�ɒ��ӂ���͓̂���Ǝv��������ł��B
 *	  ����́Aquark_from_string()/quark_to_string()/intern_string()��A�������ԐړI�ɌĂяo���e���W���[���̊֐����A���荞�݃��[�`����}���`�X���b�h���ň��S�Ɏg�p�o����悤�ɂȂ�܂����B
 *	* Mon Jun 06 21:45:00 JST 2016 Naoyuki Sawa
 *	- memrotate()��ǉ����܂����B
 *	* Tue Jun 07 22:13:11 JST 2016 Naoyuki Sawa
 *	- memrotate()�̎�����ύX���܂����B
 *	  �ڍׂ́Aclipstl.c���W���[���́Astd_rotate()�֐��̃R�����g���Q�Ƃ��ĉ������B
 *	  memrotate()�̊֐��d�l�͕ς���Ă��܂���B
 */
#ifndef __CLIP_STR_H__
#define __CLIP_STR_H__

#ifdef  __cplusplus
extern "C" {
#endif//__cplusplus

/*****************************************************************************
 *	�g����
 *****************************************************************************/

/* �w�肵��������������菜���B
 * [in]
 *	s		���̕�����
 *	mode		��������ʒu�̎w��	0 ���[�ɂ���w�蕶������������
 *						1 ���[�ɂ���w�蕶������������
 *						2 �E�[�ɂ���w�蕶������������
 *						3 ��������ɂ���S�Ă̎w�蕶������������
 *	reject		�������镶��		NULL���w�肵���ꍇ�́A�󔒕�������������
 * [out]
 *	�߂�l		���ʂ̕�����		�Ăяo�����ɂāAfree()�ŊJ�����Ă�������
 * [note]
 *	- HSP��strtrim�֐��̎d�l�ɕ킢�܂����B
 * [�g�p��]
 *	const char src[] = "   - - - Sample  String - - -   ";
 *	char* dst;
 *	dst = strtrim(src, 0, NULL); puts(dst); free(dst); //   "- - - Sample  String - - -"
 *	dst = strtrim(src, 1, NULL); puts(dst); free(dst); //   "- - - Sample  String - - -   "
 *	dst = strtrim(src, 2, NULL); puts(dst); free(dst); //"   - - - Sample  String - - -"
 *	dst = strtrim(src, 3, NULL); puts(dst); free(dst); //       "---SampleString---"
 *	dst = strtrim(src, 0, " -"); puts(dst); free(dst); //         "Sample  String"
 *	dst = strtrim(src, 1, " -"); puts(dst); free(dst); //         "Sample  String - - -   "
 *	dst = strtrim(src, 2, " -"); puts(dst); free(dst); //"   - - - Sample  String"
 *	dst = strtrim(src, 3, " -"); puts(dst); free(dst); //          "SampleString"
 */
char* strtrim(const char* s, int mode, const char* reject);

/* ���[(�ړI�����Ȃ��),����,����(strchug),����,�E��(strchomp)�̋󔒕�������������B
 * [in]
 *	s		���̕�����
 * [out]
 *	�߂�l		����s�����̂܂ܕԂ��B
 * [note]
 *	- �����̊֐��́A�V������������쐬�����ɁA���̕�����𒼐ڕύX����B
 *	- GLib��g_strstrip(),g_strchug(),g_strchomp()�Ɍ݊��ł��B
 *	  �����̊֐���ǉ��������R�́AGLib�݊��̊֐���p�ӂ��Ă������Ǝv�������炾���ł��B
 *	  '�g����'���s���ړI�����Ȃ�΁A���strtrim()�֐��̕������@�\�ł��B
 *	  ���ۂ̂Ƃ��낱���̊֐��͓����ŁAstrtrim()�֐����Ăяo���Ď������܂����B
 */
char* strstrip(char* s);
char* strchug( char* s);
char* strchomp(char* s);

/*****************************************************************************
 *	�G�X�P�[�v
 *****************************************************************************/

/* ������̃o�C�i���f�[�^���A�󎚉\�ȕ�����ɃG�X�P�[�v���܂��B
 * [in]
 *	s		������̃o�C�i���f�[�^
 *	need_escape	�A�v���P�[�V������`�̃G�X�P�[�v���K�v�ȕ���	(NULL=����)
 *			
 * [out]
 *	�߂�l		�G�X�P�[�v���ꂽ������	�Ăяo�����ɂāAfree()�ŊJ�����Ă��������B
 * [note]
 *	- ���֐��̖��O�́AGLib��g_strescape������܂����B
 *	  �֐��d�l��g_strescape�Ɗ��S�݊��ł͂���܂���B(need_escape���Ǝ��g���ł��B)
 *	- ���֐��́A�ȉ��̏������s���܂��B
 *	  1. C����̃G�X�P�[�v�V�[�P���X�ɑ������镶�����A�G�X�P�[�v�V�[�P���X�ɕϊ�����B
 *	  2. �A�v���P�[�V������`�̃G�X�P�[�v���K�v�ȕ����̑O�ɁA'\'��ǉ�����B
 *	  3. �󎚉\�łȂ��������A'\'+8�i�\�L3���ɕϊ�����B
 *	  4. ����ȊO�̕����́A���̂܂܏o�͂���B
 * [�g�p��]
 *	- C����̕�����G�X�P�[�v����
 *	  dst=strescape(src,NULL);
 *	- Tcl����̕�����G�X�P�[�v����(Tcl�̃\�[�X��Tcl_ConvertElement()�ɑ���)
 *	  dst=strescape(src," $;[]{}");
 *	- Tcl����̃��X�g�쐬����(Tcl�̃\�[�X��Tcl_Merge()�ɑ���)
 *	  *dst='\0';
 *	  for(i=0;i<argc;i++){
 *	    char*tmp=strescape(argv[i]," $;[]{}");
 *	    if(i){strcat(dst," ");}
 *	    strcat(dst,(*tmp)?tmp:"{}");//��v�f��{}�ɒu�������Ė����I�Ɋi�[����
 *	    free(tmp);
 *	  }
 */
char* strescape(const char* s, const char* need_escape);

/* �G�X�P�[�v���ꂽ��������A������̃o�C�i���f�[�^�ɕ������܂��B
 * [in]
 *	s		�G�X�P�[�v���ꂽ������
 *	delim		������s�̏������I�����镶���Z�b�g���w�肵�܂��B	NULL���w�肳�ꂽ�ꍇ�͕�����s�̏I�[�܂ŏ������s���܂��B
 *									endptr�Ƒg�ݍ��킹�Ďg�p���邱�Ƃ�z�肵�Ă��܂����Aendptr���w�肹��delim �̂ݎw�肷�邱�Ƃ��\�ł��B
 *	endptr		������s�̏������I�������ʒu���i�[���܂��B	NULL���w�肳�ꂽ�ꍇ�͊i�[���܂���B
 *									delim �Ƒg�ݍ��킹�Ďg�p���邱�Ƃ�z�肵�Ă��܂����Adelim ���w�肹��endptr�̂ݎw�肷�邱�Ƃ��\�ł��B
 * [out]
 *	�߂�l		������̃o�C�i���f�[�^
 * [note]
 *	- ���֐��̖��O�́AGLib��g_strcompress������܂����B
 *	  �֐��d�l��g_strcompress�Ɗ��S�݊��ł͂���܂���B(endptr,delim���Ǝ��g���ł��B)
 *	- ���֐��́A�ȉ��̏������s���܂��B
 *	  1. ������s����ꕶ���擾���A����c�Ƃ���B����c�ɂ���Ĉȉ�2.�`4.�̂����ꂩ�̏������s���B
 *	  2. ����c��'\0',����,delim(�w�肳��Ă����)�̕����Z�b�g�Ɉ�v�����ꍇ:
 *	    2-1. endptr(�w�肳��Ă����)�ɕ���c�̈ʒu���i�[����B
 *	    2-2. �o�͕������'\0'�ŏI�[����B
 *	    2-3. �������I������B
 *	  3. ����c��'\'�ł���ꍇ�A����c�̎��̕������擾���A�ȉ�3-1.�`3-5.�̂����ꂩ�̏������s���B
 *	    3-1. '\'��nul�������ꍇ�A'\'��j�����ď���2.���s���I������B('\'��nul�������̂�'�G�X�P�[�v���ꂽ������'�Ƃ��ĕs�������A������̏I�[�𒴂��Ȃ��悤�ɍŒ���̑΍���s�����Ƃɂ���)
 *	    3-2. '\'�ɑ���C����̃G�X�P�[�v�V�[�P���X�������A1�����ɕϊ����ďo�͂���B
 *	    3-3. '\'�ɑ���8�i�\�L(1�`3��)���A1�����ɕϊ����ďo�͂���B
 *	    3-4. '\'�ɑ���16�i�\�L(1�`����)���A1�����ɕϊ����ďo�͂���B
 *	    3-5. '\'�ɑ������s���A'\'���ƍ폜����B(C�v���v���Z�b�T,Python,Tcl,sed,awk,bash,�c�̍s�̌p���ɑ���)
 *	    3-6. '\'�ɑ�������ȊO�̕������A'\'���폜���ĕ����݂̂ɕϊ����ďo�͂���B('�A�v���P�[�V������`�̃G�X�P�[�v���K�v�ȕ���'���܂�)
 *	  4. ����c��2.�`3.�ȊO�ł���ꍇ�A�ȉ�4-1.�̏������s���B
 *	    4-1. ����c�����̂܂܏o�͂���B
 *	- endptr,delim�́A�u�G�X�P�[�v����Ă��Ȃ�����̕����𕪊������ƌ��Ȃ����X�g����A��v�f�𕜌����Ď擾����v���߂Ɏg�p���܂��B
 *	  ���Ƃ��΁ATcl�̃��X�g�����v�f�𕜌����Ď擾����ꍇ�Ɏg�p���܂��B
 *	  C�\�[�X������̒P���ȕ����̏ꍇ�͍l������K�v�͂���܂���̂ŁAendptr,delim��NULL���w�肵�Ă��������B
 * [�g�p��]
 *	- C����̕�����G�X�P�[�v��������
 *	  dst=strcompress(src,NULL,NULL);
 *	- Tcl����̃��X�g���������B(�����ɂ�Tcl�ł�{}�ł̃G�X�P�[�v���l������K�v�����邪�A�������ȒP�ɂ��邽�߈ȉ��̗�ł͏ȗ�����B)
 *	  for(;;){
 *	    src+=strspn(src,"\t\n\v\f\r ");                  //��s����f���~�^���΂��B
 *	    if(!*src){break;}                                //���X�g�I�[�Ȃ�ΏI������B
 *	    argv[argc++]=strcompress(src,&src,"\t\n\v\f\r ");//��v�f�𕜌����Ď擾����B
 *	  }
 */
char* strcompress(const char* s, char** endptr, const char* delim);

/*****************************************************************************
 *	������|�C���^�z��
 *****************************************************************************/

/* ������|�C���^�z��́A���������J������B
 * [in]
 *	str_array		������|�C���^�z��B	�z���NULL�|�C���^�v�f�ŏI�[����Ă��邱�ƁB(main()�֐���argv[]�Ɠ��l�ł��B)
 *							str_array���̂�NULL�|�C���^���w�肵���ꍇ�́A���֐��͉��������ɏ�����Ԃ��B
 * [note]
 *	- GLib��g_strfreev()�Ɍ݊��ł��B
 */
void strfreev(char** str_array);

/* ������|�C���^�z��́A�v�f�����擾����B
 * [in]
 *	str_array		������|�C���^�z��B	�z���NULL�|�C���^�v�f�ŏI�[����Ă��邱�ƁB(main()�֐���argv[]�Ɠ��l�ł��B)
 * [out]
 *	�߂�l			�v�f���B		�I�[��NULL�|�C���^�v�f�́A�v�f���Ɋ܂߂Ȃ��B
 * [note]
 *	- GLib��g_strv_length()�Ɍ݊��ł��B
 */
size_t strv_length(char** str_array);

/* ������|�C���^�z��́A'�[��'�������쐬����B
 * [in]
 *	str_array		��������A������|�C���^�z��B	�z���NULL�|�C���^�v�f�ŏI�[����Ă��邱�ƁB(main()�֐���argv[]�Ɠ��l�ł��B)
 * [out]
 *	�߂�l			���������A������|�C���^�z��B	�Ăяo�����ɂāAstrfreev()���g���ĊJ������B
 * [note]
 *	- GLib��g_strdupv()�Ɍ݊��ł��B�������A�ȉ��̈�_�ɂ����āA��݊��ł��B
 *	- GLib��g_strdupv()�́A����str_array��NULL�|�C���^���w�肷�邱�Ƃ��ł��Ag_strdupv()�͉���������NULL�|�C���^��Ԃ��܂��B
 *	  ����ɑ΂��������́A����str_array��NULL�|�C���^���w�肷�邱�Ƃ��ł��܂���B�����w�肵���ꍇ�́A�s���ȓ���ƂȂ�܂��B
 *	  GLib��g_strdupv()�́A�Ǝ���g_strdup()��������NULL�|�C���^�w��ł��邱�Ƃɕ���āAg_strdupv()��������NULL�|�C���^�w��Ƃ��Ă���̂��Ǝv���܂����A
 *	  g_strdup()��g_strdupv()�̈�����NULL�|�C���^�w��ł��邱�Ƃ́A�������ăo�O�������h���Ȃ��肪����悤�Ɏv���܂��B
 *	  �]���ē������ł́A�W���I��strdup()�Ɠ��l�ɁA������NULL�|�C���^�w��s�Ƃ��邱�Ƃɂ��܂����B
 */
char** strdupv(char** str_array);

/****************************************************************************
 *	������������
 ****************************************************************************/

/* asprintf()�Ɠ��l�����A�������߂�l�Ƃ���_���قȂ�B
 * [in]
 *	fmt			����������B
 *	...			�ψ����B
 * [out]
 *	�߂�l			������B		�Ăяo�����ɂāA�g�p�I����ɁAfree()�ŊJ�����邱�ƁB
 * [note]
 *	- GLib��g_strdup_printf()�Ɍ݊��ł��B
 */
char* strdup_printf(const char* fmt, ...);

/* vasprintf()�Ɠ��l�����A�������߂�l�Ƃ���_���قȂ�B
 * [in]
 *	fmt			����������B
 *	ap			�ψ����B
 * [out]
 *	�߂�l			������B		�Ăяo�����ɂāA�g�p�I����ɁAfree()�ŊJ�����邱�ƁB
 * [note]
 *	- GLib��g_strdup_vprintf() �Ɍ݊��ł��B
 */
char* strdup_vprintf(const char* fmt, va_list ap);

/*****************************************************************************
 *	�A��
 *****************************************************************************/

/* �����̕�������A��̕�����ɘA������B
 * [in]
 *	s, ...			�����̕�����B		NULL�|�C���^�ŏI�[���邱�ƁB
 * [out]
 *	�߂�l			������B		�Ăяo�����ɂāA�g�p�I����ɁAfree()�ŊJ�����邱�ƁB
 * [note]
 *	- GLib��g_strconcat()�Ɍ݊��ł��B
 *	- g_strconcat()�̎d�l�ł́A�ŏ��̕�����s�́uthe first string to add, which must not be NULL�v�ƂȂ��Ă��邪�A
 *	  ���֐��́A�ŏ��̕�����s��NULL���Ƃ��A���̏ꍇ�A���֐��͋󕶎���("")��Ԃ��悤�ɂ����B
 *	  ���̕����Astrjoin()��strjoinv()�ƑΏې����L��A�֗��ł��邵�A����Ղ��Ǝv��������ł��B
 */
char* strconcat(const char* s, ...);

/* �����̕�������A��̕�����ɘA������B
 * [in]
 *	separator		�Z�p���[�^�B		NULL�|�C���^���w�肷��ƁA""���w�肵���̂Ɠ����ƌ��Ȃ��B
 *	...			�����̕�����B		NULL�|�C���^�ŏI�[���邱�ƁB
 * [out]
 *	�߂�l			������B		�Ăяo�����ɂāA�g�p�I����ɁAfree()�ŊJ�����邱�ƁB
 * [note]
 *	- GLib��g_strjoin()�Ɍ݊��ł��B
 */
char* strjoin(const char* separator, ...);

/* ������|�C���^�z����A��̕�����ɘA������B
 * [in]
 *	separator		�Z�p���[�^�B		NULL�|�C���^���w�肷��ƁA""���w�肵���̂Ɠ����ƌ��Ȃ��B
 *	str_array		������|�C���^�z��	�z���NULL�|�C���^�v�f�ŏI�[����Ă��邱�ƁB(main()�֐���argv[]�Ɠ��l�ł��B)
 * [out]
 *	�߂�l			������B		�Ăяo�����ɂāA�g�p�I����ɁAfree()�ŊJ�����邱�ƁB
 * [note]
 *	- GLib��g_strjoinv()�Ɍ݊��ł��B
 */
char* strjoinv(const char* separator, char** str_array);

/****************************************************************************
 *	����
 ****************************************************************************/

/* ��������A�f���~�^�������p���ĕ�������B
 * [in]
 *	s			�������镶����B
 *	delimiter		�f���~�^������B	""�͎w��s�B
 *	max_tokens		�ő啪�����B		1�����̒l���w�肷��ƁA�����ƌ��Ȃ��B
 * [out]
 *	�߂�l			������|�C���^�z��B	�Ăяo�����ɂāA�g�p�I����ɁAstrfreev()�ŊJ�����邱�ƁB
 * [note]
 *	- GLib��g_strsplit()�Ɍ݊��ł��B
 */
char** strsplit(const char* s, const char* delimiter, int max_tokens);

/* ��������A�f���~�^�����Z�b�g��p���ĕ�������B
 * [in]
 *	s			�������镶����B
 *	delimiters		�f���~�^�����Z�b�g�B	""���w��\�B
 *	max_tokens		�ő啪�����B		1�����̒l���w�肷��ƁA�����ƌ��Ȃ��B
 * [out]
 *	�߂�l			������|�C���^�z��B	�Ăяo�����ɂāA�g�p�I����ɁAstrfreev()�ŊJ�����邱�ƁB
 * [note]
 *	- GLib��g_strsplit_set()�Ɍ݊��ł��B
 */
char** strsplit_set(const char* s, const char* delimiters, int max_tokens);

/* ���������s�̒����ɐ܂�Ԃ��ĕ������܂��B
 * [in]
 *	s			�܂�Ԃ�������B
 *	w			��s�̕��B���p������1,�S�p������2�Ɛ����܂��B
 * [out]
 *	�߂�l			������|�C���^�z��B	�Ăяo�����ɂāA�g�p�I����ɁAstrfreev()�ŊJ�����邱�ƁB
 * [note]
 *	- Python��textwrap.wrap()�̊֐��d�l���Q�l�ɂ��܂����B
 *	- �����I�ɂ́A�������ꂽ�X�̕�����̕���w�ȉ��ƂȂ�܂��B
 *	  �������A1������w�ȏ�̕��������݂����ꍇ�Aw�ȏ�̒����ƂȂ�ꍇ���L��܂��B��̓I�ɂ́A
 *	  w��1�������w�肵�Ă��A�������ꂽ�X�̕�����͕�1�ȏ�(���p1����,����,�S�p1����)�ɐ��蓾�܂��B
 *	  w��2�������w�肵�Ă��A�������ꂽ�X�̕�����͕�2�ȏ�(�S�p1����)�ɐ��蓾�܂��B
 *	  ���̎d�l�́APython��textwrap.wrap()�Ɠ����ł��B
 *	- ��L�̋��������p���āAw��1�ȉ����w�肷��΁A�������1�����P�ʂɕ�������p�r�ɂ����p�ł��܂��B
 *	- ���֐��́A�֑������͍s���܂���B
 *	- ���g�p��:
 *	  ��char** argv=textwrap("12345�Z78",3);
 *	  ����argv[0]="123",argv[1]="45",argv[2]="�Z7",argv[3]="8"
 *	  ��char** argv=textwrap("1��3",0);
 *	  ����argv[0]="1",argv[1]="��",argv[2]="3"
 */
char** textwrap(const char* s, int w);

// * Sat Apr 09 21:06:53 JST 2016 Naoyuki Sawa
// - strdup_splitpath()��strdup_makepath()��ǉ����܂����B
//   splitpath()��makepath()���Astrdup_printf()�Ɠ��l�Ɏg����ƕ֗��Ȃ̂ŁA�����̔��f��'strdup_*()'�o�[�W������ǉ����鎖�ɂ��܂����B
// - ���g�p��
//   ��void test(const char* path) {
//   ��  char *fname, *ext, *fullpath;
//   ��  strdup_splitpath(path, NULL, NULL, &fname, &ext);
//   ��  fullpath = strdup_makepath("C:", "\\Home", fname, ext);
//   ��  printf("%s\n", fullpath);
//   ��  free(fname);    //��
//   ��  free(ext);      //���K�[�x�[�W�R���N�^�[���g�p���Ă���ꍇ�͉�����Ȃ��Ă��\���܂���B
//   ��  free(fullpath); //��
//   ��}
void strdup_splitpath(const char* path, char** drive/*NULL��*/, char** dir/*NULL��*/, char** fname/*NULL��*/, char** ext/*NULL��*/);
char* strdup_makepath(const char* drive/*NULL��*/, const char* dir/*NULL��*/, const char* fname/*NULL��*/, const char* ext/*NULL��*/);

/*****************************************************************************
 *	��r
 *****************************************************************************/

/* ������s1�̃v���t�B�N�X�������As2�Ɉ�v���邩���ׂ�B
 * [in]
 *	s1			������B
 *	s2			�v���t�B�N�X�B		""���w��\�B
 * [out]
 *	�߂�l			��v���Ȃ����0��Ԃ��B
 *				��v������0�ȊO��Ԃ��B
 * [note]
 *	- GLib��g_str_has_prefix()�Ɍ݊��ł��B
 */
int str_has_prefix(const char* s1, const char* s2);

/* ������s1�̃T�t�B�b�N�X�������As2�Ɉ�v���邩���ׂ�B
 * [in]
 *	s1			������B
 *	s2			�T�t�B�b�N�X�B		""���w��\�B
 * [out]
 *	�߂�l			��v���Ȃ����0��Ԃ��B
 *				��v������0�ȊO��Ԃ��B
 * [note]
 *	- GLib��g_str_has_suffix()�Ɍ݊��ł��B
 */
int str_has_suffix(const char* s1, const char* s2);

/* Tcl6.7�́ustring match�v�ɏ��������A������p�^�[����r���s���܂��B
 * [in,out]
 *	��pat��str�Ƀ}�b�`���邩�ǂ������m���߂܂��B�}�b�`����Ȃ��1���A���Ȃ��Ȃ��0��Ԃ��܂��B
 *	��2�̕�����̏ƍ��ł́Apat�Ɏ��̎g������ʂȃV�[�P���X�������āA�}�b�`���e�͓���łȂ���΂Ȃ�܂���B
 *	��*       �󕶎�����܂�str���̂����镶���V�[�P���X�Ƀ}�b�`���܂��B
 *	��?       str���̂�����P�Ƃ̕����Ƀ}�b�`���܂��B
 *	��[chars] char�ɗ^����ꂽ�Z�b�g���̂ǂꂩ�̕����Ƀ}�b�`���܂��B
 *	��        x-y�`���̃V�[�P���X��chars���Ɍ����ƁAx��y���܂܂��x��y�̊Ԃ̂����ꂩ�̕����Ƀ}�b�`���܂��B
 *	��        [A-z]��'_'�Ƀ}�b�`���܂��B('_'��'Z'��'a'�̊Ԃɂ���̂�)
 *	��\x      �P��̕���x�Ƀ}�b�`���܂��B�����pat���̕���*?[]\�̓��ʂȉ��߂�h�~������@��񋟂��܂��B
 * [note]
 *	- Tcl6.7�́ustring match�v�ƁA��ʓI�Ȑ��K�\���̑���_�Ƃ��āA�ȉ��ɒ��ӂ��Ă�������:
 *	  ��ʓI�Ȑ��K�\���ł́A'['�̒����']'�͕����N���X������A�����N���X�̈ꕶ���ƌ��Ȃ���邪�ATcl6.7��string match�ɂ͂��̓��Ꮘ���������B
 *	  ��ʓI�Ȑ��K�\���ł́A�����N���X�̒��ł�'\'�ɂ����ꕶ���̃G�X�P�[�v��G�X�P�[�v�V�[�P���X���g���邪�ATcl6.7��string match�ł͎g���Ȃ��B
 *	  �s��������'^',�s��������'$',�����N���X����'^',�O���[�v������'(',')',�I������'|'�Ȃǂ͗L��܂���B���K�\���Ƃ�������VBA��Like�ɋ߂��ł��B
 */
int string_match(const char* pat, const char* str);

/* "���R��"�A���S���Y���ɂ��A�������r���s���܂��B
 * [in]
 *	s1,s2			������B
 * [out]
 *	�߂�l			strcmp()�Ɠ��l�ł��B
 * [note]
 *	- �֐��d�l�́APHP��strnatcmp(),strnatcasecmp()�Ɍ݊��ł��B
 *	  �uPHP �}�j���A�� - strnatcmp�v(http://php.net/manual/ja/function.strnatcmp.php)
 *	- ���L�̃\�[�X���Q�l�ɂ����Ē����܂����B
 *	  �uNatural Order String Comparison�v(http://sourcefrog.net/projects/natsort/)
 *	  �I���W�i���ł̎����́A������̔�r����v���ĊO���̃��[�v�֔��������A��r�ʒu���ꕶ�������i�܂Ȃ����ʂ��L��悤�ł��B
 *	  �ŏI�I�Ȕ�r���ʂɂ͈Ⴂ�͗L��܂��񂪁A���x��̖��ʂ������܂��B
 *	  �������ł͂��̓_�����P���A���̔�r�ʒu��������̎��̈ʒu�܂Ői�ނ悤�ɂ��܂����B
 *	- strnatcmp()�ƁA������strverscmp()�́A�A���S���Y���͈قȂ�̂ł����A���҂ɂ���r���ʂ͂قƂ�Ǔ����Ȃ�܂��B
 *	  ��ȈႢ�́Astrnatcmp()�͋󔒂������Ĕ�r���鎖�ƁA������(�ƌ��Ȃ����)��������̐�s����'0'�̐��ɂ�锻�f��ł��B
 *	  ��҂̈Ⴂ�ɂ��ẮAstrverscmp()�����Astrnatcmp()�̕����A���ςɋ߂����ʂ��Ǝv���܂��B
 *	  �ڍׂ́A�������̉��́u���e�X�g�X�C�[�g�v���Q�Ƃ��ĉ������B
 *	- strnatcmp()�͋󔒂������Ĕ�r���܂����A�󔒂��S�����Ӗ��ł͂Ȃ����ɒ��ӂ��ĉ������B
 *	  ���l��̌�̋󔒂́A���l����I�[������A���J�[�ƂȂ�܂��B
 *	* Fri Oct 16 21:16:43 JST 2015 Naoyuki Sawa
 *	- ����܂ł̎����́A�I���W�i���ł�strnatcmp()�̓���ɏ������Ă����̂ł����A���炩�ɕs���R�ȓ��삪�L���Ďg���h���̂ŁA���������ύX���邱�Ƃɂ��܂����B
 *	  �I���W�i���ł�strnatcmp()�́APOSIX�W���Ƃ����킯�ł͂Ȃ��A�����܂�"���R��"�A���S���Y���̎����̈�ł�����A�����ɏ������Ȃ��Ă��ǂ��Ǝv��������ł��B
 *	- �I���W�i���ł�strnatcmp()�́A���L�̃\�[�g���ɂȂ�Ƃ����s���R��������܂��B
 *	  ��AB C
 *	  ��A  C
 *	  �Ȃ������Ȃ邩�ƌ����ƁA�I���W�i���ł�strnatcmp()�͋󔒕������폜���Ĕ�r���邩��ł��B
 *	  ���̗�̏ꍇ�A���������ł́uABC�v�ƁuAC�v�̔�r�ɂȂ�A�O�҂��������ƌ��Ȃ����̂ł��B
 *	- ���R�͂킩�����̂ł����A���炩�ɒ����ɔ����Ă��ĕs���R���Ǝv���܂��B
 *	  �����ŁA�������ł́A�u�󔒕������폜����v�̂ł͂Ȃ��A�u�A�������󔒕�������̃X�y�[�X(' ')�ɒu��������v���ɂ��܂����B
 *	  ��������΁A��قǂ̗�́A���������ł́uAB C�v�ƁuA C�v�̔�r�ɂȂ�A�O�҂��傫���ƌ��Ȃ��܂��B
 *	  ��A  C
 *	  ��AB C
 *	- �Ȃ��A�u�A�������󔒕�������̃X�y�[�X(' ')�ɒu��������v���ɂ����̂́A�󔒕����̎�ނɂ���đ召�𔻒f����͕̂s���R���Ǝv��������ł��B
 *	  �Ⴆ�΁A�u\nA\nB\n�v�u\tA\tB\t�v�u\r\nA\r\nB\r\n�v���͑S�āA�u A B �v�ƌ��Ȃ��Ĕ�r���܂��B
 *	- �P��Ԃ̋󔒂ɂ��Ă͏�L�̒ʂ�Ȃ̂ł����A�s���ƍs���̋󔒂́A����܂łǂ���폜���Ĕ�r���܂��B	���������d�v��
 *	  ��������̂����R���Ǝv��������ł��B
 *	- �ȏ���܂Ƃ߂܂��ƁA����Ղ���Ƃ��ẮA�uA  B C�v�Ɓu  A B  C  �v�́A�uA B C�v�ƁuA B C�v�̔�r�ƂȂ�A��v���܂��B
 */
int strnatcmp(    const char* s1, const char* s2);
int strnatcasecmp(const char* s1, const char* s2);

/*****************************************************************************
 *	���]
 *****************************************************************************/

/* ������𔽓]����B
 * [in]
 *	s		���]���镶����B
 * [out]
 *	�߂�l		����s�����̂܂ܕԂ��B
 * [note]
 *	- GLib��g_strreverse()�Ɍ݊��ł��B
 */
char* strreverse(char* s);

/*****************************************************************************
 *	�P�ꉻ
 *****************************************************************************/

/* ������̒P�ꉻ�\���̐��l��Ԃ��B
 * [in]
 *	s		������,����,NULL�|���C�^�B
 * [out]
 *	�߂�l		�P�ꉻ�\���̐��l�B
 *			�������NULL�|�C���^���w�肵���ꍇ�́A0��Ԃ��B
 * [note]
 *	- GLib��g_quark_from_string()�Ɍ݊��ł��B
 *	- GLib��g_quark_from_static_string()�Ɍ݊��ȋ@�\�͕s�v���Ǝv���̂ŁA�ǉ����܂���ł����B
 */
int quark_from_string(const char* s);

/* �P�ꉻ�\���̐��l�ɑΉ�����A�������Ԃ��B
 * [in]
 *	q		�P�ꉻ�\���̐��l,����,0�B
 * [out]
 *	�߂�l		������B
 *			�P�ꉻ�\���̐��l��0���w�肵���ꍇ�́ANULL�|�C���^��Ԃ��B
 * [note]
 *	- ����q�Ɏw��o����̂́A'quark_from_string()���Ԃ����P�ꉻ�\���̐��l',����,'0'�݂̂ł��B
 *	  ����ȊO�̒l���w�肵���ꍇ�́A�s���ȓ���ƂȂ�܂��B
 * [note]
 *	- GLib��g_quark_to_string()�Ɍ݊��ł��B
 *	- GLib��g_quark_try_string()�Ɍ݊��ȋ@�\�͕s�v���Ǝv���̂ŁA�ǉ����܂���ł����B
 */
const char* quark_to_string(int q);

/* ������̒P�ꉻ�\���̕������Ԃ��B
 * [in]
 *	s		������,����,NULL�|���C�^�B
 * [out]
 *	�߂�l		�P�ꉻ�\���̕�����B
 *			�������NULL�|�C���^���w�肵���ꍇ�́ANULL�|�C���^��Ԃ��B
 * [note]
 *	- GLib��g_intern_string()�Ɍ݊��ł��B
 *	- GLib��g_intern_static_string()�Ɍ݊��ȋ@�\�͕s�v���Ǝv���̂ŁA�ǉ����܂���ł����B
 */
const char* intern_string(const char* s);

/*****************************************************************************
 *	�u��
 *****************************************************************************/

/* ������s�̒��́Aoldstr��S�āAnewstr�ɒu��������������쐬���ĕԂ��B
 * [in]
 *	s		���̕�����		""���w��\�B
 *	oldstr		�������镶����		""�͎w��s�B
 *	newstr		�u�����镶����		""���w��\�B
 * [out]
 *	�߂�l		������B		�Ăяo�����ɂāA�g�p�I����ɁAfree()�ŊJ�����邱�ƁB
 * [note]
 *	- ���̊֐��́AGLib�݊��ł͂���܂���B
 *	  GLib�ɂ́A������u���֐��������悤�ł��B
 */
char* strreplace(const char* s, const char* oldstr, const char* newstr);

/*****************************************************************************
 *	����������
 *****************************************************************************/

/* �������̈�𕡐�����B
 * [in]
 *	src		�������̃A�h���X�B	n=0�̏ꍇ�̂�NULL��
 *	n		�������̃o�C�g���B	0��
 * [out]
 *	�߂�l		���������������̃A�h���X�B
 *			���s�����ꍇ��NULL��Ԃ��B
 * [note]
 *	- memdup()��C�W���ł͂���܂���B
 *	  ��������Unix��I�[�v���\�[�X�̃��C�u�����Ɋ܂܂�Ă���悤�ł��B
 *	  ��http://www2.research.att.com/sw/download/man/man3/mem.html
 *	  ��http://antti-juhani.kaijanaho.fi/darcs/publib/alloc/memdup.c
 *	  ��http://opensource.apple.com/source/ksh/ksh-13/ksh/src/lib/libast/string/memdup.c
 *	- memdup()��C�W���ł͂���܂��񂪁A�֗��Ȃ̂ŁA�����C�u�����ɂ��������鎖�ɂ��܂����B
 *	  �֐��d�l�͎����ł���A�����̂ǂ̎������Q�l�ɂ��Ă��A�����֐��d�l�ɂȂ�܂��B
 */
void* memdup(const void* src, size_t n);

/*****************************************************************************
 *	���������[�e�[�g
 *****************************************************************************/

//�������u���b�N�����[�e�[�g����B
//[in]
//	base		�������u���b�N�̐擪�A�h���X
//	n		���[�e�[�g��
//				{base[0]�`base[size-1]}�͈̔͂��An�o�C�g�����[�e�[�g���Ă��炷�B
//				(n>0)�Ȃ�΁A����փ��[�e�[�g����B([0]��[1]���`��[size-1]��[0])	���E���ʂ͍��L�̒ʂ�ł����A���������Ƃ��ẮA���֐����Ō�����[�e�[�g�ƑO�����[�e�[�g�ǂ���ŏ�����������������𔻒f���A�������ŏ������܂��B
//				(n<0)�Ȃ�΁A�O���փ��[�e�[�g����B([0]��[1]���`��[size-1]��[0])	���E���[�e�[�g��(�̐�Βl)�ɁA�������u���b�N�̃T�C�Y�ȏ�̒l���w�肵�Ă��\���܂���B���֐����ŏ�]�����߂āA�ŏ����̃��[�e�[�g���ŏ������܂��B
//				(n=0)�Ȃ�΁A�������Ȃ��B
//	size		�������u���b�N�̃o�C�g��
//				(size=0)�Ȃ�΁A�������Ȃ��B
//[out]
//	�߂�l		����base�����̂܂ܕԂ��B
//[note]
//	- ���֐��̎d�l�͓Ǝ��ł��B
//	  �Q�l�ɂ��������̃��C�u�����͗L��܂���B
//	- ���֐���ǉ��������R�́A����Aclipmemc.c�Ƀ������u���b�N�̃��T�C�Y�@�\��ǉ����鎞�ɁA�g���\�肾����ł��B
//	  ��̓I�ɂ́A���T�C�Y���郁�����u���b�N���A��A�̊��蓖�čςݗ̈�̖����փ��[�e�[�g���Ĉړ����Ă���A���T�C�Y����ΊȒP�Ɏ����o����Ǝv������ł��B
//	  2016/06/06AM���݁Aclipmemc.c�ɂ͂܂���L�̋@�\��ǉ����Ă��܂��񂪁A�߁X�ǉ�����\��ł��B	��{{2016/06/06PM�R�����g�ǋL:clipmemc.c��MemCpt_Resize()��ǉ����܂����Bmemrotate()�̉��p��Ƃ���MemCpt_Resize()���Q�Ƃ��ĉ������B}}
void* memrotate(void* _base, int n, size_t size);

/*****************************************************************************
 *	�ϊ�
 *****************************************************************************/

#ifdef  WIN32
/* VBA��StrConv()�Ɍ݊��Ȋ֐��ł��B
 * [in]
 *	str		�ϊ����镶����
 *	conv		���s����ϊ��̎�ނ̒l�̍��v
 * [out]
 *	�߂�l		������B		�Ăяo�����ɂāA�g�p�I����ɁAfree()�ŊJ�����邱�ƁB
 * [note]
 *	- Win32 API���g���ĕ����̎�ނ�ϊ�����ɂ́ALCMapString()����Ăяo���K�v���L��܂��B
 *	  �A�v���P�[�V����������LCMapString()����Ăяo����Ԃ��Ȃ����߂ɁA���֐����쐬���܂����B
 *	- LCMapString()�����AVBA��StrConv()�̕�������Ă���̂ŁAStrConv()�݊��Ƃ��鎖�ɂ��܂����B
 *	  ���A�ȉ���vb�`�萔�͒���LCMAP_�`�萔�̒l�ɒ�`���Ă��ǂ������̂ł����A
 *	  �{����vb�`�萔�Ɠ����l�ɂ��Ă���������X�������Ȃ��Ǝv���̂ŁA�{����vb�`�萔�Ɠ����l�Œ�`���܂����B
 *	  vb�`�萔����LCMAP_�`�萔�ւ̕ϊ��́A���֐��̏����̒��ōs���Ă��܂��B
 *	- ���֐��̏������e�́A������/clip/tool/dpstrcnv/�c�[���ōs���Ă����������A�قڂ��̂܂܎����Ă��܂����B
 *	  �ڍׂ́A/clip/tool/dpstrcnv/app.c���Q�Ƃ��ĉ������B
 */
char* StrConv(const char* str, int conv);
//����conv�ɂ͈ȉ��̒萔��0�ȏ�̑g�ݍ��킹���w�肵�ĉ������B
#define vbUpperCase	  1	//�������啶���ɕϊ����܂��B						��
#define vbLowerCase	  2	//��������������ɕϊ����܂��B						��
//#define vbProperCase	  3	//������̊e�P��̐擪�̕�����啶���ɕϊ����܂��B			��
#define vbWide		  4	//��������̔��p����(1�o�C�g)��S�p����(2�o�C�g)�ɕϊ����܂��B		��
#define vbNarrow	  8	//��������̑S�p����(2�o�C�g)�𔼊p����(1�o�C�g)�ɕϊ����܂��B		���uMSDN ���C�u���� Microsoft Visual Studio 6.0 �����[�X - StrConv �֐��v���
#define vbKatakana	 16	//��������̂Ђ炪�Ȃ��J�^�J�i�ɕϊ����܂��B				��
#define vbHiragana	 32	//��������̃J�^�J�i���Ђ炪�Ȃɕϊ����܂��B				��
//#define vbUnicode	 64	//�V�X�e���̊���̃R�[�h�y�[�W���g���ĕ������Unicode�ɕϊ����܂��B	��
//#define vbFromUnicode	128	//�������Unicode����V�X�e���̊���̃R�[�h�y�[�W�ɕϊ����܂��B		��
#endif//WIN32

#ifdef  __cplusplus
}//extern "C"
#endif//__cplusplus

#endif/*__CLIP_STR_H__*/
