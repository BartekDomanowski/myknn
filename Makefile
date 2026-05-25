.PHONY: clean test r_test all

CXX = c++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra -Isrc
RSCRIPT = Rscript
PKG_VERSION = 0.0.1
PKG_NAME = AirRoute
PKG_TARBALL = $(PKG_NAME)_$(PKG_VERSION).tar.gz
CPP_TEST_SRC = .devel/cpptest/test_airroute.cpp
CPP_TEST_BIN = .devel/cpptest/test_airroute

clean:
	rm -f $(PKG_TARBALL) $(CPP_TEST_BIN)
	rm -f src/*.o src/*.so *.so
	rm -rf $(PKG_NAME).Rcheck

test:
	$(CXX) $(CXXFLAGS) -o $(CPP_TEST_BIN) $(CPP_TEST_SRC) src/airroute.cpp
	./$(CPP_TEST_BIN)

r_test:
	R CMD build .
	$(RSCRIPT) -e 'Rcpp::compileAttributes(".")'
	$(RSCRIPT) -e 'roxygen2::roxygenise(".")'
	$(RSCRIPT) -e 'library(AirRoute); stopifnot(abs(dist_haversine_km(52.1657, 20.9671, 51.47, -0.4543) - 1469) < 20)'

all: test r_test clean 
