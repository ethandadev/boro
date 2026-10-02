// src/terminal.cpp
#include "terminal.hpp"
#include <iostream>
#include <iomanip>
#include <string>

void render(const Buffer& buf) {
    std::cout << "\x1b[2J"; // clear the screen
    std::cout << "\x1b[H"; // move the cursor to the top-left corner

    size_t lineCount = buf.lines.size(); // size_t is an unsigned integer type used for sizes

    size_t gutterWidth = std::to_string(lineCount).size(); // variable to store the highest digit in the line numbers

    for (size_t  i = 0; i < lineCount; i++) { // for loop to iterate through the lines, start, keep going while, after each round
        std::cout << std::setw(gutterWidth) << i + 1 << " | " << buf.lines[i] << "\n"; //std::setw is used to set the width of the output, i + 1 is used to display the line number starting from 1 instead of 0
    }

    std::cout << "Cursor position: (" << buf.cursor.row << ", " << buf.cursor.col << ")\n";

    std::cout << "\x1b[" << buf.cursor.row + 1 << ";" << buf.cursor.col + gutterWidth + 3 + 1 << "H";
}