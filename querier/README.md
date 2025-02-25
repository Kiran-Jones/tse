# Querier

## Implementation Notes
The querier takes three arguments – `querier pageDirectory indexFilename`. The file `pageDirectory` is a crawler-produced directory storing webpages, with the file name being a document ID (integer), and the contents being the page's URL, page's depth from the source page during crawling, and the page's full HTML contents.The file `indexFilename` is an indexer-produced inverted index, with each line cooresponding to a unique word, followed by docID-count pairs describing the word. Further implementation specifications are outlined in the [Querier Implementation Spec](IMPLEMENTATION.md). 


## Assumptions
There were no assumptions other than those outlined in the assignment directions taken with the design of indexer. The program performs basic valid input checks for the command line arguments, and exits with an error if there were problems. Upon a non-recoverable failure, the program prints a unique note to stderr and exits. The program assumes that `pageDirectory` and `indexFilename` were created using the `crawler` and `indexer` modules, respectivly. The querier does not perform extensive checking of these file contents beyond verifying basic access permissions. This assumption matches the specification outlined in the Requirements Spec. 


## Complilation and Testing
The commands `make` or `make all` complile all needed files and creates an excecutable *querier*. For testing, `make test` runs a series of tests outlined in `testing.sh`. For cleaning, `make clean` can be used to delete all files created by other `make` commands. 

## Functionality 
The querier implements the full functionality described in the specs. It supports both *and* and *or* operators, with precedence for *and* over *or*. The program also prints the set of documents which match the query by descending score. These scores are calculated per the specs also, with *and* operations taking the minimum count for all words in the sequence and *or* operations calculating the sum of each word's count in the sequence.

## Known Issues
There are no known issues with this module. 