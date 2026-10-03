// src/buffer.hpp
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

void insertChar(Buffer& buf, char c); // declaration only

void insertNewLine(Buffer& buf); // declaration only

void deleteChar(Buffer& buf); // declaration only

bool saveFile(const std::string& path, const std::vector<std::string>& lines); // declaration only