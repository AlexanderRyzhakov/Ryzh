#pragma once

#include <algorithm>
#include <cassert>
#include <initializer_list>

template<typename Type>
class SingleLinkedList {

    template<typename ValueType>
    class BasicIterator;

public:
    using Iterator = BasicIterator<Type>;
    using ConstIterator = BasicIterator<const Type>;

public: //--------------------------contructors & destructor-------------------------
    SingleLinkedList() : head_(), size_(0u) {
    }

    SingleLinkedList(std::initializer_list<Type> values) {
        Assign(values.begin(), values.end());
    }

    SingleLinkedList(const SingleLinkedList<Type> &other) {
        Assign(other.begin(), other.end());
    }

    SingleLinkedList& operator=(const SingleLinkedList &rhs) {
        if (this != &rhs) {
            auto rhs_copy(rhs);
            swap(rhs_copy);
        }
        return *this;
    }

    ~SingleLinkedList() {
        Clear();
    }

public: // -----------------------------------------Iterators---------------------------
    [[nodiscard]] Iterator before_begin() noexcept {
        return Iterator { &head_ };
    }

    [[nodiscard]] ConstIterator cbefore_begin() const noexcept {
        return ConstIterator { const_cast<Node*>(&head_) };
    }

    [[nodiscard]] ConstIterator before_begin() const noexcept {
        return cbefore_begin();
    }

    [[nodiscard]] Iterator begin() noexcept {
        return Iterator { head_.next_node };
    }

    [[nodiscard]] ConstIterator cbegin() const noexcept {
        return ConstIterator { head_.next_node };
    }

    [[nodiscard]] ConstIterator begin() const noexcept {
        return cbegin();
    }

    [[nodiscard]] Iterator end() noexcept {
        return Iterator { };
    }

    [[nodiscard]] ConstIterator end() const noexcept {
        return ConstIterator { };
    }

    [[nodiscard]] ConstIterator cend() const noexcept {
        return ConstIterator { };
    }

public: //----------------------------------class methods-------------------------------
    void Clear() noexcept {
        Node *next_node_to_delete_ptr = head_.next_node;

        while (next_node_to_delete_ptr) {
            Node *tmp_pointer = next_node_to_delete_ptr->next_node;
            delete next_node_to_delete_ptr;
            next_node_to_delete_ptr = tmp_pointer;
        }

        head_.next_node = nullptr;
        size_ = 0u;
    }

    [[nodiscard]] size_t GetSize() const noexcept {
        return size_;
    }

    Iterator InsertAfter(ConstIterator pos, const Type &value) {
        assert(pos.node_);

        pos.node_->next_node = new Node(value, pos.node_->next_node);
        ++size_;
        return Iterator(pos.node_->next_node);
    }

    Iterator EraseAfter(ConstIterator pos) noexcept {
        assert(!IsEmpty());
        assert(pos.node_->next_node);

        Node *tmp = pos.node_->next_node;
        pos.node_->next_node = tmp->next_node;
        delete tmp;
        --size_;
        return Iterator(pos.node_->next_node);
    }

    [[nodiscard]] bool IsEmpty() const noexcept {
        return head_.next_node ? false : true;
    }

    void PopFront() noexcept {
        assert(!IsEmpty());

        Node *delete_ptr = head_.next_node;
        head_.next_node = delete_ptr->next_node;
        delete delete_ptr;
        --size_;
    }

    void PushFront(const Type &value) {
        head_.next_node = new Node(value, head_.next_node);
        ++size_;
    }

    void swap(SingleLinkedList<Type> &other) noexcept {
        std::swap(head_.next_node, other.head_.next_node);
        std::swap(size_, other.size_);
    }

private:
    using value_type = Type;
    using reference = value_type&;
    using const_reference = const value_type&;

private:
    struct Node {
        Node() = default;

        Node(const Type &new_value, Node *next) : value(new_value), next_node(next) {
        }

        Type value;
        Node *next_node = nullptr;
    };

    template<typename ValueType>
    class BasicIterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;

        using value_type = Type;
        using pointer = ValueType*;
        using reference = ValueType&;

        BasicIterator() = default;

        BasicIterator(const BasicIterator<value_type> &other) noexcept {
            node_ = other.node_;
        }

        BasicIterator& operator=(const BasicIterator &rhs) = default;

        [[nodiscard]] bool operator==(const BasicIterator<const value_type> &rhs) const noexcept {
            return node_ == rhs.node_;
        }

        [[nodiscard]] bool operator!=(const BasicIterator<const value_type> &rhs) const noexcept {
            return !(node_ == rhs.node_);
        }

        [[nodiscard]] bool operator==(const BasicIterator<value_type> &rhs) const noexcept {
            return node_ == rhs.node_;
        }

        [[nodiscard]] bool operator!=(const BasicIterator<value_type> &rhs) const noexcept {
            return !(node_ == rhs.node_);
        }

        BasicIterator& operator++() noexcept {
            node_ = node_ ? node_->next_node : node_;
            return *this;
        }

        BasicIterator operator++(int) noexcept {
            auto next_pointer(*this);
            ++(*this);
            return next_pointer;
        }

        [[nodiscard]] reference operator*() const noexcept {
            return node_->value;
        }

        [[nodiscard]] pointer operator->() const noexcept {
            return &node_->value;
        }

    private:
        friend class SingleLinkedList;

        explicit BasicIterator(Node *node) : node_(node) {
        }

    private:
        Node *node_ = nullptr;
    };

private:
    template<typename InputIterator>
    void Assign(InputIterator from, InputIterator to) {
        assert(IsEmpty());
        assert(size_ == 0u);

        SingleLinkedList<Type> tmp;
        Node *tmp_destination = &tmp.head_;
        size_t tmp_size = 0u;
        for (; from != to; ++from) {
            tmp_destination->next_node = new Node(*from, nullptr);
            tmp_destination = tmp_destination->next_node;
            ++tmp_size;
        }
        swap(tmp);
        size_ = tmp_size;
    }

private:
    Node head_;
    size_t size_ = 0;
};

//---------------------------------------bools for SingleLinkedList-----------------------
template<typename Type>
bool operator==(const SingleLinkedList<Type> &lhs, const SingleLinkedList<Type> &rhs) {
    return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template<typename Type>
bool operator!=(const SingleLinkedList<Type> &lhs, const SingleLinkedList<Type> &rhs) {
    return !(lhs == rhs);
}

template<typename Type>
bool operator<(const SingleLinkedList<Type> &lhs, const SingleLinkedList<Type> &rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template<typename Type>
bool operator>(const SingleLinkedList<Type> &lhs, const SingleLinkedList<Type> &rhs) {
    return rhs < lhs;
}

template<typename Type>
bool operator<=(const SingleLinkedList<Type> &lhs, const SingleLinkedList<Type> &rhs) {
    return (lhs < rhs) || (lhs == rhs);
}

template<typename Type>
bool operator>=(const SingleLinkedList<Type> &lhs, const SingleLinkedList<Type> &rhs) {
    return (rhs < lhs) || (lhs == rhs);
}

//-----------------------------------swap for SingleLinkedList----------------------------------
template<typename Type>
void swap(SingleLinkedList<Type> &lhs, SingleLinkedList<Type> &rhs) noexcept {
    lhs.swap(rhs);
}
