mkdir ../data
mkdir ../data/letters
mkdir ../data/scrape
mkdir ../data/wiki


# 1. Erroneous arguments

# too few arguments (1)
./crawler

# too many arguments (6)

./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/ ../data/letters 2 extraArgument oneMoreArgument 

# bad URL
./crawler iufhkcsjdns ../data/letters 2

# not internal URL
./crawler https://home.dartmouth.edu/ ..data/letters 2


# invalid maxDepth (too low)
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/ ../data/letters -1

# invalid maxDepth (too high)
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/ ../data/letters 11

# invalid maxDepth (not an int) 
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/ ../data/letters two


# 2. valgrind with moderate-sized test case

myvalgrind http://cs50tse.cs.dartmouth.edu/tse/toscrape/ ../data/scrape 1

# 3. run over all CS50 websites with specified depths

# letters with [0, 1, 2, 10]
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/ ../data/letters 0
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/ ../data/letters 10

# tests not used in testing.out
# ./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/ ../data/letters 1
# ./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/ ../data/letters 2

# toscrape with [0, 1, 2, 3]
./crawler http://cs50tse.cs.dartmouth.edu/tse/toscrape/ ../data/scrape 0
./crawler http://cs50tse.cs.dartmouth.edu/tse/toscrape/ ../data/scrape 1

# tests not used in testing.out
# ./crawler http://cs50tse.cs.dartmouth.edu/tse/toscrape/ ../data/scrape 2
# ./crawler http://cs50tse.cs.dartmouth.edu/tse/toscrape/ ../data/scrape 3

# wikipedia with [0, 1, 2]
./crawler http://cs50tse.cs.dartmouth.edu/tse/wikipedia/ ../data/wiki 0
./crawler http://cs50tse.cs.dartmouth.edu/tse/wikipedia/ ../data/wiki 1

# tests not used in testing.out
# ./crawler http://cs50tse.cs.dartmouth.edu/tse/wikipedia/ ../data/wiki 2

# 4. valgrind test

valgrind --leak-check=full --show-leak-kinds=all ./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/ ../data/letters 10
