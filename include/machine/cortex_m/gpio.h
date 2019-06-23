// EPOS ARM Cortex-M GPIO Mediator Declarations

#include <system/config.h>

#if !defined(__cortex_m_gpio_h_) && defined(__GPIO_H)
#define __cortex_m_gpio_h_

#include <machine/ic.h>
#include <machine/gpio.h>
#include <utility/observer.h>

__BEGIN_SYS

class GPIO: public GPIO_Common, private Machine_Model, public Observed
{
    friend class PWM;

private:
    static const bool supports_power_up = Machine_Model::supports_gpio_power_up;

public:
    typedef _UTIL::Observed Observed;
    typedef _UTIL::Observer Observer;

public:
    GPIO(char port, unsigned int pin, const Direction & dir, const Pull & p = UP, const Edge & int_edge = NONE)
    : _port(port), _pin(pin), _pin_bit(1 << pin), _data(&gpio(_port, _pin_bit << 2)) {
        assert(port < GPIO_PORTS);
        gpio(_port, AFSEL) &= ~_pin_bit; // Set pin as software controlled
        direction(dir);
        pull(p);
        clear_interrupt();
        if(int_edge != NONE) {
            _devices[_port][_pin] = this;
            int_enable(int_edge);
        }
    }

    ~GPIO() {
        int_disable();
        _devices[_port][_pin] = 0;
    }

    bool get() const {
        assert(_direction == IN || _direction == INOUT);
        return *_data;
    }

    void set(bool value = true) {
        assert(_direction == INOUT || _direction == OUT);
        if(_direction == INOUT) {
            gpio(_port, DIR) |= _pin_bit;
            *_data = 0xff * value;
            gpio(_port, DIR) &= ~_pin_bit;
        } else
            *_data = 0xff * value;
    }

    void clear() { set(false); }

    void direction(const Direction & dir) {
        _direction = dir;
        switch(dir) {
            case OUT:
                gpio(_port, DIR) |= _pin_bit;
                break;
            case IN:
            case INOUT:
                gpio(_port, DIR) &= ~_pin_bit;
                break;
        }
    }

    void pull(const Pull & p) {
        switch(p) {
            case UP:
                gpio_pull_up(_port, _pin);
                break;
            case DOWN:
                gpio_pull_down(_port, _pin);
                break;
            case FLOATING:
                gpio_floating(_port, _pin);
                break;
        }
    }

    void int_enable() { gpio(_port, IM) |= _pin_bit; }
    void int_enable(const Level & level, bool power_up = false, const Level & power_up_level = HIGH);
    void int_enable(const Edge & edge, bool power_up = false, const Edge & power_up_edge = RISING);
    void int_disable() { gpio(_port, IM) &= ~_pin_bit; }

    static void eoi(const IC::Interrupt_Id & i);

private:
    void clear_interrupt() {
        gpio(_port, ICR) = _pin_bit;
        gpio(_port, IRQ_DETECT_ACK) &= ~(_pin_bit << (8 * _port));
    }

    static void int_handler(const IC::Interrupt_Id & i);

private:
    Port _port;
    Pin _pin;
    unsigned int _pin_bit;
    Direction _direction;
    volatile Reg32 * _data;

    static GPIO * _devices[GPIO_PORTS][8];
    static unsigned char _mis[GPIO_PORTS];
    static unsigned int _irq_detect_ack[GPIO_PORTS];
};

__END_SYS

#endif
