// EPOS ARM Cortex-M3 Mediator Declarations

#ifndef __cortex_m_h
#define __cortex_m_h

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

public:
    Machine() {}

    static void delay(const RTC::Microsecond & time) { Machine_Model::delay(time); }

    static void panic();
    static void reboot();
    static void poweroff() { reboot(); }

    static unsigned int n_cpus() { return 1; }
    static unsigned int cpu_id() { return 0; }

    static void smp_barrier() {};
    static void smp_init(unsigned int) {};

    static const UUID & uuid() { return Machine_Model::uuid(); }

private:
    static void pre_init(System_Info * si);
    static void init();
};

__END_SYS

#endif
