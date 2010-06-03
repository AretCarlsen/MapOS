#include <ATcommon/DataStore/AtomicBuffer.hpp>

typedef uint32_t SystemTime_t;

DataStore::AtomicBuffer<SystemTimer_t> systemTimer;

inline SystemTimer_t get_systemTime(){
  return SYSTEM_TIMER_PERIOD_us*systemTimer.sourceData();
}

#include <MapOS/time.hpp>

