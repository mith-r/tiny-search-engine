# Makefile for 'indexer' module
# Builds and compiles indexer and its tests
#
# Mithun Rameshkumar  May 2025

CC       := gcc
CFLAGS   := -Wall -Wextra -std=c11 -pedantic \
             -I../common -I../libcs50
LIBS     := ../common/common.a ../libcs50/libcs50-given.a

TARGET   := indexer indextest
SRCS     := indexer.c  indextest.c
OBJS     := $(SRCS:.c=.o)

.PHONY: all clean test

all: indexer indextest

indexer: indexer.o $(LIBS)
	$(CC) $(CFLAGS) -o $@ indexer.o $(LIBS) -lm

indextest: indextest.o $(LIBS)
	$(CC) $(CFLAGS) -o $@ indextest.o $(LIBS) 

# Compile indexer.c into indexer.o
indexer.o:	indexer.c \
			../common/index.h \
            ../common/pagedir.h \
            ../libcs50/webpage.h \
            ../libcs50/mem.h 
	$(CC) $(CFLAGS) -c indexer.c 

# Compile indextest.c into indextest.o
indextest.o:	indextest.c \
				../common/index.h \
				../common/pagedir.h \
				../libcs50/webpage.h \
				../libcs50/mem.h 
	$(CC) $(CFLAGS) -c indextest.c

			 
# Functional test script
test: testing.sh
	chmod +x testing.sh
	bash -v ./testing.sh > testing.out 2>&1

clean:
	rm -f core \
          indexer indextest \
          *~ *.o \