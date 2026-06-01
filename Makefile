.PHONY: clean test all

CC = cc
CFLAGS_COMMON = -std=c11 -Wall -Wextra -Isrc
CFLAGS = $(CFLAGS_COMMON) -O3 -march=native
KDTREE_TEST_SRC = .devel/ctest/test_kdtree.c
KDTREE_TEST_BIN = .devel/ctest/test_kdtree
R_LIB = .devel/r-lib

all: test clean

clean:
	rm -f $(KDTREE_TEST_BIN)
	find . \( -name '*.o' -o -name '*.so' \) -delete

$(KDTREE_TEST_BIN): $(KDTREE_TEST_SRC) src/kdtree.c src/kdtree.h
	$(CC) $(CFLAGS) -o $@ $(KDTREE_TEST_SRC) src/kdtree.c -lm

test: $(KDTREE_TEST_BIN)
	./$(KDTREE_TEST_BIN)
	pip install -e ".[dev]"
	pytest .devel/pytest -q
	mkdir -p $(R_LIB)
	R CMD INSTALL --library=$(R_LIB) .
	R_LIBS=$(CURDIR)/$(R_LIB) Rscript .devel/tinytest.R
	R CMD check . --no-manual --no-vignettes
