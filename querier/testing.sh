#!/bin/bash
# Mithun Rameshkumar May 2025
# This bash script runs test cases for querier.c
# Assumes ../crawler/testing.sh and ../indexer/testing.sh have been run

# no arguments
./querier

# one argument
./querier ../data/letters

# three arguments
./querier ../data/letters ../data/indexes/letters extraArgument

# Invalid pageDirectory
./querier ../data/invalid ../data/indexes/noPath

# No .crawler file
./querier ../data/noCrawlerFile ../data/indexes/noPath

#No files to read
./querier ../data/letters ../data/indexes/noPath

#Invalid indexFile
./querier ../data/letters0 ../data/indexes/noPath

#valid execution with mixed validity queries
./querier ../data/wikipedia1 ../data/indexes/wikipedia1 < wikipedia1.txt

#valid execution with valid queries in valgrind
valgrind ./querier ../data/toscrape1 ../data/indexes/toscrape1 < toscrape1.txt


