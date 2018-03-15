/*
 *	garray.h
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
 *	* Sat Oct 15 21:50:56 JST 2016 Naoyuki Sawa
 *	- g_array_insert_val()�̃}�N����`���Ԉ���ċ�ɂȂ��Ă����̂��C�����܂����B
 *	  ���̂��߁Ag_array_insert_val()���Ăяo������Ag_ptr_array_insert()�̃C���f�N�X��-1�ȊO���w�肵�����ɁA�����������s���Ă��܂���ł����B
 *	  ����܂ŁA���܂��܁A��L�̃P�[�X�ɑ������鏈�����s���Ă��炸�A�C���t���Ă��܂���ł����B
 *	  ����A�A�v���P�[�V���������҂̌Ăяo�����s�����ӏ��Ŗ�肪���݉����āA�C���t���܂����B
 */
#ifndef __GARRAY_H__
#define __GARRAY_H__
#ifdef  __cplusplus
extern "C" {
#endif//__cplusplus
/*****************************************************************************
 *	GArray
 *****************************************************************************/
/*****************************************************************************
 *	�\����
 *****************************************************************************/
typedef struct _GArray {
	void*		data;		//A pointer to the element data. The data may be moved as elements are added to the GArray.
	int		len;		//The number of elements in the GArray not including the possible terminating zero element.
} GArray;
/*****************************************************************************
 *	�}�N��
 *****************************************************************************/
//Adds the value on to the end of the array.
//The array will grow in size automatically if necessary.
//g_array_append_val() is a macro which uses a reference to the value parameter v.
//This means that you cannot use it with literal values such as "27".
//You must use variables.
//Parameters
//		a			A GArray.
//		v			The value to append to the GArray.
//Returns
//		The GArray.
#define g_array_append_val(a,v)		g_array_append_vals((a),&(v),1)
/*--------------------------------------------------------------------------*/
//Adds the value on to the start of the array. The array will grow in size automatically if necessary.
//This operation is slower than g_array_append_val() since the existing elements in the array have to be moved to make space for the new element.
//g_array_prepend_val() is a macro which uses a reference to the value parameter v.
//This means that you cannot use it with literal values such as "27". You must use variables.
//Parameters
//		a			A GArray.
//		v			the value to prepend to the GArray.
//Returns
//		The GArray.
#define g_array_prepend_val(a,v)	g_array_prepend_vals((a),&(v),1)
/*--------------------------------------------------------------------------*/
//Inserts an element into an array at the given index.
//g_array_insert_val() is a macro which uses a reference to the value parameter v.
//This means that you cannot use it with literal values such as "27".
//You must use variables.
//Parameters
//		a			A GArray.
//		i			The index to place the element at.
//		v			The value to insert into the array.
//Returns
//		The GArray.
#define g_array_insert_val(a,i,v)	g_array_insert_vals((a),(i),&(v),1)							//{{2016/10/15�C��:g_array_insert_val()�̃}�N����`���Ԉ���ċ�ɂȂ��Ă����̂��C�����܂����B}}
/*--------------------------------------------------------------------------*/
//Returns the element of a GArray at the given index.
//The return value is cast to the given type.
//This example gets a pointer to an element in a GArray:
//��EDayViewEvent* event;
//��//This gets a pointer to the 4th element in the array of EDayViewEvent structs.
//��event = &g_array_index(events, EDayViewEvent, 3);
//Parameters
//		a			A GArray.
//		t			The type of the elements.
//		i			The index of the element to return.
//Returns
//		The element of the GArray at the index given by i.
#define g_array_index(a,t,i)	(*(t*)_g_array_index((a),sizeof(t),(i)))							//�����\�[�X�ł�g_array_index()�̓}�N�������ō쐬����Ă��邪�A�������ł̓}�N���Ɗ֐����g���č쐬���鎖�ɂ����B
void* _g_array_index(GArray* array, int type_size, int index);	//����J�֐�							//���֐����g�����ɂ������R�́Ag_array_index()�͎g�p�p�x�������̂ŁAg_array_index()��index�͈̔͌������s�����ŁA�o�O�����o�o����@������Ǝv��������ł��B
/*****************************************************************************
 *	�O���[�o���֐�
 *****************************************************************************/
GArray* g_array_new(int zero_terminated, int clear, int element_size);
GArray* g_array_sized_new(int zero_terminated, int clear, int element_size, int reserved_size);
GArray* g_array_ref(GArray* array);
void g_array_unref(GArray* array);
int g_array_get_element_size(GArray* array);
GArray* g_array_append_vals(GArray* array, const void* data, int len);
GArray* g_array_prepend_vals(GArray* array, const void* data, int len);
GArray* g_array_insert_vals(GArray* array, int index, const void* data, int len);
GArray* g_array_remove_index(GArray* array, int index);
GArray* g_array_remove_index_fast(GArray* array, int index);
GArray* g_array_remove_range(GArray* array, int index, int length);
void g_array_sort(GArray* array, int (*compare_func)(const void* a, const void* b));
void g_array_sort_with_data(GArray* array, int (*compare_func)(const void* a, const void* b, void* user_data), void* user_data);
GArray* g_array_set_size(GArray* array, int length);
void g_array_set_clear_func(GArray* array, void (*clear_func)(void* data));
char* g_array_free(GArray* array, int free_segment);
/*****************************************************************************
 *	GPtrArray
 *****************************************************************************/
/*****************************************************************************
 *	�\����
 *****************************************************************************/
typedef struct _GPtrArray {
	void**		pdata;		//Points to the array of pointers, which may be moved when the array grows.
	int		len;		//Number of pointers in the array.
} GPtrArray;
/*****************************************************************************
 *	�}�N��
 *****************************************************************************/
//Returns the pointer at the given index of the pointer array.
//This does not perform bounds checking on the given index, so you are responsible for checking it against the array length.
//Parameters
//		array			A GPtrArray.
//		index			The index of the pointer to return.
//Returns
//		The pointer at the given index.
#define g_ptr_array_index(array,index)	g_array_index((GArray*)(array),void*,(index))
/*****************************************************************************
 *	�O���[�o���֐�
 *****************************************************************************/
GPtrArray* g_ptr_array_new();
GPtrArray* g_ptr_array_sized_new(int reserved_size);
GPtrArray* g_ptr_array_new_with_free_func(void (*element_free_func)(void* data));
GPtrArray* g_ptr_array_new_full(int reserved_size, void (*element_free_func)(void* data));
void g_ptr_array_set_free_func(GPtrArray* array, void (*element_free_func)(void* data));
GPtrArray* g_ptr_array_ref(GPtrArray* array);
void g_ptr_array_unref(GPtrArray* array);
void g_ptr_array_add(GPtrArray* array, void* data);
void g_ptr_array_insert(GPtrArray* array, int index, void* data);
int g_ptr_array_remove(GPtrArray* array, void* data);
void* g_ptr_array_remove_index(GPtrArray* array, int index);
int g_ptr_array_remove_fast(GPtrArray* array, void* data);
void* g_ptr_array_remove_index_fast(GPtrArray* array, int index);
GPtrArray* g_ptr_array_remove_range(GPtrArray* array, int index, int length);
void g_ptr_array_sort(GPtrArray* array, int (*compare_func)(const void* a, const void* b));
void g_ptr_array_sort_with_data(GPtrArray* array, int (*compare_func)(const void* a, const void* b, void* user_data), void* user_data);
void g_ptr_array_set_size(GPtrArray* array, int length);
void** g_ptr_array_free(GPtrArray* array, int free_seg);
void g_ptr_array_foreach(GPtrArray* array, void (*func)(void* data, void* user_data), void* user_data);
/*****************************************************************************
 *	GByteArray
 *****************************************************************************/
/*****************************************************************************
 *	�}�N��
 *****************************************************************************/
//GLib�̊֐��d�l�ŁA���̂�g_byte_array_index()�͒�`����Ă��܂���B
//GByteArray.data[]�o�R�ŃA�N�Z�X�o���邩�炩�Ƃ��l�����̂ł����A����Ȃ��g_ptr_array_index()���s�v�Ȃ͂��ł���Ag_ptr_array_index()����`����Ă��闝�R�������o���܂���B
//g_ptr_array_index()����`����Ă���̂ɁAg_byte_array_index()�͒�`����Ă��Ȃ����R���s���ł��B(�����������炠�܂�[���Ӗ��͖����̂����m��܂��񂪁c)
//�Ƃ肠�������������AGLib�̊֐��d�l�ɏ]���Ă������ɂ��܂����B
/*****************************************************************************
 *	�\����
 *****************************************************************************/
typedef struct _GByteArray {
	uint8_t*	data;		//A pointer to the element data. The data may be moved as elements are added to the GByteArray.
	int		len;		//The number of elements in the GByteArray.
} GByteArray;
/*****************************************************************************
 *	�O���[�o���֐�
 *****************************************************************************/
GByteArray* g_byte_array_new();
GByteArray* g_byte_array_new_take(uint8_t* data, int len);
GByteArray* g_byte_array_sized_new(int reserved_size);
GByteArray* g_byte_array_ref(GByteArray* array);
void g_byte_array_unref(GByteArray* array);
GByteArray* g_byte_array_append(GByteArray* array, const uint8_t* data, int len);
GByteArray* g_byte_array_prepend(GByteArray* array, const uint8_t* data, int len);
GByteArray* g_byte_array_remove_index(GByteArray* array, int index);
GByteArray* g_byte_array_remove_index_fast(GByteArray* array, int index);
GByteArray* g_byte_array_remove_range(GByteArray* array, int index, int length);
void g_byte_array_sort(GByteArray* array, int (*compare_func)(const void* a, const void* b));
void g_byte_array_sort_with_data(GByteArray* array, int (*compare_func)(const void* a, const void* b, void* user_data), void* user_data);
GByteArray* g_byte_array_set_size(GByteArray* array, int length);
uint8_t* g_byte_array_free(GByteArray* array, int free_segment);
//�ȉ��͖��Ή��ł��B
//GBytes* g_byte_array_free_to_bytes(GByteArray* array);
//GBytes* g_bytes_new(const void* data, size_t size);
//GBytes* g_bytes_new_take(void* data, size_t size);
//GBytes* g_bytes_new_static(const void* data, size_t size);
//GBytes* g_bytes_new_with_free_func(const void* data, size_t size, void (*free_func)(void* user_data), void* user_data);
//GBytes* g_bytes_new_from_bytes(GBytes* bytes, size_t offset, size_t length);
//const void* g_bytes_get_data(GBytes* bytes, size_t* size);
//size_t g_bytes_get_size(GBytes* bytes);
//int g_bytes_hash(const void* bytes);
//gboolean g_bytes_equal(const void* bytes1, const void* bytes2);
//int g_bytes_compare(const void* bytes1, const void* bytes2);
//GBytes* g_bytes_ref(GBytes* bytes);
//void g_bytes_unref(GBytes* bytes);
//void* g_bytes_unref_to_data(GBytes* bytes, size_t* size);
//GByteArray* g_bytes_unref_to_array(GBytes* bytes);
#ifdef  __cplusplus
}//extern "C"
#endif//__cplusplus
#endif//__GARRAY_H__
