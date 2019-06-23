// EPOS ARMv7 CPU Mediator Initialization

#include <architecture.h>
#include <machine.h>

extern "C" { void __epos_library_app_entry(void); }

__BEGIN_SYS

void CPU::init()
{
    db<Init, CPU>(TRC) << "CPU::init()" << endl;

    if(Machine::cpu_id() == 0) {
        if(Traits<MMU>::enabled)
            MMU::init();
        else
            db<Init, MMU>(WRN) << "MMU is disabled!" << endl;
    }

    // Initialize the PMU
#ifdef __mmod_zynq__
    if(Traits<PMU>::enabled)
        PMU::init();
#endif

    if(Traits<TSC>::enabled)
        TSC::init();
}

__END_SYS
