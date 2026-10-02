// buffer.hpp
#pragma once
#include <string>
#include <vector>

struct Cursor {
    size_t row = 0;
    size_t col = 0;
};

struct Buffer {
    std::vector<std::string> lines;
    Cursor cursor;
};

bool loadFile(const std::string& path, std::vector<std::string>& lines);   // declaration only