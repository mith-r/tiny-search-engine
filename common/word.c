/*
 * word.c CS50 TSE Lab 5
 *
 * Mithun Rameshkumar May 2025
 * see word.h for usage details
 */

#include <ctype.h>
#include <string.h>

/* word_normalize()
 *
 * See word.h for more info
 */
void word_normalize(char* string)
{
    // NULL check
    if (string == NULL) {
        return;
    }

    //Loop through and lowercase each character
    for (int i = 0; i < strlen(string); i ++) {
        string[i] = tolower((unsigned char) string[i]); 
    }
}