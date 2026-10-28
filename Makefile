CXX = clang++
CXXFLAGS = -std=c++20 -Wall -Wextra -g -Isrc/core                       # CHANGED: -Isrc/core
RELFLAGS = -std=c++20 -Wall -Wextra -O2 -Isrc/core                      # shared release flags
SRC = $(wildcard src/core/*.cpp) $(wildcard src/terminal/*.cpp)          # CHANGED
HDR = $(wildcard src/core/*.hpp) $(wildcard src/terminal/*.hpp)          # CHANGED
FILE ?= src/terminal/main.cpp                                            # CHANGED
BUILD = build-terminal

# read the version straight from config.hpp, so it's only defined in one place
VERSION := $(shell sed -n 's/.*VERSION *= *"\([^"]*\)".*/\1/p' src/core/config.hpp)
VERSION := $(strip $(VERSION))

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
# official Qt (not Homebrew's) so the app runs on older macOS + Intel
QT_DIR = $(HOME)/Qt/6.10.3/macos
APP = build/Boro.app
APP_PKG = build/Boro-$(VERSION).pkg

gui-build:
	cmake -B build -DCMAKE_PREFIX_PATH=$(QT_DIR)
	cmake --build build

gui: gui-build
	./$(APP)/Contents/MacOS/Boro

# one-time setup: download the official Qt into ~/Qt
# (uses its own little Python venv, so it doesn't touch Homebrew's Python)
qt:
	python3 -m venv $(HOME)/Qt/.venv
	$(HOME)/Qt/.venv/bin/pip install aqtinstall
	$(HOME)/Qt/.venv/bin/python -m aqt install-qt mac desktop 6.10.3 clang_64 -O $(HOME)/Qt

# build, bundle Qt, sign, package, notarize -> build/Boro-<version>.pkg
app-pkg:
	rm -rf build
	$(MAKE) gui-build
	$(QT_DIR)/bin/macdeployqt $(APP)
	find $(APP)/Contents -name "*.dylib" -exec codesign --force --options runtime --timestamp --sign "$(APP_CERT)" {} \;
	find $(APP)/Contents/Frameworks -maxdepth 1 -name "*.framework" -exec codesign --force --options runtime --timestamp --sign "$(APP_CERT)" {} \;
	codesign --force --options runtime --timestamp --sign "$(APP_CERT)" $(APP)
	codesign --verify --deep --strict $(APP)
	pkgbuild --component $(APP) --install-location /Applications --identifier com.ethandadev.boro.app \
	         --version $(VERSION) --sign "$(INSTALL_CERT)" $(APP_PKG)
	xcrun notarytool submit $(APP_PKG) --keychain-profile "$(NOTARY_PROFILE)" --wait
	xcrun stapler staple $(APP_PKG)
	@echo "done: $(APP_PKG)"
	@vtool -show $(APP)/Contents/MacOS/Boro | grep minos

clean:
	rm -rf $(BUILD) build

.PHONY: run release install universal pkg gui gui-build qt app-pkg clean