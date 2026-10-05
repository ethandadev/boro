CXX = clang++
CXXFLAGS = -std=c++20 -Wall -Wextra -g
SRC = $(wildcard src/*.cpp)
HDR = $(wildcard src/*.hpp)
FILE ?= src/main.cpp
BUILD = build

# read the version straight from config.hpp, so it's only defined in one place
VERSION := $(shell sed -n 's/.*VERSION *= *"\([^"]*\)".*/\1/p' src/config.hpp)

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
	$(CXX) -std=c++20 -Wall -Wextra -O2 $(SRC) -o $(BUILD)/boro

install: release
	cp $(BUILD)/boro /usr/local/bin/boro

universal: $(SRC) $(HDR)
	mkdir -p $(BUILD)
	$(CXX) -std=c++20 -Wall -Wextra -O2 -arch arm64 -arch x86_64 -mmacosx-version-min=11.0 $(SRC) -o $(BUILD)/boro

pkg: universal
	codesign --force --sign "$(APP_CERT)" --options runtime --timestamp $(BUILD)/boro
	rm -rf $(BUILD)/pkgroot
	mkdir -p $(BUILD)/pkgroot/usr/local/bin
	cp $(BUILD)/boro $(BUILD)/pkgroot/usr/local/bin/
	pkgbuild --root $(BUILD)/pkgroot --identifier com.ethandadev.boro --version $(VERSION) \
	         --install-location / --sign "$(INSTALL_CERT)" $(PKG)
	xcrun notarytool submit $(PKG) --keychain-profile "$(NOTARY_PROFILE)" --wait
	xcrun stapler staple $(PKG)

clean:
	rm -rf $(BUILD)

.PHONY: run release install universal pkg clean