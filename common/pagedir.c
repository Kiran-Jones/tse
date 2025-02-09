/*
pagedir.c

Kiran Jones, CS50 25W

Error exit codes 20-24

*/

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "../libcs50/webpage.h"



bool 
pagedir_init(const char* pageDirectory)
{
    /*
    construct the pathname for the .crawler file in that directory
    open the file for writing; on error, return false.
    close the file and return true.
    */

   // extension 
   char* crawlerExtension = "/.crawler";
    // malloc space the size of length pageDirectory + length crawlerExtension + null terminator
   char* pathName = malloc(sizeof(char) * ( strlen(pageDirectory) + strlen(crawlerExtension) + 1 ));
   if (pathName == NULL) {
    fprintf(stderr, "Error creating pathName");
    exit(20);
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
    char* charID = malloc(sizeof(int) + 1);
    sprintf(charID, "%d", docID);

    char* pathName = malloc(sizeof(char) * (strlen(pageDirectory) + strlen(charID) + 2)); // plus two for backslash and null terminator 
    if (pathName == NULL) {
        fprintf(stderr, "Error creating pathName\n");
        exit(21);
    }

    

    strcpy(pathName, pageDirectory);
    strcat(strcat(pathName, "/"), charID); // adds slash to the end of pathName, and the appends charID

    free(charID);

    FILE* fp = fopen(pathName, "w");
    
    if (fp == NULL) {
        free(pathName);
        fprintf(stderr, "Unable to open file for writing\n");
        exit(22);
    }

    char* url = webpage_getURL(page);
    if (url == NULL) {
        fprintf(stderr, "Error getting webpage from URL\n");
        exit(23);
    }

    fprintf(fp, "%s\n", url);

    int depth = webpage_getDepth(page);

    fprintf(fp, "%d\n", depth);

    char* html = webpage_getHTML(page);
    if (html == NULL) {
        fprintf(stderr, "Error getting HTML from URL\n");
        exit(24);
    }

    fprintf(fp, "%s\n", html);

    printf("saved %s to file\n", webpage_getURL(page));
    free(pathName);
    fclose(fp);

    // printf("saved pageID: %d saved to file\n", docID);


}