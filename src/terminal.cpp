// src/terminal.cpp
#include "terminal.hpp"
#include <iostream>
#include <iomanip>
#include <string>
#include <termios.h> // terminal settings api
#include <unistd.h> // for STDIN_FILENO

static termios original;

void enableRawMode() {
    tcgetattr(STDIN_FILENO, &original);        // read the current settings into `original`
    termios raw = original;                    // make a copy to modify
    raw.c_lflag &= ~(ECHO | ICANON);           // turn OFF echo and line-buffering
    raw.c_iflag &= ~(IXON);    // disable Ctrl+S / Ctrl+Q flow control
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);  // apply the modified settings

    std::cout << "\x1b[?1049h";   // switch to alternate screen
}

void disableRawMode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &original);
    std::cout << "\x1b[?1049l";   // switch back to normal screen
}

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

    std::cout << std::flush;   // send everything to the terminal NOW
}