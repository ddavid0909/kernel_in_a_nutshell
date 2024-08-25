//
// Created by os on 7/12/23.
//

#ifndef PROJECT_BASE_V1_4_1_KCONSOLE_H
#define PROJECT_BASE_V1_4_1_KCONSOLE_H



#include "BoundedBuffer.hpp"
#include "syscall_c.hpp"
#include "sem.h"

class BoundedBuffer {
private:
    int cap;
    char *buffer;
    int head, tail;
public:
    sem_t spaceAvailable;
    sem_t itemAvailable;


public:
    BoundedBuffer(int _cap);
    ~BoundedBuffer();

    void put(int val);
    char get();

    int getCnt();

};

class KConsole {
public:
    static BoundedBuffer* input, *output;
    static void ConsoleControler();
    ~KConsole();
};

#endif //PROJECT_BASE_V1_4_1_KCONSOLE_H
