// src/config.hpp
// a configuration file for the Boro text editor, containing constants for the application name and version

#pragma once
#include <string>

inline const std::string APP_NAME = "Boro";
inline const std::string VERSION  = "0.3.0";
inline const size_t TAB_WIDTH = 4;

// editor settings
inline const double SCROLL_PAST_END = 0.8; // extra space below the last line, as a fraction of the window height
inline const size_t MAX_UNDO = 1000;


// GUI font settings
inline const std::string FONT_FAMILY   = "JetBrains Mono";
inline const std::string FALLBACK_FONT = "Menlo";
inline const int         FONT_SIZE     = 14;
