// src/terminal.cpp
#include "terminal.hpp"
#include "config.hpp"
#include <iostream>
#include <iomanip>
#include <string>
#include <termios.h> // terminal settings api
#include <unistd.h> // for STDIN_FILENO
#include <sys/ioctl.h> // for ioctl, winsize
#include <ctime> // for time_t

size_t getScreenRows() {
    winsize ws;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws); // ioctl is used to get the window size
    return ws.ws_row; // return the number of rows in the terminal
}

static termios original;

void enableRawMode() {
    tcgetattr(STDIN_FILENO, &original);        // read the current settings into `original`
    termios raw = original;                    // make a copy to modify
    raw.c_lflag &= ~(ECHO | ICANON);           // turn OFF echo and line-buffering
    raw.c_iflag &= ~(IXON);    // disable Ctrl+S / Ctrl+Q flow control
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);  // apply the modified settings

    std::cout << "\x1b[?1049h";   // switch to alternate screen
}

void disableRawMode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &original);
    std::cout << "\x1b[?1049l";   // switch back to normal screen
}

void render(const Buffer& buf) {
    size_t screenRows = getScreenRows() - 2; // get the available rows in the terminal, minus 2 for the status bar and title bar

    std::cout << "\x1b[2J"; // clear the screen
    std::cout << "\x1b[H"; // move the cursor to the top-left corner

    // draw title bar
    std::string state = buf.dirty ? "Not Saved" : "Saved";
    std::cout << APP_NAME << " v" << VERSION << " | " << buf.fileName << " | " << buf.lines.size() << " lines | " << state << "\n";

    size_t lineCount = buf.lines.size(); // size_t is an unsigned integer type used for sizes

    size_t gutterWidth = std::to_string(lineCount).size(); // variable to store the highest digit in the line numbers

    for (size_t  i = 0; i < screenRows; i++) { // for loop to iterate through the lines, start, keep going while, after each round
        size_t fileLine = i + buf.rowOffset;
        if (fileLine >= buf.lines.size()) {
            break;
        }
        std::cout << std::setw(gutterWidth) << fileLine + 1 << " | " << buf.lines[fileLine] << "\n"; //std::setw is used to set the width of the output, i + 1 is used to display the line number starting from 1 instead of 0
    }

    std::cout << "\x1b[" << screenRows + 2 << ";1H";
    time_t currentTime = std::time(nullptr); // get the current time
    if (!buf.status.empty() && (currentTime - buf.statusTime) < 3) {
        std::cout << buf.status; // status message will be displayed for 3 seconds
    } else {
        std::cout << "Ctrl+S to save | Ctrl+Q to quit"; // default status message
    }

    std::cout << "\x1b[" << buf.cursor.row - buf.rowOffset + 2 << ";" << buf.cursor.col + gutterWidth + 3 + 1 << "H";

    std::cout << std::flush;   // send everything to the terminal NOW
}
