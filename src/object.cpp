#include <object.hpp>
#include <garbage_collection.hpp>

void Object::Mark() {
    if (marked_) {
        return;
    }
    marked_ = true;
    for (auto dependency : dependencies_) {
        dependency->Mark();
    }
}

void Object::Unmark() {
    if (!marked_) {
        return;
    }
    marked_ = false;
    for (auto dependency : dependencies_) {
        dependency->Unmark();
    }
}

void Object::AddDependency(Object* dependency) {
    if (dependency) {
        dependencies_.insert(dependency);
    }
}

void Object::RemoveDependency(Object* dependency) {
    if (dependency) {
        dependencies_.erase(dependency);
    }
}

Number::Number(int32_t value) : value_(value) {
}

int32_t Number::GetValue() const {
    return value_;
}

Symbol::Symbol(std::string symbol) : symbol_(std::move(symbol)) {
}

const std::string& Symbol::GetName() const {
    return symbol_;
}

Cell::Cell(Object* first, Object* second) : first_(first), second_(second) {
    AddDependency(first_);
    AddDependency(second_);
}

Object* Cell::GetFirst() const {
    return first_;
}

Object* Cell::GetSecond() const {
    return second_;
}

std::string Serialize(Object* obj) {
    if (obj == nullptr) {
        return "()";
    }
    if (Is<Number>(obj)) {
        return std::to_string(As<Number>(obj)->GetValue());
    }
    if (Is<Symbol>(obj)) {
        return As<Symbol>(obj)->GetName();
    }

    auto first = As<Cell>(obj)->GetFirst();
    auto second = As<Cell>(obj)->GetSecond();
    auto s1 = Serialize(first);
    auto s2 = Serialize(second);

    if (second != nullptr && !Is<Cell>(second)) {
        return "(" + std::move(s1) + " . " + std::move(s2) + ")";
    }
    if (second == nullptr) {
        return "(" + std::move(s1) + ")";
    }
    s2.pop_back();
    s2.erase(0, 1);
    return "(" + std::move(s1) + " " + std::move(s2) + ")";
}

std::vector<Object*> ObjectToVector(Object* obj) {
    std::vector<Object*> vector{};
    while (Is<Cell>(obj)) {
        vector.push_back(As<Cell>(obj)->GetFirst());
        obj = As<Cell>(obj)->GetSecond();
    }
    vector.push_back(obj);
    return vector;
}

Object* VectorToObject(const std::vector<Object*>& vector) {
    if (vector.empty()) {
        return nullptr;
    }
    if (vector.size() == 1) {
        return vector[0];
    }
    auto obj = Heap::Instance().Make<Cell>(vector[vector.size() - 2], vector.back());
    for (auto i = static_cast<ssize_t>(vector.size()) - 3; i >= 0; --i) {
        obj = Heap::Instance().Make<Cell>(vector[i], obj);
    }
    return obj;
}
