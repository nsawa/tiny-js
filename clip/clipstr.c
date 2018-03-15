/*	
 *	clipstr.c
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
 *	* Wed Sep 07 21:52:10 JST 2016 Naoyuki Sawa
 *	- strescape(),�y��,strcompress()���A(int)�ŃA�h���X���Z���s���Ă����̂��A(intptr_t)�ōs���悤�ɏC�����܂����B
 *	  64�r�b�g�Ή��̂��߂̏����ł����A32�r�b�g�ł�(intptr_t)�ōs�������{���͐���������ł��B
 *	  �Ƃ͌����A32�r�b�g�ł͂ǂ���ł��������ʂɂȂ�̂ŁA������32�r�b�g�R�[�h�ɂ͉e���L��܂���B
 *	- quark_from_string(),quark_to_string()���A32�r�b�g�ł�64�r�b�g�łƂŁA�ʂ̎����ɂ��܂����B
 *	  32�r�b�g�ł͂���܂Œʂ�A�P�ꉻ�\���̕�����|�C���^�����̂܂�(int)�Ƃ��ĕԂ��Ηǂ��̂ł����A
 *	  64�r�b�g�ł̓|�C���^��64�r�b�g�̂��߁A������|�C���^�����̂܂�(int)�Ƃ��ĕԂ��Ȃ��Ȃ�������ł��B
 *	  64�r�b�g�ł́A�P�ꉻ�\���̕�����|�C���^��(int)�𑊌ݕϊ����邽�߂́A�V���Ȏ�����݂��鎖�ɂ��܂����B
 *	* Fri Sep 30 21:41:38 JST 2016 Naoyuki Sawa
 *	- strreverse()��C����ł̎����ŁA'goto L_START'���ϐ�'c'�̐錾���܂����ł��������C�����܂����B
 *	  �����͂��̂܂܂ł���肠��܂��񂵁AP/ECE�J����(GCC)��Windows(VC++6.0,VS2015)�ł͌x�����o�Ȃ��̂ł����AHew�ŁuC5546 (W) Transfer of control bypasses initialization of:�v�Ƃ����x�����o������ł��B
 *	  ����̕ύX�́AHew�ŏ�L�̌x�����������߂����̏C���ł���A����̏C���ɂ�铮��̕ω��͗L��܂���B
 *	  ���A����̕ύX���s���Ă��AHew�ł͂Ȃ��uC5128 (I) Loop is not reachable from preceding code�v�Ƃ����x�����o��̂ł����AC����d�l�I�ɖ�薳���A���炩�Ɍ�����x���Ȃ̂ŁA���̌x���͖������鎖�ɂ��܂����B
 */
#include "clip.h"

/*****************************************************************************
 *	�g����
 *****************************************************************************/

static const char strtrim_reject[]="\t\n\v\f\r ";//isspace()�Ɠ���	//asm�u���b�N��������Q�Ƃ���'�萔'�́A__attribute__((unused))��錾���Ȃ��Ă��x�����o�Ȃ��悤�ł��B����Aasm�u���b�N��������Q�Ƃ���'�ϐ�'��'�֐�'�́A__attribute__((unused))��錾���Ȃ���Όx�����o�܂��B
#ifndef PIECE
char* strtrim(const char* s, int mode, const char* reject) {
	int c;
	char *t, *u;
	if(!reject) { reject = strtrim_reject; }
	switch(mode) {
	case 0://���[�ɂ���w�蕶������������
		t = strtrim(s, 1, reject);					//���[�ɂ���w�蕶�����������āA�r���̕�������쐬����B
		u = strtrim(t, 2, reject);					//�E�[�ɂ���w�蕶�����������āA���ʂ̕�������쐬����B
		free(t);							//���[�ɂ���w�蕶�������������A�r���̕�������폜����B
		break;
	case 1://���[�ɂ���w�蕶������������
		while((c = *s++) && strchr(reject, c)) { /** no job **/ }	//�I�[�łȂ��A�w�蕶���ȊO��������܂ŁA���̕����֐i�߂�B
		if(!(u = strdup(s - 1))) { DIE(); }				//���ʂ̕�������쐬����Bs��1��������֐i�݉߂��Ă��邱�Ƃɒ��ӂ���B
		break;
	case 2://�E�[�ɂ���w�蕶������������
		t = strchr(s, '\0');						//�I�[��nul�̈ʒu�֐i�߂�B
		do { t--; } while((t >= s) && strchr(reject, *t));		//�O�̕����֖߂��B�擪�𒴂���,����,�w�蕶���ȊO�����������甲����B
		if(!(u = strndup(s, t - s + 1))) { DIE(); }			//���ʂ̕�������쐬����Bt��1�������O�֖߂�߂��Ă��邱�Ƃɒ��ӂ���B
		break;
	case 3://��������ɂ���S�Ă̎w�蕶������������
		if(!(t = u = strdup(s))) { DIE(); }				//������𕡐�����B
		while((c = *t++ = *s++)) { if(strchr(reject, c)) { t--; } }	//�w�蕶���ȊO��O�l�߂ŃR�s�[����B����case�̂݁A���ʂ̕�������傫�ȃ������̂܂܂ɂȂ邪�A�g�p�p�x���Ⴂ�Ǝv���̂ŋ��e���邱�Ƃɂ����B
		break;
	default:
		DIE();
	}
	return u;
}
#else //PIECE
//�A�Z���u���������ړI�́A�R�[�h�T�C�Y�̒ጸ�ł��B
//�R�[�h�T�C�Y���A(C�����=276�o�C�g)��(Asm��=192�o�C�g)�ɏ������Ȃ�܂����B
//�������x�́A�������m�ۂ̏����ɗv���鎞�Ԃ��傫���̂ŁA���X���҂ł��܂���B
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
		 cmp		%r13, 3				;//%psr := ((unsigned)mode <=> (unsigned)3)		����	*delay*
		 ext		      strtrim_reject@h		;//							�@��
		 ext		      strtrim_reject@m		;//							�@��
		 ld.w		%r14, strtrim_reject@l		;//							�@��
		jrugt		strtrim_DIE			;//if((unsigned)mode >  (unsigned)3) { goto DIE() }	����
		ld.w		%r0, %r12			;//%r0  := s
		jreq.d		strtrim_case_3			;//if((unsigned)mode == (unsigned)3) { goto case_3 }
		ld.w		%r1, %r14			;//%r1  := reject						*delay*
		cmp		%r13, 1				;//%psr := ((unsigned)mode <=> (unsigned)1)
		jrugt		strtrim_case_2			;//if((unsigned)mode >  (unsigned)1) { goto case_2 }
		jreq		strtrim_case_1			;//if((unsigned)mode == (unsigned)1) { goto case_1 }
		;//---------------------------------------------;//
;//strtrim_case_0:
		;//%r0  := s			���g��Ȃ�
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
		;//%r12 := s			���g��Ȃ�
		;//%r14 := reject		���g��Ȃ�
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
		;//%r14 := reject		���g��Ȃ�
		xcall.d		strchr				;//%r10 := t = strchr(s, '��0')
		ld.w		%r13, 0				;//%r13 :=               '��0'					*delay*
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
		;//%r14 := reject		���g��Ȃ�
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
		cmp		%r8, %r10			;//%psr(C) :=   !!p								��
		jp.d		strtrim_case_3_L10		;//										���uif(strchr(reject, c)) { t--; }�v���������򖳂��Ŏ�������H�v
		sbc		%r2, %r8			;//%r2  := t -= !!p						*delay*		��
		;//---------------------------------------------;//
strtrim_return_u:
		;//%r3  := u
		ld.w		%r10, %r3			;//%r10 := u
		popn		%r3				;//
		ret						;//return  u
");
static void __attribute__((noreturn,unused))/*asm�u���b�N����Q��*/ strtrim_DIE() { DIE(); }
#endif//PIECE

