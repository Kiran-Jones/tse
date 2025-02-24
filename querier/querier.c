/* 
* querier.c
* 
* Kiran Jones, CS50 25W
*/


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <stdbool.h>
#include "../common/index.h"
#include "../common/pagedir.h"
#include "../common/word.h"
#include "../libcs50/file.h"

typedef struct queryCoutners {
    counters_t* currentCoutners;
    counters_t* temp;
    counters_t* sequence; 
} queryCounters_t;


typedef struct maxScore {
    int maxdocID;
    int maxCount;
} maxScore_t;

int main(int argc, char* argv[]);
static void parseArgs(int argc, char* argv[], char** pageDirectory, char** indexFilename);

static void prompt(void);

int fileno(FILE *stream);

static bool getQuery(char** query);

static void cleanQuery(char* query, char** words, int* wordCount);

static void printCleanQuery(char** words, int wordCount); 

static void validateQuery(char** words, int wordCount);

static void intersectCounters(counters_t* counters1, counters_t* counters2);

static void intersectLeft(void*, const int, const int);
static void intersectRight(void*, const int, const int);

static void unionCounters(counters_t* counters1, counters_t* counters2);
static void unionCountersHelper(void* counters1, const int docID, const int count);

counters_t* processQuery(char** words, int wordCount, index_t* index);

static void printSequence(counters_t* sequence, char* pageDirectory);
static void getSequenceSize(void* sequenceSize, const int docID, const int count);
static void printSequenceHelper(void* pageDirectory, const int docID, const int count);

maxScore_t* rankSequence(counters_t* sequence);

static void rankSequenceHelper(void* arg, const int docID, const int count);

int
main(int argc, char* argv[]) 
{
    char* pageDirectory = NULL;
    char* indexFilename = NULL;
    parseArgs(argc, argv, &pageDirectory, &indexFilename);

    
    char* query = NULL;
    counters_t* sequence;
    index_t* index = index_read(indexFilename);

    while (getQuery(&query)) {
        // index = index_read(indexFilename);

        int wordCount = 0;

        char* words[strlen(query+1) / 2]; // max number of words occurs every word is one letter (50% letters, 50% spaces)

        cleanQuery(query, words, &wordCount);

        validateQuery(words, wordCount);

        printCleanQuery(words, wordCount);

        sequence = processQuery(words, wordCount, index);

        printSequence(sequence, pageDirectory);
        counters_delete(sequence);
        free(query);
    }
    index_delete(index);
    free(query);
    printf("\n");


    // accumulate (intersect for every AND in a row)
    // union once OR is reached
    // continue 
    //
    // one large set of pages (result)
    // one small set of pages (temp)
    // while processing AND commands: 
    //  temp = firstFile
    //  temp = temp intersect secondFile
    //  temp = temp intersect thirdFile
    // while processing OR commands:
    //  result = result union temp (add previous temp to result)
    //  temp = file
    //  (potential AND commands)
    // result = result union temp (add accumulated temp to result) 
    //
    // need several functions: calculate AND and calculate OR
    // both given large set and small set (result and temp) 
    // operation 
    //
    //
    // process
    // (A and B) or (P and Q) or (R and S and T) or Z
    // (A and B) or (C) or (D) or (E and F and G) 
    //
    // algorithm:
    //  while AND (prev operation not OR):
    //    if temp is empty
    //      temp = f_1
    //    else
    //      temp = temp intersect f_i
    //
    //  result = result union temp
    //  
    //
    //
    //  while have words to process: // loop over char* words[] 
    //    
    //    if last operation was OR: // boolean
    //      result = result union temp // union function
    //
    //    if temp is empty: // accumulate 
    //      temp = currentFile
    //    else:
    //      temp = temp intersect currentFile // intersect function. intersects counters to counters 
    //
    //  return result
    //
    //    
    // char* word;

    // index_find(index, word); // -> counters_t counters(docID, count) all the documents which contain word

    // intersect operator
    /* 
    *  for item in counters1:
    *       if (counters_get(counters2, docID) != 0): // check if exists in other counters (intersection)
    *           counters_set(counters1, docID, min count of counters1 and counters2)
    *       else:
    *           counters_set(counters1, docID, 0)
    * 
    */

    // union operator
    /*
    * for item in counters2
    *      counters_set(counters1, docID, counters_get(counters1, docID) + counters_get(counters2, docID)) // counters_get() returns 0 if not found
    *   
    * */

    // index_delete(index);

    return 0;
}

