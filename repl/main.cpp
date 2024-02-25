#include <scheme.hpp>
#include <iostream>
#include <exception>

int main() {
    try {
        std::string query;
        std::getline(std::cin, query);
        Interpreter interpreter{};
        auto result = interpreter.Run(query);
        std::cout << result << std::endl;

    } catch (const std::exception& ex) {
        std::cout << ex.what() << "\n";
    }
}
