// EPOS Cortex-A Mediator Initialization

#include <machine.h>
#include<machine/cortex_a/scu.h>
__BEGIN_SYS

void Machine::pre_init(System_Info * si)
{
    Machine_Model::pre_init();
    Display::init(); 
   if(Traits<Build> ::CPUS >= 2){
        smp_init(Traits<Build>::CPUS);

       enable_scu();         
       join_smp();
        
        int *apAddr = (int *)0x10000030; // SYS_FLAGSSET register
         *apAddr = (int)0x10000;          // all APs execute from 0x10000
        config_gic();
        send_sgiMe(0x00,0x0f,0x01);
     }

   
} 


void Machine::init()
{
    db<Init, Machine>(TRC) << "Machine::init()" << endl;

    Machine_Model::init();

    if(Traits<IC>::enabled)
        IC::init();
    if(Traits<Timer>::enabled)
        Timer::init();
#ifdef __USB_H
    if(Traits<USB>::enabled)
        USB::init();
#endif
}

__END_SYS
