#include <iostream> //main library to import
#include <string> //for string
#include <vector> //for vector


int main() {
    std::cout << "Hello from boro\n\n"; // cout is used to print to the console
    std::string fileName = "notes.txt"; 
    std::vector<std::string> lines = {"#include <iostream>", "", "int main() {", "    return 0;", "}"};
    size_t lineCount = lines.size(); // size_t is an unsigned integer type used for sizes
    std::cout << "Opening " << fileName << " (" << lineCount << " lines)" << "\n";

    for (size_t  i = 0; i < lineCount; i++) { // for loop to iterate through the lines, start, keep going while, after each round
        std::cout << i + 1 << " | " << lines[i] << "\n";
    }

    return 0; // return 0 indicates that the program has completed successfully
}