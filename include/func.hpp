#pragma once

#include <scope.hpp>
#include <object.hpp>

Object* Process(Object* obj, Scope* scope);

class FunctionArgs final {
public:
    using Type = std::vector<Object*>;
    using Iterator = Type::iterator;

    FunctionArgs(Iterator begin, Iterator end);

    Object* operator[](size_t ind) const;
    Object* Back() const;
    size_t Size() const;

    void SkipLast();

    Iterator begin() const;  // NOLINT
    Iterator end() const;    // NOLINT

    template <typename T>
    bool AreExpectedType() const {
        return std::all_of(begin_, end_, Is<T>);
    }

private:
    Iterator begin_;
    Iterator end_;
};

class Function : public Object {
public:
    virtual Function* Execute(FunctionArgs args, Scope* scope) = 0;
};

class ObjectHolder : public Function {
public:
    ObjectHolder(Object* object, Scope* scope = nullptr);
    Function* Execute(FunctionArgs args, Scope* scope) override;

    Scope* GetScope() const;
    Object* GetObject() const;
    const std::string& GetName() const;

    void SetObject(Object* object);
    void SetName(std::string name);

private:
    Object* object_;
    Scope* scope_;
    std::string name_;
};

class IsBoolean : public Function {
public:
    Function* Execute(FunctionArgs args, Scope* scope) override;
};

class Not : public Function {
public:
    Function* Execute(FunctionArgs args, Scope* scope) override;
};

class And : public Function {
public:
    Function* Execute(FunctionArgs args, Scope* scope) override;
};

class Or : public Function {
public:
    Function* Execute(FunctionArgs args, Scope* scope) override;
};

class IsNumber : public Function {
public:
    Function* Execute(FunctionArgs args, Scope* scope) override;
};

class Equal : public Function {
public:
    Function* Execute(FunctionArgs args, Scope* scope) override;
};

class MonotonicallyIncreasing : public Function {
public:
    Function* Execute(FunctionArgs args, Scope* scope) override;
};

class MonotonicallyDecreasing : public Function {
public:
    Function* Execute(FunctionArgs args, Scope* scope) override;
};

class MonotonicallyNonIncreasing : public Function {
public:
    Function* Execute(FunctionArgs args, Scope* scope) override;
};

class MonotonicallyNonDecreasing : public Function {
public:
    Function* Execute(FunctionArgs args, Scope* scope) override;
};

class Plus : public Function {
public:
    Function* Execute(FunctionArgs args, Scope* scope) override;
};

class Minus : public Function {
public:
    Function* Execute(FunctionArgs args, Scope* scope) override;
};

class Multiply : public Function {
public:
    Function* Execute(FunctionArgs args, Scope* scope) override;
};

class Divide : public Function {
public:
    Function* Execute(FunctionArgs args, Scope* scope) override;
};

class Quote : public Function {
public:
    Function* Execute(FunctionArgs args, Scope* scope) override;
};

class Max : public Function {
public:
    Function* Execute(FunctionArgs args, Scope* scope) override;
};

class Min : public Function {
public:
    Function* Execute(FunctionArgs args, Scope* scope) override;
};

class Abs : public Function {
public:
    Function* Execute(FunctionArgs args, Scope* scope) override;
};

class IsPair : public Function {
public:
    Function* Execute(FunctionArgs args, Scope* scope) override;
};

class IsNull : public Function {
public:
    Function* Execute(FunctionArgs args, Scope* scope) override;
};

class IsList : public Function {
public:
    Function* Execute(FunctionArgs args, Scope* scope) override;
};

class Cons : public Function {
public:
    Function* Execute(FunctionArgs args, Scope* scope) override;
};

class Car : public Function {
public:
    Function* Execute(FunctionArgs args, Scope* scope) override;
};

class Cdr : public Function {
public:
    Function* Execute(FunctionArgs args, Scope* scope) override;
};

class List : public Function {
public:
    Function* Execute(FunctionArgs args, Scope* scope) override;
};

class ListRef : public Function {
public:
    Function* Execute(FunctionArgs args, Scope* scope) override;
};

class ListTail : public Function {
public:
    Function* Execute(FunctionArgs args, Scope* scope) override;
};

class IsSymbol : public Function {
public:
    Function* Execute(FunctionArgs args, Scope* scope) override;
};

class Define : public Function {
public:
    Function* Execute(FunctionArgs args, Scope* scope) override;
};

class Set : public Function {
public:
    Function* Execute(FunctionArgs args, Scope* scope) override;
};

class SetCar : public Function {
public:
    Function* Execute(FunctionArgs args, Scope* scope) override;
};

class SetCdr : public Function {
public:
    Function* Execute(FunctionArgs args, Scope* scope) override;
};

class If : public Function {
public:
    Function* Execute(FunctionArgs args, Scope* scope) override;
};

class CreateLambda : public Function {
public:
    Function* Execute(FunctionArgs args, Scope* scope) override;
};

class Lambda : public Function {
public:
    Lambda(std::vector<Object*> args, std::vector<Object*> body, Scope* parent_scope);
    Function* Execute(FunctionArgs args, Scope* scope) override;

private:
    std::vector<Object*> args_;
    std::vector<Object*> body_;
    Scope* parent_scope_;
};
