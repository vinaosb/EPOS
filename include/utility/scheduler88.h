// EPOS Scheduler Component Declarations

#ifndef __scheduler_h
#define __scheduler_h

#include <architecture.h>
#include <utility/list.h>
#include <machine/timer.h>

__BEGIN_UTIL

// All scheduling criteria, or disciplines, must define operator int() with
// the semantics of returning the desired order of a given object within the
// scheduling list
namespace Scheduling_Criteria
{
    // Priority (static and dynamic)
    class Priority
    {
    public:
        // Common priorities
        enum {
            MAIN   = 0,
            HIGH   = 1,
            NORMAL = (unsigned(1) << (sizeof(int) * 8 - 1)) - 3,
            LOW    = (unsigned(1) << (sizeof(int) * 8 - 1)) - 2,
            IDLE   = (unsigned(1) << (sizeof(int) * 8 - 1)) - 1
        };

        // Constructor helpers
        enum {
            ANY         = -1
        };

        // Policy traits
        static const bool timed = false;
        static const bool dynamic = false;
        static const bool preemptive = true;
        static const unsigned int QUEUES = 1;

    public:
        template <typename ... Tn>
        Priority(int p = NORMAL, Tn & ... an): _priority(p) {}

        operator const volatile int() const volatile { return _priority; }

        Priority operator +=(Priority rhs) {
            if(_priority < (IDLE - rhs)) {
                _priority += rhs;
            }
            return *this;
        }

        unsigned int queue() const { return 0; }

    protected:
        volatile int _priority;
    };

    // Round-Robin
    class RR: public Priority
    {
    public:
        static const bool timed = true;
        static const bool dynamic = false;
        static const bool preemptive = true;

    public:
        template <typename ... Tn>
        RR(int p = NORMAL, Tn & ... an): Priority(p) {}
    };

    // Feedback Scheduling
    class FS: public Priority
    {
    public:
        enum {
            MAIN   = 0,
            NORMAL = 1,
            IDLE   = (unsigned(1) << (sizeof(int) * 8 - 1)) - 1
        };

        static const bool timed = true;
        static const bool dynamic = true;
        static const bool preemptive = true;

    public:
        FS(int p = NORMAL): Priority(p) {}
    };

    // First-Come, First-Served (FIFO)
    class FCFS: public Priority
    {
    public:
        static const bool timed = false;
        static const bool dynamic = false;
        static const bool preemptive = false;

    public:
        FCFS(int p = NORMAL);

        template <typename ... Tn>
        FCFS(Tn & ... an) {}
    };


    // Multicore Algorithms
    class Variable_Queue
    {
    protected:
        Variable_Queue(unsigned int queue): _queue(queue) {};

    public:
        const volatile unsigned int & queue() const volatile { return _queue; }

    protected:
        volatile unsigned int _queue;
        static volatile unsigned int _next_queue;
    };
    
    // CPU Affinity
    class CPU_Affinity: public Priority, public Variable_Queue
    {
    public:
        static const bool timed = false;
        static const bool dynamic = false;
        static const bool preemptive = true;

        static const unsigned int QUEUES = Traits<Machine>::CPUS;

    public:
        template <typename ... Tn>
        CPU_Affinity(int p = NORMAL, int cpu = ANY, Tn & ... an)
        : Priority(p), Variable_Queue(((_priority == IDLE) || (_priority == MAIN)) ? Machine::cpu_id() : (cpu != ANY) ? cpu : ++_next_queue %= Machine::n_cpus()) {}

        using Variable_Queue::queue;

        static unsigned int current_queue() { return Machine::cpu_id(); }
    };
}


// Scheduling_Queue
template<typename T, typename R = typename T::Criterion>
class Scheduling_Queue: public Scheduling_List<T> {};

template<typename T>
class Scheduling_Queue<T, Scheduling_Criteria::CPU_Affinity>:
public Scheduling_Multilist<T> {};

// Scheduler
// Objects subject to scheduling by Scheduler must declare a type "Criterion"
// that will be used as the scheduling queue sorting criterion (viz, through
// operators <, >, and ==) and must also define a method "link" to export the
// list element pointing to the object being handled.
template<typename T>
class Scheduler: public Scheduling_Queue<T>
{
private:
    typedef Scheduling_Queue<T> Base;

public:
    typedef typename T::Criterion Criterion;
    typedef Scheduling_List<T, Criterion> Queue;
    typedef typename Queue::Element Element;

public:
    Scheduler() {}

    unsigned int schedulables() { return Base::size(); }

    T * volatile chosen() {
    	// If called before insert(), chosen will dereference a null pointer!
    	// For threads, we assume this won't happen (see Init_First).
    	// But if you are unsure about your new use of the scheduler,
    	// please, pay the price of the extra "if" bellow.
//    	return const_cast<T * volatile>((Base::chosen()) ? Base::chosen()->object() : 0);
    	return const_cast<T * volatile>(Base::chosen()->object());
    }

    void insert(T * obj) {
        db<Scheduler>(TRC) << "Scheduler[chosen=" << chosen() << "]::insert(" << obj << ")" << endl;

        Base::insert(obj->link());
    }

    T * remove(T * obj) {
        db<Scheduler>(TRC) << "Scheduler[chosen=" << chosen() << "]::remove(" << obj << ")" << endl;

        return Base::remove(obj->link()) ? obj : 0;
    }

    void suspend(T * obj) {
        db<Scheduler>(TRC) << "Scheduler[chosen=" << chosen() << "]::suspend(" << obj << ")" << endl;

        Base::remove(obj->link());
    }

    void resume(T * obj) {
        db<Scheduler>(TRC) << "Scheduler[chosen=" << chosen() << "]::resume(" << obj << ")" << endl;

        Base::insert(obj->link());
    }

    T * choose() {
        db<Scheduler>(TRC) << "Scheduler[chosen=" << chosen() << "]::choose() => ";

        T * obj = Base::choose()->object();

        db<Scheduler>(TRC) << obj << endl;

        return obj;
    }

    T * choose_another() {
        db<Scheduler>(TRC) << "Scheduler[chosen=" << chosen() << "]::choose_another() => ";

        T * obj = Base::choose_another()->object();

        db<Scheduler>(TRC) << obj << endl;

        return obj;
    }

    T * choose(T * obj) {
        db<Scheduler>(TRC) << "Scheduler[chosen=" << chosen() << "]::choose(" << obj;

        if(!Base::choose(obj->link()))
            obj = 0;

        db<Scheduler>(TRC) << obj << endl;

        return obj;
    }
};

__END_UTIL

#endif
