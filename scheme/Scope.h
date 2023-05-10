#pragma once

#include "Object.h"

class Scope {
public:
    Scope();

    void add(const std::string&, std::shared_ptr<Object>);

    bool set(const std::string&, std::shared_ptr<Object>);

    std::shared_ptr<Object> get(const std::string&);

    void extend();

    void re();

    void pop();

    int count(const std::string&);

private:
    std::vector<std::unordered_map<std::string, std::shared_ptr<Object>>> table;
    std::vector<int> parent;
};

