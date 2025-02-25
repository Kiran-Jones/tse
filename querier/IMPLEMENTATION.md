# CS50 TSE Querier
## Implementation Spec

This document focuses on the core subset of the TSE Querier:

-  Control flow: Overview of the overall program flow
-  Query processing
-  Detailed function prototypes and their parameters
-  Testing plan


## Control flow

The querier is implemented in one file `querier.c`, with several functions. The querier also uses code from other modules, including `index.c`, `pagedir.c`, and `word.c`. 

### main

The `main` function handles the logical flow of the querier, calling various functions to accept, clean, process, search, and print queries and their results. 

It first calls `parseArgs` to ensure correct inputs. It then enters a loop, terminating only when `getQuery` returns false (indicating user EOF input). After accepting a query, it calls `cleanQuery` and `validateQuery` to tidy up the query and to ensure propper formatting. It creates an array of `char*` pointers called *words*, with each index in *words* being a pointer to the start of a new word in the `char* query`. The cleaned query is printed using `printCleanQuery`, and `processQuery` is called to extract a `coutners_t* sequence` collection of docID, count pairs which satisfy the query. The sequence is printed using `printSequence`, and previously-allocated memory is freed. The program will then loop back to `getQuery`, and repeat until EOF is entered as input. Once this happens, the function returns 0. 


## Query processing

The querier supports the full functionality outlined in the project description, that is precedence of *and* over *or* and printing by decreasing score. To acomplish this, the querier makes use of two main helper functions: `intersectCounters` and `unionCounters`. These operations implement the logic for *and* and *or* operations, respectively. 

The logic for processing queries is implemented in the aptly named function `processQuery`. This function uses several counters. The first, called *sequence* stores documents which are confirmed to be valid per the query. For example, upon encountering an *or* statement, we know all documents matching the query up to that point will be valid. Similarly, once the end of the query is reached the currently-matching documents are implicitly valid; these are stored in *sequence*. 

Another counters named *temp* acts as an accumulator: it stores documents that currently match the query. For example, when processing a series of *and* statements (or a series of adjacent words due to the implicit *and* operation of the TSE) *temp* will first contain all matches of *word1*, then all the matches of *word1* intersected with the matches of *word2*, then all the matches of *word1* intersected with the matches of *word2* intersected with the matches of *word3*, etc. 

The last counters is *currentCounters* and represents the documents associated with the current word. This is used in conjuction with the helper functions `intersectCounters` and `unionCounters` to modify temp. *currentCounters* never interacts with *sequence*, rather *currentCounters* is added to *temp* via intersection and union, and *temp* is later unioned to *sequence* upon encountering an *or* or the end of the query. Note: *currentCounters* is also a pointer to a *counters_t\** located within the `index_t* index`. As a result, *currentCounters* is never edited, rather *temp* is changed using information from *currentCounters*.

## intersectCounters

This function takes two `counters_t*` objects. It performs an intersection *in place* on the first argument. Using the previously outlined counters, *temp* is always the first argument and *currentCounters* is the second. Two helper functions, `intersectLeft` and `intersectRight` are used to perform the intersection. The function passes an array of containing both *temp* and *currentCounters* to each intersect function. This structure allows us to overcome the limitation of not being able to simultaneously iterate over and modify a single counter. The two helper functions act primarily the same, the only difference being editing "left" counter vs. the "right" counter: cooresponding to where *temp* was passed in. Each iterates over a counter, and sets the value of the docID in temp to the minimum value associated with the docID in two counters. 

## unionCounters

This function also accepts two `counters_t*` objects and performs an *in place* union. Like `intersectCounters`, the result of the operation is stored in the first argument, e.g. *temp*. It calls *unionCountersHelper*, which sets the value of each docID in *currentCounters* to be the sum of its value in *currentCounters* and *temp*.

## rankSequence

The querier also supports sequence ranking. This is done through the function `rankSequence`. This implements a selection sort algorithm, iterating over the compiled *sequence* of matching documents and selecting the largest count. This largest count is then printed, and then set to 0. This process repeats for all entries in *sequence*. As a note, the counters data structure does not support deleting individual count-value pairs, so setting the count to 0 signifies that the entry is "deleted" and it is then ignored. This algorithm is not particularly efficient – O(n^2) runtime – however, our *sequence* counters is generally relatively small making this a nonissue given the current size of the pageDirectories and index files. If this were to be used with a larger and a search returned many results, this could hurt performance. 

## Function prototypes

### querier
```c
int main(int argc, char* argv[]);

static void parseArgs(int argc, char* argv[], char** pageDirectory, char** indexFilename);

static void prompt(void);
int fileno(FILE *stream);
static bool getQuery(char** query);
static void cleanQuery(char* query, char** words, int* wordCount);
static void printCleanQuery(char** words, int wordCount); 
static void validateQuery(char** words, int wordCount);

static void intersectCounters(counters_t* temp, counters_t* counter);
static void intersectLeft(void* args, const int docID, const int count);
static void intersectRight(void* args, const int docID, const int count);

static void unionCounters(counters_t* counters1, counters_t* counters2);
static void unionCountersHelper(void* counters1, const int docID, const int count);

counters_t* processQuery(char** words, int wordCount, index_t* index);

static void printSequence(counters_t* sequence, char* pageDirectory);
static void getSequenceSize(void* sequenceSize, const int docID, const int count);
static void printSequenceHelper(void* pageDirectory, const int docID, const int count);

maxScore_t* rankSequence(counters_t* sequence);
static void rankSequenceHelper(void* arg, const int docID, const int count);
```

## Error handling and recovery

All the command-line parameters are rigorously checked before any data structures are allocated or work begins; problems result in a message printed to stderr and a non-zero exit status.

Out-of-memory errors are handled by checking for NULL after allocating memory. 

All code uses defensive-programming tactics to catch and exit, e.g., if a function receives bad parameters.

## Testing plan

Here is an implementation-specific testing plan.

### Integration/system testing
The `querier` uses a testing script `testing.sh`. This file is designed to test both edge cases and normal excucution when using the `querier`. It checks invalid command-line arguments and invalid query prompts, as well as correct hard-coded example runs. `valgrind` is used to check for propper memory-allocation, and the program `fuzzquery.c` is used to test for robustness. Running the script with the command `bash -v testing.sh` will produce verbose output logging, ensuring easy vertification of the results. 
