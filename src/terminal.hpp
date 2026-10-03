// src/terminal.hpp
#pragma once
#include "buffer.hpp"

void render(const Buffer& buf);

void enableRawMode();

void disableRawMode();