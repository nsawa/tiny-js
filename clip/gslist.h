/*
 *	gslist.h
 *
 *	Singly-Linked Lists - linked lists that can be iterated in one direction
 *
 *	* Wed Jul 27 21:35:30 JST 2016 Naoyuki Sawa
 *	- 1st リリース。
 *	- GLibのSingly-Linked Lists(https://developer.gnome.org/glib/stable/glib-Singly-Linked-Lists.html)を参考にしました。
 *	  関数仕様はオリジナル版と同じですが、実装は多少変更して、処理速度よりも容量削減を優先しました。
 *	- 実際のアプリケーションでは、積極的にGSListを使う事は少ないと思います。
 *	  GArrayやGTreeやght_hash_table等を使う方が、速度的にも使い易さ的にも優れているからです。
 *	  GSListが優れている点は、空のリストをNULLで表現する事が出来て、初期化が不要である事ぐらいだと思います。
 *	* Tue Oct 03 23:59:59 JST 2017 Naoyuki Sawa
 *	- g_slist_compare_np(),g_slist_compare_custom_np()を追加しました。
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
 *	構造体
 *****************************************************************************/
typedef struct _GSList {
	void*			data;		//Holds the element's data, which can be a pointer to any kind of data, or any integer value using the Type Conversion Macros.
	struct _GSList*		next;		//Contains the link to the next element in the list.
} GSList;
/*****************************************************************************
 *	グローバル関数
 *****************************************************************************/
GSList* g_slist_alloc();
GSList* g_slist_append(GSList* list/*NULL可*/, void* data);
GSList* g_slist_prepend(GSList* list/*NULL可*/, void* data);
GSList* g_slist_insert(GSList* list/*NULL可*/, void* data, int position);
GSList* g_slist_insert_before(GSList* slist/*NULL可*/, GSList* sibling/*NULL可*/, void* data);
GSList* g_slist_insert_sorted(GSList* list/*NULL可*/, void* data, int (*compare_func)(const void* a, const void* b));
GSList* g_slist_insert_sorted_with_data(GSList* list/*NULL可*/, void* data, int (*compare_func)(const void* a, const void* b, void* user_data), void* user_data);
GSList* g_slist_remove(GSList* list/*NULL可*/, const void* data);	//┐
GSList* g_slist_remove_all(GSList* list/*NULL可*/, const void* data);	//├■注意■関数名に対称性が無い。g_slist_remove()とg_slist_remove_all()はノードを削除するが、g_slist_remove_link()はノードを削除せず、g_slist_delete_link()がノードを削除する。
GSList* g_slist_remove_link(GSList* list/*NULL可*/, GSList* link_);	//│
GSList* g_slist_delete_link(GSList* list/*NULL可*/, GSList* link_);	//┘			//'link'ではなく'link_'とした理由は、オリジナル版の実装に倣ったからです。多分、オリジナル版の実装では、何か別のモジュールと名前が衝突していたのだと思います。当実装では'link'でも大丈夫なのですが、オリジナル版の実装に倣って'link_'のままにする事にしました。
void g_slist_free(GSList* list/*NULL可*/);
void g_slist_free_full(GSList* list/*NULL可*/, void (*free_func_/*NULL可*/)(void* data));	//'free_func'ではなく'free_func_'とした理由は、zlib/zlib.hのtypedef名と衝突して、gcc33.exeではコンパイルエラーになったからです。データ型の仮引数はtypedef名と衝突しても大丈夫なのですが、関数型の仮引数はtypedef名と衝突するとエラーになるようです。(※要調査)
void g_slist_free_1(GSList* list/*NULL可*/);
/*---------------------------------------------------------------------------*/
//A macro which does the same as g_slist_free_1().
#define g_slist_free1(list) g_slist_free_1(list)
/*---------------------------------------------------------------------------*/
int g_slist_length(GSList* list/*NULL可*/);
GSList* g_slist_copy(GSList* list/*NULL可*/);
GSList* g_slist_copy_deep(GSList* list/*NULL可*/, void* (*copy_func/*NULL可*/)(const void* data, void* user_data), void* user_data);
GSList* g_slist_reverse(GSList* list/*NULL可*/);
GSList* g_slist_sort(GSList* list/*NULL可*/, int (*compare_func)(const void* a, const void* b));
GSList* g_slist_sort_with_data(GSList* list/*NULL可*/, int (*compare_func)(const void* a, const void* b, void* user_data), void* user_data);
GSList* g_slist_concat(GSList* list1/*NULL可*/, GSList* list2/*NULL可*/);
void g_slist_foreach(GSList* list/*NULL可*/, void (*func)(void* data, void* user_data), void* user_data);
GSList* g_slist_last(GSList* list/*NULL可*/);
/*---------------------------------------------------------------------------*/
//A convenience macro to get the next element in a GSList.
//Parameters
//		slist		An element in a GSList.
//Returns
//		The next element, or NULL if there are no more elements.
#define g_slist_next(slist) ((slist)?(slist)->next:NULL)
/*---------------------------------------------------------------------------*/
GSList* g_slist_nth(GSList* list/*NULL可*/, int n);
void* g_slist_nth_data(GSList* list/*NULL可*/, int n);
GSList* g_slist_find(GSList* list/*NULL可*/, const void* data);
GSList* g_slist_find_custom(GSList* list/*NULL可*/, const void* data, int (*compare_func)(const void* a, const void* b));
int g_slist_position(GSList* list/*NULL可*/, GSList* llink/*NULL可*/);
int g_slist_index(GSList* list/*NULL可*/, const void* data);
/*---------------------------------------------------------------------------*/
//{{2017/10/03追加:g_slist_compare_np(),g_slist_compare_custom_np()を追加しました。
int g_slist_compare_np(GSList* list1/*NULL可*/, GSList* list2/*NULL可*/);
int g_slist_compare_custom_np(GSList* list1/*NULL可*/, GSList* list2/*NULL可*/, int (*compare_func)(const void* a, const void* b));
//}}2017/10/03追加:g_slist_compare_np(),g_slist_compare_custom_np()を追加しました。
#ifdef  __cplusplus
}//extern "C"
#endif//__cplusplus
#endif//__G_STRING_H__
