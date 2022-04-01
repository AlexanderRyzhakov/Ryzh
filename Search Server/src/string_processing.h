#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "document.h"
#include "paginator.h"

template<typename T>
std::ostream& operator<<(std::ostream &os, const std::vector<T> &constr) {
    os << "[";
    bool start = true;
    for (const auto &el : constr) {
        if (start == false) {
            os << ", ";
        }
        start = false;
        os << el;
    }
    os << "]";
    return os;
}

template<typename It>
std::ostream& operator<<(std::ostream &os, const IteratorRange<It> &range) {
    for (It left = range.begin(); left != range.end(); ++left) {
        os << *left;
    }
    return os;
}

std::ostream& operator<<(std::ostream &os, const Document &doc);

void PrintDocument(const Document& document);
