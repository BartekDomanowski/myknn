.PHONY: clean test r_test all

CXX = c++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra -Isrc
RSCRIPT = Rscript
PKG_VERSION = 0.0.1
PKG_NAME = AirRoute
PKG_TARBALL = $(PKG_NAME)_$(PKG_VERSION).tar.gz
CPP_TEST_SRC = .devel/cpptest/test_airroute.cpp
CPP_TEST_BIN = .devel/cpptest/test_airroute
TCAS_TEST_SRC = .devel/cpptest/test_tcas_brute.cpp
TCAS_TEST_BIN = .devel/cpptest/test_tcas_brute

clean:
	rm -f $(PKG_TARBALL) $(CPP_TEST_BIN) $(TCAS_TEST_BIN)
	rm -f src/*.o src/*.so *.so
	rm -rf $(PKG_NAME).Rcheck

test:
	$(CXX) $(CXXFLAGS) -o $(CPP_TEST_BIN) $(CPP_TEST_SRC) src/airroute.cpp
	./$(CPP_TEST_BIN)
	$(CXX) $(CXXFLAGS) -o $(TCAS_TEST_BIN) $(TCAS_TEST_SRC) src/airroute.cpp src/tcas.cpp
	./$(TCAS_TEST_BIN)

r_test:
	$(RSCRIPT) -e 'Rcpp::compileAttributes(".")'
	$(RSCRIPT) -e 'roxygen2::roxygenise(".")'
	R CMD INSTALL .
	$(RSCRIPT) .devel/tinytest.R

all: test r_test clean 
