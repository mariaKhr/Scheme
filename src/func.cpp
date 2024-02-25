#include <func.hpp>
#include <error.hpp>
#include <garbage_collection.hpp>
#include <scope.hpp>

namespace {
Function* ExtractFunctionAndExecute(Object* obj, Scope* scope);

Function* ExtractFunction(Object* obj, Scope* scope) {
    if (Is<Number>(obj)) {
        return As<Function>(Heap::Instance().Make<ObjectHolder>(obj, scope));

    } else if (Is<Cell>(obj)) {
        return ExtractFunctionAndExecute(obj, scope);

    } else if (Is<Symbol>(obj)) {
        const auto& name = As<Symbol>(obj)->GetName();
        return scope->GetFunction(name);
    }
    throw RuntimeError("Unexpected function");
}

Object* ExtractResult(Function* func) {
    if (!func) {
        return nullptr;
    }
    if (Is<ObjectHolder>(func)) {
        return As<ObjectHolder>(func)->GetObject();
    }
    throw RuntimeError("Unexpected result");
}

void ProcessArgs(FunctionArgs& args, Scope* scope) {
    for (auto& arg : args) {
        arg = Process(arg, scope);
    }
}

Function* ExtractFunctionAndExecute(Object* obj, Scope* scope) {
    auto vector_args = ObjectToVector(obj);
    auto func = ExtractFunction(vector_args[0], scope);

    auto args_begin = vector_args.begin() + 1;
    auto args_end = vector_args.end();
    auto func_args = FunctionArgs(args_begin, args_end);

    auto res = As<Function>(func)->Execute(std::move(func_args), scope);
    return As<Function>(res);
}

Function* GetTrueFunction(Scope* scope) {
    return scope->GetFunction("#t");
}

Function* GetFalseFunction(Scope* scope) {
    return scope->GetFunction("#f");
}
}  // namespace

Object* Process(Object* obj, Scope* scope) {
    auto res = ExtractFunctionAndExecute(obj, scope);
    return ExtractResult(res);
}

FunctionArgs::FunctionArgs(Iterator begin, Iterator end) : begin_(begin), end_(end) {
}

Object* FunctionArgs::operator[](size_t ind) const {
    return begin_[ind];
}

Object* FunctionArgs::Back() const {
    return *(end_ - 1);
}

size_t FunctionArgs::Size() const {
    return end_ - begin_;
}

void FunctionArgs::SkipLast() {
    if (end_ != begin_) {
        --end_;
    }
}

FunctionArgs::Iterator FunctionArgs::begin() const {
    return begin_;
}

FunctionArgs::Iterator FunctionArgs::end() const {
    return end_;
}

ObjectHolder::ObjectHolder(Object* object, Scope* scope) : object_(object), scope_(scope) {
    AddDependency(object_);
    AddDependency(scope_);
}

Function* ObjectHolder::Execute(FunctionArgs args, Scope*) {
    ThrowRuntimeErrorIf(args.Size() != 0, "ObjectHolder: no arguments expected");
    return this;
}

Scope* ObjectHolder::GetScope() const {
    return scope_;
}

Object* ObjectHolder::GetObject() const {
    return object_;
}

const std::string& ObjectHolder::GetName() const {
    return name_;
}

void ObjectHolder::SetObject(Object* object) {
    RemoveDependency(object_);
    object_ = object;
    AddDependency(object_);
}

void ObjectHolder::SetName(std::string name) {
    name_ = std::move(name);
}

Function* IsBoolean::Execute(FunctionArgs args, Scope* scope) {
    args.SkipLast();
    ThrowRuntimeErrorIf(args.Size() != 1, "boolean?: expected 1 argument");
    ProcessArgs(args, scope);

    if (Is<Symbol>(args[0])) {
        const auto& name = As<Symbol>(args[0])->GetName();
        if (name == "#t" || name == "#f") {
            return GetTrueFunction(scope);
        }
    }
    return GetFalseFunction(scope);
}

