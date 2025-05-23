# CS50 TSE Querier
## Implementation Spec
## Mithun Rameshkumar May 2025

This document draws from the [Requirements Specification](REQUIREMENTS.md) and [Design Specification](DESIGN.md), detailing the key implementation decisions for the *querier* component of the CS50 Tiny Search Engine.

The *querier* interprets user search queries against an index using AND and OR logic, computes relevance scores for each matching document, and returns a ranked list of documents complete with their scores and URLs.

## Data Structures

The key data structures in the Querier are:
- *index*: associates each word with a counter of document IDs and their occurrence counts, reconstructed by reading the index file.
- *counters*: maintain document IDs along with their cumulative relevance scores, which are updated as each query token is processed.

## Control Flow

The *querier* in querier.c uses parseArgs, index_save, getQueries, tokenize, and identifyDocuments to process queries and rank pages.

### main

1. Calls `parseArgs` to validate inputs.
2. Opens indexFilename and builds index via `index_save`
3. Invokes `getQueries` to enter the query loop.
4. Cleans up with `index_delete` and exits.

### parseArgs

1. Verifies exactly two arguments: pageDirectory and indexFilename
2. Checks for a .crawler file and a 1 file to ensure valid crawler output
3. Confirms indexFilename is readable
4. On any failure, prints an error to stderr and exits nonzero

### getQueries

1.	Prints "Query?"  when stdin is a TTY.
2.	Loops with `fgets` until EOF.
3.	Calls `tokenize` to lowercase, split, and validate; on NULL, re-prompts.
4.	Prints "Query: …" with cleaned tokens.
5.	Calls `identifyDocuments` to get a `counters_t*` of scores.
6.	Uses `counters_iterate` to count matches; prints match summary.
7.	In a while loop, finds top score with `getMax`, opens <pageDir>/<docID>, reads URL, prints score … doc …: …, then zeros that score.
8.	Frees `tokenArray` and `scores`, prints separator, then re-prompts.

### tokenize

1. Scans string for invalid chars (non-isalpha/isspace); on error prints and returns NULL.
2. Counts whitespace to size `tokenArray`, then splits on whitespace with `strtok`.
3. calls `word_normalize` to lowercase each token.
4. Rejects if first/last token is “and”/“or” or if two operators are adjacent, printing specific error messages.
5. On success, sets `*numTokens` and returns the array of tokens.

### identifyDocuments
1.	Initializes conj = NULL (AND accum) and total = NULL (OR accum).
2.	For each token in order:
	1. Skip "and".
	2. On "or":
	-	If total is NULL, assign total = conj; else merge conj into total via counters_iterate(conj, total, orIterate); then conj = NULL.
	-	On a word:
	    - If index_find(index, word) exists and conj == NULL, create conj and copy via copyIterate;
	    - Else if it exists, intersect with conj via            counters_iterate(conj, (void*[2]){ index_find(index, word), conj }, andIterate);
	    - If missing, counters_delete(conj); conj = NULL;.
	3.	After tokens:
	    - If total == NULL, return conj; otherwise merge any remaining conj into total with orIterate, delete conj, and return total.

### Counters Helpers
- counterSize: increments a counter for each nonzero count.
- getMax: tracks the highest count and its key to print top scores.
- copyIterate: copies each (key, count) into a fresh counters_t.
- getMin: returns the smaller of two counts.

AND/OR Merge Helpers
- andIterate: for each key in the running AND conj, sets its count to min(old, new) or zero if absent.
- orIterate: for each key in conj, adds its count to total, inserting it if new.

## Other Modules

### libcs50

Querier relies on several *libcs50* modules:

- *counters*: to store and update document scores.
- *hashtable*: used by the index for word lookups.
- *file*: for reading page directory documents.

### index

The index module provides fast lookups of word-to-document mappings.
- `index_save` reads the on-disk index file and builds an in-memory `index_t` structure.
- `index_find` returns the `counters_t` of document IDs and frequencies for a specified word

### word
Provides one function
- `word_normalize`, which lowercases a token in place

## Function protypes

### querier

Detailed descriptions are in `querier.c`

``` c
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
```

## Error Handling and Recovery
- command-line arguments are parsed and validated in advance. Erros result in stderr message and non-zero exit
- mem_malloc_assert manages memory errors
- Errors that cannot be recovered from also results in error message and non-zero exit

### Testing plan

The *testing.sh* script includes tests for the *querier* by running it with various invalid and valid queries.

1. Run *querier* with various invalid arguments.
    1. No arguments
    2. One argument
    3. Three arguments
    4. Invalid pageDirectory
    5. No .crawler file
    6. No files to read
    7. Invalid indexFile
2. Run *querier* with a valid argument and enter both valid and invalid queries to check for correct output.
3. Run *valgrind* on *querier* to check for memory leaks. 

