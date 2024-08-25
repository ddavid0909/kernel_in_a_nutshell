#include "../h/MemoryAllocator.h"

#include "../h/syscall_c.hpp"

#include "../h/print.hpp"


extern "C" void IVT();



extern void userMain();

void user(void* p) {
    userMain();

}

void main() {
    Riscv::w_stvec((uint64)&IVT | 1);
    CCB::sleepy.initialize();
    CCB* mainThread = CCB::createCoroutine(0,0,new uint64[DEFAULT_STACK_SIZE]);
    if(!mainThread) return;
    CCB::running = mainThread;
    CCB::mainThread = mainThread;


    CCB* thread = CCB::createCoroutine(user, 0, new uint64[DEFAULT_STACK_SIZE]);
    if (!thread) {
        printString1("Nije dobro");
        return;
    }

    CCB::consoleThread = CCB::createCoroutine((void(*)(void*)) KConsole::ConsoleControler, 0, new uint64[DEFAULT_STACK_SIZE]);
  //  CCB::threadsAlive--;


    KConsole::output = new BoundedBuffer(256);
    KConsole::input = new BoundedBuffer(256);


    Riscv::ms_sstatus(Riscv::SSTATUS_SIE);

  // while (CCB::threadsAlive)
  while (!thread->isFinished())
   {
        if (Scheduler::count)
            thread_dispatch();
    }

    delete CCB::consoleThread;
    delete CCB::mainThread;
    delete KConsole::input;
    delete KConsole::output;
}

