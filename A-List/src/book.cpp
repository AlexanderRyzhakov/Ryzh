#include <algorithm>
#include <cassert>

#include "book.h"

using namespace std;

Book::Book (const std::string& new_title, const std::string& new_authors = "") : title_(new_title)  {
    if (count(new_title.begin(), new_title.end(), '\n')
        || count(new_authors.begin(), new_authors.end(), '\n')) {
        throw invalid_argument("\\n in text");
    }
    if (new_title.empty()) {
        throw std::invalid_argument("No book name prvided");
    }
    if (!new_authors.empty()) {
        SetAuthors(new_authors);
    }
}

// -------------------------------------------------------------------------------- set atributes
void Book::SetAuthors(const std::string& authors) {
    auto left = authors.begin();
    for (auto i = left; i <= authors.end(); ++i) {
        if ( (*i == ',') || (i == authors.end()) ) {
            string name;
            name.assign(left, i);
            Author author(move(name));
            if (!author.GetName().empty()) {
                authors_.emplace(move(author));
            }
            left = ++i;
        }
    }
}

void Book::SetComment(const std::string& text)
{
    comment_ = text;
}

void Book::SetRating(uint8_t new_rating){
    rating = new_rating;
}

void Book::SetOwned()
{
    owned_ = true;
    wish_list_ = false;
}

void Book::SetWished()
{
    owned_ = false;
    wish_list_ = true;
}

void Book::SetGanres()
{

}

// ------------------------------------------------------------------------------------ get atributes

const set<Author>& Book::GetAuthors() const {
    return authors_;
}

string Book::GetAuthorsString() const
{
    string result;
    bool first = true;
    for (const auto& author : authors_) {
        if (!first) {
            result += ", ";
        }
        result += author.GetName();
        first = false;
    }
    return result;
}


const std::string& Book::GetComment() const {
    return comment_;
}

const string& Book::GetTitle() const {
    return title_;
}

int Book::GetRating() const
{
    return rating;
}

bool Book::IsOwned() const
{
    assert(owned_ != wish_list_);
    return owned_;
}

bool Book::IsWished() const
{
    assert(owned_ != wish_list_);
    return wish_list_;
}

//    std::set GetGanres();
