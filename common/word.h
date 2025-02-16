/* 
 * word.h - header file for CS50 word module
 *
 *
 * Kiran Jones, CS50 25W
 */


/*
* "normalizes" a given word e.g. converts a word to lowercase
* takes a char* word and modifies it in place, hence the function does not return anything
* sample usage:
* char* word = "HeLlO";
* printf("initial word: %s\n", word); 
    prints "HeLlO"
* normalizeWord(word);
* printf("normalized word: %s\n", word); 
    prints "hello"
* 
*/ 
void normalizeWord(char* word);

