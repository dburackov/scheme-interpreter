#include <fstream>
#include <sstream>
#include "Scheme.h"

Scheme::Scheme() {

}

void Scheme::run() {
    while (true) {
        std::cout << ">>> ";
        std::string str;
        std::getline(std::cin, str);
        std::istringstream input(str);
        Tokenizer t(&input);
        try {
            auto curr = read(&t);
            auto result = curr->eval();
            std::cout << result->serialize() << '\n';
        } catch (const RuntimeError &e) {
            std::cout << "RuntimeError: " << e.what() << '\n';
        } catch (const SyntaxError &e) {
            std::cout << "SyntaxError: " << e.what() << '\n';
        } catch (const LexicalError &e) {
            std::cout << "LexicalError: " << e.what() << '\n';
        } catch (const std::exception& e) {
            std::cout << "Exception: " << e.what() << '\n';
        }
    }
}

void Scheme::run(const std::string &filename) {
    std::ifstream file(filename);
    Tokenizer t(&file);

    while (!t.end()) {
        try {
            auto curr = read(&t);
            auto result = curr->eval();
            std::cout << result->serialize() << '\n';
        } catch (const RuntimeError &e) {
            std::cout << "RuntimeError: " << e.what() << '\n';
        } catch (const SyntaxError &e) {
            std::cout << "SyntaxError: " << e.what() << '\n';
        } catch (const LexicalError &e) {
            std::cout << "LexicalError: " << e.what() << '\n';
        } catch (const std::exception& e) {
            std::cout << "Exception: " << e.what() << '\n';
        }
    }
}

