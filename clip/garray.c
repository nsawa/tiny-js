/*
 *	garray.c
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
 *	構造体
 *****************************************************************************/
typedef struct _GRealArray {
//public:
	char*		data;				//A pointer to the element data. The data may be moved as elements are added to the GArray.	⇔GArray.data		⇔GPtrArray.pdata	⇔GByteArray.data	⇔GString.str
	int		len;				//The number of elements in the GArray not including the possible terminating zero element.	⇔GArray.len		⇔GPtrArray.len		⇔GByteArray.len	⇔GString.len
//private:																											⇔GString.allocated_len
	int		alloc_bytes;			//確保済みメモリのバイト数。	要素数ではない事に注意せよ。
	int		elt_size;			//要素のバイト数。
//不要	unsigned	zero_terminated		:1;	⇒常にゼロ終端する事にしたのでこのフィールドは不要になりました。ゼロ終端が不要である場合は少し容量の無駄が生じるが呼び出し側が意識する必要は有りません。
//不要	unsigned	clear			:1;	⇒g_array_set_size()で拡張した要素を常にゼロクリアする事にしたのでこのフィールドは不要になりました。ゼロクリアが不要である場合は少し処理の無駄が生じるが呼び出し側が意識する必要は有りません。
	unsigned short	ref_count;			//参照カウント。元ソースでは32ビット値だったが、当実装では16ビット値に変更してメモリを節約する事にした。普通の使い方で参照カウントが(2^16)以上になる事はまず無いと思ので、これで充分だと思う。
	unsigned short	array_type;			//配列の種類。ARRAY_TYPE_G_ARRAY,又は,ARRAY_TYPE_G_PTR_ARRAY,又は,ARRAY_TYPE_G_BYTE_ARRAY。2016/04/20現在はg_array_on_remove_element()だけで参照している。もし今後、配列の種類によって動作を変える処理を追加する場合は、このフィールドを参照せよ。
	void		(*clear_func)(void* data);	//要素を削除する関数(アプリケーション定義)。g_array_set_clear_func(),又は,g_ptr_array_set_free_func()によってセットする。g_array_on_remove_element()経由で呼び出す。直接呼び出さない理由は、GArrayならば要素のアドレスを引数として呼び出すが、GPtrArrayならば要素の値を引数として呼び出す必要が有るからである。g_array_on_remove_element()がその違いを吸収する。
} GRealArray;
/*****************************************************************************
 *	ローカル関数
 *****************************************************************************/
