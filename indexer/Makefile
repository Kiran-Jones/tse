# Compiler and Flags
CC = gcc
# Libraries
LIBS = ../libcs50/libcs50-given.a ../common/common.a

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(TESTING) -I$LIBS
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

# Dependencies 
OBJS = indexer.o ../common/pagedir.o ../common/index.o ../common/word.o

.PHONY: all clean test valgrind

# Default target
TARGET = indexer
all: $(TARGET)

# build
$(TARGET): $(OBJS) $(LIBS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $(TARGET)

# test
# test: TESTING = -DLOGGING # comment on this line to turn on detailed logging for testing
test: $(TARGET) testing.sh
	bash -v testing.sh

# valgrind
valgrind: $(TARGET)
	$(VALGRIND) ./$(TARGET) ../data/letters ../data/letters.index

# clean
clean:
	rm -f $(OBJ) $(TARGET)
	rm -rf ../data/letters.index
