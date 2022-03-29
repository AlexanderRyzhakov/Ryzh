#include <algorithm>
#include <cassert>
#include <map>
#include <stdexcept>
#include <string>
#include <set>
#include <vector>
#include <utility>

#include "author.h"
#include "book.h"
#include "library_main_class.h"
#include "string_process.h"


using namespace std;
//public:
Library::Library(const string& name)
    : library_name_(name)
{
    if (name.empty() || count(name.begin(), name.end(), '\n')) {
        throw invalid_argument("No library name prvided");
    }
}

void Library::AddBook(Book& new_book, size_t input_ind)
{
    if (Contains(new_book.GetTitle())) {
        throw invalid_argument("Book already added");
    }

    size_t index;
    if (input_ind == 0) {
        index = books_.empty() ? 1u : books_.rbegin()->first + 1u;
    } else {
        index = input_ind;
    }

    assert(!books_.count(index));

    books_.emplace(index, new_book);
    purified_titles_.emplace(CleanString(move(new_book.GetTitle())), index);

    for (const auto& author : new_book.GetAuthors()) {
        authors_[author].emplace(index);
    }

    owned_amount_ += new_book.IsOwned() ? 1 : 0;
    wished_amount_ += new_book.IsWished() ? 1 : 0;

    assert(books_.size() == purified_titles_.size());
    assert(books_.size() == owned_amount_ + wished_amount_);

    modified_ = true;
}

void Library::AddBook(const string& new_book_title, const string& authors)
{
    Book book(std::move(new_book_title), std::move(authors));
    AddBook(book);
}


//void Library::AddAuthor(const string& author)
//{
//    authors_.emplace(author);
//    modified_ = true;
//}

//void Library::AddAuthor(const Author& author)
//{
//    authors_.emplace(author);
//    modified_ = true;
//}

void Library::AddGanres()
{
    modified_ = true;
}

void Library::Clear()
{
    library_name_ = "default";
    purified_titles_.clear();
    ganres_.clear();
    books_.clear();
    authors_.clear();
    owned_amount_ = 0u;
    wished_amount_ = 0u;
    assert(books_.size() == 0 && purified_titles_.size() == 0);
    modified_ = false;
}

void Library::ChangeLibName(const string& new_name)
{
    if (new_name.empty()) {
        library_name_ = "default";
        return;
    }
    if (count(new_name.begin(), new_name.end(), '\n')) {
        throw invalid_argument("\\n in text");
    }
    library_name_ = move(new_name);
    modified_ = true;
}

void Library::DeleteBook(size_t index)
{
    // add delete function for:
    //    std::set<std::string> ganres_;

    auto delete_itr = books_.find(index);

    if (delete_itr->second.IsOwned()) {
        --owned_amount_;
    } else {
        --wished_amount_;
    }
    purified_titles_.erase(CleanString(delete_itr->second.GetTitle()));
    books_.erase(delete_itr);

    assert(books_.size() == purified_titles_.size());
    assert(books_.size() == owned_amount_ + wished_amount_);

    for (auto& [author, books] : authors_) {
        if (books.count(index)) {
            books.erase(index);
            if (books.empty()) {
                authors_.erase(author);
            }
        }
    }

    modified_ = true;
}


void Library::Saved()
{
    modified_ = false;
}


const string& Library::GetLibName() const {
    return library_name_;
}

const map<size_t, Book>& Library::GetBooks() const
{
    assert(books_.size() == purified_titles_.size());
    return books_;
}

const map<Author, set<size_t>>& Library::GetAuthors() const {
    return authors_;
}

size_t Library::GetOwnedAmount() const
{
    return owned_amount_;
}

size_t Library::GetWishedAmount() const
{
    return wished_amount_;
}

bool Library::Contains(const std::string& line) const
{
    return purified_titles_.count(CleanString(line));
}

bool Library::Edited() const
{
    return (!books_.empty() && modified_);
}

vector<pair<size_t, BookAttribute>> Library::Search(const std::string& request)
{
    vector<pair<size_t, BookAttribute>> result;
    if (request.empty()) {
        return result;
    }
    set<size_t> found_id;
    if (Contains(request)) {
        result.push_back({ purified_titles_.at(CleanString(request)), BookAttribute::TITLE });
        found_id.emplace(purified_titles_.at(CleanString(request)));
    }
    set<string> request_words = LineToSetString(request);
    for (const auto& word : request_words) {
        for (const auto& [id, book] : books_) {
            string title = CleanString(book.GetTitle());
            string author = CleanString(book.GetAuthorsString());
            string comment = CleanString(book.GetComment());

            if (title.find(CleanString(word)) != title.npos && !found_id.count(id)) {
                result.push_back({ id, BookAttribute::TITLE });
                found_id.emplace(id);
            } else if (author.find(CleanString(word)) != author.npos && !found_id.count(id)) {
                result.push_back({ id, BookAttribute::AUTHORS });
                found_id.emplace(id);
            } else if (comment.find(CleanString(word)) != comment.npos && !found_id.count(id)) {
                result.push_back({ id, BookAttribute::COMMENT });
                found_id.emplace(id);
            }
        }
    }
    return result;
}

const Book& Library::At(const int index) const
{
    return books_.at(index);
}

size_t Library::Size() const
{
    return books_.size();
}

map<size_t, Book>::iterator Library::begin()
{
    assert(books_.size() == purified_titles_.size());
    return books_.begin();
}

map<size_t, Book>::const_iterator Library::cbegin() const
{
    assert(books_.size() == purified_titles_.size());
    return books_.cbegin();
}

map<size_t, Book>::const_iterator Library::begin() const
{
    return cbegin();
}

map<size_t, Book>::iterator Library::end()
{
    return books_.end();
}

map<size_t, Book>::const_iterator Library::cend() const
{
    return books_.cend();
}

map<size_t, Book>::const_iterator Library::end() const
{
    return cend();
}

// private:
vector<size_t> Library::SearchForRequestWithMistakes(const string &request) const {
    vector<size_t> best_fit = { };

    return best_fit;
}
