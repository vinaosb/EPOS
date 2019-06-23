// EPOS PC Mediator Implementation

#include <machine/main.h>
#include <machine/pc/timer.h>
#include <machine/pc/keyboard.h>

__BEGIN_SYS

// Class attributes
volatile unsigned int Machine::_n_cpus;


// Class methods
void Machine::panic()
{
    CPU::int_disable();
    Display::position(24, 73);
    Display::puts("PANIC!");
    if(Traits<System>::reboot)
        Machine::reboot();
    else
        CPU::halt();
}

void Machine::reboot()
{
    for(int i = 0; (i < 300) && (i8042::status() & i8042::IN_BUF_FULL); i++)
        i8255::ms_delay(1);

    // Sending 0xfe to the keyboard controller port causes it to pulse
    // the reset line
    i8042::command(i8042::REBOOT);

    for(int i = 0; (i < 300) && (i8042::status() & i8042::IN_BUF_FULL); i++)
        i8255::ms_delay(1);
}

void Machine::smp_barrier(unsigned long n_cpus)
{
    static volatile unsigned long ready[2];
    static volatile unsigned long i;

    if(smp) {
        int j = i;

        CPU::finc(ready[j]);
        if(cpu_id() == 0) {
            while(ready[j] < n_cpus); // wait for all CPUs to be ready
            i = !i;                   // toggle ready
            ready[j] = 0;             // signalizes waiting CPUs
        } else {
            while(ready[j]);          // wait for CPU[0] signal
        }
    }
}

__END_SYS
