#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include "lexer.hpp"
#include "parser.hpp"

namespace {

std::string readInputFromFile(const std::string& filePath) {
    std::ifstream inputFile(filePath);
    if (!inputFile) {
        throw std::runtime_error("Cannot open input file: " + filePath);
    }
    std::ostringstream buffer;
    buffer << inputFile.rdbuf();
    return buffer.str();
}

std::string readInputFromStdin() {
    std::ostringstream buffer;
    buffer << std::cin.rdbuf();
    return buffer.str();
}

}  // namespace

int main(int argc, char* argv[]) {
    try {
        std::string input;
        if (argc == 2) {
            input = readInputFromFile(argv[1]);
        } else if (argc == 1) {
            input = readInputFromStdin();
        } else {
            std::cerr << "Usage: " << argv[0] << " [input_file]\n";
            return 2;
        }

        Lexer lexer(input);
        Parser parser(lexer);
        const bool ok = parser.parse(true);

        std::cout << (ok ? "Result: ACCEPT" : "Result: ERROR") << '\n';
        return ok ? 0 : 1;
    } catch (const std::exception& ex) {
        std::cerr << "Runtime error: " << ex.what() << '\n';
        return 2;
    }
}
