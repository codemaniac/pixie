#ifndef CONTAINERS_H
#define CONTAINERS_H

#include <array>
#include <cstddef>
#include <iostream>

template<typename T, size_t Capacity = 256>
class ArrayList: public std::array<T, Capacity> {
   private:
    size_t current_size = 0;

   public:
    template<typename... Args>
    void emplace(size_t position, Args&&... args) {
        if (position > current_size || current_size >= Capacity)
        {
            throw std::out_of_range("Invalid position or ArrayList is full.");
        }

        // Shift elements to the right to make space for the new element
        for (size_t i = current_size; i > position; --i)
        {
            (*this)[i] = std::move((*this)[i - 1]);
        }

        // Construct the new element in place
        (*this)[position] = T(std::forward<Args>(args)...);
        ++current_size;
    }

    template<typename... Args>
    void emplace_back(Args&&... args) {
        if (current_size >= Capacity)
        {
            throw std::out_of_range("ArrayList is full: Cannot emplace_back.");
        }

        // Construct the new element at the back
        (*this)[current_size++] = T(std::forward<Args>(args)...);
    }

    void push(T t) {
        if (current_size >= Capacity)
        {
            throw std::out_of_range("ArrayList is full: Cannot push.");
        }
        (*this)[current_size++] = t;
    }

    size_t size() const { return current_size; }

    typedef typename std::array<T, Capacity>::iterator       iterator;
    typedef typename std::array<T, Capacity>::const_iterator const_iterator;

    iterator begin() { return std::array<T, Capacity>::begin(); }

    iterator end() { return std::array<T, Capacity>::begin() + current_size; }

    const_iterator begin() const { return std::array<T, Capacity>::begin(); }

    const_iterator end() const { return std::array<T, Capacity>::begin() + current_size; }
};

template<typename T, size_t Capacity = 256>
class ArrayStack {
   private:
    std::array<T, Capacity> elements;
    size_t                  current_size = 0;

   public:
    // Push an element onto the stack
    void push(const T& item) {
        if (current_size >= Capacity)
        {
            throw std::overflow_error("Stack overflow: No more elements can be pushed.");
        }
        elements[current_size++] = item;
    }

    // Pop the top element off the stack
    void pop() {
        if (current_size == 0)
        {
            throw std::underflow_error("Stack underflow: No elements to pop.");
        }
        --current_size;
    }

    // Peek at the top element of the stack
    T top() const {
        if (current_size == 0)
        {
            throw std::out_of_range("Stack is empty: No top element.");
        }
        return elements[current_size - 1];
    }

    // Peek at an element at a specific position (0-based index)
    T peek(size_t position) const {
        if (position >= current_size)
        {
            throw std::out_of_range("Invalid position: Out of stack bounds.");
        }
        return elements[position];
    }

    // Check if the stack is empty
    bool is_empty() const { return current_size == 0; }

    // Get the current size of the stack
    size_t size() const { return current_size; }

    // Define iterator types using the underlying array iterators
    typedef typename std::array<T, Capacity>::iterator       iterator;
    typedef typename std::array<T, Capacity>::const_iterator const_iterator;

    // Begin iterator
    iterator begin() { return elements.begin(); }

    // End iterator (only up to the current size)
    iterator end() { return elements.begin() + current_size; }

    // Const begin iterator
    const_iterator begin() const { return elements.begin(); }

    // Const end iterator (only up to the current size)
    const_iterator end() const { return elements.begin() + current_size; }
};

#endif
