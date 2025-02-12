/* 
 * pagedir.h - header file for CS50 pagedir module
 *
 * helper module for crawler.c 
 *
 * Kiran Jones, CS50 25W
 */


/*
* Initalizer function for the pageDirectory
* Ensures that pageDirectory is a valid path
* Creates a .crawler file in the provided pageDirectory
* Returns true on success, false if error
* 
*/
bool pagedir_init(const char* pageDirectory);



/*
* Creates a file representing a webpage in pageDirectory
* Filename is docID, file location is pageDirectory/docID
* File contents are the page->url, page->depth, page->html
*/
void pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID);