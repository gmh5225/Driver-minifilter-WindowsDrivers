#pragma once
#include "stdafx.h"

template<class T>
class List
{
    //private:
public:
    class Node
    {
        friend class Iterator;
        friend class List;
        //private:
    public:
        T value;
        Node* next, * prev;
    public:
        Node() : next(this), prev(this), value(T()) {}
        Node(T t) : next(this), prev(this), value(t) {}
        ~Node() { unlink(); }

        bool push_back(Node* n)
        {
            if (n == nullptr)
                return false;
            n->next = this;
            n->prev = prev;
            prev->next = n;
            prev = n;
            return true;
        }
        void unlink() {
            Node* _next = next, * _prev = prev;
            _next->prev = _prev;
            _prev->next = _next;
            next = this;
            prev = this;
        }
    };

    class Iterator {
        friend class List;
    private:
        Node* node;
        Iterator(Node* n) { node = n; }
    public:
        Iterator& operator++() { node = node->next; return *this; }
        Iterator& operator++(int dump) { dump; node = node->next; return *this; }
        bool operator==(Iterator b) const { return node == b.node; }
        bool operator!=(Iterator b) const { return node != b.node; }
        T& operator*() const { return node->value; }
        T* operator->() const { return &node->value; }

    };

    Node list;
public:
    using iterator = Iterator;
    //using const_iterator = Iterator<T const>;

    List() {}
    ~List() { clear(); }

    bool empty() const { return list.next == &list; }

    iterator begin() { return list.next; }
    iterator end() { return &list; }

    bool push_back(T value) { return list.push_back(new Node(value)); }
    void erase(iterator i) { delete i.node; }// DbgPrint("%s() -> %d\n", __FUNCTION__, i.node->value);

    void clear() {
        while (!empty())
            erase(begin());
    }
};

