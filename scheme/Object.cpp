#include "Object.h"
#include "Scope.h"

#include <utility>

Integer::Integer(int val) : value(val) {}

int Integer::getValue() const {
    return value;
}

std::shared_ptr<Object> Integer::eval() {
    return std::make_shared<Integer>(value);
}

std::string Integer::serialize() {
    return std::to_string(getValue());
}

Name::Name(std::string name) : name(std::move(name)) {}

const std::string &Name::getName() const {
    return name;
}

Scope scope;

std::shared_ptr<Object> Name::eval() {
    if (scope.count(name)) {
        return scope.get(name);
    }
    return std::make_shared<Function>(name);
}

std::string Name::serialize() {
    return getName();
}


Bool::Bool(std::string b) {
    if (b == "#t") {
        value = true;
    } else {
        value = false;
    }
}

Bool::Bool(bool val) : value(val) {}

bool Bool::getBool() const {
    return value;
}

std::shared_ptr<Object> Bool::eval() {
    return std::make_shared<Bool>(getBool());
}

std::string Bool::serialize() {
    return getBool()? "#t" : "#f";
}


Dot::Dot() {

}

std::shared_ptr<Object> Dot::eval() {
    throw RuntimeError("Weird, bro!");
    return nullptr;
}

std::string Dot::serialize() {
    return ".";
}

Cell::Cell() {

}

std::shared_ptr<Object> Cell::getFirst() const {
    return first;
}

std::shared_ptr<Object> Cell::getSecond() const {
    return second;
}

std::shared_ptr<Object> &Cell::getFirst() {
    return first;
}

std::shared_ptr<Object> &Cell::getSecond() {
    return second;
}

std::vector<std::shared_ptr<Object>> ConvertToVector(std::shared_ptr<Object> cell) {
    std::vector<std::shared_ptr<Object>> to_ret;
    if (cell == nullptr) {
        return to_ret;
    }
    while (true) {
        if (!Is<Cell>(cell)) {
            throw RuntimeError("RE");
        }
        auto ptr_first = As<Cell>(cell)->getFirst();
        auto ptr_second = As<Cell>(cell)->getSecond();
        to_ret.push_back(ptr_first);
        if (ptr_second == nullptr) {
            break;
        }
        if (Is<Integer>(ptr_second)) {
            to_ret.push_back(ptr_second);
            break;
        }
        cell = ptr_second;
    }
    return to_ret;
}

void CheckNullptr(std::shared_ptr<Object> ptr) {
    if (ptr == nullptr) {
        throw RuntimeError("nullptr issue");
    }
}

std::vector<std::shared_ptr<Object>> EvalVector(std::vector<std::shared_ptr<Object>>& args) {
    std::vector<std::shared_ptr<Object>> result;
    for (auto it : args) {
        CheckNullptr(it);
        result.push_back(it->eval());
    }
    return result;
}


std::shared_ptr<Object> Cell::eval() {
    if (!Is<Name>(getFirst())) {
        throw RuntimeError("wrong function name");
    }
    auto function = getFirst()->eval();
    auto args = ConvertToVector(getSecond());

    if (scope.count(As<Name>(getFirst())->getName())) {
        args = EvalVector(args);
    }

    return As<Function>(function)->apply(args);
}

std::string Cell::serialize() {
    std::string fi;
    std::string se;
    if (getFirst()) {
        fi = getFirst()->serialize();
        if (Is<Cell>(getFirst()) && fi.size() != 2) {
            fi = fi.substr(1, fi.size() - 2);
        }
    }
    if (getSecond()) {
        se = getSecond()->serialize();
        if (Is<Cell>(getSecond())) {
            se = se.substr(1, se.size() - 2);
        } else {
            se = " " + se;
        }
    }
    return "(" + fi + (se.size()? " " : "") + se + ")";
}

Char::Char(char val) : value(val) {}

char Char::getChar() const {
    return value;
}

std::shared_ptr<Object> Char::eval() {
    return std::make_shared<Char>(value);
}

std::string Char::serialize() {
    return std::string(1, value);
}

