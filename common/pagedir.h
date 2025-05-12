/*
 * pagedir.h
 * Mithun Rameshkumar    May 2025
 * 
 * Provides functions to:
 *  - validate and intialize page direcotr
 *  - save webpages to files in that directory
 * 
 * See pagedir.c for implementation details
 *
 */

#ifndef __PAGEDIR_H
#define __PAGEDIR_H

#include <stdbool.h>
#include "webpage.h"

/* pagedir_init()
 *  pageDirectory: path to existing directory for crawler to use
 * 
 * Create a file named ".crawler" inside pageDirectory to mark as
 * produced by crawler.
 * 
 * Returns true if directory exists and marker file was created;
 * Else returns false
 */
bool pagedir_init(const char* pageDirectory);

/* pagedir_save()
 *  page: webpage object
 *  pageDirectory: path to the directory where crawler stores pages
 *  docID: unique integer ID for specific page (file's name)
 * 
 * Saves page to file "pageDirectory/docID"
 */
void pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID);

/* pagedir_load()
 *  fp: pointer to page file containing webpage data
 *
 * Returns pointer to new webpage data structure if successful
 * Else returns NULL
 */
webpage_t* pagedir_load(FILE* fp);

#endif // __PAGEDIR_H