Function* Not::Execute(FunctionArgs args, Scope* scope) {
    args.SkipLast();
    ThrowRuntimeErrorIf(args.Size() != 1, "not: expected 1 argument");
    ProcessArgs(args, scope);

    if (Is<Symbol>(args[0]) && As<Symbol>(args[0])->GetName() == "#f") {
        return GetTrueFunction(scope);
    }

    return GetFalseFunction(scope);
}

Function* And::Execute(FunctionArgs args, Scope* scope) {
    args.SkipLast();

    for (auto& arg : args) {
        arg = Process(arg, scope);
        if (Is<Symbol>(arg) && As<Symbol>(arg)->GetName() == "#f") {
            return GetFalseFunction(scope);
        }
    }

    if (args.Size() == 0) {
        return GetTrueFunction(scope);
    }

    return As<Function>(Heap::Instance().Make<ObjectHolder>(args.Back(), scope));
}

Function* Or::Execute(FunctionArgs args, Scope* scope) {
    args.SkipLast();

    for (auto& arg : args) {
        arg = Process(arg, scope);
        if (!(Is<Symbol>(arg) && As<Symbol>(arg)->GetName() == "#f")) {
            return As<Function>(Heap::Instance().Make<ObjectHolder>(arg, scope));
        }
    }

    return GetFalseFunction(scope);
}

Function* IsNumber::Execute(FunctionArgs args, Scope* scope) {
    args.SkipLast();
    ThrowRuntimeErrorIf(args.Size() != 1, "number?: expected 1 argument");
    ProcessArgs(args, scope);

    if (Is<Number>(args[0])) {
        return GetTrueFunction(scope);
    }

    return GetFalseFunction(scope);
}

Function* Equal::Execute(FunctionArgs args, Scope* scope) {
    args.SkipLast();
    ProcessArgs(args, scope);
    ThrowRuntimeErrorIf(!args.AreExpectedType<Number>());

    for (size_t i = 0, size = args.Size(); i + 1 < size; ++i) {
        auto cur = As<Number>(args[i]);
        auto next = As<Number>(args[i + 1]);
        if (cur->GetValue() != next->GetValue()) {
            return GetFalseFunction(scope);
        }
    }

    return GetTrueFunction(scope);
}

Function* MonotonicallyIncreasing::Execute(FunctionArgs args, Scope* scope) {
    args.SkipLast();
    ProcessArgs(args, scope);
    ThrowRuntimeErrorIf(!args.AreExpectedType<Number>());

    for (size_t i = 0, size = args.Size(); i + 1 < size; ++i) {
        auto cur = As<Number>(args[i]);
        auto next = As<Number>(args[i + 1]);
        if (cur->GetValue() >= next->GetValue()) {
            return GetFalseFunction(scope);
        }
    }

    return GetTrueFunction(scope);
}

Function* MonotonicallyDecreasing::Execute(FunctionArgs args, Scope* scope) {
    args.SkipLast();
    ProcessArgs(args, scope);
    ThrowRuntimeErrorIf(!args.AreExpectedType<Number>());

    for (size_t i = 0, size = args.Size(); i + 1 < size; ++i) {
        auto cur = As<Number>(args[i]);
        auto next = As<Number>(args[i + 1]);
        if (cur->GetValue() <= next->GetValue()) {
            return GetFalseFunction(scope);
        }
    }

    return GetTrueFunction(scope);
}

Function* MonotonicallyNonIncreasing::Execute(FunctionArgs args, Scope* scope) {
    args.SkipLast();
    ProcessArgs(args, scope);
    ThrowRuntimeErrorIf(!args.AreExpectedType<Number>());

    for (size_t i = 0, size = args.Size(); i + 1 < size; ++i) {
        auto cur = As<Number>(args[i]);
        auto next = As<Number>(args[i + 1]);
        if (cur->GetValue() < next->GetValue()) {
            return GetFalseFunction(scope);
        }
    }

    return GetTrueFunction(scope);
}

