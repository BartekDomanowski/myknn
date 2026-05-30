.PHONY: clean test all

CC = cc
CFLAGS = -std=c11 -O2 -Wall -Wextra -Isrc
KDTREE_TEST_SRC = .devel/cpptest/test_kdtree.c
KDTREE_TEST_BIN = .devel/cpptest/test_kdtree

clean:
	rm -f $(KDTREE_TEST_BIN)

test:
	$(CC) $(CFLAGS) -o $(KDTREE_TEST_BIN) $(KDTREE_TEST_SRC) src/kdtree.c 
	./$(KDTREE_TEST_BIN)

all: test
