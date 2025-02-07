/* 

crawler.c

Kiran Jones, CS50 25W

*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "hashtable.h"
#include "webpage.h"
#include "bag.h"
#include "pagedir.h"


// args: crawler seedURL pageDirectory maxDepth

// bag of pages not yet explored 
// hashtable of explored URLs (start at 200)



int
main(int argc, char* argv[])
{
    parseArgs(argc, argv, argv[1], argv[2], argv[3]);
    crawl(argv[1], argv[2], argv[3]);

    return 0;
}

static void
parseArgs(const int argc, char* argv[], char** seedURL, char** pageDirectory, int* maxDepth) 
{
    if (argc != 4) {
        printf("Invalid number of arguments: %d\n", argc);
        exit(1);
    }

    if (seedURL == NULL || pageDirectory == NULL || maxDepth == NULL) {
        exit(1);
    }


    char* normalizedURL = normalizeURL(seedURL);
    if (normalizedURL == NULL) {
        fprintf(stderr, "Error normalizing URL: %s\n", seedURL);
        exit(1);
    }
    free(normalizedURL);

    if (!isInternalURL(normalizedURL)) {
        fprintf(stderr, "Not an internal URL: %s\n", normalizedURL);
        exit(1);
    }

    if (!pagedir_init(pageDirectory)) {
        fprintf(stderr, "Error initialzing pageDirectory\n");
        exit(1);
    }

    if (maxDepth < 0 || maxDepth > 10) {
        fprintf(stderr, "Invalid maxDepth: %d, must be between 0 and 10\n", maxDepth);
        exit(1);
    }

}





static void
crawl(char* seedURL, char* pageDirectory, const int maxDepth)
{
    int num_slots = 200; // use 200 slots for table
    hashtable_t* ht = hashtable_new(num_slots);
    if (ht == NULL) {
        fprintf(stderr, "Error creating hashtable\n");
        exit(1);
    }

    bag_t* bag = bag_new();
    if (bag == NULL) {
        fprintf(stderr, "Error creating bag\n");
        exit(1);
    }

    webpage_t* seedPage = webpage_new(seedURL, 0, NULL); // need to normalize URL? 
    if (seedPage == NULL) {
        fprintf(stderr, "Error creating seedPage\n");
        exit(1);
    }
    
    
    // add seedPage to bag and hashtable
    bag_insert(bag, seedPage);
    hashtable_insert(ht, seedURL, "");

    int docID = 1;
    webpage_t* currentPage;
    while ( (currentPage = bag_extract(bag)) != NULL) {

        // fetch html of currentPage (stored in currentPage->html)
        if (webpage_fetch(currentPage)) {

            // save currentPage to page directory 
            pagedir_save(currentPage, pageDirectory, docID);

            // increment docID
            docID ++;

            // search for embedded if not at max depth
            if (webpage_getDepth(currentPage) < maxDepth) {                
                pageScan(currentPage, bag, ht);
            }
        }
        webpage_delete(currentPage);
    }
    hashtable_delete(ht, itemdelete);
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
    while ((url = webpage_getNextURL(page, &pos)) != NULL) {
        normalizedURL = normalizeURL(url);
        if (normalizedURL != NULL && isInternalURL(normalizedURL)) {
            if (hashtable_insert(pagesSeen, normalizedURL, "")) {
                webpage_t* newPage = webpage_new(normalizedURL, webpage_getDepth(page) + 1, NULL);
                bag_insert(pagesToCrawl, newPage);
            }
        }
        free(url);
        free(normalizedURL);
    }
}


static void
itemdelete(void* item) 
{
    if (item != NULL) {
        webpage_delete(item);
    }
}