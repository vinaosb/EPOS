// EPOS RealView PBX A9 (ARM Cortex A9) MCU Initialization

#include <machine/main.h>
#include<machine/cortex_a/scu.h>
#ifdef __mmod_realview_pbx__

void _startup_cache();

__BEGIN_SYS

void RealView_PBX::pre_init()
{
       ASM("mcr p15, 0, %0, c12, c0, 0" : : "p"(Traits<Machine>::VECTOR_TABLE));
}

void RealView_PBX::init()
{
}

__END_SYS

#endif