#ifndef LIBRARY_MAIN_CLASS_H
#define LIBRARY_MAIN_CLASS_H

#include <iterator>
#include <map>
#include <string>
#include <set>
#include <vector>

#include "author.h"
#include "book.h"

class Library {
public:
    explicit Library(const std::string& name);

    void AddBook(Book& new_book, size_t ind = 0u);

    void AddBook(const std::string& new_book_title, const std::string& authors = "");

//    void AddAuthor(const std::string& author);

//    void AddAuthor(const Author& author);

    void AddGanres();

    void Clear();

    void ChangeLibName(const std::string& new_name);

    void DeleteBook(size_t index);

    void Saved();

    const std::string& GetLibName() const;

    const std::map<size_t, Book>& GetBooks() const;

    const std::map<Author, std::set<size_t>>& GetAuthors() const;

    size_t GetOwnedAmount() const;

    size_t GetWishedAmount() const;

    bool Contains(const std::string&) const;

    bool Edited() const;

    std::vector<std::pair<size_t, BookAttribute>> Search(const std::string& request);

    const Book& At(const int) const;

    size_t Size() const;

    [[nodiscard]] std::map<size_t, Book>::iterator begin();

    [[nodiscard]] std::map<size_t, Book>::const_iterator cbegin() const;

    [[nodiscard]] std::map<size_t, Book>::const_iterator begin() const;

    [[nodiscard]] std::map<size_t, Book>::iterator end();

    [[nodiscard]] std::map<size_t, Book>::const_iterator cend() const;

    [[nodiscard]] std::map<size_t, Book>::const_iterator end() const;

private:
    std::vector<size_t> SearchForRequestWithMistakes(const std::string& request) const;

private:
    std::string library_name_ = "default";

    std::map<size_t, Book> books_;
    std::map<std::string, size_t> purified_titles_;
    size_t owned_amount_ = 0u;
    size_t wished_amount_ = 0u;

    bool modified_ = false;

    std::map<Author, std::set<size_t>> authors_;

    std::set<std::string> ganres_;
};

#endif // LIBRARY_MAIN_CLASS_H