/*
* Validate command-line arguments
* Ensures that pageDirectory is crawler-produced and that indexFilename is indexer-produced 
*/
static void parseArgs(int argc, char* argv[], char** pageDirectory, char** indexFilename)
{
     if (argc != 3) {
        if (argc == 1) {
            fprintf(stderr, "No arguments were given. The correct format is: querier pageDirectory indexFilename\n");
            exit(1);
        }

        if (argc > 3) {
            fprintf(stderr, "Too many arguments were given. The correct format is: querier pageDirectory indexFilename\n");
            exit(1);
        }

        fprintf(stderr, "Only two arguments were given. The correct format is: querier pageDirectory indexFilename\n");
        exit(1);
    }

    if (!pagedir_validate(argv[1])) {
        fprintf(stderr, "Error validating pageDirectory (argv1). The directory either does not exist, is not readable, or is not crawler-produced.\n");
        exit(1);
    }

    *pageDirectory = argv[1];

    FILE* fp = fopen(argv[2], "r");
    if (fp == NULL) {
        fprintf(stderr, "Provided indexFilename is not a valid file or does not have read permissions (argv2).\n");
        exit(1);
    }
    fclose(fp);

    *indexFilename = argv[2];
}


static bool
getQuery(char** query)
{
    static const int initialQuerySize = 16;
    *query = malloc(sizeof(char) * initialQuerySize);

    int c;
    int length = 0;
    int bufferSize = initialQuerySize;

    prompt();

    while ( (c = getchar()) != '\n') {

        if (c == EOF) {
            return false;
        }

        if (length + 1 >= bufferSize) {
            bufferSize += initialQuerySize;
            char* temp = realloc(*query, bufferSize);
            if (temp == NULL) {
                fprintf(stderr, "Error reallocing memory for query\n");
                exit(1);
            }
            *query = temp; 
            
        }
        (*query)[length++] = c;
        // printf("%s\n", *query);
    }
    (*query)[length] = '\0';
    return true;
}


static void
cleanQuery(char* query, char** words, int* wordCount) 
{
    int queryLength = strlen(query);
    // printf("%d\n", queryLength);

    bool inWord = false;

    for (int i = 0; i < queryLength; i++) {
        if (inWord) {
            if (isspace(query[i])) {
                inWord = false;
                query[i] = '\0';
            } else {
                query[i] = tolower(query[i]);
            }
        } else {
            if (!isspace(query[i])) {
                inWord = true;
                query[i] = tolower(query[i]);
                words[(*wordCount)++] = &query[i];
            } 
        }        
    }
    /*
    printf("wc: %d\n", *wordCount);
    
    for (int j = 0; j < *wordCount; j++) {
        printf("word: %s\n", words[j]);
    }
    */

}


