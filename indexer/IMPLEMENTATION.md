# CS50 TSE Indexer
## Implementation Spec

This document draws from the [Requirements Specification](REQUIREMENTS.md) and [Design Specification](DESIGN.md), detailing the key implementation decisions for the *indexer* component of the CS50 Tiny Search Engine.

The *indexer* builds an index data structure from a directory of crawled pages. It enables efficient lookups of words across documents by mapping each word to its document occurrences and respective counts. 

## Data Structures

An *index* uses a *hashtable* to store words and maps them to *counters*, which tracks the documents via docID numbers and counts the number of occurrences within each document

- *hashtable*: maps words (strings) to counters.
- *counters*: maps docIDs (integers) to word counts

Each hashtable has 500 slots

## Control Flow
The *indexer* is implemented in one file `indexer.c` with four functions:

### main

The `main` function simply calls `parseArgs` and  `indexBuild`, then prints the created index into the specified file and exits 0.

### parseArgs

This function validates command-line arguments for `pageDirectory` and `indexFilename`. 

- It checks that there are 2 arguments
- It makes sure that `pageDirectory`: contains a `.crawler` file, and a '1' file.
- It makes sure `indexFilename`: is writable.
- On any error, prints a message to `stderr` and exits non-zero.

### indexBuild

This function constructs the index by iterating over documents in `pageDirectory`. For each document, it calls `pagedir_load` to create a webpage from the document and `indexPage` to pass words and counters to index.

Pseudocode:
```
Create a new index structure.
Loop through documents in `pageDirectory`:
  - Load each document as a webpage using `pagedir_load`.
  - Pass the webpage to `indexPage`.
Return the created index structure.
```

### indexPage

This function extracts all of a page's words and passes that into index, incrementing the count with respect to the document ID every time.

Pseudocode:
```
Loop to extract each word from the webpage:
  - Convert the word to lowercase.
  - If valid (length ≥ 3), insert it into the index using `index_insert`.
Free the webpage after processing.
```

## Other Modules

### pagedir

The `pagedir.c` module, already used by *crawler*, is used here to load pages into `webpage_t` objects for indexing with `pagedir_load`.

Pseudocode for `pagedir_load`:

```
Read first two lines into URL and depth variables respectively with `file_readLine`.
Read the rest of the file `file_readFile` into the html variable.
Create a new webpage struct with `webpage_new` using variables mentioned above.
```

### libcs50

We use the *libcs50* modules: `webpage` to the info in crawler documents, `counters` to store document IDs and counts, `hashtable` to map words to counters, and `file` to perform file operations.

## Function Prototypes

### indexer

Each function's detailed description is in `indexer.c`:

``` c
int main(const int argc, char* argv[]);
static void parseArgs(const int argc, char* argv[], char** pageDirectory, char** indexFilename);
index_t* indexBuild(char* pageDirectory);
static void indexPage(index_t* index, webpage_t* page, int docID);
```

### pagedir

Each function's detailed description is in `pagedir.h`:

``` c
bool pagedir_init(const char* pageDirectory);
void pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID);
webpage_t* pagedir_load(FILE* fp);
```

### index

Each function's detailed description is in `index.h`:

``` c
index_t* index_new();
bool index_insert(index_t* index, const char* word, const int docID);
int index_find(index_t* index, const char* word, const int docID);
index_t* index_save(FILE* fp);
void index_print(index_t* index, FILE* fp);
void index_delete(index_t* index);
```

## Error handling and recovery
- command-line arguments are parsed and validated in advance. Errors result in stderr message and non-zero exit
- mem_malloc_assert manages memory erros
- Errors that cannot be recovered from also results in error message and non-zero exit

## Testing plan

`indextest` reads an index file into the internal index data structure then writes the index to a new index file, checking that 
`index_save` and `index_print` are working properly

`testing.sh` runs `indexer` with multiple valid and invalid checks and also crosschecks the result with `indextest` to ensure everything
is working as it should be. 

