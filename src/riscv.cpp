//
// Created by marko on 20.4.22..
//

#include "../h/riscv.hpp"
#include "../h/MemoryAllocator.h"
#include "../h/ccb.hpp"
#include "../h/sem.h"
#include "../h/print.hpp"

void Riscv::popSppSpie()
{
    if (CCB::running == CCB::mainThread or CCB::running == CCB::consoleThread) {
        ms_sstatus(SSTATUS_SPP);
    } else {
        mc_sstatus(SSTATUS_SPP);
    }

    __asm__ volatile ("csrw sepc, ra");
    __asm__ volatile ("sret");
}



extern "C" void handleSupervisorTrap1() {

    CCB * last = CCB::sleepy.peekLast();

    if (last) {

        while (CCB::sleepy.peekFirst() and CCB::sleepy.peekFirst() != last) {
            CCB *temp = CCB::sleepy.removeFirst();
            if (!temp->sleepDone())
                temp->decSleep();
            if (!temp->sleepDone()) {
                CCB::sleepy.addLast(temp);
            } else {
                temp->unblock();
                Scheduler::put(temp);
            }

        }
        CCB::sleepy.removeFirst();
        if (!last->sleepDone())
        last->decSleep();
        if (last->sleepDone()) {
            last->unblock();
            Scheduler::put(last);
        } else {
            CCB::sleepy.addLast(last);
        }
    }
    if(!CCB::running->timeExpired())
        CCB::running->decTime();
    if (CCB::running->timeExpired()) {
        CCB::running->reset();

        if (Scheduler::count) {

            uint64 sepc = Riscv::r_sepc();
            uint64 sstatus = Riscv::r_sstatus();


            thread_dispatch();

            Riscv::w_sstatus(sstatus);
            Riscv::w_sepc(sepc);
        }
    }

    Riscv::mc_sip(Riscv::SIP_SSIE);
}

extern "C" void handleSupervisorTrap2() {

   int k  =  plic_claim();
   if (k == CONSOLE_IRQ) {
       while ((*(char*) CONSOLE_STATUS)& CONSOLE_RX_STATUS_BIT) {
           char c = *(char*) CONSOLE_RX_DATA;
           KConsole::input->put(c);
       }
   }
plic_complete(k);
}

