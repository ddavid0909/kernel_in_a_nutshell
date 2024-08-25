//
// Created by os on 7/12/23.
//
#include "../h/KConsole.h"

BoundedBuffer::BoundedBuffer(int _cap) : cap(_cap + 1), head(0), tail(0) {

    MemoryAllocator& mem = MemoryAllocator::instance();
    buffer = (char *)mem.kmemalloc(((sizeof(char) * cap) + sizeof(memvals) + MEM_BLOCK_SIZE - 1)/MEM_BLOCK_SIZE);
   // buffer = (char*) __mem_alloc(sizeof(char) * cap);
    itemAvailable = sem::open(0);
    spaceAvailable = sem::open(_cap);
  //  sem_open(&itemAvailable, 0);
  //  sem_open(&spaceAvailable, _cap);
}


BoundedBuffer::~BoundedBuffer() {


    MemoryAllocator& mem = MemoryAllocator::instance();
    mem.kmemfree(buffer);
    mem.kmemfree(itemAvailable);
    mem.kmemfree(spaceAvailable);

   /* __mem_free(buffer);
    __mem_free(itemAvailable);
    __mem_free(spaceAvailable);*/

    itemAvailable->close();
    spaceAvailable->close();
     }

void BoundedBuffer::put(int val) {

    spaceAvailable->wait();

    buffer[tail] = val;
    tail = (tail + 1) % cap;

    itemAvailable->signal();
}

char BoundedBuffer::get() {

    itemAvailable->wait();


    char ret = buffer[head];
    head = (head + 1) % cap;

    spaceAvailable->signal();
    return ret;
}

int BoundedBuffer::getCnt() {
    int ret;

    if (tail >= head) {
        ret = tail - head;
    } else {
        ret = cap - head + tail;
    }

    return ret;
}
