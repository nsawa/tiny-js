/*
 *	gslist.c
 *
 *	Singly-Linked Lists - linked lists that can be iterated in one direction
 *
 *	* Wed Jul 27 21:35:30 JST 2016 Naoyuki Sawa
 *	- 1st リリース。
 *	- GLibのSingly-Linked Lists(https://developer.gnome.org/glib/stable/glib-Singly-Linked-Lists.html)を参考にしました。
 *	  関数仕様はオリジナル版と同じですが、実装は独自で、処理速度よりも容量削減を優先しました。
 *	- 実際のアプリケーションでは、積極的にGSListを使う事は少ないと思います。
 *	  GArrayやGTreeやght_hash_table等を使う方が、速度的にも使い易さ的にも優れているからです。
 *	  GSListが優れている点は、空のリストをNULLで表現する事が出来て、初期化が不要である事ぐらいだと思います。
 *	* Tue Oct 03 23:59:59 JST 2017 Naoyuki Sawa
 *	- g_slist_compare_np(),g_slist_compare_custom_np()を追加しました。
 */
#include "clip.h"
/*****************************************************************************
 *	GSList
 *****************************************************************************/
/*****************************************************************************
 *	ローカル関数
 *****************************************************************************/
static GSList* g_slist_alloc_subr(void* data, GSList* next/*NULL可*/) {
	GSList* new_list = calloc(1, sizeof(GSList));
	if(!new_list) { DIE(); }	//メモリ不足
	new_list->data = data;
	new_list->next = next;
	return new_list;
}
/*****************************************************************************
 *	グローバル関数
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
//│//Notice that these are initialized to the empty list.
//│GSList *list = NULL, *number_list = NULL;
//│//This is a list of strings.
//│list = g_slist_append(list, "first");
//│list = g_slist_append(list, "second");
//│//This is a list of integers.
//│number_list = g_slist_append(number_list, GINT_TO_POINTER(27));
//│number_list = g_slist_append(number_list, GINT_TO_POINTER(14));
//Parameters
//	list		A GSList.
//	data		The data for the new element.
//Returns
//	The new start of the GSList.
GSList* g_slist_append(GSList* list/*NULL可*/, void* data) {
	GSList* new_list = g_slist_alloc_subr(data, NULL);
	//リストが空でなければ…
	if(list) {
		GSList* last = g_slist_last(list);
		last->next = new_list;
	//リストが空ならば…
	} else {
		list = new_list;
	}
	return list;
}
/*---------------------------------------------------------------------------*/
//Adds a new element on to the start of the list.
//The return value is the new start of the list, which may have changed, so make sure you store the new value.
//│//Notice that it is initialized to the empty list.
//│GSList *list = NULL;
//│list = g_slist_prepend(list, "last");
//│list = g_slist_prepend(list, "first");
//Parameters
//	list		A GSList.
//	data		The data for the new element.
//Returns
//	The new start of the GSList.
GSList* g_slist_prepend(GSList* list/*NULL可*/, void* data) {
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
GSList* g_slist_insert(GSList* list/*NULL可*/, void* data, int position) {
	//リストが空か、先頭への挿入ならば…
	if(!list || !position) {				//「if(!list||(position<=0)){～」としないように注意しろ。先頭へ挿入するのは(position==0)の場合のみです。(position<0)が指定された場合は末尾に追加するのがg_slist_insert()の仕様です。
		//リスト(又はNULL)の先頭位置へ挿入する。
		list = g_slist_prepend(list, data);
	//空でないリストの、先頭でない位置への挿入ならば…
	} else {
		//挿入する位置の、直前のノードを取得する。
		GSList* prev = g_slist_nth(list, position - 1);
		//挿入する位置の直前のノードが有れば…		//即ち、(position≦g_slist_length(list))ならば…
		if(prev) {
			GSList* new_list = g_slist_alloc_subr(data, prev->next);
			prev->next = new_list;
		//挿入する位置の直前のノードが無ければ…	//即ち、(position＞g_slist_length(list))ならば…
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
GSList* g_slist_insert_before(GSList* slist/*NULL可*/, GSList* sibling/*NULL可*/, void* data) {
	//siblingの位置(又は-1)を求める。
	int i = g_slist_position(slist, sibling);		//(sibling==NULL)が指定された場合も、(i=-1)となる。
	//siblingの位置,又は,リストの最後に挿入する。
	slist = g_slist_insert(slist, data, i/*(-1)可*/);	//(sibling==NULL)が指定された場合も、リストの最後に追加する。
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
GSList* g_slist_insert_sorted(GSList* list/*NULL可*/, void* data, int (*compare_func)(const void* a, const void* b)) {							//g_slist_insert_sorted()やg_slist_insert_sorted_with_data()は低速です。これらの関数を使って個々の要素をソートされた位置に挿入するよりも、まずg_slist_prepend()を使って全ての要素を追加した後に、最後にg_slist_sort()やg_slist_sort_with_data()を使ってまとめてソートする方が高速です。
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
GSList* g_slist_insert_sorted_with_data(GSList* list/*NULL可*/, void* data, int (*compare_func)(const void* a, const void* b, void* user_data), void* user_data) {	//g_slist_insert_sorted()やg_slist_insert_sorted_with_data()は低速です。これらの関数を使って個々の要素をソートされた位置に挿入するよりも、まずg_slist_prepend()を使って全ての要素を追加した後に、最後にg_slist_sort()やg_slist_sort_with_data()を使ってまとめてソートする方が高速です。
	GSList* node = list;
	//指定されたdata以下となる位置(又はNULL)を見付ける。
	while(node && ((*compare_func)(data, node->data, user_data) > 0)) {
		node = node->next;
	}
	//見付けた位置に挿入(又はリストの最後に追加)する。
	return g_slist_insert_before(list, node/*NULL可*/, data);
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
GSList* g_slist_remove(GSList* list/*NULL可*/, const void* data) {	//現在の実装は低速です。(※TODO:単純化のために他の関数を利用して複数回走査しているのが低速な原因である。他の関数を利用せずに一回の走査で処理を完了すれば高速化出来る。)
	GSList* link_ = g_slist_find(list, data);
	if(link_) {
		list = g_slist_delete_link(list, link_);	//g_slist_remove_link()ではなく、g_slist_delete_link()を呼び出す事に注意せよ。関数名に対称性が無い。g_slist_remove()とg_slist_remove_all()はノードを削除するが、g_slist_remove_link()はノードを削除せず、g_slist_delete_link()がノードを削除する。
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
GSList* g_slist_remove_all(GSList* list/*NULL可*/, const void* data) {	//現在の実装は低速です。(※TODO:単純化のために他の関数を利用して複数回走査しているのが低速な原因である。他の関数を利用せずに一回の走査で処理を完了すれば高速化出来る。)
	for(;;) {
		GSList* link_ = g_slist_find(list, data);
		if(!link_) { break; }
		list = g_slist_delete_link(list, link_);	//g_slist_remove_link()ではなく、g_slist_delete_link()を呼び出す事に注意せよ。関数名に対称性が無い。g_slist_remove()とg_slist_remove_all()はノードを削除するが、g_slist_remove_link()はノードを削除せず、g_slist_delete_link()がノードを削除する。
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
GSList* g_slist_remove_link(GSList* list/*NULL可*/, GSList* link_) {
	//先頭要素を取り外す場合…
	if(list == link_) {
		list = list->next;
	//先頭要素以外を取り外す場合…
	} else {
		GSList *node = list, *prev;
		do {
			//link_がlistに属するノードでなかった場合は、エラー停止する事にした。
			// - 関数仕様では、link_がlistに属するノードでなかった場合の動作が不明である。
			//   オリジナル版の実装では、link_がlistに属するノードでなかった場合は何もしていないようだが、下記の理由により危険である。
			//   ・g_slist_delete_link()から呼び出された場合に、link_がlistに属していないと、予測できない結果となる。
			//     もしlink_が他のリストに属するノードであった場合、他のリストから取り外していないにもかかわらず、link_を開放してしまうからです。
			//   ・アプリケーションの観点から、g_slist_remove_link()を呼び出した後に、link_が確実に独立したノードになったかどうかを判断出来ない。
			//     g_slist_remove_link()が呼び出された後は、link_が独立したノードになっている事を、g_slist_remove_link()自身が保証すべきです。
			//   上記の理由により、link_がlistに属するノードでなかった場合は、エラー停止する事にした。
			if(!node) { DIE(); }	//link_を見付ける前にlistの終端に達したらエラー停止する。
			prev = node;		//直前のノードを記憶しておく。
			node = node->next;	//次のノードへ進める。
		} while(node != link_);		//link_を見付けたら抜ける。
		prev->next = link_->next;	//link_の直前のノードから、link_を取り外す。
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
GSList* g_slist_delete_link(GSList* list/*NULL可*/, GSList* link_) {
	list = g_slist_remove_link(list, link_);	//ノードを取り外す。
	g_slist_free_1(link_);				//ノードを削除する。
	return list;
}
/*---------------------------------------------------------------------------*/
//Frees all of the memory used by a GSList.
//The freed elements are returned to the slice allocator.
//If list elements contain dynamically-allocated memory, you should either use g_slist_free_full() or free them manually first.
//Parameters
//	list		A GSList.
void g_slist_free(GSList* list/*NULL可*/) {
	g_slist_free_full(list, NULL);
}
/*---------------------------------------------------------------------------*/
//Convenience method, which frees all the memory used by a GSList, and calls the specified destroy function on every element's data.
//Parameters
//	list		A pointer to a GSList.
//	free_func	The function to be called to free each element's data.
void g_slist_free_full(GSList* list/*NULL可*/, void (*free_func_/*NULL可*/)(void* data)) {
	while(list) {
		if(free_func_) { (*free_func_)(list->data); }	//このノードのデータを開放する。
		list = g_slist_delete_link(list, list);		//このノードを開放する。
	}
}
/*---------------------------------------------------------------------------*/
//Frees one GSList element. It is usually used after g_slist_remove_link().
//Parameters
//	list		A GSList element.
void g_slist_free_1(GSList* list/*NULL可*/) {
	free(list/*NULL可*/);
}
/*---------------------------------------------------------------------------*/
//Gets the number of elements in a GSList.
//This function iterates over the whole list to count its elements.
//To check whether the list is non-empty, it is faster to check list against NULL.
//Parameters
//	list		A GSList.
//Returns
//	The number of elements in the GSList.
int g_slist_length(GSList* list/*NULL可*/) {
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
GSList* g_slist_copy(GSList* list/*NULL可*/) {
	return g_slist_copy_deep(list, NULL, NULL);
}
/*---------------------------------------------------------------------------*/
//Makes a full (deep) copy of a GSList.
//In contrast with g_slist_copy(), this function uses func to make a copy of each list element, in addition to copying the list container itself.
//func , as a GCopyFunc, takes two arguments, the data to be copied and a user pointer. It's safe to pass NULL as user_data, if the copy function takes only one argument.
//For instance, if list holds a list of GObjects, you can do:
//│another_list = g_slist_copy_deep(list, (GCopyFunc)g_object_ref, NULL);
//And, to entirely free the new list, you could do:
//│g_slist_free_full(another_list, g_object_unref);
//Parameters
//	list		A GSList.
//	func		A copy function used to copy every element in the list.
//	user_data	User data passed to the copy function func , or NULL.
//Returns
//	A full copy of list , use g_slist_free_full to free it.
GSList* g_slist_copy_deep(GSList* list/*NULL可*/, void* (*copy_func/*NULL可*/)(const void* data, void* user_data), void* user_data) {
	GSList* new_list = NULL;
	while(list) {
		void* data = list->data;
		if(copy_func) { data = (*copy_func)(data, user_data); }
		new_list = g_slist_prepend(new_list, data);	//g_slist_append()は低速なので、まず高速なg_slist_prepend()で逆順にリストを作成しておき…
		list = list->next;
	}
	return g_slist_reverse(new_list);			//返す直前に正しい順に反転する。
}
/*---------------------------------------------------------------------------*/
//Reverses a GSList.
//Parameters
//	list		A GSList.
//Returns
//	The start of the reversed GSList.
GSList* g_slist_reverse(GSList* list/*NULL可*/) {
#if 0	//直観的で判り易いが、ノードのメモリ確保や解放が発生してしまう方法。
	GSList* new_list = NULL;
	while(list) {
		new_list = g_slist_prepend(new_list, list->data);	//元のリストの現在のノードのデータを、新しいリストの先頭のノードとして追加する。
		list = g_slist_delete_link(list, list);			//元のリストの現在のノードを解放する。
	}
	return new_list;
#else	//やや直観的でないが、ノードのメモリ確保や解放は発生しない方法。(オリジナル版の実装もこれと同じです。)
	GSList* prev = NULL;
	while(list) {
		GSList* next = list->next;	//次のノードを取り出しておく。
		list->next = prev;		//現在のノードの次のノードに、直前のノードを設定する。
		prev = list;			//直前のノードを記憶しておく。
		list = next;			//次のノードへ進める。
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
GSList* g_slist_sort(GSList* list/*NULL可*/, int (*compare_func)(const void* a, const void* b)) {
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
#if 0	//'1'にすると「コードサイズが小さくて低速な実装」を選択します。'0'にすると「高速だがコードサイズが大きな実装」を選択します。通常は'0'にして下さい。
//□コードサイズが小さくて低速な実装。
// - 挿入ソート(?)であり、計算量のオーダーはO(n^2)です。
//   P/ECEで実行すると、1000要素程度のリストでも7秒ぐらいかかり、あまり実用的ではありません。
//   コードサイズの制約が厳しく、かつ、要素数が非常に少ない場合しかGSListを使用しない事が判っている場合のみ、このバージョンを選択して下さい。
GSList* g_slist_sort_with_data(GSList* list/*NULL可*/, int (*compare_func)(const void* a, const void* b, void* user_data), void* user_data) {
	GSList* new_list = NULL;
	while(list) {
		new_list = g_slist_insert_sorted_with_data(new_list, list->data, compare_func, user_data);
		list = g_slist_delete_link(list, list);
	}
	return new_list;
}
#else
//高速だがコードサイズが大きな実装。
// - マージソートであり、計算量のオーダーはO(n log n)です。
//   P/ECEで実行すると、1000要素程度のリストならば1秒未満で完了し、実用的な速度だと思います。
//   通常は、このバージョンを選択して下さい。
// - clipdlst.cモジュールの、SortList_r()を元にして作成しました。
//   SortList_r()から変更した点は、マージ時に元のリストの末尾に追加するためのポインタを用いるようにした事です。(g_slist_append()は低速なので)
GSList* g_slist_sort_with_data(GSList* list/*NULL可*/, int (*compare_func)(const void* a, const void* b, void* user_data), void* user_data) {
	GSList *list1, *list2, *node1, *node2, **pnext;
	//左右のリストを初期化する。
	list1 = NULL;
	list2 = NULL;
	//元のリストが空になるまで…
	for(;;) {
		//元のリストが空ならば、抜ける。				//┐
		if(!list) { break; }						//│
		//元のリストの先頭要素を取り外し、左のリストに追加する。	//│
		node1 = list;							//│
		list = list->next;						//│
		node1->next = list1;						//│
		list1 = node1;							//│固定配列のマージソートと違って最初に全体の要素数が判らないので、二等分するために一要素づつ振り分ける事にした。
		//元のリストが空ならば、抜ける。				//│固定配列のマージソートは安定ソートであるが、当関数の実装はこの動作を行うために安定ソートではない事に注意せよ。
		if(!list) { break; }						//│
		//元のリストの先頭要素を取り外し、右のリストに追加する。	//│
		node2 = list;							//│
		list = list->next;						//│
		node2->next = list2;						//│
		list2 = node2;							//┘
	}
	//右のリストが空でなければ…
	// - 元のリストの要素数が0,又は,1になったら、再帰を停止するための判断である。
	//   この判断を行わないと、0,又は,1要素のリストに対して無限に再帰してしまう。
	if(list2) {
		//左右のリストをソートする。
		list1 = g_slist_sort_with_data(list1, compare_func, user_data);
		list2 = g_slist_sort_with_data(list2, compare_func, user_data);
	}
	//元のリストの末尾に追加するためのポインタを初期化する。
	pnext = &list;
	//左右のリストが空になるまで…
	for(;;) {
		//左右のリストの先頭要素を取り外す。空ならばNULLとする。
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
		//左の要素が有れば…
		if(node1) {
			//右の要素が有れば…
			if(node2) {
				//左の要素が右の要素よりも小さいか同じならば…
				if((*compare_func)(node1->data, node2->data, user_data) <= 0) {
					//左の要素を元のリストの末尾に追加する。
					*pnext = node1;
					pnext = &node1->next;
					//右の要素を右のリストの先頭に書き戻す。
					node2->next = list2;
					list2 = node2;
				//右の要素が左の要素よりも小さければ…
				} else {
					//右の要素を元のリストの末尾に追加する。
					*pnext = node2;
					pnext = &node2->next;
					//左の要素を左のリストの先頭に書き戻す。
					node1->next = list1;
					list1 = node1;
				}
			//右の要素が無ければ…
			} else {
				//左の要素を元のリストの末尾に追加する。
				*pnext = node1;
				pnext = &node1->next;
			}
		//左の要素が無ければ…
		} else {
			//右の要素が有れば…
			if(node2) {
				//右の要素を元のリストの末尾に追加する。
				*pnext = node2;
				pnext = &node2->next;
			//右の要素が無ければ…
			} else {
				//ループを抜ける。
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
GSList* g_slist_concat(GSList* list1/*NULL可*/, GSList* list2/*NULL可*/) {
	//後続リストが有れば…
	if(list2) {
		//先行リストが有れば…
		if(list1) {
			//先行リストの最後のノードに、後続リストを連結する。
			g_slist_last(list1)->next = list2;
		//先行リストが無ければ…
		} else {
			//後続リストを、リストの先頭とする。
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
void g_slist_foreach(GSList* list/*NULL可*/, void (*func)(void* data, void* user_data), void* user_data) {
	while(list) {
		GSList* next = list->next;		//次のノードを取り出しておく。			←┐
		(*func)(list->data, user_data);		//アプリケーション定義の関数を呼び出す。	　├オリジナル版の実装に倣ってアプリケーション定義の関数を呼び出す前に次のノードを取り出しておく事にした。アプリケーション定義の関数がリストを変更した場合のためです。例えばアプリケーション定義の関数の中で当該ノードを変更する処理('list=g_slist_remove(list,data)'等)を実行しても安全です。ただしリスト全体を変更したり次のノードを変更するような処理('list=g_slist_remove_all(list,data)'等)は不可です。
		list = next;				//次のノードへ進める。				←┘
	}
}
/*---------------------------------------------------------------------------*/
//Gets the last element in a GSList.
//This function iterates over the whole list.
//Parameters
//	list		A GSList.
//Returns
//	The last element in the GSList, or NULL if the GSList has no elements.
GSList* g_slist_last(GSList* list/*NULL可*/) {
	GSList* last = NULL;
	while(list) {
		last = list;		//最後のノードを記憶しておく。
		list = list->next;	//次のノードへ進める。
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
GSList* g_slist_nth(GSList* list/*NULL可*/, int n) {
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
void* g_slist_nth_data(GSList* list/*NULL可*/, int n) {
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
GSList* g_slist_find(GSList* list/*NULL可*/, const void* data) {
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
GSList* g_slist_find_custom(GSList* list/*NULL可*/, const void* data, int (*compare_func)(const void* a, const void* b)) {
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
int g_slist_position(GSList* list/*NULL可*/, GSList* llink/*NULL可*/) {		//(llink==NULL)が指定された場合は、結果は常に(-1)になる。
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
int g_slist_index(GSList* list/*NULL可*/, const void* data) {
	int i = 0;
	while(list) {
		if(list->data == data) { return i; }
		list = list->next;
		i++;
	}
	return -1;
}
/*---------------------------------------------------------------------------*/
//{{2017/10/03追加:g_slist_compare_np(),g_slist_compare_custom_np()を追加しました。
//要素をポインタのまま比較して、リストを比較する。(独自拡張)
//[in]
//	list1,list2	比較するリスト。
//[out]
//	戻り値		strcmp()等と同様です。
int g_slist_compare_np(GSList* list1/*NULL可*/, GSList* list2/*NULL可*/) {
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
//指定された比較関数を使用して、リストを比較する。(独自拡張)
//[in]
//	list1,list2	比較するリスト。
//[out]
//	戻り値		strcmp()等と同様です。
int g_slist_compare_custom_np(GSList* list1/*NULL可*/, GSList* list2/*NULL可*/, int (*compare_func)(const void* a, const void* b)) {
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
//}}2017/10/03追加:g_slist_compare_np(),g_slist_compare_custom_np()を追加しました。
