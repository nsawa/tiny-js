/*
 *	garray.h
 *
 *	Arrays - arrays of arbitrary elements which grow automatically as elements are added
 *
 *	* Mon Apr 18 21:50:41 JST 2016 Naoyuki Sawa
 *	- 1st リリース。
 *	- GLibのArrays(http://developer.gnome.org/glib/stable/glib-Arrays.html)を参考にしました。
 *	  関数仕様はオリジナル版と同じですが、実装は独自で、処理速度よりも容量削減を優先しました。
 *	* Tue Apr 19 21:31:14 JST 2016 Naoyuki Sawa
 *	- GPtrArray(https://developer.gnome.org/glib/stable/glib-Pointer-Arrays.html)と、
 *	  GByteArray(https://developer.gnome.org/glib/stable/glib-Byte-Arrays.html)を追加しました。
 *	  関数仕様はオリジナル版と同じですが、実装は独自で、処理速度よりも容量削減を優先しました。
 *	* Wed Apr 20 10:51:26 JST 2016 Naoyuki Sawa
 *	- g_array_set_clear_func()に処理を追加し、要素を削除する関数(アプリケーション定義)としてfree関数のアドレスが指定された場合はエラー停止するようにしました。
 *	  例えば、以下のような呼び出しはバグであり、それを検出するためです。
 *	  │void test() {
 *	  │  GPtrArray* array = g_ptr_array_new_with_free_func(free);
 *	  │  g_ptr_array_add(array, strdup("FOO"));
 *	  │  g_ptr_array_unref(array);		←ここで異常動作となる。
 *	  │}
 *	  なぜならば、strdup()の呼び出しは'gc/leak_detector.h'のマクロ関数定義によってGC_debug_strdup()に置き換えられますが、
 *	  g_ptr_array_new_with_free_func()に引き渡すfreeは関数呼び出し形式ではないのでマクロ関数の置き換え対象とはならず、GC_debug_freeの関数アドレスではなく本物のfreeの関数アドレスになってしまうからです。
 *	  結果として、GC_debug_strdup()で確保したメモリを本物のfree()で開放しようとしてしまい、異常動作となります。
 *	- 解決案の一つは、'gc/leak_detector.h'のマクロ関数定義を書き換えて、マクロ関数ではなくシンボルでも置き換えがされるようにしてしまう事ですが、検討した所それは難しい事が判りました。
 *	  'gc/leak_detector.h'からインクルードされる'gc/gc.h'が、マクロ関数に追加の引数GC_EXTRASを渡しており、もしシンボルで置き換えるマクロに書き換えるとそれが実現出来なくなるからです。
 *	- 仕方が無いので、アプリケーションに透過的な解決案はあきらめて、アプリケーションが上記のようなコードを書いてしまった時に即エラー停止して検出する方法を取る事にしました。
 *	  ここでエラー停止した場合、アプリケーションが前述のような処理を行っている可能性が高いので、探し出して以下のように変更して下さい。
 *	  │static void user_free(void* ptr) { free(ptr);//これはマクロ関数として置き換えられる。 }
 *	  │void test() {
 *	  │  GPtrArray* array = g_ptr_array_new_with_free_func(user_free);
 *	  │  g_ptr_array_add(array, strdup("FOO"));
 *	  │  g_ptr_array_unref(array);
 *	  │}
 *	* Tue May 24 23:59:14 JST 2016 Naoyuki Sawa
 *	- GRealArray構造体の、alloc_bytesフィールドとelt_sizeフィールドの順序を入れ替えました。
 *	  gstring.hモジュールのGString構造体の、str,len,allocated_lenの並びに合わせるためです。
 *	- garray.cモジュールは、GRealArray構造体のalloc_bytesフィールドとelt_sizeフィールドの順序に依存していなかったので、この変更による影響は有りません。
 *	  アプリケーションの観点からも、元々GRealArray構造体のalloc_bytesフィールドとelt_sizeフィールドは非公開だったので、この変更による影響は有りません。
 *	* Sat Jun 25 21:55:36 JST 2016 Naoyuki Sawa
 *	- _g_array_index()へのindex指定を、これまでは(＜len)までしか許可していませんでしたが、(≦len)まで許可するように変更しました。
 *	  変更した理由は、配列の末尾のアドレスを取得したい事が有りますが、これまでの処理のままでは「&g_array_index(a,int,a->len)」のように指定した時にアサートしてしまっていたからです。
 *	  変更後は、上記の指定もアサートせずに動作するようになりました。
 *	  ただし、配列の末尾に対して取得して良いのはあくまでもアドレスだけであり、値を取得すると予期出来ない動作になる事に注意して下さい。
 *	  尚、GLibのオリジナル版の実装では元々indexの範囲を検査していないので、今回の変更によってオリジナル版よりも安全性が下がる事は有りません。
 *	* Sat Oct 15 21:50:56 JST 2016 Naoyuki Sawa
 *	- g_array_insert_val()のマクロ定義が間違って空になっていたのを修正しました。
 *	  そのため、g_array_insert_val()を呼び出したり、g_ptr_array_insert()のインデクスに-1以外を指定した時に、何も処理が行われていませんでした。
 *	  これまで、たまたま、上記のケースに相当する処理を行っておらず、気が付いていませんでした。
 *	  今回、アプリケーションから後者の呼び出しを行った箇所で問題が顕在化して、気が付きました。
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
 *	構造体
 *****************************************************************************/
