# Common

## Implementation Notes
This module contains code that will be used by the `crawler`, `indexer`, and `querier`. In its current state, it contains `pagedir.h` and `pagedir.c`. `pagedir.c` saves pages as files, and marks the used directories as Crawler-produced (by including a blank .crawler file). 

## Assumptions
There were no assumptions other than those outlined in the assignment directions taken with the design of crawler. The program uses exit codes 20-24 to indicate an error, printing a unique note to stderr and exiting upon failure. These codes can be easily cross-referenced for debugging if needed. 


## Complilation and Testing
`make` create a library, `common.a`. `make clean` removes all files produced by make (i.e. the library `common.a`)

## Known Issues
There are no known issues with this module. 