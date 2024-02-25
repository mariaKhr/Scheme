#include <error.hpp>

void ThrowSyntaxErrorIf(bool condition, std::string_view msg) {
    if (condition) {
        throw SyntaxError(msg.data());
    }
}

void ThrowRuntimeErrorIf(bool condition, std::string_view msg) {
    if (condition) {
        throw RuntimeError(msg.data());
    }
}

void ThrowNameErrorIf(bool condition, std::string_view msg) {
    if (condition) {
        throw NameError(msg.data());
    }
}
