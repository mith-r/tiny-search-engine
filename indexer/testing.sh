#!/bin/bash
# Mithun Rameshkumar    May 2025
# This bash script runs test cases for crawler.c
#assumes ../crawler/testing.sh has been run

#Make directories
mkdir ../data/indexes
mkdir ../data/noCrawlerFile
touch ../data/noCrawlerFile/1

mkdir ../data/readonlydirectory
touch ../data/indexes/readonlyindexfile
chmod 555 ../data/readonlydirectory
chmod 555 ../data/indexes/readonlyindexfile

# no arguments
./indexer

# one argument
./indexer ../data/letters

# three arguments
./indexer ../data/letters ../data/indexes/letters extraArgument

# Invalid pageDirectory (nonExistentPath)
./indexer ../data/invalid ../data/indexes/noPath

# Invalid pageDirectory (not a crawler directory)
./indexer ../data/noCrawlerFile ../data/indexes/noCrawler

# Invalid indexFile (non-existent path)
./indexer ../data/letters0 ../notreal/notreal/notreal

# Invalid indexFile (readonly directory)
./indexer ../data/letters0 ../data/readonlydirectory/letters0

# Invalid indexFile (read only file)
./indexer ../data/letters0 ../data/indexes/readonlyindexfile


# valid cases
./indexer ../data/letters0 ../data/indexes/letters0
./indextest ../data/indexes/letters0 ../data/indexes/letters0copy
../../shared/tse/indexcmp ../data/indexes/letters0 ../data/indexes/letters0copy

./indexer ../data/toscrape1 ../data/indexes/toscrape1
./indextest ../data/indexes/toscrape1 ../data/indexes/toscrape1copy
../../shared/tse/indexcmp ../data/indexes/toscrape1 ../data/indexes/toscrape1copy

./indexer ../data/wikipedia1 ../data/indexes/wikipedia1
./indextest ../data/indexes/wikipedia1 ../data/indexes/wikipedia1copy
../../shared/tse/indexcmp ../data/indexes/wikipedia1 ../data/indexes/wikipedia1copy

# valgrind test
valgrind ./indexer ../data/letters10 ../data/indexes/letters10
valgrind ./indextest ../data/indexes/letters10 ../data/indexes/letters10copy
../../shared/tse/indexcmp ../data/indexes/letters10 ../data/indexes/letters10copy




