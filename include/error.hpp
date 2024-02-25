#pragma once

#include <stdexcept>

struct SyntaxError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

struct RuntimeError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

struct NameError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

void ThrowSyntaxErrorIf(bool condition, std::string_view msg = "");

void ThrowRuntimeErrorIf(bool condition, std::string_view msg = "");

void ThrowNameErrorIf(bool condition, std::string_view msg = "");
