# crawler
This directory contains the **crawler** subsystem for Lab 4 of the CS50 Tiny Search Engine.

## Overview

`crawler` traverses an internal CS50 TSE website starting from a seed URL, up to a specified maximum depth, and saves each fetched page into a page directory.

## Files

- **`crawler.c`** – implements  
  - `parseArgs(...)`  
  - `crawl(...)`  
  - `pageScan(...)`  
- **`Makefile`** – builds and links against `common.a` and `libcs50-given.a`  
- **`testing.sh`** – functional tests for error cases and valid crawls  
- **`.gitignore`** – ignores object files, executable, and test outputs

## Build

```bash
cd crawler
make          # builds `crawler`
```

## Usage
```./crawler SEED_URL PAGE_DIRECTORY MAX_DEPTH```

## Assumptions

No assumptions beyond those specified by instructions