/*
* Check that the cleaned query conforms to the required syntax
* Exit upon invalid query
*/
static void 
validateQuery(char** words, int wordCount)
{
    // check that the first word is not or
    if (strcmp(words[0], "or") == 0) {
        fprintf(stderr, "Error: 'or' cannot be first\n");
        exit(1);
    }

    // check that the first word is not and
    if (strcmp(words[0], "and") == 0) {
        fprintf(stderr, "Error: 'and' cannot be first\n");
        exit(1);
    }

   
    bool prevLiteral = false;

    for (int i = 0; i < wordCount; i++) {
        
        // ensure that two literals are not next to each other
        if (strcmp(words[i], "and") == 0 || strcmp(words[i], "or") == 0) {
            if (!prevLiteral) {
                prevLiteral = true;
            } else {
                fprintf(stderr, "Error: cannot have literals adjacent\n");
                exit(1);
            }

        } else {
            prevLiteral = false;
        }


        /*
        // TODO: add more detailed logging to support messages
        // Error: 'and' and 'or' cannot be adjacent, Error: 'or' and 'or' cannot be adjacent, Error: 'and' and 'and' cannot be adjacent
        if (strcmp(words[i], "and") == 0 || strcmp(words[i], "or") == 0) {
            fprintf(stderr, "Error: cannot have literals adjacent\n");
            exit(1);
        }
        */
    
    
       // iterates through each chatacter in the current word, ensuring that is alphabetical (A-Z, a-z)
       // could be more concise (e.g. exit upon first non-alphabetical character), but I found this detailed
       // implementation to be more helpful for testing/code examination 
        bool invalidInput = false;
        for (int j = 0; j < strlen(words[i]); j++) {
            if (!isalpha(words[i][j])) {
                if (invalidInput) {
                    fprintf(stderr, ", %c", words[i][j]);
                } else {
                    fprintf(stderr, "Error: bad character(s): %c", words[i][j]);
                    invalidInput = true;
                }   
            }
        }
        if (invalidInput) {
            fprintf(stderr, " in query\n");
            exit(1);
        }
    }

    // check that the last word is not or/and
    const int lastWordIndex = wordCount - 1;
    
    if (strcmp(words[lastWordIndex], "or") == 0) {
        fprintf(stderr, "Error: 'or' cannot be last\n");
        exit(1);
    }

    if (strcmp(words[lastWordIndex], "and") == 0) {
        fprintf(stderr, "Error: 'and' cannot be last\n");
        exit(1);
    } 
}


/*
* Helper function to print cleaned query to stdout
*/
static void 
printCleanQuery(char** words, int wordCount) 
{
    // printf("wordCount: %d\n", wordCount);
    
    printf("Clean Query: ");
    for (int i = 0; i < wordCount; i++) {
        printf("%s ", words[i]);
    }
    printf("\n");
}


static void
prompt(void)
{
  // print a prompt iff stdin is a tty (terminal)
  if (isatty(fileno(stdin))) {
    printf("Query? ");
  }
}



/* intersect operator
*  for item in counters1:
*       if (counters_get(counters2, docID) != 0): // check if exists in other counters (intersection)
*           counters_set(counters1, docID, min count of counters1 and counters2)
*       else:
*           counters_set(counters1, docID, 0)
* 
*/

// should modify counters1
static void 
intersectCounters(counters_t* temp, counters_t* counter) 
{
    // printf("intersecting counters\ncounters1: ");
    // counters_print(counters1, stdout);

    // printf("\ncounters2: ");
    // counters_print(counters2, stdout);

    counters_t* args[2] = {temp, counter};
    //counters_print(temp, stdout);
    // printf("\n\n");
    //counters_print(counter, stdout);
    //printf("\n\n");
    counters_iterate(temp, args, intersectLeft);
    //counters_print(temp, stdout);
    //printf("\n\n");
    //counters_print(counter, stdout);
    //printf("\n\n");
    counters_iterate(counter, args, intersectRight);
    //counters_print(temp, stdout);
    //printf("\n\n");
    //counters_print(counter, stdout);
    //printf("\n\n");
    //counters_iterate(counter, temp, );
    //counters_iterate(counters1, counters2, intersectCountersHelper);
    //counters_iterate(counters2, counters1, intersectCountersHelper); // iterate over counters2

}

static void
intersectRight(void* args, const int docID, const int count) 
{
  counters_t **countersArray = (counters_t**)args;
  counters_t *temp = countersArray[0];

  int counterGot = counters_get(temp, docID);

  if (counterGot < count) {
    counters_set(temp, docID, counterGot);
  } else {
    counters_set(temp, docID, count);
  }
}

