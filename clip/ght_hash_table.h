/*
 *	ght_hash_table.h
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
 *	* Wed Nov 18 21:37:53 JST 2015 Naoyuki Sawa
 *	- 'extern "C" {'～'}'で囲みました。.cppを含むWin32プロジェクトからもインクルード出来るようにするためです。
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
 */
#ifndef __GHT_HASH_TABLE_H__
#define __GHT_HASH_TABLE_H__
#ifdef  __cplusplus
extern "C" {
#endif//__cplusplus
/****************************************************************************
 *	
 ****************************************************************************/
typedef struct s_hash_entry {
	LIST_ENTRY		le_iterator;
	LIST_ENTRY		le_bucket;
	void*			p_entry_data;			//Entry data.
	int			i_key_size;			//The size in bytes of the key.
#ifndef __RENESAS__
	unsigned char		key_data[0/*i_key_size*/];	//Key data.
#else //__RENESAS__
	unsigned char		key_data[1/*i_key_size*/];	//Key data.		//Hewは要素数[0]の配列に対応していないので、要素数[1]として調整しました。
#endif//__RENESAS__
} ght_hash_entry_t;
/*--------------------------------------------------------------------------*/
typedef struct {
	LIST_ENTRY*		le_next;			//The current entry.		//イテレータは次のノードを指しているので、イテレーションの中で当該ノードをght_remove()しても安全です。使用例はght_finalize()の実装を参照してください。
} ght_iterator_t;
/*--------------------------------------------------------------------------*/
typedef struct {
	struct {
	  unsigned		b_rehash	:1;		//Automatic rehashing status.
	} flag;
	int			i_size;				//The current number of items in the table.
	LIST_ENTRY		le_iterator;
	int			i_table_size;			//The number of buckets.
	LIST_ENTRY*		le_bucket/*[i_table_size]*/;
	int			bucket_limit;			//The maximum number of items in each bucket. If limit is set to 0, bounded buckets are disabled.
	void  (*fn_bucket_free)(void* data, const void* key);	//The function called when a bucket overflows.
	void* (*fn_alloc)(size_t size);				//The function used for allocating entries.		Set the allocation/freeing functions to use for a hash table. The allocation function will only be called when a new entry is inserted. The allocation size will always be sizeof(ght_hash_entry_t) + sizeof(ght_hash_key_t) + key_size. The actual size varies with the key size. If this function is not called, malloc() and free() will be used for allocation and freeing. Always call this function before any entries are inserted into the table. Otherwise, the new free() might be called on something that were allocated with another allocation function.
	void  (*fn_free)(void* ptr);				//The function used for freeing entries.		//                                                                                                                                                                                                    xxxxxxxxxxxxxxxxxxxxxx オリジナル版の説明は上記の通りですが、当実装では'sizeof(ght_hash_key_t)'は有りません。確保するサイズは常に'sizeof(ght_hash_entry_t) + key_size'となります。key_sizeが固定ならば確保するサイズも固定なので、メモリプールを利用してメモリ確保を高速化出来ます。
} ght_hash_table_t;
/****************************************************************************
 *	
 ****************************************************************************/
ght_hash_table_t* ght_create(int i_table_size);
int ght_size(ght_hash_table_t* p_ht);
int ght_table_size(ght_hash_table_t* p_ht);
int ght_insert(ght_hash_table_t* p_ht, void* p_entry_data, int i_key_size, const void* p_key_data);
void* ght_replace(ght_hash_table_t* p_ht, void* p_entry_data, int i_key_size, const void* p_key_data);
void* ght_get(ght_hash_table_t* p_ht, int i_key_size, const void* p_key_data);
void* ght_remove(ght_hash_table_t* p_ht, int i_key_size, const void* p_key_data);
void* ght_first(ght_hash_table_t* p_ht, ght_iterator_t* p_iterator, const void** pp_key_data/*NULL可*/);
void* ght_first_keysize(ght_hash_table_t* p_ht, ght_iterator_t* p_iterator, const void** pp_key_data/*NULL可*/, int* p_key_size/*NULL可*/);
void* ght_next(ght_hash_table_t* p_ht, ght_iterator_t* p_iterator, const void** pp_key_data/*NULL可*/);
void* ght_next_keysize(ght_hash_table_t* p_ht, ght_iterator_t* p_iterator, const void** pp_key_data/*NULL可*/, int* p_key_size/*NULL可*/);
void ght_finalize(ght_hash_table_t* p_ht);
void ght_set_rehash(ght_hash_table_t* p_ht, int b_rehash);
void ght_rehash(ght_hash_table_t* p_ht, int i_table_size);
void ght_set_bounded_buckets(ght_hash_table_t* p_ht, int bucket_limit, void (*fn_bucket_free)(void* data, const void* key));
void ght_set_alloc(ght_hash_table_t* p_ht, void* (*fn_alloc)(size_t size), void (*fn_free)(void* ptr));
/*--------------------------------------------------------------------------*/
//未対応の関数と、未対応とした理由は、以下の通りです:
//・ght_set_hash		One-at-a-Time Hash固定で充分だから。
//・ght_set_heuristics		速度性能は重視しないから。
//・ght_one_at_a_time_hash	One-at-a-Time Hash固定で充分だから。
//・ght_rotating_hash		One-at-a-Time Hash固定で充分だから。
//・ght_crc_hash		One-at-a-Time Hash固定で充分だから。
//・ght_print			デバッグ機能が必要な程複雑ではない。
#ifdef  __cplusplus
}//extern "C"
#endif//__cplusplus
#endif//__GHT_HASH_TABLE_H__
