/* 
 * querier.c
 * Mithun Rameshkumar    May 2025
 * 
 * Implements a querier for CS50 Tiny Search Engine (Lab 5)
 * Reads the index file produced by the TSE Indexer and page files produced by
 * the TSE Querier and answers queries submitted via stdin
 * 
 * Functions:
 *  parseArgs - validate command-line arguments
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <unistd.h>  

#include "hashtable.h"
#include "mem.h"
#include "pagedir.h"
#include "index.h"
#include "word.h"
#include "counters.h"

// Constants
static const int NUM_ARGS = 3;
static const int MAX_LINE = 1024;

//function prototypes
int fileno(FILE *stream);
static void prompt(void);
static void getQueries(index_t* index, char* pageDirectory);
static void getMax(void* arg, int key, int count);
static void counterSize(void* arg, const int key, const int count);
static counters_t* identifyDocuments(char** tokenArray, int numTokens, index_t* index);
static void andIterate(void* arg, int key, int count);
static int getMin(int totalCount, int conjCount);
static void copyIterate(void* arg, int key, int count);
static void orIterate(void* arg, int key, int count);
static char** tokenize(char* string, int* numTokens);
static void parseArgs(const int argc, char* argv[], char** pageDirectory, char** indexFilename);


/* main()
 *  argc: number of command-line arguments
 *  argv: array of arguments
 * 
 * Calls parseArgs, loads index from indexFileName
 * and reads query from stdin until EOF
 * 
 * Exits 0 on success; non-zero on error
*/
int main(const int argc, char* argv[])
{
    char* pageDirectory = NULL;
    char* indexFilename = NULL;

    parseArgs(argc, argv, &pageDirectory, &indexFilename);

    FILE *fp = fopen(indexFilename, "r");
    if(fp == NULL) {
        fprintf(stderr, "indexFile cannot be read\n");
        exit(EXIT_FAILURE);
    }

    //save index
    index_t* index = index_save(fp);
    fclose(fp);

    if (index == NULL) {
        fprintf(stderr, "Couldn't create index from indexFIlename\n");
        exit(EXIT_FAILURE);
    }

    //Clean up
    getQueries(index, pageDirectory);
    index_delete(index);
    exit(EXIT_SUCCESS);
}

/* getQueries()
 *  index - pointer to populated index
 *  pageDirectory - the corresponding pageDirectory
 * 
 * Continuously prompts user for Queries
 *
 */
static void getQueries(index_t* index, char* pageDirectory)
{
    char line[MAX_LINE];

    prompt();
    while (fgets(line, MAX_LINE, stdin) != NULL) { //continuously get user query
        int numTokens = 0;
        char** tokenArray = tokenize(line, &numTokens); //convert string to array of tokens
        if (tokenArray == NULL) {
            prompt();
            continue;
        }

        //print cleaned query
        printf("Query: ");
        for (int i = 0; i < numTokens; i ++) {
            printf(" %s", tokenArray[i]);
        }
        printf("\n");

        //Using index to identify documents
        counters_t* scores = identifyDocuments(tokenArray, numTokens, index);

        //Iterate through counter to find size
        int count = 0;
        counters_iterate(scores, &count, counterSize);

        //If matches, print count line
        if (count > 0) {
            printf("Matches %d documents (ranked):\n", count);
        } else {
            printf("No documents match.\n");
        }

        //Loop to rank results in descending order
        int maxInt = -1;
        while(maxInt != 0) {
            maxInt = 0;
            int maxKey = 0;
            int* intArray[2];

            intArray[0] = &maxInt;
            intArray[1] = &maxKey;

            //Finds max score and inputs key and score into maxInt/maxKey
            counters_iterate(scores, intArray, getMax);

            //if max score greater than 0, find URL of document and print result

            if (maxInt > 0) {

                //Create string to hold file path
                int docIDsize = 8; //(1 million pages)
                char* pathName = mem_malloc_assert(strlen(pageDirectory) + docIDsize, "Out of memory");

                strcpy(pathName, pageDirectory);
                sprintf(pathName, "%s/%d", pageDirectory, maxKey);

                FILE* fp;
                if ((fp = fopen(pathName, "r")) == NULL) {
                    fprintf(stderr, "Could not read document\n");
                    exit(EXIT_FAILURE);
                }
                char* url = file_readLine(fp);
                printf("score %3d  doc %2d: %s\n", maxInt, maxKey, url);

                //clean up;
                mem_free(pathName);
                mem_free(url);
                fclose(fp);

                //Set found document's count to 0 so no duplicates
                if (!counters_set(scores, maxKey, 0)) {
                    fprintf(stderr, "Out of memory\n");
                    exit(EXIT_FAILURE);
                }
            }
        }
        mem_free(tokenArray);
        counters_delete(scores);
        prompt();
    }
}

