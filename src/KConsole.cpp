//
// Created by os on 7/12/23.
//
#include "../h/KConsole.h"

BoundedBuffer* KConsole::input = nullptr;
BoundedBuffer* KConsole::output = nullptr;


void KConsole::ConsoleControler() {

    while (true) {
        __asm__ volatile ("mv a0, %0" : : "r" (0xff));
        __asm__ volatile ("ecall");
        thread_dispatch();
    }

}

KConsole::~KConsole() {
    delete input;
    delete output;
}