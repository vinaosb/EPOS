// EPOS GPIO Mediator Common package

#ifndef __gpio_h
#define __gpio_h

#include <system/config.h>

__BEGIN_SYS

class GPIO_Common
{
protected:
    GPIO_Common() {}

public:
    enum Level {
        HIGH,
        LOW
    };

    enum Edge {
        RISING,
        FALLING,
        BOTH,
        NONE
    };

    enum Direction {
        IN,
        OUT,
        INOUT
    };

    enum Pull {
        UP,
        DOWN,
        FLOATING
    };

    typedef unsigned int Port;
    enum {
        A,
        B,
        C,
        D,
        E,
        F
    };

    typedef unsigned int Pin;
};

__END_SYS

#ifdef __GPIO_H
#include __GPIO_H
#endif

#endif
