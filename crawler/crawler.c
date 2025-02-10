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

    
    char* seedURL = NULL;
    char* pageDirectory = NULL;
    int maxDepth = 0;
    parseArgs(argc, argv, &seedURL, &pageDirectory, &maxDepth);
    crawl(seedURL, pageDirectory, maxDepth);

    return 0;
}

static void
parseArgs(const int argc, char* argv[], char** seedURL, char** pageDirectory, int* maxDepth) 
{
    if (argc != 4) {
        printf("Invalid number of arguments: %d\n", argc);
        printf("Correct format: crawler seedURL pageDirectory maxDepth\n");
        exit(1);
    }


    char* normalizedURL = normalizeURL(argv[1]);
    if (normalizedURL == NULL) {
        fprintf(stderr, "Error normalizing URL (argv1)\n");
        exit(2);
    }

    if (!isInternalURL(normalizedURL)) {
        free(normalizedURL);
        fprintf(stderr, "Not an internal URL (argv1)\n");
        exit(3);
    }

    *seedURL = normalizedURL;

    if (!pagedir_init(argv[2])) {
        fprintf(stderr, "Error initialzing pageDirectory (argv2)\n");
        exit(4);
    }

    *pageDirectory = argv[2];

    int depth;

    if (sscanf(argv[3], "%d", &depth) != 1) {
        fprintf(stderr, "Error parsing maxDepth (argv3)\n");
        exit(5);
    }

    *maxDepth = depth;

    if (*maxDepth < 0 || *maxDepth > 10) {
        fprintf(stderr, "Invalid maxDepth, must be between 0 and 10 (argv3)\n");
        exit(6);
    }

}





static void
crawl(char* seedURL, char* pageDirectory, const int maxDepth)
{
    int num_slots = 200; // use 200 slots for table
    hashtable_t* ht = hashtable_new(num_slots);
    if (ht == NULL) {
        fprintf(stderr, "Error creating hashtable\n");
        exit(7);
    }

    bag_t* bag = bag_new();
    if (bag == NULL) {
        fprintf(stderr, "Error creating bag\n");
        exit(8);
    }

    webpage_t* seedPage = webpage_new(seedURL, 0, NULL); // need to normalize URL? 
    if (seedPage == NULL) {
        fprintf(stderr, "Error creating seedPage\n");
        exit(9);
    }
    
    
    // add seedPage to bag and hashtable
    bag_insert(bag, seedPage);
    hashtable_insert(ht, seedURL, "");

    int docID = 1;
    webpage_t* currentPage;
    while ( (currentPage = bag_extract(bag)) != NULL) {

        // fetch html of currentPage (stored in currentPage->html)
        if (webpage_fetch(currentPage)) {
            logr("Fetched", webpage_getDepth(currentPage), webpage_getURL(currentPage));

            // save currentPage to page directory 
            pagedir_save(currentPage, pageDirectory, docID);


            // increment docID
            docID ++;

            // search for embedded if not at max depth
            if (webpage_getDepth(currentPage) < maxDepth) {      
                logr("Scanning", webpage_getDepth(currentPage), webpage_getURL(currentPage));
                pageScan(currentPage, bag, ht);
            }
        }
        webpage_delete(currentPage);
    }
    hashtable_delete(ht, NULL); // all values in ht are "", no need to free them
    bag_delete(bag, itemdelete);
}



static void
pageScan(webpage_t* page, bag_t* pagesToCrawl, hashtable_t* pagesSeen)
{
    if (page == NULL || pagesToCrawl == NULL || pagesSeen == NULL) {
        exit(10);
    }
    int pos = 0;
    char* url;
    char* normalizedURL;
    while ( (url = webpage_getNextURL(page, &pos)) != NULL) {
        normalizedURL = normalizeURL(url);

        // used complex if-else structure to allow for propper logging messages 
        if (normalizedURL == NULL) {
            free(normalizedURL);
        } else {
            logr("Found", webpage_getDepth(page) + 1, normalizedURL);
            if (isInternalURL(normalizedURL)) {
                if (hashtable_insert(pagesSeen, normalizedURL, "")) {
                    webpage_t* newPage = webpage_new(normalizedURL, webpage_getDepth(page) + 1, NULL);
                    bag_insert(pagesToCrawl, newPage);
                    logr("Added", webpage_getDepth(newPage), webpage_getURL(newPage));
                } else {
                    logr("IgnDupl", webpage_getDepth(page) + 1, normalizedURL);
                    free(normalizedURL);
                }
            } else {
                logr("IgnExtern", webpage_getDepth(page) + 1, normalizedURL);
                free(normalizedURL);
            }
        }
        free(url);
    }
}




static void
itemdelete(void* item) 
{
    if (item != NULL) {
        free(item);
    }
}

// logging function from crawler-logging.md
static void logr(const char *word, const int depth, const char *url)
{
#ifdef LOGGING
  printf("%2d %*s%9s: %s\n", depth, depth, "", word, url);
#else
  ;
#endif
}