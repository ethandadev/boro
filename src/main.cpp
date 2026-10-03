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

    // render(buf); // call the render function to display the buffer contents
    // std::cin.get(); // wait for user input before exiting

    enableRawMode(); // call the enableRawMode function to enable raw mode

    while (true) { // infinite loop to keep the program running until the user exits
        char c;
        read(STDIN_FILENO, &c, 1);    // wait for 1 key, store it in c

        std::cout << int(c) << "\n"; // print the integer value of the character pressed

        if (c == 'q') {
            break;
        }
    }
    disableRawMode(); // call the disableRawMode function to disable raw mode
    
    return 0; // return 0 indicates that the program has completed successfully
}