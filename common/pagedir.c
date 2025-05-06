/*
 * pagedir.c
 * Mithun Rameshkumar     May 2025
 * 
 * Implements page directory module for CS50 Tiny Search Engine (Lab 4).
 * Functions:
 *  pagedir_init - mark a directory for crawler's use
 *  pagedir_save - save webpage into that directory
 * 
 * see pagedir.h for usage
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "pagedir.h"
#include "mem.h"
#include "file.h"

/* pagedir_init()
 * 
 * Pseudocode:
 *  construct the pathname for the .crawler file in that directory
 *  open the file for writing; on error, return false.
 *  close the file and return true.
 * 
 * See pagedir.h for more information
 */
bool pagedir_init(const char* pageDirectory) 
{
    // Null check
    if (pageDirectory == NULL) {
        return false;
    }

    // Allocate space for full directory
    const char* suffix = "/.crawler";
    char* pathName = mem_malloc_assert(strlen(pageDirectory) + strlen(suffix) + 1,
                                        "Out of memory for pathName");

    strcpy(pathName, pageDirectory);
    strcat(pathName, suffix);

    //Check if file can be created at directory
    FILE* fp;
    if ((fp = fopen(pathName, "w")) == NULL) {
        mem_free(pathName);
        return false;
    }

    mem_free(pathName);
    fclose(fp);
    return true;
}

/* pagedir_save()
 * 
 * Pseudocode:
 *  construct the pathname for the page file in pageDirectory
 *  open that file for writing
 *  print the URL
 *  print the depth
 *  print the contents of the webpage
 *  close the file
 * 
 * See pagedir.h for more information
 */
void pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID)
{
    //Parameter Checks
    if (page == NULL || pageDirectory == NULL) {
        fprintf(stderr, "Make sure pagedir_save's parameters are not NULL");
        exit(EXIT_FAILURE);
    }

    if (docID < 0) {
        fprintf(stderr, "ID: '%d' should be non-negative", docID);
        exit(EXIT_FAILURE);
    }

    //Allocate space for full directory + docID
    int docIDSize = 1;
    if (docID > 0) {
        docIDSize = floor(log10(docID)) + 1;
    }

    char* pathName = mem_malloc_assert((strlen(pageDirectory) + docIDSize) + 2,
                                     "Out of memory for pathName ");                          
    char* fileName = mem_malloc_assert(((docIDSize) + 1), "Out of memory for fileName");

    sprintf(fileName, "%d", docID);

    //Creating directory
    strcpy(pathName, pageDirectory);
    strcat(pathName, "/");
    strcat(pathName, fileName);

    mem_free(fileName);

    FILE* fp;

    if ((fp = fopen(pathName, "w")) == NULL) {
        mem_free(pathName);
        fprintf(stderr, "Directory Error");
        exit(1);
    }

    mem_free(pathName);
    fprintf(fp, "%s\n", webpage_getURL(page));
    fprintf(fp, "%d\n", webpage_getDepth(page));
    fprintf(fp, "%s\n", webpage_getHTML(page));
    fclose(fp);
}

