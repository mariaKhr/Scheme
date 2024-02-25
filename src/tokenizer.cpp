#include <tokenizer.hpp>
#include <error.hpp>

namespace {
ConstantToken ReadConstant(std::istream* in) {
    int32_t constant;
    *in >> constant;
    return ConstantToken{constant};
}

SymbolToken ReadSymbol(std::istream* in) {
    std::string buf{};
    char next{};
    do {
        buf.push_back(in->get());
        next = in->peek();
    } while (next != EOF && next != ')' && !std::isspace(next));
    return SymbolToken{std::move(buf)};
}
}  // namespace

Tokenizer::Tokenizer(std::istream* in) : istream_(in), eof_(false) {
    Next();
}

bool Tokenizer::IsEnd() const {
    return eof_;
}

Token Tokenizer::GetToken() const {
    return token_;
}

void Tokenizer::Next() {
    char next = istream_->peek();
    while (std::isspace(next)) {
        istream_->get();
        next = istream_->peek();
    }

    if (next == '(') {
        token_ = OpenBracketToken{};
        istream_->get();

    } else if (next == ')') {
        token_ = CloseBracketToken{};
        istream_->get();

    } else if (next == '\'') {
        token_ = QuoteToken{};
        istream_->get();

    } else if (next == '.') {
        token_ = DotToken{};
        istream_->get();

    } else if (next == '-' || next == '+') {
        char sign = istream_->get();
        if (std::isdigit(istream_->peek())) {
            istream_->unget();
            token_ = ReadConstant(istream_);
        } else {
            token_ = SymbolToken{std::string(1, sign)};
        }

    } else if (std::isdigit(next)) {
        token_ = ReadConstant(istream_);

    } else if (std::isalpha(next) || next == '<' || next == '=' || next == '>' || next == '*' ||
               next == '/' || next == '#') {
        token_ = ReadSymbol(istream_);

    } else if (next == EOF) {
        eof_ = true;

    } else {
        throw SyntaxError("Unexpected token");
    }
}

bool SymbolToken::operator==(const SymbolToken& other) const {
    return name == other.name;
}

bool QuoteToken::operator==(const QuoteToken&) const {
    return true;
}

bool DotToken::operator==(const DotToken&) const {
    return true;
}

bool OpenBracketToken::operator==(const OpenBracketToken&) const {
    return true;
}

bool CloseBracketToken::operator==(const CloseBracketToken&) const {
    return true;
}

bool ConstantToken::operator==(const ConstantToken& other) const {
    return value == other.value;
}
