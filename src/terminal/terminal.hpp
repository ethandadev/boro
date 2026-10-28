// src/terminal/terminal.hpp
// header file for terminal
#pragma once
#include "buffer.hpp"
#include <ctime>
#include <string>

// terminal view handles the view state of the terminal view
struct TerminalView {
    // terminal view for the terminal thingy mabob
    size_t rowOffset = 0;
    std::string status;
    std::time_t statusTime = 0;
};

void render(const Buffer& buf, const TerminalView& view); // render the terminal

void enableRawMode(); // enable raw mode in the terminal

void disableRawMode(); // discard raw mode in the terminal

size_t getScreenRows(); // amount of visible screen rows

void scroll(TerminalView& view, const Buffer& buf, size_t textRows); // declaration only

void setStatus(TerminalView& view, const std::string& msg); // declaration only

