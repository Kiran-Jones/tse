# CS50 TSE Querier
## Implementation Spec

This document focuses on the core subset of the TSE Querier:

-  Control flow: Overview of the overall program flow
-  Detailed function prototypes and their parameters
-  Error handling and recovery
-  Testing plan


## Control flow

The querier is implemented in one file `querier.c`, with several functions. The querier also uses code from other modules, including `index.c`, `pagedir.c`, and `word.c`. 

### main

The `main` function hanldes the logical flow of the querier, calling various functions to accept, clean, process, search, and print queries and their results. 

It first calls `parseArgs` to ensure correct inputs. It then enters a loop, terminating when `getQuery` returns false (indicating user EOF input). It then calls `cleanQuery` and `validateQuery` to tidy up the query and to ensure propper formatting. It creates an array of `char*` pointers called *words*, with each index in *words* being a pointer to the start of a new word in the `char* query`. The cleaned query is printed using `printCleanQuery`, and `processQuery` is called to extract a `coutners_t* sequence` collection of docID, count pairs which satisfy the query. The sequence is printed using `printSequence`, and previously-allocated memory is freed. The program will then loop back to `getQuery`, and repeat until EOF is entered as input. Once this happens, the funciton returns 0. 

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

static void intersectCounters(counters_t* counters1, counters_t* counters2);
static void intersectCountersHelper(void* counters1, const int docID, const int count);

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