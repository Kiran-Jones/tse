/* 

crawler.c

Kiran Jones, CS50 25W

Error exit codes 1-10

*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../libcs50/hashtable.h"
#include "../libcs50/bag.h"
#include "../libcs50/webpage.h"
#include "../common/pagedir.h"


// args: crawler seedURL pageDirectory maxDepth

// bag of pages not yet explored 
// hashtable of explored URLs (start at 200)

static void parseArgs(const int argc, char* argv[], char** seedURL, char** pageDirectory, int* maxDepth);

static void crawl(char* seedURL, char* pageDirectory, const int maxDepth);

static void pageScan(webpage_t* page, bag_t* pagesToCrawl, hashtable_t* pagesSeen);

static void itemdelete(void* item);

static void logr(const char *word, const int depth, const char *url);


int
main(int argc, char* argv[])
{

    // initialize seedURL and pageDirectory to NULL
    char* seedURL = NULL;
    char* pageDirectory = NULL;

    // initialize maxDepth to 0;
    int maxDepth = 0;

    // parse arguments
    parseArgs(argc, argv, &seedURL, &pageDirectory, &maxDepth);

    // if arguments are valid, crawl
    crawl(seedURL, pageDirectory, maxDepth);

    return 0;
}

/*
* helper function to validate command line arguments
* prints to stderr and exits if there is a problem
* otherwise, assign seedURL, pageDirectory, and maxDepth using args
*/
static void
parseArgs(const int argc, char* argv[], char** seedURL, char** pageDirectory, int* maxDepth) 
{
    // only four arguments
    if (argc != 4) {
        printf("Invalid number of arguments: %d\n", argc);
        printf("Correct format: crawler seedURL pageDirectory maxDepth\n");
        exit(1);
    }

    // calls normalizeURL on argv[1]
    char* normalizedURL = normalizeURL(argv[1]);

    // exit if error normalizing URL
    if (normalizedURL == NULL) {
        fprintf(stderr, "Error normalizing URL (argv1)\n");
        exit(2);
    }

    // check if URL is internal, exit if not
    if (!isInternalURL(normalizedURL)) {
        free(normalizedURL);
        fprintf(stderr, "Not an internal URL (argv1)\n");
        exit(3);
    }

    // checks passed, assign to seedURL
    *seedURL = normalizedURL;

    // attempt to initialize pagedir, exit if issue
    if (!pagedir_init(argv[2])) {
        fprintf(stderr, "Error initialzing pageDirectory (argv2)\n");
        exit(4);
    }

    // checks passed, assign to pageDir
    *pageDirectory = argv[2];

    int depth;

    // attempt to convert argv[3] to int depth, exit if error
    if (sscanf(argv[3], "%d", &depth) != 1) {
        fprintf(stderr, "Error parsing maxDepth (argv3)\n");
        exit(5);
    }

    // checks passed, assign maxDepth
    *maxDepth = depth;

    // if maxDepth is valid integer but not 0 ≤ maxDepth ≤ 10 exit
    if (*maxDepth < 0 || *maxDepth > 10) {
        fprintf(stderr, "Invalid maxDepth, must be between 0 and 10 (argv3)\n");
        exit(6);
    }

}




/*
* crawl webpages, starting at seedURL, storing to pageDir, and reaching depth of maxDepth
* uses a "bfs" style of traversal 
*/
static void
crawl(char* seedURL, char* pageDirectory, const int maxDepth)
{
    int num_slots = 200; // use 200 slots for table as reccomended in directions 

    // create hashtable, exit if error
    hashtable_t* ht = hashtable_new(num_slots);
    if (ht == NULL) {
        fprintf(stderr, "Error creating hashtable\n");
        exit(7);
    }

    // create bag, exit if error
    bag_t* bag = bag_new();
    if (bag == NULL) {
        fprintf(stderr, "Error creating bag\n");
        exit(8);
    }

    // create seedPage, exit if error
    webpage_t* seedPage = webpage_new(seedURL, 0, NULL);
    if (seedPage == NULL) {
        fprintf(stderr, "Error creating seedPage\n");
        exit(9);
    }
    
    
    // add seedPage to bag and hashtable to start traversal 
    bag_insert(bag, seedPage);
    hashtable_insert(ht, seedURL, ""); // add "" as value in hashtable, as suggested in directions 

    // start docID at 1
    int docID = 1;
    webpage_t* currentPage;

    // search while there are pages left to search
    while ( (currentPage = bag_extract(bag)) != NULL) {

        // fetch html of currentPage (stored in currentPage->html)
        if (webpage_fetch(currentPage)) {
            logr("Fetched", webpage_getDepth(currentPage), webpage_getURL(currentPage));

            // save currentPage to page directory 
            pagedir_save(currentPage, pageDirectory, docID);


            // increment docID
            docID ++;

            // search for embedded if not at max depth (get neighbors)
            if (webpage_getDepth(currentPage) < maxDepth) {      
                logr("Scanning", webpage_getDepth(currentPage), webpage_getURL(currentPage));
                pageScan(currentPage, bag, ht);
            }
        }
        
        // delete webpage once done scanning it
        webpage_delete(currentPage);
    }
    
    // delete hashtable and bag once search is done
    hashtable_delete(ht, NULL); // all values in ht are "", no need to free them
    bag_delete(bag, itemdelete);
}



static void
pageScan(webpage_t* page, bag_t* pagesToCrawl, hashtable_t* pagesSeen)
{
    // null checking 
    if (page == NULL || pagesToCrawl == NULL || pagesSeen == NULL) {
        exit(10);
    }
    int pos = 0;
    char* url;
    char* normalizedURL;
    while ( (url = webpage_getNextURL(page, &pos)) != NULL) {
        normalizedURL = normalizeURL(url);

        // used complex if-else structure to allow for propper logging messages
        // structure was more concise pre-logging, however, to propperly differentiate 
        // between duplicate and external pages for logging extra if-else blocks are needed

        // null check
        if (normalizedURL == NULL) {
            free(normalizedURL);
        } else {

            // found URL
            logr("Found", webpage_getDepth(page) + 1, normalizedURL);
            if (isInternalURL(normalizedURL)) {
                if (hashtable_insert(pagesSeen, normalizedURL, "")) {
                    
                    // found new URL! create instance, insert into bag, print
                    webpage_t* newPage = webpage_new(normalizedURL, webpage_getDepth(page) + 1, NULL);
                    bag_insert(pagesToCrawl, newPage);
                    logr("Added", webpage_getDepth(newPage), webpage_getURL(newPage));

                } else { // URL was already found, print and free
                    logr("IgnDupl", webpage_getDepth(page) + 1, normalizedURL);
                    free(normalizedURL);
                }

            } else { // URL is external, print and free
                logr("IgnExtern", webpage_getDepth(page) + 1, normalizedURL);
                free(normalizedURL);
            }
        }
        // done with url (non normalized), free it
        free(url);
    }
}



/*
* item delete funciton for generic void* item 
*/
static void
itemdelete(void* item) 
{
    if (item != NULL) {
        free(item);
    }
}

// logging function from crawler-logging.md
// can be enabled/disabled via command line/in make test
// see Makefile for more detailed instructions
static void logr(const char *word, const int depth, const char *url)
{
#ifdef LOGGING
  printf("%2d %*s%9s: %s\n", depth, depth, "", word, url);
#else
  ;
#endif
}