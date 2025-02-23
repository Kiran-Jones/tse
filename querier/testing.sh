# 0. Setup 
# create directory for files to test permissions
mkdir -p ../data/testfiles

# create non-readable pageDirectory 
touch ../data/testfiles/noreaddirectory
# removes read permissions
chmod -r ../data/testfiles/noreaddirectory

# create non-readable index file 
touch ../data/testfiles/noread.index
# removes read permissions
chmod -r ../data/testfiles/noread.index


# 1. Erroneous command line arguments for querier

# too few arguments (1)
./querier

# too many arguments (6)
./querier ../data/letters ../data/letters.index extraArg1 extraArg2 extraArg3

# bad pageDirectory
./querier iufhkcsjdns ../data/letters.index

# non crawler-produced pageDirectory
./querier ../data ..data/letters.index

# non-readable pageDirectory
./querier ../data/testfiles/noreaddirectory ../data/letters.index

# non-readable index file
./querier ../data/letters ../data/testfiles/noread.index

# 2. Erroneous query inputs for querier 

# should print error message
echo "." | ./querier ../data/letters ../data/letters.index

echo "and" | ./querier ../data/letters ../data/letters.index

echo "or" | ./querier ../data/letters ../data/letters.index

echo "page and coding or"| ./querier ../data/letters ../data/letters.index

echo "page and and coding"| ./querier ../data/letters ../data/letters.index

echo "page and or coding"| ./querier ../data/letters ../data/letters.index

echo "page?"| ./querier ../data/letters ../data/letters.index

echo "coding!"| ./querier ../data/letters ../data/letters.index

echo "123"| ./querier ../data/letters ../data/letters.index


# 2. querier with sample text 
# should return docIDs 1 & 3 with scores 1 & 1
echo "the"| ./querier ../data/letters ../data/letters.index

# should return docID 6 with score 1
echo "graph"| ./querier ../data/letters ../data/letters.index

# should return a combination of the previous two 
echo "the or graph"| ./querier ../data/letters ../data/letters.index

# should return no docIDs
echo "the graph"| ./querier ../data/letters ../data/letters.index

# should behave the same as the last test
echo "the and graph"| ./querier ../data/letters ../data/letters.index



# 3. querier with fuzz testing
# toscrape data
../../../shared/tse/fuzzquery ../data/scrape.index 10 999 | ./querier ../data/scrape ../data/scrape.index

# wiki data
../../../shared/tse/fuzzquery ../data/wiki.index 10 999 | ./querier ../data/wiki ../data/wiki.index

# 4. querier with valgrind

valgrind --leak-check=full --show-leak-kinds=all echo "the"| ./querier ../data/letters ../data/letters.index

valgrind --leak-check=full --show-leak-kinds=all echo "tse"| ./querier ../data/letters ../data/letters.index

# 5. querier with valgrind AND fuzz testing 
valgrind --leak-check=full --show-leak-kinds=all ../../../shared/tse/fuzzquery ../data/scrape.index 10 500 | ./querier ../data/scrape ../data/scrape.index


