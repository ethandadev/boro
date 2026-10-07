#CMakeLists.txt

# The oldest CMake version this file works with. If someone's CMake is older,
# it stops with a clear error instead of failing in a confusing way.
cmake_minimum_required(VERSION 3.21)

# Names the project and says it's written in C++ ("CXX" is CMake's name for C++).
project(boro_gui LANGUAGES CXX)

# Use C++20 — same as -std=c++20 in your Makefile.
set(CMAKE_CXX_STANDARD 20)

# NEW: write build/compile_commands.json so clangd knows where Qt's headers are
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find the installed Qt 6 and load its Widgets module.
# REQUIRED = stop with an error if Qt can't be found.
# This is how CMake learns where Qt's headers and libraries live on your Mac.
find_package(Qt6 REQUIRED COMPONENTS Widgets)

# Turns on Qt's standard project settings, including tools that
# automatically generate extra code Qt needs behind the scenes
# (you'll need this once you write your own widget classes).
qt_standard_project_setup()

# Create a program called "boro-gui" built from these source files.
# CHANGED: new paths, and buffer.cpp is shared with the terminal version.
# When you add more .cpp files later, they get listed here.
qt_add_executable(boro-gui MACOSX_BUNDLE
    src/gui/main.cpp
    src/gui/editor.cpp
    src/core/buffer.cpp
)

set_target_properties(boro-gui PROPERTIES
    OUTPUT_NAME "Boro"                                  # the file becomes Boro.app
    MACOSX_BUNDLE_BUNDLE_NAME "Boro"                    # the name in the menu bar
    MACOSX_BUNDLE_GUI_IDENTIFIER "com.ethandadev.boro"  # same ID as your pkg
    MACOSX_BUNDLE_SHORT_VERSION_STRING "0.2.0"          # version shown in "About"
)

qt_add_resources(boro-gui "fonts"
    PREFIX "/"
    FILES resources/fonts/JetBrainsMono-Regular.ttf
)

# NEW: let gui files write #include "buffer.hpp" even though it lives in src/core/
target_include_directories(boro-gui PRIVATE src/core)


# Link the Qt Widgets library into boro-gui, so the compiler finds
# <QApplication>/<QWidget> and the linker finds their actual code.
# PRIVATE = only boro-gui itself needs this (matters for bigger projects).
target_link_libraries(boro-gui PRIVATE Qt6::Widgets)