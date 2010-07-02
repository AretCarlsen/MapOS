
#include <ATcommon/arch/linux/linux.hpp>

// Microseconds
#define SYSTEM_TIMER_PERIOD_us 1000
#define ADDL_PROCESS_DEFINITION_COUNT 8
#define PRIMARY_MEMORY_POOL_LIMIT 1600

#define OUTGOING_PACKET_BUFFER_CAPACITY 10
#define INPUT_BUFFER_CAPACITY 100
#define OUTPUT_BUFFER_CAPACITY 100

#include <stdlib.h>
#include <stdio.h>
#include <cstring>
// Threading
#include <pthread.h>

#define INITIAL_PACKET_SINK_COUNT 2
#define INITIAL_PROCESS_DEFINITION_COUNT 2
#include <MapOS/arch/linux/main.hpp>

// I/O
#include <MapOS/arch/linux/io.hpp>
#include <MapOS/arch/linux/threads.hpp>


int main(int argc, char **argv){
/* INIT */
  // If requested, redirect stderr to dev-null.
  if(argc > 1 && !strcmp(argv[1], "-s"))
    freopen("/dev/null", "a", stderr);

// Threads
  init();

#include <MapOS/arch/linux/main.cpp>

  // Make sure outgoing data bus is triggered occasionally.
  scheduler.add_process(&process_triggerOutgoing, 500);

// Main loop.
  DEBUGprint_RARE("Sched st\n");
  while(1){
  // Simple timed scheduler.
    // Argument is in microseconds.
    scheduler.process(get_systemTime());

  // Linux: back off a bit. (Reduces x86 CPU usage from 100% to 9%.)
    usleep(100);
  }

  return 0;
}

