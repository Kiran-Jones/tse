/*
* word.c 
* 
* 
* Kiran Jones, CS50 25W
* 
*/

#include <stdlib.h>
#include <string.h>
#include <ctype.h>


void
normalizeWord(char* word) 
{
    for (int i = 0; word[i] != '\0'; i++) {
        word[i] = tolower(word[i]);
    }
}
