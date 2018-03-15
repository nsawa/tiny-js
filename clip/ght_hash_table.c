/*
 *	ght_hash_table.c
 *
 *	Generic Hash Table (GHT)
 *
 *	* Sat Jun 06 15:36:36 JST 2015 Naoyuki Sawa
 *	- 1st リリース。
 *	- 「libghthash - Generic Hash Table (GHT)」(http://www.bth.se/people/ska/sim_home/libghthash.html)互換モジュールです。
 *	  関数仕様はlibghthashと同じで、実装は独自です。
 *	  オリジナル版のlibghthashは速度性能を重視して実装されているようですが、当実装は省メモリを優先して単純に実装しました。
 *	- 関数仕様や使用例は、オリジナル版のlibghthashのドキュメントを参照してください。
 *	  オリジナル版のlibghthashを、/clip/keep/libghthash.7z に保存しておきました。
 *	* Sun Jul 12 21:50:47 JST 2015 Naoyuki Sawa
 *	- ght_set_rehash(),ght_rehash()を対応しました。
 *	  速度性能を重視しない方針でこれらの関数には対応していませんでしたが、メモリ効率が改善するために対応する事にしました。
 *	  これまでは、ght_create()の引数に有る程度大きな値を指定する必要が有り、実際の要素数が少ない時に無駄が生じていました。
 *	  今後は、常に'ght_create(0);ght_set_rehash(p_ht,1)'で良く、要素数の増減に応じて自動的にテーブルサイズが調整されます。
 *	* Tue Oct 06 21:24:18 JST 2015 Naoyuki Sawa
 *	- ght_size()を高速化しました。
 *	  これまではRAM節約を最優先してi_sizeフィールドを保持せずに、ght_size()が呼出される度にle_iteratorを辿って要素数を数えていました。
 *	  今回の変更でi_sizeフィールドを追加して、ght_size()はi_sizeを返すだけにして高速化しました。
 *	  僅かにRAM消費量が増えますが、P/ECEの場合はプログラムをRAMに展開するので、ght_size()のコードサイズ低減で実質RAM消費量も減ります。
 *	* Thu Apr 28 21:03:22 JST 2016 Naoyuki Sawa
 *	- ght_set_bounded_buckets()を対応しました。
 *	- キャッシュとしての動作によるエントリの自動削除を行うのは、ght_insert()が呼び出された時だけという仕様です。(オリジナル版ソースの動作に準拠しました。)
 *	  ght_set_rehash()を呼び出してバケットのエントリ数が増えたり、ght_set_bounded_buckets()を呼び出してバケットリミットが減ったことにより、
 *	  バケットのエントリ数がバケットリミットを超えたとしても、その時点ではエントリの自動削除は行いません。
 *	  エントリの自動削除を行うのは、それ以降にght_insert()が呼び出され、その時に参照したバケットがバケットリミットを超えていた時である事に注意して下さい。
 *	- また、直観的に理解出来ると思いますが、キャッシュとしての動作と、自動リハッシュの動作は、相性が悪いです。
 *	  バケットリミットを超えてエントリの自動削除が行われるのを期待しているのに、その前に自動リハッシュが働いてバケット数が増え、
 *	  バケット当たりのエントリが減り、いつまで経ってもエントリの自動削除が行われない可能性が高いからです。(バケットリミットの設定値にもよりますが…)
 *	- 以上をまとめると、結論としては:
 *	  □キャッシュとして使用する場合の、望ましい手順
 *	  │バケット数を明示してハッシュテーブルを作成し、その直後にバケットリミットを指定してキャッシュを有効化し、
 *	  │それ以降は、バケットリミットの変更も、自動リハッシュの有効化も行わないのが良い。
 *	  │<例>
 *	  │ght_hash_table_t* p_ht = ght_create(17);
 *	  │ght_set_bounded_buckets(p_ht, 8, fnBucketFree);
 *	  │//これ以降、ght_set_bounded_buckets()もght_set_rehash()も呼び出さない。
 *	- ght_set_bounded_buckets()を呼び出してキャッシュを有効化すると、ght_insert()が多少速度低下する事に注意して下さい。
 *	  全体のエントリ数はi_sizeフィールドに保持していますが、バケット毎のエントリ数は保持していないので、バケット溢れを判定するために都度走査するからです。
 *	  ght_insert()の速度低下を防ぐには、バケット毎にエントリ数を保持するようにすれば良いのですが、それは行わない事にしました。理由は、以下の通りです。
 *	  ・理由1. バケット内のエントリ数は極端に多くないと思うので、都度操作しても極端に遅くはないと思うから。
 *	           元々ハッシュテーブルを使用する目的が高速化のためですから、一つのバケットに極端に多くのエントリを保持するのは現実的ではなく、
 *	           ght_set_bounded_buckets()に指定するパケットリミットの値は、せいぜい1桁～2桁ぐらいの個数であるはずです。
 *	           それぐらいの数ならば、ght_insert()の都度走査しても、さほど大きな問題にはならないと思いました。
 *	           実際のところ、それはght_get()がヒットしない時に走査するエントリ数と同じですから、そんなに遅くは無いはずです。
 *	  ・理由2. ght_insert()の呼び出し頻度は、多くないと思うから。
 *	           一般的に、参照よりも変更の方が少ないと思うので、ght_get()に比べるとght_insert()の呼び出し回数は少ないと思います。
 *	           従って、ght_insert()が多少速度低下しても、全体への影響は少ないと思いました。
 *	  ・理由3. キャッシュを有効化しなければ、ght_insert()の速度低下は起こらない事。
 *	           ght_insert()がバケットを走査するのは、ght_set_bounded_buckets()を呼び出してキャッシュを有効化した場合だけです。
 *	           実際には、キャッシュを有効化せずに使う事の方が多いと思うので、ght_insert()が速度低下する事は少ないと思います。
 *	  ・理由4. バケット毎にエントリ数を保持すると、メモリ消費量が増えてしまう事。
 *	           当然ながら、バケット毎にエントリ数を保持すると、そのための変数が必要になり、常時メモリ消費量が増えます。
 *	           理由1,2,3の通り、ght_insert()の速度低下による影響は少ないのに、それを改善するために常時メモリ消費量が増えるのは無駄だと思いました。
 *	           それよりも、キャッシュを有効化した時だけght_insert()が多少速度低下する事を受け入れる方が、良いと思いました。
 *	  以上の理由により、ght_insert()の速度低下を防ぐ対策は行わない事にしました。
 *	  しばらく使ってみて、もし問題が有れば再検討しようと思いますが、おそらくこのままで問題無いと思います。
 *	* Sat Apr 30 21:45:42 JST 2016 Naoyuki Sawa
 *	- ght_set_alloc()を対応しました。
 *	* Fri May 06 21:58:10 JST 2016 Naoyuki Sawa
 *	- イテレーション用のリストの追加位置を変更しない理由について、ght_insert()内にコメントを追記しました。
 *	  コードには変更有りません。
 *	* Sun Jun 05 21:20:53 JST 2016 Naoyuki Sawa
 *	- ght_first(),ght_first_keysize(),ght_next(),ght_next_keysize()の、pp_key_data引数もNULL可としました。
 *	  変更した理由は、以下の通りです。
 *	- オリジナル版のlibghthashでは、p_key_size引数はNULL可ですが、pp_key_data引数はNULL不可でした。
 *	  しかし実際に使ってみると、列挙中にキーは不要であるケースも多く、pp_key_data引数を指定するためだけにダミー変数を用意しなければいけない事が有りました。
 *	  pp_key_data引数をNULL可とすれは、上記の手間を省く事が出来るので、pp_key_data引数もNULL可とする事にしました。
 *	- この変更は、オリジナル版のlibghthashと仕様が非互換になってしまう問題も有るのですが、上位互換だから許容して良いと思います。
 *	  もし、オリジナル版のlibghthashの仕様を厳密に守る必要が有る場合は、アプリケーションが、pp_key_data引数にNULLを指定しないように注意すれば良いからです。
 *	* Sat Oct 01 21:11:57 JST 2016 Naoyuki Sawa
 *	- Hewは要素数[0]の配列に対応していないので、要素数[1]として調整しました。
 *	  P/ECE開発環境(GCC)やWindows(VC++6.0)は、要素数[0]の配列に対応しているので、関係有りません。
 *	- ght_insert()において、変数'p_e'が無駄に二重に定義されていたのを修正しました。
 *	  動作上はそのままでも問題ありませんし、P/ECE開発環境(GCC)やWindows(VC++6.0,VS2015)では警告も出ないのですが、Hewで「C6348 (I) Declaration hides variable "p_e"」という警告が出たからです。
 *	  確かに無駄だったので、修正する事にしました。
 *	  今回の修正による動作の変化は有りません。
 */
