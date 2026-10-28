// src/config.hpp
// a configuration file for the Boro text editor, containing constants for the application name and version

// yes

#pragma once
#include <string>

inline const std::string APP_NAME = "Boro";
inline const std::string VERSION  = "0.3.0"; // someone remind me to update this every version
inline const size_t      TAB_WIDTH = 4; // normal tab width in spaces

// editor settings
inline const double SCROLL_PAST_END = 0.8; // extra space below the last line, as a fraction of the window height
inline const size_t MAX_UNDO = 1000; // most undo in letters 


// GUI font settings
inline const std::string FONT_FAMILY   = "JetBrains Mono"; //this font is nice
inline const std::string FALLBACK_FONT = "Menlo"; // if the nice font dont work use the worse font.
inline const int         FONT_SIZE     = 14; // perfect(i think)
