# Makefile for 'querier' module
# Builds and compiles querier and its tests
#
# Mithun Rameshkumar, May 2025

# object files, and the target library
C = ../common
L = ../libcs50
LIBS = -lm
LLIBS = $C/common.a $L/libcs50-given.a

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(FLAGS) -I$L -I$C
CC = gcc
MAKE = make


# default
all: querier

# querier
querier: querier.o
	$(CC) $(CFLAGS) $^ $(LIBS) $(LLIBS) -o $@

.PHONY: all test clean

#clean
clean:
	rm -f core
	rm -f querier *~ *.o

#test
test: testing.sh querier
	chmod +x testing.sh
	bash -v ./testing.sh > testing.out 2>&1