#include "clip.h"
/****************************************************************************
 *	ローカル関数
 ****************************************************************************/
static int ght_get_hash_value(ght_hash_table_t* p_ht, int i_key_size, const void* p_key_data) {
	return (unsigned)one_at_a_time_hash(p_key_data, i_key_size) % (unsigned)p_ht->i_table_size;
}
/*--------------------------------------------------------------------------*/
//Search for an element in a bucket.
static ght_hash_entry_t* ght_search_in_bucket(ght_hash_table_t* p_ht, int i_key_size, const void* p_key_data) {
	int l_key = ght_get_hash_value(p_ht, i_key_size, p_key_data);
	LIST_ENTRY* list_head = &p_ht->le_bucket[l_key];
	LIST_ENTRY* list_entry = list_head->Flink;
	while(list_entry != list_head) {
		ght_hash_entry_t* p_e = CONTAINING_RECORD(list_entry, ght_hash_entry_t, le_bucket);
		if((p_e->i_key_size == i_key_size) && !memcmp(p_e->key_data, p_key_data, i_key_size)) {
		    //	//このハッシュテーブルが、キャッシュとして動作中ならば…	//┐
		    //	if(p_ht->bucket_limit) {					//│このハッシュテーブルがキャッシュとして動作中か否かに関係無く、参照したエントリをこのバケットの先頭へ移動する事にした。
				//このエントリを、このバケットの先頭へ移動する。	//│キャッシュとして動作中でなくても、頻繁に参照するエントリの検索が早くなるという利点があるからです。
				RemoveEntryList(&p_e->le_bucket);			//│尚、ght_remove()から呼び出された場合は無駄処理ですが、動作には影響有りません。
				InsertHeadList(list_head, &p_e->le_bucket);		//│
		    //	}								//┘
			return p_e;
		}
		list_entry = list_entry->Flink;
	}
	return NULL;
}
/*--------------------------------------------------------------------------*/
static void ght_check_rehash(ght_hash_table_t* p_ht) {
	if(p_ht->flag.b_rehash) {
		//現在の要素数を取得する。ただし、要素数が(0)の場合は(1)として、以下の計算を行う。
		int i_size = p_ht->i_size;
		if(!i_size) { i_size = 1; }
		//現在のテーブルサイズが、要素数の半分未満,又は,二倍超過ならば、要素数に合わせる。
		if((p_ht->i_table_size < (i_size / 2)) ||
		   (p_ht->i_table_size > (i_size * 2))) { ght_rehash(p_ht, i_size); }
		//以上の処理によって、テーブルサイズが要素数の半分以上～二倍以下の範囲に追従する。
	}
}
/****************************************************************************
 *	グローバル関数
 ****************************************************************************/
