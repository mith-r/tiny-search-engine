/*
 * indexer.c
 * Mithun Rameshkumar    May 2025
 * 
 * Implmements an indexer for CS50 Tiny Search Engine (Lab 5)
 * Functions:
 *  indexBuild - builds an index from files it finds in directory
 *  indexPage  - scans webpage document to add words to index
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "webpage.h"
#include "mem.h"
#include "pagedir.h"
#include "index.h"
#include "word.h"

// Constants
static const int NUM_ARGS = 3;  //number of necessary arguments

//function prototypes
static void parseArgs(const int argc, char* argv[], char** pageDirectory, char** indexFilename);
index_t* indexBuild(char* pageDirectory);
static void indexPage(index_t* index, webpage_t* page, int docID);

/* main()
 *  argc: number of command-line arguments
 *  argv: array of arguments
 *
 *  calls parseArgs and indexBuild
 *  Exits 0 on success; non-zero on error
 */
int main(const int argc, char* argv[])
{
    char* pageDirectory = NULL;
    char* indexFilename = NULL;

    parseArgs(argc, argv, &pageDirectory, &indexFilename);

    index_t* index = indexBuild(pageDirectory);
    if (index == NULL) {
        fprintf(stderr, "Couldn't create index\n");
        exit(EXIT_FAILURE);
    }

    FILE* fp = fopen(indexFilename, "w");
    if (fp == NULL) {
        fprintf(stderr, "Could not write to indexFilename\n");
        index_delete(index);
        exit(EXIT_FAILURE);
    }

    index_print(index, fp);
    fclose(fp);
    index_delete(index);

    return 0;
}

/* parseArgs()
 *  argc: number of arguments
 *  argv: array of arguments
 *  pageDirectory: pointer to pageDirectory string
 *  indexFilename: pointer to index file's location
 *
 * parses and validates command-line arguments
 * 
 */
static void parseArgs(const int argc, char* argv[], char** pageDirectory, char** indexFilename)
{
    // Parameter checks
    if (argc != NUM_ARGS) {
        fprintf(stderr, "Usage: ./indexer pageDirectory indexFilename\n");
        exit(EXIT_FAILURE);
    }

    if (argv[1] == NULL || argv[2] == NULL) {
        fprintf(stderr, "Invalid pageDirectory or IndexFilename\n");
        exit(EXIT_FAILURE);
    }

    //Check for ./crawler in pageDirectory
    const char* suffix = "/.crawler";
    char* pathName = mem_malloc_assert(strlen(argv[1]) + strlen(suffix) + 1,
                                        "Out of memory for pathName");

    strcpy(pathName, argv[1]);
    strcat(pathName, suffix);

    FILE* fp;
    if ((fp = fopen(pathName, "r")) == NULL) {
        fprintf(stderr, "No /.crawler file in pageDirectory\n");
        mem_free(pathName);
        exit(EXIT_FAILURE);
    }

    mem_free(pathName);
    fclose(fp);

    //Check for file existence by looking for '1'
    suffix = "/1";
    pathName = mem_malloc_assert(strlen(argv[1]) + strlen(suffix) + 1,
                                        "Out of memory for pathName");
    
    strcpy(pathName, argv[1]);
    strcat(pathName, suffix);

    if ((fp = fopen(pathName, "r")) == NULL) {
        fprintf(stderr, "No files to read in pageDirectory\n");
        mem_free(pathName);
        exit(EXIT_FAILURE);
    }

    mem_free(pathName);
    fclose(fp);

    //Ensure indexFilename can be written to 
    if ((fp = fopen(argv[2], "w")) == NULL) {
        fprintf(stderr, "indexFilename cannot be written to\n");
        exit(EXIT_FAILURE);
    }
    fclose(fp);

    *pageDirectory = argv[1];
    *indexFilename = argv[2];
}

/* indexBuild()
 *   pageDirectory: Path to pages location
 *
 * Create an index and iterate over files to load as webpages
 * Returns pointer to new index
 * On error, returns NULL
 */
index_t* indexBuild(char* pageDirectory)
{
    //Creates new index object
    index_t* index = index_new();

    //Loop over document ID numbers from 1
    int docID = 1; //Starting ID number
    int docIDSize = 8; //Setting high limit (100 million pages)
    char* pathName = mem_malloc_assert(strlen(pageDirectory) + docIDSize + 1,
                                        "Out of memory for pathName");
    sprintf(pathName, "%s/%d", pageDirectory, docID);

    //Cycle thorugh files and create webpage object
    FILE* fp;
    while ((fp = fopen(pathName, "r")) != NULL) {
        webpage_t* page = pagedir_load(fp);
        if (page == NULL) {
            fprintf(stderr, "Unable to create webpage for index\n");
            mem_free(pathName);
            index_delete(index);
            return NULL;
        }

        indexPage(index, page, docID);

        //Iterate pagedirectory to next number
        docID++;
        sprintf(pathName, "%s/%d", pageDirectory, docID);
        fclose(fp);
    }

    mem_free(pathName);
    return index;
}

/* indexPage()
 *  index - index to add to 
 *  page - webpage to scan
 *  docID - associated docID to add to within Index
 *
 * Scans webpage to add its words to index
 * Calls word_normalize to lowercase word
 * Adds it to index with corresponding docID
 */
static void indexPage(index_t* index, webpage_t* page, int docID)
{
    char* word = NULL;
    int position = 0;

    //Iterate through and get next word of webpage until done
    while((word = webpage_getNextWord(page, &position)) != NULL) {

        //ignore words less than 3 characters
        if (strlen(word) < 3) {
            mem_free(word);
            continue;
        }

        word_normalize(word); //make word lowercase
        index_insert(index, word, docID); //insert into index

        mem_free(word);
    }
    webpage_delete(page);
}


