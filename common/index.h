/*
* index.h
* 
* Kiran Jones, CS50 25W
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../libcs50/hashtable.h"
#include "../libcs50/counters.h"

/*
* typedef of index, largely a hashtable wrapper
*/
typedef hashtable_t index_t;


/*
* Create new index 
* Returns pointer to newly-created index
*/
index_t* index_new(const int num_slots);


/*
* Insert word, counters pair into index
* Returns success boolean
*/
bool index_insert(index_t* index, char* word, counters_t* counters);


/*
* Finds word in index
* Returns pointer to counters associated with provided word (or NULL if error/word not in index)
*/
counters_t* index_find(index_t* index, char* word);


/* 
* Writes the index to a file, given the index and indexFilename
*/
void index_write(index_t* index, char* indexFilename);

/* 
* Helper function to write a word from the index (key in hashtable)
*/
void writeHashtable(void* fp, const char* word, void* counters);


/*
* Helper function to write a docID count pair from a counter 
*/
void writeCounters(void* fp, const int docID, const int count);


/* 
* Creates an index_t* from a given index file and returns it
*/
index_t* index_read(char* fileName);


/*
* Helper function to delete an index
*/
void index_delete(index_t* index);


/* 
* Helper function to delete a counter
*/
void counters_delete_helper(void* counters);


