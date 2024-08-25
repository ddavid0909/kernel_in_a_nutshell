//
// Created by marko on 20.4.22..
//

#pragma once

#include "list.hpp"

class CCB;

struct Elem
{
    //T *data;
    CCB* data;
    Elem *next;

    //Elem(T *data, Elem *next) : data(data), next(next) {}
   // Elem(CCB *data, Elem *next) : data(data), next(next) {}
};

class Scheduler
{
private:


    static Elem *head, *tail;
   // static List readyCoroutineQueue;

public:
    static int count;
    static CCB *get();

    static void put(CCB *ccb);

};
