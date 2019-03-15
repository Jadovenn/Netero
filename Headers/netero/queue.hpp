/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#pragma once

#include <exception>
#include <stdexcept>
#include <type_traits>
#include "core/internal.hpp"

template <typename T,
        typename = std::is_default_constructible<T>,
        typename  = std::is_copy_assignable<T>,
        typename  = std::is_move_constructible<T>>
class queue {
public:
    queue(): l_head(-1), l_tail(0), l_size(netero::page_size), l_count(0) {
        l_data = new(std::nothrow) T[netero::page_size];
        if (l_data == nullptr)
            throw std::bad_alloc();
    };

    queue(const queue &);
    queue(queue &&);
    queue &operator=(const queue&);
    queue &operator=(queue &&);
    virtual ~queue();

    /**
     * @brief reserve a certain amount of slot aligned on pages
     */
    void reserve(int) {

    }

    /**
     * @brief check if the queue is empty
     * @return true if its empty, false otherwise
     */
    bool    is_empty() const {
        return (l_count == 0);
    }

    /**
     * @brief emplace the given pointer to the queue
     */
    void    emplace_back(T *new_elem) {
        if (l_count == l_size)
            l_add_page();
        l_data[l_back] = new_elem;
        l_back = (l_back + 1) % l_size;
    }

    /**
     * @brief Create a copy and emplace_back
     * @param copy
     */
    void    push_back(const T &copy) {
        T   *new_elem = new (std::nothrow) T(copy);
        if (new_elem == nullptr)
            throw std::bad_alloc();
        emplace_back(new_elem);
    }

    /**
     * @brief Create a copy from move ctor and emplace_back
     * @param move
     */
    void push_back(T &&move) {
        T   *new_elem = new (std::nothrow) T(move);
        if (new_elem == nullptr)
            throw std::bad_alloc();
        emplace_back(new_elem);
    }

    /**
     * @brief pop the front element and return a pointer
     */
    void    pop_front() {
        if (is_empty())
            throw std::underflow_error("Queue is empty.");
        delete l_data[l_front];
        l_data[l_front] = nullptr;
        l_front = (l_front + 1) % l_size;
    }

private:
    void    l_extend() {
        int new_size = l_size + netero::page_size;
        T   *data = new (std::no_throw) T[new_size];
        if (!data)
            throw std::bad_alloc();
    }
    void    l_();

    int l_head;
    int l_tail;
    int l_size;
    int l_count;
    T   **l_data;
};

