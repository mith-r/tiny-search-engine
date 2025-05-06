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

## Assumptions
- No assumptions beyond those specified by instructions.

## Build

```bash
make       # produces common.a
make clean # removes common.a and object files
```