//Create a new hash table.
ght_hash_table_t* ght_create(int i_table_size) {
	ght_hash_table_t* p_ht;
	p_ht = calloc(sizeof(ght_hash_table_t), 1);	//ここではp_ht->fn_alloc,p_ht->fn_freeを使用しない事に注意して下さい。
	if(!p_ht) { DIE(); }
	InitializeListHead(&p_ht->le_iterator);
	ght_rehash(p_ht, i_table_size);
	return p_ht;
}
/*--------------------------------------------------------------------------*/
//Get the number of items in the hash table.
int ght_size(ght_hash_table_t* p_ht) {
	return p_ht->i_size;
}
/*--------------------------------------------------------------------------*/
//Get the size of the hash table.
int ght_table_size(ght_hash_table_t* p_ht) {
	return p_ht->i_table_size;
}
/*--------------------------------------------------------------------------*/
//Insert an entry into the hash table.
int ght_insert(ght_hash_table_t* p_ht, void* p_entry_data, int i_key_size, const void* p_key_data) {
	ght_hash_entry_t* p_e;
	int l_key;
	if(!p_entry_data) { DIE(); }
	if(ght_get(p_ht, i_key_size, p_key_data)) { return -1; }	//Don't insert if the key is already present.
	l_key = ght_get_hash_value(p_ht, i_key_size, p_key_data);
	//このハッシュテーブルが、キャッシュとして動作中ならば…
	if(p_ht->bucket_limit) {
		LIST_ENTRY* list_head = &p_ht->le_bucket[l_key];
		LIST_ENTRY* list_entry = list_head->Flink;
		//このバケットの先頭から(bucket_limit-1)個を超えるバケットを全て削除する。
		int i = p_ht->bucket_limit;
		while(list_entry != list_head) {
//{{2016/10/01修正:ght_insert()において、変数'p_e'が無駄に二重に定義されていたのを修正しました。
//			ght_hash_entry_t* p_e = CONTAINING_RECORD(list_entry, ght_hash_entry_t, le_bucket);
//↓2016/10/01修正:ght_insert()において、変数'p_e'が無駄に二重に定義されていたのを修正しました。
			p_e = CONTAINING_RECORD(list_entry, ght_hash_entry_t, le_bucket);
//}}2016/10/01修正:ght_insert()において、変数'p_e'が無駄に二重に定義されていたのを修正しました。
			list_entry = list_entry->Flink;
			if(--i <= 0) {	//bucket_limit個目も削除対象とするのでプレインクリメントです。
				RemoveEntryList(&p_e->le_iterator);
				RemoveEntryList(&p_e->le_bucket);
				if(p_ht->fn_bucket_free) { (*p_ht->fn_bucket_free)(p_e->p_entry_data, p_e->key_data); }
				if(p_ht->fn_free) {
					(*p_ht->fn_free)(p_e);
				} else {
					free(            p_e);	//'(p_ht->fn_free?p_ht->fn_free:free)(p_e)'は不可です。leak_detector.hによる関数置き換えの対象とするために、malloc/freeを名前で使わずに関数呼び出し形式で使う必要が有ります。詳細は、garray.cの'Wed Apr 20 10:51:26 JST 2016'のコメントを参照して下さい。
				}
				p_ht->i_size--;	//忘れないで!!
			}
		}
	}
#ifndef __RENESAS__
//□Win32とP/ECEの場合
	if(p_ht->fn_alloc) {
		p_e = (*p_ht->fn_alloc)(sizeof(ght_hash_entry_t) + i_key_size);
	} else {
		p_e = malloc(           sizeof(ght_hash_entry_t) + i_key_size);	//'(p_ht->fn_alloc?p_ht->fn_alloc:malloc)(～)'は不可です。leak_detector.hによる関数置き換えの対象とするために、malloc/freeを名前で使わずに関数呼び出し形式で使う必要が有ります。詳細は、garray.cの'Wed Apr 20 10:51:26 JST 2016'のコメントを参照して下さい。
	}
#else //__RENESAS__
//□Hewの場合
	if(p_ht->fn_alloc) {
		p_e = (*p_ht->fn_alloc)(offsetof(ght_hash_entry_t, key_data) + ((i_key_size + 3) & ~3));	//Hewは要素数[0]の配列に対応していないので、要素数[1]として調整しました。
	} else {
		p_e = malloc(           offsetof(ght_hash_entry_t, key_data) + ((i_key_size + 3) & ~3));	//Hewは要素数[0]の配列に対応していないので、要素数[1]として調整しました。
	}
#endif//__RENESAS__
	if(!p_e) { DIE(); }
	p_e->p_entry_data = p_entry_data;
	p_e->i_key_size = i_key_size;
	memcpy(p_e->key_data, p_key_data, i_key_size);
	InsertTailList(&p_ht->le_iterator, &p_e->le_iterator);		//イテレーション用のリストを直接参照している既存のアプリケーションとの互換性のために、イテレーション用のリストへの追加位置はこれまで通り末尾に追加する事にした。	{{2016/05/06コメント追記:本当は、要素の追加順とイテレーション用のリストの順序は無関係であるべきなのだが、当モジュールの実装に依存してリストの末尾に追加される事を期待した実装になってしまっているモジュールが存在する(clipstr.cのintern_string()。SqliteHelper.cも怪しいか?)。望ましくはないのだが、確かに追加順にイテレートされると便利な場面も多いので、このままにする事した。尚、イテレート前に明示的にソートしている(これも仕様外ではあるのだが)場合は、元々追加順には影響されないイテレート順となっているのでこの件とは無関係です(clipwkst.c等)。}}
	InsertHeadList(&p_ht->le_bucket[l_key], &p_e->le_bucket);	//キャッシュ対応のために、バケットのリストへの追加位置は先頭に追加するように変更した。既存のアプリケーションはバケットのリストを直接参照していないので、この変更による互換性の問題は生じないはずだ。
	p_ht->i_size++;		//┬この処理順序は必須です。
	ght_check_rehash(p_ht);	//┘
	return 0;
}
/*--------------------------------------------------------------------------*/
//Get an entry from the hash table.
void* ght_get(ght_hash_table_t* p_ht, int i_key_size, const void* p_key_data) {
	ght_hash_entry_t* p_e;
	p_e = ght_search_in_bucket(p_ht, i_key_size, p_key_data);
	if(!p_e) { return NULL; }
	return p_e->p_entry_data;
}
/*--------------------------------------------------------------------------*/
//Replace an entry from the hash table.
void* ght_replace(ght_hash_table_t* p_ht, void* p_entry_data, int i_key_size, const void* p_key_data) {
	ght_hash_entry_t* p_e;
	void* p_old_data;
	p_e = ght_search_in_bucket(p_ht, i_key_size, p_key_data);
	if(!p_e) { return NULL; }
	p_old_data = p_e->p_entry_data;
	p_e->p_entry_data = p_entry_data;
	return p_old_data;
}
/*--------------------------------------------------------------------------*/
//Remove an entry from the hash table.
void* ght_remove(ght_hash_table_t* p_ht, int i_key_size, const void* p_key_data) {
	ght_hash_entry_t* p_e;
	void* p_old_data;
	p_e = ght_search_in_bucket(p_ht, i_key_size, p_key_data);
	if(!p_e) { return NULL; }
	RemoveEntryList(&p_e->le_iterator);
	RemoveEntryList(&p_e->le_bucket);
	p_old_data = p_e->p_entry_data;
	if(p_ht->fn_free) {
		(*p_ht->fn_free)(p_e);
	} else {
		free(            p_e);	//'(p_ht->fn_free?p_ht->fn_free:free)(p_e)'は不可です。leak_detector.hによる関数置き換えの対象とするために、malloc/freeを名前で使わずに関数呼び出し形式で使う必要が有ります。詳細は、garray.cの'Wed Apr 20 10:51:26 JST 2016'のコメントを参照して下さい。
	}
	p_ht->i_size--;		//┬この処理順序は必須です。
	ght_check_rehash(p_ht);	//┘
	return p_old_data;
}
/*--------------------------------------------------------------------------*/
//Get the first entry in an iteration.
void* ght_first(ght_hash_table_t* p_ht, ght_iterator_t* p_iterator, const void** pp_key_data/*NULL可*/) {
	return ght_first_keysize(p_ht, p_iterator, pp_key_data, NULL);
}
void* ght_first_keysize(ght_hash_table_t* p_ht, ght_iterator_t* p_iterator, const void** pp_key_data/*NULL可*/, int* p_key_size/*NULL可*/) {
	LIST_ENTRY* list_head = &p_ht->le_iterator;
	LIST_ENTRY* list_entry = list_head->Flink;
	p_iterator->le_next = list_entry;
	return ght_next_keysize(p_ht, p_iterator, pp_key_data, p_key_size);
}
/*--------------------------------------------------------------------------*/
//Get the next entry in an iteration.
void* ght_next(ght_hash_table_t* p_ht, ght_iterator_t* p_iterator, const void** pp_key_data/*NULL可*/) {
	return ght_next_keysize(p_ht, p_iterator, pp_key_data, NULL);
}
void* ght_next_keysize(ght_hash_table_t* p_ht, ght_iterator_t* p_iterator, const void** pp_key_data/*NULL可*/, int* p_key_size/*NULL可*/) {
	LIST_ENTRY* list_head = &p_ht->le_iterator;
	LIST_ENTRY* list_entry = p_iterator->le_next;
	if(list_entry != list_head) {
		ght_hash_entry_t* p_e = CONTAINING_RECORD(list_entry, ght_hash_entry_t, le_iterator);
		p_iterator->le_next = list_entry->Flink;											//イテレータは次のノードを指しているので、イテレーションの中で当該ノードをght_remove()しても安全です。
		if(pp_key_data) { *pp_key_data = p_e->key_data; }
		if(p_key_size) { *p_key_size = p_e->i_key_size; }
		return p_e->p_entry_data;
	}
	return NULL;
}
/*--------------------------------------------------------------------------*/
//Finalize (free) a hash table.
void ght_finalize(ght_hash_table_t* p_ht) {
	ght_iterator_t iterator;
	void* p_entry_data;
	const void* p_key_data;
	int i_key_size;
	for(p_entry_data = ght_first_keysize(p_ht, &iterator, &p_key_data, &i_key_size);
	    p_entry_data;
	    p_entry_data = ght_next_keysize( p_ht, &iterator, &p_key_data, &i_key_size)) { ght_remove(p_ht, i_key_size, p_key_data); }		//イテレータは次のノードを指しているので、イテレーションの中で当該ノードをght_remove()しても安全です。
	free(p_ht->le_bucket);	//ここではp_ht->fn_alloc,p_ht->fn_freeを使用しない事に注意して下さい。
	free(p_ht);		//ここではp_ht->fn_alloc,p_ht->fn_freeを使用しない事に注意して下さい。
}
/*--------------------------------------------------------------------------*/
//Set the rehashing status of the table.
void ght_set_rehash(ght_hash_table_t* p_ht, int b_rehash) {
	p_ht->flag.b_rehash = b_rehash;
}
/*--------------------------------------------------------------------------*/
//Rehash the hash table.
void ght_rehash(ght_hash_table_t* p_ht, int i_table_size) {
	LIST_ENTRY* list_head = &p_ht->le_iterator;
	LIST_ENTRY* list_entry = list_head->Flink;
	int i, l_key;
	//Reallocate the new bucket with the new size.
	if(i_table_size < 1) { i_table_size = 1; }
	p_ht->i_table_size = i_table_size;
	p_ht->le_bucket = realloc(p_ht->le_bucket, sizeof(LIST_ENTRY) * i_table_size);	//ここではp_ht->fn_alloc,p_ht->fn_freeを使用しない事に注意して下さい。
	if(!p_ht->le_bucket) { DIE(); }
	for(i = 0; i < i_table_size; i++) { InitializeListHead(&p_ht->le_bucket[i]); }
	//Walk through all elements in the table and insert them into the new bucket.
	while(list_entry != list_head) {
		ght_hash_entry_t* p_e = CONTAINING_RECORD(list_entry, ght_hash_entry_t, le_iterator);
		l_key = ght_get_hash_value(p_ht, p_e->i_key_size, p_e->key_data);
		InsertHeadList(&p_ht->le_bucket[l_key], &p_e->le_bucket);	//キャッシュ対応のために、バケットのリストへの追加位置は先頭に追加するように変更した。既存のアプリケーションはバケットのリストを直接参照していないので、この変更による互換性の問題は生じないはずだ。
		list_entry = list_entry->Flink;
	}
}
/*--------------------------------------------------------------------------*/
//Enable or disable bounded buckets.
void ght_set_bounded_buckets(ght_hash_table_t* p_ht, int bucket_limit, void (*fn_bucket_free)(void* data, const void* key)) {
	if((bucket_limit < 0) ||
	  (!bucket_limit && fn_bucket_free)) { DIE(); }
	p_ht->bucket_limit = bucket_limit;
	p_ht->fn_bucket_free = fn_bucket_free;
}
/*--------------------------------------------------------------------------*/
//Set the allocation/freeing functions to use for a hash table.
void ght_set_alloc(ght_hash_table_t* p_ht, void* (*fn_alloc)(size_t size), void (*fn_free)(void* ptr)) {
	p_ht->fn_alloc = fn_alloc;
	p_ht->fn_free = fn_free;
}
