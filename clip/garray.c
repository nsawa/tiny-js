/*
 *	garray.c
 *
 *	Arrays - arrays of arbitrary elements which grow automatically as elements are added
 *
 *	* Mon Apr 18 21:50:41 JST 2016 Naoyuki Sawa
 *	- 1st �����[�X�B
 *	- GLib��Arrays(http://developer.gnome.org/glib/stable/glib-Arrays.html)���Q�l�ɂ��܂����B
 *	  �֐��d�l�̓I���W�i���łƓ����ł����A�����͓Ǝ��ŁA�������x�����e�ʍ팸��D�悵�܂����B
 *	* Tue Apr 19 21:31:14 JST 2016 Naoyuki Sawa
 *	- GPtrArray(https://developer.gnome.org/glib/stable/glib-Pointer-Arrays.html)�ƁA
 *	  GByteArray(https://developer.gnome.org/glib/stable/glib-Byte-Arrays.html)��ǉ����܂����B
 *	  �֐��d�l�̓I���W�i���łƓ����ł����A�����͓Ǝ��ŁA�������x�����e�ʍ팸��D�悵�܂����B
 *	* Wed Apr 20 10:51:26 JST 2016 Naoyuki Sawa
 *	- g_array_set_clear_func()�ɏ�����ǉ����A�v�f���폜����֐�(�A�v���P�[�V������`)�Ƃ���free�֐��̃A�h���X���w�肳�ꂽ�ꍇ�̓G���[��~����悤�ɂ��܂����B
 *	  �Ⴆ�΁A�ȉ��̂悤�ȌĂяo���̓o�O�ł���A��������o���邽�߂ł��B
 *	  ��void test() {
 *	  ��  GPtrArray* array = g_ptr_array_new_with_free_func(free);
 *	  ��  g_ptr_array_add(array, strdup("FOO"));
 *	  ��  g_ptr_array_unref(array);		�������ňُ퓮��ƂȂ�B
 *	  ��}
 *	  �Ȃ��Ȃ�΁Astrdup()�̌Ăяo����'gc/leak_detector.h'�̃}�N���֐���`�ɂ����GC_debug_strdup()�ɒu���������܂����A
 *	  g_ptr_array_new_with_free_func()�Ɉ����n��free�͊֐��Ăяo���`���ł͂Ȃ��̂Ń}�N���֐��̒u�������ΏۂƂ͂Ȃ炸�AGC_debug_free�̊֐��A�h���X�ł͂Ȃ��{����free�̊֐��A�h���X�ɂȂ��Ă��܂�����ł��B
 *	  ���ʂƂ��āAGC_debug_strdup()�Ŋm�ۂ�����������{����free()�ŊJ�����悤�Ƃ��Ă��܂��A�ُ퓮��ƂȂ�܂��B
 *	- �����Ă̈�́A'gc/leak_detector.h'�̃}�N���֐���`�����������āA�}�N���֐��ł͂Ȃ��V���{���ł��u�������������悤�ɂ��Ă��܂����ł����A��������������͓����������܂����B
 *	  'gc/leak_detector.h'����C���N���[�h�����'gc/gc.h'���A�}�N���֐��ɒǉ��̈���GC_EXTRAS��n���Ă���A�����V���{���Œu��������}�N���ɏ���������Ƃ��ꂪ�����o���Ȃ��Ȃ邩��ł��B
 *	- �d���������̂ŁA�A�v���P�[�V�����ɓ��ߓI�ȉ����Ă͂�����߂āA�A�v���P�[�V��������L�̂悤�ȃR�[�h�������Ă��܂������ɑ��G���[��~���Č��o������@����鎖�ɂ��܂����B
 *	  �����ŃG���[��~�����ꍇ�A�A�v���P�[�V�������O�q�̂悤�ȏ������s���Ă���\���������̂ŁA�T���o���Ĉȉ��̂悤�ɕύX���ĉ������B
 *	  ��static void user_free(void* ptr) { free(ptr);//����̓}�N���֐��Ƃ��Ēu����������B }
 *	  ��void test() {
 *	  ��  GPtrArray* array = g_ptr_array_new_with_free_func(user_free);
 *	  ��  g_ptr_array_add(array, strdup("FOO"));
 *	  ��  g_ptr_array_unref(array);
 *	  ��}
 *	* Tue May 24 23:59:14 JST 2016 Naoyuki Sawa
 *	- GRealArray�\���̂́Aalloc_bytes�t�B�[���h��elt_size�t�B�[���h�̏��������ւ��܂����B
 *	  gstring.h���W���[����GString�\���̂́Astr,len,allocated_len�̕��тɍ��킹�邽�߂ł��B
 *	- garray.c���W���[���́AGRealArray�\���̂�alloc_bytes�t�B�[���h��elt_size�t�B�[���h�̏����Ɉˑ����Ă��Ȃ������̂ŁA���̕ύX�ɂ��e���͗L��܂���B
 *	  �A�v���P�[�V�����̊ϓ_������A���XGRealArray�\���̂�alloc_bytes�t�B�[���h��elt_size�t�B�[���h�͔���J�������̂ŁA���̕ύX�ɂ��e���͗L��܂���B
 *	* Sat Jun 25 21:55:36 JST 2016 Naoyuki Sawa
 *	- _g_array_index()�ւ�index�w����A����܂ł�(��len)�܂ł��������Ă��܂���ł������A(��len)�܂ŋ�����悤�ɕύX���܂����B
 *	  �ύX�������R�́A�z��̖����̃A�h���X���擾�����������L��܂����A����܂ł̏����̂܂܂ł́u&g_array_index(a,int,a->len)�v�̂悤�Ɏw�肵�����ɃA�T�[�g���Ă��܂��Ă�������ł��B
 *	  �ύX��́A��L�̎w����A�T�[�g�����ɓ��삷��悤�ɂȂ�܂����B
 *	  �������A�z��̖����ɑ΂��Ď擾���ėǂ��̂͂����܂ł��A�h���X�����ł���A�l���擾����Ɨ\���o���Ȃ�����ɂȂ鎖�ɒ��ӂ��ĉ������B
 *	  ���AGLib�̃I���W�i���ł̎����ł͌��Xindex�͈̔͂��������Ă��Ȃ��̂ŁA����̕ύX�ɂ���ăI���W�i���ł������S���������鎖�͗L��܂���B
 */
#include "clip.h"
/*****************************************************************************
 *	GArray
 *****************************************************************************/
//GRealArray.array_type
#define ARRAY_TYPE_G_ARRAY		0
#define ARRAY_TYPE_G_PTR_ARRAY		1
#define ARRAY_TYPE_G_BYTE_ARRAY		2
/*****************************************************************************
 *	�\����
 *****************************************************************************/