extern "C" void handleSupervisorTrap() {

    Riscv::aRegistersPush();
    uint64 scause = Riscv::r_scause();



  if (scause == 0x0000000000000008UL || scause == 0x0000000000000009UL) {


        uint64 volatile status = Riscv::r_sstatus();

        uint64 volatile sepc = Riscv::r_sepc()+4;


        uint64 branch ;
        __asm__ volatile ("ld a0, 10*8(s0)");
        __asm__ volatile ("mv %0, a0" : "=r" (branch));


        if (branch == 0x01) {
            Riscv::aRegistersPop();
            size_t size;
            __asm__ volatile ("mv %0, a1" : "=r" (size));
            MemoryAllocator& mem = MemoryAllocator::instance();
            void* ret = mem.kmemalloc(size);
            //void * ret = __mem_alloc(size);
            __asm__ volatile("mv a0, %0" : : "r" (ret));
        } else if (branch == 0x02) {
            Riscv::aRegistersPop();
            void* addr;
            __asm__ volatile ("mv %0, a1" : "=r" (addr));
            MemoryAllocator& mem = MemoryAllocator::instance();
            int val = mem.kmemfree(addr);
            //int val = __mem_free(addr);
            __asm__ volatile("mv a0, %0" : : "r" (val));
        } else if (branch == 0x11) {
            Riscv::aRegistersPop();
            uint64* stack;
            CCB::Body body;
            void* args;
            void** pointer;

            __asm__ volatile ("mv %0, a1" : "=r" (pointer));
            __asm__ volatile ("mv %0, a2" : "=r" (body));
            __asm__ volatile ("mv %0, a3" : "=r" (args));
            __asm__ volatile ("mv %0, a4" : "=r" (stack));

            *pointer = CCB::createCoroutine((CCB::Body)body, (void*)args, (uint64*)stack);


            __asm__ volatile ("mv a0, %0" : : "r" (*pointer));
        } else if (branch == 0x13) {
            Riscv::aRegistersPop();
            CCB::dispatch();

        } else if (branch == 0x12) {
           // MemoryAllocator& mem = MemoryAllocator::instance();
           // mem.kmemfree(CCB::running);
            Riscv::aRegistersPop();
            uint64 ret = CCB::running->exit();
            __asm__ volatile ("mv a0, %0" : : "r" (ret));
        } else if (branch == 0x14) {
            Riscv::aRegistersPop();
            CCB* current;
            __asm__ volatile ("mv %0, a1" : "=r" (current));
            CCB::running->join(current);
            //CCB::dispatch();
        } else if (branch == 0x18) {
            Riscv::aRegistersPop();
            CCB* myHandle;
            __asm__ volatile ("mv %0, a1" : "=r" (myHandle));
            Scheduler::put((CCB*)myHandle);
        }
        else if (branch == 0x21) {
            Riscv::aRegistersPop();
            sem** sem_handle;
            int init;
            __asm__ volatile ("mv %0, a1" : "=r" (sem_handle));
            __asm__ volatile ("mv %0, a2" : "=r" (init));
            *sem_handle = sem::open(init);
            __asm__ volatile ("mv a0, %0" : : "r" (sem_handle));
        } else if (branch == 0x22) {
            Riscv::aRegistersPop();
            Riscv::aRegistersPush();
            sem* handle;
            __asm__ volatile ("mv %0, a1" : "=r" (handle));

            MemoryAllocator& mem = MemoryAllocator::instance();
            mem.kmemfree(handle);
           // __mem_free(handle);
            Riscv::aRegistersPop();
            sem* sem_handle;
            __asm__ volatile ("mv %0, a1" : "=r" (sem_handle));
            int ret = sem_handle->close();
            __asm__ volatile ("mv a0, %0" : : "r" (ret));

        } else if (branch == 0x23) {
            Riscv::aRegistersPop();
            sem* sem_handle;
            __asm__ volatile ("mv %0, a1" : "=r" (sem_handle));
            int ret = sem_handle->wait();
            __asm__ volatile ("mv a0, %0" : : "r" (ret));
        } else if (branch == 0x24) {
            Riscv::aRegistersPop();
            sem* sem_handle;
            __asm__ volatile ("mv %0, a1" : "=r" (sem_handle));
            int ret = sem_handle->signal();
            __asm__ volatile ("mv a0, %0" : : "r" (ret));
        } else if (branch == 0x31) {
            Riscv::aRegistersPop();
            time_t time;
            __asm__ volatile ("mv %0, a1" : "=r" (time));
            CCB::setSleep(time);
            __asm__ volatile ("mv a0, %0" : : "r" (0x00));
        }
        else if (branch == 0x41) {

            Riscv::aRegistersPop();

            char c = (char)KConsole::input->get();
            __asm__ volatile ("mv a0, %0" : : "r" (c));


        } else if (branch == 0x42) {

            Riscv::aRegistersPop();
            char c;
            __asm__ volatile ("mv %0, a1" : "=r" (c));
            KConsole::output->put(c);


        } else if (branch == 0xff) {
            Riscv::aRegistersPop();
            while ((*(char*) CONSOLE_STATUS) & CONSOLE_TX_STATUS_BIT) {
                char c = (char)KConsole::output->get();
                *(char *) CONSOLE_TX_DATA = c;
            }
        }


        Riscv::w_sepc(sepc);
        Riscv::w_sstatus(status);
        __asm__ volatile ("sd a0, 10*8(s0)");

    } else if (scause == 0x0000000000000002UL or scause == 0x0000000000000005UL or scause == 0x0000000000000007UL){
     while (true) {
     printString1("GRESKA ");
     printInteger1(scause);
     printString1("\n");
    }
  }




}


