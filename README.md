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
Early days: loads selected file and displays lines