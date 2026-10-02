CXX = clang++
CXXFLAGS = -std=c++20 -Wall -Wextra -g
SRC = $(wildcard src/*.cpp)
HDR = $(wildcard src/*.hpp)
FILE ?= src/main.cpp

boro: $(SRC) $(HDR)
	$(CXX) $(CXXFLAGS) $(SRC) -o boro

run: boro
	./boro $(FILE)

release: $(SRC) $(HDR)
	$(CXX) -std=c++20 -Wall -Wextra -O2 $(SRC) -o boro

install: release
	cp boro /usr/local/bin/boro

.PHONY: run release