typedef struct _GRealArray {
//public:
	char*		data;				//A pointer to the element data. The data may be moved as elements are added to the GArray.	��GArray.data		��GPtrArray.pdata	��GByteArray.data	��GString.str
	int		len;				//The number of elements in the GArray not including the possible terminating zero element.	��GArray.len		��GPtrArray.len		��GByteArray.len	��GString.len
//private:																											��GString.allocated_len
	int		alloc_bytes;			//�m�ۍς݃������̃o�C�g���B	�v�f���ł͂Ȃ����ɒ��ӂ���B
	int		elt_size;			//�v�f�̃o�C�g���B
//�s�v	unsigned	zero_terminated		:1;	�ˏ�Ƀ[���I�[���鎖�ɂ����̂ł��̃t�B�[���h�͕s�v�ɂȂ�܂����B�[���I�[���s�v�ł���ꍇ�͏����e�ʂ̖��ʂ������邪�Ăяo�������ӎ�����K�v�͗L��܂���B
//�s�v	unsigned	clear			:1;	��g_array_set_size()�Ŋg�������v�f����Ƀ[���N���A���鎖�ɂ����̂ł��̃t�B�[���h�͕s�v�ɂȂ�܂����B�[���N���A���s�v�ł���ꍇ�͏��������̖��ʂ������邪�Ăяo�������ӎ�����K�v�͗L��܂���B
	unsigned short	ref_count;			//�Q�ƃJ�E���g�B���\�[�X�ł�32�r�b�g�l���������A�������ł�16�r�b�g�l�ɕύX���ă�������ߖ񂷂鎖�ɂ����B���ʂ̎g�����ŎQ�ƃJ�E���g��(2^16)�ȏ�ɂȂ鎖�͂܂������Ǝv�̂ŁA����ŏ[�����Ǝv���B
	unsigned short	array_type;			//�z��̎�ށBARRAY_TYPE_G_ARRAY,����,ARRAY_TYPE_G_PTR_ARRAY,����,ARRAY_TYPE_G_BYTE_ARRAY�B2016/04/20���݂�g_array_on_remove_element()�����ŎQ�Ƃ��Ă���B��������A�z��̎�ނɂ���ē����ς��鏈����ǉ�����ꍇ�́A���̃t�B�[���h���Q�Ƃ���B
	void		(*clear_func)(void* data);	//�v�f���폜����֐�(�A�v���P�[�V������`)�Bg_array_set_clear_func(),����,g_ptr_array_set_free_func()�ɂ���ăZ�b�g����Bg_array_on_remove_element()�o�R�ŌĂяo���B���ڌĂяo���Ȃ����R�́AGArray�Ȃ�Ηv�f�̃A�h���X�������Ƃ��ČĂяo�����AGPtrArray�Ȃ�Ηv�f�̒l�������Ƃ��ČĂяo���K�v���L�邩��ł���Bg_array_on_remove_element()�����̈Ⴂ���z������B
} GRealArray;
/*****************************************************************************
 *	���[�J���֐�
 *****************************************************************************/
//�K�v�ȃo�C�g������A���ۂɊm�ۂ���o�C�g�������߂�B
//���ۂɊm�ۂ���o�C�g���́A�K�v�ȃo�C�g���ȏ�,����,�ŏ���(2^N)�Ƃ���B
static int g_array_pow2_alloc_bytes(int want_alloc) {
	int actual_alloc = 1;	//���ۂɊm�ۂ���o�C�g���̍ŏ��l�ł��B�����Ƒ傫�Ȓl����n�߂Ă��\���܂���B���\�[�X�ł�16�ƂȂ��Ă��܂����������ȈӖ��̂���l�ł͂Ȃ��Ǝv���܂��B�����̕p�ɂȍĊ��蓖�Ăɂ�鐫�\�ቺ�����ɂȂ鎖�͏��Ȃ��Ǝv���̂œ������ł͒P����1�Ƃ��܂����B
	while((unsigned)actual_alloc < (unsigned)want_alloc) {
		if(!(actual_alloc <<= 1)) { DIE(); }	//want_alloc���傫�߂���B
	}
	return actual_alloc;
}
/*--------------------------------------------------------------------------*/
//((���݂̗v�f��)+(len)+(�[���I�[))�̃��������m�ۂ���B
static GRealArray* g_array_maybe_expand(GRealArray* rarray, int len) {
	int new_alloc_bytes;
	if(len < 0) { DIE(); }
	//�K�v�ȃ������̃o�C�g�������߂�B
	new_alloc_bytes = rarray->elt_size * (rarray->len + len + 1/*�[���I�[*/);
	//�K�v�ȃ������̃o�C�g�����A�m�ۍς݃������̃o�C�g�������傫����΁c
	if((unsigned)new_alloc_bytes > (unsigned)rarray->alloc_bytes) {
		//�K�v�ȃo�C�g������A���ۂɊm�ۂ���o�C�g�������߂�B
		new_alloc_bytes = g_array_pow2_alloc_bytes(new_alloc_bytes);
		//���������m��,����,�g������B
		rarray->data = realloc(rarray->data, new_alloc_bytes);
		if(!rarray->data) { DIE(); }	//�������s��
		//�V�����m�ۂ����v�f���[���N���A����B
		memset(&rarray->data[rarray->alloc_bytes], 0, (new_alloc_bytes - rarray->alloc_bytes));
		//�m�ۍς݃������̃o�C�g�����i�[����B
		rarray->alloc_bytes = new_alloc_bytes;
	}
	return rarray;
}
/*--------------------------------------------------------------------------*/
//�m�ۍς݃������̔����𒴂��镔�����]���Ȃ�΁A���������k������B
// - ���\�[�X�̎����ł́A��U�m�ۂ��ꂽ�������́A�z����폜����܂ŊJ������Ȃ��悤�ł��B
//   �����������A�v�f���̑��������傫���A��u�����v�f�����ɒ[�ɑ������㌸�鎖������ꍇ�A���ʂȃ������������ԑ��݂��Ă��܂���肪�����܂��B
//   ��L�̖�������邽�߂ɁA�������ł́Ag_array_remove_�`()�����s������ɁA�z��m�ۍς݃������̔����𒴂��閳�ʂ������Ă�����A���������k�����鎖�ɂ��܂����B
// - ���̕ύX�ɂ�镾�Q�Ƃ��āAg_array_sized_new()���g���ĈӐ}�I��(������Ԃ�)���ʂȃ�������\�񂵂��z����쐬�����ꍇ�A���̒����g_array_remove_�`()���Ăяo���ƁA�\�񂵂����������k������Ă��܂��܂��B
//   ���������Ȃ��Ă����쌋�ʂɂ͖��L��܂��񂪁A���\�ቺ�������N�����\�����L��܂��B
//   ���������ۂɂ́A���̂悤�Ȏ����N����P�[�X�͏��Ȃ��Ǝv���܂��B
//   g_array_sized_new()���g���ĈӐ}�I��(������Ԃ�)���ʂȃ�������\�񂵂��z����쐬�����ꍇ�A���̒����g_array_remove_�`()���Ăяo�����͏��Ȃ��Ǝv������ł��B
//   g_array_sized_new()���g�����ꍇ�A���̌�ɍs������́A�A���I��g_array_append_�`()�ł��鎖���قƂ�ǂ��Ǝv���܂��B
//   g_array_maybe_reduce()���Ăяo���̂�g_array_remove_�`(),�y��,(�ԐړI��)g_array_set_size()�����ł���Ag_array_append_�`()���Ăяo����Ă����������k�����鑀��͍s���܂���B
static GRealArray* g_array_maybe_reduce(GRealArray* rarray) {
	int new_alloc_bytes;
	//�K�v�ȃ������̃o�C�g�������߂�B
	new_alloc_bytes = rarray->elt_size * (rarray->len + 1/*�[���I�[*/);
	//�K�v�ȃo�C�g������A���ۂɊm�ۂ���o�C�g�������߂�B
	new_alloc_bytes = g_array_pow2_alloc_bytes(new_alloc_bytes);
	//�m�ۍς݃������̃o�C�g�����A���ۂɊm�ۂ���o�C�g����2�{/*������*/�𒴉߂��Ă�����c
	if((unsigned)rarray->alloc_bytes > (unsigned)(new_alloc_bytes << 1/*������*/)) {
		//���������k������B
		rarray->data = realloc(rarray->data, new_alloc_bytes);
		if(!rarray->data) { DIE(); }	//�k��������realloc()�����s���鎖�͖����͂��B
		//�m�ۍς݃������̃o�C�g�����i�[����B
		rarray->alloc_bytes = new_alloc_bytes;
	}
	return rarray;
}
/*--------------------------------------------------------------------------*/
//�v�f���폜���鎞�ɌĂяo�����[�J���֐��B
static void g_array_on_remove_element(GRealArray* rarray, int index) {
	//�v�f���폜����֐�(�A�v���P�[�V������`)���Z�b�g����Ă�����c
	if(rarray->clear_func) {
		//�v�f�̃A�h���X���擾����B
		void* data = &rarray->data[rarray->elt_size * index];
		//�z��̎�ނɂ���āc
		switch(rarray->array_type) {
		default:DIE();	//�o�O
		case ARRAY_TYPE_G_ARRAY:
			//GArray�Ȃ�΁A�v�f�̃A�h���X�������Ƃ��ėv�f���폜����֐�(�A�v���P�[�V������`)�����s����̂ŁAdata�͂��̂܂܂ŗǂ��B
			/** no job **/
			break;
		case ARRAY_TYPE_G_PTR_ARRAY:
			//GPtrArray�Ȃ�΁A�v�f�̒l�������Ƃ��ėv�f���폜����֐�(�A�v���P�[�V������`)�����s����̂ŁAdata���w���l�����o���B
			data = *(void**)data;
			break;
	    //	case ARRAY_TYPE_G_BYTE_ARRAY:
	    //		//GByteArray�Ȃ�΁Ararray->clear_func����NULL�ɂȂ鎖�͖����͂��ł���B������NULL�Ȃ��default:�߂ŃG���[��~������B
		}
		//�v�f�̃A�h���X,����,�v�f�̒l�������Ƃ��āA�v�f���폜����֐�(�A�v���P�[�V������`)�����s����B
		(*rarray->clear_func)(data);
	}
}
/*****************************************************************************
 *	�O���[�o���֐�
 *****************************************************************************/
