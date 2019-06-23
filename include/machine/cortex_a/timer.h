// EPOS Cortex-A Timer Mediator Declarations

#ifndef __cortex_a_timer_h
#define __cortex_a_timer_h

#include <architecture.h>
#include <machine/ic.h>
#include <machine/rtc.h>
#include <machine/timer.h>
#include <machine/main.h>
#include __MODEL_H

__BEGIN_SYS

// Cortex-A Private Timer
class System_Timer_Engine: public Machine_Model
{
public:
    typedef CPU::Reg32 Count;
    static const unsigned int CLOCK = Traits<CPU>::CLOCK;

protected:
    System_Timer_Engine() {}

public:
    static TSC::Hertz clock() { return CLOCK; }

    static void enable() { priv_timer(PTCLR) |= PT_TIMER_ENABLE; }
    static void disable() { priv_timer(PTCLR) &= ~PT_TIMER_ENABLE; }

    static void eoi(const IC::Interrupt_Id & int_id) { priv_timer(PTISR) = PT_INT_CLR; }

    void power(const Power_Mode & mode);

    static void init(unsigned int f) {
        priv_timer(PTCLR) = 0;
        priv_timer(PTISR) = PT_INT_CLR;
        priv_timer(PTLR) = CLOCK / f;
        priv_timer(PTCLR) = PT_IRQ_EN | PT_AUTO_RELOAD;
    }
};

// Cortex-A Global Timer
class User_Timer_Engine: public Machine_Model
{
protected:
    typedef CPU::Reg64 Count;
    typedef TSC::Hertz Hertz;

public:
    static const Hertz CLOCK = Traits<CPU>::CLOCK / 2;

public:
    User_Timer_Engine(unsigned int channel, const Count & count, bool interrupt = true, bool periodic = true);

    static Hertz clock() { return CLOCK; }

    Count read() {
        Reg32 high, low;

        do {
            high = global_timer(GTCTRH);
            low = global_timer(GTCTRL);
        } while(global_timer(GTCTRH) != high);

        return static_cast<Count>(high) << 32 | low;
    }

    static void enable();
    static void disable();

    void set(const Count & count) {
        // Disable counting before programming
        global_timer(GTCLR) = 0;

        global_timer(GTCTRL) = count & 0xffffffff;
        global_timer(GTCTRH) = count >> 32;

        // Re-enable counting
        global_timer(GTCLR) = 1;
    }

protected:
    static void eoi(const IC::Interrupt_Id & int_id) {}
};

// Tick timer used by the system
class Timer: private Timer_Common, private System_Timer_Engine
{
    friend class Machine;
    friend class Init_System;

protected:
    static const unsigned int CHANNELS = 2;
    static const unsigned int FREQUENCY = Traits<Timer>::FREQUENCY;

    typedef System_Timer_Engine Engine;
    typedef Engine::Count Count;
    typedef IC::Interrupt_Id Interrupt_Id;

public:
    using Timer_Common::Hertz;
    using Timer_Common::Tick;
    using Timer_Common::Handler;

    // Channels
    enum {
        SCHEDULER,
        ALARM,
        USER
    };

protected:
    Timer(unsigned int channel, const Hertz & frequency, const Handler & handler, bool retrigger = true)
    : _channel(channel), _initial(FREQUENCY / frequency), _retrigger(retrigger), _handler(handler) {
        db<Timer>(TRC) << "Timer(f=" << frequency << ",h=" << reinterpret_cast<void*>(handler) << ",ch=" << channel << ") => {count=" << _initial << "}" << endl;

        if(_initial && (channel < CHANNELS) && !_channels[channel])
            _channels[channel] = this;
        else
            db<Timer>(WRN) << "Timer not installed!"<< endl;

        for(unsigned int i = 0; i < Traits<Machine>::CPUS; i++)
            _current[i] = _initial;
    }

public:
    ~Timer() {
        db<Timer>(TRC) << "~Timer(f=" << frequency() << ",h=" << reinterpret_cast<void*>(_handler) << ",ch=" << _channel << ") => {count=" << _initial << "}" << endl;

        _channels[_channel] = 0;
    }

