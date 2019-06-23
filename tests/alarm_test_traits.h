#ifndef __traits_h
#define __traits_h

#include <system/config.h>

__BEGIN_SYS

// Global Configuration
template<typename T>
struct Traits
{
    // EPOS software architecture (aka mode)
    enum {LIBRARY, BUILTIN, KERNEL};

    // CPU hardware architectures
    enum {AVR8, H8, ARMv4, ARMv7, ARMv8, IA32, X86_64, SPARCv8, PPC32};

    // Machines
    enum {eMote1, eMote2, STK500, RCX, Cortex_A, Cortex_M, Cortex_R, PC, Leon, Virtex};

    // Machine models
    enum {Unique, Legacy_PC, eMote3, LM3S811, Realview_PBX, Zynq};

    // Serial display engines
    enum {UART, USB};

    // Life span multipliers
    enum {FOREVER = 0, SECOND = 1, MINUTE = 60, HOUR = 3600, DAY = 86400, WEEK = 604800, MONTH = 2592000, YEAR = 31536000};

    // IP configuration strategies
    enum {STATIC, MAC, INFO, RARP, DHCP};

    // SmartData predictors
    enum :unsigned char {NONE, LVP, DBP};

    // Default traits
    static const bool enabled = true;
    static const bool debugged = true;
    static const bool monitored = false;
    static const bool hysterically_debugged = false;

    typedef LIST<> DEVICES;
    typedef TLIST<> ASPECTS;
};

template<> struct Traits<Build>: public Traits<void>
{
    static const unsigned int MODE = LIBRARY;
    static const unsigned int ARCHITECTURE = ARMv7;
    static const unsigned int MACHINE = Cortex_M;
    static const unsigned int MODEL = LM3S811;
    static const unsigned int CPUS = 1;
    static const unsigned int NODES = 1; // (> 1 => NETWORKING)
    static const unsigned int EXPECTED_SIMULATION_TIME = 60; // s (0 => not simulated)
};


// Utilities
template<> struct Traits<Debug>: public Traits<void>
{
    static const bool error   = true;
    static const bool warning = true;
    static const bool info    = false;
    static const bool trace   = false;
};

template<> struct Traits<Lists>: public Traits<void>
{
    static const bool debugged = hysterically_debugged;
};

template<> struct Traits<Spin>: public Traits<void>
{
    static const bool debugged = hysterically_debugged;
};

template<> struct Traits<Heaps>: public Traits<void>
{
    static const bool debugged = hysterically_debugged;
};

template<> struct Traits<Observers>: public Traits<void>
{
    // Some observed objects are created before initializing the Display
    // Enabling debug may cause trouble in some Machines
    static const bool debugged = false;
};

// System Parts (mostly to fine control debugging)
template<> struct Traits<Boot>: public Traits<void>
{
};

template<> struct Traits<Setup>: public Traits<void>
{
};

template<> struct Traits<Init>: public Traits<void>
{
};

// Mediators
template<> struct Traits<Serial_Display>: public Traits<void>
{
    static const bool enabled = (Traits<Build>::EXPECTED_SIMULATION_TIME != 0);
    static const int ENGINE = UART;
    static const int COLUMNS = 80;
    static const int LINES = 24;
    static const int TAB_SIZE = 8;
};

template<> struct Traits<Serial_Keyboard>: public Traits<void>
{
    static const bool enabled = (Traits<Build>::EXPECTED_SIMULATION_TIME != 0);
};

__END_SYS

#include __ARCH_TRAITS_H
#include __MACH_TRAITS_H

__BEGIN_SYS


// Components
template<> struct Traits<Application>: public Traits<void>
{
    static const unsigned int STACK_SIZE = Traits<Machine>::STACK_SIZE;
    static const unsigned int HEAP_SIZE = Traits<Machine>::HEAP_SIZE;
    static const unsigned int MAX_THREADS = Traits<Machine>::MAX_THREADS;
};

template<> struct Traits<System>: public Traits<void>
{
    static const unsigned int mode = Traits<Build>::MODE;
    static const bool multithread = (Traits<Build>::CPUS > 1) || (Traits<Application>::MAX_THREADS > 1);
    static const bool multitask = (mode != Traits<Build>::LIBRARY);
    static const bool multicore = (Traits<Build>::CPUS > 1) && multithread;
    static const bool multiheap = multitask || Traits<Scratchpad>::enabled;

    static const unsigned long LIFE_SPAN = 1 * YEAR; // s

    static const bool reboot = true;

    static const unsigned int STACK_SIZE = Traits<Machine>::STACK_SIZE;
    static const unsigned int HEAP_SIZE = (Traits<Application>::MAX_THREADS + 1) * Traits<Application>::STACK_SIZE;
};

template<> struct Traits<Thread>: public Traits<void>
{
    static const bool enabled = Traits<System>::multithread;
    static const bool smp = Traits<System>::multicore;
    static const bool trace_idle = hysterically_debugged;

    typedef Scheduling_Criteria::Priority Criterion;
    static const unsigned int QUANTUM = 10000; // us
};

template<> struct Traits<Scheduler<Thread>>: public Traits<void>
{
    static const bool debugged = Traits<Thread>::trace_idle || hysterically_debugged;
};

template<> struct Traits<Alarm>: public Traits<void>
{
    static const bool visible = hysterically_debugged;
};

template<> struct Traits<Synchronizer>: public Traits<void>
{
    static const bool enabled = Traits<System>::multithread;
};

__END_SYS

#endif
