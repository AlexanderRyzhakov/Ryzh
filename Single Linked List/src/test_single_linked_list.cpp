#include "test_single_linked_list.h"

#include <cassert>
#include <stdexcept>

#include "single_linked_list.h"

using namespace std;

struct DeletionSpy {
    DeletionSpy() = default;
    explicit DeletionSpy(int &instance_counter) noexcept : instance_counter_ptr_(&instance_counter) {
        OnAddInstance();
    }
    DeletionSpy(const DeletionSpy &other) noexcept : instance_counter_ptr_(other.instance_counter_ptr_) {
        OnAddInstance();
    }
    DeletionSpy& operator=(const DeletionSpy &rhs) noexcept {
        if (this != &rhs) {
            auto rhs_copy(rhs);
            std::swap(instance_counter_ptr_, rhs_copy.instance_counter_ptr_);
        }
        return *this;
    }
    ~DeletionSpy() {
        OnDeleteInstance();
        if (deletion_counter_ptr) {
            ++(*deletion_counter_ptr);
        }
    }

    int *deletion_counter_ptr = nullptr;

private:
    void OnAddInstance() noexcept {
        if (instance_counter_ptr_) {
            ++(*instance_counter_ptr_);
        }
    }
    void OnDeleteInstance() noexcept {
        if (instance_counter_ptr_) {
            assert(*instance_counter_ptr_ != 0);
            --(*instance_counter_ptr_);
        }
    }

    int *instance_counter_ptr_ = nullptr;
};

struct ThrowOnCopy {
    ThrowOnCopy() = default;
    explicit ThrowOnCopy(int &copy_counter) noexcept : countdown_ptr(&copy_counter) {
    }
    ThrowOnCopy(const ThrowOnCopy &other) : countdown_ptr(other.countdown_ptr) {
        if (countdown_ptr) {
            if (*countdown_ptr == 0) {
                throw std::bad_alloc();
            } else {
                --(*countdown_ptr);
            }
        }
    }
    ThrowOnCopy& operator=(const ThrowOnCopy &rhs) = delete;
    int *countdown_ptr = nullptr;
};

void Test1() {
    // PushFront test
    {
        SingleLinkedList<int> l;
        assert(l.IsEmpty());
        assert(l.GetSize() == 0u);

        l.PushFront(0);
        l.PushFront(1);
        assert(l.GetSize() == 2);
        assert(!l.IsEmpty());

        l.Clear();
        assert(l.GetSize() == 0);
        assert(l.IsEmpty());
    }

    // Deletion test
    {
        int item0_counter = 0;
        int item1_counter = 0;
        int item2_counter = 0;
        {
            SingleLinkedList<DeletionSpy> list;
            list.PushFront(DeletionSpy { item0_counter });
            list.PushFront(DeletionSpy { item1_counter });
            list.PushFront(DeletionSpy { item2_counter });

            assert(item0_counter == 1);
            assert(item1_counter == 1);
            assert(item2_counter == 1);
            list.Clear();
            assert(item0_counter == 0);
            assert(item1_counter == 0);
            assert(item2_counter == 0);

            list.PushFront(DeletionSpy { item0_counter });
            list.PushFront(DeletionSpy { item1_counter });
            list.PushFront(DeletionSpy { item2_counter });
            assert(item0_counter == 1);
            assert(item1_counter == 1);
            assert(item2_counter == 1);
        }
        assert(item0_counter == 0);
        assert(item1_counter == 0);
        assert(item2_counter == 0);
    }

    {
        bool exception_was_thrown = false;
        for (int max_copy_counter = 5; max_copy_counter >= 0; --max_copy_counter) {
            SingleLinkedList<ThrowOnCopy> list;
            list.PushFront(ThrowOnCopy { });
            try {
                int copy_counter = max_copy_counter;
                list.PushFront(ThrowOnCopy(copy_counter));
                assert(list.GetSize() == 2);
            } catch (const std::bad_alloc&) {
                exception_was_thrown = true;
                assert(list.GetSize() == 1);
                break;
            }
        }
        assert(exception_was_thrown);
    }
}

void Test2() {
    // Empty list iteration
    {
        SingleLinkedList<int> list;
        const auto &const_list = list;

        assert(list.begin() == list.end());
        assert(const_list.begin() == const_list.end());
        assert(list.cbegin() == list.cend());
        assert(list.cbegin() == const_list.begin());
        assert(list.cend() == const_list.end());
    }

    // Nonempty list iteration
    {
        SingleLinkedList<int> list;
        const auto &const_list = list;

        list.PushFront(1);
        assert(list.GetSize() == 1u);
        assert(!list.IsEmpty());

        assert(const_list.begin() != const_list.end());
        assert(const_list.cbegin() != const_list.cend());
        assert(list.begin() != list.end());

        assert(const_list.begin() == const_list.cbegin());

        assert(*list.cbegin() == 1);
        *list.begin() = -1;
        assert(*list.cbegin() == -1);

        const auto old_begin = list.cbegin();
        list.PushFront(2);
        assert(list.GetSize() == 2);

        const auto new_begin = list.cbegin();
        assert(new_begin != old_begin);
        {
            auto new_begin_copy(new_begin);
            assert((++(new_begin_copy)) == old_begin);
        }
        {
            auto new_begin_copy(new_begin);
            assert(((new_begin_copy)++) == new_begin);
            assert(new_begin_copy == old_begin);
        }
        {
            auto old_begin_copy(old_begin);
            assert((++old_begin_copy) == list.end());
        }
    }

    // Itarator modify test
    {
        SingleLinkedList<int> list;
        list.PushFront(1);

        // ConstItrator from Iterator
        SingleLinkedList<int>::ConstIterator const_it(list.begin());

        assert(const_it == list.cbegin());
        assert(*const_it == *list.cbegin());

        SingleLinkedList<int>::ConstIterator const_it1;

        const_it1 = list.begin();
        assert(const_it1 == const_it);
    }
    // operator -> test
    {
        using namespace std;
        SingleLinkedList<std::string> string_list;

        string_list.PushFront("one"s);
        assert(string_list.cbegin()->length() == 3u);
        string_list.begin()->push_back('!');
        assert(*string_list.begin() == "one!"s);
    }
}

void Test3() {
    // equality  test
    {
        SingleLinkedList<int> list_1;
        list_1.PushFront(1);
        list_1.PushFront(2);

        SingleLinkedList<int> list_2;
        list_2.PushFront(1);
        list_2.PushFront(2);
        list_2.PushFront(3);

        SingleLinkedList<int> list_1_copy;
        list_1_copy.PushFront(1);
        list_1_copy.PushFront(2);

        SingleLinkedList<int> empty_list;
        SingleLinkedList<int> another_empty_list;

        // selfequlaity test
        assert(list_1 == list_1);
        assert(empty_list == empty_list);

        // List with same values shuld be equal
        assert(list_1 == list_1_copy);
        assert(list_1 != list_2);
        assert(list_2 != list_1);
        assert(empty_list == another_empty_list);
    }

    // list exchange test
    {
        SingleLinkedList<int> first;
        first.PushFront(1);
        first.PushFront(2);

        SingleLinkedList<int> second;
        second.PushFront(10);
        second.PushFront(11);
        second.PushFront(15);

        const auto old_first_begin = first.begin();
        const auto old_second_begin = second.begin();
        const auto old_first_size = first.GetSize();
        const auto old_second_size = second.GetSize();

        first.swap(second);

        assert(second.begin() == old_first_begin);
        assert(first.begin() == old_second_begin);
        assert(second.GetSize() == old_first_size);
        assert(first.GetSize() == old_second_size);

        // swap test
        {
            using std::swap;

            swap(first, second);

            assert(first.begin() == old_first_begin);
            assert(second.begin() == old_second_begin);
            assert(first.GetSize() == old_first_size);
            assert(second.GetSize() == old_second_size);
        }
    }

    // std::initializer_list test
    {
        SingleLinkedList<int> list { 1, 2, 3, 4, 5 };
        assert(list.GetSize() == 5);
        assert(!list.IsEmpty());
        assert(std::equal(list.begin(), list.end(), std::begin( { 1, 2, 3, 4, 5 })));
    }

    // lexicographical comparison test
    {
        using IntList = SingleLinkedList<int>;

        assert((IntList { 1, 2, 3 } < IntList { 1, 2, 3, 1 }));
        assert((IntList { 1, 2, 3 } <= IntList { 1, 2, 3 }));
        assert((IntList { 1, 2, 4 } > IntList { 1, 2, 3 }));
        assert((IntList { 1, 2, 3 } >= IntList { 1, 2, 3 }));
    }

    // Lists copy
    {
        const SingleLinkedList<int> empty_list { };
        {
            auto list_copy(empty_list);
            assert(list_copy.IsEmpty());
        }

        SingleLinkedList<int> non_empty_list { 1, 2, 3, 4 };

        {
            auto list_copy(non_empty_list);

            assert(non_empty_list.begin() != list_copy.begin());
            assert(list_copy == non_empty_list);
        }
    }

    // assignment test
    {
        const SingleLinkedList<int> source_list { 1, 2, 3, 4 };

        SingleLinkedList<int> receiver { 5, 4, 3, 2, 1 };
        receiver = source_list;
        assert(receiver.begin() != source_list.begin());
        assert(receiver == source_list);
    }

    // safe assignment test
    {
        SingleLinkedList<ThrowOnCopy> src_list;
        src_list.PushFront(ThrowOnCopy { });
        src_list.PushFront(ThrowOnCopy { });

        auto thrower = src_list.begin();
        src_list.PushFront(ThrowOnCopy { });

        int copy_counter = 0;  // throw on first copy attempt
        thrower->countdown_ptr = &copy_counter;

        SingleLinkedList<ThrowOnCopy> dst_list;
        dst_list.PushFront(ThrowOnCopy { });
        int dst_counter = 10;
        dst_list.begin()->countdown_ptr = &dst_counter;
        dst_list.PushFront(ThrowOnCopy { });

        try {
            dst_list = src_list;
            assert(false);
        } catch (const std::bad_alloc&) {
            assert(dst_list.GetSize() == 2);
            auto it = dst_list.begin();
            assert(it != dst_list.end());
            assert(it->countdown_ptr == nullptr);
            ++it;
            assert(it != dst_list.end());
            assert(it->countdown_ptr == &dst_counter);
            assert(dst_counter == 10);
        } catch (...) {
            assert(false);
        }
    }
}