    Hertz frequency() const { return (FREQUENCY / _initial); }
    void frequency(const Hertz & f) { _initial = FREQUENCY / f; reset(); }

    Tick read() { return _current[Machine::cpu_id()]; }

    int reset() {
        db<Timer>(TRC) << "Timer::reset() => {f=" << frequency()
                       << ",h=" << reinterpret_cast<void*>(_handler)
                       << ",count=" << _current[Machine::cpu_id()] << "}" << endl;

        int percentage = _current[Machine::cpu_id()] * 100 / _initial;
        _current[Machine::cpu_id()] = _initial;

        return percentage;
    }

    void handler(const Handler & handler) { _handler = handler; }

    static void eoi(const IC::Interrupt_Id & int_id) { Engine::eoi(int_id); }

private:
    static Hertz count2freq(const Count & c) { return c ? Engine::clock() / c : 0; }
    static Count freq2count(const Hertz & f) { return f ? Engine::clock() / f : 0;}

    static void int_handler(const Interrupt_Id & i);

    static void init();

private:
    unsigned int _channel;
    Count _initial;
    bool _retrigger;
    volatile Count _current[Traits<Machine>::CPUS];
    Handler _handler;

    static Timer * _channels[CHANNELS];
};

// Timer used by Thread::Scheduler
class Scheduler_Timer: public Timer
{
private:
    typedef RTC::Microsecond Microsecond;

public:
    Scheduler_Timer(const Microsecond & quantum, const Handler & handler): Timer(SCHEDULER, 1000000 / quantum, handler) {}
};

// Timer used by Alarm
class Alarm_Timer: public Timer
{
public:
    static const unsigned int FREQUENCY = Timer::FREQUENCY;

public:
    Alarm_Timer(const Handler & handler): Timer(ALARM, FREQUENCY, handler) {}
};


// User timer
class User_Timer: private Timer_Common, private User_Timer_Engine
{
    friend class PWM;

private:
    typedef User_Timer_Engine Engine;

public:
    using Timer_Common::Microsecond;
    using Timer_Common::Handler;

public:
    User_Timer(unsigned int channel, const Microsecond & time, const Handler & handler, bool periodic = false)
    : Engine(channel, us2count(time), handler ? true : false, periodic), _channel(channel), _handler(handler) {
        assert(channel < Machine_Model::TIMERS - Traits<TSC>::enabled); // TSC uses the last timer channel. To use the last channel, you must disable the TSC
        if(_handler) {
            IC::Interrupt_Id id = _channel == 0 ? IC::INT_USER_TIMER0 : _channel == 1 ? IC::INT_USER_TIMER1 :
                                  _channel == 2 ? IC::INT_USER_TIMER2 : IC::INT_USER_TIMER3;
            IC::int_vector(id, _handler);
            IC::enable(id);
        }
    }
    ~User_Timer() {
        if(_handler) {
            IC::Interrupt_Id id = _channel == 0 ? IC::INT_USER_TIMER0 : _channel == 1 ? IC::INT_USER_TIMER1 :
                                  _channel == 2 ? IC::INT_USER_TIMER2 : IC::INT_USER_TIMER3;
            IC::disable(id);
        }
    }

    Microsecond read() { return count2us(Engine::read()); }

    void enable() { Engine::enable(); }
    void disable() { Engine::disable(); }
    void power(const Power_Mode & mode) { power_user_timer(_channel, mode); }

    static void eoi(const IC::Interrupt_Id & int_id) { Engine::eoi(int_id); }

private:
    static Count us2count(const Microsecond & us) { return static_cast<unsigned long long>(us) * CLOCK / 1000000; }
    static Microsecond count2us(const Count & count) { return static_cast<unsigned long long>(count) * 1000000 / CLOCK; }

private:
    unsigned int _channel;
    Handler _handler;
};

__END_SYS

#endif
