/*
* index.h
* 
* 
* Kiran Jones, CS50 25W
* 
*/


index_t* index_new(const int num_slots);

bool index_insert(index_t* index, char* word, counters_t* counters);

counters_t* index_find(index_t* index, char* word);

void index_write(index_t* index, FILE* fp);

void writeHashtable(void* fp, const char* word, void* counters);

void writeCounters(void* fp, const int docID, const int count);

index_t* index_read(char* fileName);

void index_delete(index_t* index);


typedef hashtable_t index_t;