//必要なバイト数から、実際に確保するバイト数を求める。
//実際に確保するバイト数は、必要なバイト数以上,かつ,最小の(2^N)とする。
static int g_array_pow2_alloc_bytes(int want_alloc) {
	int actual_alloc = 1;	//実際に確保するバイト数の最小値です。もっと大きな値から始めても構いません。元ソースでは16となっていましたが厳密な意味のある値ではないと思います。初期の頻繁な再割り当てによる性能低下が問題になる事は少ないと思うので当実装では単純に1としました。
	while((unsigned)actual_alloc < (unsigned)want_alloc) {
		if(!(actual_alloc <<= 1)) { DIE(); }	//want_allocが大き過ぎる。
	}
	return actual_alloc;
}
/*--------------------------------------------------------------------------*/
//((現在の要素数)+(len)+(ゼロ終端))のメモリを確保する。
static GRealArray* g_array_maybe_expand(GRealArray* rarray, int len) {
	int new_alloc_bytes;
	if(len < 0) { DIE(); }
	//必要なメモリのバイト数を求める。
	new_alloc_bytes = rarray->elt_size * (rarray->len + len + 1/*ゼロ終端*/);
	//必要なメモリのバイト数が、確保済みメモリのバイト数よりも大きければ…
	if((unsigned)new_alloc_bytes > (unsigned)rarray->alloc_bytes) {
		//必要なバイト数から、実際に確保するバイト数を求める。
		new_alloc_bytes = g_array_pow2_alloc_bytes(new_alloc_bytes);
		//メモリを確保,又は,拡張する。
		rarray->data = realloc(rarray->data, new_alloc_bytes);
		if(!rarray->data) { DIE(); }	//メモリ不足
		//新しく確保した要素をゼロクリアする。
		memset(&rarray->data[rarray->alloc_bytes], 0, (new_alloc_bytes - rarray->alloc_bytes));
		//確保済みメモリのバイト数を格納する。
		rarray->alloc_bytes = new_alloc_bytes;
	}
	return rarray;
}
/*--------------------------------------------------------------------------*/
//確保済みメモリの半分を超える部分が余分ならば、メモリを縮小する。
// - 元ソースの実装では、一旦確保されたメモリは、配列を削除するまで開放されないようです。
//   しかしもし、要素数の増減幅が大きく、一瞬だけ要素数が極端に増えた後減る事がある場合、無駄なメモリが長期間存在してしまう問題が生じます。
//   上記の問題を避けるために、当実装では、g_array_remove_～()を実行した後に、配列確保済みメモリの半分を超える無駄が生じていたら、メモリを縮小する事にしました。
// - この変更による弊害として、g_array_sized_new()を使って意図的に(初期状態で)無駄なメモリを予約した配列を作成した場合、その直後にg_array_remove_～()を呼び出すと、予約したメモリが縮小されてしまいます。
//   もしそうなっても動作結果には問題有りませんが、性能低下を引き起こす可能性が有ります。
//   しかし実際には、そのような事が起きるケースは少ないと思います。
//   g_array_sized_new()を使って意図的に(初期状態で)無駄なメモリを予約した配列を作成した場合、その直後にg_array_remove_～()を呼び出す事は少ないと思うからです。
//   g_array_sized_new()を使った場合、その後に行う操作は、連続的なg_array_append_～()である事がほとんどだと思います。
//   g_array_maybe_reduce()を呼び出すのはg_array_remove_～(),及び,(間接的に)g_array_set_size()だけであり、g_array_append_～()が呼び出されてもメモリを縮小する操作は行われません。
static GRealArray* g_array_maybe_reduce(GRealArray* rarray) {
	int new_alloc_bytes;
	//必要なメモリのバイト数を求める。
	new_alloc_bytes = rarray->elt_size * (rarray->len + 1/*ゼロ終端*/);
	//必要なバイト数から、実際に確保するバイト数を求める。
	new_alloc_bytes = g_array_pow2_alloc_bytes(new_alloc_bytes);
	//確保済みメモリのバイト数が、実際に確保するバイト数の2倍/*調整可*/を超過していたら…
	if((unsigned)rarray->alloc_bytes > (unsigned)(new_alloc_bytes << 1/*調整可*/)) {
		//メモリを縮小する。
		rarray->data = realloc(rarray->data, new_alloc_bytes);
		if(!rarray->data) { DIE(); }	//縮小方向のrealloc()が失敗する事は無いはず。
		//確保済みメモリのバイト数を格納する。
		rarray->alloc_bytes = new_alloc_bytes;
	}
	return rarray;
}
/*--------------------------------------------------------------------------*/
//要素を削除する時に呼び出すローカル関数。
static void g_array_on_remove_element(GRealArray* rarray, int index) {
	//要素を削除する関数(アプリケーション定義)がセットされていたら…
	if(rarray->clear_func) {
		//要素のアドレスを取得する。
		void* data = &rarray->data[rarray->elt_size * index];
		//配列の種類によって…
		switch(rarray->array_type) {
		default:DIE();	//バグ
		case ARRAY_TYPE_G_ARRAY:
			//GArrayならば、要素のアドレスを引数として要素を削除する関数(アプリケーション定義)を実行するので、dataはそのままで良い。
			/** no job **/
			break;
		case ARRAY_TYPE_G_PTR_ARRAY:
			//GPtrArrayならば、要素の値を引数として要素を削除する関数(アプリケーション定義)を実行するので、dataが指す値を取り出す。
			data = *(void**)data;
			break;
	    //	case ARRAY_TYPE_G_BYTE_ARRAY:
	    //		//GByteArrayならば、rarray->clear_funcが非NULLになる事は無いはずである。もし非NULLならばdefault:節でエラー停止させる。
		}
		//要素のアドレス,又は,要素の値を引数として、要素を削除する関数(アプリケーション定義)を実行する。
		(*rarray->clear_func)(data);
	}
}
/*****************************************************************************
 *	グローバル関数
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
//		zero_terminated		TRUE if the array should have an extra element at the end with all bits cleared.	⇒当実装ではこの引数は参照しません。
//		clear			TRUE if all bits in the array should be cleared to 0 on allocation.			⇒当実装ではこの引数は参照しません。
//		element_size		Size of each element in the array.
//		reserved_size		Number of elements preallocated.
//Returns
//		The new GArray.
GArray* g_array_sized_new(int zero_terminated, int clear, int element_size, int reserved_size) {
	GRealArray* rarray;
	if((element_size < 0) /*|| (reserved_size < 0)*/) { DIE(); }	//(reserved_size<0)はg_array_maybe_expand()で検査されるのでここでは検査不要です。
	//構造体のメモリを確保する。
	rarray = calloc(1, sizeof(GRealArray));		//elt_size以外のフィールドを0に初期化するために、malloc()でなくcalloc()が必須です。
	if(!rarray) { DIE(); }
	//構造体のフィールドを初期化する。
	rarray->elt_size   = element_size;
	rarray->ref_count  = 1;
