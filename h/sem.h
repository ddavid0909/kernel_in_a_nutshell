//
// Created by os on 6/27/23.
//

#ifndef PROJECT_BASE_V1_1_SEM_H
#define PROJECT_BASE_V1_1_SEM_H
#include "list.hpp"
#include "ccb.hpp"
#include "MemoryAllocator.h"

class sem {
private:
    int val;
    List blocked;
    bool success = true;
public:

    static sem* open(int val);
    int wait();
    int signal();
    int close();

};

#endif //PROJECT_BASE_V1_1_SEM_H
