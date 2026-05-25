.PHONY: clean build test

CXX = c++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra -Isrc
PKG_VERSION = 0.0.1
PKG_NAME = AirRoute
PKG_TARBALL = $(PKG_NAME)_$(PKG_VERSION).tar.gz
CPP_TEST_SRC = .devel/cpptest/test_airroute.cpp
CPP_TEST_BIN = .devel/cpptest/test_airroute

clean:
	rm -f $(PKG_TARBALL) $(CPP_TEST_BIN)
	rm -rf $(PKG_NAME).Rcheck

build:
	R CMD build .

test:
	$(CXX) $(CXXFLAGS) -o $(CPP_TEST_BIN) $(CPP_TEST_SRC) src/airroute.cpp
	./$(CPP_TEST_BIN)
