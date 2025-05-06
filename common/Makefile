# Makefile for 'common' module
# Compiles pagedir.c
#
# Mithun Rmaeshkumar May 2025

CC       := gcc
CFLAGS   := -Wall -Wextra -std=c11 -pedantic -I../libcs50
AR       := ar
ARFLAGS  := cr

TARGET   := common.a
SRCS     := pagedir.c
OBJS     := $(SRCS:.c=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(AR) $(ARFLAGS) $@ $^

# Rebuild pagedir.o if pagedir.c or its headers change
pagedir.o: pagedir.c pagedir.h ../libcs50/webpage.h
	$(CC) $(CFLAGS) -c pagedir.c -o pagedir.o

clean:
	rm -f $(OBJS) $(TARGET)