#ifndef PIECE
static char* strstrip_strchug_strchomp(char* s, int mode) {
	char* t = strtrim(s, mode, NULL);	//�󔒕������������ꎞ�I�ȕ�������쐬����B
	strcpy(s, t);	//�ꎞ�I�ȕ���������̕�����ɃR�s�[����B
	free(t);	//�ꎞ�I�ȕ�������J������B
	return s;	//���̕������Ԃ��B
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
 *	�G�X�P�[�v
 *****************************************************************************/

//�A�Z���u���������ړI�́A�R�[�h�T�C�Y�̒ጸ�ł��B
//�R�[�h�T�C�Y���A(C�����=528�o�C�g)��(Asm��=456�o�C�g)�ɏ������Ȃ�܂����B(TBL_strescape[],strescape(),strcompress()�̍��v�T�C�Y)	��{{2016/05/26�R�����g�ǋL:strescape()��strcompress()���}���`�o�C�g�Ή��������ɂ��A�R�[�h�T�C�Y�����L�����������܂����B}}
//�������x�́A�������m�ۂ̏����ɗv���鎞�Ԃ��傫���̂ŁA���X���҂ł��܂���B
#ifdef  __GNUC__
//GCC�́u\e�v(��W���̃G�X�P�[�v�V�[�P���X)�ɑΉ����Ă���B
#define SIZE_strescape 13//������������������������������������
static const char TBL_strescape[]={//                        ��		//asm�u���b�N��������Q�Ƃ���'�萔'�́A__attribute__((unused))��錾���Ȃ��Ă��x�����o�Ȃ��悤�ł��B����Aasm�u���b�N��������Q�Ƃ���'�ϐ�'��'�֐�'�́A__attribute__((unused))��錾���Ȃ���Όx�����o�܂��B
//1    2    3    4    5    6    7    8    9   10   11   12   13
'\a','\b','\e','\f','\n','\r','\t','\v','\?','\'','\"','\\','\0',	//������'\0'��strescape()  �ōs��strchr()�̏I�[�p�ł��B�G�X�P�[�v�Ώە����ł͂���܂���B
 'a', 'b', 'e', 'f', 'n', 'r', 't', 'v','\?','\'','\"','\\','\0'};	//������'\0'��strcompress()�ōs��strchr()�̏I�[�p�ł��B�G�X�P�[�v�Ώە����ł͂���܂���B
#else //__GNUC__
//VC++6.0�́u\e�v(��W���̃G�X�P�[�v�V�[�P���X)�ɑΉ����Ă��Ȃ��B
#define SIZE_strescape 12//������������������������������������
static const char TBL_strescape[]={//                        ��
//1    2         3    4    5    6    7    8    9   10   11   12
'\a','\b',     '\f','\n','\r','\t','\v','\?','\'','\"','\\','\0',	//������'\0'��strescape()  �ōs��strchr()�̏I�[�p�ł��B�G�X�P�[�v�Ώە����ł͂���܂���B
 'a', 'b',      'f', 'n', 'r', 't', 'v','\?','\'','\"','\\','\0'};	//������'\0'��strcompress()�ōs��strchr()�̏I�[�p�ł��B�G�X�P�[�v�Ώە����ł͂���܂���B
#endif//__GNUC__
// * Thu May 26 23:31:18 JST 2016 Naoyuki Sawa
// - strescape()���}���`�o�C�g�Ή����܂����B
//   ���̑Ή��ɂ��Astrescape()��strcompress()�̑Ώ̐����������܂����B
//   �ڍׂ́A'Wed May 25 10:22:02 JST 2016'�̃R�����g���Q�Ƃ��ĉ������B
//   �ȉ��̓���m�F���s���܂����B
//   ��//���L�̃e�X�g�֐����痘�p����T�u���[�`��
//   ��void test_subr(const char* s, const char* need_escape) {
//   ��  char *t, *u;
//   ��  t = strescape(s, need_escape);	//���G�X�P�[�v�������ʂ�\������B
//   ��  printf("'%s'\n", t);		//��
//   ��  u = strcompress(t, NULL, NULL);	//���G�X�P�[�v���������Č��̕�����ƃo�C�i����v���鎖���m�F����B
//   ��  if(strcmp(s, u)) { DIE(); }	//��
//   ��  free(t);
//   ��  free(u);
//   ��}
//   ��//���}���`�o�C�g�Ή��ł��l�X�ȃP�[�X�Ő��������삵�Ă��鎖���m�F����e�X�g
//   ��void test() {
//   ��  test_subr("\1\2\3\4\5\6\7\10\11\12\13\14\15\16\17", NULL);
//   ��  //��'\001\002\003\004\005\006\a\b\t\n\v\f\r\016\017'
//   ��  test_subr("C:\\usr\\PIECE\\docs\\PIECE �|�[�g���.htm", NULL);
//   ��  //��'C:\\usr\\PIECE\\docs\\PIECE �|�[�g���.htm'
//   ��  test_subr("ABC.DEF[123]", NULL);
//   ��  //��'ABC.DEF[123]'
//   ��  test_subr("ABC.DEF[123]", ".[]");
//   ��  //��'ABC\.DEF\[123\]'
//   ��  test_subr("�\��.�\��[�\��]", NULL);
//   ��  //��'�\��.�\��[�\��]'				//�S�p�����̓G�X�P�[�v�����ɂ��̂܂ܕ\������悤�ɂȂ����B
//   ��  test_subr("�\��.�\��[�\��]", ".[]");
//   ��  //��'�\��\.�\��\[�\��\]'
//   ��  test_subr("�\���D�\��m�\���n", "�D�m�n");	//�S�p�������G�X�P�[�v�Ώۂɏo���܂��B
//   ��  //��'�\��\�D�\��\�m�\��\�n'
//   ��}
#ifndef PIECE
char* strescape(const char* s, const char* need_escape) {
	char* r = NULL;
	for(;;) {//1���ڂŕ������𐔂��A2���ڂŕ������i�[����
		const char *t = s, *p;
		      char *u = r;
		int c;
	//{{2016/05/26�ύX:strescape()���}���`�o�C�g�Ή����܂����B
	//	while((c = *t++)) {
	//��2016/05/26�ύX:strescape()���}���`�o�C�g�Ή����܂����B
		for(;;) {
			c = mbsnextc(t);
			t = mbsinc(t);
			if(!c) { break; }
	//}}2016/05/26�ύX:strescape()���}���`�o�C�g�Ή����܂����B
			/* C����̃G�X�P�[�v�V�[�P���X */
	//{{2016/05/26�ύX:strescape()���}���`�o�C�g�Ή����܂����B
	//		if((p = strchr(TBL_strescape, c))) {		//�e�[�u���̑O�����猟��
	//��2016/05/26�ύX:strescape()���}���`�o�C�g�Ή����܂����B
			if((p = mbschr(TBL_strescape, c))) {		//�e�[�u���̑O�����猟��
	//}}2016/05/26�ύX:strescape()���}���`�o�C�g�Ή����܂����B
				if(r/*2����*/) {
					u[0] = '\\';
					u[1] = *(p + SIZE_strescape);	//�e�[�u���̌㔼�փV�t�g
				}
				u += 2;
			/* �A�v���P�[�V������`�̃G�X�P�[�v���K�v�ȕ��� */
	//{{2016/05/26�ύX:strescape()���}���`�o�C�g�Ή����܂����B
	//		} else if(need_escape && strchr(need_escape, c)) {
	//			if(r/*2����*/) {
	//				u[0] = '\\';
	//				u[1] = c;
	//			}
	//			u += 2;
	//��2016/05/26�ύX:strescape()���}���`�o�C�g�Ή����܂����B
			} else if(need_escape && mbschr(need_escape, c)) {
				if(c > UCHAR_MAX) {
					if(r/*2����*/) {
						u[0] = '\\';
						u[1] = (c >> 8);
						u[2] = c;
					}
					u += 3;
				} else {
					if(r/*2����*/) {
						u[0] = '\\';
						u[1] = c;
					}
					u += 2;
				}
	//}}2016/05/26�ύX:strescape()���}���`�o�C�g�Ή����܂����B
	//{{2016/05/26�ǉ�:strescape()���}���`�o�C�g�Ή����܂����B
			/* �S�p���� */
			} else if(c > UCHAR_MAX) {
				if(r/*2����*/) {
					u[0] = (c >> 8);
					u[1] = c;
				}
				u += 2;
	//}}2016/05/26�ǉ�:strescape()���}���`�o�C�g�Ή����܂����B
			/* �󎚉\�łȂ����� */
			} else if(!isprint(c)) {
				if(r/*2����*/) {
					u[0] = '\\';
					u[1] = '0' + ((c >> 6) & 3);	//��
					u[2] = '0' + ((c >> 3) & 7);	//�����̌��'0'�`'7'�������ꍇ�̂��߂ɏ�ɍő��3�����o�͂��Ă����B
					u[3] = '0' + ((c >> 0) & 7);	//��
				}
				u += 4;
			/* ����ȊO */
			} else {
				if(r/*2����*/) {
					u[0] = c;
				}
				u += 1;
			}
		}
		if(r/*2����*/) {
			u[0] = '\0';
			return r;
		}
//{{2016/09/07�ύX:strescape(),�y��,strcompress()���A(int)�ŃA�h���X���Z���s���Ă����̂��A(intptr_t)�ōs���悤�ɏC�����܂����B
//		if(!(r = malloc((int)u + 1/*nul*/))) { DIE(); }
//��2016/09/07�ύX:strescape(),�y��,strcompress()���A(int)�ŃA�h���X���Z���s���Ă����̂��A(intptr_t)�ōs���悤�ɏC�����܂����B
		if(!(r = malloc((intptr_t)u + 1/*nul*/))) { DIE(); }
//}}2016/09/07�ύX:strescape(),�y��,strcompress()���A(int)�ŃA�h���X���Z���s���Ă����̂��A(intptr_t)�ōs���悤�ɏC�����܂����B
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
	;//{{2016/05/26�ύX:strescape()���}���`�o�C�g�Ή����܂����B
	;//	ld.b		%r3, [%r1]+			;//  %r3  := c = *t++
	;//	cmp		%r3, 0				;//  if(!c) {
	;//	jrne		strescape_L30
	;//��2016/05/26�ύX:strescape()���}���`�o�C�g�Ή����܂����B
		xcall.d		mbsnextc			;//  %r10 := c = mbsnextc(t)
		ld.w		%r12, %r1			;//  %r12 :=              t					*delay*
		ld.w		%r3, %r10			;//  %r3  := c
		xcall.d		mbsinc				;//  %r10 := t = mbsinc(t)
		ld.w		%r12, %r1			;//  %r12 :=            t					*delay*
		cmp		%r3, 0				;//  if(!c) {
		jrne.d		strescape_L30
		ld.w		%r1, %r10			;//  %r1  := t							*delay*
	;//}}2016/05/26�ύX:strescape()���}���`�o�C�g�Ή����܂����B
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
		ld.b		[%r2], %r8			;//      *u    = '��0'
		ld.w		%r10, %r0			;//      return  r
		xadd		%sp, %sp, 8
		popn		%r3
		ret						;//    }
		;//---------------------------------------------;//  } else {
strescape_L30:
	;//{{2016/05/26�ύX:strescape()���}���`�o�C�g�Ή����܂����B
	;//	xld.w		%r12, TBL_strescape		;//    %r12 :=            TBL_strescape
	;//	xcall.d		strchr				;//    %r10 := p = strchr(TBL_strescape, c))
	;//	ld.w		%r13, %r3			;//    %r13 :=                           c			*delay*
	;//	cmp		%r10, 0				;//    if(    (p = strchr(TBL_strescape, c)) {
	;//	jreq		strescape_L40
	;//��2016/05/26�ύX:strescape()���}���`�o�C�g�Ή����܂����B
		xld.w		%r12, TBL_strescape		;//    %r12 :=            TBL_strescape
		xcall.d		mbschr				;//    %r10 := p = mbschr(TBL_strescape, c))
		ld.w		%r13, %r3			;//    %r13 :=                           c			*delay*
		cmp		%r10, 0				;//    if(    (p = mbschr(TBL_strescape, c)) {
		jreq		strescape_L40
	;//}}2016/05/26�ύX:strescape()���}���`�o�C�g�Ή����܂����B
		cmp		%r0, 0				;//      if(!r) { u += 2
		jreq		strescape_ADD_2			;//      } else {
		xld.w		%r9, 92				;//        %r9  := '��'
		ld.b		[%r2]+, %r9			;//        *u++  = '��'
		add		%r10, 13			;//        %r10 :=   p + SIZE_strescape
		ld.b		%r9, [%r10]			;//        %r9  := *(p + SIZE_strescape)
		ld.b		[%r2]+, %r9			;//        *u++  = *(p + SIZE_strescape)
		jp		strescape_L20			;//      }
		;//---------------------------------------------;//    } else
strescape_L40:
	;//{{2016/05/26�ύX:strescape()���}���`�o�C�g�Ή����܂����B
	;//	xld.w		%r12, [%sp+0]			;//    %r12 := need_escape
	;//	cmp		%r12, 0				;//    if((    need_escape) &&
	;//	jreq		strescape_L50
	;//	xcall.d		strchr				;//    strchr( need_escape, c)) {
	;//	ld.w		%r13, %r3			;//    %r13 :=              c					*delay*
	;//	cmp		%r10, 0
	;//	jreq		strescape_L50
	;//	cmp		%r0, 0				;//      if(!r) { u += 2
	;//	jreq		strescape_ADD_2			;//      } else {
	;//	xld.w		%r9, 92				;//        %r9  := '��'
	;//	ld.b		[%r2]+, %r9			;//        *u++  = '��'
	;//	ld.b		[%r2]+, %r3			;//        *u++  = c
	;//	jp		strescape_L20			;//      }
	;//��2016/05/26�ύX:strescape()���}���`�o�C�g�Ή����܂����B
		xld.w		%r12, [%sp+0]			;//    %r12 := need_escape
		cmp		%r12, 0				;//    if((    need_escape) &&
		jreq		strescape_L50
		xcall.d		mbschr				;//    mbschr( need_escape, c)) {
		ld.w		%r13, %r3			;//    %r13 :=              c					*delay*
		cmp		%r10, 0
		jreq		strescape_L50
		ld.w		%r4, %r3			;//      %r4  :=   c
		sra		%r4, 8				;//      %r4  :=  (c >> 8)
		xld.w		%r5, 92				;//                 %r5  := '��'	����������	(!r)�̎��͖��ʏ����ƂȂ邪�Q�͖����B�R�[�h�T�C�Y�ጸ��D�悷�鎖�ɂ����B
		jreq.d		strescape_L45			;//      if(      (c >> 8)) {			��
		cmp		%r0, 0				;//      %psr(Z) := !r			������	��		*delay*
		jreq		strescape_ADD_3			;//        if(!r) { u += 3		������	��
	  ;//	xld.w		%r5, 92				;//        } else { %r5  := '��'	����������
		ld.b		[%r2]+, %r5			;//                 *u++  = '��'	�@�@��	��
		ld.b		[%r2]+, %r4			;//                 *u++  = (c >> 8)	�@�@��	��
		ld.b		[%r2]+, %r3			;//                 *u++  =  c		�@�@��	��
		jp		strescape_L20			;//        }				�@�@��	��
strescape_L45:							;//      } else {			�@�@��	��
		jreq		strescape_ADD_2			;//        if(!r) { u += 2		������	��
	  ;//	xld.w		%r5, 92				;//        } else { %r5  := '��'	����������
		ld.b		[%r2]+, %r5			;//                 *u++  = '��'
		ld.b		[%r2]+, %r3			;//                 *u++  =  c
		jp		strescape_L20			;//      } }
	;//}}2016/05/26�ύX:strescape()���}���`�o�C�g�Ή����܂����B
		;//---------------------------------------------;//    } else
strescape_L50:
	;//{{2016/05/26�ǉ�:strescape()���}���`�o�C�g�Ή����܂����B
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
	;//}}2016/05/26�ǉ�:strescape()���}���`�o�C�g�Ή����܂����B
		xcall.d		isprint				;//    if(isprint(c)) {
		ld.w		%r12, %r3			;//    %r12 :=    c						*delay*
		cmp		%r10, 0
		jrne		strescape_L60
		cmp		%r0, 0				;//      if(!r) { u += 4
		jreq		strescape_ADD_4			;//      } else {
		xld.w		%r9, 92				;//        %r9  := '��'
		ld.b		[%r2]+, %r9			;//        *u++  = '��'
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
	;//{{2016/05/26�ύX:strescape()���}���`�o�C�g�Ή����܂����B
	;//	add		%r2, 2				;//%r2  := u += 2 + 1 + 1
	;//��2016/05/26�ύX:strescape()���}���`�o�C�g�Ή����܂����B
		add		%r2, 1				;//%r2  := u += 1+1+1 + 1
strescape_ADD_3:
		add		%r2, 1				;//%r2  := u +=   1+1 + 1
	;//}}2016/05/26�ύX:strescape()���}���`�o�C�g�Ή����܂����B
strescape_ADD_2:
		add		%r2, 1				;//%r2  := u +=     1 + 1
strescape_ADD_1:
		jp.d		strescape_L20
		add		%r2, 1				;//%r2  := u +=         1					*delay*
");
static void __attribute__((noreturn,unused))/*asm�u���b�N����Q��*/ strescape_DIE() { DIE(); }
#endif//PIECE

// * Wed May 25 10:22:02 JST 2016 Naoyuki Sawa
// - C����ł�strcompress()���}���`�o�C�g�Ή����܂����B
//   strcompress()���}���`�o�C�g�Ή������ړI�́Aclipyaml.c���W���[����yaml_path_get_node(),yaml_path_set_string()�Ŏg�p���邽�߂ł��B
//   �ڍׂ́Aclipyaml.c���W���[���̓����̃R�����g���Q�Ƃ��ĉ������B
// - �A�Z���u���ł�strcompress()�͂܂��A�}���`�o�C�g�Ή����Ă��܂���B
//   �A�Z���u���ł�strcompress()���A�}���`�o�C�g�Ή�����K�v���L��܂��B	��{{2016/05/26�R�����g�ǋL:�A�Z���u���ł�strcompress()���}���`�o�C�g�Ή����܂����B}}
// - strcompress()��strescape()�Ƒ΂𐬂��֐��ł����A����strcompress()�������}���`�o�C�g�Ή��������ɂ��Astrcompress()��strescape()�̔�Ώ̐��������܂����B
//   ���p�����������g������͖��͐����܂��񂪁A�S�p�������g�p����ƁAstrescape()�ŃG�X�P�[�v�����������strcompress()�ŕ����������ɁA���̕�����ɂȂ�Ȃ��\�����L��܂��B
//   �Ώ̐���ۂ��߂ɁAstrescape()���}���`�o�C�g�Ή�����K�v���L��܂��B	��{{2016/05/26�R�����g�ǋL:strescape()���}���`�o�C�g�Ή����܂����B}}
// * Thu May 26 20:37:02 JST 2016 Naoyuki Sawa
// - �A�Z���u���ł�strcompress()���}���`�o�C�g�Ή����܂����B
//   �ȉ��̓���m�F���s���܂����B
//   ��//���L�̃e�X�g�֐����痘�p����T�u���[�`��
//   ��void test_subr(const char* s, const char* delim) {
//   ��  do {
//   ��    char* t = strcompress(s, (char**)&s, delim);
//   ��    printf("'%s'\n", t);
//   ��    free(t);
//   ��    s = mbsninc(s, 1);  //s������'\0'���w���Ă����ꍇ�ɐi�܂Ȃ��悤�ɂ��邽��mbsninc()���g���K�v���L�鎖�ɒ��ӂ���Bmbsinc()�ł�s������'\0'���w���Ă��Ă��i��ł��܂��B
//   ��  } while(*s);
//   ��}
//   ��//���}���`�o�C�g�Ή��ɂȂ��������m�F����e�X�g
//   ��void test1() {
//   ��  test_subr("�\��\\�\��\\�\��", "\\");
//   ��  //���ύX�O
//   ��  //��'\x95'	���ύX�O�́A�S�p������2�o�C�g�ڂ�'\'���ԈႦ�ăG�X�P�[�v��'\'�ƌ��Ȃ��Ă��܂��Ă����B
//   ��  //��'��'
//   ��  //��'\x8F'
//   ��  //��'��'
//   ��  //��'\x97'
//   ��  //��'��'
//   ��  //���ύX�O
//   ��  //��'�\��'	���ύX��́A�������Ȃ����B
//   ��  //��'�\��'
//   ��  //��'�\��'
//   ��}
//   ��//���}���`�o�C�g�Ή��ł��l�X�ȃP�[�X�Ő��������삵�Ă��鎖���m�F����e�X�g(�ȉ��͑S�ĕύX��̏o�͂ł�)
//   ��void test2() {
//   ��  test_subr("", NULL);
//   ��  //��''
//   ��  test_subr("�ݶ�", NULL);
//   ��  //��'�ݶ�'
//   ��  test_subr("�S�p", NULL);
//   ��  //��'�S�p'
//   ��  test_subr("\\x30\\x31\\x32", NULL);
//   ��  //��'012'
//   ��  test_subr("\\x30\\x31\\x32", "\\");
//   ��  //��''
//   ��  //��'x30'
//   ��  //��'x31'
//   ��  //��'x32'
//   ��  test_subr("������.����[����]", ".[]");
//   ��  //��'������'
//   ��  //��'����'
//   ��  //��'����'
//   ��  test_subr("�������D�����m�����n", "�D�m�n");		//�S�p�����̃f���~�^�����p�o����悤�ɂȂ����B
//   ��  //��'������'
//   ��  //��'����'
//   ��  //��'����'
//   ��  test_subr("������\\.����\\[����\\]", ".[]");
//   ��  //��'������.����[����]'
//   ��  test_subr("������\\�D����\\�m����\\�n", "�D�m�n");	//�S�p�����̃f���~�^��'\'�ŃG�X�P�[�v�o���܂��B
//   ��  //��'�������D�����m�����n'
//   ��}
#ifndef PIECE
char* strcompress(const char* s, char** endptr, const char* delim) {
	char* r = NULL;
	for(;;) {//1���ڂŕ������𐔂��A2���ڂŕ������i�[����
		const char *t = s, *p;
		      char *u = r;
		int c;
	//{{2016/05/25�ύX:strcompress()���}���`�o�C�g�Ή����܂����B
	//	while((c = *t++) && !(delim && strchr(delim, c))) {
	//��2016/05/25�ύX:strcompress()���}���`�o�C�g�Ή����܂����B
		for(;;) {
			c = mbsnextc(t);
			t = mbsinc(t);
			if(!c) { break; }
			if(delim && mbschr(delim, c)) { break; }
	//}}2016/05/25�ύX:strcompress()���}���`�o�C�g�Ή����܂����B
			if(c != '\\') { goto L_COPY; }
			/* '\'��nul�������ꍇ�A'\'��j�����ď���2.���s���I������B('\'��nul�������̂�'�G�X�P�[�v���ꂽ������'�Ƃ��ĕs�������A������̏I�[�𒴂��Ȃ��悤�ɍŒ���̑΍���s�����Ƃɂ���)
			 * �ustrchr(�`,'\0')�v�͕K����NULL�̌��ʂɂȂ�̂ŁAstrchr()��p���锻�������ɍs��Ȃ���΂Ȃ�Ȃ����Ƃɒ��ӂ���B */
	//{{2016/05/25�ύX:strcompress()���}���`�o�C�g�Ή����܂����B
	//		if(!(c = *t++)) { break; }
	//��2016/05/25�ύX:strcompress()���}���`�o�C�g�Ή����܂����B
			c = mbsnextc(t);
			t = mbsinc(t);
			if(!c) { break; }
	//}}2016/05/25�ύX:strcompress()���}���`�o�C�g�Ή����܂����B
			/* '\'�ɑ���C����̃G�X�P�[�v�V�[�P���X�������A1�����ɕϊ����ďo�͂���B */
	//{{2016/05/25�ύX:strcompress()���}���`�o�C�g�Ή����܂����B
	//		if((p = strchr(TBL_strescape + SIZE_strescape, c))) {	//�e�[�u���̌㔼���猟��
	//��2016/05/25�ύX:strcompress()���}���`�o�C�g�Ή����܂����B
			if((p = mbschr(TBL_strescape + SIZE_strescape, c))) {	//�e�[�u���̌㔼���猟��
	//}}2016/05/25�ύX:strcompress()���}���`�o�C�g�Ή����܂����B
				if(r/*2����*/) { *u = *(p - SIZE_strescape); }	//�e�[�u���̑O���փV�t�g
				u++;
			/* '\'�ɑ���8�i�\�L(1�`3��)���A1�����ɕϊ����ďo�͂���B */
			} else if((c >= '0') && (c <= '7')) {
				int x = (c - '0');
				if(((c = *t) >= '0') && (c <= '7')) {
					x = (x << 3) | (c - '0'), t++;
					if(((c = *t) >= '0') && (c <= '7')) {
						x = (x << 3) | (c - '0'), t++;
					}
				}
				if(r/*2����*/) { *u = x; }
				u++;
			/* '\'�ɑ���16�i�\�L(1�`����)���A1�����ɕϊ����ďo�͂���B */
			} else if((c == 'x') || (c == 'X')) {
				int x = strtoul(t, (char**)&t, 16);
				if(r/*2����*/) { *u = x; }
				u++;
			/* �s�̌p�� */
			} else if(c == '\n') {
				/** no job **/
			/* '\'�ɑ�������ȊO�̕������A'\'���폜���ĕ����݂̂ɕϊ����ďo�͂���B('�A�v���P�[�V������`�̃G�X�P�[�v���K�v�ȕ���'���܂�) */
			} else {
L_COPY:
	//{{2016/05/25�ǉ�:strcompress()���}���`�o�C�g�Ή����܂����B
				if(c > UCHAR_MAX) {
					if(r/*2����*/) { *u = (c >> 8); }
					u++;
				}
	//}}2016/05/25�ǉ�:strcompress()���}���`�o�C�g�Ή����܂����B
				if(r/*2����*/) { *u = c; }
				u++;
			}
		}
		if(r/*2����*/) {
	//{{2016/05/25�ύX:strcompress()���}���`�o�C�g�Ή����܂����B
	//		if(endptr) { *endptr = (char*)(t - 1); }
	//��2016/05/25�ύX:strcompress()���}���`�o�C�g�Ή����܂����B
			if(endptr) { *endptr = mbsdec(s, t); }
	//}}2016/05/25�ύX:strcompress()���}���`�o�C�g�Ή����܂����B
			*u = '\0';
			return r;
		}
//{{2016/09/07�ύX:strescape(),�y��,strcompress()���A(int)�ŃA�h���X���Z���s���Ă����̂��A(intptr_t)�ōs���悤�ɏC�����܂����B
//		if(!(r = malloc((int)u + 1/*nul*/))) { DIE(); }
//��2016/09/07�ύX:strescape(),�y��,strcompress()���A(int)�ŃA�h���X���Z���s���Ă����̂��A(intptr_t)�ōs���悤�ɏC�����܂����B
		if(!(r = malloc((intptr_t)u + 1/*nul*/))) { DIE(); }
//}}2016/09/07�ύX:strescape(),�y��,strcompress()���A(int)�ŃA�h���X���Z���s���Ă����̂��A(intptr_t)�ōs���悤�ɏC�����܂����B
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
	;//{{2016/05/26�ύX:�A�Z���u���ł�strcompress()���}���`�o�C�g�Ή����܂����B
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
	;//��2016/05/26�ύX:�A�Z���u���ł�strcompress()���}���`�o�C�g�Ή����܂����B
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
	;//}}2016/05/26�ύX:�A�Z���u���ł�strcompress()���}���`�o�C�g�Ή����܂����B
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
	;//{{2016/05/26�ύX:�A�Z���u���ł�strcompress()���}���`�o�C�g�Ή����܂����B
	;//	xld.w		%r9, [%sp+8]			;//      %r9  := endptr
	;//	cmp		%r9, 0				;//      if(endptr) {
	;//	jreq		3
	;//	 sub		%r1, 1				;//        *endptr = t - 1
	;//	 ld.w		[%r9], %r1			;//      }
	;//��2016/05/26�ύX:�A�Z���u���ł�strcompress()���}���`�o�C�g�Ή����܂����B
		xld.w		%r3, [%sp+8]			;//      %r3  := endptr
		cmp		%r3, 0				;//      if(endptr) {						!INTERLOCK!
		jreq		strcompress_L26
		xld.w		%r12, [%sp+4]			;//        %r12 :=          s
		xcall.d		mbsdec				;//        %r10 :=   mbsdec(s, t)
		ld.w		%r13, %r1			;//        %r13 :=             t				*delay*
		ld.w		[%r3], %r10			;//        *endptr = mbsdec(s, t)
strcompress_L26:						;//      }
	;//}}2016/05/26�ύX:�A�Z���u���ł�strcompress()���}���`�o�C�g�Ή����܂����B
		ld.b		[%r2], %r8			;//      *u    = '��0'
		ld.w		%r10, %r0			;//      return  r
		xadd		%sp, %sp, 16
		popn		%r3
		ret						;//    }
		;//---------------------------------------------;//  } else {
strcompress_L30:
		xcmp		%r3, 92				;//    if(c != '��') { goto COPY }
		jrne		strcompress_COPY
	;//{{2016/05/26�ύX:�A�Z���u���ł�strcompress()���}���`�o�C�g�Ή����܂����B
	;//	ld.b		%r3, [%r1]+			;//    %r3  := c = *t++
	;//	cmp		%r3, 0				;//    if(!c) { goto BREAK }
	;//	jreq		strcompress_BREAK
	;//��2016/05/26�ύX:�A�Z���u���ł�strcompress()���}���`�o�C�g�Ή����܂����B
		xcall.d		mbsnextc			;//    %r10 := c = mbsnextc(t)
		ld.w		%r12, %r1			;//    %r12 :=              t					*delay*
		ld.w		%r3, %r10			;//    %r3  := c
		xcall.d		mbsinc				;//    %r10 := t = mbsinc(t)
		ld.w		%r12, %r1			;//    %r12 :=            t					*delay*
		cmp		%r3, 0				;//    if(!c) { break }
		jreq.d		strcompress_BREAK		;//    
		ld.w		%r1, %r10			;//    %r1  := t						*delay*
	;//}}2016/05/26�ύX:�A�Z���u���ł�strcompress()���}���`�o�C�g�Ή����܂����B
	;//{{2016/05/26�ύX:�A�Z���u���ł�strcompress()���}���`�o�C�g�Ή����܂����B
	;//	xld.w		%r12, TBL_strescape+13		;//    %r12 :=            TBL_strescape + SIZE_strescape
	;//	xcall.d		strchr				;//    %r10 := p = strchr(TBL_strescape + SIZE_strescape, c))
	;//	ld.w		%r13, %r3			;//    %r13 :=                                            c	*delay*
	;//	cmp		%r10, 0				;//    if(    (p = strchr(TBL_strescape + SIZE_strescape, c)) {
	;//	jreq		strcompress_L40
	;//��2016/05/26�ύX:�A�Z���u���ł�strcompress()���}���`�o�C�g�Ή����܂����B
		xld.w		%r12, TBL_strescape+13		;//    %r12 :=            TBL_strescape + SIZE_strescape
		xcall.d		mbschr				;//    %r10 := p = mbschr(TBL_strescape + SIZE_strescape, c))
		ld.w		%r13, %r3			;//    %r13 :=                                            c	*delay*
		cmp		%r10, 0				;//    if(    (p = mbschr(TBL_strescape + SIZE_strescape, c)) {
		jreq		strcompress_L40
	;//}}2016/05/26�ύX:�A�Z���u���ł�strcompress()���}���`�o�C�g�Ή����܂����B
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
		cmp		%r3, 10				;//    if(c == '��n') {
		jreq		strcompress_L20			;//      /** no job **/
		;//---------------------------------------------;//    } else {
strcompress_COPY:
	;//{{2016/05/26�ǉ�:�A�Z���u���ł�strcompress()���}���`�o�C�g�Ή����܂����B
		ld.w		%r9, %r3			;//      %r9  :=   c
		sra		%r9, 8				;//      %r9  :=  (c >> 8)
		jreq		5				;//      if(      (c >> 8)) {
		 cmp		%r0, 0				;//        if(r) {
		 jreq		2				;//          *u = (c >> 8)
		  ld.b		[%r2], %r9			;//        }
		 add		%r2, 1				;//        %r2  := u += 1   }
	;//}}2016/05/26�ǉ�:�A�Z���u���ł�strcompress()���}���`�o�C�g�Ή����܂����B
		cmp		%r0, 0				;//      if(!r) { u += 1
		jreq		strcompress_ADD_1		;//      } else {
		ld.b		[%r2]+, %r3			;//        *u++  = c
		jp		strcompress_L20			;//      }
		;//---------------------------------------------;//} } }
strcompress_ADD_1:
		jp.d		strcompress_L20
		add		%r2, 1				;//%r2  := u += 1						*delay*
");
static void __attribute__((noreturn,unused))/*asm�u���b�N����Q��*/ strcompress_DIE() { DIE(); }
#endif//PIECE

/*****************************************************************************
 *	������|�C���^�z��
 *****************************************************************************/

#ifndef PIECE
void strfreev(char** str_array) {
	/* str_array���̂�NULL�|�C���^�łȂ���΁c */
	if(str_array) {
		char **p = str_array, *str;
		/* NULL�|�C���^�v�f�ɓ��B����܂ŁA��������J������B */
		while((str = *p++)) { free(str); }
		/* str_array���̂��J������B */
		free(str_array);
	}
}
#else /*PIECE*/
void strfreev(char** str_array);
//�f���ȃo�[�W����(16����)�ƁA4���ߐߖ�o�[�W����(12����)���쐬���܂����B�T�C�Y��D�悵�Č�҂��g�p���邱�Ƃɂ��܂����B
//strfreev()���g�p����悤�ȏ����́A���x�����܂�d�v�łȂ����Ƃ������A�����ł������������ǂ��Ǝv��������ł��B
#if 0
//�ق�C����ǂ���f���Ɏ��������o�[�W�����ł��B
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
//4���ߐߖ񂵂��o�[�W�����B�����������ᑬ�ł��B
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
		call		strfreev_L30		;// free(str)		�uxcall free�v�Ə��������A���̕���ext����1�ߖ�ł���B
		jp		strfreev_L10		;//}
strfreev_L20:
		popn		%r12
strfreev_L30:
		xjp		free			;//free(str_array)	(!str_array)�̏ꍇ�����s����邪�Afree(NULL)�͈��S�ł���B
");
#endif
#endif/*PIECE*/

#ifndef PIECE
size_t strv_length(char** str_array) {
	/* NULL�|�C���^�v�f�ɑ΂���(+1)�𑊎E���邽�߁A�����l��(-1)�Ƃ��Ă����B */
	int n = -1;
	/* NULL�|�C���^�v�f���܂߂āA�v�f����(+1)����BNULL�|�C���^�v�f�ɑ΂���(+1)�́A�����l��(-1)�Ƒ��E�����B */
	do { n++; } while(*str_array++);
	/* �v�f����Ԃ��B */
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
	/* ������|�C���^�z���(�v�f��+1(�I�[��NULL))�́A���������m�ۂ���B
	 * calloc()���g�p���Ċm�ۂ���̂ŁA�I�[��NULL�͊��ɐݒ肳��Ă���B */
	p = q = calloc(strv_length(str_array) + 1/*�I�[��NULL*/, sizeof(char*));
	if(!p) { DIE(); }
	goto L_START;
	do {
		/* ������𕡐�����B */
		r = strdup(r);
		if(!r) { DIE(); }
		/* ����������������i�[����B */
		*q++ = r;
		/* ��������擾����B�I�[�Ȃ�΁A������B */
L_START:	r = *str_array++;
	} while(r);
	/* ��������������|�C���^�z���Ԃ��B */
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
static void __attribute__((noreturn,unused))/*asm�u���b�N����Q��*/ strdupv_DIE() { DIE(); }
#endif//PIECE

/****************************************************************************
 *	������������
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
static void __attribute__((noreturn,unused))/*asm�u���b�N����Q��*/ strdup_vprintf_DIE() { DIE(); }
#endif//PIECE

/*****************************************************************************
 *	�A��
 *****************************************************************************/

#ifndef PIECE
char* strconcat(const char* s, ...) {
	va_list ap;
	char *newStr, *oldStr;
	/* ������̏����l���A""�Ƃ���B */
	if(!(newStr = strdup(""))) { DIE(); }
	/* ����s,�y��,�ψ������ANULL�|�C���^�v�f�ɓ��B����܂Łc */
	va_start(ap, s);
	while(s) {	//����s��NULL�������ꍇ�́A���[�v�ɓ��炸�ɔ�����B
		/* �Â�������ƁA�v�f�̕������A�����A�V����������Ƃ���B */
		newStr = strjoin(NULL, (oldStr = newStr), s, NULL);
		/* �Â���������J������B */
		free(oldStr);
		/* ���̉ψ������擾����B */
		s = va_arg(ap, char*);
	}
	va_end(ap);
	/* �A�������������Ԃ��B */
	return newStr;
}
char* strjoin(const char* separator, ...) {
	va_list ap;
	char *newStr, *oldStr, *str;
	/* ��v�f�ڂɐ�s����Z�p���[�^��""�Œ�Ƃ���B */
	const char* sep = "";
	/* ������̏����l���A""�Ƃ���B */
	if(!(newStr = strdup(""))) { DIE(); }
	/* �ψ������ANULL�|�C���^�v�f�ɓ��B����܂Łc */
	va_start(ap, separator);
	while((str = va_arg(ap, char*))) {
		/* �Â�������ƁA�Z�p���[�^�ƁA�v�f�̕������A�����A�V����������Ƃ���B */
		newStr = strdup_printf("%s%s%s", (oldStr = newStr), sep, str);
		/* �Â���������J������B */
		free(oldStr);
		/* �Z�p���[�^���w�肳��Ă�����A��v�f�ڈȍ~�ɐ�s����Z�p���[�^�Ƃ���B */
		if(separator) { sep = separator; }
	}
	va_end(ap);
	/* �A�������������Ԃ��B */
	return newStr;
}
char* strjoinv(const char* separator, char** str_array) {
	char *newStr, *oldStr, *str;
	/* ��v�f�ڂɐ�s����Z�p���[�^��""�Œ�Ƃ���B */
	const char* sep = "";
	/* ������̏����l���A""�Ƃ���B */
	if(!(newStr = strdup(""))) { DIE(); }
	/* ������|�C���^�z�񂪁ANULL�|�C���^�v�f�ɓ��B����܂Łc */
	while((str = *str_array++)) {
		/* �Â�������ƁA�Z�p���[�^�ƁA�v�f�̕������A�����A�V����������Ƃ���B */
		newStr = strdup_printf("%s%s%s", (oldStr = newStr), sep, str);
		/* �Â���������J������B */
		free(oldStr);
		/* �Z�p���[�^���w�肳��Ă�����A��v�f�ڈȍ~�ɐ�s����Z�p���[�^�Ƃ���B */
		if(separator) { sep = separator; }
	}
	/* �A�������������Ԃ��B */
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
		xadd		%r13, %sp, 4			;//%r13 := str_array = &{s,...}		//s��str_array�̐擪�v�f�Ƃ��邱�Ƃɒ��ӂ���B
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
		xld.w		%r12, strjoinv_FMT		;//%r12     := �g%s%s%s�h	�y���Ӂz���p�̓�d���p�����g����asm�u���b�N�̕�����I�[�ƌ��Ȃ����̂Ŏg�p�s�B
		xld.w		[%sp+ 0], %r12			;//[%sp+ 0] := �g%s%s%s�h
		add		%r12, 6				;//%r12     :=       �g�h
		xld.w		[%sp+ 8], %r12			;//[%sp+ 8] := sep = �g�h
		xcall		strdup				;//%r10     := newStr = strdup(�g�h)
		cmp		%r10, 0				;//if(!newStr) { DIE() }
		jreq		strjoinv_DIE
		xld.w		[%sp+ 4], %r10			;//[%sp+ 4] := oldStr = newStr			�ŏ��̃��[�v��oldStr
strjoinv_L10:	;//---------------------------------------------;//for(;;) {
		;//%r0      := separator
		;//%r1      := str_array
		;//%r10     := newStr
		;//[%sp+ 0] := �g%s%s%s�h	strdup_printf�ւ̈���
		;//[%sp+ 4] := oldStr		strdup_printf�ւ̈���
		;//[%sp+ 8] := sep		strdup_printf�ւ̈���
		;//[%sp+12] := str		strdup_printf�ւ̈���
		ld.w		%r9, [%r1]+			;//  %r9      := str = *str_array++
		cmp		%r9, 0
		jreq		strjoinv_L20
		xld.w		[%sp+12], %r9			;//  [%sp+12] := str
		xcall		strdup_printf			;//  %r10     := newStr = strdup_printf(�g%s%s%s�h, oldStr, sep, str)
		xld.w		%r12, [%sp+ 4]			;//  %r12     := oldStr		����������
		xld.w		[%sp+ 4], %r10			;//  [%sp+ 4] := oldStr = newStr	��	����̃��[�v��oldStr
		xcall		free				;//         free(oldStr)	����������
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
static void __attribute__((noreturn,unused))/*asm�u���b�N����Q��*/ strjoinv_DIE() { DIE(); }
static const char strjoinv_FMT[] = "%s%s%s";
#endif/*PIECE*/

/****************************************************************************
 *	����
 ****************************************************************************/

//������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������
//* Sun Mar 08 10:55:52 JST 2015 Naoyuki Sawa
//- strsplit_set()�ƃr�b�g�}�b�vGC�𕹗p���A������x�傫�ȃt�@�C���𕪊����悤�Ƃ���ƁA�r�b�g�}�b�vGC���G���[��~���鎖���L��܂��B
//  ���̖��ɂ��Ē����������ʂ́A�ȉ��̒ʂ�ł��B
//���Č����@
//��//�\�ߢisd.exe =w \Home\Share\Piece\clip\getline.c����Ƃ���P/ECE�ɉ���÷�Ă�]�����Ă����������x�傫��̧�قŤ���s������CR+LF���Ɩ�肪�Č����Ղ��
//��FILE* fp = fopen("getline.c","r");
//��char* s = NULL;
//��int len = 0;
//��getdelim(&s,&len,0,fp);
//��//�ޯ�ϯ��GC���N�����顂����Ƒ����N�����Ă����Ă��������ʡ�ƯĂ͊֌W�����
//��BitmapGC_Init(3);
//��//CR,����,LF�𕪊������Ƃ��Ľ��دĂ���Ƥclipbmgc.c��new_pceHeapAlloc()���Ŵװ��~����
//��strsplit_set(s,"\r\n",0);
//������
//��������޸ނł͂Ȃ��strsplit_set()����؊m������݂Ƥ�ޯ�ϯ��GC�̓����̂��߂ɤ����׸���ð��݂��������Ĥ�傫�ȋ������ۯ����m�ۂł��Ȃ��Ȃ��Ă��܂������L��̂������ł��
//����̓I�ɂͤstrsplit_set()����Ăяo�����strsplit_sub()�̒��Łc
//���@��������߲���z����g�����顣�����Ť1�v�f��argv���m�ۂ��顁������ۯ����蓖��
//���Aargv[0]�ɤ�ŏ���'\r'�܂ł̕�����𕡐����Ċi�[���顁������ۯ����蓖��
//���B��������߲���z����g�����顣�����Ť2�v�f��argv�Ɋg�����顂��̎���@�̌��ɇA�����蓖�čς݂Ȃ̂ł��̏��realloc�ł���free+malloc�ɂȂ��ޯ�ϯ��GC��free��ۗ�����malloc�̂ݍs����������ۯ����蓖��
//���Cargv[1]�ɤ'\r'��'\n'�̊Ԃ̕�����,����,�󕶎���𕡐����Ċi�[���顁������ۯ����蓖��
//���D�e�s�ɂ��ć@�`�C�̏������J��Ԃ��
//���E�C�̂��߂Ɏ��ۂ̍s���̓�{�ׂ̍��ȕ������s���Ĥ���Ȃ葁������؂������Ȃ顂���Ƥ�ޯ�ϯ��GC���N������
//���F�A�ƇC�ŕ�������������͊J���ł��Ȃ���@�ƇB�Ŋm�ۂ���argv�̂�����Ō�Ɋm�ۂ������̈ȊO���J�������
//���G�����������ɂ���ĊJ������؂ͤ�����_��argv[argc]�������Ȃ��悤�f���������ۯ��Ȃ̂Ť�V����argv[argc]���m�ۂ���ɂ͏����߂���
//���H���ʂƂ��Ĥclipbmgc.c��new_pceHeapAlloc()��2nd try�����s���Ĥ�װ��~����
//���l�@
//�����̖����������ɂͤargv���܂Ƃ߂Ċg������Ƃ�����X�ޯ�ϯ��GC���N�����đ��߂ɕs�v��argv����؂��J�����Ĥ���̽�߰��ɕ����񂪕��������悤�ɂ���Ƃ���΍����@�������͂Ȃ�����΍�͍s��Ȃ����ɂ����
//���O�q�̂Ƃ���strsplit_set()����؊m������݂Ƥ�ޯ�ϯ��GC�̓����ɂ��'�������'�������ł����޸ނł͂Ȃ�����ł��
//������̗p�r�ɍ��킹�đ΍���s���Ƥstrsplit_set()���ޯ�ϯ��GC�̒P�������������ɂȂ褂��̕����Q���傫���Ǝv��������ł��
//��ײ���ؑ��ł̑΍�͍s���܂�����ع���݂�����L�̎b��΍���s���ĉ�����Ă��������
//���b��΍�
//���傫��÷��(���ɤ��������Ƌ󕶎��񂪂������񐶂���悤�ȕ��������)�̕����ɂͤstrsplit_set()���g�p�����strtok()��strsep()�����g�p���Ă��������
//������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������
// * Sat May 21 22:36:02 JST 2016 Naoyuki Sawa
// - strsplit()��strsplit_set()���}���`�o�C�g�Ή����܂����B
//   �Â��R�[�h�ł͔z��̃������m�ۂ��֐����ōs���Ă��܂������A�V�����R�[�h�ł�strv.c�̋@�\���g���悤�ɕύX���A�R�[�h���Ȍ��ɂȂ�܂����B
//   �R�[�h���Ȍ��ɂȂ������Ƃɂ���āA�}���`�o�C�g�Ή���ǉ������ɂ�������炸�A�����W���[���̃R�[�h�T�C�Y����24�o�C�g�������Ȃ�܂����B
//   �������Astrv.o�������N�����悤�ɂȂ������R�[�h�T�C�Y����������̂ŁA�S�̂Ƃ��Ă̓R�[�h�T�C�Y�͑������Ă���Ǝv���܂��B
// - �V�����R�[�h�Ŏ�����啝�ɕύX���܂������A��{�I�ȃA���S���Y���͌Â��R�[�h�ƕς���Ă��܂���B
//   �V�����R�[�h�ɂ͏ڍׂȃR�����g��t���Ă��Ȃ��̂ŁA�A���S���Y���ɂ��Ă͌Â��R�[�h�̃R�����g���Q�Ƃ��ĉ������B
// - �}���`�o�C�g��Ή��̌Â��R�[�h���A����܂Œ������肵�ē��삵�Ă������т��L��̂ŁA�R�[�h���c���Ă������ɂ��܂����B
//   USE_STRSPLIT_MBCS�V���{�����`���ă��C�u�������r���h����ƁA�V�����}���`�o�C�g�Ή��ł�strsplit()��strsplit_set()���g�p���܂��B
//   USE_STRSPLIT_MBCS�V���{�����`�����Ƀ��C�u�������r���h����ƁA�Â��}���`�o�C�g��Ή��ł�strsplit()��strsplit_set()���g�p���܂��B
//   ��{�I�ɂ́A�V�����}���`�o�C�g�Ή��ł�strsplit()��strsplit_set()���g���\��ł����A��������}���`�o�C�g�Ή����s�v�ɂȂ����ꍇ�́A�Â��R�[�h�ɖ߂��ĉ������B
// - �����؃v���O����
//   ��void test1() {
//   ��  char** s = strsplit("�\��\\�\��\\�\��", "\\", 0);
//   ��  char** t = s;
//   ��  while(*t) { printf("'%s'\n", *t++); }
//   ��  strfreev(s);
//   ��}
//   ��//������(�}���`�o�C�g��Ή���)�c���
//   ��//��'\x95'  �u�\�v��2�o�C�g�ڂ�'\'���}�b�`���Ă��܂��Ă���B
//   ��//��'��'
//   ��//��'\x8F'  �u�\�v��2�o�C�g�ڂ�'\'���}�b�`���Ă��܂��Ă���B
//   ��//��'��'
//   ��//��'\x97'  �u�\�v��2�o�C�g�ڂ�'\'���}�b�`���Ă��܂��Ă���B
//   ��//��'��'
//   ��//������(�}���`�o�C�g�Ή���)�c������
//   ��//��'�\��'
//   ��//��'�\��'
//   ��//��'�\��'
//   ��void test2() {
//   ��  char** s = strsplit_set("�\��\\�\��\\�\��", "�\�\�\", 0);
//   ��  char** t = s;
//   ��  while(*t) { printf("'%s'\n", *t++); }
//   ��  strfreev(s);
//   ��}
//   ��//������(�}���`�o�C�g��Ή���)�c���
//   ��//��''      �u�\�v��1�o�C�g�ڂ��ʂɁu�\�v��1�o�C�g�ڂɃ}�b�`���Ă��܂��Ă���B
//   ��//��''      �u�\�v��2�o�C�g�ڂ��ʂɁu�\�v��2�o�C�g�ڂɃ}�b�`���Ă��܂��Ă���B
//   ��//��'��'    �u�\�v���́u�\�v���́u�\�v��2�o�C�g�ڂ�'\'�Ƀ}�b�`���Ă��܂��Ă���B
//   ��//��''      �u�\�v��1�o�C�g�ڂ��ʂɁu�\�v��1�o�C�g�ڂɃ}�b�`���Ă��܂��Ă���B
//   ��//��''      �u�\�v��2�o�C�g�ڂ��ʂɁu�\�v��2�o�C�g�ڂɃ}�b�`���Ă��܂��Ă���B
//   ��//��'��'    �u�\�v���́u�\�v���́u�\�v��2�o�C�g�ڂ�'\'�Ƀ}�b�`���Ă��܂��Ă���B
//   ��//��''      �u�\�v��1�o�C�g�ڂ��ʂɁu�\�v��1�o�C�g�ڂɃ}�b�`���Ă��܂��Ă���B
//   ��//��''      �u�\�v��2�o�C�g�ڂ��ʂɁu�\�v��2�o�C�g�ڂɃ}�b�`���Ă��܂��Ă���B
//   ��//��'��'    �u�\�v���́u�\�v���́u�\�v��2�o�C�g�ڂ�'\'�Ƀ}�b�`���Ă��܂��Ă���B
//   ��//������(�}���`�o�C�g�Ή���)�c������
//   ��//��''
//   ��//��'��\'
//   ��//��'��\'
//   ��//��'��'
#define USE_STRSPLIT_MBCS	//���̃V���{�����`����ƃ}���`�o�C�g�Ή��ł�strsplit()��strsplit_set()���g�p���܂��B���̃V���{�����`���Ȃ��ƃ}���`�o�C�g��Ή��ł�strsplit()��strsplit_set()���g�p���܂��B
//�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d
//���}���`�o�C�g��Ή��ł�strsplit()��strsplit_set()	2014/10�`2016/05�܂ŁA���肵�ē��삵�Ă����R�[�h�ł��B
#ifndef USE_STRSPLIT_MBCS
#ifndef PIECE
static char** strsplit_sub(const char* s, const char* delimiter,  int max_tokens, int delimiter_len);
       char** strsplit(    const char* s, const char* delimiter,  int max_tokens) { return strsplit_sub(s, delimiter,  max_tokens, 0); }
       char** strsplit_set(const char* s, const char* delimiters, int max_tokens) { return strsplit_sub(s, delimiters, max_tokens, 1); }
static char** strsplit_sub(const char* s, const char* delimiter,  int max_tokens, int delimiter_len) {
	int    argc = 0;
	char **argv = NULL, *p;
	if(*s) {
		char* (*fn)(const char*, const char*) = strpbrk; /* �f���~�^�����֐� */
		/* strsplit()    ����Ăяo���ꂽ���A(delimiter_len=0)�ł���B�����Ńf���~�^������̎��ۂ̕������𐔂���B(delimiter=""�͕s��)
		 * strsplit_set()����Ăяo���ꂽ���A(delimiter_len=1)�ł���B�X�̃f���~�^�͈ꕶ���ƌ��Ȃ��̂ł��̂܂܁B(delimiter=""���\) */
		if(!delimiter_len) {
			if(!(delimiter_len = strlen(delimiter))) { DIE(); } /* �ustrsplit(s,"",max_tokens)�v�͕s�B�ustrsplit_set(s,"",max_tokens)�v�͉\�B */
			fn = strstr; /* �f���~�^�����֐� */
		}
		/* �󕶎��񂪎w�肳�ꂽ�ꍇ�̌��ʂ́A�I�[��NULL�|�C���^�݂̂��琬���̕�����|�C���^�z��Ƃ���B
		 * - As a special case, the result of splitting the empty string "" is an empty vector, not a vector containing a single string.
		 *   The reason for this special case is that being able to represent a empty vector is typically more useful than consistent handling of empty elements.
		 *   If you do need to represent empty elements, you'll need to check for the empty string before calling g_strsplit(). */
		for(;;) {
			/* ������|�C���^�z����g������B */
			if(!(argv = realloc(argv, sizeof(char*) * (argc + 1)))) { DIE(); }
			/* �������̍ŏI�Ȃ�΁A�f���~�^��������Ȃ������̂Ɠ����Ƃ���B
			 * - The maximum number of pieces to split string into. If this is less than 1, the string is split completely. */
			if((max_tokens >= 1) && (--max_tokens == 0)) { break; }
			/* �f���~�^��������Ȃ���΁A������B */
			if(!(p = (*fn)(s, delimiter))) { break; }
			/* ���݈ʒu����f���~�^�̒��O�܂ł̕�������A�������ĕ�����|�C���^�z��Ɋi�[����B
			 * - ������̐擪�Ƀf���~�^���L�����ꍇ�A�ŏ��Ɋi�[����v�f�͋󕶎���ƂȂ�B */
			if(!(argv[argc++] = strndup(s, p - s))) { DIE(); }
			/* ���݈ʒu���A�f���~�^�̒���֐i�߂�B */
			s = p + delimiter_len;
		}
		/* ���݈ʒu���當����̏I�[�܂ł̕�������A�������ĕ�����|�C���^�z��Ɋi�[����B
		 * - ������̖����Ƀf���~�^���L�����ꍇ�A�Ō�Ɋi�[����v�f�͋󕶎���ƂȂ�B */
		if(!(argv[argc++] = strdup(s))) { DIE(); }
	}
	/* ������|�C���^�z��̏I�[�Ƃ��āANULL�|�C���^���i�[����B */
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
static void __attribute__((noreturn,unused))/*asm�u���b�N����Q��*/ strsplit_sub_DIE() { DIE(); }
#endif/*PIECE*/
//�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d
#else //USE_STRSPLIT_MBCS
//�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d
//���}���`�o�C�g�Ή��ł�strsplit()��strsplit_set()	2016/05�`�g�p�J�n����R�[�h�ł��B
#ifndef PIECE
char** strsplit(const char* s, const char* delimiter,  int max_tokens) {
	char **v, *p, *t;
	if(!*delimiter) { DIE(); }	//�ustrsplit(s,"",max_tokens)�v�͕s�B
	v = strv_new(NULL);
	if(*s) {
		while(--max_tokens) {	//�����ɂ́uwhile((max_tokens<1)||(--max_tokens)){�`�v�����A�g�[�N������(2^31)�ȏ�ɂȂ鎖�͂܂������̂ō��L�̂悤�Ɋȗ������Ė�薳�����낤�B
			if(!(p = mbsstr(s, delimiter))) { break; }	//��strsplit()��strsplit_set()�ƂňقȂ�̂͂��̍s�ł��B�A�Z���u���łł�(fn1)�ɑ������܂��B
			if(!(t = strndup(s, p - s))) { DIE(); }
			strv_push(&v, t);
			s = p + strlen(delimiter);			//��strsplit()��strsplit_set()�ƂňقȂ�̂͂��̍s�ł��B�A�Z���u���łł�(fn2)�ɑ������܂��B
		}
		strv_extend(&v, s);
	}
	return v;
}
char** strsplit_set(const char* s, const char* delimiters, int max_tokens) {
	char **v, *p, *t;
//�s�v	if(!*delimiters) { DIE(); }	//�ustrsplit_set(s,"",max_tokens)�v�͉\�B
	v = strv_new(NULL);
	if(*s) {
		while(--max_tokens) {	//�����ɂ́uwhile((max_tokens<1)||(--max_tokens)){�`�v�����A�g�[�N������(2^31)�ȏ�ɂȂ鎖�͂܂������̂ō��L�̂悤�Ɋȗ������Ė�薳�����낤�B
			if(!(p = mbspbrk(s, delimiters))) { break; }	//��strsplit()��strsplit_set()�ƂňقȂ�̂͂��̍s�ł��B�A�Z���u���łł�(fn1)�ɑ������܂��B
			if(!(t = strndup(s, p - s))) { DIE(); }
			strv_push(&v, t);
			s = mbsinc(p);					//��strsplit()��strsplit_set()�ƂňقȂ�̂͂��̍s�ł��B�A�Z���u���łł�(fn2)�ɑ������܂��B
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
	;//	ld.w		%r2, %r14			;//%r2     := max_tokens	����������
		xld.w		[%sp+0], %r8			;//[%sp+0] :=              NULL		��
		xcall.d		strv_new			;//%r10    := v = strv_new(NULL)	��
		ld.w		%r2, %r14			;//%r2     := max_tokens	����������	*delay*
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
		ext		mbsstr@rm			;//xjp mbsstr	��(fn1)��2���ߌŒ�
		jp		mbsstr@rl			;//		��
;//strsplit_fn2:
		xcall.d		strlen				;//%r10    :=     strlen(delimiter)
		ld.w		%r12, %r1			;//%r12    :=            delimiter		*delay*
		ret.d
		add		%r10, %r3			;//%r10    := p + strlen(delimiter)		*delay*
		;//- - - - - - - - - - - - - - - - - - - - - - -;//
strsplit_set_vtbl:
;//strsplit_set_fn1:
		ext		mbspbrk@rm			;//xjp mbspbrk	��(fn1)��2���ߌŒ�
		jp		mbspbrk@rl			;//		��
;//strsplit_set_fn2:
		xjp.d		mbsinc				;//%r10    := mbsinc(p)
		ld.w		%r12, %r3			;//%r12    :=        p				*delay*
");
static void __attribute__((noreturn,unused))/*asm�u���b�N����Q��*/ strsplit_DIE() { DIE(); }
#endif//PIECE
#endif//USE_STRSPLIT_MBCS
//������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������

//������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������
// * Sun May 22 21:58:55 JST 2016 Naoyuki Sawa
// - textwrap()�̎�����ύX���܂����B���ʂ͕ς��܂���B
//   �ύX�_�͈ȉ��̒ʂ�ł��B
// - �ύX�O�́Atextwrap()�̒��ŕ�����z��̃������m�ۂ��s���Ă��܂����B
//   �ύX��́Astrv.c���W���[���̋@�\���g���ĕ�����z��̃������m�ۂ��s���悤�ɂ��܂����B
//   ���̕ύX�ɂ���āA�֐��̎������P���ɂȂ�A�R�[�h�T�C�Y����44�o�C�g�������Ȃ�܂����B
//   strv.o�������N���邽�߂̃R�[�h�T�C�Y�����͗L��܂����A����strstrip()��strv.o���g�p���Ă���̂ŁA�V���ȑ����͗L��܂���B
// - �ύX�O�̃R�[�h���A����܂ň��肵��(�c�Ƃ������p�ɂɂ͎g���Ă��܂���ł������c)���삵�Ă����̂ŁA#if 0�`�Ő؂蕪���Ďc���Ă���܂��B
//   �ύX��̃R�[�h�ɂ́A�ύX�O�̃R�[�h�قǏڍׂȃR�����g�͕t���Ă��Ȃ��̂ŁA�A���S���Y���ɂ��Ă͕ύX�O�̃R�[�h�̃R�����g���Q�Ƃ��ĉ������B
//   �ύX��̃R�[�h���A�ύX�O�̃R�[�h�Ɠ����A���S���Y���ŁA������z��̃��������m�ۂ�����@���ς���������ł��B
//�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d
//���ύX�O�̃R�[�h�B������z��̃������m�ۂ��֐����ōs���܂��Bstrv.c���W���[���Ɉˑ����܂��񂪁A���֐��̃R�[�h�T�C�Y�͏����傫���ł��B	2015/03�`2016/05�܂œ��삵�Ă����R�[�h�ł��B
#if 0
#ifndef PIECE
char** textwrap(const char* s, int w) {
	char** argv = NULL;
	int    argc = 0, n = 0;
	/* �����񂪎c���Ă���ԌJ��Ԃ��B */
	while(*s) {
		/* ���݂̕������I�[�����Ŗ�����΁c */
		if(s[n + 0]) {
			/* ���݂̕����̔��p(c=1)/�S�p(c=2)�𔻒肷��B */
			int c = 1;
			if(ismbblead(s[n + 0]) && ismbbtrail(s[n + 1])) {
				c = 2;
			}
			/* ���݂̕������܂߂����݂̕�����̒��������߂�B */
			n += c;
			/* ���݂̕��������݂̕������1�����ڂ�,����,�s���ȉ��Ȃ�Όp������B
			 * �O�҂̏�����(w��1)���w�肳��Ă��������[�v�Ɋׂ�Ȃ��΍�ł��B */
			if((n == c) || (n <= w)) { continue; }
			/* �s���𒴂����猻�݂̕������܂߂Ȃ��Ō��݂̕������؂�o���B */
			n -= c;
		}
		/* ������|�C���^�z����g����������𕡐����Ċi�[����B
		 * �ǂ��炩�����s�����烁�����s���Ȃ̂ŃG���[��~����B */
		if(!(argv = realloc(argv, sizeof(char*) * (argc + 1))) ||
		   !(argv[argc++] = strndup(s, n))) { DIE(); }
		/* �؂�o������������΂�������̒��������Z�b�g����B */
		s += n, n = 0;
	}
	/* ������|�C���^�z��̏I�[�Ƃ���NULL�|�C���^���i�[����B */
	if(!(argv = realloc(argv, sizeof(char*) * (argc + 1)))) { DIE(); }
	argv[argc] = NULL;
	return argv;
}
#else //PIECE
//�A�Z���u���������ړI�́A�R�[�h�T�C�Y�̒ጸ�ł��B
//�R�[�h�T�C�Y���A(C�����=200�o�C�g)��(Asm��=156�o�C�g)�ɏ������Ȃ�܂����B
//�����񕪊����s���v���O�����͑��x���d�v�łȂ��Ǝv���̂ŁA�d�����Ă��܂���B
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
		cmp		%r3, %r4		;//    if(n == c) { goto LOOP }					s�͕ω����Ă��Ȃ��̂�(*s)�̌����͕s�v�ł���LOOP�֖߂��ėǂ��BC����ł�continue���g���̂�while(*s)�ɖ߂��Ă��܂������ۂ̂Ƃ���(*s)�̌����͖��ʂł���B
		jreq		textwrap_LOOP		;//    
		xld.w		%r5, [%sp+0]		;//    %r5  := w
		cmp		%r3, %r5		;//    if(n <= w) { goto LOOP }			!INTERLOCK!	s�͕ω����Ă��Ȃ��̂�(*s)�̌����͕s�v�ł���LOOP�֖߂��ėǂ��BC����ł�continue���g���̂�while(*s)�ɖ߂��Ă��܂������ۂ̂Ƃ���(*s)�̌����͖��ʂł���B
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
		ld.w		%r3, 0			;//  %r3  :=       n = 0			*delay*		�����'n=0'�ƕ������؂�o�������'n=0'�����˂�B
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
static void __attribute__((noreturn,unused))/*asm�u���b�N����Q��*/ textwrap_DIE() { DIE(); }
#endif//PIECE
//�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d
#else
//�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d�d
//���ύX��̃R�[�h�Bstrv.c���W���[���̋@�\���g���ĕ�����z��̃������m�ۂ��s���܂��B�֐��̎������P���ɂȂ�R�[�h�T�C�Y���������Ȃ�܂����B	2016/05�`�g�p�J�n����R�[�h�ł��B
#ifndef PIECE
char** textwrap(const char* s, int w) {
	char** v = strv_new(NULL);
	while(*s) {
		int n = 0, c;
		while((c = mbsnextc(&s[n]))) {
			c = (c >> 8) ? 2 : 1;
			n += c;
			if((n != c) && (n > w)) { break; }	//�uif((n != c) && (n >= w))�`�v�Ƃ��Ă͂����܂���B���̉��Łun -= c�v���s���̂ŁA���������I�[�o�[���Ă��烋�[�v�𔲂���K�v���L��܂��B
		}
		n -= c;						//�uwhile((c = mbsnextc(&s[n]))) {�v�Ŕ������ꍇ�́A(c==0)�Ȃ̂Ń_�~�[�����ƂȂ�܂��B
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
	;//	ld.w		%r1, %r13		;//%r1     := w		������������������
		xld.w		[%sp+0], %r8		;//[%sp+0] :=              NULL		��
		xcall.d		strv_new		;//%r10    :=     strv_new(NULL)	��
		ld.w		%r1, %r13		;//%r1     := w		������������������	*delay*
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
static void __attribute__((noreturn,unused))/*asm�u���b�N����Q��*/ textwrap_DIE() { DIE(); }
#endif//PIECE
#endif
//������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������

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
void strdup_splitpath(const char* path, char** drive/*NULL��*/, char** dir/*NULL��*/, char** fname/*NULL��*/, char** ext/*NULL��*/) {
	char _drive[MAX_DRIVE], _dir[MAX_DIR], _fname[MAX_FNAME], _ext[MAX_EXT];
	splitpath(path, _drive, _dir, _fname, _ext);
	if(drive) { if(!(*drive = strdup(_drive))) { DIE(); } }
	if(dir  ) { if(!(*dir   = strdup(_dir  ))) { DIE(); } }
	if(fname) { if(!(*fname = strdup(_fname))) { DIE(); } }
	if(ext  ) { if(!(*ext   = strdup(_ext  ))) { DIE(); } }
}
char* strdup_makepath(const char* drive/*NULL��*/, const char* dir/*NULL��*/, const char* fname/*NULL��*/, const char* ext/*NULL��*/) {
	char _path[MAX_PATH], *path;
	makepath(_path, drive, dir, fname, ext);
	if(!(path = strdup(_path))) { DIE(); }
	return path;
}

/*****************************************************************************
 *	��r
 *****************************************************************************/

#ifndef PIECE
#if 0
//�f���Ɏ��������o�[�W�����B
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
//���ʏ������܂Ƃ߂ėe�ʐߖ񂵂��o�[�W�����B
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
//���ʏ������܂Ƃ߂ėe�ʐߖ񂵂��o�[�W���������ɁA�A�Z���u�������܂����B
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
		jrult		str_has_prefix_suffix_L10	;//if((unsigned)n1<(unsigned)n2) { return 0 }					��
		and		%r3, %r2			;//%r3  :=                        (n1-n2)&mask					��
		or		%r10, %r2			;//%r10 :=                                            n2|mask			��
		ld.w		%r12, %r0			;//%r12 :=                  s1							��
		add		%r12, %r3			;//%r12 :=                  s1 + ((n1-n2)&mask)					��
		ld.w		%r13, %r1			;//%r13 :=                                       s2				��
		xcall.d		strncmp				;//%r10 := retval = strncmp(s1 + ((n1-n2)&mask), s2, (n2|mask))			��
		ld.w		%r14, %r10			;//%r14 :=                                            n2|mask			��	*delay*
		cmp		%r8, %r10			;//%psr(C) := retval  ?  1 : 0							��
str_has_prefix_suffix_L10:					;//			���@��							��
		ld.w		%r10, 1				;//%r10    := 1		������������������					��
		popn		%r3				;//					��					��
		ret.d						;//					��					��
		sbc		%r10, %r8			;//%r10    := %psr(C) ?  0 : 1	��������������������������������������������������	*delay*
");
#endif/*PIECE*/

#ifndef PIECE
int string_match(const char* pat, const char* str) {
	int p, q, s;
	for(;;) {
		//�p�^�[������ꕶ���擾���A���̈ʒu�֐i�߂�B
		p = *pat++;
		//�����񂩂�ꕶ���擾���A���̈ʒu�֐i�߂�B
		s = *str++;
		//�p�^�[�����I�[�������Ȃ�΁c
		if(!p) {
			//�����񂪏I�[�������Ȃ�΁A�}�b�`�����ƌ��肷��B
			//�����񂪏I�[�łȂ������Ȃ�΁A�}�b�`���Ȃ������ƌ��肷��B
			return !s;
		}
		//�����񂪏I�[�������Ȃ�΁c
		if(!s) {
			//�p�^�[���̕�����'*'�ȊO�������Ȃ�΁A�}�b�`���Ȃ������ƌ��肷��B
			if(p != '*') { return 0; }
			//�p�^�[���̕�����'*'�������Ȃ�΁A�ȉ��̃u���b�N�ɂď������s���B
		}								//��
		//�p�^�[���̕�����'*'�������Ȃ�΁c				//��
		if(p == '*') {							//��
			//�p�^�[�����I�[�Ȃ�΁A�}�b�`�����ƌ��肷��B		//��
			if(!*pat) { return 1; }					//��
			//������̈ʒu���A����s�̈ʒu�֖߂��B����s��'\0'�ł��邱�Ƃ��L�蓾��B
			str--;
			for(;;) {
				//���݈ʒu�ȍ~�̕�����ƃp�^�[�����}�b�`������A�}�b�`�����ƌ��肷��B
				if(string_match(pat, str)) { return 1; }
				//'*'������ł��镶�����c���Ă��Ȃ���΁A�}�b�`���Ȃ������ƌ��肷��B
				if(!*str++) { return 0; }
				//'*'���ꕶ��������ƌ��Ȃ��āA��r���p������B
			}
		}
		//�p�^�[���̕�����'?'�������Ȃ�΁A�ꕶ����v�����ƌ��Ȃ��āA���̕����̔�r�����ֈڍs����B
		if(p == '?') { continue; }
		//�p�^�[���̕�����'['�������Ȃ�΁c
		if(p == '[') {
			int match = 0;
			for(;;) {
				//�p�^�[������ꕶ���擾���A���̈ʒu�֐i�߂�B
				p = q = *pat++;
				//�p�^�[�����I�[�������Ȃ�΁A���Ă��Ȃ������N���X�ł���A�s���ȃp�^�[���ł���B
				if(!p) { DIE(); }
				//�p�^�[���̕�����']'�������Ȃ�΁A���[�v�𔲂���B
				//��Tcl6.7��string match�ƁA��ʓI�Ȑ��K�\���̑���_�Ƃ��āA�ȉ��ɒ��ӂ���:
				//����ʓI�Ȑ��K�\���ł́A'['�̒����']'�͕����N���X������A�����N���X�̈ꕶ���ƌ��Ȃ���邪�ATcl6.7��string match�ɂ͂��̓��Ꮘ���������B
				//����ʓI�Ȑ��K�\���ł́A�����N���X�̒��ł�'\'�ɂ����ꕶ���̃G�X�P�[�v��G�X�P�[�v�V�[�P���X���g���邪�ATcl6.7��string match�ł͎g���Ȃ��B
				if(p == ']') { break; }
				//�p�^�[���̕�����'-'�Ȃ�΁c
				if(*pat == '-') {
					//�p�^�[���̈ʒu���A'-'�̎��̈ʒu�֐i�߂�B
					pat++;
					//�p�^�[������ꕶ���擾���A���̈ʒu�֐i�߂�B
					q = *pat++;
					//�p�^�[�����I�[�������Ȃ�΁A���Ă��Ȃ������N���X�ł���A�s���ȃp�^�[���ł���B
					if(!q) { DIE(); }
				}
				//�����������N���X�͈͓̔��Ȃ�΁A�����������N���X�Ɉ�v�������Ƃ��}�[�N����B
				//�����N���X�̎c���ǂݎ̂Ă邽�߂ɁA���[�v���p������B
				if((s >= p) && (s <= q)) { match = 1; }
				if((s >= q) && (s <= p)) { match = 1; }	//�t�͈͂͐��K�\���̎d�l�O�����ATcl6.7�̎����ł͋��e���Ă���B
			}
			//�����������N���X�Ɉ�v���Ă��Ȃ���΁A�}�b�`���Ȃ������ƌ��肷��B
			if(!match) { return 0; }
			//�ꕶ����v�����ƌ��Ȃ��āA���̕����̔�r�����ֈڍs����B
			continue;
		}
		//�p�^�[���̕�����'\'�������Ȃ�΁c
		if(p == '\\') {
			//�p�^�[������ꕶ���擾���A���̈ʒu�֐i�߂�B
			p = *pat++;
			//�p�^�[�����I�[�������Ȃ�΁A�s���ȃG�X�P�[�v�ł���A�s���ȃp�^�[���ł���B
			if(!p) { DIE(); }
		}
		//�p�^�[���̕����ƕ�����̕�������v���Ȃ���΁A�}�b�`���Ȃ������ƌ��肷��B
		if(p != s) { return 0; }
	}
}
#else //PIECE
//�A�Z���u���������ړI�́A�R�[�h�T�C�Y�̒ጸ�ł��B
//�R�[�h�T�C�Y���A(C�����=224�o�C�g)��(Asm��=172�o�C�g)�ɏ������Ȃ�܂����B
//�������r���s���v���O�����͑��x���d�v�łȂ��Ǝv���̂ŁA�d�����Ă��܂���B
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
		 cmp		%r5, 0				;//  %psr(Z) := !s			����	*delay*
		 jreq		string_match_RET_1		;//    return !s			����
		 jp		string_match_RET_0		;//  }					�@��
		jrne.d		3				;//  if(!s) {				����
		 sub		%r4, 42				;//  %psr(Z) := (p == '*')		����	*delay*		�����ucmp %r4,42�v�́ucmp %rd,sign6�v�͈̔͊O�Ȃ̂ŕs�B
		 jrne		string_match_RET_0		;//    if(p != '*') { return 0 } }	����			�@���͈͊O�ł���ɂ�������炸�A�Z���u���G���[���o�Ȃ��̂Œ��ӂ���B
		jrne.d		string_match_L20		;//  if(p == '*') {			����			�@���uxcmp %r4,42�v�ɂ����2�����ɏ����K�v�������邪�A�usub %r4,42�v�Ŕ��f���āuadd %r4,42�v�Ŗ߂���1�����ōςށB
		add		%r4, 42				;//  						*delay*		���������ł́usub %r4,42�v�`�uadd %r4,42�v�̊Ԃ�%r4�̒l���ω����Ă���薳�����Ƃ𗘗p�����H�v�ł���B
		ld.b		%r4, [%r12]			;//    %r4  := p = *pat
		cmp		%r4, 0				;//    if(!p) { return 1 }
		jreq		string_match_RET_1
		sub		%r13, 1				;//    %r13 := str--
string_match_L10:						;//    for(;;) {
	;//{{�ǂ��炩�����I�����Ă��������B
	;//���X�^�b�N����ʂ����Ȃ��o�[�W����
	;//	xsub		%sp, %sp, 8
	;//	xld.w		[%sp+0], %r12			;//      [%sp+0] := pat
	;//	xld.w		[%sp+4], %r13			;//      [%sp+4] := str
	;//	call		string_match			;//      %r10 := retval = string_match(pat,str)
	;//	xld.w		%r12, [%sp+0]			;//      %r12 := pat
	;//	xld.w		%r13, [%sp+4]			;//      %r13 := str
	;//	xadd		%sp, %sp, 8
	;//	cmp		%r10, 0				;//      if(retval) { return 1 }
	;//||�ǂ��炩�����I�����Ă��������B
	;//���X�^�b�N����ʂ�������4���ߐߖ񂵂��o�[�W����
		pushn		%r13
		call		string_match			;//      %r10 := retval = string_match(pat,str)
		cmp		%r10, 0				;//      if(retval) { return 1 }
		popn		%r13
	;//}}�ǂ��炩�����I�����Ă��������B
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
		 xor		%r4, %r7			;//        %r4  := p ^= q	��
		 xor		%r7, %r4			;//        %r7  := q ^= p	��p��q������
		 xor		%r4, %r7			;//        %r4  := p ^= q }	��
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
		xcmp		%r4, 92				;//  if(p == '��') {
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
static void __attribute__((noreturn,unused))/*asm�u���b�N����Q��*/ string_match_DIE() { DIE(); }
#endif//PIECE

#ifndef PIECE
static int strnatcmp_subr(const unsigned char* s1, const unsigned char* s2, int (*conv)(int)) {
	int c1, c2;
	//�s���̋󔒂�ǂݔ�΂��B
	// - �s���̋󔒂͔�r�ΏۂɊ܂߂܂���B
	while(isspace(c1 = *s1++)) { /** no job **/ }
	while(isspace(c2 = *s2++)) { /** no job **/ }
	for(;;) {
		//�����������Ȃ�΁c
		if(isdigit(c1) && isdigit(c2)) {
			//���Ȃ��Ƃ�������������Ȃ�΁c
			// - '0'����n�܂鐔����͈Öق�'.'����s���Ă���ƌ��Ȃ��B
			if((c1 == '0') || (c2 == '0')) {
				//Leading zeros are not ignored, which tends to give more reasonable results on decimal fractions.
				//1.001 < 1.002 < 1.010 < 1.02 < 1.1 < 1.3
				for(;;) {
					if(isdigit(c1)) {
						//�����Ƃ������̏ꍇ�c
						if(isdigit(c2)) {
							if((c1 -= c2)) { return c1; }
						//���̌����̕��������ꍇ�c
						} else {
							return  1;
						}
					} else {
						//�E�̌����̕��������ꍇ�c
						if(isdigit(c2)) {
							return -1;
						//�����Ƃ������łȂ��ꍇ�c
						} else {
							break;	//�A�������󔒕�������̃X�y�[�X(' ')�ɒu�������鏈���ֈڍs����B
						}
					}
					//���̕������擾����B
					c1 = *s1++;
					c2 = *s2++;
				}
			//�����Ƃ��������Ȃ�΁c
			} else {
				int bias = 0;
				for(;;) {
					if(isdigit(c1)) {
						//�����Ƃ������̏ꍇ�c
						if(isdigit(c2)) {
							if(!bias) { bias = c1 - c2; }
						//���̌����̕��������ꍇ�c
						} else {
							return  1;
						}
					} else {
						//�E�̌����̕��������ꍇ�c
						if(isdigit(c2)) {
							return -1;
						//�����Ƃ������łȂ��ꍇ�c
						} else {
							if(bias) { return bias; }
							break;	//�A�������󔒕�������̃X�y�[�X(' ')�ɒu�������鏈���ֈڍs����B
						}
					}
					//���̕������擾����B
					c1 = *s1++;
					c2 = *s2++;
				}
			}
		//���Ȃ��Ƃ�����������łȂ���΁c
		} else {
			//�Е��̕����񂪏I�[�Ȃ�΁A�����Е��̕�����̋󔒕�����ǂݔ�΂��B
			// - ���̌��ʁA�����Е��̕�������I�[�ɒB������A��v�ƌ��Ȃ����߂ł��B
			//   �s���̋󔒂͔�r�ΏۂɊ܂߂܂���B
			if(!c1) { while(isspace(c2)) { c2 = *s2++; } }
			if(!c2) { while(isspace(c1)) { c1 = *s1++; } }
			//�K�v�ɉ����đ啶���ɕϊ�����B
			c1 = (*conv)(c1);
			c2 = (*conv)(c2);
			//c1,c2��(unsigned char*)����ǂݏo�����l�Ȃ̂ŁAOverflow�̐S�z�͖������Z�Ŕ�r�o����B
			c1 -= c2;
			//c1,c2���قȂ邩,����,�������I�[�ɒB������A��r���ʂ�Ԃ��B
			// - ��҂̏������f�́A�O�҂̏������f���ے肳�ꂽ�̌�Ȃ̂ŁA�Е���������������Ηǂ��B
			if(c1 || !c2) { return c1; }
			//���̕������擾����B
			c1 = *s1++;
			c2 = *s2++;
		}
		//�P��Ԃ̘A�������󔒕�������̃X�y�[�X(' ')�ɒu��������B
		// - �P��Ԃ̋󔒕����͈�̃X�y�[�X(' ')�ƌ��Ȃ��Ĕ�r���܂��B
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
		;//�s���̋󔒂�ǂݔ�΂��B
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
		;//�����������Ȃ�΁c				;//    
		;//�����Ƃ��������Ȃ�΁c			;//    
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
		;//���Ȃ��Ƃ�������������Ȃ�΁c		;//    } else {
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
		;//���Ȃ��Ƃ�����������łȂ���΁c		;//    
		;//�Е��̕����񂪏I�[�Ȃ�΁A�����Е��̕�����̋󔒕�����ǂݔ�΂��B
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
		;//�K�v�ɉ����đ啶���ɕϊ�����B		;//    
		xld.w		%r9, [%sp+0]			;//    %r9  :=        conv
		call.d		%r9				;//    %r10 := c2 = (*conv)(c2)
		ld.w		%r12, %r3			;//    %r12 :=              c2										*delay*
		ld.w		%r3, %r10			;//    %r3  := c2
		xld.w		%r9, [%sp+0]			;//    %r9  :=        conv
		call.d		%r9				;//    %r10 := c1 = (*conv)(c1)
		ld.w		%r12, %r2			;//    %r12 :=              c1										*delay*
		;//c1,c2���قȂ邩,����,�������I�[�ɒB������A��r���ʂ�Ԃ��B
		sub		%r10, %r3			;//    %r10 := c1 -= c2
		jrne		strnatcmp_subr_RET		;//    if( c1) { return c1 }
		cmp		%r3, 0				;//    if(!c2) { return c1 }
		jreq		strnatcmp_subr_RET		;//    
		;//���̕������擾����B				;//    
		ld.ub		%r2, [%r0]+			;//    %r2  := c1 = *s1++
		ld.ub		%r3, [%r1]+			;//    %r3  := c2 = *s2++
strnatcmp_subr_LOOP3_NEXT:					;//  }
		;//�A�������󔒕�������̃X�y�[�X(' ')�ɒu��������B
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
		adc		%r4, %r4			;//%r4[0]  := isdigit(c1),                         %r4[31:1] := (�s��l)
		sub		%r3, 48				;//%r3     := c2 -= '0'
		cmp		%r3, 10				;//%psr(C) := (c2 <= 9)
		adc		%r4, %r4			;//%r4[0]  := isdigit(c2), %r4[1]  := isdigit(c1), %r4[31:2] := (�s��l)
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
//���e�X�g�X�C�[�g
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
//		assert(strnatcmp("000", "00") > 0);	//������strverscmp()�ƈႢ�܂��Bstrnatcmp()�̕������ςɋ߂��ł��B
//		assert(strnatcmp("00", "000") < 0);	//������strverscmp()�ƈႢ�܂��Bstrnatcmp()�̕������ςɋ߂��ł��B
//		assert(strnatcmp("a0", "a") > 0);
//		assert(strnatcmp("00", "01") < 0);
//		assert(strnatcmp("01", "010") < 0);
//		assert(strnatcmp("010", "09") < 0);
//		assert(strnatcmp("09", "0") > 0);	//������strverscmp()�ƈႢ�܂��Bstrnatcmp()�̕������ςɋ߂��ł��B
//		assert(strnatcmp("9", "10") < 0);
//		assert(strnatcmp("0a", "0") > 0);
//	}

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
#ifndef PIECE
char* strreverse(char* s) {
	char* p1 = s;
	char* p2 = strchr(s, '\0');
//{{2016/09/30�ǉ�:strreverse()��C����ł̎����ŁA'goto L_START'���ϐ�'c'�̐錾���܂����ł��������C�����܂����B
	int c;
//}}2016/09/30�ǉ�:strreverse()��C����ł̎����ŁA'goto L_START'���ϐ�'c'�̐錾���܂����ł��������C�����܂����B
	goto L_START;
	do {
//{{2016/09/30�ύX:strreverse()��C����ł̎����ŁA'goto L_START'���ϐ�'c'�̐錾���܂����ł��������C�����܂����B
//		int c = *p1;
//��2016/09/30�ύX:strreverse()��C����ł̎����ŁA'goto L_START'���ϐ�'c'�̐錾���܂����ł��������C�����܂����B
		    c = *p1;
//}}2016/09/30�ύX:strreverse()��C����ł̎����ŁA'goto L_START'���ϐ�'c'�̐錾���܂����ł��������C�����܂����B
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
		xcall.d		strchr			;//%r10 := p2 = strchr(s, '���O')
		ld.w		%r13, 0			;//%r13 :=                '���O'	*delay*
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
 *	�P�ꉻ
 *****************************************************************************/

// * Wed Sep 07 21:52:10 JST 2016 Naoyuki Sawa
// - quark_from_string(),quark_to_string()���A32�r�b�g�ł�64�r�b�g�łƂŁA�ʂ̎����ɂ��܂����B
//   32�r�b�g�ł͂���܂Œʂ�A�P�ꉻ�\���̕�����|�C���^�����̂܂�(int)�Ƃ��ĕԂ��Ηǂ��̂ł����A
//   64�r�b�g�ł̓|�C���^��64�r�b�g�̂��߁A������|�C���^�����̂܂�(int)�Ƃ��ĕԂ��Ȃ��Ȃ�������ł��B
//   64�r�b�g�ł́A�P�ꉻ�\���̕�����|�C���^��(int)�𑊌ݕϊ����邽�߂́A�V���Ȏ�����݂��鎖�ɂ��܂����B
#ifndef _WIN64
//32�r�b�g��
int quark_from_string(const char* s) {
	return (int)intern_string(s);
}
const char* quark_to_string(int q) {
	return (const char*)q;
}
#else //_WIN64
//64�r�b�g��
static ght_hash_table_t* ht_quark_from_string;	//Key=�P�ꉻ�\���̕�����|�C���^(const char*),Data=������̒P�ꉻ�\���̐��l(int)	������̓o�^����quark_from_string()�ɂč쐬����B�Е��������쐬����鎖�͖����B
static ght_hash_table_t* ht_quark_to_string;	//Key=������̒P�ꉻ�\���̐��l(int),Data=�P�ꉻ�\���̕�����|�C���^(const char*)	��
int quark_from_string(const char* s) {
	//NULL�|�C���^�ɑ΂��Ă͐��l0��Ԃ��d�l�ł��B
	int q = 0, n;
	if(s) {	//NULL�|�C���^�ȊO�Ȃ�΁c
ENTER_CS;
		//�n�b�V���e�[�u�����쐬����Ă��Ȃ���΁c
		if(!ht_quark_from_string) {
			//�n�b�V���e�[�u�����쐬����B
			ght_set_rehash((ht_quark_from_string = ght_create(0)), 1);
			ght_set_rehash((ht_quark_to_string   = ght_create(0)), 1);
		}
		//�P�ꉻ�\���̕�����|�C���^���擾����B
		s = intern_string(s);
		//������̒������擾����B�L�[�̖����Ƀk�������͕s�v�Ȃ̂ŁA(+1)�͕s�v�ł��B
		n = strlen(s);
		//���ɓo�^����Ă�����A�P�ꉻ�\���̐��l���擾����B
		q = (intptr_t)ght_get(ht_quark_from_string, n, s);
		//�܂��o�^����Ă��Ȃ���΁c
		if(!q) {
			//�P�ꉻ�\���̐��l���쐬����B�o�^����1,2,3,...�Ƃ��鎖�ɂ����B(�K�{�ł͂���܂���B)
			q = ght_size(ht_quark_from_string) + 1;
			//�n�b�V���e�[�u���ɓo�^����B
			if(ght_insert(ht_quark_from_string, (void*)(intptr_t)q, n, s)) { DIE(); }	//Key=�P�ꉻ�\���̕�����|�C���^(const char*),Data=������̒P�ꉻ�\���̐��l(int)
			if(ght_insert(ht_quark_to_string, (char*)s, sizeof q, &q)) { DIE(); }		//Key=������̒P�ꉻ�\���̐��l(int),Data=�P�ꉻ�\���̕�����|�C���^(const char*)
		}	//                                ~~~~~~~���̃L���X�g�́Aconst�x���}���̂��߂ł��B
LEAVE_CS;
	}
	return q;
}
const char* quark_to_string(int q) {
	//���l0�ɑ΂��Ă�NULL�|�C���^��Ԃ��d�l�ł��B
	const char* s = NULL;
	if(q) {	//���l0�ȊO�Ȃ�΁c
ENTER_CS;
		//�n�b�V���e�[�u�����쐬����Ă��Ȃ���΁c
		if(!ht_quark_to_string) { DIE(); }	//�P�ꉻ�\���̐��l���o�^����Ă��Ȃ���ԂŁA���l0�ȊO�ɑ΂���quark_to_string()���Ăяo���Ă͂����Ȃ��B�����Œ�~�����ꍇ�͌Ăяo�����̃o�O�ł��B
		//�P�ꉻ�\���̕�����|�C���^���擾����B
		s = ght_get(ht_quark_to_string, sizeof q, &q);
		if(!s) { DIE(); }	//���o�^�̒P�ꉻ�\���̐��l�ɑ΂���quark_to_string()���Ăяo���Ă͂����Ȃ��B�����Œ�~�����ꍇ�͌Ăяo�����̃o�O�ł��B
LEAVE_CS;
	}
	return s;
}
#endif//_WIN64
/*---------------------------------------------------------------------------*/
const char* intern_string(const char* s) {
//{{2016/05/06�ύX:intern_string()���A�L�[�ƒl���������������w���悤�ɂ��āA�������̖��ʂ�ጸ���܂����B
//	//�d�����Ȃ��������o�^����n�b�V���e�[�u���B
//	// - �L�[�ƒl�ɓ��������񂪊i�[����Ă��胁�����̖��ʂ����A���݂͏����̒P������D�悵�Ă��̂܂܂Ƃ��Ă���B	//��
//	//   �������P����ɂ́A�V�K�������o�^������Ƀn�b�V���e�[�u���𑖍����ăG���g���̃L�[�̃A�h���X���擾���A	//����{{2016/05/06�ǋL:���L�̖����������܂����B�悭�l������n�b�V���e�[�u���𑖍����Ȃ��Ă��A�V�����G���g�����C�e���[�V�����p�̃��X�g�̖����ɒǉ����������𗘗p����Ηǂ����ɋC�t��������ł��B�ڍׂ́A�ύX��̎������Q�Ƃ��ĉ������B
//	//   �f�[�^�|�C���^���L�[�̃A�h���X���w���悤��ght_replace()�ŏ���������Ή\�ł͂���B(���Ȃ�x���Ȃ邪)	//��
//	//   ������������ꍇ�́A�L�[�̕�����Ƀk���������܂߂�K�v���L��̂ŁA�L�[�̒�����(+1)����悤�ɒ��ӂ���B	//��
//	static ght_hash_table_t* ht;
//	//�n�b�V���e�[�u�������쐬�Ȃ�΁c
//	if(!ht) {
//		//�n�b�V���e�[�u�����쐬����B
//		ht = ght_create(0);	//�����T�C�Y�𖳎w��Ƃ���B
//		ght_set_rehash(ht, 1);	//�������T�C�Y��L���ɂ���B
//	}
//	//������|�C���^��NULL�̏ꍇ�́A���̂܂�NULL��Ԃ��B�����łȂ���΁c
//	if(s) {
//		//������̒������擾����B�L�[�̖����Ƀk�������͕s�v�Ȃ̂ŁA(+1)�͕s�v�ł��B
//		int n = strlen(s);
//		//�n�b�V���e�[�u�����猟������B
//		char* t = ght_get(ht, n, s);
//		//�o�^����Ă�����A���̃f�[�^��Ԃ��B�o�^����Ă��Ȃ���΁c
//		if(!t) {
//			//������𕡐�����B
//			t = strdup(s);
//			//����������������A�����񎩐g���L�[�Ƃ��ēo�^����B
//			if(ght_insert(ht, t, n, t)) { DIE(); }	//�o�O
//		}
//		//�n�b�V���e�[�u�����ɕێ�����������̃f�[�^��߂�l�Ƃ���B
//		s = t;
//	}
//	return s;
//��2016/05/06�ύX:intern_string()���A�L�[�ƒl���������������w���悤�ɂ��āA�������̖��ʂ�ጸ���܂����B
	//�d�����Ȃ��������o�^����n�b�V���e�[�u���B
	// - intern_string()���A�L�[�ƒl���������������w���悤�ɂ��āA�������̖��ʂ�ጸ���܂����B
	//   ���֐��̏����́Aght_hash_table.c�̎����́A�ȉ��̓����Ɉˑ����Ă��܂��B			//��
	//   ���Eght_insert()�Œǉ������V�����G���g���́A�C�e���[�V�����p�̃��X�g�̖����ɒǉ������B	//��
	//   ���Eght_hash_entry_t�̎��̂̓��n�b�V�����������Ă����̂܂܎g�p����A�J��/�č쐬����Ȃ��B	//�����������ӁFght_hash_table_t�̎����ˑ��̏����ł��B������
	//   ���E�]���āAght_hash_entry_t.key_data�̃A�h���X�́Aght_remove()���Ăяo���܂ŕs�ςł���B	//��
	//   ��L�̓����𗘗p���āA��������ɓ������e�̕�������d�ɕێ����Ă��܂����ʂ��Ȃ��܂����B	//��
	static ght_hash_table_t* ht;
    //{{2016/05/30�ǉ�:intern_string()���g�Ŕr��������s���悤�ɂ��܂����B����܂ł́A�A�v���P�[�V���������ӂ���intern_string()�����荞�݃��[�`����}���`�X���b�h���Ŏg�p���Ȃ��悤�ɂ��Ă��܂������Aintern_string()�͗l�X�ȃ��W���[��������ԐړI�ɗ��p����̂ŁA�A�v���P�[�V�������Ӑ}���Ċ��荞�݃��[�`����}���`�X���b�h���Ŏg�p���Ȃ��悤�ɒ��ӂ���͓̂���Ǝv��������ł��B����́Aquark_from_string()/quark_to_string()/intern_string()��A�������ԐړI�ɌĂяo���e���W���[���̊֐����A���荞�݃��[�`����}���`�X���b�h���ň��S�Ɏg�p�o����悤�ɂȂ�܂����B
    ENTER_CS;
    //}}2016/05/30�ǉ�:intern_string()���g�Ŕr��������s���悤�ɂ��܂����B����܂ł́A�A�v���P�[�V���������ӂ���intern_string()�����荞�݃��[�`����}���`�X���b�h���Ŏg�p���Ȃ��悤�ɂ��Ă��܂������Aintern_string()�͗l�X�ȃ��W���[��������ԐړI�ɗ��p����̂ŁA�A�v���P�[�V�������Ӑ}���Ċ��荞�݃��[�`����}���`�X���b�h���Ŏg�p���Ȃ��悤�ɒ��ӂ���͓̂���Ǝv��������ł��B����́Aquark_from_string()/quark_to_string()/intern_string()��A�������ԐړI�ɌĂяo���e���W���[���̊֐����A���荞�݃��[�`����}���`�X���b�h���ň��S�Ɏg�p�o����悤�ɂȂ�܂����B
	//�n�b�V���e�[�u�������쐬�Ȃ�΁c
	if(!ht) {
		//�n�b�V���e�[�u�����쐬����B
		ht = ght_create(0);	//�����T�C�Y�𖳎w��Ƃ���B
		ght_set_rehash(ht, 1);	//�������T�C�Y��L���ɂ���B
	}
	//������|�C���^��NULL�̏ꍇ�́A���̂܂�NULL��Ԃ��B�����łȂ���΁c
	if(s) {
		//������̒������擾����B�L�[��l�Ƃ��Ă��g�p����̂ŃL�[�̖����Ƀk���������K�v�ł���A(+1)���K�v�ł��B
		int n = strlen(s) + 1/*nul*/;
		//�n�b�V���e�[�u�����猟������B
		char* t = ght_get(ht, n, s);
		//�o�^����Ă�����A���̃f�[�^��Ԃ��B�o�^����Ă��Ȃ���΁c
		if(!t) {
			ght_hash_entry_t* e;
			//����������������A�����񎩐g���L�[�Ƃ��ēo�^����B
			if(ght_insert(ht, (void*)s/*�_�~�[*/, n, s)) { DIE(); }	//�o�O
			//�V�����G���g���́A�C�e���[�V�����p�̃��X�g�̖����ɒǉ�����Ă���͂��ł���B	//��
			// - ght_hash_table.c��ght_insert()�̎���,�y��,2016/05/06�R�����g���Q�Ƃ���B	//��
			e = CONTAINING_RECORD(ht->le_iterator.Blink, ght_hash_entry_t, le_iterator);	//��
#ifdef  DEBUG												//��
			if(e->p_entry_data != s/*�_�~�[*/) { DIE(); }	//�o�O				//��
#endif//DEBUG												//��
			//�G���g�����g�̃L�[���A�n�b�V���e�[�u�����ɕێ�����������Ƃ��Ď擾����B	//��
			t = e->key_data;								//��
#ifdef  DEBUG												//�����������ӁFght_hash_table_t�̎����ˑ��̏����ł��B������
			if(strcmp(s, t)) { DIE(); }	//�o�O						//��
#endif//DEBUG												//��
			//�G���g���̃f�[�^���A�G���g�����g�̃L�[���w���悤�ɒu��������B		//��
		    //{{�ǂ���ł��������ʂɂȂ�܂��B��҂̕��������̂ŁA��҂��g�����ɂ��܂����B	//��
		    //	if(ght_replace(ht, t, n, s) != s/*�_�~�[*/) { DIE(); }	//�o�O			//��
		    //���ǂ���ł��������ʂɂȂ�܂��B��҂̕��������̂ŁA��҂��g�����ɂ��܂����B	//��
			e->p_entry_data = t;								//��
		    //}}�ǂ���ł��������ʂɂȂ�܂��B��҂̕��������̂ŁA��҂��g�����ɂ��܂����B	//��
		}
		//�n�b�V���e�[�u�����ɕێ�����������̃f�[�^��߂�l�Ƃ���B
		s = t;
	}
    //{{2016/05/30�ǉ�:intern_string()���g�Ŕr��������s���悤�ɂ��܂����B����܂ł́A�A�v���P�[�V���������ӂ���intern_string()�����荞�݃��[�`����}���`�X���b�h���Ŏg�p���Ȃ��悤�ɂ��Ă��܂������Aintern_string()�͗l�X�ȃ��W���[��������ԐړI�ɗ��p����̂ŁA�A�v���P�[�V�������Ӑ}���Ċ��荞�݃��[�`����}���`�X���b�h���Ŏg�p���Ȃ��悤�ɒ��ӂ���͓̂���Ǝv��������ł��B����́Aquark_from_string()/quark_to_string()/intern_string()��A�������ԐړI�ɌĂяo���e���W���[���̊֐����A���荞�݃��[�`����}���`�X���b�h���ň��S�Ɏg�p�o����悤�ɂȂ�܂����B
    LEAVE_CS;
    //}}2016/05/30�ǉ�:intern_string()���g�Ŕr��������s���悤�ɂ��܂����B����܂ł́A�A�v���P�[�V���������ӂ���intern_string()�����荞�݃��[�`����}���`�X���b�h���Ŏg�p���Ȃ��悤�ɂ��Ă��܂������Aintern_string()�͗l�X�ȃ��W���[��������ԐړI�ɗ��p����̂ŁA�A�v���P�[�V�������Ӑ}���Ċ��荞�݃��[�`����}���`�X���b�h���Ŏg�p���Ȃ��悤�ɒ��ӂ���͓̂���Ǝv��������ł��B����́Aquark_from_string()/quark_to_string()/intern_string()��A�������ԐړI�ɌĂяo���e���W���[���̊֐����A���荞�݃��[�`����}���`�X���b�h���ň��S�Ɏg�p�o����悤�ɂȂ�܂����B
	return s;
//}}2016/05/06�ύX:intern_string()���A�L�[�ƒl���������������w���悤�ɂ��āA�������̖��ʂ�ጸ���܂����B
}

/*****************************************************************************
 *	�u��
 *****************************************************************************/

#ifndef PIECE
char* strreplace(const char* s, const char* oldstr, const char* newstr) {
	//���̕�������A�������镶����ŕ�������B
	char** argv = strsplit(s, oldstr, 0);
	//����������������A�u�����镶���������ŘA������B
	char* t = strjoinv(newstr, argv);
	//����������������J������B
	strfreev(argv);
	//�A�������������Ԃ��B
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
 *	����������
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
 *	���������[�e�[�g
 *****************************************************************************/
// * Mon Jun 06 21:45:00 JST 2016 Naoyuki Sawa
// - memrotate()��ǉ����܂����B
// * Tue Jun 07 22:13:11 JST 2016 Naoyuki Sawa
// - memrotate()�̎�����ύX���܂����B
//   �ڍׂ́Aclipstl.c���W���[���́Astd_rotate()�֐��̃R�����g���Q�Ƃ��ĉ������B
//   memrotate()�̊֐��d�l�͕ς���Ă��܂���B
#define USE_MEMROTATE_STD_ROTATE	//���̃V���{�����`����ƁAstd_rotate()���Ăяo���o�[�W������memrotate()���g�p���܂��B���̃V���{�����`���Ȃ��ƁA�Ǝ��A���S���Y���̑f���Ȏ����o�[�W�������g�p���܂��B		2016/06/07���݂́Astd_rotate()���Ăяo���o�[�W������memrotate()���g�p����悤�ɂ��Ă��܂��B����A�ύX���鎖�͖����Ǝv���܂��B
#ifndef USE_MEMROTATE_STD_ROTATE
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
//���Ǝ��A���S���Y���̑f���Ȏ����B2016/06/06�ɍ쐬���A2016/06/06�̈���Ԃ����g�p���܂���ł������AC����ŁE�A�Z���u���ŋ���(clipmemc.c���W���[����MemCpt_Resize()���g����)�\�����؂����̂ŁA����͐������Ǝv���܂��B2016/06/07��memrotate()��std_rotate()���g���悤�ɕύX���āA����������̃o�[�W�������g�����͖����Ǝv���܂����A�ꉞ�c���Ă����܂��B
#ifndef PIECE
void* memrotate(void* _base, int n, size_t size) {	//���ϓI�ɂ�'memrotate(base,size,n)'�̈������̕������R�ȋC�����邪�Amemset()���̈����d�l�ɍ��킹��size�������Ō�ɂ��鎖�ɂ����B
	char* const base = _base;
	int i, m;
	//�ꎞ�o�b�t�@�B���̃T�C�Y�𑝂₷�قǍ����ɂȂ�܂��B���s���ɂ���ăX�^�b�N�����Ȃ����x�ŁA�Ȃ�ׂ��傫�ȃT�C�Y�ɂ���悤�������ĉ������B
#ifdef  WIN32
	char tmp[4096/*������*/];	//Windows�̏ꍇ
#else //WIN32
	char tmp[256/*������*/];	//P/ECE�₻�̑��̑g�ݍ��݊��̏ꍇ
#endif//WIN32
	//�������u���b�N�̃T�C�Y��0�Ȃ�΁A�������Ȃ��B�����łȂ���΁c
	if(size) {
		//���[�e�[�g�����A-(�������u���b�N�̃T�C�Y-1)�`+(�������u���b�N�̃T�C�Y-1) �ɕ␳����B
		n %= (int)size;
		//���[�e�[�g����0�ɂȂ�����A�������Ȃ��B�����łȂ���΁c
		if(n) {
			//������[�e�[�g����ꍇ�̃��[�e�[�g�� 1�`(�������u���b�N�̃T�C�Y-1) �����߂�B
			if(n < 0) { n += size; }
			//�O�����[�e�[�g����ꍇ�̃��[�e�[�g�� 1�`(�������u���b�N�̃T�C�Y-1) �����߂�B
			m = size - n;
			//������[�e�[�g�̕������[�e�[�g�������Ȃ���΁A������[�e�[�g���s���B
			if(n < m) {
				do {
					//����̃��[�e�[�g�������߂�B
					i = (n <= sizeof tmp) ? n : sizeof tmp;
					//�������u���b�N�̖����́A����̃��[�e�[�g�����̃f�[�^���A�ꎞ�o�b�t�@�ɑޔ�����B
					//
					//	base
					//	����������������������������������
					//	��			��	��
					//	����������������������������������
					//				�@������
					//				����������
					//				��tmp	��
					//				����������
					//
					memcpy(tmp, &base[size - i], i);
					//�������u���b�N���A����̃��[�e�[�g�����A����փV�t�g����B
					//
					//	base
					//	����������������������������������
					//	����������			��
					//	����������������������������������
					//
					//				����������
					//				��tmp	��
					//				����������
					//
					memmove(&base[i], &base[0], size - i);
					//�ꎞ�o�b�t�@�ɑޔ������f�[�^���A�������u���b�N�̐擪�ɕ��A����B
					//
					//	base
					//	����������������������������������
					//	��	��			��
					//	����������������������������������
					//	�@������
					//	�@������		����������
					//	�@������������������������tmp	��
					//				����������
					//
					memcpy(&base[0], tmp, i);
				} while(n -= i);	//�c�胍�[�e�[�g�������炵�A0�ɂȂ�܂ŌJ��Ԃ��B
			//�O�����[�e�[�g�̕������[�e�[�g�������Ȃ���΁A�O�����[�e�[�g���s���B		������[�e�[�g�ƑO�����[�e�[�g�̃��[�e�[�g���������ꍇ�͂ǂ���ł��ǂ����A�O�����[�e�[�g�̕���memmove()��memcpy()�Ăяo���ɂȂ��č����Ȃ̂ŁA�����ꍇ�͑O�����[�e�[�g���g�����Ƃɂ����B(cliplibc.c�̃A�Z���u����memmove()�̎������Q�Ƃ���B)
			} else {
				do {
					//����̃��[�e�[�g�������߂�B
					i = (m <= sizeof tmp) ? m : sizeof tmp;
					//�������u���b�N�̐擪�́A����̃��[�e�[�g�����̃f�[�^���A�ꎞ�o�b�t�@�ɑޔ�����B
					//
					//	base
					//	����������������������������������
					//	��	��		��	��
					//	����������������������������������
					//	�@������
					//	����������
					//	��tmp	��
					//	����������
					//
					memcpy(tmp, &base[0], i);
					//�������u���b�N���A����̃��[�e�[�g�����A�O���փV�t�g����B
					//
					//	base
					//	����������������������������������
					//	��			����������
					//	����������������������������������
					//
					//	����������
					//	��tmp	��
					//	����������
					//
					memmove(&base[0], &base[i], size - i);
					//�ꎞ�o�b�t�@�ɑޔ������f�[�^���A�������u���b�N�̖����ɕ��A����B
					//
					//	base
					//	����������������������������������
					//	��			��	��
					//	����������������������������������
					//				�@������
					//	����������		�@������
					//	��tmp	������������������������
					//	����������
					//
					memcpy(&base[size - i], tmp, i);
				} while(m -= i);	//�c�胍�[�e�[�g�������炵�A0�ɂȂ�܂ŌJ��Ԃ��B
			}
		}
	}
	return base;	//�������u���b�N�̐擪�A�h���X�����̂܂ܕԂ��B���ɈӖ��͖������Amemset()���̖߂�l�d�l�ɍ��킹�鎖�ɂ����B
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
		sub		%sp, 256/4			;//%sp  := tmp					*delay*		�����������X�^�b�N���tmp[256]���m�ۂ�����:�������火��������
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
		xadd		%sp, %sp, 256			;//								�����������X�^�b�N���tmp[256]���m�ۂ�����:�����܂Ł���������
memrotate_NOJOB:						;//
		ld.w		%r10, %r0			;//%r10 := base
		popn		%r3				;//
		ret						;//return  base
");
#endif//PIECE
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#else //USE_MEMROTATE_STD_ROTATE
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
//��std_rotate()���Ăяo���o�[�W����	2016/06/07�`�g�p�J�n
void* memrotate(void* _base, int n, size_t size) {	//���ϓI�ɂ�'memrotate(base,size,n)'�̈������̕������R�ȋC�����邪�Amemset()���̈����d�l�ɍ��킹��size�������Ō�ɂ��鎖�ɂ����B
	char* const base = _base;
	//�������u���b�N�̃T�C�Y��0�Ȃ�΁A�������Ȃ��B�����łȂ���΁c
	if(size) {
		//���[�e�[�g�����A-(�������u���b�N�̃T�C�Y-1)�`+(�������u���b�N�̃T�C�Y-1) �ɕ␳����B
		n %= (int)size;
		//���[�e�[�g����0�ɂȂ�����A�������Ȃ��B�����łȂ���΁c
		if(n) {
			//������[�e�[�g����ꍇ�̃��[�e�[�g�� 1�`(�������u���b�N�̃T�C�Y-1) �����߂�B
			if(n < 0) { n += size; }
			//�O�����[�e�[�g����ꍇ�̃��[�e�[�g�� 1�`(�������u���b�N�̃T�C�Y-1) �����߂�B
			n = size - n;
			//std_rotate()�ւ̑�2�����̓��[�e�[�g��ɐ擪�ɂȂ�v�f�ւ̃|�C���^���w�肷��C�����C(�O�����[�e�[�g��)�Ԗڂ̗v�f�ւ̃|�C���^�ł��B
			//
			//	base	&base[n]		&base[size]
			//	����������������������������������
			//	��	��			��
			//	����������������������������������
			//	�������� n
			//
			std_rotate(base, &base[n], &base[size]);
		}
	}
	return base;	//�������u���b�N�̐擪�A�h���X�����̂܂ܕԂ��B���ɈӖ��͖������Amemset()���̖߂�l�d�l�ɍ��킹�鎖�ɂ����B
}
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#endif//USE_MEMROTATE_STD_ROTATE

/*****************************************************************************
 *	�ϊ�
 *****************************************************************************/

#ifdef  WIN32
char* StrConv(const char* str, int conv) {
	char* buf;
	int size, flag = 0;
	//VBA��StrConv()�݊��̃t���O���AWin32��LCMapString()�݊��̃t���O�ɕϊ�����B
	if(conv & vbUpperCase) { flag |= LCMAP_UPPERCASE; }	//Use uppercase.
	if(conv & vbLowerCase) { flag |= LCMAP_LOWERCASE; }	//Use lowercase.
	if(conv & vbWide     ) { flag |= LCMAP_FULLWIDTH; }	//Wide characters (where applicable).
	if(conv & vbNarrow   ) { flag |= LCMAP_HALFWIDTH; }	//Narrow characters (where applicable).
	if(conv & vbKatakana ) { flag |= LCMAP_KATAKANA;  }	//Katakana.
	if(conv & vbHiragana ) { flag |= LCMAP_HIRAGANA;  }	//Hiragana.
	//���̕�����̕ϊ��ɕK�v�ȃT�C�Y���擾����B
	// - �ϊ��ɕK�v�ȃT�C�Y���擾����ꍇ�́A�Ō�̈�����0���w�肷��K�v������B
	//   �b��̏o�̓o�b�t�@�T�C�Y���w�肵�āA�����Ȃ�Έ�x�ōς܂��悤�Ƃ���ƁA�G���[(0)���Ԃ���A�K�v�ȃT�C�Y���擾�ł��Ȃ��B
	size = LCMapString(GetSystemDefaultLCID(), flag, str, -1, NULL, 0);
	if(!size) { DIE(); }
	//���̕�����̕ϊ��ɕK�v�ȃT�C�Y�́A�o�̓o�b�t�@�����蓖�Ă�B
	// - LCMapString API���Ԃ��A�ϊ��ɕK�v�ȃT�C�Y�́A�I�[��nul���܂�ł���B
	//   �A�v���P�[�V�����v���O�������ŁA���蓖�ăT�C�Y��(+1)����K�v�͖����B
	buf = malloc(size);
	if(!buf) { DIE(); }
	//���̕������ϊ�����B
	size = LCMapString(GetSystemDefaultLCID(), flag, str, -1, buf, size);
	if(!size) { DIE(); }
	//�ϊ������������Ԃ��B
	// - �Ăяo������free()���ĊJ������B
	return buf;
}
#endif//WIN32
