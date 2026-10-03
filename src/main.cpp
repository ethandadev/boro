//src/main.cpp
#include <iostream> //main library to import
#include <string> //for string
#include <vector> //for vector
#include <iomanip> //for std::setw
#include <unistd.h> //for STDIN_FILENO
#include "buffer.hpp" //include the buffer header file
#include "terminal.hpp" //include the terminal header file


//main function, the entry point of the program
int main(int argc, char* argv[]) {

    if (argc < 2) { // argc is the number of arguments passed to the program, if less than 2, then no file name was provided
        std::cerr << "Usage: boro <filename>\n"; // cerr is used to print error messages to the console
        return 1; // return 1 indicates that the program has encountered an error
    }


    std::string fileName = argv[1]; // argv is an array of strings, argv[0] is the program name, argv[1] is the first argument passed to the program
    Buffer buf;
    if (!loadFile(fileName, buf.lines)) { // if loadFile returns false, then there was an error opening the file
        return 1;
    }


    enableRawMode(); // call the enableRawMode function to enable raw mode

    while (true) { // infinite loop to keep the program running until the user exits
        render(buf); // call the render function to display the buffer contents

        char c;
        read(STDIN_FILENO, &c, 1);    // wait for 1 key, store it in c


        if (int(c) == 17) {
            break; // exit the loop if Ctrl+Q is pressed
        } else if (int(c) == 127) { // delete key
            deleteChar(buf); // call the deleteChar function to delete a character
        } else if (int(c) == 10) { // enter key
            insertNewLine(buf); // call the insertNewLine function to insert a new line
        } else if (c >= 32 && c <= 126) { // printable characters
            insertChar(buf, c); // call the insertChar function to insert a character
        } else if (int(c) == 19) { // Ctrl+S
            if (!saveFile(fileName, buf.lines)) { // if saveFile returns false, then there was an error saving the file
                std::cerr << "Error: Could not save file " << fileName << "\n"; // print an error message to the console
            }
        }
    }
    disableRawMode(); // call the disableRawMode function to disable raw mode
    
    return 0; // return 0 indicates that the program has completed successfully
}