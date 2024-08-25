//
// Created by os on 6/26/23.
//

#include "../h/syscall_c.hpp"
#include "../h/syscall_cpp.hpp"
#include "../h/ccb.hpp"
Thread::Thread(void (*body)(void *), void *arg) {
    CCB::cpp = true;
    thread_create(&myHandle, body, arg);
}
Thread::~Thread() {
    CCB* t = myHandle;
    delete t->getStack();
    delete t;
}

int Thread::start() {
    __asm__ volatile ("mv a1, %0" : : "r" (this->myHandle));
    __asm__ volatile ("mv a0, %0" : : "r" (0x18));
    __asm__ volatile ("ecall");
    return 0;
}

void Thread::join() {
    thread_join(myHandle);
}

void Thread::dispatch() {
    thread_dispatch();
}

Thread::Thread() {
    CCB::cpp = true;
    thread_create(&this->myHandle, wrapper, this);
}

void Thread::wrapper(void *arg) {
    Thread* a = (Thread*) arg;
    a->run();
}

int Thread::sleep(time_t time) {
    return time_sleep(time);
}

Semaphore::Semaphore(unsigned int init) {
    sem_open(&this->myHandle, init);
}

int Semaphore::signal() {
    return sem_signal(this->myHandle);
}

int Semaphore::wait() {
    return sem_wait(this->myHandle);
}

Semaphore::~Semaphore() {
    sem_close(this->myHandle);
}


PeriodicThread::PeriodicThread(time_t period) : Thread(PeriodicThread::periodicWrapper, this), period(period), done(false){};


void PeriodicThread::terminate() {
    done = true;
}

void PeriodicThread::periodicWrapper(void *arg) {
    if (!arg) return;
    PeriodicThread* t = (PeriodicThread*) arg;
    while (!t->done) {
        time_sleep(t->period);
        t->periodicActivation();
    }
}


char Console::getc() {
    return ::getc();
}

void Console::putc(char c) {
    ::putc(c);
}