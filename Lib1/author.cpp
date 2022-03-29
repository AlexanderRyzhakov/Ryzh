#include "author.h"
#include "string_process.h"

using namespace std;

Author::Author(const std::string &text) : name_(LineToVectorString(text)) {
}

string Author::GetName() const {
    return WordContainerToStringWihtSpaces(name_);
}

void Author::AddBookToAuthor(size_t id)
{
    books_.emplace(id);
}

const std::set<size_t>& Author::GetAuthorBooks() const
{
    return books_;
}

bool operator<(const Author &author1, const Author &author2) {
    auto first = CleanString(WordContainerToStringWihtSpaces(LineToSetString(author1.GetName())));
    auto second = CleanString(WordContainerToStringWihtSpaces(LineToSetString(author2.GetName())));
    return first < second;
}

bool operator==(const Author &author1, const Author &author2) {
    auto first = CleanString(WordContainerToStringWihtSpaces(LineToSetString(author1.GetName())));
    auto second = CleanString(WordContainerToStringWihtSpaces(LineToSetString(author2.GetName())));
    return first == second;
}
