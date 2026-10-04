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
    std::string status = ""; // status message to display at the bottom of the screen 
    std::time_t statusTime = 0;
    std::string fileName = "";
    bool dirty = false; // flag to indicate if the buffer has unsaved changes
    size_t rowOffset = 0; // row offset for scrolling
};

bool loadFile(Buffer& buf, const std::string& path);   // declaration only

void insertChar(Buffer& buf, char c); // declaration only

void insertNewLine(Buffer& buf); // declaration only

void deleteChar(Buffer& buf); // declaration only

bool saveFile(Buffer& buf, const std::string& path); // declaration only

void setStatus(Buffer& buf, const std::string& msg); // declaration only

// moving the cursor with arrow keys

void moveUp(Buffer& buf); // declaration only

void moveDown(Buffer& buf); // declaration only

void moveLeft(Buffer& buf); // declaration only

void moveRight(Buffer& buf); // declaration only

void scroll(Buffer& buf, size_t textRows); // declaration only