// EPOS ARM Cortex-A IC Mediator Declarations

#ifndef __cortex_a_ic_h
#define __cortex_a_ic_h

#include <architecture/cpu.h>
#include <machine/ic.h>
#include __MODEL_H

__BEGIN_SYS

class GIC: public IC_Common, protected Machine_Model
{
public:
    // IRQs
    static const unsigned int IRQS = Machine_Model::IRQS;
    typedef Interrupt_Id IRQ;
    enum {
        IRQ_SOFTWARE0           = 0,
        IRQ_SOFTWARE1           = 1,
        IRQ_SOFTWARE2           = 2,
        IRQ_SOFTWARE3           = 3,
        IRQ_SOFTWARE4           = 4,
        IRQ_SOFTWARE5           = 5,
        IRQ_SOFTWARE6           = 6,
        IRQ_SOFTWARE7           = 7,
        IRQ_SOFTWARE8           = 8,
        IRQ_SOFTWARE9           = 9,
        IRQ_SOFTWARE10          = 10,
        IRQ_SOFTWARE11          = 11,
        IRQ_SOFTWARE12          = 12,
        IRQ_SOFTWARE13          = 13,
        IRQ_SOFTWARE14          = 14,
        IRQ_SOFTWARE15          = 15,
        IRQ_GIC0                = 16,
        IRQ_GIC1                = 17,
        IRQ_GIC2                = 18,
        IRQ_GIC3                = 19,
        IRQ_GIC4                = 20,
        IRQ_GIC5                = 21,
        IRQ_GIC6                = 22,
        IRQ_GIC7                = 23,
        IRQ_GIC8                = 24,
        IRQ_GIC9                = 25,
        IRQ_GIC10               = 26,
        IRQ_GIC11_GLOBAL_TIMER  = 27,
        IRQ_GIC12               = 28,
        IRQ_GIC13_PRIV_TIMER    = 29,
        IRQ_GIC14               = 30,
        IRQ_GIC15               = 31,
        IRQ_WATCHDOG            = 32,
        IRQ_SOFTWARE            = 33,
        IRQ_COMMRX              = 34,
        IRQ_COMMTX              = 35,
        IRQ_TIMER0AND1          = 36,
        IRQ_TIMER2AND3          = 37,
        IRQ_RTC                 = 42,
        IRQ_UART0               = 44,
        IRQ_UART1               = 45,
        IRQ_UART2               = 46,
        IRQ_UART3               = 47,
        IRQ_DMAC                = 56,
        IRQ_T1_INT0             = 64,
        IRQ_T1_INT1             = 65,
        IRQ_T1_INT2             = 66,
        IRQ_T1_INT3             = 67,
        IRQ_T1_INT4             = 68,
        IRQ_T1_INT5             = 69,
        IRQ_T1_INT6             = 70,
        IRQ_T1_INT7             = 71,
        IRQ_T2_INT0             = 72,
        IRQ_T2_INT1             = 73,
        IRQ_T2_INT2             = 74,
        IRQ_T2_INT3             = 75,
        IRQ_T2_INT4             = 76,
        IRQ_T2_INT5             = 77,
        IRQ_T2_INT6             = 78,
        IRQ_T2_INT7             = 79,
    };

    // Interrupts
    static const unsigned int INTS = 96;
    static const unsigned int EXC_INT = 0;
    enum {
        INT_TIMER = IRQ_GIC13_PRIV_TIMER,
        INT_USER_TIMER0  = IRQ_TIMER0AND1,
        INT_USER_TIMER1  = IRQ_TIMER0AND1,
        INT_USER_TIMER2  = IRQ_TIMER2AND3,
        INT_USER_TIMER3  = IRQ_TIMER2AND3,
        INT_RESCHEDULER  = IRQ_SOFTWARE0,
    };

public:
    GIC() {}

    static int irq2int(int i) { return i; }
    static int int2irq(int i) { return i; }

    static void enable() {
        int_dist(GIC_DIST_SETENABLE0) = ~0;
        int_dist(GIC_DIST_SETENABLE1) = ~0;
        int_dist(GIC_DIST_SETENABLE2) = ~0;
    }

    static void enable(const Interrupt_Id & id) {
        int_dist(GIC_DIST_SETENABLE0 + (id / 32) * 4) = 1 << (id % 32);
    }

    static void disable() {
        int_dist(GIC_DIST_CLEARENABLE0) = ~0;
        int_dist(GIC_DIST_CLEARENABLE1) = ~0;
        int_dist(GIC_DIST_CLEARENABLE2) = ~0;
    }

    static void disable(const Interrupt_Id & id) {
        int_dist(GIC_DIST_CLEARENABLE0 + (id / 32) * 4) = 1 << (id % 32);
    }

    // Only works in handler mode (inside IC::entry())
    static Interrupt_Id int_id() { 
        Reg32 int_ack = gic(GIC_INT_ACK) & 0X3ff; // 10 bits mask

        gic(GIC_EOI) = int_ack;
        return int_ack;
    }

    static void init() {
        // Set interrup priority mask
        gic(GIC_PRIORITY_CONTROL) = 0xFF;
        
        // Enable CPU interface control register to signal interrupts
        gic(GIC_CPU_CONTROL) = 1;
    
        // Enable distributor control register to send interrupts to CPUs
        int_dist(GIC_DIST_CONTROL_REGISTER) = 1;
    };
};

class IC: private GIC
{
    friend class Machine;

private:
    typedef GIC Engine;

public:
    using IC_Common::Interrupt_Id;
    using IC_Common::Interrupt_Handler;
    using Engine::INT_TIMER;
    using Engine::INT_USER_TIMER0;
    using Engine::INT_USER_TIMER1;
    using Engine::INT_USER_TIMER2;
    using Engine::INT_USER_TIMER3;
    using Engine::INT_RESCHEDULER;

public:
    IC() {}

    static Interrupt_Handler int_vector(const Interrupt_Id & i) {
        assert(i < INTS);
        return _int_vector[i];
    }

    static void int_vector(const Interrupt_Id & i, const Interrupt_Handler & h) {
        db<IC>(TRC) << "IC::int_vector(int=" << i << ",h=" << reinterpret_cast<void *>(h) <<")" << endl;
        assert(i < INTS);
        _int_vector[i] = h;
    }

    static void enable() {
        db<IC>(TRC) << "IC::enable()" << endl;
        Engine::enable();
    }

    static void enable(const Interrupt_Id & i) {
        db<IC>(TRC) << "IC::enable(int=" << i << ")" << endl;
        assert(i < INTS);
        Engine::enable(i);
    }

    static void disable() {
        db<IC>(TRC) << "IC::disable()" << endl;
        Engine::disable();
    }

    static void disable(const Interrupt_Id & i) {
        db<IC>(TRC) << "IC::disable(int=" << i << ")" << endl;
        assert(i < INTS);
        Engine::disable(i);
    }

    using Engine::irq2int;

    static void ipi_send(unsigned int cpu, Interrupt_Id int_id) {}

    void undefined_instruction();
    void software_interrupt();
    void prefetch_abort();
    void data_abort();
    void reserved();
    void fiq();

private:
    static void dispatch(unsigned int i);
    static void eoi(unsigned int i);

    // Logical handlers
    static void int_not(const Interrupt_Id & i);
    static void hard_fault(const Interrupt_Id & i);

    // Physical handler
    static void entry();

    static void init();

private:
    static Interrupt_Handler _int_vector[INTS];
    static Interrupt_Handler _eoi_vector[INTS];
};

__END_SYS

#endif