//Creates a new GArray with a reference count of 1.
//Parameters
//		zero_terminated		TRUE if the array should have an extra element at the end which is set to 0.
//		clear			TRUE if GArray elements should be automatically cleared to 0 when they are allocated.
//		element_size		The size of each element in bytes.
//Returns
//		The new GArray.
GArray* g_array_new(int zero_terminated, int clear, int element_size) {
	return g_array_sized_new(zero_terminated, clear, element_size, 0);
}
/*--------------------------------------------------------------------------*/
//Creates a new GArray with reserved_size elements preallocated and a reference count of 1.
//This avoids frequent reallocation, if you are going to add many elements to the array.
//Note however that the size of the array is still 0.
//Parameters
//		zero_terminated		TRUE if the array should have an extra element at the end with all bits cleared.	�˓������ł͂��̈����͎Q�Ƃ��܂���B
//		clear			TRUE if all bits in the array should be cleared to 0 on allocation.			�˓������ł͂��̈����͎Q�Ƃ��܂���B
//		element_size		Size of each element in the array.
//		reserved_size		Number of elements preallocated.
//Returns
//		The new GArray.
GArray* g_array_sized_new(int zero_terminated, int clear, int element_size, int reserved_size) {
	GRealArray* rarray;
	if((element_size < 0) /*|| (reserved_size < 0)*/) { DIE(); }	//(reserved_size<0)��g_array_maybe_expand()�Ō��������̂ł����ł͌����s�v�ł��B
	//�\���̂̃��������m�ۂ���B
	rarray = calloc(1, sizeof(GRealArray));		//elt_size�ȊO�̃t�B�[���h��0�ɏ��������邽�߂ɁAmalloc()�łȂ�calloc()���K�{�ł��B
	if(!rarray) { DIE(); }
	//�\���̂̃t�B�[���h������������B
	rarray->elt_size   = element_size;
	rarray->ref_count  = 1;
//�s�v	rarray->array_type = ARRAY_TYPE_G_ARRAY;	//�z��̎�ނ��i�[����B
	//�������������m�ۂ���B
	return (GArray*)g_array_maybe_expand(rarray, reserved_size);
}
/*--------------------------------------------------------------------------*/
//Atomically increments the reference count of array by one.
//This function is MT-safe and may be called from any thread.									�˓������ł̓X���b�h�Z�[�t�ł͂���܂���B�����̃X���b�h����Q�Ƃ��鎖�͖����Ǝv��������ł��B���ۂ̏�g_array_ref()/g_array_unref()�������X���b�h�Z�[�t�ɂ��Ă����̊֐����X���b�h�Z�[�t�łȂ���΂��܂�Ӗ��͖����Ǝv���܂��B
//Parameters
//		array			A GArray.
//Returns
//		The passed in GArray.
GArray* g_array_ref(GArray* array) {
	GRealArray* rarray = (GRealArray*)array;
	if(!rarray->ref_count || (rarray->ref_count > (USHRT_MAX-1))) { DIE(); }
	//�Q�ƃJ�E���g�𑝂₷�B
	rarray->ref_count++;
	return (GArray*)rarray;
}
/*--------------------------------------------------------------------------*/
//Atomically decrements the reference count of array by one.
//If the reference count drops to 0, all memory allocated by the array is released.
//This function is MT-safe and may be called from any thread.									�˓������ł̓X���b�h�Z�[�t�ł͂���܂���B�����̃X���b�h����Q�Ƃ��鎖�͖����Ǝv��������ł��B���ۂ̏�g_array_ref()/g_array_unref()�������X���b�h�Z�[�t�ɂ��Ă����̊֐����X���b�h�Z�[�t�łȂ���΂��܂�Ӗ��͖����Ǝv���܂��B
//Parameters
//		array			A GArray.
void g_array_unref(GArray* array) {
	GRealArray* rarray = (GRealArray*)array;
	if(!rarray->ref_count /*|| (rarray->ref_count > USHRT_MAX)*/) { DIE(); }
	//�Ō�̎Q�ƂłȂ���΁c
	if(rarray->ref_count != 1) {
		//�Q�ƃJ�E���g�����炷�B
		rarray->ref_count--;
	//�Ō�̎Q�ƂȂ�΁c
	// - �����ŎQ�ƃJ�E���g�����炳�Ȃ��悤���ӂ���B
	//   �Ō�̎Q�ƃJ�E���g�����炷������g_array_free()���s���B
	} else {
		//�\���̂ƃf�[�^�̃��������J������B
		g_array_free((GArray*)rarray, 1);
	}
}
/*--------------------------------------------------------------------------*/
//Gets the size of the elements in array.
//Returns
//		Size of each element, in bytes.
int g_array_get_element_size(GArray* array) {
	GRealArray* rarray = (GRealArray*)array;
	return rarray->elt_size;
}
/*--------------------------------------------------------------------------*/
//Adds len elements onto the end of the array.
//Parameters
//		array			A GArray.
//		data			A pointer to the elements to append to the end of the array.	[not nullable]
//		len			The number of elements to append.
//Returns
//		The GArray.
GArray* g_array_append_vals(GArray* array, const void* data, int len) {
	return g_array_insert_vals(array, array->len, data, len);
}
/*--------------------------------------------------------------------------*/
//Adds len elements onto the start of the array.
//This operation is slower than g_array_append_vals() since the existing elements in the array have to be moved to make space for the new elements.
//Parameters
//		array			A GArray.
//		data			A pointer to the elements to prepend to the start of the array.	[not nullable]
//		len			The number of elements to prepend.
//Returns
//		The GArray.
GArray* g_array_prepend_vals(GArray* array, const void* data, int len) {
	return g_array_insert_vals(array, 0, data, len);
}
/*--------------------------------------------------------------------------*/
//Inserts len elements into a GArray at the given index.
//Parameters
//		array			A GArray.
//		index			The index to place the elements at.
//		data			A pointer to the elements to insert.	[not nullable]
//		len			The number of elements to insert.
//Returns
//		The GArray.
GArray* g_array_insert_vals(GArray* array, int index, const void* data, int len) {
	GRealArray* rarray = (GRealArray*)array;
	if((unsigned)index > (unsigned)rarray->len) { DIE(); }
	//�K�v�ȃ��������m�ۂ���B
	g_array_maybe_expand(rarray, len);
	//�����̃f�[�^�����ւ��炷�B
	// - (index==rarray->len)�������ꍇ���_�~�[�����ƂȂ���S�ł��B
	memmove(&rarray->data[rarray->elt_size * (index + len)],
	        &rarray->data[rarray->elt_size * index],
	        (rarray->elt_size * (rarray->len - index)));
	//�V�����f�[�^���i�[����B
	memcpy(&rarray->data[rarray->elt_size * index], data, (rarray->elt_size * len));
	//�v�f���𑝂₷�B
	rarray->len += len;
	return (GArray*)rarray;
}
/*--------------------------------------------------------------------------*/
//Removes the element at the given index from a GArray.
//The following elements are moved down one place.
//Parameters
//		array			A GArray.
//		index			The index of the element to remove.
//Returns
//		The GArray.
GArray* g_array_remove_index(GArray* array, int index) {
	return g_array_remove_range(array, index, 1);
}
/*--------------------------------------------------------------------------*/
//Removes the element at the given index from a GArray.
//The last element in the array is used to fill in the space, so this function does not preserve the order of the GArray.
//But it is faster than g_array_remove_index().
//Parameters
//		array			A GArray.
//		index			The index of the element to remove.
//Returns
//		The GArray.
GArray* g_array_remove_index_fast(GArray* array, int index) {
	GRealArray* rarray = (GRealArray*)array;
	if((unsigned)index >= (unsigned)rarray->len) { DIE(); }
	//�v�f���폜���鎞�ɌĂяo�����[�J���֐������s����B
	g_array_on_remove_element(rarray, index);
	//�폜�����v�f�̈ʒu�ɁA�Ō�̗v�f���R�s�[����B
	// - (index==(rarray->len-1))�������ꍇ���_�~�[�����ƂȂ���S�ł��B
	memcpy(&rarray->data[rarray->elt_size * index],
	       &rarray->data[rarray->elt_size * (rarray->len - 1)],
	       rarray->elt_size);
	//�v�f�������炷�B
	rarray->len--;
	//�󂢂����������[���N���A����B
	memset(&rarray->data[rarray->elt_size * rarray->len], 0, rarray->elt_size);
	//�]���ȃ��������J������B
	return (GArray*)g_array_maybe_reduce(rarray);
}
/*--------------------------------------------------------------------------*/
//Removes the given number of elements starting at the given index from a GArray.
//The following elements are moved to close the gap.
//Parameters
//		array			a GArray
//		index			the index of the first element to remove
//		length			the number of elements to remove
//Returns
//		The GArray.
GArray* g_array_remove_range(GArray* array, int index, int length) {
	GRealArray* rarray = (GRealArray*)array;
	int i;
	if(((unsigned)(index         ) > (unsigned)rarray->len) ||
	   ((unsigned)(index + length) > (unsigned)rarray->len) || (length < 0)) { DIE(); }
	//�v�f���폜���鎞�ɌĂяo�����[�J���֐������s����B
	for(i = 0; i < length; i++) {
		g_array_on_remove_element(rarray, index + i);
	}
	//�����̃f�[�^��O�ւ��炷�B
	memmove(&rarray->data[rarray->elt_size * index],
	        &rarray->data[rarray->elt_size * (index + length)],
	        rarray->elt_size * (rarray->len - (index + length)));
	//�v�f�������炷�B
	rarray->len -= length;
	//�󂢂����������[���N���A����B
	memset(&rarray->data[rarray->elt_size * rarray->len], 0, (rarray->elt_size * length));
	//�]���ȃ��������J������B
	return (GArray*)g_array_maybe_reduce(rarray);
}
/*--------------------------------------------------------------------------*/
//Sorts a GArray using compare_func which should be a qsort()-style comparison function (returns less than zero for first arg is less than second arg, zero for equal, greater zero if first arg is greater than second arg).
//This is guaranteed to be a stable sort since version 2.32.									�˓������ł͈���\�[�g�ł͂���܂���B
//Parameters
//		array			A GArray.
//		compare_func		Comparison function.
void g_array_sort(GArray* array, int (*compare_func)(const void* a, const void* b)) {
	g_array_sort_with_data(array, (int (*)(const void*,const void*,void*))compare_func, NULL);
}
/*--------------------------------------------------------------------------*/
//Like g_array_sort(), but the comparison function receives an extra user data argument.
//This is guaranteed to be a stable sort since version 2.32.									�˓������ł͈���\�[�g�ł͂���܂���B
//There used to be a comment here about making the sort stable by using the addresses of the elements in the comparison function.
//This did not actually work, so any such code should be removed.
//		array			A GArray.
//		compare_func		Comparison function.
//		user_data		Data to pass to compare_func.
void g_array_sort_with_data(GArray* array, int (*compare_func)(const void* a, const void* b, void* user_data), void* user_data) {
	GRealArray* rarray = (GRealArray*)array;
	qsort_r(rarray->data, rarray->len, rarray->elt_size, compare_func, user_data);
}
/*--------------------------------------------------------------------------*/
//Sets the size of the array, expanding it if necessary.
//If the array was created with clear_ set to TRUE, the new elements are set to 0.
//Parameters
//		array			A GArray.
//		length			The new size of the GArray.
//Returns
//		The GArray.
GArray* g_array_set_size(GArray* array, int length) {
	GRealArray* rarray = (GRealArray*)array;
	if(length < 0) { DIE(); }
	//�����Ȃ�΁c
	if(length > rarray->len) {
		g_array_maybe_expand(rarray, length - rarray->len);
		rarray->len = length;
	//����,����,�ω����Ȃ��Ȃ�΁c
	} else {
		g_array_remove_range((GArray*)rarray, length, rarray->len - length);	//(length==rarray->len)�������ꍇ�����̏������Ăяo���Ĉ��S�ł��B
	}
	return (GArray*)rarray;
}
/*--------------------------------------------------------------------------*/
//Sets a function to clear an element of array.
//The clear_func will be called when an element in the array data segment is removed and when the array is freed and data segment is deallocated as well.
//Note that in contrast with other uses of GDestroyNotify functions, clear_func is expected to clear the contents of the array element it is given, but not free the element itself.
//Parameters
//		array			A GArray.
//		clear_func		A function to clear an element of array.
void g_array_set_clear_func(GArray* array, void (*clear_func)(void* data)) {
	GRealArray* rarray = (GRealArray*)array;
//{{2016/04/20�ǉ�:g_array_set_clear_func()�ɏ�����ǉ����A�v�f���폜����֐�(�A�v���P�[�V������`)�Ƃ���free�֐��̃A�h���X���w�肳�ꂽ�ꍇ�̓G���[��~����悤�ɂ��܂����B
#ifdef  GC_LEAK_DETECTOR_H
	// * Wed Apr 20 10:51:26 JST 2016 Naoyuki Sawa
	// - g_array_set_clear_func()�ɏ�����ǉ����A�v�f���폜����֐�(�A�v���P�[�V������`)�Ƃ���free�֐��̃A�h���X���w�肳�ꂽ�ꍇ�̓G���[��~����悤�ɂ��܂����B
	//   �Ⴆ�΁A�ȉ��̂悤�ȌĂяo���̓o�O�ł���A��������o���邽�߂ł��B
	//   ��void test() {
	//   ��  GPtrArray* array = g_ptr_array_new_with_free_func(free);
	//   ��  g_ptr_array_add(array, strdup("FOO"));
	//   ��  g_ptr_array_unref(array);	�������ňُ퓮��ƂȂ�B
	//   ��}
	//   �Ȃ��Ȃ�΁Astrdup()�̌Ăяo����'gc/leak_detector.h'�̃}�N���֐���`�ɂ����GC_debug_strdup()�ɒu���������܂����A
	//   g_ptr_array_new_with_free_func()�Ɉ����n��free�͊֐��Ăяo���`���ł͂Ȃ��̂Ń}�N���֐��̒u�������ΏۂƂ͂Ȃ炸�AGC_debug_free�̊֐��A�h���X�ł͂Ȃ��{����free�̊֐��A�h���X�ɂȂ��Ă��܂�����ł��B
	//   ���ʂƂ��āAGC_debug_strdup()�Ŋm�ۂ�����������{����free()�ŊJ�����悤�Ƃ��Ă��܂��A�ُ퓮��ƂȂ�܂��B
	// - �����Ă̈�́A'gc/leak_detector.h'�̃}�N���֐���`�����������āA�}�N���֐��ł͂Ȃ��V���{���ł��u�������������悤�ɂ��Ă��܂����ł����A��������������͓����������܂����B
	//   'gc/leak_detector.h'����C���N���[�h�����'gc/gc.h'���A�}�N���֐��ɒǉ��̈���GC_EXTRAS��n���Ă���A�����V���{���Œu��������}�N���ɏ���������Ƃ��ꂪ�����o���Ȃ��Ȃ邩��ł��B
	// - �d���������̂ŁA�A�v���P�[�V�����ɓ��ߓI�ȉ����Ă͂�����߂āA�A�v���P�[�V��������L�̂悤�ȃR�[�h�������Ă��܂������ɑ��G���[��~���Č��o������@����鎖�ɂ��܂����B
	//   �����ŃG���[��~�����ꍇ�A�A�v���P�[�V�������O�q�̂悤�ȏ������s���Ă���\���������̂ŁA�T���o���Ĉȉ��̂悤�ɕύX���ĉ������B
	//   ��static void user_free(void* ptr) { free(ptr);//����̓}�N���֐��Ƃ��Ēu����������B }
	//   ��void test() {
	//   ��  GPtrArray* array = g_ptr_array_new_with_free_func(user_free);
	//   ��  g_ptr_array_add(array, strdup("FOO"));
	//   ��  g_ptr_array_unref(array);
	//   ��}
	if(clear_func == free) { DIE(); }
#endif//GC_LEAK_DETECTOR_H
//}}2016/04/20�ǉ�:g_array_set_clear_func()�ɏ�����ǉ����A�v�f���폜����֐�(�A�v���P�[�V������`)�Ƃ���free�֐��̃A�h���X���w�肳�ꂽ�ꍇ�̓G���[��~����悤�ɂ��܂����B
	rarray->clear_func = clear_func;
}
/*--------------------------------------------------------------------------*/
//Frees the memory allocated for the GArray.
//If free_segment is TRUE it frees the memory block holding the elements as well and also each element if array has a element_free_func set.
//Pass FALSE if you want to free the GArray wrapper but preserve the underlying array for use elsewhere.
//If the reference count of array is greater than one, the GArray wrapper is preserved but the size of array will be set to zero.
//Parameters
//		array			A GArray.
//		free_segment		If TRUE the actual element data is freed as well.
//Returns
//		The element data if free_segment is FALSE, otherwise NULL.
//		The element data should be freed using g_free().
char* g_array_free(GArray* array, int free_segment) {
	GRealArray* rarray = (GRealArray*)array;
	char* segment;
	if(!rarray->ref_count /*|| (rarray->ref_count > USHRT_MAX)*/) { DIE(); }
	//�߂�l���f�[�^�̃������Ƃ��Ă����B
	segment = rarray->data;
	//�f�[�^�̃��������J������Ȃ�΁c
	if(free_segment) {
		int i;
		//�v�f���폜���鎞�ɌĂяo�����[�J���֐������s����B
		for(i = 0; i < rarray->len; i++) {
			g_array_on_remove_element(rarray, i);
		}
		//�f�[�^�̃��������J������B
		free(rarray->data);
		//�߂�l��NULL�ɕύX����B
		segment = NULL;
	}
	//������ɂ��Ă��A�ȉ��̃t�B�[���h�̓N���A���Ă����B
	// - �Q�Ƃ��c���Ă���ꍇ�́A�����������̍\���̂��A'�f�[�^�̃��������m�ۂ���Ă��Ȃ��v�f��0�̔z��'�Ƃ��Ďg�p���邽�߂ł��B
	// - �������A���̏�Ԃł́A�[���I�[�����݂��Ȃ����ɒ��ӂ��Ă��������B
	//   �c���Ă���Q�Ƃ��o�R����g_array_append_val()�����Ăяo���āA���߂ăf�[�^�̃��������m�ۂ����܂ŁA���̏�Ԃ̂܂܂ł��B
	//   �ʏ�́A�c���Ă���Q�Ƃ��f�[�^���Q�Ƃ����ɁAg_array_free(),����,g_array_unref()���Ăяo���������Ǝv���܂����A�����f�[�^���Q�Ƃ��悤�Ƃ����ꍇ�́A�[���I�[�����݂��Ȃ��Ƃ������ق��L��܂��B
	//   ���̋����́A���\�[�X(glib-2.48.0)�ł������ł��B
	rarray->data        = NULL;
	rarray->len         = 0;
	rarray->alloc_bytes = 0;
	//�Q�ƃJ�E���g�����炵�A�Ō�̎Q�Ƃ������Ȃ�΁c
	if(!--rarray->ref_count) {
		//�\���̂̃��������J������B
		free(rarray);
	}
	return segment;
}
/*****************************************************************************
 *	����J�֐�
 *****************************************************************************/
