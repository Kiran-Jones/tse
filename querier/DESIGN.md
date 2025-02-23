# CS50 TSE Querier
## Design Spec

The the TSE *Querier* is a standalone program that reads the document files produced by the TSE crawler, builds an index, and writes that index to a file. 

### User interface

The Querier's only interface with the user is on the command-line; it must always have two arguments (in addition to the program executable).

```
Querier pageDirectory indexFilename
```

For example, if `letters` is a pageDirectory in `../data`, `letters.index` is an indexer-producted index, and `./querier` is the executable of `querier.c`,

``` bash
$ ./querier ../data/letters ../data/letters.index
```

### Inputs and outputs

**Input**: The Querier reads input and loops until the user terminates the program. Each submitted line is refered to as a unique "query". Each query is treated independetly. 

**Output**: For each submitted query, the querier finds all matching documents in the provided index file. It then calculates a score for each document, and prints all matches in order of decreasing score. For each match, the queier returns the determined score, the unique document ID, and a hyperlink to the document.  

### Functional decomposition into modules

The querier uses many functions to parse and clean arguments, locate matching pages, union and intersect coutnters data structures, rank matches, and print results. 
Some of the primary functions are as follows: 

 1. *main*, which parses arguments and holds a loop to continually process queries. 
 2. *getQuery*, which dynamically allocates memory and stores a provided query
 3. *cleanQuery*, which removes extraneous spaces and capitalizations from the query
 4. *validateQuery*, which ensures that the query has propper operation placement - "and" and "or" – and does not contain extraneous characters
 5. *processQuery*, which iterates over the query, building a collection of matching documents as it runs. It also supports "and" and "or" operations, recording matching documents as such
 6. *intersectCounters*, which calculates the intersection of two counters objects
 7. *unionCounters*, which calculates the union of two counters objects
 8. *rankSequence*, which ranks the calculated list of matching documents by score

And some helper modules that provide data structures:

 1. *index*, a module providing the data structure to represent the in-memory index, and functions to read and write index files;
 1. *webpage*, a module providing the data structure to represent webpages, and to scan a webpage for words;
 2. *pagedir*, a module providing functions to load webpages from files in the pageDirectory;
 4. *word*, a module providing a function to normalize a word.

### Pseudo code for logic/algorithmic flow

The Querier will run as follows:

     parse the command line, validate parameters, initialize other modules
     while there is a query to process
       store the query
       create a new 'index' object
       clean the query
       validate the query
       print the cleaned query
       calculated matching documents
       print the matching documents by descending score

where *getQuery:*

      allocates memory for string of initial size
      while there are more characters to read
        if character signals EOF
          return false
        if not enough memory is in the string to hold more characters
          reallocate more memory
        set the next character in the string to newly-read character
      add terminating character
      return true

where *cleanQuery:*

     create boolean tracking if in word
       for each character in the query
       if currently in a word:
        if the current character a space:
          change boolean to not be in a word (reached the end of the word)
        else if the current character is not a space:
          normalize the character
      else if current character is not a space:
        change boolean to be in a word (start of new word)
        normalize character


where *validateQuery:*

     ensure the first and last words are not literals (and/or)
     loop over query words
       ensure no adjacent literals
      loop over query characters
        ensure only alphabetic characters 


where *processQuery:*

     create sequence to track confirmed matching documents
     create temp to track running total of currently matching documents 
     for each word in the query
       if word is or:
        union previously accumulated temp to sequence
       if word is and:
        continue
       intersect documents cooreponding to word with temp
     union sequence and temp
     return the sequence 


where *intersectCounters:*

     iterate over counters, setting the count of documents not appearing in both to 0


where *unionCounters:*

     iterate over counters, adding docID and count to the first if docID does not exist, or taking the lower count of it does
  

where *rankSequence:*

     iterate over the sequence, returning the highest count and docID pair

### Major data structures
To store the query three *counters* are used: sequence, temp, and currentCounters. Sequence stores all documents confirmed to be in the final set. For example, if the querier encounters an *or*, all documents matching up untill the *or* will be valid, and are stored. *temp* stores the currently matching set of documents. For example, if the querier has processed word1 *and* word2, temp will store the intersection of word1 and word2's documents. Lastly, *currentCounters* stores the counter of the current word. This is used in conjunction with `intersectCounters()`, which will add it to temp. 

Another important data structure is the *index*, which maps from *word* to *(docID, #occurrences)* pairs. This is used to retrieve the docID, count pairs for each word. 
The *index* is a *hashtable* keyed by *word* and storing *counters* as items.
The *counters* is keyed by *docID* and stores a count of the number of occurrences of that word in the document with that ID. 
