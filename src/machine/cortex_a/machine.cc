// EPOS Cortex-A Mediator Implementation

#include <machine/cortex_a/machine.h>
#include <machine/display.h>
#define GIC_BASE 0x1F000000
__BEGIN_SYS

// Class attributes
volatile unsigned int Machine::_n_cpus;

void Machine::panic()
{
    CPU::int_disable();
    if(Traits<Display>::enabled)
        Display::puts("PANIC!\n");
    if(Traits<System>::reboot)
        reboot();
    else
        CPU::halt();
}

void Machine::reboot()
{
    db<Machine>(WRN) << "Machine::reboot()" << endl;
    Machine_Model::reboot();
}


int Machine::config_gic()
{
// set int priority mask register
*(int *)(GIC_BASE + 0x104) = 0xFF;
// Enable CPU interface control register to signal interrupts
*(int *)(GIC_BASE + 0x100) = 1;
// Enable distributor control register to send interrupts to CPUs
*(int *)(GIC_BASE + 0x1000) = 1;
// config_int(36, 0); // timer ID=36 to CPU0
// config_int(44, 1); // UART0 ID=44 to CPU1
return 0;
}

// int irq_chandler()
// {
// // read ICCIAR of CPU interface in the GIC
// int intID = *(int *)(GIC_BASE + 0x10C);
// switch(intID){
// case 36: timer0_handler(); break; // timer interrupt
// case 44: uart_handler(0); break; // UART0 interrupt
// case 45: uart_handler(1); break; // UART1 interrupt
// case 52: kbd_handler(); break; // KBD interrupt
// }
// *(int *)(GIC_BASE + 0x110) = intID; // write EOI
// }
//SGI method
void Machine::send_sgiMe(int intID, int targetCPU, int filter)
{
    int *sgi_reg = (int *)(0x1f000000 + 0x1F00);
    *sgi_reg = (filter << 24) | ((1 << targetCPU) << 16) | (intID);
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