Function* MonotonicallyNonDecreasing::Execute(FunctionArgs args, Scope* scope) {
    args.SkipLast();
    ProcessArgs(args, scope);
    ThrowRuntimeErrorIf(!args.AreExpectedType<Number>());

    for (size_t i = 0, size = args.Size(); i + 1 < size; ++i) {
        auto cur = As<Number>(args[i]);
        auto next = As<Number>(args[i + 1]);
        if (cur->GetValue() > next->GetValue()) {
            return GetFalseFunction(scope);
        }
    }

    return GetTrueFunction(scope);
}

Function* Plus::Execute(FunctionArgs args, Scope* scope) {
    args.SkipLast();
    ProcessArgs(args, scope);
    ThrowRuntimeErrorIf(!args.AreExpectedType<Number>());

    int32_t sum = 0;
    for (const auto& arg : args) {
        sum += As<Number>(arg)->GetValue();
    }

    return As<Function>(Heap::Instance().Make<ObjectHolder>(Heap::Instance().Make<Number>(sum)));
}

Function* Minus::Execute(FunctionArgs args, Scope* scope) {
    args.SkipLast();
    ThrowRuntimeErrorIf(args.Size() == 0, "-: expected >= 1 argument");
    ProcessArgs(args, scope);
    ThrowRuntimeErrorIf(!args.AreExpectedType<Number>());

    int32_t result = As<Number>(args[0])->GetValue();
    for (size_t i = 1, size = args.Size(); i < size; ++i) {
        result -= As<Number>(args[i])->GetValue();
    }

    return As<Function>(Heap::Instance().Make<ObjectHolder>(Heap::Instance().Make<Number>(result)));
}

Function* Multiply::Execute(FunctionArgs args, Scope* scope) {
    args.SkipLast();
    ProcessArgs(args, scope);
    ThrowRuntimeErrorIf(!args.AreExpectedType<Number>());

    int32_t prod = 1;
    for (const auto& arg : args) {
        prod *= As<Number>(arg)->GetValue();
    }

    return As<Function>(Heap::Instance().Make<ObjectHolder>(Heap::Instance().Make<Number>(prod)));
}

Function* Divide::Execute(FunctionArgs args, Scope* scope) {
    args.SkipLast();
    ThrowRuntimeErrorIf(args.Size() == 0, "/: expected >= 1 argument");
    ProcessArgs(args, scope);
    ThrowRuntimeErrorIf(!args.AreExpectedType<Number>());

    int32_t result = As<Number>(args[0])->GetValue();
    for (size_t i = 1, size = args.Size(); i < size; ++i) {
        result /= As<Number>(args[i])->GetValue();
    }

    return As<Function>(Heap::Instance().Make<ObjectHolder>(Heap::Instance().Make<Number>(result)));
}

Function* Max::Execute(FunctionArgs args, Scope* scope) {
    args.SkipLast();
    ThrowRuntimeErrorIf(args.Size() == 0, "max: expected >= 1 argument");
    ProcessArgs(args, scope);
    ThrowRuntimeErrorIf(!args.AreExpectedType<Number>());

    int32_t max = As<Number>(args[0])->GetValue();
    for (const auto& arg : args) {
        max = std::max(max, As<Number>(arg)->GetValue());
    }

    return As<Function>(Heap::Instance().Make<ObjectHolder>(Heap::Instance().Make<Number>(max)));
}

Function* Min::Execute(FunctionArgs args, Scope* scope) {
    args.SkipLast();
    ThrowRuntimeErrorIf(args.Size() == 0, "min: expected >= 1 argument");
    ProcessArgs(args, scope);
    ThrowRuntimeErrorIf(!args.AreExpectedType<Number>());

    int32_t min = As<Number>(args[0])->GetValue();
    for (const auto& arg : args) {
        min = std::min(min, As<Number>(arg)->GetValue());
    }

    return As<Function>(Heap::Instance().Make<ObjectHolder>(Heap::Instance().Make<Number>(min)));
}

