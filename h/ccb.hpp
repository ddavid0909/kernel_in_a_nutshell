//
// Created by marko on 20.4.22..
//

#pragma once

#include "../lib/hw.h"
#include "../h/list.hpp"
#include "scheduler.hpp"
#include "../h/syscall_c.hpp"

void GlFunc();

class CCB {
public:
    static CCB* consoleThread;
    static CCB* mainThread;
    static bool cpp;
    void block();
    void unblock();
    ~CCB() { delete[] stack; }

    uint64* getStack() { return this->stack;};

    static int threadsAlive;

    int exit();

    static void join(CCB *thread);

    bool isFinished() const { return finished; }

    void setFinished(bool value) { finished = value; }

    using Body = void (*)(void *);

    static CCB *createCoroutine(Body body, void *arg, uint64 *stackk);
    // omogucava stvaranje jedne korutine, prosljedjujuci joj pokazivac na funkciju.

    void decTime() {
        time_slice--;
    }

    bool timeExpired() {
        return time_slice == 0;
    }

    bool sleepDone() {
        return sleep_time == 0;
    }

    static void setSleep(uint64 time);

    void decSleep() {
        sleep_time--;
    }

    void reset() {
        time_slice = DEFAULT_TIME_SLICE;
    }

    static void yield();

    static CCB *running;

    static void dispatch();

    static void GlFunc();

    uint64 getTimeSlice() {
        return time_slice;
    }

    static List sleepy;
private:

    uint64 sleep_time = 0;
    uint64 time_slice = DEFAULT_TIME_SLICE;

    bool suspended = false;




    explicit CCB(Body body, void *arg, uint64 *a_stack) :
            args(arg), body(body)
    {
        this->stack = a_stack;
        if (body) {
            this->context.ra = (uint64) GlFunc;
        } else {
            this->context.ra = 0;
        }
        //body!=nullptr? (uint64) nullptr : 0;
        this->context.sp = stack!= nullptr? (uint64) &a_stack[DEFAULT_STACK_SIZE] : 0;
        this->finished = false;
        if (body != nullptr) { Scheduler::put(this); }
        CCB::threadsAlive++;
    };
    void initialize(Body body, void* arg, uint64* a_stack);

    struct Context
    {
        uint64 ra;   // povratna adresa
        uint64 sp;
        //
    };
    List waiting;
    void* args = nullptr;
    //pokazivac na funkciju koja se izvrsava:
    Body body = nullptr;
    //pokazivac na stek
    uint64 *stack;
    // jedan kontekst
    Context context;
    // cuvamo kontekst
    bool finished;

    static void contextSwitch(Context *oldContext, Context *runningContext);




};




