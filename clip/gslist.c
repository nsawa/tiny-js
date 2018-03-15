/*
 *	gslist.c
 *
 *	Singly-Linked Lists - linked lists that can be iterated in one direction
 *
 *	* Wed Jul 27 21:35:30 JST 2016 Naoyuki Sawa
 *	- 1st �����[�X�B
 *	- GLib��Singly-Linked Lists(https://developer.gnome.org/glib/stable/glib-Singly-Linked-Lists.html)���Q�l�ɂ��܂����B
 *	  �֐��d�l�̓I���W�i���łƓ����ł����A�����͓Ǝ��ŁA�������x�����e�ʍ팸��D�悵�܂����B
 *	- ���ۂ̃A�v���P�[�V�����ł́A�ϋɓI��GSList���g�����͏��Ȃ��Ǝv���܂��B
 *	  GArray��GTree��ght_hash_table�����g�������A���x�I�ɂ��g���Ղ��I�ɂ��D��Ă��邩��ł��B
 *	  GSList���D��Ă���_�́A��̃��X�g��NULL�ŕ\�����鎖���o���āA���������s�v�ł��鎖���炢���Ǝv���܂��B
 *	* Tue Oct 03 23:59:59 JST 2017 Naoyuki Sawa
 *	- g_slist_compare_np(),g_slist_compare_custom_np()��ǉ����܂����B
 */
#include "clip.h"
/*****************************************************************************
 *	GSList
 *****************************************************************************/
/*****************************************************************************
 *	���[�J���֐�
 *****************************************************************************/
static GSList* g_slist_alloc_subr(void* data, GSList* next/*NULL��*/) {
	GSList* new_list = calloc(1, sizeof(GSList));
	if(!new_list) { DIE(); }	//�������s��
	new_list->data = data;
	new_list->next = next;
	return new_list;
}
/*****************************************************************************
 *	�O���[�o���֐�
 *****************************************************************************/
