#include <parser.hpp>
#include <error.hpp>
#include <visitor_helper.hpp>
#include <garbage_collection.hpp>

Object* Read(Tokenizer* tokenizer) {
    if (tokenizer->IsEnd()) {
        throw SyntaxError("Unexpected end of input stream");
    }
    Token token = tokenizer->GetToken();
    tokenizer->Next();

    auto visitor = Overloaded{
        [](const ConstantToken& token) -> Object* {
            return Heap::Instance().Make<Number>(token.value);
        },
        [](const SymbolToken& token) -> Object* {
            return Heap::Instance().Make<Symbol>(token.name);
        },
        [&tokenizer](const OpenBracketToken&) -> Object* { return ReadList(tokenizer); },
        [&tokenizer](const QuoteToken&) -> Object* {
            auto first = Heap::Instance().Make<Symbol>("quote");
            auto second = Read(tokenizer);
            return Heap::Instance().Make<Cell>(first, Heap::Instance().Make<Cell>(second, nullptr));
        },
        [](const auto&) -> Object* { throw SyntaxError("Unexpected token"); }};
    return std::visit(std::move(visitor), std::move(token));
}

Object* ReadList(Tokenizer* tokenizer) {
    if (tokenizer->IsEnd()) {
        throw SyntaxError("Unexpected end of input stream");
    }

    if (std::holds_alternative<CloseBracketToken>(tokenizer->GetToken())) {
        tokenizer->Next();
        return nullptr;
    }

    auto first = Read(tokenizer);
    Object* second{};
    if (std::holds_alternative<DotToken>(tokenizer->GetToken())) {
        tokenizer->Next();
        second = Read(tokenizer);
        if (tokenizer->IsEnd() ||
            !std::holds_alternative<CloseBracketToken>(tokenizer->GetToken())) {
            throw SyntaxError("Expected ')'");
        }
        tokenizer->Next();

    } else {
        second = ReadList(tokenizer);
    }
    return Heap::Instance().Make<Cell>(first, second);
}