Quote::Quote(std::shared_ptr<Object> val) : value(val) {}

std::shared_ptr<Object> Quote::getValue() {
    return value;
}

std::shared_ptr<Object> Quote::eval() {
    return value;
}

std::string Quote::serialize() {
    return nullptr;
}

bool equal(std::shared_ptr<Object> fi, std::shared_ptr<Object> se) {
    bool result = false;
    if (Is<Integer>(fi) && Is<Integer>(se)) {
        result = As<Integer>(fi)->getValue() == As<Integer>(se)->getValue();
    }
    if (Is<Bool>(fi) && Is<Bool>(se)) {
        result = As<Bool>(fi)->getBool() == As<Bool>(se)->getBool();
    }
    if (Is<Char>(fi) && Is<Char>(se)) {
        result = As<Char>(fi)->getChar() == As<Char>(se)->getChar();
    }
    return result;
}

template <typename T>
void CheckIfValidTypes(std::vector<std::shared_ptr<Object>>& vec) {
    for (auto it : vec) {
        if (!Is<T>(it)) {
            throw RuntimeError("type mismatch");
        }
    }
}

void CheckIfBadArgsCount(std::vector<std::shared_ptr<Object>>& vec, std::vector<size_t> bad_counts,
                         std::vector<size_t> good_counts) {
    if (!bad_counts.empty()) {
        for (auto elem : bad_counts) {
            if (vec.size() == elem) {
                throw SyntaxError("unexpected arguments number: " + std::to_string(elem));
            }
        }
    } else {
        bool fl = false;
        for (auto elem : good_counts) {
            if (vec.size() == elem) {
                fl = true;
            }
        }
        if (!fl) {
            std::string lst;
            for (auto &it : good_counts) {
                lst = lst + " or " + std::to_string(it);
            }
            throw SyntaxError("expected" + lst.substr(3) + " arguments");
        }
    }
}

std::shared_ptr<Object> ListASTFromVector(std::vector<std::shared_ptr<Object>> list) {
    std::shared_ptr<Object> to_ret = std::make_shared<Cell>();
    if (list.empty()) {
        return to_ret;
    }
    auto cur = to_ret;
    auto prev = cur;
    for (size_t i = 0; i < list.size(); ++i) {
        if (Is<Dot>(list[i])) {
            if (i + 2 != list.size() || i == 0) {
                throw SyntaxError("unexpected token \'.\'");
            }
            As<Cell>(prev)->getSecond() = list.back();
            break;
        } else {
            As<Cell>(cur)->getFirst() = list[i];
            if (i != list.size() - 1) {
                As<Cell>(cur)->getSecond() = std::make_shared<Cell>();
            }
        }
        prev = cur;
        cur = As<Cell>(cur)->getSecond();
    }
    return to_ret;
}

