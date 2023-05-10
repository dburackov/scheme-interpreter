#pragma once

#include "Tokenizer.h"

#include <memory>
#include <string>
#include <memory>
#include <vector>
#include <functional>

class Object : public std::enable_shared_from_this<Object> {
public:
    virtual ~Object() = default;
    virtual std::shared_ptr<Object> eval() = 0;
    virtual std::string serialize() = 0;
};

class Integer : public Object {
public:
    explicit Integer(int val);
    std::shared_ptr<Object> eval() override;
    std::string serialize() override;

    int getValue() const;

private:
    int value;
};

class Name : public Object {
public:
    explicit Name(std::string name);

    std::shared_ptr<Object> eval() override;
    std::string serialize() override;

    const std::string& getName() const;

private:
    std::string name;
};

class Bool : public Object {
public:
    explicit Bool(std::string val);
    explicit Bool(bool val);

    std::shared_ptr<Object> eval() override;
    std::string serialize() override;

    bool getBool() const;

private:
    bool value;
};

class Char : public Object {
public:
    explicit Char(char val);

    std::shared_ptr<Object> eval() override;
    std::string serialize() override;

    char getChar() const;

private:
    char value;
};

class Function : public Object {
public:
    explicit Function(std::string name);

    Function(std::shared_ptr<Object>, std::vector<std::string>);

    std::shared_ptr<Object> eval() override;
    std::string serialize() override;

    std::shared_ptr<Object> apply(std::vector<std::shared_ptr<Object>> args);

private:
    std::function<std::shared_ptr<Object>(std::vector<std::shared_ptr<Object>>&)> func;

//    std::shared_ptr<Object> body;
//    std::vector<std::string> params;
};

//class Func : public Object {
//public:
//    explicit Func();
//
//    std::shared_ptr<Object> eval() override;
//    std::string serialize() override;
//
//private:
//    std::shared_ptr<Object> body;
//    std::vector<std::string> args;
//};

class Dot : public Object {
public:
    Dot();
    std::shared_ptr<Object> eval() override;
    std::string serialize() override;
};

class Quote : public Object {
public:
    explicit Quote(std::shared_ptr<Object> val);

    std::shared_ptr<Object> eval() override;
    std::string serialize() override;

    std::shared_ptr<Object> getValue();

private:
    std::shared_ptr<Object> value;
};

class Cell : public Object {
public:
    Cell();

    std::shared_ptr<Object> eval() override;
    std::string serialize() override;

    std::shared_ptr<Object> getFirst() const;
    std::shared_ptr<Object> getSecond() const;
    std::shared_ptr<Object>& getFirst();
    std::shared_ptr<Object>& getSecond();

private:
    std::shared_ptr<Object> first = nullptr;
    std::shared_ptr<Object> second = nullptr;
};

//===============================================//

template <class T>
std::shared_ptr<T> As(const std::shared_ptr<Object>& obj) {
    return std::dynamic_pointer_cast<T>(obj);
}

template <class T>
bool Is(const std::shared_ptr<Object>& obj) {
    auto ptr = std::dynamic_pointer_cast<T>(obj);
    if (ptr) {
        return true;
    }
    return false;
}


