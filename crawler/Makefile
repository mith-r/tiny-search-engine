# Makefile for 'crawler' module
# Builds and compiles crawler and its tests
#
# Mithun Rameshkumar  May 2025

# crawler/Makefile

CC       := gcc
CFLAGS   := -Wall -Wextra -std=c11 -pedantic \
             -I../common -I../libcs50
LIBS := ../common/common.a ../libcs50/libcs50-given.a

TARGET   := crawler
SRCS     := crawler.c
OBJS     := $(SRCS:.c=.o)

.PHONY: all clean test valgrind

all: $(TARGET)

$(TARGET): $(OBJS) $(LIBS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS) -lm

# Compile crawler.c into crawler.o
crawler.o: crawler.c \
           ../common/pagedir.h \
           ../libcs50/webpage.h \
           ../libcs50/bag.h \
           ../libcs50/hashtable.h \
           ../libcs50/mem.h
	$(CC) $(CFLAGS) -c crawler.c

# Run the functional test script, output to testing.out
test: testing.sh
	chmod +x testing.sh
	bash -v ./testing.sh


# Run crawler under Valgrind for memory-leak checking
# Adjust SEED, DIR, and DEPTH as needed
valgrind: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all \
	  ./$(TARGET) http://cs50tse.cs.dartmouth.edu/tse/letters \
	                data 1

clean:
	rm -f core
	rm -f crawler *~ *.o