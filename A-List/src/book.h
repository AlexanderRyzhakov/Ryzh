#ifndef BOOK_H
#define BOOK_H

#include <string>
#include <set>

#include "author.h"

enum class BookAttribute {
    ID,
    TITLE,
    AUTHORS,
    RATING,
    OWNED,
    WISHED,
    COMMENT
};

class Book {
public:
    explicit Book(const std::string& new_title, const std::string& new_authors);


    void SetAuthors(const std::string&);

    void SetGanres();

    void SetComment(const std::string&);

    void SetRating(uint8_t new_rating);

    void SetOwned();

    void SetWished();


    bool IsOwned() const;

    bool IsWished() const;


    const std::set<Author>& GetAuthors() const;

    std::string GetAuthorsString() const;

    const std::string& GetComment() const;

    const std::string& GetTitle() const;

    int GetRating() const;

    const std::set<std::string>& GetGanres() const;

private:
    std::string title_;
    std::set<Author> authors_ = {};
    std::set<std::string> ganres_ = {};
    std::string comment_ = "";
    uint8_t rating = 0;

    bool owned_ = true;
    bool wish_list_ = false;
};

#endif // BOOK_H
