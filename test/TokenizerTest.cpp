#include <sstream>

#include "TokenizerTest.h"

#include <vector>
#include <iomanip>
#include <set>

void TokenizerTest::run(const std::string& str) {

    std::istringstream input(str);

    Tokenizer t(&input);

    std::cout << std::setiosflags(std::ios::left);
    std::cout << str << '\n';

    std::vector tm = {"INT", "BOOL", "CHAR", "BRACKET", "NAME", "QUOTE", "DOT"};

//    while (!t.end()) {
//        Token curr = t.get();
//
//        switch (curr.index()) {
//            case 0:
//                std::cout << std::setw(10) << std::get<IntegerToken>(curr).row;
//                std::cout << std::setw(10) << "INTEGER" << '\n';
//                break;
//            case 1:
//                std::cout << std::setw(10) << std::get<BoolToken>(curr).row;
//                std::cout << std::setw(10) << "BOOL" << '\n';
//                break;
//            case 2:
//                std::cout << std::setw(10) << std::get<CharToken>(curr).row;
//                std::cout << std::setw(10) << "CHAR" << '\n';
//                break;
//            case 3:
//                std::cout << std::setw(10) << (std::get<BracketToken>(curr) == BracketToken::OPEN? "(" : ")");
//                std::cout << std::setw(10) << (std::get<BracketToken>(curr) == BracketToken::OPEN? "OPEN BRACKET" : "CLOSE BRACKET") << '\n';
//                break;
//            case 4:
//                std::cout << std::setw(10) << std::get<NameToken>(curr).name;
//                std::cout << std::setw(10) << "NAME" << '\n';
//                break;
//            case 5:
//                std::cout << std::setw(10) << '\'';
//                std::cout << std::setw(10) << "QUOTE" << '\n';
//                break;
//            case 6:
//                std::cout << std::setw(10) << '.';
//                std::cout << std::setw(10) << "DOT" << '\n';
//                break;
//            default:
//                std::cout << tm[curr.index()] << '\n';
//        }
//    }


    std::set<std::string> i;
    std::set<std::string> b;
    std::set<std::string> c;
    std::set<std::string> n;

    while (!t.end()) {
        Token curr = t.get();

        switch (curr.index()) {
            case 0:
                i.insert(std::get<IntegerToken>(curr).row);
                break;
            case 1:
                b.insert(std::get<BoolToken>(curr).row);
                break;
            case 2:
                c.insert(std::get<CharToken>(curr).row);
                break;
            case 4:
                n.insert(std::get<NameToken>(curr).name);
                break;
            default:
                break;
        }
    }

    int tab = 15;

    std::cout << '\n';
    std::cout << std::setw(tab) << "TOKEN";
    std::cout << std::setw(tab) << "TYPE" << '\n';
    std::cout << '\n';

    for (const auto& it : i) {
        std::cout << std::setw(tab) << it;
        std::cout << std::setw(tab) << "INTEGER" << '\n';
    }

    std::cout << '\n';
    std::cout << '\n';

    std::cout << std::setw(tab) << "TOKEN";
    std::cout << std::setw(tab) << "TYPE" << '\n';
    std::cout << '\n';

    for (const auto& it : b) {
        std::cout << std::setw(tab) << it;
        std::cout << std::setw(tab) << "BOOL" << '\n';
    }

    std::cout << '\n';
    std::cout << '\n';

    std::cout << std::setw(tab) << "TOKEN";
    std::cout << std::setw(tab) << "TYPE" << '\n';
    std::cout << '\n';

    for (const auto& it : c) {
        std::cout << std::setw(tab) << it;
        std::cout << std::setw(tab) << "CHAR" << '\n';
    }

    std::cout << '\n';
    std::cout << '\n';

    std::cout << std::setw(tab) << "TOKEN";
    std::cout << std::setw(tab) << "TYPE" << '\n';
    std::cout << '\n';

    for (const auto& it : n) {
        std::cout << std::setw(tab) << it;
        std::cout << std::setw(tab) << "NAME" << '\n';
    }

}



int main() {
    //TokenizerTest::run("12a( 12.3 12>5");
    //TokenizerTest::run("(xxx +5 -3 #t #f #\\a . ')");
    //TokenizerTest::run("(fh43../()555 #\\! (+ 3 +3))");
    //TokenizerTest::run("><!%%^a \' 3 \' #f #z (.)/");
    TokenizerTest::run("(())10 ((123 110 -123123 12)) . . '' asd() aw^1{}$$% ^&&HDAH_!@# assa #t #t #f #f #\\a #\\space #\\tab #\\b");
    return 0;
}
