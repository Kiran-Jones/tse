


# Compiler and Flags
CC = gcc
# Libraries
LIBS = ../libcs50/libcs50-given.a ../common/common.a

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(TESTING) -I$LIBS
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all



# Dependencies 
OBJS = crawler.o ../common/pagedir.o

.PHONY: all clean test valgrind


# Default target
TARGET = crawler
all: $(TARGET)

# build
$(TARGET): $(OBJS) $(LIBS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $(TARGET)

# test
test: $(TARGET) testing.sh
	bash -v testing.sh

# valgrind
valgrind: $(TARGET)
	$(VALGRIND) ./$(TARGET) http://cs50tse.cs.dartmouth.edu/tse/letters/ ../data/letters/ 4

# clean
clean:
	rm -f $(OBJ) $(TARGET)
	rm -rf ../data/letters*
	rm -rf ../data/scrape*
	rm -rf ../data/wiki*

