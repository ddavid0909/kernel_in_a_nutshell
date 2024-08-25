
#include "../lib/hw.h"
#include "MemoryAllocator.h"
#include "riscv.hpp"
#include "ccb.hpp"
#include "../lib/console.h"

void* mem_alloc (size_t bytes) ;

int mem_free(void * addr) ;

class CCB;
typedef CCB* thread_t;

int thread_create ( thread_t* handle, void(*start_routine) (void*), void* arg
        ) ;

int thread_exit();

void thread_dispatch();

void thread_join(
        thread_t handle
        );

class sem;
typedef sem* sem_t;

int sem_open(sem_t* handle, unsigned init);

int sem_close (sem_t handle);

int sem_wait(sem_t id);

int sem_signal(sem_t id);

typedef unsigned long time_t;

int time_sleep(time_t);


char getc();

void putc(char);