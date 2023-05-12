#include "Parser.h"

#include <iostream>


std::shared_ptr<Object> read(Tokenizer* tokenizer);

std::shared_ptr<Object> readList(Tokenizer* tokenizer) {
    std::vector<std::shared_ptr<Object>> list;
    while (true) {
        if (tokenizer->end()) {
            throw SyntaxError("expected token \')\'");
        }
        if (IsCloseBracket(tokenizer->peek())) {
            tokenizer->get();
            break;
        }
        list.push_back(read(tokenizer));
    }
    if (list.empty()) {
        return nullptr;
    }

    std::shared_ptr<Object> result = std::make_shared<Cell>();

    auto curr = result;
    auto prev = curr;
    for (int i = 0; i < list.size(); ++i) {
        if (Is<Dot>(list[i])) {
            if (i + 2 != list.size() || i == 0) {
                throw SyntaxError("unexpected token \'.\'");
            }
            As<Cell>(prev)->getSecond() = list.back();
            break;
        } else {
            As<Cell>(curr)->getFirst() = list[i];
            if (i != list.size() - 1) {
                As<Cell>(curr)->getSecond() = std::make_shared<Cell>();
            }
        }
        prev = curr;
        curr = As<Cell>(curr)->getSecond();
    }
    return result;
}

std::shared_ptr<Object> read(Tokenizer* tokenizer) {
    Token token = tokenizer->get();

    if (std::holds_alternative<IntegerToken>(token)) {
        return std::make_shared<Integer>(std::get<IntegerToken>(token).value);
    }
    if (std::holds_alternative<BoolToken>(token)) {
        return std::make_shared<Bool>(std::get<BoolToken>(token).value);
    }
    if (std::holds_alternative<CharToken>(token)) {
        return std::make_shared<Char>(std::get<CharToken>(token).value);
    }
    if (std::holds_alternative<NameToken>(token)) {
        return std::make_shared<Name>(std::get<NameToken>(token).name);
    }
    if (std::holds_alternative<QuoteToken>(token)) {
        return std::make_shared<Quote>(read(tokenizer));
    }
    if (IsOpenBracket(token)) {
        return readList(tokenizer);
    }
    if (IsCloseBracket(token)) {
        throw SyntaxError("Wrong Syntax: unexpected token \')\'");
    }
    if (std::holds_alternative<DotToken>(token)) {
        throw SyntaxError("Wrong Syntax: unexpected token \'.\'");
    }
    throw SyntaxError("Wrong Syntax: unexpected token");
}