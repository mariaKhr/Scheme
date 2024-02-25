#pragma once

#include <object.hpp>
#include <unordered_map>

class Function;

class Scope final : public Object {
public:
    using UnorderedMap = std::unordered_map<std::string, Function*>;
    using Iterator = UnorderedMap::iterator;

    Scope(Scope* parent_scope = nullptr);

    void PutFunction(std::string name, Function* func);
    void SetFunction(const std::string& name, Function* func);
    Function* GetFunction(const std::string& name) const;

    Iterator begin();  // NOLINT
    Iterator end();    // NOLINT

private:
    Scope* parent_scope_;
    UnorderedMap scope_;
};
