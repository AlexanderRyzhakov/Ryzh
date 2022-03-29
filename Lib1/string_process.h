#ifndef STRING_PROCESS_H
#define STRING_PROCESS_H

#include <string>
#include <vector>
#include <set>

#include <QString>

template <typename Container>
std::string WordContainerToStringWihtSpaces (const Container& container) {
    std::string result;
    bool first_word = true;

    for (const auto& i : container) {
        if (!first_word) {
            result += ' ';
        }
        first_word = false;
        result += i;
    }

    return result;
}

std::string CleanString(const std::string& line);

std::vector<std::string> LineToVectorString(const std::string& input_text);

std::set<std::string> LineToSetString(const std::string& input_text);

void RemoveNewLineSymbol(std::string& text);

#endif // STRING_PROCESS_H
