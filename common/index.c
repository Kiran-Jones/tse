/*
* index.c
* 
* Kiran Jones, CS50 25W
* 
*/


// hashtable(word) -> counters(docID) -> count of word in docID

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../libcs50/hashtable.h"
#include "../libcs50/counters.h"
#include "../libcs50/file.h"

typedef hashtable_t index_t;


index_t* index_new(const int num_slots);

bool index_insert(index_t* index, char* word, counters_t* counters);

counters_t* index_find(index_t* index, char* word);

void index_write(index_t* index, char* indexFilename);

void writeHashtable(void* fp, const char* word, void* counters);

void writeCounters(void* fp, const int docID, const int count);

index_t* index_read(char* fileName);

void index_delete(index_t* index);

void counters_delete_helper(void* counters);


/*
* Create new index 
* Returns pointer to newly-created index
*/
index_t* 
index_new(const int num_slots) 
{
    return (index_t*) hashtable_new(num_slots);
}

/*
* Insert word, counters pair into index
* Returns success boolean
* 
* named function index_insert() to match with hashtable_insert() instead of index_save() which was reccomended in the design document 
*/
bool 
index_insert(index_t* index, char* word, counters_t* counters)
{
    return hashtable_insert((hashtable_t*) index, word, counters);
}

/*
* Find in index from word
* Returns pointer to counters associated with provided word (or NULL if error/word not in index)
*/
counters_t* 
index_find(index_t* index, char* word)
{
    return hashtable_find((hashtable_t*) index, word);
}


/* 
* Writes the index to a file, given the index and indexFilename
*/
void 
index_write(index_t* index, char* indexFilename)
{
    // open indexFilename
    FILE * fp = fopen(indexFilename, "w");

    if (index != NULL && fp != NULL)
    {
        // iterate over each slot in the hashtable, call writeHashtable() on each 
        hashtable_iterate((hashtable_t*) index, fp, writeHashtable);

        // close the opened file pointer
        fclose(fp);

        // delete the index
        index_delete(index);
    }
}


/* 
* Helper function to write a word from the index (key in hashtable)
*/
void 
writeHashtable(void* fp, const char* word, void* counters) 
{

    // write the word followed by a space to fp
    fprintf((FILE*)fp, "%s ", word);

    // iterate over counters, call writeCounters on each 
    counters_iterate((counters_t*)counters, fp, writeCounters);

    // print a new line after each docID count pair
    fprintf((FILE*)fp, "\n");

}

/*
* Helper function to write a docID count pair from a counter 
*/
void 
writeCounters(void* fp, const int docID, const int count) 
{
    // print the docID followed by a space followed by the count followed by a space
    // new line not printed here as every docID and count pair associated with a given word is printed on the same line
    // ex. with with docID and count pairs [1, 5] and [2, 10] (in two calls via counters_iterate): "1 5 2 10 "
    fprintf((FILE*)fp, "%d %d ", docID, count);
}


// creates an index from a given file and returns it
index_t*
index_read(char* fileName)
{
    // constant indexSize
    const static int indexSize = 500;

    // creates new index of size indexSize
    index_t* index = index_new(indexSize);

    // check if index_new() failed
    if (index == NULL) {
        fprintf(stderr, "Error creating new index of size %d\n", indexSize);
        return NULL;
    }

    // attempt to open fileName with read ability 
    FILE* fp = fopen(fileName, "r");

    // check if error opening file
    if (fp == NULL) {
        fprintf(stderr, "Error opening file: %s\n", fileName);
        return NULL;
    }

    char* word;
    counters_t* counters;
    int docID = 0;
    int count = 0;

    // iterate over each word in provided file fp
    while ( (word = file_readWord(fp)) != NULL) {

        if ((counters = index_find(index, word)) == NULL) {
            counters = counters_new();
        }

        if (counters == NULL) {
            fprintf(stderr, "Error creating counters\n");
            return NULL;
        }

        while (fscanf(fp, " %d %d", &docID, &count) == 2) {
            counters_set(counters, docID, count);
        }

        index_insert(index, word, counters);

        free(word);

    }
    fclose(fp);
    return index;
}


void 
index_delete(index_t* index) 
{
    hashtable_delete((hashtable_t*) index, counters_delete_helper);
}

void
counters_delete_helper(void* counters) {
    counters_delete((counters_t*) counters);
}