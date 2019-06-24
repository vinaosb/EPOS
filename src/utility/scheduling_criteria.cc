// EPOS CPU Affinity Scheduler Component Implementation

#include <utility/scheduler.h>
#include <time.h>

__BEGIN_UTIL

// The following Scheduling Criteria depend on Alarm, which is not available at scheduler.h
namespace Scheduling_Criteria
{
FCFS::FCFS(int p) : Priority((p == IDLE) ? IDLE : Alarm::elapsed()){};
HRRN::HRRN(int p, int d) : Priority(p), _deadline(d), _creationTime(Alarm::_elapsed), _createdPriority(p){};
void HRRN::update()
{
    (_priority == IDLE or _priority <= MAIN) ? _priority = _priority : _priority = (int)(_createdPriority - ((_deadline + Alarm::_elapsed - _creationTime) / _deadline));
}
}; // namespace Scheduling_Criteria

__END_UTIL