// src/terminal.hpp
#pragma once
#include "buffer.hpp"
#include <ctime>
#include <string>

// terminal view handles the view state of the terminal view
struct TerminalView {
    size_t rowOffset = 0;
    std::string status;
    std::time_t statusTime = 0;
};

void render(const Buffer& buf, const TerminalView& view);

void enableRawMode();

void disableRawMode();

size_t getScreenRows();

void scroll(TerminalView& view, const Buffer& buf, size_t textRows); // declaration only

void setStatus(TerminalView& view, const std::string& msg); // declaration only

