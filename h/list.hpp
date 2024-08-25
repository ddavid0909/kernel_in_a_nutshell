
#pragma once
#include "MemoryAllocator.h"
#include "../lib/mem.h"

class CCB;

class List
{
private:
    struct Elem
    {
        //T *data;
        CCB* data;
        Elem *next;

        //Elem(T *data, Elem *next) : data(data), next(next) {}
        Elem(CCB *data, Elem *next) : data(data), next(next) {}
    };

    Elem* head = 0;
    Elem* tail = 0;

public:

    void initialize();
    List() : head(0), tail(0) {}

    //List(const List<T> &) = delete;
    List(const List &) = delete;
    List &operator=(const List &) = delete;

    void addFirst(CCB *data);

    void addLast(CCB *data);
    CCB *removeFirst();

    CCB *peekFirst();

    CCB *removeLast();

    CCB *peekLast();
};


