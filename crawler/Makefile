


# Compiler and Flags
CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb $(TESTING) -I../lib
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

# Libraries
LIBS = ../libcs50/libcs50-given.a

# Dependencies 
OBJS = crawler.o ../common/pagedir.o

.PHONY: all clean test valgrind


# Default target
TARGET = crawler
all: $(TARGET)

# build
$(TARGET): $(OBJS) $(LIBS)
	$(CC) $(CFLAGS) -o $(TARGET)

# test
test: $(TARGET) testing.sh
	bash -v testing.sh

# valgrind
valgrind: $(TARGET)
	$(VALGRIND) ./$(TARGET) http://cs50tse.cs.dartmouth.edu/tse/ ../data/letters/

# clean
clean:
	rm -f $(OBJ) $(TARGET)
	rm -rf ../data/letters*

