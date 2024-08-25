//
// Created by os on 6/26/23.
//
#include "../h/list.hpp"

class CCB;
void List::initialize() {
    this->head = 0;
    this->tail = 0;
}

void List::addFirst(CCB *data)
{

    MemoryAllocator& mem = MemoryAllocator::instance();
    Elem * elem = (Elem*) mem.kmemalloc((sizeof(Elem)+sizeof(memvals)+MEM_BLOCK_SIZE-1)/MEM_BLOCK_SIZE);

    //Elem* elem = (Elem*) __mem_alloc(sizeof(Elem));
    elem->data = data;
    elem->next = head;
    //Elem *elem = new Elem(data, head);
    head = elem;
    if (!tail) { tail = head; }
}


void List::addLast(CCB *data)
{
    MemoryAllocator& mem = MemoryAllocator::instance();
    Elem* elem = (Elem*) mem.kmemalloc((sizeof(Elem)+sizeof(memvals)+MEM_BLOCK_SIZE-1)/MEM_BLOCK_SIZE);

    //Elem* elem = (Elem*) __mem_alloc(sizeof(Elem));

    elem->data = data;
    elem->next = nullptr;
    // Elem *elem = new Elem(data, 0);
    if (tail)
    {
        tail = tail->next = elem;
    } else
    {
        head = tail = elem;
    }
}

CCB* List::peekFirst()
{
    if (!head) { return 0; }
    return head->data;
}


CCB* List::removeFirst()
{
    if (!head) { return 0; }

    Elem *elem = head;
    head = head->next;
    if (!head) { tail = nullptr; }

    CCB *ret = elem->data;

    MemoryAllocator& mem = MemoryAllocator::instance();
    mem.kmemfree(elem);

    //__mem_free(elem);
    //delete elem;
    return ret;
}

CCB* List::removeLast()
{
    if (!head) { return nullptr; }

    Elem *prev = nullptr;
    for (Elem *curr = head; curr && curr != tail; curr = curr->next)
    {
        prev = curr;
    }

    Elem *elem = tail;
    if (prev) { prev->next = nullptr; }
    else { head = nullptr; }
    tail = prev;

    CCB *ret = elem->data;
    MemoryAllocator& mem = MemoryAllocator::instance();
    mem.kmemfree(elem);
    //__mem_free(elem);
    //delete elem;
    return ret;
}

CCB* List::peekLast()
{
    if (!tail) { return nullptr; }
    return tail->data;
}