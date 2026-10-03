# boro

A terminal code editor written in C++, built from scratch while learning the language.
The goal: edit boro's own source code in boro.

macOS only.

## Build
```
clang++ -std=c++20 -Wall -Wextra src/*.cpp -o boro
./boro src/main.cpp
```

## Status
Early days: working kinda, you can edit files but working on the capability to move with arrow keys.