static void 
intersectLeft(void* args, const int docID, const int count)
{
    counters_t **countersArray = (counters_t**)args;
    counters_t *temp = countersArray[0];
    counters_t *counter = countersArray[1];

    //Number of times word occured in docID in count
    int counterGot = counters_get(counter, docID);

    //min is counterGot
    if (counterGot < count) {
      counters_set(temp, docID, counterGot);
    } else {
      counters_set(temp, docID, count);
    }


    //int counters1Count = counters_get((counters_t*)counters, docID);
    //if ( counters1Count > 0) { // docID in counters2 and counters1
    //    if (counters1Count > count) { 
    //        counters_set((counters_t*)counters, docID, count);
    //    }
    //} else {
    //    // docID does not exist in counters, set to 0
    //    counters_set((counters_t*)counters, docID, 0);
    //}
}


/* union operator
*  for item in counters2
*      counters_set(counters1, docID, counters_get(counters1, docID) + counters_get(counters2, docID)) // counters_get() returns 0 if not found
*   
* 
*/
static void 
unionCounters(counters_t* counters1, counters_t* counters2) 
{   
    counters_iterate(counters2, counters1, unionCountersHelper);
}

static void
unionCountersHelper(void* counters1, const int docID, const int count)
{
    int counters1Count = counters_get((counters_t*)counters1, docID); // returns 0 if not found, no need to manually check
    counters_set((counters_t*)counters1, docID, count + counters1Count);
}


 //  while have words to process: // loop over char* words[] 
    //    
    //    if last operation was OR: // boolean
    //      result = result union temp // union function
    //
    //    if temp is empty: // accumulate 
    //      temp = currentFile
    //    else:
    //      temp = temp intersect currentFile // intersect function. intersects counters to counters 
    //
    //  return result


counters_t*
processQuery(char** words, int wordCount, index_t* index)
{
    counters_t* sequence = counters_new();
    if (sequence == NULL) {
        fprintf(stderr, "Error creating sequence\n");
        exit(1);
    }

    counters_t* temp = NULL;
    counters_t* currentCounters = NULL;

    for (int i = 0; i < wordCount; i++) {

        // printf("temp: ");
        // counters_print(temp, stdout);
        // printf("\n");

        char* currentWord = words[i];

        if (strcmp(currentWord, "and") == 0) {
            continue;
        }

        if (i > 0) {
            if (strcmp(currentWord, "or") == 0) {
                //printf("unioning\n");
                unionCounters(sequence, temp);
                counters_delete(temp);
                temp = NULL;
                continue;
            }
        }
       /*
        currentCounters = index_find(index, currentWord);
        intersectCounters(temp, currentCounters);
      */

      
        if (temp == NULL) { 
          temp = counters_new();
          currentCounters = index_find(index, currentWord);
          unionCounters(temp, currentCounters);


           // temp = index_find(index, currentWord);
            // printf("new temp:");
            //     counters_print(temp, stdout);
            //     printf("\n");
        } else {
            currentCounters = index_find(index, currentWord);
            // counters_print(temp, stdout);            
            // printf("\n\n%s\n\n", currentWord);
            intersectCounters(temp, currentCounters);
        }
        // printf("temp when i=%d\n", i);
        // counters_print(sequence, stdout);
        // printf("\n");
        //
        //
    }
    unionCounters(sequence, temp);
    counters_delete(temp);
    // free(temp);
    return sequence;
}


