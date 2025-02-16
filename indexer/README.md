# Indexer

## Implementation Notes
The indexer takes three arguments – `indexer pageDirectory indexFilename`. The file `pageDirectory` is a crawler-produced directory storing webpages, with the file name being a document ID (integer), and the contents being the page's URL, page's depth from the source page during crawling, and the page's full HTML contents. The indexer creates an inverted-index data strucutre – as implemented in `index.c` – which stores words cooresponding to counters with docID-count pairs. Further implementation specifications are outlined in the [Indexer Implementation Spec](IMPLEMENTATION.md). 


## Assumptions
There were no assumptions other than those outlined in the assignment directions taken with the design of indexer. The program performs basic valid input checks for the command line arguments, and exits with an error if there were problems. Upon a non-recoverable failure, the program prints a unique note to stderr and exits. 


## Complilation and Testing
The commands `make` or `make all` complile all needed files and creates two excecutables: indexer and indextest. To compile only one, `make indexer` or `make indextest` can be used. For testing, `make test` runs a series of tests outlined in `testing.sh`. For cleaning, `make clean` can be used to delete all files created by other `make` commands. 

## Known Issues
There are no known issues with this module. 