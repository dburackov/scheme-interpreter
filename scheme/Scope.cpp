#include "Scope.h"

Scope::Scope() {
    table = std::vector<std::unordered_map<std::string, std::shared_ptr<Object>>>(1);
    parent.push_back(0);
}

void Scope::add(const std::string& name, std::shared_ptr<Object> value) {
    table.back()[name] = value;
}


bool Scope::set(const std::string& name, std::shared_ptr<Object> value) {
    bool result = false;
    int curr = table.size() - 1;
    while (true) {
        if (table[curr].count(name)) {
            table[curr][name] = value;
            result = true;
            break;
        }
        if (curr == 0) {
            break;
        }
        curr = parent[curr];
    }
    return result;
}

std::shared_ptr<Object> Scope::get(const std::string& name) {
    int curr = table.size() - 1;
    while (true) {
        if (table[curr].count(name)) {
            return table[curr][name];
        }
        curr = parent[curr];
    }
}

void Scope::extend() {
    table.push_back(std::unordered_map<std::string, std::shared_ptr<Object>>());
    parent.push_back(parent.size() - 1);
}

void Scope::re() {
    table.push_back(std::unordered_map<std::string, std::shared_ptr<Object>>());
    parent.push_back(0);
}

void Scope::pop() {
    table.pop_back();
    parent.pop_back();
}

int Scope::count(const std::string& name) {
    int result = 0;
    int curr = table.size() - 1;
    while (true) {
        result += table[curr].count(name);
        if (curr == 0) {
            break;
        }
        curr = parent[curr];
    }
    return result;
}
