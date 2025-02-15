/*
* indexer.c
* 
* 
* Kiran Jones, CS50 25W
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../common/pagedir.h"
#include "../libcs50/hashtable.h"
#include "../libcs50/webpage.h"
#include "../libcs50/counters.h"
#include "../common/index.h"
#include "../common/word.h"



static void parseArgs(int argc, char* argv[], char** pageDirectory, char** indexFilename);

static index_t* indexBuild(char* pageDirectory);

static void indexPage(webpage_t* webpage, int docID, index_t* index);

// ./indexer pageDirectory indexFilename

int 
main(int argc, char* argv[])
{

    // initialize pageDirectory and indexFilename to NULL, these will be assigned in parseArgs assuming valid command-line arguments 
    char* pageDirectory = NULL;
    char* indexFilename = NULL;

    // call parseArgs to check command-line arguments and to assign pageDirectory and indexFilename 
    parseArgs(argc, argv, &pageDirectory, &indexFilename);

    // build an index using the provided pageDirectory 
    index_t* index = indexBuild(pageDirectory);

    // write the created index to the provided indexFilename
    index_write(index, indexFilename);
    
    return 0;
}

static void parseArgs(int argc, char* argv[], char** pageDirectory, char** indexFilename) 
{
    // check if an invalid number of arguments was provided
    if (argc != 3) {
        if (argc == 1) {
            fprintf(stderr, "No arguments were given. The correct format is: indexer pageDirectory indexFilename\n");
            exit(1);
        }

        if (argc > 3) {
            fprintf(stderr, "Too many arguments were given. The correct format is: indexer pageDirectory indexFilename\n");
            exit(1);
        }

        fprintf(stderr, "Only two arguments were given. The correct format is: indexer pageDirectory indexFilename\n");
        exit(1);
    }

    // validate the provided pageDirectory
    if (!pagedir_validate(argv[1])) {
        fprintf(stderr, "Error validating pageDirectory (argv1). The directory either does not exist, or is not crawler-produced.\n");
        exit(1);
    }
    // pageDirectory is valid, assign it
    *pageDirectory = argv[1];

    // attempt to open the provided indexFilename with write permissions (as required to later write the index to the file)
    FILE* fp = fopen(argv[2], "w");
    if (fp == NULL) {
        fprintf(stderr, "Provided indexFilename is not a valid file or does not have write permissions (argv2).\n");
        exit(1);
    }
    fclose(fp);

    // indexFilename is valid, assign it
    *indexFilename = argv[2];
}

/*
* Builds an index from a provided pageDirectory
* 
* 
*/
static index_t* 
indexBuild(char* pageDirectory)
{
    // create new index object
    const int indexSize = 500;
    index_t* index = index_new(indexSize);

    int docID = 1; // starting docID
    webpage_t* webpage = NULL;

    while ((webpage = pagedir_load(pageDirectory, docID)) != NULL) { 
        
        indexPage(webpage, docID, index);
        webpage_delete(webpage);
        docID++;
    }
    return index;
}



/*
* Indexes a provided webpage with provided docID to provided index index
* 
* 
*/
static void 
indexPage(webpage_t* webpage, int docID, index_t* index)
{
    char* word;
    int pos = 0;
    while ( (word = webpage_getNextWord(webpage, &pos) ) != NULL) {

        // skip trivial words less than length 3
        if (strlen(word) > 2) {

            // normalize the word (make it all lowercase)
            normalizeWord(word);

            counters_t* counters;

            // tries to find the current word in the index
            if ( (counters = index_find(index, word)) != NULL) {

                // if the current word is found, increment its counter cooresponding to the docID
                counters_add(counters, docID);

            } else {

                // if the current word is not found, create a new counter for it 
                counters = counters_new();

                // initialize the counter associated with the current word and docID to 1
                counters_set(counters, docID, 1);

                // add the newly-created counter to the index
                index_insert(index, word, counters);
            }
        }
        // free the current word before looping to the next one 
        free(word);
    }
}