typedef struct _GArray {
	void*		data;		//A pointer to the element data. The data may be moved as elements are added to the GArray.
	int		len;		//The number of elements in the GArray not including the possible terminating zero element.
} GArray;
/*****************************************************************************
 *	マクロ
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
#define g_array_insert_val(a,i,v)	g_array_insert_vals((a),(i),&(v),1)							//{{2016/10/15修正:g_array_insert_val()のマクロ定義が間違って空になっていたのを修正しました。}}
/*--------------------------------------------------------------------------*/
//Returns the element of a GArray at the given index.
//The return value is cast to the given type.
//This example gets a pointer to an element in a GArray:
//│EDayViewEvent* event;
//│//This gets a pointer to the 4th element in the array of EDayViewEvent structs.
//│event = &g_array_index(events, EDayViewEvent, 3);
//Parameters
//		a			A GArray.
//		t			The type of the elements.
//		i			The index of the element to return.
//Returns
//		The element of the GArray at the index given by i.
#define g_array_index(a,t,i)	(*(t*)_g_array_index((a),sizeof(t),(i)))							//┬元ソースではg_array_index()はマクロだけで作成されているが、当実装ではマクロと関数を使って作成する事にした。
void* _g_array_index(GArray* array, int type_size, int index);	//非公開関数							//┘関数を使う事にした理由は、g_array_index()は使用頻度が高いので、g_array_index()がindexの範囲検査を行う事で、バグを検出出来る機会が多いと思ったからです。
/*****************************************************************************
 *	グローバル関数
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
 *	構造体
 *****************************************************************************/
typedef struct _GPtrArray {
	void**		pdata;		//Points to the array of pointers, which may be moved when the array grows.
	int		len;		//Number of pointers in the array.
} GPtrArray;
/*****************************************************************************
 *	マクロ
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
 *	グローバル関数
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
 *	マクロ
 *****************************************************************************/
//GLibの関数仕様で、何故かg_byte_array_index()は定義されていません。
//GByteArray.data[]経由でアクセス出来るからかとも考えたのですが、それならばg_ptr_array_index()も不要なはずであり、g_ptr_array_index()が定義されている理由が説明出来ません。
//g_ptr_array_index()が定義されているのに、g_byte_array_index()は定義されていない理由が不明です。(もしかしたらあまり深い意味は無いのかも知れませんが…)
//とりあえず当実装も、GLibの関数仕様に従っておく事にしました。
/*****************************************************************************
 *	構造体
 *****************************************************************************/
typedef struct _GByteArray {
	uint8_t*	data;		//A pointer to the element data. The data may be moved as elements are added to the GByteArray.
	int		len;		//The number of elements in the GByteArray.
} GByteArray;
/*****************************************************************************
 *	グローバル関数
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
//以下は未対応です。
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
