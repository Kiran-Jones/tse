/*
* 
* 
* 
* 
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../common/pagedir.h"
// #include "../common/index.h"
// #include "../common/word.h"
#include "../libcs50/hashtable.h"
#include "../libcs50/webpage.h"
#include "../libcs50/counters.h"
#include "../common/index.h"
#include "../common/word.h"




static void parseArgs(const int argc, char* argv[], char** pageDirectory, char** indexFilename);

static index_t* indexBuild(const char** pageDirectory);

static void indexPage(webpage_t* webpage, int docID, index_t* index);

// ./indexer pageDirectory indexFilename

int main(const int argc, char* argv[])
{

    char* pageDirectory = NULL;
    char* indexFilename = NULL;
    parseArgs(argc, argv, &pageDirectory, &indexFilename);
    index_t* index = indexBuild(pageDirectory);
    index_write(index, indexFilename);

}

static void parseArgs(const int argc, char* argv[], char** pageDirectory, char** indexFilename) 
{
    if (argc != 3) {
        exit(1);
    }
    
    *pageDirectory = argv[1];
    *indexFilename = argv[2];

}



static index_t* 
indexBuild(const char** pageDirectory)
{
    // create new index object
    const int indexSize = 500;
    index_t* index = index_new(indexSize);

    int docID = 1; // starting docID
    webpage_t* webpage = NULL;

    while ((webpage = pagedir_load(pageDirectory, docID))) { // implement pagedir_load
        
        indexPage(webpage, docID, index);
        webpage_delete(webpage);
        docID++;
    }


}


static void 
indexPage(webpage_t* webpage, int docID, index_t* index)
{
    char* word;
    int pos = 0;
    while ( (word = webpage_getNextWord(webpage, &pos) ) != NULL) {
        if (strlen(word) > 2) {
            normalizeWord(word);
            counters_t* counters = NULL;
            if ( (counters = index_find(index, word)) != NULL) {
                counters_add(counters, docID);
            } else {
                counters = counters_new();
                counters_set(counters, docID, 1);
                index_insert(index, word, counters);
            }
        }
    }
    free(word);
}