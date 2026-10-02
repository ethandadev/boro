#include <iostream> //main library to import
#include <string> //for string
#include <vector> //for vector
#include <fstream> //for file input/output

//main function, the entry point of the program
int main() {
    std::cout << "Hello from boro\n\n"; // cout is used to print to the console

    std::string fileName = "src/main.cpp"; 
    std::vector<std::string> lines; // vector is like a list or array

    //load file
    std::ifstream file(fileName);
    std::string line;
    while (std::getline(file, line)) { // while loop to read each line of the file until the end
        lines.push_back(line); // add the line to the vector
    }
    
    size_t lineCount = lines.size(); // size_t is an unsigned integer type used for sizes


    std::cout << "Opening " << fileName << " (" << lineCount << " lines)" << "\n";
    

    for (size_t  i = 0; i < lineCount; i++) { // for loop to iterate through the lines, start, keep going while, after each round
        std::cout << i + 1 << " | " << lines[i] << "\n";
    }

    return 0; // return 0 indicates that the program has completed successfully
}