/* getMax()
 *  arg - array with two pointers: maxInt to store highest count and maxKey to store key of maxCount
 *  key - an integer representing current docID
 *  count - the score associated with the key
 * 
 * Finds the maximum score and its associated docID within a counters struct.
 * It is passed to counters_iterate and updates the maxInt and maxKey in array
 * with the highest scoring document.
 * 
 * Called by counters_iterate in getQueries();
 * 
 */
static void getMax(void* arg, int key, int count)
{
    int** intArray = (int**) arg;
    int* maxInt = intArray[0];
    int* maxKey = intArray[1];

    if (count > *maxInt) {
        *maxInt = count;
        *maxKey = key;
    }
}

/* counterSize()
 *  arg - pointer to int holding score
 *  key - integer representing current docID
 *  count - score associated with each key
 *  
 * Counts number of non-zero counts within scores counter
 * 
 * Called by counters_iterate within getQueries()
 * Exits on unrecoverable error
 */
static void counterSize(void* arg, const int key, const int count)
{
    if (count > 0) {
        (*(int*) arg) ++;
    }
}

/* identifyDocuments()
 *  tokenArray - array of Strings (tokens)
 *  numTokens - number of strings(tokens) in tokenArray
 *  index - a pointer to an index of word frequences
 *
 * Goes through each token in tokenArray and finds documents that
 * match the query. Scores each document based on 'or' and 'and operators
 * 
 * Returns a counter with docIDs mapped to total scores
 * 
 * Assumes non-empty array, as should be prechecked by caller
 */
static counters_t* identifyDocuments(char** tokenArray, int numTokens, index_t* index)
{
    //Create counters to track scores
    counters_t* total = NULL; //Holds 'or' unions of completed 'and' segments
    counters_t* conj = NULL; //holds running 'and' segments

    //Loop through tokens and keep a running score
    for (int i = 0; i < numTokens; i++) {
        char* word = tokenArray[i];

        //Skip 'and's (implicit)
        if (strcmp(word, "and") == 0) {
            continue;
        }

        //Find intersection of counters if 'or'
        if (strcmp(word, "or") == 0) {
            if (total == NULL) { //If first merge, simply transfer pointer directly to total
                total = conj;
                conj = NULL;
            }
            else { //Else merge the two by running orIterate
                counters_iterate(conj,total, orIterate);
                counters_delete(conj);
                conj = NULL;
            }
            continue; //Go to next iteration. Did this so it wasn't too many indentations
        } 

        //Everything else is considered an 'and'
        if (index_find(index, tokenArray[i])) { // If word found in index
            if (conj == NULL) { //If first word in 'conj', can be copied directly
                conj = counters_new();
                counters_iterate(index_find(index, tokenArray[i]), conj, copyIterate); 
            } else { //Else merge the two by running andIterate
                counters_t* counterArray[2]; //Bundling so I can iterate and modify

                counterArray[0] = index_find(index,word);
                counterArray[1] = conj;

                counters_iterate(conj, counterArray, andIterate);

            }
        } else if (conj != NULL) { //if word is not found, then delete the counter
            counters_delete(conj);
            conj = NULL;
        }
    }

    if (total == NULL) {
        total = conj;
        conj = NULL;
    } else if (conj != NULL) {
        counters_iterate(conj, total, orIterate);
        counters_delete(conj);
        conj = NULL;
    }

    return total;

}