//���\�[�X�ł�g_array_index()�̓}�N�������ō쐬����Ă��邪�A�������ł̓}�N���Ɗ֐����g���č쐬���鎖�ɂ����B
//�֐����g�����ɂ������R�́Ag_array_index()�͎g�p�p�x�������̂ŁAg_array_index()��index�͈̔͌������s�����ŁA�o�O�����o�o����@������Ǝv��������ł��B
void* _g_array_index(GArray* array, int type_size, int index) {
	GRealArray* rarray = (GRealArray*)array;
	if((type_size != rarray->elt_size) ||				//���Ԃ�Ag_array_index()�}�N���̑������Ɏw�肵���^���Ԉ���Ă���B
//{{2016/06/25�ύX:_g_array_index()�ւ�index�w����A����܂ł�(��len)�܂ł��������Ă��܂���ł������A(��len)�܂ŋ�����悤�ɕύX���܂����B
//	  ((unsigned)index >= (unsigned)rarray->len)) { DIE(); }	//g_array_index()�}�N���̑�O�����Ɏw�肵���C���f�N�X���Ԉ���Ă���B
//��2016/06/25�ύX:_g_array_index()�ւ�index�w����A����܂ł�(��len)�܂ł��������Ă��܂���ł������A(��len)�܂ŋ�����悤�ɕύX���܂����B
	  ((unsigned)index >  (unsigned)rarray->len)) { DIE(); }	//g_array_index()�}�N���̑�O�����Ɏw�肵���C���f�N�X���Ԉ���Ă���B
//}}2016/06/25�ύX:_g_array_index()�ւ�index�w����A����܂ł�(��len)�܂ł��������Ă��܂���ł������A(��len)�܂ŋ�����悤�ɕύX���܂����B
	return &rarray->data[type_size * index];
}
/*****************************************************************************
 *	GPtrArray
 *****************************************************************************/
