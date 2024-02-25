#pragma once

#include <variant>
#include <istream>

struct SymbolToken {
    std::string name;

    bool operator==(const SymbolToken& other) const;
};

struct QuoteToken {
    bool operator==(const QuoteToken&) const;
};

struct DotToken {
    bool operator==(const DotToken&) const;
};

struct OpenBracketToken {
    bool operator==(const OpenBracketToken&) const;
};

struct CloseBracketToken {
    bool operator==(const CloseBracketToken&) const;
};

struct ConstantToken {
    int32_t value;

    bool operator==(const ConstantToken& other) const;
};

using Token = std::variant<ConstantToken, OpenBracketToken, CloseBracketToken, SymbolToken,
                           QuoteToken, DotToken>;

class Tokenizer final {
public:
    Tokenizer(std::istream* in);

    bool IsEnd() const;
    void Next();
    Token GetToken() const;

private:
    std::istream* istream_;
    Token token_;
    bool eof_;
};
