// src/buffer.cpp — how it works
#include "buffer.hpp"
#include "config.hpp"
#include <fstream>
#include <iostream>
#include <filesystem>

//load file function to make main cleaner
bool loadFile(Buffer& buf, const std::string& path) {
    buf.fileName = path; // store the file name in the buffer
    if (!std::filesystem::exists(path)) {
        buf.lines.push_back(""); // if the file does not exist, create a new empty buffer
        buf.isNew = true;
        return true; // return true to indicate success
    }
    std::ifstream file(path);
    if (!file) {          // ! means "not", so "if the file did NOT open"
        // handle the problem
        std::cerr << "Error: Could not open file " << path << "\n"; // cerr is used to print error messages to the console
        return false;
    }

    std::string line;

    while (std::getline(file, line)) { // while loop to read each line of the file until the end
        buf.lines.push_back(line); // add the line to the vector
    }

    if (buf.lines.empty()) {
        buf.lines.push_back(""); // if the file is empty, add an empty line to the vector
    }

    buf.isNew = false;
    return true; // return true to indicate success
}

void insertChar(Buffer& buf, char c) {
    buf.lines[buf.cursor.row].insert(buf.cursor.col, 1, c); // insert the character at the cursor position
    buf.cursor.col++; // Move cursor to the right
    buf.dirty = true; // set the dirty flag to true to indicate that the buffer has unsaved changes
}

void insertNewLine(Buffer& buf) {
    std::string afterCursor = buf.lines[buf.cursor.row].substr(buf.cursor.col); // substr is like s[5:] in python
    buf.lines[buf.cursor.row].erase(buf.cursor.col); // erase everything after the cursor position
    buf.lines.insert(buf.lines.begin() + buf.cursor.row + 1, afterCursor);
    buf.cursor.row++; // Move cursor down
    buf.cursor.col = 0; // Move cursor to the beginning of the new line
    buf.dirty = true; // set the dirty flag to true to indicate that the buffer has unsaved changes
}

void deleteChar(Buffer& buf) {
    if (buf.cursor.row == 0 && buf.cursor.col == 0) {
        return; // Nothing to delete if at the beginning of the buffer
    } else if (buf.cursor.col > 0) {
        buf.lines[buf.cursor.row].erase(buf.cursor.col - 1, 1); // erase the character before the cursor position
        buf.cursor.col--; // Move cursor to the left
    } else {
        size_t prevLineLength = buf.lines[buf.cursor.row - 1].length(); // get the length of the previous line
        buf.lines[buf.cursor.row - 1] += buf.lines[buf.cursor.row]; // append the current line to the previous line
        buf.lines.erase(buf.lines.begin() + buf.cursor.row); // erase the current line
        buf.cursor.row--; // Move cursor up
        buf.cursor.col = prevLineLength; // Move cursor to the end of the previous line
    } 
    buf.dirty = true; // set the dirty flag to true to indicate that the buffer has unsaved changes
}

bool saveFile(Buffer& buf,const std::string& path) {
    std::ofstream file(path);
    if (!file) {
        return false;
    }

    for (const std::string& line : buf.lines) {
        file << line << "\n";
    }

    buf.dirty = false; // reset the dirty flag after saving
    return true;
}

void moveUp(Buffer& buf) {
    if (buf.cursor.row > 0) {
        buf.cursor.row--;
        if (buf.cursor.col > buf.lines[buf.cursor.row].length()) {
            buf.cursor.col = buf.lines[buf.cursor.row].length(); // Move cursor to the end of the line if it's longer than the current column
        }
    }
}

void moveDown(Buffer& buf) {
    if (buf.cursor.row < buf.lines.size() - 1) {
        buf.cursor.row++;
        if (buf.cursor.col > buf.lines[buf.cursor.row].length()) {
            buf.cursor.col = buf.lines[buf.cursor.row].length(); // Move cursor to the end of the line if it's longer than the current column
        }
    }
}

void moveLeft(Buffer& buf) {
    if (buf.cursor.col > 0) {
        buf.cursor.col--;
    } else if (buf.cursor.row > 0) {
        buf.cursor.row--;
        buf.cursor.col = buf.lines[buf.cursor.row].length(); // Move cursor to the end of the previous line
    }
}

void moveRight(Buffer& buf) {
    if (buf.cursor.col < buf.lines[buf.cursor.row].length()) {
        buf.cursor.col++;
    } else if (buf.cursor.row < buf.lines.size() - 1) {
        buf.cursor.row++;
        buf.cursor.col = 0; // Move cursor to the beginning of the next line
    }
}

void insertTab(Buffer& buf) {
    buf.lines[buf.cursor.row].insert(buf.cursor.col, TAB_WIDTH, ' '); // insert the character at the cursor position
    buf.cursor.col += TAB_WIDTH; // Move cursor to the right
    buf.dirty = true; // set the dirty flag to true to indicate that the buffer has unsaved changes
}