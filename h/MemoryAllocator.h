

#ifndef PROJECT_BASE_V1_1_MEMORYALLOCATOR_H
#define PROJECT_BASE_V1_1_MEMORYALLOCATOR_H
#include "../lib/hw.h"
#include "../lib/console.h"
struct memvals {
    size_t size;
    memvals *next;
};

class MemoryAllocator {
private:

    memvals *kfree;
    memvals *kbusy;

    MemoryAllocator(const MemoryAllocator&) = delete;
    MemoryAllocator(MemoryAllocator&&) = delete;
    MemoryAllocator& operator = (const MemoryAllocator&) = delete;
    MemoryAllocator& operator = (MemoryAllocator&&) = delete;

    MemoryAllocator() {

            // pocetak slobodnog bloka na samom pocetku jeste ovdje
            kfree = (memvals *) HEAP_START_ADDR;
            // velicina, u blokovima, je razlika pocetka i kraja
            kfree->size = (size_t) (((uint64) HEAP_END_ADDR - (uint64) HEAP_START_ADDR) / MEM_BLOCK_SIZE);
            // trenutno ne postoji naredni;
            kfree->next = nullptr;
            kbusy = nullptr;

        // nula za zauzete - svi su slobodni
    }

public:
    int kbusytest() {
        int count = 0;
        memvals* p = kbusy;
        while (p) {
            count++;
            p = p->next;
        }
        return count;
    }

    int kfreetest() {
        int count = 0;
        memvals* p = kfree;
        while (p) {
            count++;
            p = p->next;
        }
        return count;
    }

    static MemoryAllocator &instance() {
        static MemoryAllocator memoryAllocator;
        return memoryAllocator;
    }

    void *kmemalloc(size_t size);

    int kmemfree(void *addr);

};

#endif //PROJECT_BASE_V1_1_MEMORYALLOCATOR_H
