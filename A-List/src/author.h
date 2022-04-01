#ifndef AUTHOR_H
#define AUTHOR_H

#include <set>
#include <string>
#include <vector>

class Author {
public:
    explicit Author(const std::string& text);

    void AddBookToAuthor(size_t id);

    const std::set<size_t>& GetAuthorBooks() const;

    std::string GetName() const;

private:
    std::vector<std::string> name_;
    std::set<size_t> books_ = {};
};

bool operator<(const Author &author1, const Author &author2);

bool operator==(const Author &author1, const Author &author2);
#endif // AUTHOR_H
