# Crawler

## Implementation Notes
The crawler takes four arguments – `crawler seedURL pageDirectory maxDepth`. It "crawls", begining at the seedURL and traveling maxDepth connections away using embeded page links. 
The crawler creates a file located in pageDirectory for each page it visits. Each created file includes a page's hyperlink, depth from the seedURL, and full HTML content. 

This program makes use of the `hashtable`, `bag`, and `webpage` modules provided in `libcs50`. The hashtable is used to store URLs, the bag is used to maintain a collection of pages to visit, and the webpage module is used to create `webpage_t` instances and interact with web pages. The crawler performs an approximation of a breadth-first search (pages are explored in an unpredictable order, not a true BFS) to scrape embeded links and to store these pages to be later visited. To create files for each page, the crawler uses `pagedir.c`, a program which creates files to write to, and saves page content with the specified format (link, depth from seedURL, full HTML content). 

## Assumptions
There were no assumptions other than those outlined in the assignment directions taken with the design of crawler. The program performs basic valid input checks for the command line arguments, and exits with an error if there were problems. The program uses exit codes 1-10 to indicate an error, printing a unique note to stderr and exiting upon failure. These codes can be easily cross-referenced for simple debugging/input errors. 


## Complilation and Testing
`make` compliles all needed files and creates an excecutable, named crawler. `make test` can be used to test the crawler. By default, verbose logging is enabled. To disable this feature, comment out the line `test: TESTING = -DLOGGING`, as specified in the Makefile. `make valgrind` will run a memory test using crawler, which is passed without memory leaks. Finally, `make clean` can be used to delete all files created by other `make` commands. 