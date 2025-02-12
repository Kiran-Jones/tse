# builds common.a library
# also has clean which removes files produced by make

OBJS = pagedir.o
LIB = common.a

CFLAGS = -Wall -pedantic -std=c11 -ggdb
CC = gcc

# Build $(LIB) by archiving object files
# (you will need to drop in copy of set.c, counters.c, hashtable.c)
$(LIB): $(OBJS)
	ar cr $(LIB) $(OBJS)

# Dependencies: object files depend on header files
pagedir.o: pagedir.h

.PHONY: clean 


# clean up after our compilation
clean:
	rm -f pagedir.o common.a
