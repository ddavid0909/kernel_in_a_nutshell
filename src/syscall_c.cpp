//
// Created by os on 6/21/23.
//
#include "../h/syscall_c.hpp"



void* mem_alloc (size_t bytes) {
    if (bytes == 0) return nullptr;
    size_t blocks = (bytes + sizeof(memvals) + MEM_BLOCK_SIZE - 1)/MEM_BLOCK_SIZE;
    __asm__ volatile ("mv a1, %0" : : "r" (blocks));
    __asm__ volatile ("mv a0, %0" : : "r" (0x01));
    __asm__ volatile ("ecall");
    void * retVal;
    __asm__ volatile ("mv %0, a0" : "=r" (retVal));
    return retVal;
}

int mem_free(void * addr) {
    if (addr == nullptr) return -1;
    __asm__ volatile ("mv a1, %0" : : "r" (addr));
    __asm__ volatile ("mv a0, %0" : : "r" (0x02));
    __asm__ volatile ("ecall");
    int ret;
    __asm__ volatile ("mv %0, a0" : "=r" (ret));
    return ret;

}


int thread_create ( thread_t* handle, void(*start_routine) (void*), void* arg
) {
    __asm__ volatile("mv a3, a2");
    __asm__ volatile("mv a2, a1");
    __asm__ volatile("mv a1, a0");
    Riscv::aRegistersPush();
    uint64* stack = (uint64*) mem_alloc(DEFAULT_STACK_SIZE);
    __asm__ volatile("mv a7, %0" : : "r" (stack));
    Riscv::aRegistersPop();
    __asm__ volatile("mv a4, a7");
    __asm__ volatile("mv a0, %0" : : "r" (0x11));
    __asm__ volatile("ecall");

    uint64 ret;
    __asm__ volatile ("mv %0, a0" : "=r" (ret));
    *handle = (thread_t) ret;
    if (ret) return 0;
    return -1;
};

int thread_exit() {
    __asm__ volatile ("mv a0, %0" : : "r" (0x12));
    __asm__ volatile ("ecall");
    uint64 ret;
    __asm__ volatile ("mv %0, a0" : "=r" (ret));
    return ret;
};

void thread_dispatch() {
    __asm__ volatile("mv a0, %0" : : "r" (0x13));
    __asm__ volatile("ecall");
}

void thread_join(thread_t handle) {
    __asm__ volatile("mv a1, %0" : : "r" (handle));
    __asm__ volatile("mv a0, %0" : : "r" (0x14));
    __asm__ volatile("ecall");

};

int sem_open (sem_t* handle, unsigned init) {
    if (!handle) return -2;
    __asm__ volatile("mv a2, a1");
    __asm__ volatile("mv a1, a0");
    __asm__ volatile("mv a0, %0" : : "r" (0x21));
    __asm__ volatile("ecall");
    __asm__ volatile("mv %0, a0" : "=r" (handle));
    if (*handle) return 0;
    return -1;
}

int sem_close(sem_t handle) {
    if (!handle) return 0;
    __asm__ volatile ("mv a1, a0");
    __asm__ volatile ("mv a0, %0" : : "r" (0x22));
    __asm__ volatile ("ecall");
    int ret;
    __asm__ volatile ("mv %0, a0" : "=r" (ret));
    return ret;
}

int sem_wait(sem_t id) {
    if (!id) return -1;
    __asm__ volatile ("mv a1, a0");
    __asm__ volatile ("mv a0, %0" : : "r" (0x23));
    __asm__ volatile ("ecall");
    int ret;
    __asm__ volatile ("mv %0, a0" : "=r" (ret));
    return ret;
}

int sem_signal(sem_t id) {
    if (!id) return -1;
    __asm__ volatile ("mv a1, a0");
    __asm__ volatile ("mv a0, %0" : : "r" (0x24));
    __asm__ volatile ("ecall");
    int ret;
    __asm__ volatile ("mv %0, a0" : "=r" (ret));
    return ret;
}

int time_sleep(time_t time) {
    if (time == 0) return 0;
    __asm__ volatile ("mv a1, a0");
    __asm__ volatile ("mv a0, %0" : : "r" (0x31));
    __asm__ volatile ("ecall");
    int ret;
    __asm__ volatile ("mv %0, a0" : "=r" (ret));
    return ret;
};

char getc() {

    __asm__ volatile ("mv a0, %0" : : "r" (0x41));
    __asm__ volatile ("ecall");
    char c;
    __asm__ volatile ("mv %0, a0" : "=r" (c));
    return c;
}

void putc(char c) {

    __asm__ volatile ("mv a1, a0" : : "r" (c));
    __asm__ volatile ("mv a0, %0" : : "r" (0x42));
    __asm__ volatile ("ecall");
}

const int EOF = -1;