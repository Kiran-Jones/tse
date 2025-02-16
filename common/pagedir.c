/*
* pagedir.c
* 
* Kiran Jones, CS50 25W
* 
* Error exit codes 20-24
* 
*/

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdbool.h>
#include "../libcs50/webpage.h"
#include "pagedir.h"
#include "../libcs50/file.h"

bool pagedir_init(char* pageDirectory);

void pagedir_save(webpage_t* page, char* pageDirectory, int docID);

bool pagedir_validate(char* pageDirectory);

webpage_t* pagedir_load(char* pageDirectory, int docID);


/*
* Initalizer function for the pageDirectory
* Ensures that pageDirectory is a valid path
* Creates a .crawler file in the provided pageDirectory
* Returns true on success, false if error
*/
bool 
pagedir_init(char* pageDirectory)
{

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



/*
* Creates a file for the given webpage and writes the URL, depth, and html to it
*/
void 
pagedir_save(webpage_t* page, char* pageDirectory, int docID)
{
    // create a char* of docID
    char* charID = malloc(sizeof(int) + 1);
    sprintf(charID, "%d", docID);

    // construct the pathname for the given file in pageDirectory
    char* pathName = malloc(sizeof(char) * (strlen(pageDirectory) + strlen(charID) + 2)); // plus two for backslash and null terminator 

    // ensure malloc worked
    if (pathName == NULL) {
        fprintf(stderr, "Error creating pathName\n");
        exit(21);
    }

    // adds pageDirectory to newly-created pathName
    strcpy(pathName, pageDirectory);
    strcat(strcat(pathName, "/"), charID); // adds slash to the end of pathName, and the appends charID

    // frees the char* representation of docID
    free(charID);

    FILE* fp = fopen(pathName, "w");
    
    // check the file was opened sucessfully 
    if (fp == NULL) {
        free(pathName);
        fprintf(stderr, "Unable to open file for writing\n");
        exit(22);
    }

    // retrived the URL
    char* url = webpage_getURL(page);
    if (url == NULL) {
        fprintf(stderr, "Error getting webpage from URL\n");
        exit(23);
    }

    fprintf(fp, "%s\n", url);

    // retrive the depth
    int depth = webpage_getDepth(page);

    fprintf(fp, "%d\n", depth);

    // retrive the HTML
    char* html = webpage_getHTML(page);
    if (html == NULL) {
        fprintf(stderr, "Error getting HTML from URL\n");
        exit(24);
    }

    fprintf(fp, "%s\n", html);

    free(pathName);
    fclose(fp);
}


/*
* check if a pageDirectory is crawler-produced e.g. has a file named .crawler
*/
bool 
pagedir_validate(char* pageDirectory)
{
    if (pageDirectory == NULL) {
        return false;
    }

    // crawler extension added to crawler-made directories 
    char* crawlerExtension = "/.crawler";

    // calloc space for pageDirectory + crawlerExtension + null terminator 
    // used calloc to avoid error from using uninitialized variable with the next line (via strcat)
    char* crawlerPath = calloc((strlen(pageDirectory) + strlen(crawlerExtension))  + 1, sizeof(char) );

    strcat(crawlerPath, pageDirectory);
    strcat(crawlerPath, crawlerExtension);

    // strcat adds null terminator 
    // strcat(crawlerPath, '\0');

    FILE* fp = fopen(crawlerPath, "r");

    free(crawlerPath);

    // if file does not exist (not a crawler directory)
    if (fp == NULL) {
        return false;
    }

    // a file named .crawler exists, close it and return true
    fclose(fp);
    return true;
}



/*
* Creates and returns a webpage_t* given a pageDirectory and docID (e.g. file created by crawler)
* Reconstructs the file path, obtains the url, depth, and HTML from the file, creates new webpage_t* 
*/
webpage_t*
pagedir_load(char* pageDirectory, int docID) 
{
    if (pageDirectory == NULL) {
        return NULL;
    }

    char* path = malloc(sizeof(char) * (strlen(pageDirectory) + sizeof(int) + 1 + 1 + 1));
    if (path == NULL) {
        fprintf(stderr, "Error creating path for webpage\n");
        exit(1);
    }

    strcpy(path, pageDirectory);

    char* charID = malloc(sizeof(int) + 1);

    sprintf(charID, "%d", docID);

    strcat(strcat(path, "/"), charID);

    free(charID);

    FILE* fp = fopen(path, "r");

    if (fp == NULL) {
        free(path);
        return NULL;
    }


    // first line is URL
    char* url = file_readLine(fp);

    // second line is depth, convert char* to int
    char* charDepth = file_readLine(fp);
    int depth;
    sscanf(charDepth, "%d", &depth);
    free(charDepth);

    // reads the remainder of the file (HTML is more than 1 line)
    char* html = file_readFile(fp);

    fclose(fp);
    free(path);

    webpage_t* webpage = webpage_new(url, depth, html);

    if (webpage == NULL) {
        fprintf(stderr, "Error creating webpage\n");
        exit(1);
    }
    return webpage;
}
