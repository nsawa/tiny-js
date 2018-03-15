/*
 *	ght_hash_table.c
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
 *	- ght_insert()�ɂ����āA�ϐ�'p_e'�����ʂɓ�d�ɒ�`����Ă����̂��C�����܂����B
 *	  �����͂��̂܂܂ł���肠��܂��񂵁AP/ECE�J����(GCC)��Windows(VC++6.0,VS2015)�ł͌x�����o�Ȃ��̂ł����AHew�ŁuC6348 (I) Declaration hides variable "p_e"�v�Ƃ����x�����o������ł��B
 *	  �m���ɖ��ʂ������̂ŁA�C�����鎖�ɂ��܂����B
 *	  ����̏C���ɂ�铮��̕ω��͗L��܂���B
 */
#include "clip.h"
/****************************************************************************
 *	���[�J���֐�
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
		    //	//���̃n�b�V���e�[�u�����A�L���b�V���Ƃ��ē��쒆�Ȃ�΁c	//��
		    //	if(p_ht->bucket_limit) {					//�����̃n�b�V���e�[�u�����L���b�V���Ƃ��ē��쒆���ۂ��Ɋ֌W�����A�Q�Ƃ����G���g�������̃o�P�b�g�̐擪�ֈړ����鎖�ɂ����B
				//���̃G���g�����A���̃o�P�b�g�̐擪�ֈړ�����B	//���L���b�V���Ƃ��ē��쒆�łȂ��Ă��A�p�ɂɎQ�Ƃ���G���g���̌����������Ȃ�Ƃ������_�����邩��ł��B
				RemoveEntryList(&p_e->le_bucket);			//�����Aght_remove()����Ăяo���ꂽ�ꍇ�͖��ʏ����ł����A����ɂ͉e���L��܂���B
				InsertHeadList(list_head, &p_e->le_bucket);		//��
		    //	}								//��
			return p_e;
		}
		list_entry = list_entry->Flink;
	}
	return NULL;
}
/*--------------------------------------------------------------------------*/
static void ght_check_rehash(ght_hash_table_t* p_ht) {
	if(p_ht->flag.b_rehash) {
		//���݂̗v�f�����擾����B�������A�v�f����(0)�̏ꍇ��(1)�Ƃ��āA�ȉ��̌v�Z���s���B
		int i_size = p_ht->i_size;
		if(!i_size) { i_size = 1; }
		//���݂̃e�[�u���T�C�Y���A�v�f���̔�������,����,��{���߂Ȃ�΁A�v�f���ɍ��킹��B
		if((p_ht->i_table_size < (i_size / 2)) ||
		   (p_ht->i_table_size > (i_size * 2))) { ght_rehash(p_ht, i_size); }
		//�ȏ�̏����ɂ���āA�e�[�u���T�C�Y���v�f���̔����ȏ�`��{�ȉ��͈̔͂ɒǏ]����B
	}
}
/****************************************************************************
 *	�O���[�o���֐�
 ****************************************************************************/
