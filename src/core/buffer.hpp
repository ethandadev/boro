// src/buffer.hpp
#pragma once
#include <string>
#include <vector>
#include <compare>

struct Cursor {
    size_t row = 0;
    size_t col = 0;
    auto operator<=>(const Cursor&) const = default;
};

struct Buffer {
    std::vector<std::string> lines;
    Cursor cursor;
    std::string fileName = "";
    bool dirty = false; // flag to indicate if the buffer has unsaved changes
    bool isNew = false;
};

bool loadFile(Buffer& buf, const std::string& path);   // declaration only

void insertChar(Buffer& buf, char c); // declaration only

void insertNewLine(Buffer& buf); // declaration only

void deleteChar(Buffer& buf); // declaration only

bool saveFile(Buffer& buf, const std::string& path); // declaration only


// moving the cursor with arrow keys

void moveUp(Buffer& buf); // declaration only

void moveDown(Buffer& buf); // declaration only

void moveLeft(Buffer& buf); // declaration only

void moveRight(Buffer& buf); // declaration only


void insertTab(Buffer& buf); // declaration only

//helper functions for gui

std::string textBetween(const Buffer& buf, Cursor start, Cursor end);

void eraseRange(Buffer& buf, Cursor start, Cursor end);

Cursor insertText(Buffer& buf, Cursor at, const std::string& text);

Cursor endOf(Cursor start, const std::string& text);

std::string cleanText(const std::string& text);        // used by paste, see 1d