//不要	rarray->array_type = ARRAY_TYPE_G_ARRAY;	//配列の種類を格納する。
	//初期メモリを確保する。
	return (GArray*)g_array_maybe_expand(rarray, reserved_size);
}
/*--------------------------------------------------------------------------*/
//Atomically increments the reference count of array by one.
//This function is MT-safe and may be called from any thread.									⇒当実装ではスレッドセーフではありません。複数のスレッドから参照する事は無いと思ったからです。実際の所g_array_ref()/g_array_unref()だけをスレッドセーフにしても他の関数がスレッドセーフでなければあまり意味は無いと思います。
//Parameters
//		array			A GArray.
//Returns
//		The passed in GArray.
GArray* g_array_ref(GArray* array) {
	GRealArray* rarray = (GRealArray*)array;
	if(!rarray->ref_count || (rarray->ref_count > (USHRT_MAX-1))) { DIE(); }
	//参照カウントを増やす。
	rarray->ref_count++;
	return (GArray*)rarray;
}
/*--------------------------------------------------------------------------*/
//Atomically decrements the reference count of array by one.
//If the reference count drops to 0, all memory allocated by the array is released.
//This function is MT-safe and may be called from any thread.									⇒当実装ではスレッドセーフではありません。複数のスレッドから参照する事は無いと思ったからです。実際の所g_array_ref()/g_array_unref()だけをスレッドセーフにしても他の関数がスレッドセーフでなければあまり意味は無いと思います。
//Parameters
//		array			A GArray.
void g_array_unref(GArray* array) {
	GRealArray* rarray = (GRealArray*)array;
	if(!rarray->ref_count /*|| (rarray->ref_count > USHRT_MAX)*/) { DIE(); }
	//最後の参照でなければ…
	if(rarray->ref_count != 1) {
		//参照カウントを減らす。
		rarray->ref_count--;
	//最後の参照ならば…
	// - ここで参照カウントを減らさないよう注意せよ。
	//   最後の参照カウントを減らす処理はg_array_free()が行う。
	} else {
		//構造体とデータのメモリを開放する。
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
	//必要なメモリを確保する。
	g_array_maybe_expand(rarray, len);
	//既存のデータを後ろへずらす。
	// - (index==rarray->len)だった場合もダミー処理となり安全です。
	memmove(&rarray->data[rarray->elt_size * (index + len)],
	        &rarray->data[rarray->elt_size * index],
	        (rarray->elt_size * (rarray->len - index)));
	//新しいデータを格納する。
	memcpy(&rarray->data[rarray->elt_size * index], data, (rarray->elt_size * len));
	//要素数を増やす。
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
	//要素を削除する時に呼び出すローカル関数を実行する。
	g_array_on_remove_element(rarray, index);
	//削除した要素の位置に、最後の要素をコピーする。
	// - (index==(rarray->len-1))だった場合もダミー処理となり安全です。
	memcpy(&rarray->data[rarray->elt_size * index],
	       &rarray->data[rarray->elt_size * (rarray->len - 1)],
	       rarray->elt_size);
	//要素数を減らす。
	rarray->len--;
	//空いたメモリをゼロクリアする。
	memset(&rarray->data[rarray->elt_size * rarray->len], 0, rarray->elt_size);
	//余分なメモリを開放する。
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
	//要素を削除する時に呼び出すローカル関数を実行する。
	for(i = 0; i < length; i++) {
		g_array_on_remove_element(rarray, index + i);
	}
	//既存のデータを前へずらす。
	memmove(&rarray->data[rarray->elt_size * index],
	        &rarray->data[rarray->elt_size * (index + length)],
	        rarray->elt_size * (rarray->len - (index + length)));
	//要素数を減らす。
	rarray->len -= length;
	//空いたメモリをゼロクリアする。
	memset(&rarray->data[rarray->elt_size * rarray->len], 0, (rarray->elt_size * length));
	//余分なメモリを開放する。
	return (GArray*)g_array_maybe_reduce(rarray);
}
/*--------------------------------------------------------------------------*/
//Sorts a GArray using compare_func which should be a qsort()-style comparison function (returns less than zero for first arg is less than second arg, zero for equal, greater zero if first arg is greater than second arg).
//This is guaranteed to be a stable sort since version 2.32.									⇒当実装では安定ソートではありません。
//Parameters
//		array			A GArray.
//		compare_func		Comparison function.
void g_array_sort(GArray* array, int (*compare_func)(const void* a, const void* b)) {
	g_array_sort_with_data(array, (int (*)(const void*,const void*,void*))compare_func, NULL);
}
/*--------------------------------------------------------------------------*/
//Like g_array_sort(), but the comparison function receives an extra user data argument.
//This is guaranteed to be a stable sort since version 2.32.									⇒当実装では安定ソートではありません。
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
	//増加ならば…
	if(length > rarray->len) {
		g_array_maybe_expand(rarray, length - rarray->len);
		rarray->len = length;
	//減少,又は,変化しないならば…
	} else {
		g_array_remove_range((GArray*)rarray, length, rarray->len - length);	//(length==rarray->len)だった場合もこの処理を呼び出して安全です。
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
//{{2016/04/20追加:g_array_set_clear_func()に処理を追加し、要素を削除する関数(アプリケーション定義)としてfree関数のアドレスが指定された場合はエラー停止するようにしました。
#ifdef  GC_LEAK_DETECTOR_H
	// * Wed Apr 20 10:51:26 JST 2016 Naoyuki Sawa
	// - g_array_set_clear_func()に処理を追加し、要素を削除する関数(アプリケーション定義)としてfree関数のアドレスが指定された場合はエラー停止するようにしました。
	//   例えば、以下のような呼び出しはバグであり、それを検出するためです。
	//   │void test() {
	//   │  GPtrArray* array = g_ptr_array_new_with_free_func(free);
	//   │  g_ptr_array_add(array, strdup("FOO"));
	//   │  g_ptr_array_unref(array);	←ここで異常動作となる。
	//   │}
	//   なぜならば、strdup()の呼び出しは'gc/leak_detector.h'のマクロ関数定義によってGC_debug_strdup()に置き換えられますが、
	//   g_ptr_array_new_with_free_func()に引き渡すfreeは関数呼び出し形式ではないのでマクロ関数の置き換え対象とはならず、GC_debug_freeの関数アドレスではなく本物のfreeの関数アドレスになってしまうからです。
	//   結果として、GC_debug_strdup()で確保したメモリを本物のfree()で開放しようとしてしまい、異常動作となります。
	// - 解決案の一つは、'gc/leak_detector.h'のマクロ関数定義を書き換えて、マクロ関数ではなくシンボルでも置き換えがされるようにしてしまう事ですが、検討した所それは難しい事が判りました。
	//   'gc/leak_detector.h'からインクルードされる'gc/gc.h'が、マクロ関数に追加の引数GC_EXTRASを渡しており、もしシンボルで置き換えるマクロに書き換えるとそれが実現出来なくなるからです。
	// - 仕方が無いので、アプリケーションに透過的な解決案はあきらめて、アプリケーションが上記のようなコードを書いてしまった時に即エラー停止して検出する方法を取る事にしました。
	//   ここでエラー停止した場合、アプリケーションが前述のような処理を行っている可能性が高いので、探し出して以下のように変更して下さい。
	//   │static void user_free(void* ptr) { free(ptr);//これはマクロ関数として置き換えられる。 }
	//   │void test() {
	//   │  GPtrArray* array = g_ptr_array_new_with_free_func(user_free);
	//   │  g_ptr_array_add(array, strdup("FOO"));
	//   │  g_ptr_array_unref(array);
	//   │}
	if(clear_func == free) { DIE(); }
#endif//GC_LEAK_DETECTOR_H
//}}2016/04/20追加:g_array_set_clear_func()に処理を追加し、要素を削除する関数(アプリケーション定義)としてfree関数のアドレスが指定された場合はエラー停止するようにしました。
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
	//戻り値をデータのメモリとしておく。
	segment = rarray->data;
	//データのメモリを開放するならば…
	if(free_segment) {
		int i;
		//要素を削除する時に呼び出すローカル関数を実行する。
		for(i = 0; i < rarray->len; i++) {
			g_array_on_remove_element(rarray, i);
		}
		//データのメモリを開放する。
		free(rarray->data);
		//戻り値をNULLに変更する。
		segment = NULL;
	}
	//いずれにしても、以下のフィールドはクリアしておく。
	// - 参照が残っている場合は、引き続きこの構造体を、'データのメモリが確保されていない要素数0の配列'として使用するためです。
	// - ただし、この状態では、ゼロ終端が存在しない事に注意してください。
	//   残っている参照を経由してg_array_append_val()等を呼び出して、改めてデータのメモリが確保されるまで、その状態のままです。
	//   通常は、残っている参照もデータを参照せずに、g_array_free(),又は,g_array_unref()を呼び出すだけだと思いますが、もしデータを参照しようとした場合は、ゼロ終端が存在しないという差異が有ります。
	//   この挙動は、元ソース(glib-2.48.0)でも同じです。
	rarray->data        = NULL;
	rarray->len         = 0;
	rarray->alloc_bytes = 0;
	//参照カウントを減らし、最後の参照だったならば…
	if(!--rarray->ref_count) {
		//構造体のメモリを開放する。
		free(rarray);
	}
	return segment;
}
/*****************************************************************************
 *	非公開関数
 *****************************************************************************/
//元ソースではg_array_index()はマクロだけで作成されているが、当実装ではマクロと関数を使って作成する事にした。
//関数を使う事にした理由は、g_array_index()は使用頻度が高いので、g_array_index()がindexの範囲検査を行う事で、バグを検出出来る機会が多いと思ったからです。
void* _g_array_index(GArray* array, int type_size, int index) {
	GRealArray* rarray = (GRealArray*)array;
	if((type_size != rarray->elt_size) ||				//たぶん、g_array_index()マクロの第二引数に指定した型が間違っている。
//{{2016/06/25変更:_g_array_index()へのindex指定を、これまでは(＜len)までしか許可していませんでしたが、(≦len)まで許可するように変更しました。
//	  ((unsigned)index >= (unsigned)rarray->len)) { DIE(); }	//g_array_index()マクロの第三引数に指定したインデクスが間違っている。
//↓2016/06/25変更:_g_array_index()へのindex指定を、これまでは(＜len)までしか許可していませんでしたが、(≦len)まで許可するように変更しました。
	  ((unsigned)index >  (unsigned)rarray->len)) { DIE(); }	//g_array_index()マクロの第三引数に指定したインデクスが間違っている。
//}}2016/06/25変更:_g_array_index()へのindex指定を、これまでは(＜len)までしか許可していませんでしたが、(≦len)まで許可するように変更しました。
	return &rarray->data[type_size * index];
}
/*****************************************************************************
 *	GPtrArray
 *****************************************************************************/
/*****************************************************************************
 *	グローバル関数
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
	((GRealArray*)array)->array_type = ARRAY_TYPE_G_PTR_ARRAY;	//配列の種類を格納する。
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
//This function is thread-safe and may be called from any thread.								⇒当実装ではスレッドセーフではありません。複数のスレッドから参照する事は無いと思ったからです。実際の所g_array_ref()/g_array_unref()だけをスレッドセーフにしても他の関数がスレッドセーフでなければあまり意味は無いと思います。
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
//		index			The index to place the new element at, or -1 to append.					//g_array_insert_～()はindexに-1を指定出来ないが、g_ptr_array_insert()はindexに-1を指定出来るというGLibの関数仕様です。何故そういう仕様なのか不思議なのですが、当実装もGLibの関数仕様に従っておく事にしました。
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
//Note that the comparison function for g_ptr_array_sort() doesn't take the pointers from the array as arguments, it takes pointers to the pointers in the array.		★★★★★重要!!比較関数にはポインタが渡されるのではなく、ポインタへのポインタが渡される仕様である事に注意して下さい。例えば、文字列ポインタを格納した配列をソートする際、比較関数としてstrcmp()を指定する事は出来ません。★★★★★
//This is guaranteed to be a stable sort since version 2.32.									⇒当実装では安定ソートではありません。
//Parameters
//		array			A GPtrArray.
//		compare_func		Comparison function.
void g_ptr_array_sort(GPtrArray* array, int (*compare_func)(const void* a, const void* b)) {
	g_array_sort((GArray*)array, compare_func);
}
/*--------------------------------------------------------------------------*/
//Like g_ptr_array_sort(), but the comparison function has an extra user data argument.
//Note that the comparison function for g_ptr_array_sort_with_data() doesn't take the pointers from the array as arguments, it takes pointers to the pointers in the array.	★★★★★重要!!比較関数にはポインタが渡されるのではなく、ポインタへのポインタが渡される仕様である事に注意して下さい。例えば、文字列ポインタを格納した配列をソートする際、比較関数としてstrcmp()を指定する事は出来ません。★★★★★
//This is guaranteed to be a stable sort since version 2.32.									⇒当実装では安定ソートではありません。
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
 *	グローバル関数
 *****************************************************************************/
//Creates a new GByteArray with a reference count of 1.
//Returns
//		The new GByteArray.	[transfer full]										//[transfer full]とは'Free data after the code is done.'の事です(https://developer.gnome.org/glib/stable/annotation-glossary.html)。しかし、https://developer.gnome.org/glib/stable/glib-Byte-Arrays.html の説明ではどうも[transfer full]の指定に一貫性が無いような気がするので、あまり気にしない方が良いかも知れません。説明を読めば誰がメモリを開放するかは大体直観通りです。
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
	((GRealArray*)array)->array_type = ARRAY_TYPE_G_BYTE_ARRAY;	//配列の種類を格納する。
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
