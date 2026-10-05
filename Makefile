CXX = clang++
CXXFLAGS = -std=c++20 -Wall -Wextra -g
SRC = $(wildcard src/*.cpp)
HDR = $(wildcard src/*.hpp)
FILE ?= src/main.cpp

# read the version straight from config.hpp, so it's only defined in one place
VERSION := $(shell sed -n 's/.*VERSION *= *"\([^"]*\)".*/\1/p' src/config.hpp)

# signing settings — fill these in
APP_CERT     = Developer ID Application: Beyond Diamond Limited (92JK43YAHC)
INSTALL_CERT = Developer ID Installer: Beyond Diamond Limited (92JK43YAHC)
NOTARY_PROFILE = boro-notary
PKG = boro-$(VERSION).pkg

boro: $(SRC) $(HDR)
	$(CXX) $(CXXFLAGS) $(SRC) -o boro
	mkdir build
	mv boro build

run: boro
	./boro $(FILE)

release: $(SRC) $(HDR)
	$(CXX) -std=c++20 -Wall -Wextra -O2 $(SRC) -o boro
	mkdir build
	mv boro build

install: release
	cp build/boro /usr/local/bin/boro

# universal (Apple Silicon + Intel) release build
universal: $(SRC) $(HDR)
	$(CXX) -std=c++20 -Wall -Wextra -O2 -arch arm64 -arch x86_64 -mmacosx-version-min=11.0 $(SRC) -o boro

# signed + notarized installer
pkg: universal
	codesign --force --sign "$(APP_CERT)" --options runtime --timestamp boro
	rm -rf build/pkgroot
	mkdir -p pkgroot/usr/local/bin
	cp boro pkgroot/usr/local/bin/
	pkgbuild --root pkgroot --identifier com.ethandadev.boro --version $(VERSION) \
	         --install-location / --sign "$(INSTALL_CERT)" $(PKG)
	xcrun notarytool submit $(PKG) --keychain-profile "$(NOTARY_PROFILE)" --wait
	xcrun stapler staple $(PKG)
	mv pkgbuild build
	mv $(PKG) build
	mv boro build


clean:
	rm -rf boro pkgroot *.pkg build

.PHONY: run release install universal pkg clean