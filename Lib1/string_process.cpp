#include "string_process.h"

#include <string>
#include <vector>
#include <set>

#include <QString>

using namespace std;

vector<string> LineToVectorString(const string &input_text) {
    vector<string> result;
    string word;
    for (const char c : input_text) {
        if (c == ' ') {
            if (!word.empty()) {
                result.push_back(word);
                word.clear();
            }
        } else {
            word += c;
        }
    }
    if (!word.empty()) {
        result.push_back(word);
    }

    return result;
}

set<string> LineToSetString(const string& input_text) {
    set<string> result;
    for (const auto& i: LineToVectorString(input_text)) {
        result.emplace(i);
    }
    return result;
}

string CleanString(const string& line)
{
    string str;
    std::set<char> acceptable_symblos = { '~', '!', '@', '#', '$', '%', '^', '&', '(', ')', '+', '-', '_' };
    for (char c : line) {
        if ((c >= 'a' && c <= 'z')
            || (c >= -32 && c <= -1)
            || (c >= '0' && c <= '9')
            || acceptable_symblos.count(c)) {
            str += static_cast<char>(c);
        } else if (c >= 'A' && c <= 'Z') {
            str += static_cast<char>(c + ('a' - 'A'));
        } else if (c >= -64 && c <= -33) {
            str += static_cast<char>(c + 32);
        }
    }
    return str;
}

void RemoveNewLineSymbol(std::string& text)
{
    auto itr = find(text.begin(), text.end(), '\n');
    while (itr != text.end()) {
        text.erase(itr);
        itr = find(text.begin(), text.end(), '\n');
    }
}

