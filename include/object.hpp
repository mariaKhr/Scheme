#pragma once

#include <unordered_set>
#include <vector>

class Object {
public:
    Object() = default;

    Object(const Object&) = delete;
    Object& operator=(const Object&) = delete;

    Object(Object&&) = default;
    Object& operator=(Object&&) = default;

    virtual ~Object() = default;

protected:
    void Mark();
    void Unmark();

    void AddDependency(Object* dependency);
    void RemoveDependency(Object* dependency);

protected:
    std::unordered_set<Object*> dependencies_;
    bool marked_{false};

    friend class Heap;
};

class Number : public Object {
public:
    Number(int32_t value);
    int32_t GetValue() const;

private:
    int32_t value_;
};

class Symbol : public Object {
public:
    Symbol(std::string symbol);
    const std::string& GetName() const;

private:
    std::string symbol_;
};

class Cell : public Object {
public:
    Cell(Object* first, Object* second);
    Object* GetFirst() const;
    Object* GetSecond() const;

private:
    Object* first_;
    Object* second_;
};

std::string Serialize(Object* obj);

std::vector<Object*> ObjectToVector(Object* obj);
Object* VectorToObject(const std::vector<Object*>& vector);

///////////////////////////////////////////////////////////////////////////////

// Runtime type checking and convertion.

template <class T>
T* As(Object* obj) {
    return dynamic_cast<T*>(obj);
}

template <class T>
bool Is(Object* obj) {
    if (obj) {
        return typeid(T) == typeid(*obj);
    }
    return false;
}