//Create a new hash table.
ght_hash_table_t* ght_create(int i_table_size) {
	ght_hash_table_t* p_ht;
	p_ht = calloc(sizeof(ght_hash_table_t), 1);	//�����ł�p_ht->fn_alloc,p_ht->fn_free���g�p���Ȃ����ɒ��ӂ��ĉ������B
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
	//���̃n�b�V���e�[�u�����A�L���b�V���Ƃ��ē��쒆�Ȃ�΁c
	if(p_ht->bucket_limit) {
		LIST_ENTRY* list_head = &p_ht->le_bucket[l_key];
		LIST_ENTRY* list_entry = list_head->Flink;
		//���̃o�P�b�g�̐擪����(bucket_limit-1)�𒴂���o�P�b�g��S�č폜����B
		int i = p_ht->bucket_limit;
		while(list_entry != list_head) {
//{{2016/10/01�C��:ght_insert()�ɂ����āA�ϐ�'p_e'�����ʂɓ�d�ɒ�`����Ă����̂��C�����܂����B
//			ght_hash_entry_t* p_e = CONTAINING_RECORD(list_entry, ght_hash_entry_t, le_bucket);
//��2016/10/01�C��:ght_insert()�ɂ����āA�ϐ�'p_e'�����ʂɓ�d�ɒ�`����Ă����̂��C�����܂����B
			p_e = CONTAINING_RECORD(list_entry, ght_hash_entry_t, le_bucket);
//}}2016/10/01�C��:ght_insert()�ɂ����āA�ϐ�'p_e'�����ʂɓ�d�ɒ�`����Ă����̂��C�����܂����B
			list_entry = list_entry->Flink;
			if(--i <= 0) {	//bucket_limit�ڂ��폜�ΏۂƂ���̂Ńv���C���N�������g�ł��B
				RemoveEntryList(&p_e->le_iterator);
				RemoveEntryList(&p_e->le_bucket);
				if(p_ht->fn_bucket_free) { (*p_ht->fn_bucket_free)(p_e->p_entry_data, p_e->key_data); }
				if(p_ht->fn_free) {
					(*p_ht->fn_free)(p_e);
				} else {
					free(            p_e);	//'(p_ht->fn_free?p_ht->fn_free:free)(p_e)'�͕s�ł��Bleak_detector.h�ɂ��֐��u�������̑ΏۂƂ��邽�߂ɁAmalloc/free�𖼑O�Ŏg�킸�Ɋ֐��Ăяo���`���Ŏg���K�v���L��܂��B�ڍׂ́Agarray.c��'Wed Apr 20 10:51:26 JST 2016'�̃R�����g���Q�Ƃ��ĉ������B
				}
				p_ht->i_size--;	//�Y��Ȃ���!!
			}
		}
	}
#ifndef __RENESAS__
//��Win32��P/ECE�̏ꍇ
	if(p_ht->fn_alloc) {
		p_e = (*p_ht->fn_alloc)(sizeof(ght_hash_entry_t) + i_key_size);
	} else {
		p_e = malloc(           sizeof(ght_hash_entry_t) + i_key_size);	//'(p_ht->fn_alloc?p_ht->fn_alloc:malloc)(�`)'�͕s�ł��Bleak_detector.h�ɂ��֐��u�������̑ΏۂƂ��邽�߂ɁAmalloc/free�𖼑O�Ŏg�킸�Ɋ֐��Ăяo���`���Ŏg���K�v���L��܂��B�ڍׂ́Agarray.c��'Wed Apr 20 10:51:26 JST 2016'�̃R�����g���Q�Ƃ��ĉ������B
	}
#else //__RENESAS__
//��Hew�̏ꍇ
	if(p_ht->fn_alloc) {
		p_e = (*p_ht->fn_alloc)(offsetof(ght_hash_entry_t, key_data) + ((i_key_size + 3) & ~3));	//Hew�͗v�f��[0]�̔z��ɑΉ����Ă��Ȃ��̂ŁA�v�f��[1]�Ƃ��Ē������܂����B
	} else {
		p_e = malloc(           offsetof(ght_hash_entry_t, key_data) + ((i_key_size + 3) & ~3));	//Hew�͗v�f��[0]�̔z��ɑΉ����Ă��Ȃ��̂ŁA�v�f��[1]�Ƃ��Ē������܂����B
	}
#endif//__RENESAS__
	if(!p_e) { DIE(); }
	p_e->p_entry_data = p_entry_data;
	p_e->i_key_size = i_key_size;
	memcpy(p_e->key_data, p_key_data, i_key_size);
	InsertTailList(&p_ht->le_iterator, &p_e->le_iterator);		//�C�e���[�V�����p�̃��X�g�𒼐ڎQ�Ƃ��Ă�������̃A�v���P�[�V�����Ƃ̌݊����̂��߂ɁA�C�e���[�V�����p�̃��X�g�ւ̒ǉ��ʒu�͂���܂Œʂ薖���ɒǉ����鎖�ɂ����B	{{2016/05/06�R�����g�ǋL:�{���́A�v�f�̒ǉ����ƃC�e���[�V�����p�̃��X�g�̏����͖��֌W�ł���ׂ��Ȃ̂����A�����W���[���̎����Ɉˑ����ă��X�g�̖����ɒǉ�����鎖�����҂��������ɂȂ��Ă��܂��Ă��郂�W���[�������݂���(clipstr.c��intern_string()�BSqliteHelper.c����������?)�B�]�܂����͂Ȃ��̂����A�m���ɒǉ����ɃC�e���[�g�����ƕ֗��ȏ�ʂ������̂ŁA���̂܂܂ɂ��鎖�����B���A�C�e���[�g�O�ɖ����I�Ƀ\�[�g���Ă���(������d�l�O�ł͂���̂���)�ꍇ�́A���X�ǉ����ɂ͉e������Ȃ��C�e���[�g���ƂȂ��Ă���̂ł��̌��Ƃ͖��֌W�ł�(clipwkst.c��)�B}}
	InsertHeadList(&p_ht->le_bucket[l_key], &p_e->le_bucket);	//�L���b�V���Ή��̂��߂ɁA�o�P�b�g�̃��X�g�ւ̒ǉ��ʒu�͐擪�ɒǉ�����悤�ɕύX�����B�����̃A�v���P�[�V�����̓o�P�b�g�̃��X�g�𒼐ڎQ�Ƃ��Ă��Ȃ��̂ŁA���̕ύX�ɂ��݊����̖��͐����Ȃ��͂����B
	p_ht->i_size++;		//�����̏��������͕K�{�ł��B
	ght_check_rehash(p_ht);	//��
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
		free(            p_e);	//'(p_ht->fn_free?p_ht->fn_free:free)(p_e)'�͕s�ł��Bleak_detector.h�ɂ��֐��u�������̑ΏۂƂ��邽�߂ɁAmalloc/free�𖼑O�Ŏg�킸�Ɋ֐��Ăяo���`���Ŏg���K�v���L��܂��B�ڍׂ́Agarray.c��'Wed Apr 20 10:51:26 JST 2016'�̃R�����g���Q�Ƃ��ĉ������B
	}
	p_ht->i_size--;		//�����̏��������͕K�{�ł��B
	ght_check_rehash(p_ht);	//��
	return p_old_data;
}
/*--------------------------------------------------------------------------*/
//Get the first entry in an iteration.
void* ght_first(ght_hash_table_t* p_ht, ght_iterator_t* p_iterator, const void** pp_key_data/*NULL��*/) {
	return ght_first_keysize(p_ht, p_iterator, pp_key_data, NULL);
}
void* ght_first_keysize(ght_hash_table_t* p_ht, ght_iterator_t* p_iterator, const void** pp_key_data/*NULL��*/, int* p_key_size/*NULL��*/) {
	LIST_ENTRY* list_head = &p_ht->le_iterator;
	LIST_ENTRY* list_entry = list_head->Flink;
	p_iterator->le_next = list_entry;
	return ght_next_keysize(p_ht, p_iterator, pp_key_data, p_key_size);
}
/*--------------------------------------------------------------------------*/
//Get the next entry in an iteration.
void* ght_next(ght_hash_table_t* p_ht, ght_iterator_t* p_iterator, const void** pp_key_data/*NULL��*/) {
	return ght_next_keysize(p_ht, p_iterator, pp_key_data, NULL);
}
void* ght_next_keysize(ght_hash_table_t* p_ht, ght_iterator_t* p_iterator, const void** pp_key_data/*NULL��*/, int* p_key_size/*NULL��*/) {
	LIST_ENTRY* list_head = &p_ht->le_iterator;
	LIST_ENTRY* list_entry = p_iterator->le_next;
	if(list_entry != list_head) {
		ght_hash_entry_t* p_e = CONTAINING_RECORD(list_entry, ght_hash_entry_t, le_iterator);
		p_iterator->le_next = list_entry->Flink;											//�C�e���[�^�͎��̃m�[�h���w���Ă���̂ŁA�C�e���[�V�����̒��œ��Y�m�[�h��ght_remove()���Ă����S�ł��B
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
	    p_entry_data = ght_next_keysize( p_ht, &iterator, &p_key_data, &i_key_size)) { ght_remove(p_ht, i_key_size, p_key_data); }		//�C�e���[�^�͎��̃m�[�h���w���Ă���̂ŁA�C�e���[�V�����̒��œ��Y�m�[�h��ght_remove()���Ă����S�ł��B
	free(p_ht->le_bucket);	//�����ł�p_ht->fn_alloc,p_ht->fn_free���g�p���Ȃ����ɒ��ӂ��ĉ������B
	free(p_ht);		//�����ł�p_ht->fn_alloc,p_ht->fn_free���g�p���Ȃ����ɒ��ӂ��ĉ������B
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
	p_ht->le_bucket = realloc(p_ht->le_bucket, sizeof(LIST_ENTRY) * i_table_size);	//�����ł�p_ht->fn_alloc,p_ht->fn_free���g�p���Ȃ����ɒ��ӂ��ĉ������B
	if(!p_ht->le_bucket) { DIE(); }
	for(i = 0; i < i_table_size; i++) { InitializeListHead(&p_ht->le_bucket[i]); }
	//Walk through all elements in the table and insert them into the new bucket.
	while(list_entry != list_head) {
		ght_hash_entry_t* p_e = CONTAINING_RECORD(list_entry, ght_hash_entry_t, le_iterator);
		l_key = ght_get_hash_value(p_ht, p_e->i_key_size, p_e->key_data);
		InsertHeadList(&p_ht->le_bucket[l_key], &p_e->le_bucket);	//�L���b�V���Ή��̂��߂ɁA�o�P�b�g�̃��X�g�ւ̒ǉ��ʒu�͐擪�ɒǉ�����悤�ɕύX�����B�����̃A�v���P�[�V�����̓o�P�b�g�̃��X�g�𒼐ڎQ�Ƃ��Ă��Ȃ��̂ŁA���̕ύX�ɂ��݊����̖��͐����Ȃ��͂����B
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