Function::Function(std::string name) {
    if (name == "quote") {
        func = [](std::vector<std::shared_ptr<Object>> &args) { return nullptr; };
    } /*NUMBER FUNCTIONS*/ else if (name == "number?") {
        func = [](std::vector<std::shared_ptr<Object>> &args) {
            args = EvalVector(args);
            std::shared_ptr<Object> result;
            result = std::make_shared<Bool>(Is<Integer>(args.front()));
            return result;
        };
    } else if (name == "=") {
        func = [](std::vector<std::shared_ptr<Object>> &args) {
            args = EvalVector(args);
            CheckIfValidTypes<Integer>(args);
            std::shared_ptr<Object> result;
            bool res = true;
            if (!args.empty()) {
                for (size_t i = 0; i < args.size() - 1; ++i) {
                    if (As<Integer>(args[i])->getValue() == As<Integer>(args[i + 1])->getValue()) {

                    } else {
                        res = false;
                    }
                }
            }
            result = std::make_shared<Bool>(res);
            return result;
        };
    } else if (name == ">") {
        func = [](std::vector<std::shared_ptr<Object>> &args) {
            args = EvalVector(args);
            CheckIfValidTypes<Integer>(args);
            std::shared_ptr<Object> result;
            bool res = true;
            if (!args.empty()) {
                for (size_t i = 0; i < args.size() - 1; ++i) {
                    if (As<Integer>(args[i])->getValue() > As<Integer>(args[i + 1])->getValue()) {

                    } else {
                        res = false;
                    }
                }
            }
            result = std::make_shared<Bool>(res);
            return result;
        };
    } else if (name == "<") {
        func = [](std::vector<std::shared_ptr<Object>> &args) {
            args = EvalVector(args);
            CheckIfValidTypes<Integer>(args);
            std::shared_ptr<Object> result;
            bool res = true;
            if (!args.empty()) {
                for (size_t i = 0; i < args.size() - 1; ++i) {
                    if (As<Integer>(args[i])->getValue() < As<Integer>(args[i + 1])->getValue()) {

                    } else {
                        res = false;
                    }
                }
            }
            result = std::make_shared<Bool>(res);
            return result;
        };
    } else if (name == "<=") {
        func = [](std::vector<std::shared_ptr<Object>> &args) {
            args = EvalVector(args);
            CheckIfValidTypes<Integer>(args);
            std::shared_ptr<Object> result;
            bool res = true;
            if (!args.empty()) {
                for (size_t i = 0; i < args.size() - 1; ++i) {
                    if (As<Integer>(args[i])->getValue() <= As<Integer>(args[i + 1])->getValue()) {

                    } else {
                        res = false;
                    }
                }
            }
            result = std::make_shared<Bool>(res);
            return result;
        };
    } else if (name == ">=") {
        func = [](std::vector<std::shared_ptr<Object>> &args) {
            args = EvalVector(args);
            CheckIfValidTypes<Integer>(args);
            std::shared_ptr<Object> result;
            bool res = true;
            if (!args.empty()) {
                for (size_t i = 0; i < args.size() - 1; ++i) {
                    if (As<Integer>(args[i])->getValue() >= As<Integer>(args[i + 1])->getValue()) {

                    } else {
                        res = false;
                    }
                }
            }
            result = std::make_shared<Bool>(res);
            return result;
        };
    } else if (name == "+") {
        func = [](std::vector<std::shared_ptr<Object>> &args) {
            args = EvalVector(args);
            CheckIfValidTypes<Integer>(args);
            std::shared_ptr<Object> to_ret;
            int64_t res = 0;
            for (size_t i = 0; i < args.size(); ++i) {
                res += As<Integer>(args[i])->getValue();
            }
            to_ret = std::make_shared<Integer>(res);
            return to_ret;
        };
    } else if (name == "*") {
        func = [](std::vector<std::shared_ptr<Object>> &args) {
            args = EvalVector(args);
            CheckIfValidTypes<Integer>(args);
            std::shared_ptr<Object> to_ret;
            int64_t res = 1;
            for (size_t i = 0; i < args.size(); ++i) {
                res *= As<Integer>(args[i])->getValue();
            }
            to_ret = std::make_shared<Integer>(res);
            return to_ret;
        };
    } else if (name == "-") {
        func = [](std::vector<std::shared_ptr<Object>> &args) {
            args = EvalVector(args);
            CheckIfValidTypes<Integer>(args);
            CheckIfBadArgsCount(args, {0}, {});
            std::shared_ptr<Object> to_ret;
            int64_t res = As<Integer>(args[0])->getValue();
            for (size_t i = 1; i < args.size(); ++i) {
                res -= As<Integer>(args[i])->getValue();
            }
            to_ret = std::make_shared<Integer>(res);
            return to_ret;
        };
    } else if (name == "/") {
        func = [](std::vector<std::shared_ptr<Object>> &args) {
            args = EvalVector(args);
            CheckIfValidTypes<Integer>(args);
            CheckIfBadArgsCount(args, {0}, {});
            std::shared_ptr<Object> to_ret;
            int64_t res = As<Integer>(args[0])->getValue();
            for (size_t i = 1; i < args.size(); ++i) {
                res /= As<Integer>(args[i])->getValue();
            }
            to_ret = std::make_shared<Integer>(res);
            return to_ret;
        };
    } else if (name == "max") {
        func = [](std::vector<std::shared_ptr<Object>> &args) {
            args = EvalVector(args);
            CheckIfValidTypes<Integer>(args);
            CheckIfBadArgsCount(args, {0}, {});
            std::shared_ptr<Object> to_ret;
            int64_t res = As<Integer>(args[0])->getValue();
            for (size_t i = 1; i < args.size(); ++i) {
                int64_t cur_val = As<Integer>(args[i])->getValue();
                res = std::max(res, cur_val);
            }
            to_ret = std::make_shared<Integer>(res);
            return to_ret;
        };
    } else if (name == "eq?") {
        func = [](std::vector<std::shared_ptr<Object>> &args) {
            args = EvalVector(args);
            CheckIfBadArgsCount(args, {0}, {});
            bool result = true;
            for (int i = 0; i < args.size() - 1; ++i) {
                result &= equal(args[i], args[i + 1]);
            }
            return std::make_shared<Bool>(result);
        };
    } else if (name == "set!") {
        func = [](std::vector<std::shared_ptr<Object>> &args) {
            if (args.size() != 2) {
                throw SyntaxError("set! must contain 2 arguments");
            }
            std::shared_ptr<Object> result;
            if (Is<Name>(args[0])) {
                if (scope.count(As<Name>(args[0])->getName())) {
                    scope.set(As<Name>(args[0])->getName(), args[1]->eval());
                    result = scope.get(As<Name>(args[0])->getName());
                } else {
                    throw RuntimeError("unknown name: " + As<Name>(args[0])->getName());
                }
            } else {
                throw SyntaxError("invalid set! syntax");
            }
            return result;
        };
    } else if (name == "define") {
        func = [](std::vector<std::shared_ptr<Object>> &args) {
            if (args.size() != 2) {
                throw SyntaxError("define must contain 2 arguments");
            }
            std::shared_ptr<Object> result;
            if (Is<Name>(args[0])) {
                scope.add(As<Name>(args[0])->getName(), args[1]->eval());
                result = scope.get(As<Name>(args[0])->getName());
            } else if (Is<Cell>(args[0])) {
                auto decl = ConvertToVector(args[0]);
                if (!Is<Name>(decl[0])) {
                    throw SyntaxError("invalid function declaration");
                }
                std::vector<std::string> par;
                for (int i = 1; i < decl.size(); ++i) {
                    if (!Is<Name>(decl[i])) {
                        throw SyntaxError("invalid function declaration");
                    }
                    par.push_back(As<Name>(decl[i])->getName());
                }
                auto f = std::make_shared<Function>(args[1], par);
                scope.add(As<Name>(decl[0])->getName(), f);
                result = std::make_shared<Cell>();
            } else {
                throw SyntaxError("invalid define syntax");
            }
            return result;
        };
    } else if (name == "if") {
        func = [](std::vector<std::shared_ptr<Object>> &args) {
            if (args.size() == 0) {
                throw SyntaxError("expected condition in if statement");
            } else if (args.size() == 1) {
                throw SyntaxError("expected then branch in if statement");
            } else if (args.size() > 3) {
                throw SyntaxError("expected 2 or 3 arguments in if statement");
            }
            args[0] = args[0]->eval();
            std::shared_ptr<Object> to_ret;
            if (Is<Bool>(args[0])) {

            } else {
                throw RuntimeError("invalid condition");
            }

            if (As<Bool>(args[0])->getBool()) {
                return args[1]->eval();
            }
            if (args.size() == 2) {
                to_ret = std::make_shared<Cell>();
                return to_ret;
                //throw RuntimeError("Unspecified return value");
            }
            //CheckIfBadArgsCount(args, {}, {2, 3});
            return args[2]->eval();
        };
    } else if (name == "case") {
        func = [](std::vector<std::shared_ptr<Object>>& args) {
            CheckIfBadArgsCount(args, {0}, {});
            bool br = false;
            for (int i = 1; i < args.size() && !br; ++i)  {
                if (!Is<Cell>(args[i])) {
                    throw SyntaxError("invalid case statement");
                }
                auto curr = ConvertToVector(args[i]);
                CheckIfBadArgsCount(curr, {}, {2, 3});
                if (Is<Name>(curr[0])) {
                    if (As<Name>(curr[0])->getName() == "default") {
                        curr[1]->eval();
                        if (curr.size() == 3) {
                            if (Is<Name>(curr[2]) && As<Name>(curr[2])->getName() == "break") {
                                br = true;
                                break;
                            } else {
                                throw SyntaxError("invalid case statement");
                            }
                        }
                        continue;
                    }
                }
                if (equal(args[0]->eval(), curr[0]->eval())) {
                    curr[1]->eval();
                    if (curr.size() == 3) {
                        if (Is<Name>(curr[2]) && As<Name>(curr[2])->getName() == "break") {
                            br = true;
                            break;
                        } else {
                            throw SyntaxError("invalid case statement");
                        }
                    }
                }
            }
            return std::make_shared<Cell>();
        };
    } else if (name == "begin") {
        func = [](std::vector<std::shared_ptr<Object>>& args) {
            scope.extend();
            args = EvalVector(args);
            std::shared_ptr<Object> to_ret;
            if (args.empty()) {
                to_ret = std::make_shared<Cell>();
                scope.pop();
                return to_ret;
            }
            scope.pop();
            return args.back();
        };
    } else if (name == "min") {
        func = [](std::vector<std::shared_ptr<Object>>& args) {
            args = EvalVector(args);
            CheckIfValidTypes<Integer>(args);
            CheckIfBadArgsCount(args, {0}, {});
            std::shared_ptr<Object> to_ret;
            int64_t res = As<Integer>(args[0])->getValue();
            for (size_t i = 1; i < args.size(); ++i) {
                int64_t cur_val = As<Integer>(args[i])->getValue();
                res = std::min(res, cur_val);
            }
            to_ret = std::make_shared<Integer>(res);
            return to_ret;
        };
    } else if (name == "abs") {
        func = [](std::vector<std::shared_ptr<Object>>& args) {
            args = EvalVector(args);
            CheckIfValidTypes<Integer>(args);
            CheckIfBadArgsCount(args, {}, {1});
            std::shared_ptr<Object> to_ret;
            to_ret = std::make_shared<Integer>(std::abs(As<Integer>(args.front())->getValue()));
            return to_ret;
        };
    } else if (name == "even?"){
        func = [](std::vector<std::shared_ptr<Object>>& args) {
            args = EvalVector(args);
            CheckIfValidTypes<Integer>(args);
            CheckIfBadArgsCount(args, {}, {1});
            std::shared_ptr<Object> to_ret;
            to_ret = std::make_shared<Bool>(As<Integer>(args[0])->getValue() % 2 == 0);
            return to_ret;
        };
    } else if (name == "odd?") {
        func = [](std::vector<std::shared_ptr<Object>>& args) {
            args = EvalVector(args);
            CheckIfValidTypes<Integer>(args);
            CheckIfBadArgsCount(args, {}, {1});
            std::shared_ptr<Object> to_ret;
            to_ret = std::make_shared<Bool>(As<Integer>(args[0])->getValue() % 2 == 1);
            return to_ret;
        };
    } /*BOOLEAN FUNCTIONS*/ else if (name == "boolean?") {
        func = [](std::vector<std::shared_ptr<Object>>& args) {
            args = EvalVector(args);
            std::shared_ptr<Object> to_ret;
            to_ret = std::make_shared<Bool>(Is<Bool>(args.front()));
            return to_ret;
        };
    } else if (name == "not") {
        func = [](std::vector<std::shared_ptr<Object>>& args) {
            args = EvalVector(args);
            CheckIfBadArgsCount(args, {}, {1});
            std::shared_ptr<Object> to_ret;
            bool value = false;
            if (Is<Bool>(args.front())) {
                value = !As<Bool>(args.front())->getBool();
            }
            to_ret = std::make_shared<Bool>(value);
            return to_ret;
        };
    } else if (name == "and") {
        func = [](std::vector<std::shared_ptr<Object>>& args) {
            std::shared_ptr<Object> to_ret = std::make_shared<Bool>(true);
            for (auto elem : args) {
                auto evaluated = elem->eval();
                if (Is<Bool>(evaluated) && !As<Bool>(evaluated)->getBool()) {
                    to_ret = evaluated;
                    break;
                }
                to_ret = evaluated;
            }
            return to_ret;
        };
    } else if (name == "or") {
        func = [](std::vector<std::shared_ptr<Object>>& args) {
            std::shared_ptr<Object> to_ret = std::make_shared<Bool>(false);
            for (auto elem : args) {
                auto evaluated = elem->eval();
                if (Is<Bool>(evaluated) && !As<Bool>(evaluated)->getBool()) {
                } else {
                    to_ret = evaluated;
                    break;
                }
            }
            return to_ret;
        };
    } /*LIST OPERATIONS*/ else if (name == "pair?") {
        func = [](std::vector<std::shared_ptr<Object>>& args) {
            args = EvalVector(args);
            CheckIfBadArgsCount(args, {}, {1});
            CheckIfValidTypes<Cell>(args);
            std::shared_ptr<Object> to_ret;
            auto temp = ConvertToVector(args.front());
            bool res = false;
            if (temp.size() == 2) {
                res = true;
            }
            to_ret = std::make_shared<Bool>(res);
            return to_ret;
        };
    } else if (name == "null?") {
        func = [](std::vector<std::shared_ptr<Object>>& args) {
            args = EvalVector(args);
            CheckIfBadArgsCount(args, {}, {1});
            CheckIfValidTypes<Cell>(args);
            std::shared_ptr<Object> to_ret;
            auto temp = As<Cell>(args.front());
            bool res = false;
            if (temp->getFirst() == nullptr && temp->getSecond() == nullptr) {
                res = true;
            }
            to_ret = std::make_shared<Name>("+");
            return to_ret;
        };
    } else if (name == "list?") {
        func = [](std::vector<std::shared_ptr<Object>>& args) {
            args = EvalVector(args);
            CheckIfBadArgsCount(args, {}, {1});
            std::shared_ptr<Object> to_ret;
            if (Is<Cell>(args[0])) {
                bool res = false;
                auto cell = As<Cell>(args.front())->getFirst();
                if (cell == nullptr) {
                    res = true;
                } else {
                    while (true) {
                        auto ptr_first = As<Cell>(cell)->getFirst();
                        auto ptr_second = As<Cell>(cell)->getSecond();
                        if (ptr_second == nullptr) {
                            res = true;
                            break;
                        }
                        if (Is<Integer>(ptr_second)) {
                            break;
                        }
                        cell = ptr_second;
                    }
                }
                to_ret = std::make_shared<Bool>(res);
            } else {
                to_ret = std::make_shared<Bool>(false);
            }
            return to_ret;
        };
    } else if (name == "cons") {
        func = [](std::vector<std::shared_ptr<Object>>& args) {
            CheckIfBadArgsCount(args, {}, {2});
            args = EvalVector(args);
            //std::shared_ptr<Object> to_ret;
            //auto dot = std::make_shared<Dot>();
            //args.insert(++args.begin(), dot);
            for (int i = 0; i < 2; ++i) {
                if (!Is<Cell>(args[i])) {
                    auto tmp = std::make_shared<Cell>();
                    tmp->getFirst() = args[i];
                    args[i] = tmp;
                }
            }
            auto first = ConvertToVector(args[0]);
            auto second = ConvertToVector(args[1]);
            for (int i = 0; i < second.size(); ++i) {
                first.push_back(second[i]);
            }
            return ListASTFromVector(first);
        };
    } else if (name == "length") {
        func = [](std::vector<std::shared_ptr<Object>> &args) {
            args = EvalVector(args);
            CheckIfBadArgsCount(args, {}, {1});
            CheckIfValidTypes<Cell>(args);
            args = ConvertToVector(args.front());
            return std::make_shared<Integer>(args.size());
        };
    } else if (name == "car") {
        func = [](std::vector<std::shared_ptr<Object>> &args) {
            args = EvalVector(args);
            CheckIfBadArgsCount(args, {}, {1});
            CheckIfValidTypes<Cell>(args);
            args = ConvertToVector(args.front());
            CheckIfBadArgsCount(args, {0}, {});
            std::shared_ptr<Object> to_ret = args.front();
            return to_ret;
        };
    } else if (name == "cdr") {
        func = [](std::vector<std::shared_ptr<Object>>& args) {
            args = EvalVector(args);
            CheckIfBadArgsCount(args, {}, {1});
            CheckIfValidTypes<Cell>(args);
            std::shared_ptr<Object> to_ret;
            auto temp_vec = ConvertToVector(args.front());
            CheckIfBadArgsCount(temp_vec, {0}, {});
            if (temp_vec.size() == 1) {
                to_ret = std::make_shared<Cell>();
                return to_ret;
            }
            if (temp_vec.size() >= 2) {
                return As<Cell>(args.front())->getSecond();
            }
        };
    } else if (name == "list") {
        func = [](std::vector<std::shared_ptr<Object>>& args) {
            args = EvalVector(args);
            std::shared_ptr<Object> to_ret;
            return ListASTFromVector(args);
        };
    } else if (name == "list-ref") {
        func = [](std::vector<std::shared_ptr<Object>>& args) {
            args = EvalVector(args);
            CheckIfBadArgsCount(args, {}, {2});
            if (!Is<Cell>(args.front()) || !Is<Integer>(args.back())) {
                throw RuntimeError("type mismatch");
            }
            int ind = As<Integer>(args.back())->getValue();
            auto list_vec = ConvertToVector(args.front());
            if (ind >= list_vec.size() || ind < 0) {
                throw RuntimeError("index out of range");
            }
            std::shared_ptr<Object> to_ret = list_vec[ind];
            return to_ret;
        };
    } else if (name == "list-tail") {
        func = [](std::vector<std::shared_ptr<Object>>& args) {
            std::shared_ptr<Object> to_ret;
            args = EvalVector(args);
            CheckIfBadArgsCount(args, {}, {2});
            if (!Is<Cell>(args.front()) || !Is<Integer>(args.back())) {
                throw RuntimeError("type mismatch");
            }
            int ind = As<Integer>(args.back())->getValue();
            auto list_vec = ConvertToVector(args.front());
            if (ind > list_vec.size() || ind < 0) {
                throw RuntimeError("index out of range");
            }
            std::vector<std::shared_ptr<Object>> res;
            for (int i = ind; i < list_vec.size(); ++i) {
                res.push_back(list_vec[i]);
            }
            return ListASTFromVector(res);
        };
    } else if (name == "list-set") {
        func = [](std::vector<std::shared_ptr<Object>>& args) {
            std::shared_ptr<Object> to_ret;
            args = EvalVector(args);
            CheckIfBadArgsCount(args, {}, {3});
            if (!Is<Cell>(args.front()) || !Is<Integer>(args[1])) {
                throw RuntimeError("type mismatch");
            }
            int ind = As<Integer>(args[1])->getValue();
            auto list_vec = ConvertToVector(args.front());
            if (ind > list_vec.size() || ind < 0) {
                throw RuntimeError("index out of range");
            }
            list_vec[ind] = std::make_shared<Integer>(As<Integer>(args.back())->getValue());
            return ListASTFromVector(list_vec);
        };
    } else if (scope.count(name)) {

    } else {
        throw RuntimeError("unknown name: " + name);
    }
}

std::shared_ptr<Object> Function::eval() {
    return nullptr;
}

std::string Function::serialize() {
    return "";
}

std::shared_ptr<Object> Function::apply(std::vector<std::shared_ptr<Object>> args) {
    return func(args);
}

Function::Function(std::shared_ptr<Object> body, std::vector<std::string> params) {
    func = [params, body](std::vector<std::shared_ptr<Object>>& args) {
        scope.re();
        if (args.size() != params.size()) {
            throw RuntimeError("wrong args number in function");
        }
        //args = EvalVector(args);
        for (int i = 0; i < args.size(); ++i) {
            scope.add(params[i], args[i]);
        }
        std::shared_ptr<Object> result = body->eval();
        scope.pop();
        return result;
    };
}
