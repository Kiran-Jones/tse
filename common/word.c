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
    // iterate over word, call tolower() on each character
    // modifies in place, no need to return
    for (int i = 0; word[i] != '\0'; i++) {
        word[i] = tolower(word[i]);
    }
}
