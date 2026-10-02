#include <iostream>

int main() {
    std::cout << "Hello from boro\n";
    std::string fileName = "notes.txt";
    int lineCount = 42;
    std::cout << "Opening " << fileName << " (" << lineCount << " lines)" << std::endl;
    return 0;
}