#include "Tokenizer.h"

#include <utility>
#include <exception>

bool NameToken::operator==(const NameToken &other) const {
    return name == other.name;
}

bool QuoteToken::operator==(const QuoteToken&) const {
    return true;
}

bool DotToken::operator==(const DotToken &) const {
    return true;
}

bool IntegerToken::operator==(const IntegerToken &other) const {
    return value == other.value;
}

bool BoolToken::operator==(const BoolToken &other) const {
    return value == other.value;
}

bool CharToken::operator==(const CharToken &other) const {
    return value == other.value;
}

//bool isIntegerToken(Token token) {
//    return std::holds_alternative<IntegerToken>(token);
//}

Tokenizer::Tokenizer(std::istream *expr) : expr(expr) {}

bool isControlChar(char x) {
    return x == ' ' || x == '\t' || x == '\n';
}

void skipControlChars(std::istream *expr) {
    while (isControlChar(expr->peek())) {
        expr->get();
    }
}

bool Tokenizer::end() {
    skipControlChars(expr);
    return expr->peek() == EOF;
}

bool isDigit(char x) {
    return x >= '0' && x <= '9';
}

bool isInteger(std::string str) {
    if (str.front() == '-' || str.front() == '+') {
        str = str.substr(1);
    }
    bool result = !str.empty();
    for (int i = 0; i < str.size() && result; ++i) {
        result &= isDigit(str[i]);
    }
    return result;
}

Token toIntegerToken(const std::string& str) {
    return IntegerToken{std::stoi(str), str};
}

bool isBool(const std::string& str) {
    return str == "#t" || str == "#f";
}

Token toBoolToken(const std::string& str) {
    return BoolToken{str == "#t", str};
}

bool isChar(const std::string& str) {
    if (str.size() >= 3) {
        if (str[0] == '#' && str[1] == '\\') {
            if (str.size() == 3 || str.substr(2) == "tab" || str.substr(2) == "space") {
                return true;
            }
            //throw SyntaxError("Wrong Syntax");
            //throw std::runtime_error("unknown character name " + str.substr(2));
            throw LexicalError("unknown character name: " + str.substr(2));
        }
    }
    return false;
}

Token toCharToken(const std::string& str) {
    if (str.size() == 3) {
        return CharToken{str[2], str};
    } else if (str.substr(2) == "tab") {
        return CharToken{'\t', str};
    } else if (str.substr(2) == "space") {
        return CharToken{' ', str};
    }
}

bool isName(const std::string& str) {
    if (str.front() == '-' && str.size() > 1) {
        //exception
        //throw std::runtime_error("name can not start with \'-\': " + str);
        throw LexicalError("name can not start with \'-\': " + str);
    } else if (str.front() == '+' && str.size() > 1) {
        //exception
        //throw std::runtime_error("name can not start with \'+\': " + str);
        throw LexicalError("name can not start with \'+\': " + str);
    }
    return true;
}

Token Tokenizer::get() {
    if (end()) {
        return {};
    }

    skipControlChars(expr);
    char curr = expr->get();
    if (curr == '(') {
        return BracketToken::OPEN;
    } else if (curr == ')') {
        return BracketToken::CLOSE;
    } else if (curr == '\'') {
        return QuoteToken();
    } else if (curr == '.') {
        return DotToken();
    } else {
        std::string str(1, curr);
        while (expr->peek() != '(' && expr->peek() != ')' && !isControlChar(expr->peek()) && expr->peek() != EOF) {
            str.push_back(expr->get());
        }
        if (isInteger(str)) {
            return toIntegerToken(str);
        } else if (isBool(str)) {
            return toBoolToken(str);
        } else if (isChar(str)) {
            return toCharToken(str);
        } else if (isName(str)) {
            return NameToken{str};
        }
    }
}

void putBack(std::string& str, std::istream* expr) {
    while (!str.empty()) {
        expr->putback(str.back());
        str.pop_back();
    }
}

Token Tokenizer::peek() {
    if (end()) {
        return {};
    }
    skipControlChars(expr);
    char curr = expr->peek();
    if (curr == '(') {
        return BracketToken::OPEN;
    } else if (curr == ')') {
        return BracketToken::CLOSE;
    } else if (curr == '\'') {
        return QuoteToken();
    } else if (curr == '.') {
        return DotToken();
    } else {
        std::string str;
        while (expr->peek() != '(' && expr->peek() != ')' && !isControlChar(expr->peek()) && expr->peek() != EOF) {
            str.push_back(expr->get());
        }
        putBack(str, expr);
        if (isInteger(str)) {
            return toIntegerToken(str);
        } else if (isBool(str)) {
            return toBoolToken(str);
        } else if (isChar(str)) {
            return toCharToken(str);
        } else if (isName(str)) {
            return NameToken{str};
        }
    }
}

bool IsOpenBracket(const Token& token) {
    if (std::holds_alternative<BracketToken>(token)) {
        return std::get<BracketToken>(token) == BracketToken::OPEN;
    }
    return false;
}

bool IsCloseBracket(const Token& token) {
    if (std::holds_alternative<BracketToken>(token)) {
        return std::get<BracketToken>(token) == BracketToken::CLOSE;
    }
    return false;
}