Function* Abs::Execute(FunctionArgs args, Scope* scope) {
    args.SkipLast();
    ThrowRuntimeErrorIf(args.Size() != 1, "abs: expected 1 argument");
    ProcessArgs(args, scope);
    ThrowRuntimeErrorIf(!args.AreExpectedType<Number>());

    auto value = As<Number>(args[0])->GetValue();
    return As<Function>(Heap::Instance().Make<ObjectHolder>(Heap::Instance().Make<Number>(std::abs(value))));
}

Function* Quote::Execute(FunctionArgs args, Scope* scope) {
    args.SkipLast();
    ThrowRuntimeErrorIf(args.Size() != 1, "quote: expected 1 argument");

    return As<Function>(Heap::Instance().Make<ObjectHolder>(args[0], scope));
}

Function* IsPair::Execute(FunctionArgs args, Scope* scope) {
    args.SkipLast();
    ThrowRuntimeErrorIf(args.Size() != 1, "pair?: expected 1 argument");
    ProcessArgs(args, scope);

    auto vector = ObjectToVector(args[0]);
    if (auto size = vector.size(); size == 2 || (size == 3 && vector.back() == nullptr)) {
        return GetTrueFunction(scope);
    }

    return GetFalseFunction(scope);
}

Function* IsNull::Execute(FunctionArgs args, Scope* scope) {
    args.SkipLast();
    ThrowRuntimeErrorIf(args.Size() != 1, "null?: expected 1 argument");
    ProcessArgs(args, scope);

    auto vector = ObjectToVector(args[0]);
    if (vector.size() == 1 && vector.back() == nullptr) {
        return GetTrueFunction(scope);
    }

    return GetFalseFunction(scope);
}

Function* IsList::Execute(FunctionArgs args, Scope* scope) {
    args.SkipLast();
    ThrowRuntimeErrorIf(args.Size() != 1, "list?: expected 1 argument");
    ProcessArgs(args, scope);

    auto vector = ObjectToVector(args[0]);
    if (vector.back() == nullptr) {
        return GetTrueFunction(scope);
    }

    return GetFalseFunction(scope);
}

Function* Cons::Execute(FunctionArgs args, Scope* scope) {
    args.SkipLast();
    ThrowRuntimeErrorIf(args.Size() != 2, "cons: expected 2 arguments");

    return As<Function>(Heap::Instance().Make<ObjectHolder>(Heap::Instance().Make<Cell>(args[0], args[1]),
                                               scope));
}

Function* Car::Execute(FunctionArgs args, Scope* scope) {
    args.SkipLast();
    ThrowRuntimeErrorIf(args.Size() != 1, "car: expected 1 argument");

    auto func = ExtractFunctionAndExecute(args[0], scope);
    auto obj = As<ObjectHolder>(func)->GetObject();
    auto obj_scope = As<ObjectHolder>(func)->GetScope();
    auto vector = ObjectToVector(obj);
    ThrowRuntimeErrorIf(vector.size() < 2, "car: expected list with >= 1 argument");

    return ExtractFunction(vector[0], obj_scope);
}

Function* Cdr::Execute(FunctionArgs args, Scope* scope) {
    args.SkipLast();
    ThrowRuntimeErrorIf(args.Size() != 1, "cdr: expected 1 argument");

    auto func = ExtractFunctionAndExecute(args[0], scope);
    auto obj = As<ObjectHolder>(func)->GetObject();
    auto obj_scope = As<ObjectHolder>(func)->GetScope();
    auto vector = ObjectToVector(obj);
    ThrowRuntimeErrorIf(vector.size() < 2, "cdr: expected list with >= 1 argument");
    vector.erase(vector.begin());

    if (vector[0] == nullptr || Is<Number>(vector[0])) {
        return As<Function>(Heap::Instance().Make<ObjectHolder>(VectorToObject(vector), obj_scope));
    }

    return ExtractFunction(VectorToObject(vector), obj_scope);
}

