#include <scope.hpp>
#include <error.hpp>
#include <func.hpp>

Scope::Scope(Scope* parent_scope) : parent_scope_(parent_scope) {
    if (parent_scope_) {
        AddDependency(parent_scope_);
    }
}

void Scope::PutFunction(std::string name, Function* func) {
    if (Is<ObjectHolder>(func)) {
        As<ObjectHolder>(func)->SetName(name);
    }

    if (auto it = scope_.find(name); it != scope_.end()) {
        RemoveDependency(it->second);
    }

    scope_[std::move(name)] = func;
    AddDependency(func);
}

void Scope::SetFunction(const std::string& name, Function* func) {
    if (auto it = scope_.find(name); it != scope_.end()) {
        RemoveDependency(it->second);
        it->second = func;
        AddDependency(func);
        return;
    }
    if (parent_scope_) {
        parent_scope_->SetFunction(name, func);
        return;
    }
    throw NameError("Invalid name: " + name);
}

Function* Scope::GetFunction(const std::string& name) const {
    if (auto it = scope_.find(name); it != scope_.end()) {
        return it->second;
    }
    if (parent_scope_) {
        return parent_scope_->GetFunction(name);
    }
    throw NameError("Invalid name: " + name);
}

Scope::Iterator Scope::begin() {
    return scope_.begin();
}

Scope::Iterator Scope::end() {
    return scope_.end();
}
