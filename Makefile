.PHONY: clean build

PKG_VERSION = 0.0.1
PKG_NAME = AirRoute
PKG_TARBALL = $(PKG_NAME)_$(PKG_VERSION).tar.gz

clean:
	rm -f $(PKG_TARBALL)
	rm -rf $(PKG_NAME).Rcheck

build:
	R CMD build .