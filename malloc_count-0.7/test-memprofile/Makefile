# Simplistic Makefile for malloc_count example

CC = gcc
CXX = g++
CFLAGS = -g -W -Wall -ansi -I..
CXXFLAGS = -g -W -Wall -ansi -I..
LDFLAGS =
LIBS = -ldl
OBJS = test.o ../malloc_count.o

all: test

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $<

test: $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

clean:
	rm -f *.o test
