# Compiler and Flags
CC = gcc
# Libraries
LIBS = ../libcs50/libcs50-given.a ../common/common.a

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(TESTING) -I$LIBS
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

# Dependencies 
INDEXER_OBJS = indexer.o ../common/pagedir.o ../common/index.o ../common/word.o

INDEXTEST_OBJS = indextest.o ../common/pagedir.o ../common/index.o ../common/word.o

.PHONY: all indexer indextest test clean

# Default target
all: indexer indextest

# build
indexer: $(INDEXER_OBJS) $(LIBS)
	$(CC) $(CFLAGS) $(INDEXER_OBJS) $(LIBS) -o indexer

indextest: $(INDEXTEST_OBJS) $(LIBS)
	$(CC) $(CFLAGS) $(INDEXTEST_OBJS) $(LIBS) -o indextest

# test
test: indexer indextest testing.sh
	bash -v testing.sh

# clean
clean:
	rm -f $(INDEXER_OBJS) $(INDEXTEST_OBJS) indexer indextest
	rm -f ../data/letters.index ../data/letters.indextest
	rm -f ../data/scrape.index ../data/scrape.indextest
	rm -f ../data/wiki.index ../data/wiki.indextest

