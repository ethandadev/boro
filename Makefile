CXX = clang++
CXXFLAGS = -std=c++20 -Wall -Wextra -g -Isrc/core                       # CHANGED: -Isrc/core
RELFLAGS = -std=c++20 -Wall -Wextra -O2 -Isrc/core                      # NEW: shared release flags
SRC = $(wildcard src/core/*.cpp) $(wildcard src/terminal/*.cpp)          # CHANGED
HDR = $(wildcard src/core/*.hpp) $(wildcard src/terminal/*.hpp)          # CHANGED
FILE ?= src/terminal/main.cpp                                            # CHANGED
BUILD = build-terminal

# read the version straight from config.hpp, so it's only defined in one place
VERSION := $(shell sed -n 's/.*VERSION *= *"\([^"]*\)".*/\1/p' src/core/config.hpp)   # CHANGED path

APP_CERT       = Developer ID Application: Beyond Diamond Limited (92JK43YAHC)
INSTALL_CERT   = Developer ID Installer: Beyond Diamond Limited (92JK43YAHC)
NOTARY_PROFILE = boro-notary
PKG = $(BUILD)/boro-$(VERSION).pkg

# debug build — the target name is the file it produces
$(BUILD)/boro: $(SRC) $(HDR)
	mkdir -p $(BUILD)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(BUILD)/boro

run: $(BUILD)/boro
	./$(BUILD)/boro $(FILE)

release: $(SRC) $(HDR)
	mkdir -p $(BUILD)
	$(CXX) $(RELFLAGS) $(SRC) -o $(BUILD)/boro

install: release
	cp $(BUILD)/boro /usr/local/bin/boro

universal: $(SRC) $(HDR)
	mkdir -p $(BUILD)
	$(CXX) $(RELFLAGS) -arch arm64 -arch x86_64 -mmacosx-version-min=11.0 $(SRC) -o $(BUILD)/boro

pkg: universal
	codesign --force --sign "$(APP_CERT)" --options runtime --timestamp $(BUILD)/boro
	rm -rf $(BUILD)/pkgroot
	mkdir -p $(BUILD)/pkgroot/usr/local/bin
	cp $(BUILD)/boro $(BUILD)/pkgroot/usr/local/bin/
	pkgbuild --root $(BUILD)/pkgroot --identifier com.ethandadev.boro --version $(VERSION) \
	         --install-location / --sign "$(INSTALL_CERT)" $(PKG)
	xcrun notarytool submit $(PKG) --keychain-profile "$(NOTARY_PROFILE)" --wait
	xcrun stapler staple $(PKG)

# build and launch the Qt version
gui:
	cmake -B build -DCMAKE_PREFIX_PATH=$$(brew --prefix qt)
	cmake --build build
		./build/Boro.app/Contents/MacOS/Boro

clean:
	rm -rf $(BUILD) build

.PHONY: run release install universal pkg gui clean