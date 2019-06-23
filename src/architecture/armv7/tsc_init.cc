// EPOS ARMv7 Time-Stamp Counter Mediator Initialization

#include <machine/main.h>
#include <machine/timer.h>

__BEGIN_SYS

void TSC::init()
{
    db<Init, TSC>(TRC) << "TSC::init()" << endl;

#if defined(__mach_cortex_a__)

    if(Machine::cpu_id() == 0) {
        // Disable counting before programming
        reg(GTCLR) = 0;

        // Set timer to 0
        reg(GTCTRL) = 0;
        reg(GTCTRH) = 0;

        // Re-enable counting
        reg(GTCLR) = 1;
    }
    
#elif defined(__mach_cortex_m__)

    reg(Machine_Model::GPTMCTL) &= ~Machine_Model::TAEN; // Disable timer
    Machine_Model::power_user_timer(Machine_Model::TIMERS - 1, FULL);
    reg(Machine_Model::GPTMCFG) = 0; // 32-bit timer
    if(Traits<Build>::MODEL == Traits<Build>::LM3S811)
        reg(Machine_Model::GPTMTAMR) = 1; // One-shot
    else {
        reg(Machine_Model::GPTMTAMR) = Machine_Model::TCDIR | 2; // Up-counting, periodic

        // Set time-out value (0xffffffff)
        reg(Machine_Model::GPTMTAILR) = -1;
        reg(Machine_Model::GPTMTAPR) = 0;

        reg(Machine_Model::GPTMIMR) |= Machine_Model::TATO_INT; // Enable timeout interrupt
        reg(Machine_Model::GPTMCTL) |= Machine_Model::TAEN; // Enable timer

        // time-out interrupt will be registered later at IC::init(), because IC hasn't been initialized yet
    }

#endif

}

__END_SYS