/*****************************************************************************
 *	�O���[�o���֐�
 *****************************************************************************/
//Creates a new GPtrArray with a reference count of 1.
//Returns
//		The new GPtrArray.
GPtrArray* g_ptr_array_new() {
	return g_ptr_array_new_full(0, NULL);
}
/*--------------------------------------------------------------------------*/
//Creates a new GPtrArray with reserved_size pointers preallocated and a reference count of 1.
//This avoids frequent reallocation, if you are going to add many pointers to the array.
//Note however that the size of the array is still 0.
//Parameters
//		reserved_size		Number of pointers preallocated.
//Returns
//		The new GPtrArray.
GPtrArray* g_ptr_array_sized_new(int reserved_size) {
	return g_ptr_array_new_full(reserved_size, NULL);
}
/*--------------------------------------------------------------------------*/
//Creates a new GPtrArray with a reference count of 1 and use element_free_func for freeing each element when the array is destroyed either via g_ptr_array_unref(), when g_ptr_array_free() is called with free_segment set to TRUE or when removing elements.
//Parameters
//		element_free_func	A function to free elements with destroy array or NULL.		[allow-none]
//Returns
//		The new GPtrArray.
GPtrArray* g_ptr_array_new_with_free_func(void (*element_free_func)(void* data)) {
	return g_ptr_array_new_full(0, element_free_func);
}
/*--------------------------------------------------------------------------*/
//Creates a new GPtrArray with reserved_size pointers preallocated and a reference count of 1.
//This avoids frequent reallocation, if you are going to add many pointers to the array.
//Note however that the size of the array is still 0.
//It also set element_free_func for freeing each element when the array is destroyed either via g_ptr_array_unref(), when g_ptr_array_free() is called with free_segment set to TRUE or when removing elements.
//Parameters
//		reserved_size		Number of pointers preallocated.
//		element_free_func	A function to free elements with destroy array or NULL.		[allow-none]
//Returns
//		The new GPtrArray.
GPtrArray* g_ptr_array_new_full(int reserved_size, void (*element_free_func)(void* data)) {
	GPtrArray* array = (GPtrArray*)g_array_sized_new(0, 0, sizeof(void*), reserved_size);
	((GRealArray*)array)->array_type = ARRAY_TYPE_G_PTR_ARRAY;	//�z��̎�ނ��i�[����B
	g_ptr_array_set_free_func(array, element_free_func);
	return array;
}
/*--------------------------------------------------------------------------*/
//Sets a function for freeing each element when array is destroyed either via g_ptr_array_unref(), when g_ptr_array_free() is called with free_segment set to TRUE or when removing elements.
//Parameters
//		array			A GPtrArray.
//		element_free_func	A function to free elements with destroy array or NULL.		[allow-none]
void g_ptr_array_set_free_func(GPtrArray* array, void (*element_free_func)(void* data)) {
	g_array_set_clear_func((GArray*)array, element_free_func);
}
/*--------------------------------------------------------------------------*/
//Atomically increments the reference count of array by one.
//This function is thread-safe and may be called from any thread.								�˓������ł̓X���b�h�Z�[�t�ł͂���܂���B�����̃X���b�h����Q�Ƃ��鎖�͖����Ǝv��������ł��B���ۂ̏�g_array_ref()/g_array_unref()�������X���b�h�Z�[�t�ɂ��Ă����̊֐����X���b�h�Z�[�t�łȂ���΂��܂�Ӗ��͖����Ǝv���܂��B
//Parameters
//		array			A GPtrArray.
//Returns
//		The passed in GPtrArray.
GPtrArray* g_ptr_array_ref(GPtrArray* array) {
	return (GPtrArray*)g_array_ref((GArray*)array);
}
/*--------------------------------------------------------------------------*/
//Atomically decrements the reference count of array by one.
//If the reference count drops to 0, the effect is the same as calling g_ptr_array_free() with free_segment set to TRUE.
//This function is MT-safe and may be called from any thread.
//Parameters
//		array			A GPtrArray.
void g_ptr_array_unref(GPtrArray* array) {
	g_array_unref((GArray*)array);
}
/*--------------------------------------------------------------------------*/
//Adds a pointer to the end of the pointer array.
//The array will grow in size automatically if necessary.
//Parameters
//		array			A GPtrArray.
//		data			The pointer to add.
void g_ptr_array_add(GPtrArray* array, void* data) {
	g_array_append_val((GArray*)array, data);
}
/*--------------------------------------------------------------------------*/
//Inserts an element into the pointer array at the given index.
//The array will grow in size automatically if necessary.
//Parameters
//		array			A GPtrArray.
//		index			The index to place the new element at, or -1 to append.					//g_array_insert_�`()��index��-1���w��o���Ȃ����Ag_ptr_array_insert()��index��-1���w��o����Ƃ���GLib�̊֐��d�l�ł��B���̂��������d�l�Ȃ̂��s�v�c�Ȃ̂ł����A��������GLib�̊֐��d�l�ɏ]���Ă������ɂ��܂����B
//		data			The pointer to add.
void g_ptr_array_insert(GPtrArray* array, int index, void* data) {
	if(index == -1) {
		g_array_append_val((GArray*)array, data);
	} else {
		g_array_insert_val((GArray*)array, index, data);
	}
}
/*--------------------------------------------------------------------------*/
//Removes the first occurrence of the given pointer from the pointer array.
//The following elements are moved down one place.
//If array has a non-NULL GDestroyNotify function it is called for the removed element.
//It returns TRUE if the pointer was removed, or FALSE if the pointer was not found.
//Parameters
//		array			A GPtrArray.
//		data			The pointer to remove.
//Returns
//		TRUE if the pointer is removed, FALSE if the pointer is not found in the array.
int g_ptr_array_remove(GPtrArray* array, void* data) {
	int i;
	for(i = 0; i < array->len; i++) {
		if(array->pdata[i] == data) {
			g_ptr_array_remove_index(array, i);
			return 1;
		}
	}
	return 0;
}
/*--------------------------------------------------------------------------*/
//Removes the pointer at the given index from the pointer array.
//The following elements are moved down one place.
//If array has a non-NULL GDestroyNotify function it is called for the removed element.
//Parameters
//		array			A GPtrArray.
//		index			The index of the pointer to remove.
//Returns
//		The pointer which was removed.
void* g_ptr_array_remove_index(GPtrArray* array, int index) {
	void* result = g_ptr_array_index((GArray*)array, index);
	g_array_remove_index((GArray*)array, index);
	return result;
}
/*--------------------------------------------------------------------------*/
//Removes the first occurrence of the given pointer from the pointer array.
//The last element in the array is used to fill in the space, so this function does not preserve the order of the array.
//But it is faster than g_ptr_array_remove().
//If array has a non-NULL GDestroyNotify function it is called for the removed element.
//It returns TRUE if the pointer was removed, or FALSE if the pointer was not found.
//Parameters
//		array			A GPtrArray.
//		data			The pointer to remove.
//Returns
//		TRUE if the pointer was found in the array.
int g_ptr_array_remove_fast(GPtrArray* array, void* data) {
	int i;
	for(i = 0; i < array->len; i++) {
		if(array->pdata[i] == data) {
			g_ptr_array_remove_index_fast(array, i);
			return 1;
		}
	}
	return 0;
}
/*--------------------------------------------------------------------------*/
//Removes the pointer at the given index from the pointer array.
//The last element in the array is used to fill in the space, so this function does not preserve the order of the array.
//But it is faster than g_ptr_array_remove_index().
//If array has a non-NULL GDestroyNotify function it is called for the removed element.
//Parameters
//		array			A GPtrArray.
//		index			The index of the pointer to remove.
//Returns
//		The pointer which was removed.
void* g_ptr_array_remove_index_fast(GPtrArray* array, int index) {
	void* result = g_ptr_array_index((GArray*)array, index);
	g_array_remove_index_fast((GArray*)array, index);
	return result;
}
/*--------------------------------------------------------------------------*/
//Removes the given number of pointers starting at the given index from a GPtrArray.
//The following elements are moved to close the gap.
//If array has a non-NULL GDestroyNotify function it is called for the removed elements.
//Parameters
//		array			A GPtrArray.
//		index			The index of the first pointer to remove.
//		length			The number of pointers to remove.
//Returns
//		The array.
GPtrArray* g_ptr_array_remove_range(GPtrArray* array, int index, int length) {
	return (GPtrArray*)g_array_remove_range((GArray*)array, index, length);
}
/*--------------------------------------------------------------------------*/
//Sorts the array, using compare_func which should be a qsort()-style comparison function (returns less than zero for first arg is less than second arg, zero for equal, greater than zero if irst arg is greater than second arg).
//Note that the comparison function for g_ptr_array_sort() doesn't take the pointers from the array as arguments, it takes pointers to the pointers in the array.		�����������d�v!!��r�֐��ɂ̓|�C���^���n�����̂ł͂Ȃ��A�|�C���^�ւ̃|�C���^���n�����d�l�ł��鎖�ɒ��ӂ��ĉ������B�Ⴆ�΁A������|�C���^���i�[�����z����\�[�g����ہA��r�֐��Ƃ���strcmp()���w�肷�鎖�͏o���܂���B����������
//This is guaranteed to be a stable sort since version 2.32.									�˓������ł͈���\�[�g�ł͂���܂���B
//Parameters
//		array			A GPtrArray.
//		compare_func		Comparison function.
void g_ptr_array_sort(GPtrArray* array, int (*compare_func)(const void* a, const void* b)) {
	g_array_sort((GArray*)array, compare_func);
}
/*--------------------------------------------------------------------------*/
//Like g_ptr_array_sort(), but the comparison function has an extra user data argument.
//Note that the comparison function for g_ptr_array_sort_with_data() doesn't take the pointers from the array as arguments, it takes pointers to the pointers in the array.	�����������d�v!!��r�֐��ɂ̓|�C���^���n�����̂ł͂Ȃ��A�|�C���^�ւ̃|�C���^���n�����d�l�ł��鎖�ɒ��ӂ��ĉ������B�Ⴆ�΁A������|�C���^���i�[�����z����\�[�g����ہA��r�֐��Ƃ���strcmp()���w�肷�鎖�͏o���܂���B����������
//This is guaranteed to be a stable sort since version 2.32.									�˓������ł͈���\�[�g�ł͂���܂���B
//Parameters
//		array			A GPtrArray.
//		compare_func		Comparison function.
//		user_data		Data to pass to compare_func.
void g_ptr_array_sort_with_data(GPtrArray* array, int (*compare_func)(const void* a, const void* b, void* user_data), void* user_data) {
	g_array_sort_with_data((GArray*)array, compare_func, user_data);
}
/*--------------------------------------------------------------------------*/
//Sets the size of the array.
//When making the array larger, newly-added elements will be set to NULL.
//When making it smaller, if array has a non-NULL GDestroyNotify function then it will be called for the removed elements.
//Parameters
//		array			A GPtrArray.
//		length			The new length of the pointer array.
void g_ptr_array_set_size(GPtrArray* array, int length) {
	g_array_set_size((GArray*)array, length);
}
/*--------------------------------------------------------------------------*/
//Frees the memory allocated for the GPtrArray.
//If free_seg is TRUE it frees the memory block holding the elements as well.
//Pass FALSE if you want to free the GPtrArray wrapper but preserve the underlying array for use elsewhere.
//If the reference count of array is greater than one, the GPtrArray wrapper is preserved but the size of array will be set to zero.
//If array contents point to dynamically-allocated memory, they should be freed separately if free_seg is TRUE and no GDestroyNotify function has been set for array.
//Parameters
//		array			A GPtrArray.
//		free_seg		If TRUE the actual pointer array is freed as well.
//Returns
//		The pointer array if free_seg is FALSE, otherwise NULL.
//		The pointer array should be freed using g_free().
void** g_ptr_array_free(GPtrArray* array, int free_seg) {
	return (void**)g_array_free((GArray*)array, free_seg);
}
/*--------------------------------------------------------------------------*/
//Calls a function for each element of a GPtrArray.
//Parameters
//		array			A GPtrArray.
//		func			The function to call for each array element.
//		user_data		User data to pass to the function.
void g_ptr_array_foreach(GPtrArray* array, void (*func)(void* data, void* user_data), void* user_data) {
	int i;
	for(i = 0; i < array->len; i++) {
		(*func)(array->pdata[i], user_data);
	}
}
/*****************************************************************************
 *	GByteArray
 *****************************************************************************/
