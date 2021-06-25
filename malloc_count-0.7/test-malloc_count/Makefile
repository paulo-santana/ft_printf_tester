# Simplistic Makefile for malloc_count example

CC = gcc
CFLAGS = -g -W -Wall -ansi -I..
LDFLAGS =
LIBS = -ldl
OBJS = test.o ../malloc_count.o ../stack_count.o

all: test

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

test: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

clean:
	rm -f *.o test
