#pragma once

#include <string>
#include <iostream>
#include <variant>
#include <vector>

struct NameToken {
    std::string name;

    bool operator==(const NameToken& other) const;
};

struct QuoteToken {
    bool operator==(const QuoteToken&) const;
};

struct DotToken {
    bool operator==(const DotToken&) const;
};

enum class BracketToken { OPEN, CLOSE };

struct IntegerToken {
    int value;
    std::string row;
    bool operator==(const IntegerToken& other) const;
};

struct BoolToken {
    bool value;
    std::string row;
    bool operator==(const BoolToken& other) const;
};

struct CharToken {
    char value;
    std::string row;
    bool operator==(const CharToken& other) const;
};


using Token = std::variant<IntegerToken, BoolToken, CharToken, BracketToken, NameToken, QuoteToken, DotToken>;


class Tokenizer {
public:
    explicit Tokenizer(std::istream *expr);

    bool end();

    Token peek();

    Token get();

private:
    std::istream *expr;

};

bool IsOpenBracket(const Token& token);

bool IsCloseBracket(const Token& token);


struct SyntaxError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

struct RuntimeError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

struct LexicalError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};
