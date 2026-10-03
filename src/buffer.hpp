// src/buffer.hpp
#pragma once
#include <string>
#include <vector>
#include <ctime>

struct Cursor {
    size_t row = 0;
    size_t col = 0;
};

struct Buffer {
    std::vector<std::string> lines;
    Cursor cursor;
    std::string status = "";
    std::time_t statusTime = 0;
};

bool loadFile(const std::string& path, std::vector<std::string>& lines);   // declaration only

void insertChar(Buffer& buf, char c); // declaration only

void insertNewLine(Buffer& buf); // declaration only

void deleteChar(Buffer& buf); // declaration only

bool saveFile(const std::string& path, const std::vector<std::string>& lines); // declaration only

void setStatus(Buffer& buf, const std::string& msg); // declaration only