//Allocates space for one GSList element.
//It is called by the g_slist_append(), g_slist_prepend(), g_slist_insert() and g_slist_insert_sorted() functions and so is rarely used on its own.
//Returns
//	A pointer to the newly-allocated GSList element.
GSList* g_slist_alloc() {
	return g_slist_alloc_subr(NULL, NULL);
}
/*---------------------------------------------------------------------------*/
//Adds a new element on to the end of the list.
//The return value is the new start of the list, which may have changed, so make sure you store the new value.
//Note that g_slist_append() has to traverse the entire list to find the end, which is inefficient when adding multiple elements.
//A common idiom to avoid the inefficiency is to prepend the elements and reverse the list when all elements have been added.
//��//Notice that these are initialized to the empty list.
//��GSList *list = NULL, *number_list = NULL;
//��//This is a list of strings.
//��list = g_slist_append(list, "first");
//��list = g_slist_append(list, "second");
//��//This is a list of integers.
//��number_list = g_slist_append(number_list, GINT_TO_POINTER(27));
//��number_list = g_slist_append(number_list, GINT_TO_POINTER(14));
//Parameters
//	list		A GSList.
//	data		The data for the new element.
//Returns
//	The new start of the GSList.
GSList* g_slist_append(GSList* list/*NULL��*/, void* data) {
	GSList* new_list = g_slist_alloc_subr(data, NULL);
	//���X�g����łȂ���΁c
	if(list) {
		GSList* last = g_slist_last(list);
		last->next = new_list;
	//���X�g����Ȃ�΁c
	} else {
		list = new_list;
	}
	return list;
}
/*---------------------------------------------------------------------------*/
//Adds a new element on to the start of the list.
//The return value is the new start of the list, which may have changed, so make sure you store the new value.
//��//Notice that it is initialized to the empty list.
//��GSList *list = NULL;
//��list = g_slist_prepend(list, "last");
//��list = g_slist_prepend(list, "first");
//Parameters
//	list		A GSList.
//	data		The data for the new element.
//Returns
//	The new start of the GSList.
GSList* g_slist_prepend(GSList* list/*NULL��*/, void* data) {
	return g_slist_alloc_subr(data, list);
}
/*---------------------------------------------------------------------------*/
//Inserts a new element into the list at the given position.
//Parameters
//	list		A GSList.
//	data		The data for the new element.
//	position	The position to insert the element.
//			If this is negative, or is larger than the number of elements in the list, the new element is added on to the end of the list.
//Returns
//	The new start of the GSList.
GSList* g_slist_insert(GSList* list/*NULL��*/, void* data, int position) {
	//���X�g���󂩁A�擪�ւ̑}���Ȃ�΁c
	if(!list || !position) {				//�uif(!list||(position<=0)){�`�v�Ƃ��Ȃ��悤�ɒ��ӂ���B�擪�֑}������̂�(position==0)�̏ꍇ�݂̂ł��B(position<0)���w�肳�ꂽ�ꍇ�͖����ɒǉ�����̂�g_slist_insert()�̎d�l�ł��B
		//���X�g(����NULL)�̐擪�ʒu�֑}������B
		list = g_slist_prepend(list, data);
	//��łȂ����X�g�́A�擪�łȂ��ʒu�ւ̑}���Ȃ�΁c
	} else {
		//�}������ʒu�́A���O�̃m�[�h���擾����B
		GSList* prev = g_slist_nth(list, position - 1);
		//�}������ʒu�̒��O�̃m�[�h���L��΁c		//�����A(position��g_slist_length(list))�Ȃ�΁c
		if(prev) {
			GSList* new_list = g_slist_alloc_subr(data, prev->next);
			prev->next = new_list;
		//�}������ʒu�̒��O�̃m�[�h��������΁c	//�����A(position��g_slist_length(list))�Ȃ�΁c
		} else {
			list = g_slist_append(list, data);
		}
	}
	return list;
}
/*---------------------------------------------------------------------------*/
//Inserts a node before sibling containing data.
//Parameters
//	slist		A GSList.
//	sibling		Node to insert data before.
//	data		Data to put in the newly-inserted node.
//Returns
//	The new head of the list.
GSList* g_slist_insert_before(GSList* slist/*NULL��*/, GSList* sibling/*NULL��*/, void* data) {
	//sibling�̈ʒu(����-1)�����߂�B
	int i = g_slist_position(slist, sibling);		//(sibling==NULL)���w�肳�ꂽ�ꍇ���A(i=-1)�ƂȂ�B
	//sibling�̈ʒu,����,���X�g�̍Ō�ɑ}������B
	slist = g_slist_insert(slist, data, i/*(-1)��*/);	//(sibling==NULL)���w�肳�ꂽ�ꍇ���A���X�g�̍Ō�ɒǉ�����B
	return slist;
}
/*---------------------------------------------------------------------------*/
//Inserts a new element into the list, using the given comparison function to determine its position.
//Parameters
//	list		A GSList.
//	data		The data for the new element.
//	func		The function to compare elements in the list.
//			It should return a number > 0 if the first parameter comes after the second parameter in the sort order.
//Returns
//	The new start of the GSList.
GSList* g_slist_insert_sorted(GSList* list/*NULL��*/, void* data, int (*compare_func)(const void* a, const void* b)) {							//g_slist_insert_sorted()��g_slist_insert_sorted_with_data()�͒ᑬ�ł��B�����̊֐����g���ČX�̗v�f���\�[�g���ꂽ�ʒu�ɑ}����������A�܂�g_slist_prepend()���g���đS�Ă̗v�f��ǉ�������ɁA�Ō��g_slist_sort()��g_slist_sort_with_data()���g���Ă܂Ƃ߂ă\�[�g������������ł��B
	return g_slist_insert_sorted_with_data(list, data, (int (*)(const void*, const void*, void*))compare_func, NULL);
}
/*---------------------------------------------------------------------------*/
//Inserts a new element into the list, using the given comparison function to determine its position.
//Parameters
//	list		A GSList.
//	data		The data for the new element.
//	func		The function to compare elements in the list.
//			It should return a number > 0 if the first parameter comes after the second parameter in the sort order.
//	user_data	Data to pass to comparison function.
//Returns
//	The new start of the GSList.
GSList* g_slist_insert_sorted_with_data(GSList* list/*NULL��*/, void* data, int (*compare_func)(const void* a, const void* b, void* user_data), void* user_data) {	//g_slist_insert_sorted()��g_slist_insert_sorted_with_data()�͒ᑬ�ł��B�����̊֐����g���ČX�̗v�f���\�[�g���ꂽ�ʒu�ɑ}����������A�܂�g_slist_prepend()���g���đS�Ă̗v�f��ǉ�������ɁA�Ō��g_slist_sort()��g_slist_sort_with_data()���g���Ă܂Ƃ߂ă\�[�g������������ł��B
	GSList* node = list;
	//�w�肳�ꂽdata�ȉ��ƂȂ�ʒu(����NULL)�����t����B
	while(node && ((*compare_func)(data, node->data, user_data) > 0)) {
		node = node->next;
	}
	//���t�����ʒu�ɑ}��(���̓��X�g�̍Ō�ɒǉ�)����B
	return g_slist_insert_before(list, node/*NULL��*/, data);
}
/*---------------------------------------------------------------------------*/
//Removes an element from a GSList.
//If two elements contain the same data, only the first is removed.
//If none of the elements contain the data, the GSList is unchanged.
//Parameters
//	list		A GSList.
//	data		The data of the element to remove.
//Returns
//	The new start of the GSList.
GSList* g_slist_remove(GSList* list/*NULL��*/, const void* data) {	//���݂̎����͒ᑬ�ł��B(��TODO:�P�����̂��߂ɑ��̊֐��𗘗p���ĕ����񑖍����Ă���̂��ᑬ�Ȍ����ł���B���̊֐��𗘗p�����Ɉ��̑����ŏ�������������΍������o����B)
	GSList* link_ = g_slist_find(list, data);
	if(link_) {
		list = g_slist_delete_link(list, link_);	//g_slist_remove_link()�ł͂Ȃ��Ag_slist_delete_link()���Ăяo�����ɒ��ӂ���B�֐����ɑΏ̐��������Bg_slist_remove()��g_slist_remove_all()�̓m�[�h���폜���邪�Ag_slist_remove_link()�̓m�[�h���폜�����Ag_slist_delete_link()���m�[�h���폜����B
	}
	return list;
}
/*---------------------------------------------------------------------------*/
//Removes all list nodes with data equal to data.
//Returns the new head of the list.
//Contrast with g_slist_remove() which removes only the first node matching the given data.
//Parameters
//	list		A GSList.
//	data		Data to remove.
//Returns
//	New head of list.
GSList* g_slist_remove_all(GSList* list/*NULL��*/, const void* data) {	//���݂̎����͒ᑬ�ł��B(��TODO:�P�����̂��߂ɑ��̊֐��𗘗p���ĕ����񑖍����Ă���̂��ᑬ�Ȍ����ł���B���̊֐��𗘗p�����Ɉ��̑����ŏ�������������΍������o����B)
	for(;;) {
		GSList* link_ = g_slist_find(list, data);
		if(!link_) { break; }
		list = g_slist_delete_link(list, link_);	//g_slist_remove_link()�ł͂Ȃ��Ag_slist_delete_link()���Ăяo�����ɒ��ӂ���B�֐����ɑΏ̐��������Bg_slist_remove()��g_slist_remove_all()�̓m�[�h���폜���邪�Ag_slist_remove_link()�̓m�[�h���폜�����Ag_slist_delete_link()���m�[�h���폜����B
	}
	return list;
}
/*---------------------------------------------------------------------------*/
//Removes an element from a GSList, without freeing the element.
//The removed element's next link is set to NULL, so that it becomes a self-contained list with one element.
//Removing arbitrary nodes from a singly-linked list requires time that is proportional to the length of the list (ie. O(n)).
//If you find yourself using g_slist_remove_link() frequently, you should consider a different data structure, such as the doubly-linked GList.
//Parameters
//	list		A GSList.
//	link_		An element in the GSList.
//Returns
//	The new start of the GSList, without the element.
GSList* g_slist_remove_link(GSList* list/*NULL��*/, GSList* link_) {
	//�擪�v�f�����O���ꍇ�c
	if(list == link_) {
		list = list->next;
	//�擪�v�f�ȊO�����O���ꍇ�c
	} else {
		GSList *node = list, *prev;
		do {
			//link_��list�ɑ�����m�[�h�łȂ������ꍇ�́A�G���[��~���鎖�ɂ����B
			// - �֐��d�l�ł́Alink_��list�ɑ�����m�[�h�łȂ������ꍇ�̓��삪�s���ł���B
			//   �I���W�i���ł̎����ł́Alink_��list�ɑ�����m�[�h�łȂ������ꍇ�͉������Ă��Ȃ��悤�����A���L�̗��R�ɂ��댯�ł���B
			//   �Eg_slist_delete_link()����Ăяo���ꂽ�ꍇ�ɁAlink_��list�ɑ����Ă��Ȃ��ƁA�\���ł��Ȃ����ʂƂȂ�B
			//     ����link_�����̃��X�g�ɑ�����m�[�h�ł������ꍇ�A���̃��X�g������O���Ă��Ȃ��ɂ�������炸�Alink_���J�����Ă��܂�����ł��B
			//   �E�A�v���P�[�V�����̊ϓ_����Ag_slist_remove_link()���Ăяo������ɁAlink_���m���ɓƗ������m�[�h�ɂȂ������ǂ����𔻒f�o���Ȃ��B
			//     g_slist_remove_link()���Ăяo���ꂽ��́Alink_���Ɨ������m�[�h�ɂȂ��Ă��鎖���Ag_slist_remove_link()���g���ۏ؂��ׂ��ł��B
			//   ��L�̗��R�ɂ��Alink_��list�ɑ�����m�[�h�łȂ������ꍇ�́A�G���[��~���鎖�ɂ����B
			if(!node) { DIE(); }	//link_�����t����O��list�̏I�[�ɒB������G���[��~����B
			prev = node;		//���O�̃m�[�h���L�����Ă����B
			node = node->next;	//���̃m�[�h�֐i�߂�B
		} while(node != link_);		//link_�����t�����甲����B
		prev->next = link_->next;	//link_�̒��O�̃m�[�h����Alink_�����O���B
	}
	link_->next = NULL;	//The removed element's next link is set to NULL, so that it becomes a self-contained list with one element.
	return list;
}
/*---------------------------------------------------------------------------*/
//Removes the node link_ from the list and frees it.
//Compare this to g_slist_remove_link() which removes the node without freeing it.
//Removing arbitrary nodes from a singly-linked list requires time that is proportional to the length of the list (ie. O(n)).
//If you find yourself using g_slist_delete_link() frequently, you should consider a different data structure, such as the doubly-linked GList.
//Parameters
//	list		A GSList.
//	link_		Node to delete.
//Returns
//	The new head of list.
GSList* g_slist_delete_link(GSList* list/*NULL��*/, GSList* link_) {
	list = g_slist_remove_link(list, link_);	//�m�[�h�����O���B
	g_slist_free_1(link_);				//�m�[�h���폜����B
	return list;
}
/*---------------------------------------------------------------------------*/
//Frees all of the memory used by a GSList.
//The freed elements are returned to the slice allocator.
//If list elements contain dynamically-allocated memory, you should either use g_slist_free_full() or free them manually first.
//Parameters
//	list		A GSList.
void g_slist_free(GSList* list/*NULL��*/) {
	g_slist_free_full(list, NULL);
}
/*---------------------------------------------------------------------------*/
//Convenience method, which frees all the memory used by a GSList, and calls the specified destroy function on every element's data.
//Parameters
//	list		A pointer to a GSList.
//	free_func	The function to be called to free each element's data.
void g_slist_free_full(GSList* list/*NULL��*/, void (*free_func_/*NULL��*/)(void* data)) {
	while(list) {
		if(free_func_) { (*free_func_)(list->data); }	//���̃m�[�h�̃f�[�^���J������B
		list = g_slist_delete_link(list, list);		//���̃m�[�h���J������B
	}
}
/*---------------------------------------------------------------------------*/
//Frees one GSList element. It is usually used after g_slist_remove_link().
//Parameters
//	list		A GSList element.
void g_slist_free_1(GSList* list/*NULL��*/) {
	free(list/*NULL��*/);
}
/*---------------------------------------------------------------------------*/
//Gets the number of elements in a GSList.
//This function iterates over the whole list to count its elements.
//To check whether the list is non-empty, it is faster to check list against NULL.
//Parameters
//	list		A GSList.
//Returns
//	The number of elements in the GSList.
int g_slist_length(GSList* list/*NULL��*/) {
	int length = 0;
	while(list) {
		list = list->next;
		length++;
	}
	return length;
}
/*---------------------------------------------------------------------------*/
//Copies a GSList.
//Note that this is a "shallow" copy.
//If the list elements consist of pointers to data, the pointers are copied but the actual data isn't.
//See g_slist_copy_deep() if you need to copy the data as well.
//Parameters
//	list		A GSList.
//Returns
//	A copy of list.
GSList* g_slist_copy(GSList* list/*NULL��*/) {
	return g_slist_copy_deep(list, NULL, NULL);
}
/*---------------------------------------------------------------------------*/
//Makes a full (deep) copy of a GSList.
//In contrast with g_slist_copy(), this function uses func to make a copy of each list element, in addition to copying the list container itself.
//func , as a GCopyFunc, takes two arguments, the data to be copied and a user pointer. It's safe to pass NULL as user_data, if the copy function takes only one argument.
//For instance, if list holds a list of GObjects, you can do:
//��another_list = g_slist_copy_deep(list, (GCopyFunc)g_object_ref, NULL);
//And, to entirely free the new list, you could do:
//��g_slist_free_full(another_list, g_object_unref);
//Parameters
//	list		A GSList.
//	func		A copy function used to copy every element in the list.
//	user_data	User data passed to the copy function func , or NULL.
//Returns
//	A full copy of list , use g_slist_free_full to free it.
GSList* g_slist_copy_deep(GSList* list/*NULL��*/, void* (*copy_func/*NULL��*/)(const void* data, void* user_data), void* user_data) {
	GSList* new_list = NULL;
	while(list) {
		void* data = list->data;
		if(copy_func) { data = (*copy_func)(data, user_data); }
		new_list = g_slist_prepend(new_list, data);	//g_slist_append()�͒ᑬ�Ȃ̂ŁA�܂�������g_slist_prepend()�ŋt���Ƀ��X�g���쐬���Ă����c
		list = list->next;
	}
	return g_slist_reverse(new_list);			//�Ԃ����O�ɐ��������ɔ��]����B
}
/*---------------------------------------------------------------------------*/
//Reverses a GSList.
//Parameters
//	list		A GSList.
//Returns
//	The start of the reversed GSList.
GSList* g_slist_reverse(GSList* list/*NULL��*/) {
#if 0	//���ϓI�Ŕ���Ղ����A�m�[�h�̃������m�ۂ������������Ă��܂����@�B
	GSList* new_list = NULL;
	while(list) {
		new_list = g_slist_prepend(new_list, list->data);	//���̃��X�g�̌��݂̃m�[�h�̃f�[�^���A�V�������X�g�̐擪�̃m�[�h�Ƃ��Ēǉ�����B
		list = g_slist_delete_link(list, list);			//���̃��X�g�̌��݂̃m�[�h���������B
	}
	return new_list;
#else	//��Ⓖ�ϓI�łȂ����A�m�[�h�̃������m�ۂ����͔������Ȃ����@�B(�I���W�i���ł̎���������Ɠ����ł��B)
	GSList* prev = NULL;
	while(list) {
		GSList* next = list->next;	//���̃m�[�h�����o���Ă����B
		list->next = prev;		//���݂̃m�[�h�̎��̃m�[�h�ɁA���O�̃m�[�h��ݒ肷��B
		prev = list;			//���O�̃m�[�h���L�����Ă����B
		list = next;			//���̃m�[�h�֐i�߂�B
	}
	return prev;
#endif
}
/*---------------------------------------------------------------------------*/
//Sorts a GSList using the given comparison function.
//Parameters
//	list		A GSList.
//	compare_func	The comparison function used to sort the GSList.
//			This function is passed the data from 2 elements of the GSList and should return 0 if they are equal, a negative value if the first element comes before the second, or a positive value if the first element comes after the second.
//Returns
//	The start of the sorted GSList.
GSList* g_slist_sort(GSList* list/*NULL��*/, int (*compare_func)(const void* a, const void* b)) {
	return g_slist_sort_with_data(list, (int (*)(const void*, const void*, void*))compare_func, NULL);
}
/*---------------------------------------------------------------------------*/
//Like g_slist_sort(), but the sort function accepts a user data argument.
//Parameters
//	list		A GSList.
//	compare_func	Comparison function.
//	user_data	Data to pass to comparison function.
//Returns
//	New head of the list.
#if 0	//'1'�ɂ���Ɓu�R�[�h�T�C�Y���������Ēᑬ�Ȏ����v��I�����܂��B'0'�ɂ���Ɓu���������R�[�h�T�C�Y���傫�Ȏ����v��I�����܂��B�ʏ��'0'�ɂ��ĉ������B
//���R�[�h�T�C�Y���������Ēᑬ�Ȏ����B
// - �}���\�[�g(?)�ł���A�v�Z�ʂ̃I�[�_�[��O(n^2)�ł��B
//   P/ECE�Ŏ��s����ƁA1000�v�f���x�̃��X�g�ł�7�b���炢������A���܂���p�I�ł͂���܂���B
//   �R�[�h�T�C�Y�̐��񂪌������A���A�v�f�������ɏ��Ȃ��ꍇ����GSList���g�p���Ȃ����������Ă���ꍇ�̂݁A���̃o�[�W������I�����ĉ������B
GSList* g_slist_sort_with_data(GSList* list/*NULL��*/, int (*compare_func)(const void* a, const void* b, void* user_data), void* user_data) {
	GSList* new_list = NULL;
	while(list) {
		new_list = g_slist_insert_sorted_with_data(new_list, list->data, compare_func, user_data);
		list = g_slist_delete_link(list, list);
	}
	return new_list;
}
#else
//���������R�[�h�T�C�Y���傫�Ȏ����B
// - �}�[�W�\�[�g�ł���A�v�Z�ʂ̃I�[�_�[��O(n log n)�ł��B
//   P/ECE�Ŏ��s����ƁA1000�v�f���x�̃��X�g�Ȃ��1�b�����Ŋ������A���p�I�ȑ��x���Ǝv���܂��B
//   �ʏ�́A���̃o�[�W������I�����ĉ������B
// - clipdlst.c���W���[���́ASortList_r()�����ɂ��č쐬���܂����B
//   SortList_r()����ύX�����_�́A�}�[�W���Ɍ��̃��X�g�̖����ɒǉ����邽�߂̃|�C���^��p����悤�ɂ������ł��B(g_slist_append()�͒ᑬ�Ȃ̂�)
GSList* g_slist_sort_with_data(GSList* list/*NULL��*/, int (*compare_func)(const void* a, const void* b, void* user_data), void* user_data) {
	GSList *list1, *list2, *node1, *node2, **pnext;
	//���E�̃��X�g������������B
	list1 = NULL;
	list2 = NULL;
	//���̃��X�g����ɂȂ�܂Łc
	for(;;) {
		//���̃��X�g����Ȃ�΁A������B				//��
		if(!list) { break; }						//��
		//���̃��X�g�̐擪�v�f�����O���A���̃��X�g�ɒǉ�����B	//��
		node1 = list;							//��
		list = list->next;						//��
		node1->next = list1;						//��
		list1 = node1;							//���Œ�z��̃}�[�W�\�[�g�ƈ���čŏ��ɑS�̗̂v�f��������Ȃ��̂ŁA�񓙕����邽�߂Ɉ�v�f�ÂU�蕪���鎖�ɂ����B
		//���̃��X�g����Ȃ�΁A������B				//���Œ�z��̃}�[�W�\�[�g�͈���\�[�g�ł��邪�A���֐��̎����͂��̓�����s�����߂Ɉ���\�[�g�ł͂Ȃ����ɒ��ӂ���B
		if(!list) { break; }						//��
		//���̃��X�g�̐擪�v�f�����O���A�E�̃��X�g�ɒǉ�����B	//��
		node2 = list;							//��
		list = list->next;						//��
		node2->next = list2;						//��
		list2 = node2;							//��
	}
	//�E�̃��X�g����łȂ���΁c
	// - ���̃��X�g�̗v�f����0,����,1�ɂȂ�����A�ċA���~���邽�߂̔��f�ł���B
	//   ���̔��f���s��Ȃ��ƁA0,����,1�v�f�̃��X�g�ɑ΂��Ė����ɍċA���Ă��܂��B
	if(list2) {
		//���E�̃��X�g���\�[�g����B
		list1 = g_slist_sort_with_data(list1, compare_func, user_data);
		list2 = g_slist_sort_with_data(list2, compare_func, user_data);
	}
	//���̃��X�g�̖����ɒǉ����邽�߂̃|�C���^������������B
	pnext = &list;
	//���E�̃��X�g����ɂȂ�܂Łc
	for(;;) {
		//���E�̃��X�g�̐擪�v�f�����O���B��Ȃ��NULL�Ƃ���B
		node1 = list1;
		if(node1) {
			list1 = node1->next;
			node1->next = NULL;
		}
		node2 = list2;
		if(node2) {
			list2 = node2->next;
			node2->next = NULL;
		}
		//���̗v�f���L��΁c
		if(node1) {
			//�E�̗v�f���L��΁c
			if(node2) {
				//���̗v�f���E�̗v�f�����������������Ȃ�΁c
				if((*compare_func)(node1->data, node2->data, user_data) <= 0) {
					//���̗v�f�����̃��X�g�̖����ɒǉ�����B
					*pnext = node1;
					pnext = &node1->next;
					//�E�̗v�f���E�̃��X�g�̐擪�ɏ����߂��B
					node2->next = list2;
					list2 = node2;
				//�E�̗v�f�����̗v�f������������΁c
				} else {
					//�E�̗v�f�����̃��X�g�̖����ɒǉ�����B
					*pnext = node2;
					pnext = &node2->next;
					//���̗v�f�����̃��X�g�̐擪�ɏ����߂��B
					node1->next = list1;
					list1 = node1;
				}
			//�E�̗v�f��������΁c
			} else {
				//���̗v�f�����̃��X�g�̖����ɒǉ�����B
				*pnext = node1;
				pnext = &node1->next;
			}
		//���̗v�f��������΁c
		} else {
			//�E�̗v�f���L��΁c
			if(node2) {
				//�E�̗v�f�����̃��X�g�̖����ɒǉ�����B
				*pnext = node2;
				pnext = &node2->next;
			//�E�̗v�f��������΁c
			} else {
				//���[�v�𔲂���B
				break;
			}
		}
	}
	return list;
}
#endif
/*---------------------------------------------------------------------------*/
//Adds the second GSList onto the end of the first GSList.
//Note that the elements of the second GSList are not copied.
//They are used directly.
//Parameters
//	list1		A GSList.
//	list2		The GSList to add to the end of the first GSList.
//Returns
//	The start of the new GSList.
GSList* g_slist_concat(GSList* list1/*NULL��*/, GSList* list2/*NULL��*/) {
	//�㑱���X�g���L��΁c
	if(list2) {
		//��s���X�g���L��΁c
		if(list1) {
			//��s���X�g�̍Ō�̃m�[�h�ɁA�㑱���X�g��A������B
			g_slist_last(list1)->next = list2;
		//��s���X�g��������΁c
		} else {
			//�㑱���X�g���A���X�g�̐擪�Ƃ���B
			list1 = list2;
		}
	}
	return list1;
}
/*---------------------------------------------------------------------------*/
//Calls a function for each element of a GSList.
//Parameters
//	list		A GSList.
//	func		The function to call with each element's data.
//	user_data	User data to pass to the function.
void g_slist_foreach(GSList* list/*NULL��*/, void (*func)(void* data, void* user_data), void* user_data) {
	while(list) {
		GSList* next = list->next;		//���̃m�[�h�����o���Ă����B			����
		(*func)(list->data, user_data);		//�A�v���P�[�V������`�̊֐����Ăяo���B	�@���I���W�i���ł̎����ɕ���ăA�v���P�[�V������`�̊֐����Ăяo���O�Ɏ��̃m�[�h�����o���Ă������ɂ����B�A�v���P�[�V������`�̊֐������X�g��ύX�����ꍇ�̂��߂ł��B�Ⴆ�΃A�v���P�[�V������`�̊֐��̒��œ��Y�m�[�h��ύX���鏈��('list=g_slist_remove(list,data)'��)�����s���Ă����S�ł��B���������X�g�S�̂�ύX�����莟�̃m�[�h��ύX����悤�ȏ���('list=g_slist_remove_all(list,data)'��)�͕s�ł��B
		list = next;				//���̃m�[�h�֐i�߂�B				����
	}
}
/*---------------------------------------------------------------------------*/
//Gets the last element in a GSList.
//This function iterates over the whole list.
//Parameters
//	list		A GSList.
//Returns
//	The last element in the GSList, or NULL if the GSList has no elements.
GSList* g_slist_last(GSList* list/*NULL��*/) {
	GSList* last = NULL;
	while(list) {
		last = list;		//�Ō�̃m�[�h���L�����Ă����B
		list = list->next;	//���̃m�[�h�֐i�߂�B
	}
	return last;
}
/*---------------------------------------------------------------------------*/
//Gets the element at the given position in a GSList.
//Parameters
//	list		A GSList.
//	n		The position of the element, counting from 0.
//Returns
//	The element, or NULL if the position is off the end of the GSList.
GSList* g_slist_nth(GSList* list/*NULL��*/, int n) {
	while(list && n--) {
		list = list->next;
	}
	return list;
}
/*---------------------------------------------------------------------------*/
//Gets the data of the element at the given position.
//Parameters
//	list		A GSList.
//	n		The position of the element.
//Returns
//	The element's data, or NULL if the position is off the end of the GSList.
void* g_slist_nth_data(GSList* list/*NULL��*/, int n) {
	list = g_slist_nth(list, n);
	return list ? list->data : NULL;
}
/*---------------------------------------------------------------------------*/
//Finds the element in a GSList which contains the given data.
//Parameters
//	list		A GSList.
//	data		The element data to find.
//Returns
//	The found GSList element, or NULL if it is not found.
GSList* g_slist_find(GSList* list/*NULL��*/, const void* data) {
	while(list && (list->data != data)) {
		list = list->next;
	}
	return list;
}
/*---------------------------------------------------------------------------*/
//Finds an element in a GSList, using a supplied function to find the desired element.
//It iterates over the list, calling the given function which should return 0 when the desired element is found.
//The function takes two gconstpointer arguments, the GSList element's data as the first argument and the given user data.
//Parameters
//	list		A GSList.
//	data		User data passed to the function.
//	func		The function to call for each element.
//Returns
//	The found GSList element, or NULL if it is not found.
GSList* g_slist_find_custom(GSList* list/*NULL��*/, const void* data, int (*compare_func)(const void* a, const void* b)) {
	while(list && (*compare_func)(list->data, data)) {
		list = list->next;
	}
	return list;
}
/*---------------------------------------------------------------------------*/
//Gets the position of the given element in the GSList (starting from 0).
//Parameters
//	list		A GSList.
//	llink		An element in the GSList.
//Returns
//	The position of the element in the GSList, or -1 if the element is not found.
int g_slist_position(GSList* list/*NULL��*/, GSList* llink/*NULL��*/) {		//(llink==NULL)���w�肳�ꂽ�ꍇ�́A���ʂ͏��(-1)�ɂȂ�B
	int i = 0;
	while(list) {
		if(list == llink) { return i; }
		list = list->next;
		i++;
	}
	return -1;
}
/*---------------------------------------------------------------------------*/
//Gets the position of the element containing the given data (starting from 0).
//Parameters
//	list		A GSList.
//	data		The data to find.
//Returns
//	The index of the element containing the data, or -1 if the data is not found.
int g_slist_index(GSList* list/*NULL��*/, const void* data) {
	int i = 0;
	while(list) {
		if(list->data == data) { return i; }
		list = list->next;
		i++;
	}
	return -1;
}
/*---------------------------------------------------------------------------*/
//{{2017/10/03�ǉ�:g_slist_compare_np(),g_slist_compare_custom_np()��ǉ����܂����B
//�v�f���|�C���^�̂܂ܔ�r���āA���X�g���r����B(�Ǝ��g��)
//[in]
//	list1,list2	��r���郊�X�g�B
//[out]
//	�߂�l		strcmp()���Ɠ��l�ł��B
int g_slist_compare_np(GSList* list1/*NULL��*/, GSList* list2/*NULL��*/) {
	while(list1 && list2) {
		if(list1->data < list2->data) { return -1; }
		if(list1->data > list2->data) { return  1; }
		list1 = list1->next;
		list2 = list2->next;
	}
	if(list1 < list2) { return -1; }	//!list1 &&  list2
	if(list1 > list2) { return  1; }	// list1 && !list2
	return 0;				//!list1 && !list2
}
/*---------------------------------------------------------------------------*/
//�w�肳�ꂽ��r�֐����g�p���āA���X�g���r����B(�Ǝ��g��)
//[in]
//	list1,list2	��r���郊�X�g�B
//[out]
//	�߂�l		strcmp()���Ɠ��l�ł��B
int g_slist_compare_custom_np(GSList* list1/*NULL��*/, GSList* list2/*NULL��*/, int (*compare_func)(const void* a, const void* b)) {
	while(list1 && list2) {
		int retval = (*compare_func)(list1->data, list2->data);
		if(retval) { return retval; }
		list1 = list1->next;
		list2 = list2->next;
	}
	if(list1 < list2) { return -1; }	//!list1 &&  list2
	if(list1 > list2) { return  1; }	// list1 && !list2
	return 0;				//!list1 && !list2
}
//}}2017/10/03�ǉ�:g_slist_compare_np(),g_slist_compare_custom_np()��ǉ����܂����B
