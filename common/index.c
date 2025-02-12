/*
* 
* 
* 
* 
*/


// hashtable(word) -> counters(docID) -> count of word in docID

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../libcs50/hashtable.h"
#include "../libcs50/counters.h"

index_t* index_new(const int num_slots);

bool index_insert(index_t* index, char* word, counters_t* counters);

counters_t* index_find(index_t* index, char* word);

void index_write(index_t* index, FILE* fp);

void writeHashtable(void* fp, const char* word, void* counters);

void writeCounters(void* fp, const int docID, const int count);

index_t* index_read(char* fileName);

void index_delete(index_t* index);


typedef hashtable_t index_t;


index_t* 
index_new(const int num_slots) 
{
    return (index_t*) hashtable_new(num_slots);
}

// named function index_insert() to match with hashtable_insert() instead of index_save() as reccomended in the design document 
bool 
index_insert(index_t* index, char* word, counters_t* counters)
{
    return hashtable_insert((hashtable_t*) index, word, counters);
}

counters_t* 
index_find(index_t* index, char* word)
{
    return hashtable_find((hashtable_t*) index, word);
}


// writes the index to a file, given the index and file
void 
index_write(index_t* index, FILE* fp)
{

    // for all words in index
        // print word (space)
        // for all counters in word
            // print docID (space) count (space)
        // print \n

    if (index != NULL && fp != NULL)
    {
        hashtable_iterate((hashtable_t*) index, fp, writeHashtable);
        fclose(fp);
        index_delete(index);
    }
}

void 
writeHashtable(void* fp, const char* word, void* counters) 
{
    // key is word item is counters_t* 
    // arg is fp

    // print the word followed by a space
    fprintf((FILE*)fp, "%s ", word);

    // iterate over counters
    counters_iterate((counters_t*)counters, fp, writeCounters);

    // print a new line after each docID count pair
    fprintf((FILE*)fp, "\n");

}

void 
writeCounters(void* fp, const int docID, const int count) 
{
    fprintf((FILE*)fp, "%d %d ", docID, count);
}


// creates an index from a given file, and returns it
index_t*
index_read(char* fileName)
{
    const static int indexSize = 500;
    index_t* index = index_new(indexSize);

    FILE* fp = fopen(fileName, "r");
    if (fp == NULL) {
        fprintf(stderr, "Error opening file: %s\n", fileName);
        return NULL;
    }

    char* word;
    counters_t* counters;
    int docID = 0;
    int count = 0;

    while ( (word = file_readWord(fp)) != NULL) {
        counters = counters_new();

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
    hashtable_delete((hashtable_t*) index, counters_delete);
}