#include <sstream>
#include <iostream>
#include <fstream>

#include "ParserTest.h"

void display(std::shared_ptr<Object> v, int deep = 0) {
    if (v == nullptr) {
        return;
    }
    if (Is<Cell>(v)) {
        if (Is<Cell>(As<Cell>(v)->getFirst())) {
            display(As<Cell>(v)->getFirst(), deep + 1);
            for (int i = 0; i < deep + 1; ++i) {
                std::cout << "\t";
            }
            std::cout << "Cell" << '\n';
        } else {
            display(As<Cell>(v)->getFirst(), deep);
        }
        display(As<Cell>(v)->getSecond(), deep);
    } else {
        for (int i = 0; i < deep; ++i) {
            std::cout << "\t";
        }
        std::cout << v->serialize() << '\n';
    }
}

void ParserTest::run(const std::string &str) {

    std::ifstream file("../prog.txt");
    Tokenizer t(&file);

    //std::cout << str << '\n';




    auto result = read(&t);
    if (!t.end()) {
        throw SyntaxError("expected token: \')\'");
    }

    //std::cout << str << '\n';
    //auto a = result->eval();
    //std::cout << a->serialize();
//    display(result);
}



int main() {
    ParserTest::run("prog.txt");
    //ParserTest::run("(define (asdf 1 2) (123 12 2) (1 1 1 1  1) ) ");
    //ParserTest::run("(define x 1 2)");
    return 0;
}
