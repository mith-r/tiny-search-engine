/*
 * word.h - header file for CS50 word module
 *
 * provides function to normalize string
 * 
 * Mithun Rameshkumar May 2025
 * See word.c for implementation details
 */

 #ifndef __WORD_H
 #define __WORD_H

 #include <ctype.h>
 #include <string.h>

 /* word_normalize()
  *
  * normalizes given string by converting to lowercase
  */
void word_normalize(char* string);

#endif // __WORD_H