/* andIterate
 *  arg - pointer to both conj and newCounter
 *  key - a docID in conj
 *  count - the number of occurences for each key in conj
 * 
 * Iterates through conj counter. For each key, checks if that key is in newCounter. If it is, takes the minimum
 * of the value and sets the total to that. If not present, set counter to 0. 
 *
 * Called by identifyDocuments
 * Exits on unrecoverable error
 * 
 */
static void andIterate(void* arg, int key, int count)
{
    counters_t** counterArray = (counters_t**)arg;
    counters_t* newCounter = counterArray[0];
    counters_t* conj = counterArray[1];
    
    if (!counters_set(conj, key, getMin(count, counters_get(newCounter, key)))){  //Sets to 0 if not found in newCounter.
        fprintf(stderr, "Error setting counter\n");
        exit(EXIT_FAILURE);
    }
}

/* getMinimum()
 *  totalCount - count within total Counter
 *  conjCount - count within conj Counter
 * 
 * Finds and returns the minimum count between two values
 * Called by andIterate
 */
static int getMin(int totalCount, int conjCount) 
{
    if (totalCount >= conjCount) {
        return conjCount;
    } else {
        return totalCount;
    }
}

/* copyIterate
 *  arg - pointer to conj
 *  key - a docID within a counter from the index
 *  count - the number of occurences for each key in the index's counter
 *  
 *
 * Copies counter from within index 
 * Called by identifyDocuments
 * Exits on unrecoverable error
 *
 */
static void copyIterate(void* arg, int key, int count) 
{
    counters_t* conj = (counters_t*)arg;
    if ((counters_set(conj, key, count)) == false) {
        fprintf(stderr, "Error setting counter\n");
        exit(EXIT_FAILURE);
    }
}


/* orIterate()
 *  arg - pointer to total
 *  key - a docID in conj
 *  count - the number of occurences for each key in conj
 * 
 * Iterates through conj counter. For each key, checks if that key is in total. If it is,
 * adds value to counter. If not, creates counter within total. 
 * 
 * Used by counters_iterate in orMergeCounters to perform 'or'
 * Exits on unrecoverable error
 */
static void orIterate(void* arg, int key, int count)
{
    counters_t* total = (counters_t*)arg;

    int totalCount = counters_get(total,key) + count; //counter_get returns 0 if doesn't exist

    if ((counters_set(total,key, totalCount)) == false) {
        fprintf(stderr, "Error setting counter\n");
        exit(EXIT_FAILURE);
    }

}

/* tokenize()
 *  query - string containing user's original query
 *  numTokens - pointer to integer to store number of tokens
 * 
 * Turns user's query into array of tokens split by whitespace. 
 * Makes all words lowercase and ensures its a valid query
 * 
 * Returns an array of string with cleaned tokens
 * NULL if invalid query
 * 
 */
