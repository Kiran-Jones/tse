/* 
 * pagedir.h - header file for CS50 pagedir module
 *
 * helper module for crawler.c 
 *
 * Kiran Jones, CS50 25W
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdbool.h>
#include "../libcs50/webpage.h"

/*
* Initalizer function for the pageDirectory
* Ensures that pageDirectory is a valid path
* Creates a .crawler file in the provided pageDirectory
* Returns true on success, false if error
* 
*/
bool pagedir_init(char* pageDirectory);



/*
* Creates a file representing a webpage in pageDirectory
* Filename is docID, file location is pageDirectory/docID
* File contents are the page->url, page->depth, page->html
*/
void pagedir_save(webpage_t* page, char* pageDirectory, int docID);


bool pagedir_validate(char* pageDirectory);

webpage_t* pagedir_load(char* pageDirectory, int docID);