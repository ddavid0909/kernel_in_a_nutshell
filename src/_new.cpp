//
// Created by marko on 20.4.22..
//

#include "../lib/hw.h"
#include "../h/syscall_c.hpp"


void *operator new(size_t n)
{
    //volatile size_t nn = (volatile size_t) n;
    //MemoryAllocator& mem = MemoryAllocator::instance();
    //return mem.kmemalloc((nn+16+MEM_BLOCK_SIZE-1)/MEM_BLOCK_SIZE);
    return mem_alloc(n);
}

void *operator new[](size_t n)
{
    //MemoryAllocator& mem = MemoryAllocator::instance();
    //return mem.kmemalloc((n+16+MEM_BLOCK_SIZE-1)/MEM_BLOCK_SIZE);
    return mem_alloc(n);
}

void operator delete(void *p) noexcept
{
    mem_free(p);
    //MemoryAllocator& mem = MemoryAllocator::instance();
    //mem.kmemfree(p);
}

void operator delete[](void *p) noexcept
{
    mem_free(p);
    //MemoryAllocator& mem = MemoryAllocator::instance();
    //mem.kmemfree(p);
}