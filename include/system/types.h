// EPOS Internal Type Management System

typedef __SIZE_TYPE__ size_t;

#ifndef __types_h
#define __types_h

// Memory allocators
__BEGIN_API

enum System_Allocator
{
    SYSTEM
};
enum Scratchpad_Allocator
{
    SCRATCHPAD
};
enum Color
{
    COLOR_0,
    COLOR_1,
    COLOR_2,
    COLOR_3,
    COLOR_4,
    COLOR_5,
    COLOR_6,
    COLOR_7,
    COLOR_8,
    COLOR_9,
    COLOR_10,
    COLOR_11,
    COLOR_12,
    COLOR_13,
    COLOR_14,
    COLOR_15,
    COLOR_16,
    COLOR_17,
    COLOR_18,
    COLOR_19,
    COLOR_20,
    COLOR_21,
    COLOR_22,
    COLOR_23,
    COLOR_24,
    COLOR_25,
    COLOR_26,
    COLOR_27,
    COLOR_28,
    COLOR_29,
    COLOR_30,
    COLOR_31,
    WHITE = COLOR_0
};

// Power Management Modes
enum Power_Mode
{
    ENROLL,
    DISMISS,
    SAME,
    FULL,
    LIGHT,
    SLEEP,
    OFF
};

__END_API

extern "C"
{
    void *malloc(size_t);
    void free(void *);
}

inline void *operator new(size_t s, void *a) { return a; }
inline void *operator new[](size_t s, void *a) { return a; }

void *operator new(size_t, const EPOS::System_Allocator &);
void *operator new[](size_t, const EPOS::System_Allocator &);

void *operator new(size_t, const EPOS::Scratchpad_Allocator &);
void *operator new[](size_t, const EPOS::Scratchpad_Allocator &);

void *operator new(size_t, const EPOS::Color &);
void *operator new[](size_t, const EPOS::Color &);

// Utilities
// Generic names such as "lists" and "hashes" are used to specify Traits<> and control debugging for all related utilities
__BEGIN_UTIL

template <int BITS>
class Padding
{
} __attribute__((packed));
template <>
class Padding<8>
{
    char _padding;
} __attribute__((packed));
template <>
class Padding<16>
{
    short int _padding;
} __attribute__((packed));
template <>
class Padding<32>
{
    long int _padding;
} __attribute__((packed));
template <>
class Padding<64>
{
    long long int _padding;
} __attribute__((packed));

typedef unsigned char Percent;
typedef unsigned char UUID[8];

class Bitmaps;
class CRC;
class Debug;
class Dummy
{
};
class ELF;
class Handler;
class Hashes;
class Heaps;
class Lists;
class Observeds;
class Observers;
class OStream;
class Queues;
class Random;
class Spin;
class SREC;
class Vectors;
template <unsigned int KEY_SIZE>
class _AES;
template <typename>
class Scheduler;
namespace Scheduling_Criteria
{
class Priority;
class FCFS;
class RR;
class FS;
class GRR;
class CPU_Affinity;
class CPU_Affinity_HRRN;
class HRRN;
class GHRRN;
}; // namespace Scheduling_Criteria

__END_UTIL

__BEGIN_SYS

// System parts
class Build;
class Boot;
class Setup;
class Init;
class Utility;

// Architecture Hardware Mediators
class CPU;
class TSC;
class MMU;
class FPU;
class PMU;

// Machine Hardware Mediators
class Machine;
class PCI;
class IC;
class Timer;
class RTC;
class UART;
class SPI;
class RS485;
class USB;
class EEPROM;
class Display;
class Serial_Display;
class Keyboard;
class Serial_Keyboard;
class Scratchpad;
class Watchdog;
template <unsigned int KEY_SIZE>
class _AES;
class GPIO;
class I2C;
class ADC;

// Components
class System;
class Application;

class Thread;
class Active;

class Address_Space;
class Segment;

class Synchronizer;
class Mutex;
class Semaphore;
class Condition;

class Time;
class Clock;
class Chronometer;
class Alarm;
class Delay;

__END_SYS

#endif