Function* List::Execute(FunctionArgs args, Scope* scope) {
    args.SkipLast();

    auto vector = std::vector(args.begin(), args.end());
    vector.push_back(nullptr);

    return As<Function>(Heap::Instance().Make<ObjectHolder>(VectorToObject(vector), scope));
}

Function* ListRef::Execute(FunctionArgs args, Scope* scope) {
    args.SkipLast();
    ThrowRuntimeErrorIf(args.Size() != 2, "list-ref: expected 2 arguments");
    ProcessArgs(args, scope);
    ThrowRuntimeErrorIf(!Is<Number>(args[1]), "list-ref: expected <List> <Ind>");

    auto vector = ObjectToVector(args[0]);
    size_t ind = As<Number>(args[1])->GetValue();
    ThrowRuntimeErrorIf(ind >= vector.size() - 1, "list-ref: index out of range");

    return As<Function>(Heap::Instance().Make<ObjectHolder>(vector[ind], scope));
}

Function* ListTail::Execute(FunctionArgs args, Scope* scope) {
    args.SkipLast();
    ThrowRuntimeErrorIf(args.Size() != 2, "list-tail: expected 2 arguments");
    ProcessArgs(args, scope);
    ThrowRuntimeErrorIf(!Is<Number>(args[1]), "list-tail: expected <List> <Ind>");

    auto vector = ObjectToVector(args[0]);
    size_t ind = As<Number>(args[1])->GetValue();
    ThrowRuntimeErrorIf(ind > vector.size() - 1, "list-tail: index out of range");

    auto result_vector = std::vector(vector.begin() + ind, vector.end());

    return As<Function>(Heap::Instance().Make<ObjectHolder>(VectorToObject(result_vector), scope));
}

Function* IsSymbol::Execute(FunctionArgs args, Scope* scope) {
    args.SkipLast();
    ThrowRuntimeErrorIf(args.Size() != 1, "symbol?: expected 1 argument");
    ProcessArgs(args, scope);

    if (Is<Symbol>(args[0])) {
        return GetTrueFunction(scope);
    }

    return GetFalseFunction(scope);
}

Function* Define::Execute(FunctionArgs args, Scope* scope) {
    args.SkipLast();

    if (bool used_syntax_sugar = Is<Cell>(args[0]); used_syntax_sugar) {
        ThrowSyntaxErrorIf(args.Size() < 2, "define: lambda sugar");
        auto vector = ObjectToVector(args[0]);
        const auto& name = As<Symbol>(vector[0])->GetName();
        auto func = As<Function>(Heap::Instance().Make<Lambda>(std::vector(vector.begin() + 1, vector.end() - 1),
                                                  std::vector(args.begin() + 1, args.end()), scope));
        scope->PutFunction(name, func);

    } else {
        ThrowSyntaxErrorIf(args.Size() != 2, "define: expected 2 arguments");
        const auto& name = As<Symbol>(args[0])->GetName();
        scope->PutFunction(name, ExtractFunction(args[1], scope));
    }

    return nullptr;
}

Function* Set::Execute(FunctionArgs args, Scope* scope) {
    args.SkipLast();
    ThrowSyntaxErrorIf(args.Size() != 2, "set!: expected 2 arguments");
    ThrowRuntimeErrorIf(!Is<Symbol>(args[0]), "set!: expected <Name> <Expr>");

    const auto& name = As<Symbol>(args[0])->GetName();
    scope->SetFunction(name, ExtractFunctionAndExecute(args[1], scope));

    return nullptr;
}

