# Compiler and Flags
CC = gcc
# Libraries
LIBS = ../libcs50/libcs50-given.a ../common/common.a

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(TESTING) -I$LIBS
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

# Dependencies 
OBJS = querier.o ../common/pagedir.o ../common/index.o ../common/word.o

.PHONY: all querier test clean

# Default target
all: querier

# build
querier: $(OBJS) $(LIBS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o querier

# test make test &> testing.out
test: querier testing.sh
	bash -v testing.sh

# clean
clean:
	rm -f $(OBJS) querier


