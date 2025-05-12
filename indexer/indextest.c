/*
 * indextest.c - test file for CS50 index module
 *
 * Checks if index_save() works accurately by creating index from file
 * by printing back to another file
 * 
 * Mithun Rameshkumar
 */

#include "webpage.h"
#include "mem.h"
#include "index.h"
#include "pagedir.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

/* main()
 *  argc - number of arguments
 *  argv - array of arguments
 *
 * Validates command line arguments and opens index file
 * Calls index_save() to load onto structure and then copies to another index file
 */
int main(const int argc, char* argv[])
{
    //Parameter check
    if (argc != 3) {
        fprintf(stderr, "Usage: ./indextest oldIndexFileName newIndexFileName\n");
        exit(EXIT_FAILURE);
    }

    //Open old file
    FILE* fp;
    if ((fp = fopen(argv[1], "r")) == NULL) {
        fprintf(stderr, "Couldn't open oldFile\n");
        exit(EXIT_FAILURE);
    }

    //Read file onto index
    index_t* index = index_save(fp);
    if (index == NULL) {
        fprintf(stderr, "Error creating index\n");
        exit(EXIT_FAILURE);
    }
    fclose(fp);

    //Write onto new file
    if ((fp = fopen(argv[2],"w")) == NULL) {
        fprintf(stderr, "Couldn't write to newFile");
        exit(EXIT_FAILURE);
    }

    index_print(index,fp);
    fclose(fp);
    index_delete(index);
    return 0;
}
