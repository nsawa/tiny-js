/*
 *	gslist.h
 *
 *	Singly-Linked Lists - linked lists that can be iterated in one direction
 *
 *	* Wed Jul 27 21:35:30 JST 2016 Naoyuki Sawa
 *	- 1st �����[�X�B
 *	- GLib��Singly-Linked Lists(https://developer.gnome.org/glib/stable/glib-Singly-Linked-Lists.html)���Q�l�ɂ��܂����B
 *	  �֐��d�l�̓I���W�i���łƓ����ł����A�����͑����ύX���āA�������x�����e�ʍ팸��D�悵�܂����B
 *	- ���ۂ̃A�v���P�[�V�����ł́A�ϋɓI��GSList���g�����͏��Ȃ��Ǝv���܂��B
 *	  GArray��GTree��ght_hash_table�����g�������A���x�I�ɂ��g���Ղ��I�ɂ��D��Ă��邩��ł��B
 *	  GSList���D��Ă���_�́A��̃��X�g��NULL�ŕ\�����鎖���o���āA���������s�v�ł��鎖���炢���Ǝv���܂��B
 *	* Tue Oct 03 23:59:59 JST 2017 Naoyuki Sawa
 *	- g_slist_compare_np(),g_slist_compare_custom_np()��ǉ����܂����B
 */
#ifndef __G_SLIST_H__
#define __G_SLIST_H__
#ifdef  __cplusplus
extern "C" {
#endif//__cplusplus
/*****************************************************************************
 *	GSList
 *****************************************************************************/
/*****************************************************************************
 *	�\����
 *****************************************************************************/
typedef struct _GSList {
	void*			data;		//Holds the element's data, which can be a pointer to any kind of data, or any integer value using the Type Conversion Macros.
	struct _GSList*		next;		//Contains the link to the next element in the list.
} GSList;
/*****************************************************************************
 *	�O���[�o���֐�
 *****************************************************************************/
GSList* g_slist_alloc();
GSList* g_slist_append(GSList* list/*NULL��*/, void* data);
GSList* g_slist_prepend(GSList* list/*NULL��*/, void* data);
GSList* g_slist_insert(GSList* list/*NULL��*/, void* data, int position);
GSList* g_slist_insert_before(GSList* slist/*NULL��*/, GSList* sibling/*NULL��*/, void* data);
GSList* g_slist_insert_sorted(GSList* list/*NULL��*/, void* data, int (*compare_func)(const void* a, const void* b));
GSList* g_slist_insert_sorted_with_data(GSList* list/*NULL��*/, void* data, int (*compare_func)(const void* a, const void* b, void* user_data), void* user_data);
GSList* g_slist_remove(GSList* list/*NULL��*/, const void* data);	//��
GSList* g_slist_remove_all(GSList* list/*NULL��*/, const void* data);	//�������Ӂ��֐����ɑΏ̐��������Bg_slist_remove()��g_slist_remove_all()�̓m�[�h���폜���邪�Ag_slist_remove_link()�̓m�[�h���폜�����Ag_slist_delete_link()���m�[�h���폜����B
GSList* g_slist_remove_link(GSList* list/*NULL��*/, GSList* link_);	//��
GSList* g_slist_delete_link(GSList* list/*NULL��*/, GSList* link_);	//��			//'link'�ł͂Ȃ�'link_'�Ƃ������R�́A�I���W�i���ł̎����ɕ��������ł��B�����A�I���W�i���ł̎����ł́A�����ʂ̃��W���[���Ɩ��O���Փ˂��Ă����̂��Ǝv���܂��B�������ł�'link'�ł����v�Ȃ̂ł����A�I���W�i���ł̎����ɕ����'link_'�̂܂܂ɂ��鎖�ɂ��܂����B
void g_slist_free(GSList* list/*NULL��*/);
void g_slist_free_full(GSList* list/*NULL��*/, void (*free_func_/*NULL��*/)(void* data));	//'free_func'�ł͂Ȃ�'free_func_'�Ƃ������R�́Azlib/zlib.h��typedef���ƏՓ˂��āAgcc33.exe�ł̓R���p�C���G���[�ɂȂ�������ł��B�f�[�^�^�̉�������typedef���ƏՓ˂��Ă����v�Ȃ̂ł����A�֐��^�̉�������typedef���ƏՓ˂���ƃG���[�ɂȂ�悤�ł��B(���v����)
void g_slist_free_1(GSList* list/*NULL��*/);
/*---------------------------------------------------------------------------*/
//A macro which does the same as g_slist_free_1().
#define g_slist_free1(list) g_slist_free_1(list)
/*---------------------------------------------------------------------------*/
int g_slist_length(GSList* list/*NULL��*/);
GSList* g_slist_copy(GSList* list/*NULL��*/);
GSList* g_slist_copy_deep(GSList* list/*NULL��*/, void* (*copy_func/*NULL��*/)(const void* data, void* user_data), void* user_data);
GSList* g_slist_reverse(GSList* list/*NULL��*/);
GSList* g_slist_sort(GSList* list/*NULL��*/, int (*compare_func)(const void* a, const void* b));
GSList* g_slist_sort_with_data(GSList* list/*NULL��*/, int (*compare_func)(const void* a, const void* b, void* user_data), void* user_data);
GSList* g_slist_concat(GSList* list1/*NULL��*/, GSList* list2/*NULL��*/);
void g_slist_foreach(GSList* list/*NULL��*/, void (*func)(void* data, void* user_data), void* user_data);
GSList* g_slist_last(GSList* list/*NULL��*/);
/*---------------------------------------------------------------------------*/
//A convenience macro to get the next element in a GSList.
//Parameters
//		slist		An element in a GSList.
//Returns
//		The next element, or NULL if there are no more elements.
#define g_slist_next(slist) ((slist)?(slist)->next:NULL)
/*---------------------------------------------------------------------------*/
GSList* g_slist_nth(GSList* list/*NULL��*/, int n);
void* g_slist_nth_data(GSList* list/*NULL��*/, int n);
GSList* g_slist_find(GSList* list/*NULL��*/, const void* data);
GSList* g_slist_find_custom(GSList* list/*NULL��*/, const void* data, int (*compare_func)(const void* a, const void* b));
int g_slist_position(GSList* list/*NULL��*/, GSList* llink/*NULL��*/);
int g_slist_index(GSList* list/*NULL��*/, const void* data);
/*---------------------------------------------------------------------------*/
//{{2017/10/03�ǉ�:g_slist_compare_np(),g_slist_compare_custom_np()��ǉ����܂����B
int g_slist_compare_np(GSList* list1/*NULL��*/, GSList* list2/*NULL��*/);
int g_slist_compare_custom_np(GSList* list1/*NULL��*/, GSList* list2/*NULL��*/, int (*compare_func)(const void* a, const void* b));
//}}2017/10/03�ǉ�:g_slist_compare_np(),g_slist_compare_custom_np()��ǉ����܂����B
#ifdef  __cplusplus
}//extern "C"
#endif//__cplusplus
#endif//__G_STRING_H__