/*
counters_t*
processQuery(char** words, int wordCount, index_t* index)
{
    queryCounters_t* queryCounters = malloc(sizeof(queryCounters_t));
    if (queryCounters == NULL) {
        fprintf(stderr,"Error allocating space for queryCounters\n");
        exit(1);
    }
    queryCounters->sequence = counters_new();
    if (queryCounters->sequence == NULL) {
        fprintf(stderr, "Error creating sequence\n");
        exit(1);
    }
    queryCounters->temp = counters_new();
     if (queryCounters->temp == NULL) {
        fprintf(stderr, "Error creating temp\n");
        exit(1);
    }
    queryCounters->currentCoutners = counters_new();
    if (queryCounters->currentCoutners == NULL) {
        fprintf(stderr, "Error creating currentCounters\n");
        exit(1);
    }

    for (int i = 0; i < wordCount; i++) {

        printf("temp: ");
        counters_print(queryCounters->temp, stdout);
        printf("\n");

        char* currentWord = words[i];

        if (strcmp(currentWord, "and") == 0) {
            continue;
        }

        if (i > 1) {
            if (strcmp(currentWord, "or") == 0) {
                // printf("unioning\n");
                unionCounters(queryCounters->sequence, queryCounters->temp);
                counters_delete(queryCounters->temp);
                queryCounters->temp = counters_new();
                if (queryCounters->temp == NULL) {
                    fprintf(stderr, "Error creating temp\n");
                    exit(1);
                }
                continue;
            }
        }

           
        if (queryCounters->temp == NULL) { 
            queryCounters->temp = index_find(index, currentWord);

            printf("new temp:");
            counters_print(queryCounters->temp, stdout);
            printf("\n");
        } else {
            queryCounters->currentCoutners = index_find(index, currentWord);
            intersectCounters(queryCounters->temp, queryCounters->currentCoutners);
            counters_delete(queryCounters->currentCoutners);
            queryCounters->currentCoutners = counters_new();
        }
        // printf("temp when i=%d\n", i);
        // counters_print(sequence, stdout);
        // printf("\n");
    }
    unionCounters(queryCounters->sequence, queryCounters->temp);
    counters_delete(queryCounters->temp);
    counters_delete(queryCounters->currentCoutners);
    return queryCounters->sequence;
}
*/

static void
printSequence(counters_t* sequence, char* pageDirectory)
{
    int sequenceSize = 0;
    maxScore_t* max;
    counters_iterate(sequence, &sequenceSize, getSequenceSize);

    if (sequenceSize == 0) {
        printf("No documents match.\n");
        // return;
    }

    printf("Matches %d documents (ranked):\n", sequenceSize);

    for (int i = 0; i < sequenceSize; i++) {
        max = rankSequence(sequence);

        printSequenceHelper(pageDirectory, max->maxdocID, max->maxCount);
        counters_set(sequence, max->maxdocID, 0);
        free(max);
    }



    // counters_iterate(sequence, pageDirectory, printSequenceHelper);
    printf("-----------------------------------------------\n");
}

static void
getSequenceSize(void* sequenceSize, const int docID, const int count)
{
    if (count > 0) {
        (*(int*)sequenceSize) += 1;
    }
}

static void
printSequenceHelper(void* pageDirectory, const int docID, const int count)
{
    // pages with count 0 are "deleted", only consider positive docID
    if (count > 0) {

        // convert int docID to char* charID
        char* charID = malloc(sizeof(int) + 1);
        sprintf(charID, "%d", docID);

        // construct the pathname for the given file in pageDirectory
        char* pathName = malloc(sizeof(char) * (strlen(pageDirectory) + strlen(charID) + 2)); // plus two for backslash and null terminator 

        // ensure malloc worked
        if (pathName == NULL) {
            fprintf(stderr, "Error creating pathName\n");
            exit(21);
        }

        // adds pageDirectory to newly-created pathName
        strcpy(pathName, pageDirectory);
        strcat(strcat(pathName, "/"), charID); // adds slash to the end of pathName, and the appends charID
        FILE* fp = fopen(pathName, "r");

        char* url = file_readLine(fp);

        printf("score   %d doc %d: %s\n", count, docID, url);

        fclose(fp);
        free(url);
        free(charID);
        free(pathName);
    }
}



/*
*  
*/
maxScore_t*
rankSequence(counters_t* sequence)
{
    // const int docID, const int count
    maxScore_t* max = malloc(sizeof(maxScore_t));
    max->maxCount = 0;
    max->maxdocID = 0;
    counters_iterate(sequence, max, rankSequenceHelper);
    return max;

}

static void
rankSequenceHelper(void* arg, const int docID, const int count)
{
    maxScore_t* max = (maxScore_t*) arg;
    if (count > max->maxCount) {
        max->maxCount = count;
        max->maxdocID = docID;
    }
}


/*
counters_t*
copyCounters(counters_t* counters)
{
  counters_iterate()

}
*/
