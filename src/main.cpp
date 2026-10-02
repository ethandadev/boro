//main.cpp
#include <iostream> //main library to import
#include <string> //for string
#include <vector> //for vector
#include <iomanip> //for std::setw
#include "buffer.hpp" //include the buffer header file


//main function, the entry point of the program
int main(int argc, char* argv[]) {
    std::cout << "Hello from boro\n\n"; // cout is used to print to the console

    if (argc < 2) { // argc is the number of arguments passed to the program, if less than 2, then no file name was provided
        std::cerr << "Usage: boro <filename>\n"; // cerr is used to print error messages to the console
        return 1; // return 1 indicates that the program has encountered an error
    }


    std::string fileName = argv[1]; // argv is an array of strings, argv[0] is the program name, argv[1] is the first argument passed to the program
    Buffer buf;
    if (!loadFile(fileName, buf.lines)) { // if loadFile returns false, then there was an error opening the file
        return 1;
    }
    size_t lineCount = buf.lines.size(); // size_t is an unsigned integer type used for sizes

    size_t gutterWidth = std::to_string(lineCount).size(); // variable to store the highest digit in the line numbers



    std::cout << "Opening " << fileName << " (" << lineCount << " lines)" << "\n";


    for (size_t  i = 0; i < lineCount; i++) { // for loop to iterate through the lines, start, keep going while, after each round
        std::cout << std::setw(gutterWidth) << i + 1 << " | " << buf.lines[i] << "\n"; //std::setw is used to set the width of the output, i + 1 is used to display the line number starting from 1 instead of 0
    }

    std::cout << "Cursor position: (" << buf.cursor.row << ", " << buf.cursor.col << ")\n";

    return 0; // return 0 indicates that the program has completed successfully
}