void Test4() {
    // PopFront test
    {
        SingleLinkedList<int> numbers { 3, 14, 15, 92, 6 };
        numbers.PopFront();
        assert((numbers == SingleLinkedList<int> { 14, 15, 92, 6 }));

        SingleLinkedList<DeletionSpy> list;
        list.PushFront(DeletionSpy { });
        int deletion_counter = 0;
        list.begin()->deletion_counter_ptr = &deletion_counter;
        assert(deletion_counter == 0);
        list.PopFront();
        assert(deletion_counter == 1);
    }

    // before_begin() test
    {
        SingleLinkedList<int> empty_list;
        const auto &const_empty_list = empty_list;
        assert(empty_list.before_begin() == empty_list.cbefore_begin());
        assert(++empty_list.before_begin() == empty_list.begin());
        assert(++empty_list.cbefore_begin() == const_empty_list.begin());

        SingleLinkedList<int> numbers { 1, 2, 3, 4 };
        const auto &const_numbers = numbers;
        assert(numbers.before_begin() == numbers.cbefore_begin());
        assert(++numbers.before_begin() == numbers.begin());
        assert(++numbers.cbefore_begin() == const_numbers.begin());
    }

    // insertion test
    {
        // insertion to empty list
        {
            SingleLinkedList<int> lst;
            const auto inserted_item_pos = lst.InsertAfter(lst.before_begin(), 123);
            assert((lst == SingleLinkedList<int> { 123 }));
            assert(inserted_item_pos == lst.begin());
            assert(*inserted_item_pos == 123);

        }
        // insertion to nonempty list
        {
            SingleLinkedList<int> lst { 1, 2, 3 };
            auto inserted_item_pos = lst.InsertAfter(lst.before_begin(), 123);

            assert(inserted_item_pos == lst.begin());
            assert(inserted_item_pos != lst.end());
            assert(*inserted_item_pos == 123);
            assert((lst == SingleLinkedList<int> { 123, 1, 2, 3 }));

            inserted_item_pos = lst.InsertAfter(lst.begin(), 555);
            assert(++SingleLinkedList<int>::Iterator(lst.begin()) == inserted_item_pos);
            assert(*inserted_item_pos == 555);
            assert((lst == SingleLinkedList<int> { 123, 555, 1, 2, 3 }));
        }
    }

    // strict safety garantie test on copy
    {
        bool exception_was_thrown = false;
        for (int max_copy_counter = 10; max_copy_counter >= 0; --max_copy_counter) {
            SingleLinkedList<ThrowOnCopy> list { ThrowOnCopy { }, ThrowOnCopy { }, ThrowOnCopy { } };
            try {
                int copy_counter = max_copy_counter;
                list.InsertAfter(list.cbegin(), ThrowOnCopy(copy_counter));
                assert(list.GetSize() == 4u);
            } catch (const std::bad_alloc&) {
                exception_was_thrown = true;
                assert(list.GetSize() == 3u);
                break;
            }
        }
        assert(exception_was_thrown);
    }

    // deletion after position test
    {
        {
            SingleLinkedList<int> lst { 1, 2, 3, 4 };
            const auto &const_lst = lst;
            const auto item_after_erased = lst.EraseAfter(const_lst.cbefore_begin());
            assert((lst == SingleLinkedList<int> { 2, 3, 4 }));
            assert(item_after_erased == lst.begin());
        }
        {
            SingleLinkedList<int> lst { 1, 2, 3, 4 };
            const auto item_after_erased = lst.EraseAfter(lst.cbegin());
            assert((lst == SingleLinkedList<int> { 1, 3, 4 }));
            assert(item_after_erased == (++lst.begin()));
        }
        {
            SingleLinkedList<int> lst { 1, 2, 3, 4 };
            const auto item_after_erased = lst.EraseAfter(++(++lst.cbegin()));
            assert((lst == SingleLinkedList<int> { 1, 2, 3 }));
            assert(item_after_erased == lst.end());
        }
        {
            SingleLinkedList<DeletionSpy> list { DeletionSpy { }, DeletionSpy { }, DeletionSpy { } };
            auto after_begin = ++list.begin();
            int deletion_counter = 0;
            after_begin->deletion_counter_ptr = &deletion_counter;
            assert(deletion_counter == 0u);
            list.EraseAfter(list.cbegin());
            assert(deletion_counter == 1u);
        }
    }
}

void RunSingleLinkedListTests() {
    Test1();
    Test2();
    Test3();
    Test4();
}
