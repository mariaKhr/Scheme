#include <scheme.hpp>
#include <sstream>
#include <garbage_collection.hpp>
#include <error.hpp>
#include <parser.hpp>
#include <func.hpp>

Interpreter::Interpreter() : global_scope_(std::make_unique<Scope>()) {
    std::unordered_map<std::string, Object*> scope = {
        {"#t", Heap::Instance().Make<ObjectHolder>(Heap::Instance().Make<Symbol>("#t"))},
        {"#f", Heap::Instance().Make<ObjectHolder>(Heap::Instance().Make<Symbol>("#f"))},

        {"boolean?", Heap::Instance().Make<IsBoolean>()},
        {"not", Heap::Instance().Make<Not>()},
        {"and", Heap::Instance().Make<And>()},
        {"or", Heap::Instance().Make<Or>()},

        {"number?", Heap::Instance().Make<IsNumber>()},
        {"=", Heap::Instance().Make<Equal>()},
        {"<", Heap::Instance().Make<MonotonicallyIncreasing>()},
        {">", Heap::Instance().Make<MonotonicallyDecreasing>()},
        {"<=", Heap::Instance().Make<MonotonicallyNonDecreasing>()},
        {">=", Heap::Instance().Make<MonotonicallyNonIncreasing>()},
        {"+", Heap::Instance().Make<Plus>()},
        {"-", Heap::Instance().Make<Minus>()},
        {"*", Heap::Instance().Make<Multiply>()},
        {"/", Heap::Instance().Make<Divide>()},
        {"max", Heap::Instance().Make<Max>()},
        {"min", Heap::Instance().Make<Min>()},
        {"abs", Heap::Instance().Make<Abs>()},

        {"quote", Heap::Instance().Make<Quote>()},

        {"pair?", Heap::Instance().Make<IsPair>()},
        {"null?", Heap::Instance().Make<IsNull>()},
        {"list?", Heap::Instance().Make<IsList>()},
        {"cons", Heap::Instance().Make<Cons>()},
        {"car", Heap::Instance().Make<Car>()},
        {"cdr", Heap::Instance().Make<Cdr>()},
        {"list", Heap::Instance().Make<List>()},
        {"list-ref", Heap::Instance().Make<ListRef>()},
        {"list-tail", Heap::Instance().Make<ListTail>()},

        {"symbol?", Heap::Instance().Make<IsSymbol>()},
        {"define", Heap::Instance().Make<Define>()},
        {"set!", Heap::Instance().Make<Set>()},
        {"set-car!", Heap::Instance().Make<SetCar>()},
        {"set-cdr!", Heap::Instance().Make<SetCdr>()},

        {"if", Heap::Instance().Make<If>()},
        {"lambda", Heap::Instance().Make<CreateLambda>()},
    };

    for (auto &&[name, func] : scope) {
        global_scope_->PutFunction(std::move(name), As<Function>(func));
    }
}

std::string Interpreter::Run(const std::string &input) {
    std::stringstream stream{input};
    auto tokenizer = Tokenizer(&stream);
    auto object = Read(&tokenizer);
    ThrowSyntaxErrorIf(!tokenizer.IsEnd(), "Syntax error when parsing the query");

    auto result = Process(object, global_scope_.get());
    auto serialized_result = Serialize(result);
    Heap::Instance().MarkAndSweep(global_scope_.get());
    return serialized_result;
}

Interpreter::~Interpreter() {
    Heap::Instance().MarkAndSweep(nullptr);
}
