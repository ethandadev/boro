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
    TerminalView view;
    if (!loadFile(buf, fileName)) { // if loadFile returns false, then there was an error opening the file
        return 1;
    }

    if (buf.isNew) {
        setStatus(view, "New File: " + fileName);
    }
    enableRawMode(); // call the enableRawMode function to enable raw mode

    while (true) { // infinite loop to keep the program running until the user exits

        size_t textRows = getScreenRows() - 2; // get the number of rows in the terminal, minus 2 for the status bar and title bar
        scroll(view, buf, textRows); // call the scroll function to update the row offset

        render(buf, view); // call the render function to display the buffer contents

        char c;
        ssize_t n = read(STDIN_FILENO, &c, 1);    // wait for 1 key, store it in c

        if (n == 0) {
            continue; // if no key was pressed, continue the loop
        } else if (n == -1) {
            std::cerr << "Error reading input\n"; // if there was an error reading input, print an error message
            break; // exit the loop
        }

        if (int(c) == 17) {
            break; // exit the loop if Ctrl+Q is pressed
        } else if (int(c) == 127) { // delete key
            deleteChar(buf); // call the deleteChar function to delete a character
        } else if (int(c) == 10) { // enter key
            insertNewLine(buf); // call the insertNewLine function to insert a new line
        } else if (c >= 32 && c <= 126) { // printable characters
            insertChar(buf, c); // call the insertChar function to insert a character
        } else if (int(c) == 19) { // Ctrl+S
            if (!saveFile(buf, fileName)) { // if saveFile returns false, then there was an error saving the file
                setStatus(view, "Error: Could not save file " + fileName); // set the status message
            } else {
                setStatus(view, "Saved " + std::to_string(buf.lines.size()) + " lines to " + fileName); // set the status message
            }
        } else if (int(c) == 27) { // escape sequence for arrow keys
            char a;
            char b;
            ssize_t na = read(STDIN_FILENO, &a, 1); // read the next character
            ssize_t nb = read(STDIN_FILENO, &b, 1); // read the next character
            if (na == 0 || nb == 0) {
                continue; // if no key was pressed, continue the loop
            } else if (a == '[') { // if the first character is '[', then it is an arrow key
                if (b == 'A') { // up arrow
                    moveUp(buf); // call the moveUp function to move the cursor up
                } else if (b == 'B') { // down arrow
                    moveDown(buf); // call the moveDown function to move the cursor down
                } else if (b == 'C') { // right arrow
                    moveRight(buf); // call the moveRight function to move the cursor right
                } else if (b == 'D') { // left arrow
                    moveLeft(buf); // call the moveLeft function to move the cursor left
                }
            }
        } else if (int(c) == 9) {
            insertTab(buf);
        }

    }
    disableRawMode(); // call the disableRawMode function to disable raw mode
    
    return 0; // return 0 indicates that the program has completed successfully
}
