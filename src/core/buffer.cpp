// src/buffer.cpp — how it works
#include "buffer.hpp"
#include "config.hpp"
#include <fstream>
#include <iostream>
#include <filesystem>

//load file function to make main cleaner
bool loadFile(Buffer& buf, const std::string& path) {
    // loads a file in the buffer.
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
    // insert a character in the buffer
    buf.lines[buf.cursor.row].insert(buf.cursor.col, 1, c); // insert the character at the cursor position
    buf.cursor.col++; // Move cursor to the right
    buf.dirty = true; // set the dirty flag to true to indicate that the buffer has unsaved changes
}

void insertNewLine(Buffer& buf) {
    // insert new line in buffer
    std::string afterCursor = buf.lines[buf.cursor.row].substr(buf.cursor.col); // substr is like s[5:] in python
    buf.lines[buf.cursor.row].erase(buf.cursor.col); // erase everything after the cursor position
    buf.lines.insert(buf.lines.begin() + buf.cursor.row + 1, afterCursor);
    buf.cursor.row++; // Move cursor down
    buf.cursor.col = 0; // Move cursor to the beginning of the new line
    buf.dirty = true; // set the dirty flag to true to indicate that the buffer has unsaved changes
}

void deleteChar(Buffer& buf) {
    // delete character in buffer
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
    // saves file from buffer
    std::ofstream file(path);
    if (!file) {
        return false;
    }

    for (const std::string& line : buf.lines) {
        file << line << "\n";
    }

    buf.dirty = false; // reset the dirty flag after saving
    buf.isNew = false; // no longer new as you saved
    return true;
}

void moveUp(Buffer& buf) {
    // moves cursor up
    if (buf.cursor.row > 0) {
        buf.cursor.row--;
        if (buf.cursor.col > buf.lines[buf.cursor.row].length()) {
            buf.cursor.col = buf.lines[buf.cursor.row].length(); // Move cursor to the end of the line if it's longer than the current column
        }
    }
}

void moveDown(Buffer& buf) {
    // move cursor down
    if (buf.cursor.row < buf.lines.size() - 1) {
        buf.cursor.row++;
        if (buf.cursor.col > buf.lines[buf.cursor.row].length()) {
            buf.cursor.col = buf.lines[buf.cursor.row].length(); // Move cursor to the end of the line if it's longer than the current column
        }
    }
}

void moveLeft(Buffer& buf) {
    // moves cursor left
    if (buf.cursor.col > 0) {
        buf.cursor.col--;
    } else if (buf.cursor.row > 0) {
        buf.cursor.row--;
        buf.cursor.col = buf.lines[buf.cursor.row].length(); // Move cursor to the end of the previous line
    }
}

void moveRight(Buffer& buf) {
    // move cursor to thr right.
    if (buf.cursor.col < buf.lines[buf.cursor.row].length()) {
        buf.cursor.col++;
    } else if (buf.cursor.row < buf.lines.size() - 1) {
        buf.cursor.row++;
        buf.cursor.col = 0; // Move cursor to the beginning of the next line
    }
}

void insertTab(Buffer& buf) {
    // insert tab
    buf.lines[buf.cursor.row].insert(buf.cursor.col, TAB_WIDTH, ' '); // insert the character at the cursor position
    buf.cursor.col += TAB_WIDTH; // Move cursor to the right
    buf.dirty = true; // set the dirty flag to true to indicate that the buffer has unsaved changes
}

//helper functions
// i dont want to add comments so.

std::string textBetween(const Buffer& buf, Cursor start, Cursor end) {
    if (start.row == end.row) {
        return buf.lines[start.row].substr(start.col, end.col-start.col);
    }
    
    std::string text = buf.lines[start.row].substr(start.col);
    for (size_t row = start.row + 1; row < end.row; row++) {
        text += "\n" + buf.lines[row];
    }
    text += "\n" + buf.lines[end.row].substr(0, end.col);

    return text;
}

void eraseRange(Buffer& buf, Cursor start, Cursor end) {
    std::string after = buf.lines[end.row].substr(end.col);       // grab this BEFORE changing anything
    buf.lines[start.row] = buf.lines[start.row].substr(0, start.col) + after;
    buf.lines.erase(buf.lines.begin() + start.row + 1, buf.lines.begin() + end.row + 1);
    buf.cursor = start;
    buf.dirty = true;
}

Cursor insertText(Buffer& buf, Cursor at, const std::string& text) {
    std::vector<std::string> pieces;
    size_t from = 0;
    while (true) {
        size_t nl = text.find('\n', from);                  // like Python's text.find("\n", from), but npos instead of -1
        if (nl == std::string::npos) { pieces.push_back(text.substr(from)); break; }
        pieces.push_back(text.substr(from, nl - from));
        from = nl + 1;
    }

    std::string head = buf.lines[at.row].substr(0, at.col);
    std::string tail = buf.lines[at.row].substr(at.col);
    
    Cursor end;

    if (pieces.size() == 1) {
        buf.lines[at.row] = head + pieces[0] + tail;
        end = Cursor(at.row, head.size() + pieces[0].size());
    } else {
        buf.lines[at.row] = head + pieces[0];
        end = Cursor(at.row + pieces.size() - 1, pieces.back().size());
        pieces.back() += tail;      // back() = Python's [-1]
        buf.lines.insert(buf.lines.begin() + at.row + 1, pieces.begin() + 1, pieces.end()); 
    }

    buf.cursor = end;
    buf.dirty = true;
    return end;
}

Cursor endOf(Cursor start, const std::string& text) {
    Cursor end = start;
    for (char c : text) {
        if (c == '\n') {
            end.row++;
            end.col = 0;
        } else {
            end.col++;
        }
    }

    return end;
}

std::string cleanText(const std::string& text) {
    std::string out = "";

    for (char c : text) {
        if (c == '\t') {
            out += std::string(TAB_WIDTH, ' ');
        } else if (c == '\n' || (32 <= c && c <= 126)) {
            out += c;
        }
    }

    return out;
}