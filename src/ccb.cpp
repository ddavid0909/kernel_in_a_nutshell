//
// Created by marko on 20.4.22..
//

#include "../h/ccb.hpp"
#include "../h/riscv.hpp"
#include "../h/MemoryAllocator.h"


List CCB::sleepy;
CCB* CCB::mainThread = nullptr;
CCB* CCB::consoleThread = nullptr;
bool CCB::cpp = false;
CCB *CCB::running = nullptr;
int CCB::threadsAlive = 0;
CCB *CCB::createCoroutine(Body body, void* arg, uint64* stackk)
{

    size_t size = (sizeof(CCB)+sizeof(memvals)+MEM_BLOCK_SIZE-1)/MEM_BLOCK_SIZE;


    CCB* answer = (CCB*)(MemoryAllocator::instance()).kmemalloc((size_t)size);
    //CCB* answer = (CCB*) __mem_alloc(size);
    answer->initialize(body, arg, stackk);
   return answer;
}

void CCB::yield()
{

    thread_dispatch();
}

void CCB::dispatch()
{
  //  asm volatile("csrr t6, sepc");
    CCB *old = running;
    if (old && !old->finished && !old->suspended) { Scheduler::put(old); }
    running = Scheduler::get();

    CCB::contextSwitch(&old->context, &running->context);
}
void CCB::block() {
    suspended = true;
}

void CCB::unblock() {
    suspended = false;
}

int CCB::exit() {
    CCB::threadsAlive--;
    finished = true;

    MemoryAllocator& mem = MemoryAllocator::instance();
    mem.kmemfree(CCB::running->stack);
    mem.kmemfree(CCB::running);
  //  __mem_free(CCB::running->stack);
  //  __mem_free(CCB::running);

    while (waiting.peekFirst()) {

        CCB* waiter = waiting.removeFirst();
        waiter->suspended = false;
        Scheduler::put(waiter);

    }

    CCB::yield();
    return 1;
}

void CCB::join(CCB *thread) {
    if (!thread || thread->isFinished()) return;
    thread->waiting.addLast(CCB::running);
    CCB::running->suspended = true;
    CCB::yield();

}
void CCB::setSleep(uint64 time) {
    if(time == 0) return;
    CCB::running->sleep_time = time;
    CCB::running->suspended = true;
    CCB::running->reset();
    sleepy.addLast(CCB::running);
    thread_dispatch();
}

void CCB::initialize(Body body, void* arg, uint64* a_stack) {
    this->time_slice = DEFAULT_TIME_SLICE;
    this->sleep_time = 0;
    this->body = body;
    this->args = arg;
    this->stack = a_stack;
    this->suspended = false;
    this->waiting.initialize();

    if (body) {
        this->context.ra = (uint64) GlFunc;
    } else {
        this->context.ra = 0;
    }
    //body!=nullptr? (uint64) nullptr : 0;
    this->context.sp = stack!= nullptr? (uint64) &a_stack[DEFAULT_STACK_SIZE] : 0;
    this->finished = false;
    if (body != nullptr && !cpp) { Scheduler::put(this); }
    if (body != nullptr) CCB::threadsAlive++;
    if (cpp) CCB::threadsAlive--;
   // cpp = false;
}



void  CCB::GlFunc() {
    Riscv::popSppSpie();
    CCB::running->body(CCB::running->args);
    thread_exit();
}