/*****************************************************************************
 *	�O���[�o���֐�
 *****************************************************************************/
//Creates a new GByteArray with a reference count of 1.
//Returns
//		The new GByteArray.	[transfer full]										//[transfer full]�Ƃ�'Free data after the code is done.'�̎��ł�(https://developer.gnome.org/glib/stable/annotation-glossary.html)�B�������Ahttps://developer.gnome.org/glib/stable/glib-Byte-Arrays.html �̐����ł͂ǂ���[transfer full]�̎w��Ɉ�ѐ��������悤�ȋC������̂ŁA���܂�C�ɂ��Ȃ������ǂ������m��܂���B������ǂ߂ΒN�����������J�����邩�͑�̒��ϒʂ�ł��B
GByteArray* g_byte_array_new() {
	return g_byte_array_sized_new(0);
}
/*--------------------------------------------------------------------------*/
//Create byte array containing the data.
//The data will be owned by the array and will be freed with g_free(), i.e. it could be allocated using g_strdup().
//Parameters
//		data			Byte data for the array.	[transfer full][array length=len]
//		len			Length of data.
//Returns
//		A new GByteArray.	[transfer full]
GByteArray* g_byte_array_new_take(uint8_t* data, int len) {
	GRealArray* rarray = (GRealArray*)g_byte_array_new();
	free(rarray->data);
	rarray->data        = data;
	rarray->len         = len;
	rarray->alloc_bytes = len;
	return (GByteArray*)rarray;
}
/*--------------------------------------------------------------------------*/
//Creates a new GByteArray with reserved_size bytes preallocated.
//This avoids frequent reallocation, if you are going to add many bytes to the array.
//Note however that the size of the array is still 0.
//Parameters
//		reserved_size		Number of bytes preallocated.
//Returns
//		The new GByteArray.
GByteArray* g_byte_array_sized_new(int reserved_size) {
	GByteArray* array = (GByteArray*)g_array_sized_new(0, 0, 1, reserved_size);
	((GRealArray*)array)->array_type = ARRAY_TYPE_G_BYTE_ARRAY;	//�z��̎�ނ��i�[����B
	return array;
}
/*--------------------------------------------------------------------------*/
//Atomically increments the reference count of array by one.
//This function is thread-safe and may be called from any thread.
//Parameters
//		array			A GByteArray.
//Returns
//		The passed in GByteArray.
GByteArray* g_byte_array_ref(GByteArray* array) {
	return (GByteArray*)g_array_ref((GArray*)array);
}
/*--------------------------------------------------------------------------*/
//Atomically decrements the reference count of array by one.
//If the reference count drops to 0, all memory allocated by the array is released.
//This function is thread-safe and may be called from any thread.
//Parameters
//		array			A GByteArray.
void g_byte_array_unref(GByteArray* array) {
	g_array_unref((GArray*)array);
}
/*--------------------------------------------------------------------------*/
//Adds the given bytes to the end of the GByteArray.
//The array will grow in size automatically if necessary.
//Parameters
//		array			A GByteArray.
//		data			The byte data to be added.
//		len			the number of bytes to add.
//Returns
//		The GByteArray.
GByteArray* g_byte_array_append(GByteArray* array, const uint8_t* data, int len) {
	return (GByteArray*)g_array_append_vals((GArray*)array, data, len);
}
/*--------------------------------------------------------------------------*/
//Adds the given data to the start of the GByteArray.
//The array will grow in size automatically if necessary.
//Parameters
//		array			A GByteArray.
//		data			The byte data to be added.
//		len			The number of bytes to add.
//Returns
//		The GByteArray.
GByteArray* g_byte_array_prepend(GByteArray* array, const uint8_t* data, int len) {
	return (GByteArray*)g_array_prepend_vals((GArray*)array, data, len);
}
/*--------------------------------------------------------------------------*/
//Removes the byte at the given index from a GByteArray.
//The following bytes are moved down one place.
//Parameters
//		array			A GByteArray.
//		index			The index of the byte to remove.
//Returns
//		The GByteArray.
GByteArray* g_byte_array_remove_index(GByteArray* array, int index) {
	return (GByteArray*)g_array_remove_index((GArray*)array, index);
}
/*--------------------------------------------------------------------------*/
//Removes the byte at the given index from a GByteArray.
//The last element in the array is used to fill in the space, so this function does not preserve the order of the GByteArray.
//But it is faster than g_byte_array_remove_index().
//Parameters
//		array			A GByteArray.
//		index			The index of the byte to remove.
//Returns
//		The GByteArray.
GByteArray* g_byte_array_remove_index_fast(GByteArray* array, int index) {
	return (GByteArray*)g_array_remove_index_fast((GArray*)array, index);
}
/*--------------------------------------------------------------------------*/
//Removes the given number of bytes starting at the given index from a GByteArray.
//The following elements are moved to close the gap.
//Parameters
//		array			A GByteArray.
//		index			The index of the first byte to remove.
//		length			The number of bytes to remove.
//Returns
//		The GByteArray.
GByteArray* g_byte_array_remove_range(GByteArray* array, int index, int length) {
	return (GByteArray*)g_array_remove_range((GArray*)array, index, length);
}
/*--------------------------------------------------------------------------*/
//Sorts a byte array, using compare_func which should be a qsort()-style comparison function (returns less than zero for first arg is less than second arg, zero for equal, greater than zero if first arg is greater than second arg).
//If two array elements compare equal, their order in the sorted array is undefined.
//If you want equal elements to keep their order (i.e. you want a stable sort) you can write a comparison function that, if two elements would otherwise compare equal, compares them by their addresses.
//Parameters
//		array			A GByteArray.
//		compare_func		Comparison function.
void g_byte_array_sort(GByteArray* array, int (*compare_func)(const void* a, const void* b)) {
	g_array_sort((GArray*)array, compare_func);
}
/*--------------------------------------------------------------------------*/
//Like g_byte_array_sort(), but the comparison function takes an extra user data argument.
//Parameters
//		array			A GByteArray.
//		compare_func		Comparison function.
//		user_data		data to pass to compare_func.
void g_byte_array_sort_with_data(GByteArray* array, int (*compare_func)(const void* a, const void* b, void* user_data), void* user_data) {
	g_array_sort_with_data((GArray*)array, compare_func, user_data);
}
/*--------------------------------------------------------------------------*/
//Sets the size of the GByteArray, expanding it if necessary.
//Parameters
//		array			A GByteArray
//		length			The new size of the GByteArray
//Returns
//		The GByteArray.
GByteArray* g_byte_array_set_size(GByteArray* array, int length) {
	return (GByteArray*)g_array_set_size((GArray*)array, length);
}
/*--------------------------------------------------------------------------*/
//Frees the memory allocated by the GByteArray.
//If free_segment is TRUE it frees the actual byte data.
//If the reference count of array is greater than one, the GByteArray wrapper is preserved but the size of array will be set to zero.
//Parameters
//		array			A GByteArray.
//		free_segment		If TRUE the actual byte data is freed as well.
//Returns
//		The element data if free_segment is FALSE, otherwise NULL.
//		The element data should be freed using g_free().
uint8_t* g_byte_array_free(GByteArray* array, int free_segment) {
	return (uint8_t*)g_array_free((GArray*)array, free_segment);
}
