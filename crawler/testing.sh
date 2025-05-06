#!/bin/bash
# Mithun Rameshkumar   May 2025
# This bash script runs test cases for crawler.c

#make data/letters directory to store output results
mkdir ../data

mkdir ../data/letters
mkdir ../data/letters0
mkdir ../data/letters10

mkdir ../data/toscrape0
mkdir ../data/toscrape1

mkdir ../data/wikipedia0
mkdir ../data/wikipedia1

#invalid urls:
./crawler https://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../data/letters 2

./crawler randomarg ../data/letters 2

#invalid directories
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html randomdirectory/doesntexist/ 2

./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html randomarg 2

#invalid depths
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../data/letters -1

./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../data/letters 12

#valgrind test
valgrind ./crawler http://cs50tse.cs.dartmouth.edu/tse/toscrape/index.html ../data/toscrape1 1

# #valid test cases 
 ./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../data/letters0 0
 ./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../data/letters10 10

 ./crawler http://cs50tse.cs.dartmouth.edu/tse/toscrape/index.html ../data/toscrape0 0

 ./crawler http://cs50tse.cs.dartmouth.edu/tse/wikipedia/index.html ../data/wikipedia0 0
 ./crawler http://cs50tse.cs.dartmouth.edu/tse/wikipedia/index.html ../data/wikipedia1 1