static char** tokenize(char* string, int* numTokens)
{
    int whitespace = 0; //Tracking number of spaces to get estimate for array size
    for (int i = 0; i < strlen(string); i++) { //Ensure no improper characters
        if (isspace(string[i])) {
            whitespace ++;
            continue;
        }

        if (!(isalpha(string[i]))) {
            printf("Error: bad character '%c' in query.\n", string[i]);
            return NULL;
        }
    }

    //Creating array of strings
    int arraySize = whitespace + 1; 
    char** tokenArray = mem_malloc_assert(arraySize * sizeof(char*), "Out of memory for stringArray");

    int tokenCount = 0;

    //Split into tokens based on white space
    char* delimiter = " \t\n\v\f\r";
    char* token = strtok(string, delimiter);
    if (token == NULL) {
        mem_free(tokenArray);
        return NULL;
    }

    word_normalize(token); //make word lowercase

    //Making sure first word isn't 'and' or 'or'
    if (strcmp(token, "and") == 0) {
        printf("Error: 'and' cannot be first\n");
        mem_free(tokenArray);
        return NULL;
    }

    if (strcmp(token, "or") == 0) {
        printf("Error: 'or' cannot be first\n");
        mem_free(tokenArray);
        return NULL;
    }

    //Adding word to array
    tokenArray[tokenCount] = token;
    tokenCount++;

    //Flags to detect sequential 'and' or 'or' tokens
    bool previousAnd = false;
    bool previousOr = false;

    while ((token = strtok(NULL, delimiter)) != NULL) {
        word_normalize(token); //make word lowercase
        
        //Check for sequential 'and' or 'or' tokens
        if(previousAnd) { 
            if (strcmp(token, "and") == 0) {
                printf("Error: 'and' and 'and' cannot be adjacent\n");
                mem_free(tokenArray);
                return NULL;
            }

            if (strcmp(token, "or") == 0) {
                printf("Error: 'and' and 'or' cannot be adjacent\n");
                mem_free(tokenArray);
                return NULL;
            }

            previousAnd = false;
        }

        if (previousOr) {
            if (strcmp(token, "and") == 0) {
                printf("Error, 'or' and 'and' cannot be adjacent\n");
                return NULL;
            }

            if (strcmp(token, "or") == 0) {
                printf("Error, 'or' and 'or' cannot be adjacent\n");
                mem_free(tokenArray);
                return NULL;
            }
            previousOr = false;
        }

        if (strcmp(token, "and") == 0) {
            previousAnd = true;
        }

        if (strcmp(token, "or") == 0) {
            previousOr = true;
        }

        //Adding to Array
        tokenArray[tokenCount] = token;
        tokenCount++;
    }

    //Make sure last token isn't 'or' or 'and'
    if (previousAnd) {
        printf("Error: 'and' cannot be last\n");
        mem_free(tokenArray);
        return NULL;
    }
    
    if (previousOr) {
        printf("Error: 'or' cannot be last\n");
        mem_free(tokenArray);
        return NULL;
    }
    //return number and array of tokens
    *numTokens = tokenCount; 
    return tokenArray;
}

/* parseArgs()
 *  argc: number of arguments
 *  argv: array of arguments
 *  pageDirectory: pointer to pageDirectory string
 *  indexFilename: pointer to index file's location
 * 
 * parses and validates command-line arguments
 */
static void parseArgs(const int argc, char* argv[], char** pageDirectory, char** indexFilename)
{
    // Parameter checks
    if (argc != NUM_ARGS) {
        fprintf(stderr, "Usage: ./querier pageDirectory indexFilename\n");
        exit(EXIT_FAILURE);
    }

    if (argv[1] == NULL || argv[2] == NULL) {
        fprintf(stderr, "Invalid pageDirectory or indexFilename\n");
        exit(EXIT_FAILURE);
    }

    // Check for ./crawler in pageDirectory
    const char* suffix = "/.crawler";
    char* pathName = mem_malloc_assert(strlen(argv[1]) + strlen(suffix) + 1,
                                        "Out of memory for pathName");
    strcpy(pathName, argv[1]);
    strcat(pathName, suffix);

    FILE* fp;
    if ((fp = fopen(pathName, "r")) == NULL) {
        fprintf(stderr, "No /.crawler file in pageDirectory or invalid pageDirectory\n");
        mem_free(pathName);
        exit(EXIT_FAILURE);
    }

    mem_free(pathName);
    fclose(fp);

    //Check for file existence by lookin for '1'
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

    //Ensure indexFilename exists
    if ((fp = fopen(argv[2], "r")) == NULL) {
        fprintf(stderr, "indexFilename cannot be read\n");
        exit(EXIT_FAILURE);
    }
    fclose(fp);

    *pageDirectory = argv[1];
    *indexFilename = argv[2];
}

/* prompt()
 * 
 * Only prompts user when entering information from a keyboard
 * Makes output files look nicer when testing
 *
 */
static void prompt(void)
{
  // print a prompt iff stdin is a tty (terminal)
  if (isatty(fileno(stdin))) {
    printf("Query? ");
  }
}