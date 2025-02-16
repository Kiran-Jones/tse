# 0. Setup 

# create index and indextest files for each directory
touch ../data/letters.index ../data/letters.indextest
touch ../data/scrape.index ../data/scrape.indextest
touch ../data/wiki.index ../data/wiki.indextest

# create directory for files to test permissions
mkdir -p ../data/testfiles

# create non-readable pageDirectory 
touch ../data/testfiles/noreaddirectory
# removes read permissions
chmod -r ../data/testfiles/noreaddirectory

# create non-writeable index file
touch ../data/testfiles/nowriteindex
# removes write permissions 
chmod -w ../data/testfiles/nowriteindex


# 1. Erroneous arguments for indexer

# too few arguments (1)
./indexer

# too many arguments (6)
./indexer ../data/letters ../data/letters.index extraArg1 extraArg2 extraArg3

# bad pageDirectory
./indexer iufhkcsjdns ../data/letters.index

# non crawler-produced pageDirectory
./indexer ../data ..data/letters.index

# non-readable pageDirectory
./indexer ../data/testfiles/noreaddirectory ../data/letters.index

# non-writable indexFilename
./indexer ../data/letters ../data/testfiles/nowriteindex


# 2. indexer and indextest
# ran with valgrind, passes all checks 
valgrind --leak-check=full --show-leak-kinds=all ./indexer ../data/letters ../data/letters.index
valgrind --leak-check=full --show-leak-kinds=all ./indextest ../data/letters.index ../data/letters.indextest
~/cs50-dev/shared/tse/indexcmp ../data/letters.index ../data/letters.indextest
# no output from indexcmp signifies equivalent file contents 

./indexer ../data/scrape ../data/scrape.index
./indextest ../data/scrape.index ../data/scrape.indextest
~/cs50-dev/shared/tse/indexcmp ../data/scrape.index ../data/scrape.indextest
# no output from indexcmp signifies equivalent file contents 

./indexer ../data/wiki ../data/wiki.index
./indextest ../data/wiki.index ../data/wiki.indextest
~/cs50-dev/shared/tse/indexcmp ../data/wiki.index ../data/wiki.indextest
# no output from indexcmp signifies equivalent file contents 


# 3. Erroneous arguments for indextest

# too few arguments (1)
./indextest

# too many arguments (6)
./indextest ../data/letters.index ../data/letters.indextest extraArg1 extraArg2 extraArg3

# non-readable pageDirectory
./indextest ../data/testfiles/noreaddirectory ../data/letters.indextest

# non-writable indexFilename
./indextest ../data/letters.index ../data/testfiles/nowriteindex
