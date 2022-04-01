#pragma once

#include <stdexcept>
#include <vector>

template<typename Iterator>
class IteratorRange {
public:
    IteratorRange(Iterator left, Iterator right) :
        first_(left), last_(right), size_(distance(left, right)) {
    }

public:
    Iterator begin() const {
        return first_;
    }

    Iterator end() const {
        return last_;
    }

    int size() const {
        return size_;
    }
private:
    Iterator first_;
    Iterator last_;
    int size_;
};

template<typename Iterator>
class Paginator {
public:
    explicit Paginator(Iterator left, Iterator right, int page_size) {
        if (page_size <= 0) {
            throw std::invalid_argument("Incorrect page size");
        }
        while (left < right) {
            if (distance(left, right) >= page_size) {
                pages_.push_back( { left, left + page_size });
                advance(left, page_size);
            } else {
                pages_.push_back( { left, right });
                break;
            }
        }
    }

public:
    auto begin() const {
        return pages_.begin();
    }

    auto end() const {
        return pages_.end();
    }

    int size() const {
        return pages_.size();
    }

private:
    std::vector<IteratorRange<Iterator>> pages_;
};

template<typename Container>
auto Paginate(const Container &c, int page_size) {
    return Paginator(begin(c), end(c), page_size);
}
