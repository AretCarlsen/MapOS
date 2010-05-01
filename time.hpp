#pragma once

#include "../DataStore/AtomicBuffer.hpp"

void sleep_us(uint32_t delay){
  for(uint32_t endTime = get_systemTime() + delay; get_systemTime() < endTime; );
}
inline void sleep_ms(uint32_t delay){
  sleep_us(1000 * delay);
}

// Snap a process period to the nearest SystemTimer value.
SystemTimer_t snapTo(SystemTimer_t value, SystemTimer_t period){
  if(value % period != 0)
    value += period - (value % period);

  return value;
}

