/*
* indextest.c
* 
* Kiran Jones, CS50 25W
*/


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../common/index.h"
#include "../libcs50/counters.h"
#include "../libcs50/file.h"

static void parseArgs(int argc, char* argv[], char** oldIndexFilename, char** newIndexFilename);

index_t* loadIndex(char* oldIndexFilename); 

// ./indextest oldIndexFilename newIndexFilename
// reads from oldIndexFilename which is produced by indexer
    // format: word docID count [docID count] ...


// Unit testing. A program indextest will serve as a unit test for the index module. 
// It reads an index file into the internal index data structure, then writes the index out to a new index file.


int
main(int argc, char* argv[]) 
{
    char* oldIndexFilename = NULL;
    char* newIndexFilename = NULL;
    parseArgs(argc, argv, &oldIndexFilename, &newIndexFilename);

    index_t* index = loadIndex(oldIndexFilename);

    index_write(index, newIndexFilename);

    return 0;
}


static void
parseArgs(int argc, char* argv[], char** oldIndexFilename, char** newIndexFilename)
{
    // check for incorrect number of arguments
    if (argc != 3) {
        if (argc == 1) {
            fprintf(stderr, "No arguments were given. The correct format is: indextest oldIndexFilename newIndexFilename\n");
            exit(1);
        }

        if (argc > 3) {
            fprintf(stderr, "Too many arguments were given. The correct format is: indextest oldIndexFilename newIndexFilename\n");
            exit(1);
        }

        fprintf(stderr, "Only two arguments were given. The correct format is: indextest oldIndexFilename newIndexFilename\n");
        exit(1);
    }


    // open oldIndexFilename with read permissions
    FILE* fp = fopen(argv[1], "r");
    if (fp == NULL) {
        fprintf(stderr, "Error opening oldIndexFilename with read permissions, file does not exist or is not readable.\n");
        exit(1);
    }
    // file was sucessfully opened, close it and assign to oldIndexFilename
    fclose(fp);
    *oldIndexFilename = argv[1];

    // open newIndexFilename with write permissions 
    fp = fopen(argv[2], "w");
    if (fp == NULL) {
            fprintf(stderr, "Error opening newIndexFilename with write permissions, file does not exist or is not writeable.\n");
            exit(1);
        }
    // file was sucessfully opened, close it and assign to newIndexFilename
    fclose(fp);
    *newIndexFilename = argv[2];

}

/*
* Creates and returns an inverted-index data structure from an indexer-produced file
*/
index_t*
loadIndex(char* oldIndexFilename) 
{

    // initialize new index of size indexSize (500)
    const int indexSize = 500;
    index_t* index = index_new(indexSize);

    // variable declarations 
    char* line;
    char* word;
    char* token;
    int docID;
    int count;
    counters_t* counters;

    // open oldIndexFilename with read permissions 
    FILE* fp = fopen(oldIndexFilename, "r");

    // double check successful open, this should not trigger due to previous checking in parseArgs() 
    if (fp == NULL) {
        fprintf(stderr, "Error opening oldIndexFilename with read permissions");
        exit(1);
    }

    // iterate over each line of the file
    while ( (line = file_readLine(fp)) != NULL) 
    {    

        // the word is the first word of the line
        word = strtok(line, " ");

        // instantiate new counters associated with the current word
        counters = counters_new();

        // iterate over the rest of the line
        while  ( (token = strtok(NULL, " ")) != NULL)
        {
            // docID follows word (or previous docID/count pair)
            docID = atoi(token);

            // iterate to the next word
            token = strtok(NULL, " ");

            // count follows docID
            count = atoi(token);

            // check counters was successfully created
            if (counters == NULL) {
                fprintf(stderr, "Error creating counters.\n");
                exit(1);
            }

            // add the docID and count to the newly-created counter
            counters_set(counters, docID, count);

            // add the newly-created counter to the index
            index_insert(index, word, counters);
        }
        
        // free the line before iterating to the next one
        free(line);
    }

    // close fp
    fclose(fp);

    return index;
}

