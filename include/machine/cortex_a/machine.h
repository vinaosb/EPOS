// EPOS ARM Cortex-A Mediator Declarations

#ifndef __cortex_a_h
#define __cortex_a_h

#include <architecture.h>
#include <machine/main.h>
#include <machine/rtc.h>
#include __MODEL_H
#include "info.h"
#include "memory_map.h"

__BEGIN_SYS

class Machine: private Machine_Common, private Machine_Model
{
    friend class Init_System;
    friend class First_Object;

private:
    static const bool smp = Traits<System>::multicore;

public:
    Machine() {}

    static void delay(const RTC::Microsecond & time);
    
    static void panic();
    static void reboot();
    static void poweroff();

    static unsigned int n_cpus() { return smp ? _n_cpus : 1; }
    static unsigned int cpu_id() { return smp ? Machine_Model::cpu_id() : 0; }

    static void smp_init(unsigned int n_cpus) { _n_cpus = n_cpus; }
    static void smp_barrier(unsigned long n_cpus = _n_cpus);

    static const UUID & uuid() { return Machine_Model::uuid(); }

private:
    static void pre_init(System_Info * si);
    static void init();
    static int config_gic();
    static void send_sgiMe(int intID, int targetCPU, int filter);

private:
    static volatile unsigned int _n_cpus;

};

__END_SYS

#endif
