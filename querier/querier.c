/* 
* querier.c
* 
* Standalone program that accepts user queries, and provides matches using a provided pageDirectory and index file 
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
static void intersectCounters(counters_t* temp, counters_t* currentCounters);
static void intersectLeft(void* args, const int docID, const int count);
static void intersectRight(void* args, const int docID, const int count);
static void unionCounters(counters_t* temp, counters_t* currentCounters);
static void unionCountersHelper(void* temp, const int docID, const int count);
counters_t* processQuery(char** words, int wordCount, index_t* index);
static void printSequence(counters_t* sequence, char* pageDirectory);
static void getSequenceSize(void* sequenceSize, const int docID, const int count);
static void printSequenceHelper(void* pageDirectory, const int docID, const int count);
maxScore_t* rankSequence(counters_t* sequence);
static void rankSequenceHelper(void* arg, const int docID, const int count);


/*
* Main function handling control flow of the querier 
*/
int
main(int argc, char* argv[]) 
{
    // initialize variables to hold the pageDirectory and indexFilename 
    char* pageDirectory = NULL;
    char* indexFilename = NULL;

    // validate commmand line args, if valid assign them to pageDirectory and indexFilename
    parseArgs(argc, argv, &pageDirectory, &indexFilename);

    
    char* query = NULL;
    counters_t* sequence;

    // create index from provided index file
    index_t* index = index_read(indexFilename);
    if (index == NULL) {
        fpritnf("Error creating index\n");
        exit(1);
    }

    // iterate until user enters EOF
    while (getQuery(&query)) {

        int wordCount = 0;
        // max number of words occurs if every word is one letter (50% letters, 50% spaces)
        char* words[strlen(query+1) / 2]; 

        cleanQuery(query, words, &wordCount);

        validateQuery(words, wordCount);

        printCleanQuery(words, wordCount);

        // sequence is a counter of documents matching the query 
        sequence = processQuery(words, wordCount, index);

        printSequence(sequence, pageDirectory);

        // free the sequence and query (will be re-written during subsequent iterations)
        counters_delete(sequence);
        free(query);
    }
    // once done, free the index
    index_delete(index);

    // when getQuery returns false a new query has already been malloc'd, need to free it before terminating program 
    free(query);
    printf("\n");

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

/* 
* Accept a query from the user
*/
static bool
getQuery(char** query)
{
    // constant initial size for the query to be
    static const int initialQuerySize = 16;

    // allocate memory for the query 
    *query = malloc(sizeof(char) * initialQuerySize);

    int c;
    int length = 0;
    int bufferSize = initialQuerySize;

    // if the user is a terminal, prompt asking for a query 
    prompt();

    // accepts characters until a new line is entered
    while ( (c = getchar()) != '\n') {

        // if the user entered EOF, return false (end of program)
        if (c == EOF) {
            return false;
        }

        // if the current buffer cannot accomodate more characters 
        if (length + 1 >= bufferSize) {
            
            // add more space to the buffer
            bufferSize += initialQuerySize;

            // reallocate the query to a temp pointer 
            char* temp = realloc(*query, bufferSize);
            if (temp == NULL) {
                fprintf(stderr, "Error reallocing memory for query\n");
                exit(1);
            }
            // if realloc successful, assign the value back to query 
            *query = temp; 
            
        }

        // add the current character to the next position in query
        (*query)[length++] = c;
    }
    // add a termination character to signify the end of the query
    (*query)[length] = '\0';

    // return true signifying there is a new query to process
    return true;
}

/*
* "Cleans" the query, normalizing all characters and creating words
* Words is a array of char* pointers, containing pointers to the start of each word in the char* query
*/
static void
cleanQuery(char* query, char** words, int* wordCount) 
{
    int queryLength = strlen(query);

    bool inWord = false;

    // iterate over the query by character 
    for (int i = 0; i < queryLength; i++) {
        if (inWord) {
            // if in word and current character is space end of word is reached
            if (isspace(query[i])) {
                inWord = false;
                query[i] = '\0';
            } else {
                // otherwise, normalize character 
                query[i] = tolower(query[i]);
            }
        } else { 
            // if not in word and current character is not a space, reached the start of a new word
            if (!isspace(query[i])) {
                inWord = true;
                query[i] = tolower(query[i]);
                words[(*wordCount)++] = &query[i];
            } 
        }        
    }
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
        
       /* iterates through each chatacter in the current word, ensuring that is alphabetical (A-Z, a-z)
            could be more concise (e.g. exit upon first non-alphabetical character), but I found this detailed
            implementation to be more helpful for testing/code examination */
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
* Print cleaned query to stdout
*/
static void 
printCleanQuery(char** words, int wordCount) 
{    
    printf("Clean Query: ");
    for (int i = 0; i < wordCount; i++) {
        printf("%s ", words[i]);
    }
    printf("\n");
}


/*
* Print prompt if user is a terminal
* Function taken from querier README.md
*/
static void
prompt(void)
{
  // print a prompt iff stdin is a tty (terminal)
  if (isatty(fileno(stdin))) {
    printf("Query? ");
  }
}



/*
* Perform in-place intersection of two counters, storing the result in the first
*/
static void 
intersectCounters(counters_t* temp, counters_t* currentCounters) 
{
    // create arg to pass to helper functions 
    counters_t* args[2] = {temp, currentCounters};
    counters_iterate(temp, args, intersectLeft);
    counters_iterate(currentCounters, args, intersectRight);
}

/*
* Iterate over currentCounters, storing the lower count associated with each docID in temp 
* Adds docIDs not in temp to temp with count 0
*/
static void
intersectRight(void* args, const int docID, const int count) 
{
  counters_t** countersArray = (counters_t**)args;
  counters_t* temp = countersArray[0];

  int counterGot = counters_get(temp, docID);

  if (counterGot < count) {
    counters_set(temp, docID, counterGot);
  } else {
    counters_set(temp, docID, count);
  }
}

/*
* Iterate over temp, storing the lower count associated with each docID in temp
*/
static void 
intersectLeft(void* args, const int docID, const int count)
{
    counters_t** countersArray = (counters_t**)args;
    counters_t* temp = countersArray[0];
    counters_t* currentCounters = countersArray[1];

    //Number of times word occured in docID in count
    int counterGot = counters_get(currentCounters, docID);

    //min is counterGot
    if (counterGot < count) {
      counters_set(temp, docID, counterGot);
    } else {
      counters_set(temp, docID, count);
    }
}


/*
* Perform in-place union on two counters, storing the result in the first 
*/
static void 
unionCounters(counters_t* temp, counters_t* currentCounters) 
{   
    counters_iterate(currentCounters, temp, unionCountersHelper);
}

/*
* Set the count in temp for docID to be the sum of temp and currentCounters counts (for the same docID)
*/
static void
unionCountersHelper(void* temp, const int docID, const int count)
{
    int tempCount = counters_get((counters_t*)temp, docID); // returns 0 if not found, no need to manually check
    counters_set((counters_t*)temp, docID, count + tempCount);
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
    // allocate memory for a counters_t* sequence. 
    // memory is later freed in main
    counters_t* sequence = counters_new();
    if (sequence == NULL) {
        fprintf(stderr, "Error creating sequence\n");
        exit(1);
    }

    counters_t* temp = NULL;
    counters_t* currentCounters = NULL;

    // iterate over each word in words
    for (int i = 0; i < wordCount; i++) {

        char* currentWord = words[i];

        // TSE implements implicit and between words, the and literal can be ignored 
        if (strcmp(currentWord, "and") == 0) {
            continue;
        }

        if (i > 0) {
            // if or is encountered, add previously-accumlated documents (temp) to confirmed to be matching documents (sequence)
            if (strcmp(currentWord, "or") == 0) {
                unionCounters(sequence, temp);

                // reset temp
                counters_delete(temp);
                temp = NULL;
                continue;
            }
        }

      
        if (temp == NULL) { 
          temp = counters_new();
          currentCounters = index_find(index, currentWord);
          unionCounters(temp, currentCounters);

        } else {
            currentCounters = index_find(index, currentWord);
            intersectCounters(temp, currentCounters);
        }
    }
    unionCounters(sequence, temp);
    counters_delete(temp);
    return sequence;
}


/*
* Master function to rank and print a sequence of matching queries 
*/
static void
printSequence(counters_t* sequence, char* pageDirectory)
{
    int sequenceSize = 0;
    maxScore_t* max;

    // iterate over sequence to obtain its size, store the result in sequenceSize
    counters_iterate(sequence, &sequenceSize, getSequenceSize);

    // empty sequence indicates no matches
    if (sequenceSize == 0) {
        printf("No documents match.\n");
        printf("-----------------------------------------------\n");
        return;
    }

    // non-empty sequence indicates matches!
    printf("Matches %d documents (ranked):\n", sequenceSize);

    // iterate over sequence, for sequenceSize iterations
    for (int i = 0; i < sequenceSize; i++) {

        // obtain the document with the highest count
        max = rankSequence(sequence);

        // print this largest count document
        printSequenceHelper(pageDirectory, max->maxdocID, max->maxCount);

        // "delete" the document by setting its count to 0
        counters_set(sequence, max->maxdocID, 0);

        // free the memory allocated in rankSequnce for the max struct
        free(max);
    }


    // print a long line once the query is done to visually seperate searches 
    printf("-----------------------------------------------\n");
}


/*
* Calculates the size of a sequence, e.g. the number of docIDs with positive (>0) counts
*/
static void
getSequenceSize(void* sequenceSize, const int docID, const int count)
{
    if (count > 0) {
        // cast sequenceSize to an int*, dereference and increment it
        (*(int*)sequenceSize) += 1;
    }
}


/*
* Print a single matching entry to stdout
*/
static void
printSequenceHelper(void* pageDirectory, const int docID, const int count)
{
    // pages with count 0 are "deleted", only consider positive docID values
    if (count > 0) {

        // convert int docID to char* charID
        char* charID = malloc(sizeof(int) + 1);
        sprintf(charID, "%d", docID);

        // construct the pathname for the given file in pageDirectory
        char* pathName = malloc(sizeof(char) * (strlen(pageDirectory) + strlen(charID) + 2)); // plus two for backslash and null terminator 

        // ensure malloc worked
        if (pathName == NULL) {
            fprintf(stderr, "Error creating pathName\n");
            exit(1);
        }

        // adds pageDirectory to newly-created pathName
        strcpy(pathName, pageDirectory);
        strcat(strcat(pathName, "/"), charID); // adds slash to the end of pathName, and the appends charID
        FILE* fp = fopen(pathName, "r");

        // the url is the first line of the file as specified in crawler
        char* url = file_readLine(fp);

        // print in the format: score   score# doc docID#: URL
        printf("score   %d doc %d: %s\n", count, docID, url);

        // frees memory alllocated in the function
        fclose(fp);
        free(url);
        free(charID);
        free(pathName);
    }
}



/*
* Find the document with the greatest count in a sequence
* Result is stored in a maxScore_t* pointer, holding both the docID and count values
* Allocated memory is freed after use in printSequence
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


/*
* Iterate over sequence, save largest count (and its associated docID) in maxScore_t* max
*/
static void
rankSequenceHelper(void* arg, const int docID, const int count)
{
    maxScore_t* max = (maxScore_t*)arg;
    if (count > max->maxCount) {
        max->maxCount = count;
        max->maxdocID = docID;
    }
}