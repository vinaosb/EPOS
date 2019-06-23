// EPOS ARM Cortex-M IC Mediator Declarations

#ifndef __cortex_m_ic_h
#define __cortex_m_ic_h

#include <architecture/cpu.h>
#include <machine/ic.h>
#include __MODEL_H

__BEGIN_SYS

class NVIC: public IC_Common, protected Machine_Model
{
public:
    // IRQs
    static const unsigned int IRQS = Machine_Model::IRQS;
    typedef Interrupt_Id IRQ;
    enum {
        IRQ_GPIOA       = 0,
        IRQ_GPIOB       = 1,
        IRQ_GPIOC       = 2,
        IRQ_GPIOD       = 3,
        IRQ_GPIOE       = 4,
        IRQ_UART0       = 5,
        IRQ_UART1       = 6,
        IRQ_SSI0        = 7,
        IRQ_I2C         = 8,
        IRQ_ADC         = 14,
        IRQ_WATCHDOG    = 18,
        IRQ_GPT0A       = 19,
        IRQ_GPT0B       = 20,
        IRQ_GPT1A       = 21,
        IRQ_GPT1B       = 22,
        IRQ_GPT2A       = 23,
        IRQ_GPT2B       = 24,
        IRQ_AC          = 25,
        IRQ_RFTXRX      = 26,
        IRQ_RFERR       = 27,
        IRQ_SC          = 28,
        IRQ_FC          = 29,
        IRQ_AES         = 30,
        IRQ_PKA         = 31,
        IRQ_SMT         = 32,
        IRQ_MACTIMER    = 33,
        IRQ_SSI1        = 34,
        IRQ_GPT3A       = 35,
        IRQ_GPT3B       = 36,
        IRQ_USB         = 44, // Using alternate interrupt mapping
        IRQ_UDMASW      = 46,
        IRQ_UDMAERR     = 47,
        IRQ_LAST        = IRQ_UDMAERR
    };

    // Interrupts
    static const unsigned int INTS = 64;
    static const unsigned int LAST_INT = INTS;
    static const unsigned int EXC_INT = 0;
    static const unsigned int HARD_INT = 16;
    static const unsigned int SOFT_INT = HARD_INT + IRQS;
    enum {
        INT_HARD_FAULT  = ARMv7_M::EXC_HARD,
        INT_TIMER       = ARMv7_M::EXC_SYSTICK,
        INT_FIRST_HARD  = HARD_INT,
        INT_USER_TIMER0 = HARD_INT + IRQ_GPT0A,
        INT_USER_TIMER1 = HARD_INT + IRQ_GPT1A,
        INT_USER_TIMER2 = HARD_INT + IRQ_GPT2A,
        INT_USER_TIMER3 = HARD_INT + IRQ_GPT3A,
        INT_MACTIMER    = HARD_INT + IRQ_MACTIMER,
        INT_GPIOA       = HARD_INT + IRQ_GPIOA,
        INT_GPIOB       = HARD_INT + IRQ_GPIOB,
        INT_GPIOC       = HARD_INT + IRQ_GPIOC,
        INT_GPIOD       = HARD_INT + IRQ_GPIOD,
        INT_NIC0_RX     = HARD_INT + IRQ_RFTXRX,
        INT_NIC0_TX     = HARD_INT + IRQ_RFTXRX,
        INT_NIC0_ERR    = HARD_INT + IRQ_RFERR,
        INT_NIC0_TIMER  = HARD_INT + IRQ_MACTIMER,
        INT_USB0        = HARD_INT + IRQ_USB,
        INT_LAST_HARD   = HARD_INT + IRQS,
        INT_RESCHEDULER = SOFT_INT
    };

public:
    NVIC() {}

    static int irq2int(int i) { return i + HARD_INT; }
    static int int2irq(int i) { return i - HARD_INT; }

    static void enable() {
        db<IC>(TRC) << "IC::enable()" << endl;
        scs(IRQ_ENABLE0) = ~0;
        if(IRQS > 32) scs(IRQ_ENABLE1) = ~0;
        if(IRQS > 64) scs(IRQ_ENABLE2) = ~0;
    }

    static void enable(const Interrupt_Id & id) {
        if(id < HARD_INT)
            return;
        IRQ i = int2irq(id);
        db<IC>(TRC) << "IC::enable(irq=" << i << ")" << endl;
        assert(i < IRQS);
        if(i < 32) scs(IRQ_ENABLE0) = 1 << i;
        else if((IRQS > 32) && (i < 64)) scs(IRQ_ENABLE1) = 1 << (i - 32);
        else if(IRQS > 64) scs(IRQ_ENABLE2) = 1 << (i - 64);
    }

    static void disable() {
        db<IC>(TRC) << "IC::disable()" << endl;
        scs(IRQ_DISABLE0) = ~0;
        if(IRQS > 32) scs(IRQ_DISABLE1) = ~0;
        if(IRQS > 64) scs(IRQ_DISABLE2) = ~0;
    }

    static void disable(const Interrupt_Id & id) {
        if(id < HARD_INT)
            return;
        IRQ i = int2irq(id);
        db<IC>(TRC) << "IC::disable(irq=" << i << ")" << endl;
        assert(i < IRQS);
        if(i < 32) scs(IRQ_DISABLE0) = 1 << i;
        else if((IRQS > 32) && (i < 64)) scs(IRQ_DISABLE1) = 1 << (i - 32);
        else if(IRQS > 64) scs(IRQ_DISABLE2) = 1 << (i - 64);
        unpend(i);
    }

    // Only works in handler mode (inside IC::entry())
    static Interrupt_Id int_id() { return CPU::flags() & 0x3f; }

    static void init(void) {};

private:
    static void unpend() {
        db<IC>(TRC) << "IC::unpend()" << endl;
        scs(IRQ_UNPEND0) = ~0;
        scs(IRQ_UNPEND1) = ~0;
        scs(IRQ_UNPEND2) = ~0;
    }

    static void unpend(const IRQ & i) {
        db<IC>(TRC) << "IC::unpend(irq=" << i << ")" << endl;
        assert(i < IRQS);
        if(i < 32) scs(IRQ_UNPEND0) = 1 << i;
        else if((IRQS > 32) && (i < 64)) scs(IRQ_UNPEND1) = 1 << (i - 32);
        else if(IRQS > 64) scs(IRQ_UNPEND2) = 1 << (i - 64);
    }
};


class IC: private NVIC
{
    friend class Machine;

private:
    typedef NVIC Engine;

public:
    using IC_Common::Interrupt_Id;
    using IC_Common::Interrupt_Handler;
    using Engine::INT_TIMER;
    using Engine::INT_USER_TIMER0;
    using Engine::INT_USER_TIMER1;
    using Engine::INT_USER_TIMER2;
    using Engine::INT_USER_TIMER3;
    using Engine::INT_GPIOA;
    using Engine::INT_GPIOB;
    using Engine::INT_GPIOC;
    using Engine::INT_GPIOD;
    using Engine::INT_USB0;
    using Engine::INT_NIC0_RX;
    using Engine::INT_NIC0_TX;
    using Engine::INT_NIC0_ERR;
    using Engine::INT_NIC0_TIMER;
    using Engine::INT_RESCHEDULER;
    using Engine::LAST_INT;

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
