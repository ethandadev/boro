#include <iostream>
#include <string>
#include <vector>

int main() {
    std::cout << "Hello from boro\n\n";
    std::string fileName = "notes.txt";
    std::vector<std::string> lines = {"#include <iostream>", "", "int main() {", "    return 0;", "}"};
    size_t lineCount = lines.size();
    std::cout << "Opening " << fileName << " (" << lineCount << " lines)" << "\n";

    for (size_t  i = 0; i < lineCount; i++) {
        std::cout << i + 1 << " | " << lines[i] << "\n";
    }

    return 0;
}