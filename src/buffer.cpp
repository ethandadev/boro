// buffer.cpp — how it works
#include "buffer.hpp"
#include <fstream>
#include <iostream>

//load file function to make main cleaner
bool loadFile(const std::string& path, std::vector<std::string>& lines) {
    std::ifstream file(path);
    if (!file) {          // ! means "not", so "if the file did NOT open"
        // handle the problem
        std::cerr << "Error: Could not open file " << path << "\n"; // cerr is used to print error messages to the console
        return false;
    }

    std::string line;

    while (std::getline(file, line)) { // while loop to read each line of the file until the end
        lines.push_back(line); // add the line to the vector
    }

    return true; // return true to indicate success
}

void insertChar(Buffer& buf, char c) {
    buf.lines[buf.cursor.row].insert(buf.cursor.col, 1, c); // insert the character at the cursor position
    buf.cursor.col++; // Move cursor to the right
}