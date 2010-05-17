#pragma once

inline void sleep_us(SystemTime_t delay){
/*
  SystemTime_t lastTime = get_systemTime();
  DEBUGprint("Dly X%x; init T X%x;", delay, lastTime);
*/
  for(SystemTime_t endTime = get_systemTime() + delay; get_systemTime() < endTime; );
/* {
    SystemTime_t tmpTime = get_systemTime();
    if(tmpTime > lastTime + 1000){
      DEBUGprint_FORCE("%d;", tmpTime);
      lastTime = tmpTime;
    }
  }
  DEBUGprint("Dly cmplt;");
*/
}
inline void sleep_ms(uint32_t delay){
  sleep_us(1000 * delay);
}

// Snap a process period to the nearest SystemTime value.
SystemTime_t snapTo(SystemTime_t value, const SystemTime_t period){
  if(value % period != 0)
    value += period - (value % period);

  return value;
}

