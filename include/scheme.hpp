#pragma once

#include <memory>

class Scope;

class Interpreter final {
public:
    Interpreter();
    std::string Run(const std::string&);

    ~Interpreter();

private:
    std::unique_ptr<Scope> global_scope_;
};
