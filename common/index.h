/*
 * index.h - header file for CS50 index module
 *
 * implements an abstract index_t type that represents an index in memory
 * maps words to counters and tracks number of occurrences of word
 * in each document
 * 
 * Mithun Rameshkumar May 2025
 * See index.c for implementation details
 */

#ifndef __INDEX_H
#define __INDEX_H

#include "hashtable.h"
#include "counters.h"
#include "mem.h"
#include "bag.h"
#include "file.h"
#include <stdio.h>
#include <stdbool.h>

// Global types
typedef struct index index_t;

//functions

/* index_new()
 * Create a new index structure
 *
 * Returns a pointer to the new index and NULL on error
 */ 
index_t* index_new();

/* index_insert()
 *
 * Insert a word and docID into the index
 * 
 * Caller provides:
 *  pointer to index
 *  string for word
 *  positive integer for docID
 * 
 * Returns true if success, false otherwise
 */
bool index_insert(index_t* index, const char* word, const int docID);

/* index_find()
 *
 * Retrieve counter for number of occurrences of word in document
 * 
 * Caller provides:
 *  pointer to index
 *  string for word
 * 
 * Returns counter if found, else returns NULL
 */
counters_t* index_find(index_t* index, const char* word);

/* index_print()
 *
 * Print index to file in format:
 *  word followed by docID and count pairs separated by spaces
 * 
 * caller provides:
 *  pointer to index 
 *  pointer to open file 
 */
void index_print(index_t* index, FILE* fp);

/* index_save()
 *
 * Load an index from a file if it has proper format
 * 
 * Caller provides:
 *  file pointer to open file 
 * 
 * Returns pointer to new index, else returns NULL
 * Must be freed later with index_delete()
 */
index_t* index_save(FILE* fp);

/* index_delete()
 *
 * Delete index and free its memory
 * 
 * Caller provides:
 *  pointer to index
 */
void index_delete(index_t* index);


#endif // __INDEX_H