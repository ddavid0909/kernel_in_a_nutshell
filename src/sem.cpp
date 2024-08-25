//
// Created by os on 6/27/23.
//
#include "../h/sem.h"




sem* sem::open(int val) {
    //asm volatile("csrr t6, sepc");
    MemoryAllocator& mem = MemoryAllocator::instance();
    sem* semaphore = (sem*) mem.kmemalloc((sizeof(sem) + sizeof(memvals) + MEM_BLOCK_SIZE -1)/ MEM_BLOCK_SIZE);
  //  sem* semaphore = (sem*) __mem_alloc(sizeof(sem));
    semaphore->val = val;
    semaphore->blocked.initialize();
    return semaphore;
}

int sem::wait() {
    //asm volatile("csrr t6, sepc");
    if (--val < 0) {

        CCB* old = CCB::running;
        old->block();
        blocked.addLast(old);
        thread_dispatch();
    }
    if (success) return 0;
    return -1;
}

int sem::signal() {
   // asm volatile("csrr t6, sepc");
    if (++val <= 0) {
        CCB* curr = blocked.removeFirst();
        curr->unblock();
        Scheduler::put(curr);
    }
    return 0;
}

int sem::close() {
  //  asm volatile("csrr t6, sepc");
    if (!blocked.peekFirst()) {
        success = true;
        return 0;
    }
    while (blocked.peekFirst()) {
        CCB* curr = blocked.removeFirst();
        curr->unblock();
        Scheduler::put(curr);
    }
    success = false;
    return -1;
}