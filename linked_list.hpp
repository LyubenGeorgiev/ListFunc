#pragma once

#include <cstddef>
#include <iostream>

template <typename T>
class linked_list
{
private:
    struct Node
    {
        T data;
        Node* next;
        Node(const T &data, Node* next = nullptr) : data(data), next(next) {}
    };

    Node* _head = nullptr;
    Node* _tail = nullptr;
    size_t _size = 0;

    template<typename U>
    class iterator
    {
    public:
        iterator(Node* n = nullptr) noexcept : currentNode(n) {}
 
        // Prefix ++ overload
        iterator& operator++()
        {
            if (currentNode)
                currentNode = currentNode->next;

            return *this;
        }
 
        // Postfix ++ overload
        iterator operator++(int)
        {
            iterator iterator = *this;
            ++*this;
            return iterator;
        }
 
        friend bool operator!=(const iterator &first, const iterator &second) noexcept
        {
            return !(first.currentNode == second.currentNode);
        }
 
        U& operator*()
        {
            return currentNode->data;
        }
 
    private:
        Node* currentNode;
    };

    template<typename U>
    class const_iterator
    {
    public:
        const_iterator(Node* n = nullptr) noexcept : currentNode(n) {}
 
        // Prefix ++ overload
        const_iterator& operator++()
        {
            if (currentNode)
                currentNode = currentNode->next;

            return *this;
        }
 
        // Postfix ++ overload
        const_iterator operator++(int)
        {
            const_iterator iterator = *this;
            ++*this;
            return iterator;
        }
 
        friend bool operator!=(const const_iterator &first, const const_iterator &second) noexcept
        {
            return !(first.currentNode == second.currentNode);
        }
 
        const U& operator*()
        {
            return currentNode->data;
        }
 
    private:
        const Node* currentNode;
    };

    void free()
    {
        Node* toDel;
        while (_head)
        {
            toDel = _head;
            _head = _head->next;
            delete toDel;
        }
    }

    void copy(const linked_list &other)
    {
        for (const T &d : other)
        {
            push_back(d);
        }
    }

public:
    bool empty() const noexcept { return _head == nullptr; }

    size_t size() const
    {
        return _size;
    }

    T& front() { return _head->data; }
    const T& front() const { return _head->data; }

    void pop_front()
    {
        Node* toDel = _head;
        if (_head)
        {
            _head = _head->next;
            --_size;
        }

        if (_head == nullptr) _tail = nullptr;

        delete toDel;
    }

    void push_front(const T &data)
    {
        _head = new Node(data, _head);

        if (_head->next == nullptr)
        {
            _tail = _head;
        }

        ++_size;
    }

    void push_back(const T &data)
    {
        if (_tail != nullptr)
        {
            _tail->next = new Node(data);
            _tail = _tail->next;
        }
        else
        {
            _head = _tail = new Node(data);
        }
        ++_size;
    }

    linked_list() = default;

    ~linked_list()
    {
        free();
    }

    linked_list(const linked_list &other)
    {
        copy(other);
    }

    linked_list& operator=(const linked_list &other)
    {
        if (this != &other)
        {
            free();
            copy(other);
        }

        return *this;
    }

    linked_list<T>& operator+=(const linked_list<T> &other)
    {
        for (T data : other)
        {
            push_back(data);
        }

        return *this;
    }

    

    iterator<T> begin()
    {
        return iterator<T>(_head);
    }

    iterator<T> end()
    {
        return iterator<T>(nullptr);
    }

    

    const_iterator<T> begin() const
    {
        return const_iterator<T>(_head);
    }

    const_iterator<T> end() const
    {
        return const_iterator<T>(nullptr);
    }
    
};

// template <typename T>
// typename linked_list<T>::Node* linked_list<T>::_head = nullptr;