Function* SetCar::Execute(FunctionArgs args, Scope* scope) {
    args.SkipLast();
    ThrowSyntaxErrorIf(args.Size() != 2, "set-car!: expected 2 arguments");

    auto first = ExtractFunctionAndExecute(args[0], scope);
    auto pair = As<ObjectHolder>(first)->GetObject();
    auto vector = ObjectToVector(pair);
    ThrowRuntimeErrorIf(vector.size() != 2);

    auto second = ExtractFunctionAndExecute(args[1], scope);
    auto second_arg = As<ObjectHolder>(second)->GetObject();
    if (Is<Number>(second_arg)) {
        vector[0] = second_arg;
    } else {
        vector[0] = Heap::Instance().Make<Symbol>(As<ObjectHolder>(second)->GetName());
    }

    auto new_object = VectorToObject(vector);
    As<ObjectHolder>(first)->SetObject(new_object);

    return nullptr;
}

Function* SetCdr::Execute(FunctionArgs args, Scope* scope) {
    args.SkipLast();
    ThrowSyntaxErrorIf(args.Size() != 2, "set-cdr!: expected 2 arguments");

    auto first = ExtractFunctionAndExecute(args[0], scope);
    auto pair = As<ObjectHolder>(first)->GetObject();
    auto vector = ObjectToVector(pair);
    ThrowRuntimeErrorIf(vector.size() != 2);

    auto second = ExtractFunctionAndExecute(args[1], scope);
    auto second_arg = As<ObjectHolder>(second)->GetObject();
    if (Is<Number>(second_arg)) {
        vector[1] = second_arg;
    } else {
        vector[1] = Heap::Instance().Make<Symbol>(As<ObjectHolder>(second)->GetName());
    }

    auto new_object = VectorToObject(vector);
    As<ObjectHolder>(first)->SetObject(new_object);

    return nullptr;
}

Function* If::Execute(FunctionArgs args, Scope* scope) {
    args.SkipLast();
    ThrowSyntaxErrorIf(args.Size() != 2 && args.Size() != 3,
                       "if: expected <cond> <true_br> [<false_br>]");

    auto cond = Process(args[0], scope);
    if (!(Is<Symbol>(cond) && As<Symbol>(cond)->GetName() == "#f")) {
        return ExtractFunctionAndExecute(args[1], scope);
    }

    if (args.Size() == 3) {
        return ExtractFunctionAndExecute(args[2], scope);
    }

    return nullptr;
}

Function* CreateLambda::Execute(FunctionArgs args, Scope* scope) {
    args.SkipLast();
    ThrowSyntaxErrorIf(args.Size() < 2, "Invalid lambda syntax");

    auto lambda_params = ObjectToVector(args[0]);
    lambda_params.pop_back();
    auto lambda_body = std::vector(args.begin() + 1, args.end());

    return As<Function>(Heap::Instance().Make<Lambda>(
        std::move(lambda_params), std::move(lambda_body), scope));
}

Lambda::Lambda(std::vector<Object*> args, std::vector<Object*> body, Scope* parent_scope)
    : args_(std::move(args)), body_(std::move(body)), parent_scope_(parent_scope) {
    for (auto arg : args_) {
        AddDependency(arg);
    }
    for (auto body_expr : body_) {
        AddDependency(body_expr);
    }
    AddDependency(parent_scope_);
}

Function* Lambda::Execute(FunctionArgs args, Scope* scope) {
    args.SkipLast();
    ThrowRuntimeErrorIf(args.Size() != args_.size(), "lambda: invalid number of arguments");

    auto cur_scope = As<Scope>(Heap::Instance().Make<Scope>(parent_scope_));

    for (size_t i = 0, size = args.Size(); i < size; ++i) {
        const auto& name = As<Symbol>(args_[i])->GetName();
        cur_scope->PutFunction(name, ExtractFunctionAndExecute(args[i], scope));
    }

    Function* res{};
    for (auto& body_expr : body_) {
        res = ExtractFunction(body_expr, cur_scope);
    }

    return res;
}
