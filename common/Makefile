# Makefile for 'common' module
# Compiles pagedir.c
#
# Mithun Rmaeshkumar May 2025

#object files and target
L = ../libcs50
OBJS = pagedir.o index.o word.o 
LIB = common.a

CFLAGS = -Wall -pedantic -std=c11 -ggdb -I$(L) $(FLAGS)
CC = gcc
Make = make

#Build library with object files
$(LIB): $(OBJS)
	ar cr $(LIB) $(OBJS)

#Dependenceis
pagedir.o: pagedir.h
index.o: index.h
word.o: word.h

.Phony: clean sourcelist

clean:
	rm -f core
	rm -f $(LIB) *~ *.o