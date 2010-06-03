/* TIME */

#include <ATcommon/DataStore/AtomicBuffer.hpp>

typedef uint32_t SystemTime_t;

DataStore::AtomicBuffer<SystemTime_t> raw_systemTime;
inline SystemTime_t get_systemTime(){
  return SYSTEM_TIMER_PERIOD_us * raw_systemTime.sourceData();
}
inline void increment_raw_systemTime(){
  raw_systemTime.sinkData(raw_systemTime.sourceNewData() + 1);
}


/*
// Thread-safe time retrieval
volatile uint8_t raw_systemTime(0);
SystemTime_t get_systemTime(){
  static SystemTime_t systemTime(0);
  static uint8_t last_systemTime(0);
  // Atomic (single byte)
  uint8_t saved_rawTime = raw_systemTime;
  // Add raw time delta to true time
  systemTime += saved_rawTime - last_systemTime;
  last_systemTime = saved_rawTime;

// Return value to microsecond
  return SYSTEM_TIMER_PERIOD_us * systemTime;
}
inline void increment_raw_systemTime(){
  raw_systemTime++;
}
*/

#include <MapOS/time.hpp>

