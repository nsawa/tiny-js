/*
 *	ght_hash_table.h
 *
 *	Generic Hash Table (GHT)
 *
 *	* Sat Jun 06 15:36:36 JST 2015 Naoyuki Sawa
 *	- 1st �����[�X�B
 *	- �ulibghthash - Generic Hash Table (GHT)�v(http://www.bth.se/people/ska/sim_home/libghthash.html)�݊����W���[���ł��B
 *	  �֐��d�l��libghthash�Ɠ����ŁA�����͓Ǝ��ł��B
 *	  �I���W�i���ł�libghthash�͑��x���\���d�����Ď�������Ă���悤�ł����A�������͏ȃ�������D�悵�ĒP���Ɏ������܂����B
 *	- �֐��d�l��g�p��́A�I���W�i���ł�libghthash�̃h�L�������g���Q�Ƃ��Ă��������B
 *	  �I���W�i���ł�libghthash���A/clip/keep/libghthash.7z �ɕۑ����Ă����܂����B
 *	* Sun Jul 12 21:50:47 JST 2015 Naoyuki Sawa
 *	- ght_set_rehash(),ght_rehash()��Ή����܂����B
 *	  ���x���\���d�����Ȃ����j�ł����̊֐��ɂ͑Ή����Ă��܂���ł������A���������������P���邽�߂ɑΉ����鎖�ɂ��܂����B
 *	  ����܂ł́Aght_create()�̈����ɗL����x�傫�Ȓl���w�肷��K�v���L��A���ۂ̗v�f�������Ȃ����ɖ��ʂ������Ă��܂����B
 *	  ����́A���'ght_create(0);ght_set_rehash(p_ht,1)'�ŗǂ��A�v�f���̑����ɉ����Ď����I�Ƀe�[�u���T�C�Y����������܂��B
 *	* Tue Oct 06 21:24:18 JST 2015 Naoyuki Sawa
 *	- ght_size()�����������܂����B
 *	  ����܂ł�RAM�ߖ���ŗD�悵��i_size�t�B�[���h��ێ������ɁAght_size()���ďo�����x��le_iterator��H���ėv�f���𐔂��Ă��܂����B
 *	  ����̕ύX��i_size�t�B�[���h��ǉ����āAght_size()��i_size��Ԃ������ɂ��č��������܂����B
 *	  �͂���RAM����ʂ������܂����AP/ECE�̏ꍇ�̓v���O������RAM�ɓW�J����̂ŁAght_size()�̃R�[�h�T�C�Y�ጸ�Ŏ���RAM����ʂ�����܂��B
 *	* Wed Nov 18 21:37:53 JST 2015 Naoyuki Sawa
 *	- 'extern "C" {'�`'}'�ň݂͂܂����B.cpp���܂�Win32�v���W�F�N�g������C���N���[�h�o����悤�ɂ��邽�߂ł��B
 *	* Thu Apr 28 21:03:22 JST 2016 Naoyuki Sawa
 *	- ght_set_bounded_buckets()��Ή����܂����B
 *	- �L���b�V���Ƃ��Ă̓���ɂ��G���g���̎����폜���s���̂́Aght_insert()���Ăяo���ꂽ�������Ƃ����d�l�ł��B(�I���W�i���Ń\�[�X�̓���ɏ������܂����B)
 *	  ght_set_rehash()���Ăяo���ăo�P�b�g�̃G���g��������������Aght_set_bounded_buckets()���Ăяo���ăo�P�b�g���~�b�g�����������Ƃɂ��A
 *	  �o�P�b�g�̃G���g�������o�P�b�g���~�b�g�𒴂����Ƃ��Ă��A���̎��_�ł̓G���g���̎����폜�͍s���܂���B
 *	  �G���g���̎����폜���s���̂́A����ȍ~��ght_insert()���Ăяo����A���̎��ɎQ�Ƃ����o�P�b�g���o�P�b�g���~�b�g�𒴂��Ă������ł��鎖�ɒ��ӂ��ĉ������B
 *	- �܂��A���ϓI�ɗ����o����Ǝv���܂����A�L���b�V���Ƃ��Ă̓���ƁA�������n�b�V���̓���́A�����������ł��B
 *	  �o�P�b�g���~�b�g�𒴂��ăG���g���̎����폜���s����̂����҂��Ă���̂ɁA���̑O�Ɏ������n�b�V���������ăo�P�b�g���������A
 *	  �o�P�b�g������̃G���g��������A���܂Ōo���Ă��G���g���̎����폜���s���Ȃ��\������������ł��B(�o�P�b�g���~�b�g�̐ݒ�l�ɂ����܂����c)
 *	- �ȏ���܂Ƃ߂�ƁA���_�Ƃ��Ă�:
 *	  ���L���b�V���Ƃ��Ďg�p����ꍇ�́A�]�܂����菇
 *	  ���o�P�b�g���𖾎����ăn�b�V���e�[�u�����쐬���A���̒���Ƀo�P�b�g���~�b�g���w�肵�ăL���b�V����L�������A
 *	  ������ȍ~�́A�o�P�b�g���~�b�g�̕ύX���A�������n�b�V���̗L�������s��Ȃ��̂��ǂ��B
 *	  ��<��>
 *	  ��ght_hash_table_t* p_ht = ght_create(17);
 *	  ��ght_set_bounded_buckets(p_ht, 8, fnBucketFree);
 *	  ��//����ȍ~�Aght_set_bounded_buckets()��ght_set_rehash()���Ăяo���Ȃ��B
 *	- ght_set_bounded_buckets()���Ăяo���ăL���b�V����L��������ƁAght_insert()���������x�ቺ���鎖�ɒ��ӂ��ĉ������B
 *	  �S�̂̃G���g������i_size�t�B�[���h�ɕێ����Ă��܂����A�o�P�b�g���̃G���g�����͕ێ����Ă��Ȃ��̂ŁA�o�P�b�g���𔻒肷�邽�߂ɓs�x�������邩��ł��B
 *	  ght_insert()�̑��x�ቺ��h���ɂ́A�o�P�b�g���ɃG���g������ێ�����悤�ɂ���Ηǂ��̂ł����A����͍s��Ȃ����ɂ��܂����B���R�́A�ȉ��̒ʂ�ł��B
 *	  �E���R1. �o�P�b�g���̃G���g�����͋ɒ[�ɑ����Ȃ��Ǝv���̂ŁA�s�x���삵�Ă��ɒ[�ɒx���͂Ȃ��Ǝv������B
 *	           ���X�n�b�V���e�[�u�����g�p����ړI���������̂��߂ł�����A��̃o�P�b�g�ɋɒ[�ɑ����̃G���g����ێ�����̂͌����I�ł͂Ȃ��A
 *	           ght_set_bounded_buckets()�Ɏw�肷��p�P�b�g���~�b�g�̒l�́A��������1���`2�����炢�̌��ł���͂��ł��B
 *	           ���ꂮ�炢�̐��Ȃ�΁Aght_insert()�̓s�x�������Ă��A���قǑ傫�Ȗ��ɂ͂Ȃ�Ȃ��Ǝv���܂����B
 *	           ���ۂ̂Ƃ���A�����ght_get()���q�b�g���Ȃ����ɑ�������G���g�����Ɠ����ł�����A����Ȃɒx���͖����͂��ł��B
 *	  �E���R2. ght_insert()�̌Ăяo���p�x�́A�����Ȃ��Ǝv������B
 *	           ��ʓI�ɁA�Q�Ƃ����ύX�̕������Ȃ��Ǝv���̂ŁAght_get()�ɔ�ׂ��ght_insert()�̌Ăяo���񐔂͏��Ȃ��Ǝv���܂��B
 *	           �]���āAght_insert()���������x�ቺ���Ă��A�S�̂ւ̉e���͏��Ȃ��Ǝv���܂����B
 *	  �E���R3. �L���b�V����L�������Ȃ���΁Aght_insert()�̑��x�ቺ�͋N����Ȃ����B
 *	           ght_insert()���o�P�b�g�𑖍�����̂́Aght_set_bounded_buckets()���Ăяo���ăL���b�V����L���������ꍇ�����ł��B
 *	           ���ۂɂ́A�L���b�V����L���������Ɏg�����̕��������Ǝv���̂ŁAght_insert()�����x�ቺ���鎖�͏��Ȃ��Ǝv���܂��B
 *	  �E���R4. �o�P�b�g���ɃG���g������ێ�����ƁA����������ʂ������Ă��܂����B
 *	           ���R�Ȃ���A�o�P�b�g���ɃG���g������ێ�����ƁA���̂��߂̕ϐ����K�v�ɂȂ�A�펞����������ʂ������܂��B
 *	           ���R1,2,3�̒ʂ�Aght_insert()�̑��x�ቺ�ɂ��e���͏��Ȃ��̂ɁA��������P���邽�߂ɏ펞����������ʂ�������͖̂��ʂ��Ǝv���܂����B
 *	           ��������A�L���b�V����L��������������ght_insert()���������x�ቺ���鎖���󂯓��������A�ǂ��Ǝv���܂����B
 *	  �ȏ�̗��R�ɂ��Aght_insert()�̑��x�ቺ��h���΍�͍s��Ȃ����ɂ��܂����B
 *	  ���΂炭�g���Ă݂āA������肪�L��΍Č������悤�Ǝv���܂����A�����炭���̂܂܂Ŗ�薳���Ǝv���܂��B
 *	* Sat Apr 30 21:45:42 JST 2016 Naoyuki Sawa
 *	- ght_set_alloc()��Ή����܂����B
 *	* Fri May 06 21:58:10 JST 2016 Naoyuki Sawa
 *	- �C�e���[�V�����p�̃��X�g�̒ǉ��ʒu��ύX���Ȃ����R�ɂ��āAght_insert()���ɃR�����g��ǋL���܂����B
 *	  �R�[�h�ɂ͕ύX�L��܂���B
 *	* Sun Jun 05 21:20:53 JST 2016 Naoyuki Sawa
 *	- ght_first(),ght_first_keysize(),ght_next(),ght_next_keysize()�́App_key_data������NULL�Ƃ��܂����B
 *	  �ύX�������R�́A�ȉ��̒ʂ�ł��B
 *	- �I���W�i���ł�libghthash�ł́Ap_key_size������NULL�ł����App_key_data������NULL�s�ł����B
 *	  ���������ۂɎg���Ă݂�ƁA�񋓒��ɃL�[�͕s�v�ł���P�[�X�������App_key_data�������w�肷�邽�߂����Ƀ_�~�[�ϐ���p�ӂ��Ȃ���΂����Ȃ������L��܂����B
 *	  pp_key_data������NULL�Ƃ���́A��L�̎�Ԃ��Ȃ������o����̂ŁApp_key_data������NULL�Ƃ��鎖�ɂ��܂����B
 *	- ���̕ύX�́A�I���W�i���ł�libghthash�Ǝd�l����݊��ɂȂ��Ă��܂������L��̂ł����A��ʌ݊������狖�e���ėǂ��Ǝv���܂��B
 *	  �����A�I���W�i���ł�libghthash�̎d�l�������Ɏ��K�v���L��ꍇ�́A�A�v���P�[�V�������App_key_data������NULL���w�肵�Ȃ��悤�ɒ��ӂ���Ηǂ�����ł��B
 *	* Sat Oct 01 21:11:57 JST 2016 Naoyuki Sawa
 *	- Hew�͗v�f��[0]�̔z��ɑΉ����Ă��Ȃ��̂ŁA�v�f��[1]�Ƃ��Ē������܂����B
 *	  P/ECE�J����(GCC)��Windows(VC++6.0)�́A�v�f��[0]�̔z��ɑΉ����Ă���̂ŁA�֌W�L��܂���B
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
	unsigned char		key_data[1/*i_key_size*/];	//Key data.		//Hew�͗v�f��[0]�̔z��ɑΉ����Ă��Ȃ��̂ŁA�v�f��[1]�Ƃ��Ē������܂����B
