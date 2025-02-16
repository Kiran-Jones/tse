# CS50 TSE Indexer
## Implementation Spec

This document focuses on the core subset of the TSE indexer:

-  Data structures
-  Control flow: pseudo code for overall flow, and for each of the functions
-  Detailed function prototypes and their parameters
-  Error handling and recovery
-  Testing plan

## Data structures 

The main data structure used is an `index`. An `index` is a hashtable of `char*` keys – each representing a unique word – and `counters_t*` items. The `counters_t*` counters each store a `int` docID and corresponding `int` count, representing the number of occorences of word in each docID.


## Control flow

The Crawler is implemented in one file `indexer.c`, with four functions. Indexer also uses code from other modules, including `index.c`, `pagedir.c`, and `word.c`. 

### main

The `main` function first calls `parseArgs` to ensure correct inputs. It then calls `indexBuild` to create the inverted-index data structure from crawler-produced directory `pageDirectory` and finally calls `index_write` to write the created index to `indexFilename`. The function terminates by exiting with code 0. 

### parseArgs

Given arguments from the command line, extract them into the function parameters; return only if successful.

* for `pageDirectory`, call `pagedir_validate()`
* for `indexFilename`, attempt to open with write permissions
* if any trouble is found, print an error to stderr and exit non-zero.

### indexBuild

Build an `index_t*` index from the provided `pageDirectory`.
Pseudocode:
```
create a new index object
while there are webpages to be loaded from pageDirecotry
    add the current webpage to the index
    delete the instance of the current webpage
return the index
```

### indexPage

Adds every word in webpage to the index. 
Pseudocode:
```
while there are words to be read from the page
    if the word is significant (> 2 characters)
        normalize the word (make it all lowercase)
        if there is a counter for this word with the given docID
            increment the counter
        else
            create a counter for the given docID, initalize it to 1 and add it to the index
    free the word
```

## Other modules

### index

This module creates a data structure for the inverted-index. The structure contains standard methods such as `index_new`, `index_insert`, `index_find`, and `index_delete`. Much of the structure is a wrapper for the hashtable module. This also implements two methods, `index_write()` and `index_read()` which are used by `indexer`. 

Pseudocode for `index_write`:
```
open the file with writing permissions
for all the words in the index
    print the current word with a space
        for all counters associated with the current word
            print the docID, space, count, space
        print a new line
close the file
delete the index
```

Pseudocode for `index_read`:
```
create a new index
open the file with reading permissions 
for all the words in the file
    create a counters
        while there are docID and counts for the word
            add the docID count pairs to counters
    insert the word and its counter to the index
    free the word
close the file
return the index
```

### pagedir

This module implements `pagedir_validate()` and `pagedir_load()` which are used by the `indexer`. 

Pseudocode for `pagedir_validate`:
```
construct the pathname for the .crawler file in that directory
attempt to open the file with read permissions
if not able to access
    return false
return true

```

Pseudocode for `pagedir_load`:
```
construct the pathname for the page file in pageDirectory
open that file for reading
get URL from file
get depth from file
get HTML from file
create new webpage using URL, depth, and HTML
return webpage
```

### word

This module implements `normalize_word()` which is by the `indexer`. Note: the implementation of `normalize_word()` modifies the letters in-place using a pointer, hence there is no need to return the normalized word. 

Pseudocode for `normalize_word`:
```
for each character in the word
    convert the character to lowercase
```

### libcs50

We leverage the modules of libcs50, most notably `bag`, `hashtable`, and `webpage`.
See that directory for module interfaces.
The new `webpage` module allows us to represent pages as `webpage_t` objects, to fetch a page from the Internet, and to scan a (fetched) page for URLs; in that regard, it serves as the *pagefetcher* described in the design.
Indeed, `webpage_fetch` enforces the 1-second delay for each fetch, so our crawler need not implement that part of the spec.

## Function prototypes

### indexer
```c
int main(const int argc, char* argv[]);
static void parseArgs(int argc, char* argv[], char** pageDirectory, char** indexFilename);
static index_t* indexBuild(char* pageDirectory);
static void indexPage(webpage_t* webpage, int docID, index_t* index);
```

### index
```c
index_t* index_new(const int num_slots);
bool index_insert(index_t* index, char* word, counters_t* counters);
counters_t* index_find(index_t* index, char* word);
void index_write(index_t* index, char* indexFilename);
index_t* index_read(char* fileName);
void index_delete(index_t* index);
```
### indextest
```c
static void parseArgs(int argc, char* argv[], char** oldIndexFilename, char** newIndexFilename);
index_t* loadIndex(char* oldIndexFilename); 
```

### pagedir
```c
bool pagedir_init(char* pageDirectory);
void pagedir_save(webpage_t* page, char* pageDirectory, int docID);
bool pagedir_validate(char* pageDirectory);
webpage_t* pagedir_load(char* pageDirectory, int docID);
```

### word
```c
void normalizeWord(char* word);
```

## Error handling and recovery

All the command-line parameters are rigorously checked before any data structures are allocated or work begins; problems result in a message printed to stderr and a non-zero exit status.

Out-of-memory errors are handled by checking for NULL after allocating memory. 

All code uses defensive-programming tactics to catch and exit, e.g., if a function receives bad parameters.

## Testing plan

Here is an implementation-specific testing plan.

### Using indextest

As a whole, the `indexer` has many parts working together. The program `indextest` takes a file created by a previous run of `indexer` and re-creates an `index_t*` index from its contents. This inverted-index structure is then re-written to a new file. The originally-produced file and newly-produced files can then be checked for equivalence. Note: the created index files do not always have the same words on each line. To overcome this for testing file equivalence, we can use `indexcmp` located at `~/cs50-dev/shared/tse/indexcmp` to test for differences between the two files. The program prints each difference, e.g. if a run produces no output the files contain the same information (potentially in different orders). 

### Integration/system testing

There is a script `testing.sh` that invokes the indexer several times, with a variety of command-line arguments. 
First, a sequence of invocations with erroneous arguments, testing each of the possible mistakes that can be made for the indexer.
Second, the three directories – letters, scrape, and wikipedia – are indexed and index tested. As previously described, the `indexcmp` is used to test the created index files. The letters test is run with valgrind to showcase propper memory allocation. The other tests are not, as their significantly larger size leads to quite slow performence with valgrind. 
Third, a sequence of invocations with erroneous arguments, testing each of the possible mistakes that can be made for the indextest. These tests can be run with the command `make test`, and the output can be stored in `testing.out` with `make test &> testing.out`.