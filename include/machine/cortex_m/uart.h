// EPOS Cortex-M UART Mediator Declarations

#ifndef __cortex_m_uart_h__
#define __cortex_m_uart_h__

#include <architecture/cpu.h>
#include <machine/uart.h>
#include __MODEL_H
#include <machine/common/pl011.h>

__BEGIN_SYS

typedef PL011 UART_Engine;

class UART: private UART_Common, private UART_Engine
{
private:
    typedef UART_Engine Engine;

    static const unsigned int UNIT = Traits<UART>::DEF_UNIT;
    static const unsigned int BAUD_RATE = Traits<UART>::DEF_BAUD_RATE;
    static const unsigned int DATA_BITS = Traits<UART>::DEF_DATA_BITS;
    static const unsigned int PARITY = Traits<UART>::DEF_PARITY;
    static const unsigned int STOP_BITS = Traits<UART>::DEF_STOP_BITS;

public:
    UART(unsigned int unit = UNIT, unsigned int baud_rate = BAUD_RATE, unsigned int data_bits = DATA_BITS, unsigned int parity = PARITY, unsigned int stop_bits = STOP_BITS)
    : Engine((enable_uart(unit), unit), baud_rate, data_bits, parity, stop_bits), _unit(unit) {}

    void config(unsigned int baud_rate, unsigned int data_bits, unsigned int parity, unsigned int stop_bits) {
        Engine::config(baud_rate, data_bits, parity, stop_bits);
    }
    void config(unsigned int * baud_rate, unsigned int * data_bits, unsigned int * parity, unsigned int * stop_bits) {
        Engine::config(*baud_rate, *data_bits, *parity, *stop_bits);
    }

    char get() { while(!rxd_ok()); return rxd(); }
    void put(char c) { while(!txd_ok()); txd(c); }

    void flush() { while(!txd_empty()); }

    bool ready_to_get() { return rxd_ok(); }
    bool ready_to_put() { return txd_ok(); }

    void int_enable(bool receive = true, bool send = true, bool line = true, bool modem = true) {
        Engine::int_enable(receive, send, line, modem);
    }
    void int_disable(bool receive = true, bool send = true, bool line = true, bool modem = true) {
        Engine::int_disable(receive, send, line, modem);
    }

    void reset() { Engine::reset(); }
    void loopback(bool flag) { Engine::loopback(flag); }

    void power(const Power_Mode & mode) { power_uart(_unit, mode); }

private:
    unsigned int _unit;
};

__END_SYS

#endif
