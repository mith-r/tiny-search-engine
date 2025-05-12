# common
This directory provides the **pagedir** module shared by the crawler subsystem.

## Module

- **pagedir.h / pagedir.c**  
  - `bool pagedir_init(const char *pageDirectory)`  
    Creates an empty file named `.crawler` inside `pageDirectory` to mark it as crawler output. Returns `true` on success, `false` on failure.  
  - `void pagedir_save(const webpage_t *page, const char *pageDirectory, int docID)`  
    Writes a fetched page to `<pageDirectory>/<docID>` with this format:  
    1. URL  
    2. depth  
    3. raw HTML
- **index.h / index.c**  
  - `index_t* index_new()`  
    Create a new, empty in-memory index. Returns a pointer to the new `index_t` on success or `NULL` on error.  
  - `bool index_insert(index_t* index, const char* word, const int docID)`  
    Add one occurrence of `word` in document `docID` to the index. Caller must supply a valid index pointer, a non-NULL string, and a positive `docID`. Returns `true` on success, `false` on failure.  
  - `counters_t* index_find(index_t* index, const char* word)`  
    Look up `word` in the index and return its associated `counters_t*` (mapping docIDs to counts), or `NULL` if not found or on error.  
  - `void index_print(index_t* index, FILE* fp)`  
    Write the entire index to the open file `fp` in text format, one line per word:  
      1. the word itself  
      2. followed by pairs of `docID count`, each prefixed by a space  
  - `index_t* index_save(FILE* fp)`  
    Read an index from the open file `fp`, expecting the same format produced by `index_print`. Returns a newly allocated `index_t*` on success (caller must later call `index_delete()`), or `NULL` on error.  
  - `void index_delete(index_t* index)`  
    Free all memory associated with `index`, including its internal hashtable and counters. If `index` is `NULL`, does nothing.  
- **word.h / word.c**  
  - `void word_normalize(char* string)`  
    Normalize the given string in place by converting each character to lowercase.  


## Assumptions
- No assumptions beyond those specified by instructions.

## Build

```bash
make       # produces common.a
make clean # removes common.a and object files
```