//
// Created by marko on 20.4.22..
//

#include "../h/scheduler.hpp"

//List Scheduler::readyCoroutineQueue;
Elem* Scheduler::head = 0;
Elem* Scheduler::tail = 0;
int Scheduler::count = 0;
CCB *Scheduler::get()
{
    if (!head) { return 0; }
    count--;
    Elem *elem = head;
    head = head->next;
    if (!head) { tail = 0; }

    CCB *ret = elem->data;
   MemoryAllocator& mem = MemoryAllocator::instance();
   mem.kmemfree(elem);
   // __mem_free(elem);
    //delete elem;
    return ret;
}

void Scheduler::put(CCB *ccb)
{

    if (!ccb) return;
    count++;
    MemoryAllocator& mem = MemoryAllocator::instance();
    Elem* elem = (Elem*) mem.kmemalloc((sizeof(Elem)+sizeof(memvals)+MEM_BLOCK_SIZE-1)/MEM_BLOCK_SIZE);

   //Elem* elem = (Elem*) __mem_alloc(sizeof(Elem));

    elem->data = ccb;
    elem->next = 0;
    // Elem *elem = new Elem(data, 0);
    if (tail)
    {
        tail->next = elem;
        tail = elem;
    } else
    {
        head = elem;
        tail = elem;
    }
}