#endif//__RENESAS__
} ght_hash_entry_t;
/*--------------------------------------------------------------------------*/
typedef struct {
	LIST_ENTRY*		le_next;			//The current entry.		//�C�e���[�^�͎��̃m�[�h���w���Ă���̂ŁA�C�e���[�V�����̒��œ��Y�m�[�h��ght_remove()���Ă����S�ł��B�g�p���ght_finalize()�̎������Q�Ƃ��Ă��������B
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
	void  (*fn_free)(void* ptr);				//The function used for freeing entries.		//                                                                                                                                                                                                    xxxxxxxxxxxxxxxxxxxxxx �I���W�i���ł̐����͏�L�̒ʂ�ł����A�������ł�'sizeof(ght_hash_key_t)'�͗L��܂���B�m�ۂ���T�C�Y�͏��'sizeof(ght_hash_entry_t) + key_size'�ƂȂ�܂��Bkey_size���Œ�Ȃ�Ίm�ۂ���T�C�Y���Œ�Ȃ̂ŁA�������v�[���𗘗p���ă������m�ۂ��������o���܂��B
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
void* ght_first(ght_hash_table_t* p_ht, ght_iterator_t* p_iterator, const void** pp_key_data/*NULL��*/);
void* ght_first_keysize(ght_hash_table_t* p_ht, ght_iterator_t* p_iterator, const void** pp_key_data/*NULL��*/, int* p_key_size/*NULL��*/);
void* ght_next(ght_hash_table_t* p_ht, ght_iterator_t* p_iterator, const void** pp_key_data/*NULL��*/);
void* ght_next_keysize(ght_hash_table_t* p_ht, ght_iterator_t* p_iterator, const void** pp_key_data/*NULL��*/, int* p_key_size/*NULL��*/);
void ght_finalize(ght_hash_table_t* p_ht);
void ght_set_rehash(ght_hash_table_t* p_ht, int b_rehash);
void ght_rehash(ght_hash_table_t* p_ht, int i_table_size);
void ght_set_bounded_buckets(ght_hash_table_t* p_ht, int bucket_limit, void (*fn_bucket_free)(void* data, const void* key));
void ght_set_alloc(ght_hash_table_t* p_ht, void* (*fn_alloc)(size_t size), void (*fn_free)(void* ptr));
/*--------------------------------------------------------------------------*/
//���Ή��̊֐��ƁA���Ή��Ƃ������R�́A�ȉ��̒ʂ�ł�:
//�Eght_set_hash		One-at-a-Time Hash�Œ�ŏ[��������B
//�Eght_set_heuristics		���x���\�͏d�����Ȃ�����B
//�Eght_one_at_a_time_hash	One-at-a-Time Hash�Œ�ŏ[��������B
//�Eght_rotating_hash		One-at-a-Time Hash�Œ�ŏ[��������B
//�Eght_crc_hash		One-at-a-Time Hash�Œ�ŏ[��������B
//�Eght_print			�f�o�b�O�@�\���K�v�Ȓ����G�ł͂Ȃ��B
#ifdef  __cplusplus
}//extern "C"
#endif//__cplusplus
#endif//__GHT_HASH_TABLE_H__
