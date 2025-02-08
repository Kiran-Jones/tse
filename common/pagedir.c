/*
pagedir.c

Kiran Jones, CS50 25W

*/

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "webpage.h"



bool 
pagedir_init(const char* pageDirectory)
{
    /*
    construct the pathname for the .crawler file in that directory
    open the file for writing; on error, return false.
    close the file and return true.
    */

   char* crawlerExtension = '/.crawler';

   char* pathName = malloc(sizeof(char) * ( strlen(pageDirectory) + strlen(crawlerExtension) + 1 ));
   if (pathName == NULL) {
    fprintf(stderr, "Error creating pathName");
    exit(11);
   }

    // copy the path that leads to the directory to the newly-allocated space
   strcpy(pathName, pageDirectory);
   strcat(pathName, crawlerExtension);

   FILE* fp = fopen(pathName, "w");

   if (fp == NULL) {
    free(pathName);
    return false;
   }

   fclose(fp);
   free(pathName);
   return true;

}



// creates a file for the given webpage and writes the URL, depth, and html to it
void 
pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID)
{
    /*
    construct the pathname for the page file in pageDirectory
    open that file for writing
    print the URL
    print the depth
    print the contents of the webpage
    close the file
    */
    char* pathName = malloc(sizeof(char) * (strlen(pageDirectory) + sizeof(int)));
    if (pathName == NULL) {
        fprintf(stderr, "Error creating pathName");
        exit(12);
    }

    char* charID = malloc(sizeof(int) + 1);
    sprintf(charID, "%d", docID);

    strcpy(pathName, pageDirectory);
    strcat(pathName, charID);

    free(charID);

    FILE* fp = fopen(pathName, "w");
    
    if (fp == NULL) {
        free(pathName);
        exit(13);
    }

    fprintf(fp, "%s", page->)





}