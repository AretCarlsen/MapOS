// Copyright (C) 2010, Aret N Carlsen (aretcarlsen@autonomoustools.com).
// MapOS: Micro Addressing Protocol Operating System.
// Licensed under GPLv3 and later versions. See license.txt or <http://www.gnu.org/licenses/>.

// TimedScheduler: Minimal process scheduler.


#pragma once

// Status codes
#include <ATcommon/Status/Status.hpp>
// ArrayBuffer
#include <ATcommon/DataStore/Buffer.hpp>

typedef uint32_t SystemTimer_t;

class Process {
public:
  virtual Status::Status_t process() = 0;
};


struct ProcessDefinition {
public:
  typedef uint32_t Time_t;

  Process *process;
  Time_t period;
  Time_t last_call;
};

class TimedScheduler {
// Max capacity: 255
  // static
  DataStore::DynamicArrayBuffer<ProcessDefinition, uint8_t> processDefinitions;

  static const uint8_t CapacityIncrement = 2;
  static const uint8_t CapacityMax = 20;

public:

  TimedScheduler(MemoryPool *new_memoryPool, uint8_t initial_buffer_capacity)
  : processDefinitions(new_memoryPool, initial_buffer_capacity)
  { }

  void add_process(Process *process, ProcessDefinition::Time_t period){
    ProcessDefinition new_definition = processDefinitions.get(processDefinitions.get_size());
    new_definition.process = process;
    new_definition.period = period;
    new_definition.last_call = 0;
    processDefinitions.sinkExpand(new_definition, CapacityIncrement, CapacityMax);
  }

  Status::Status_t process(ProcessDefinition::Time_t new_time){
//DEBUGprint("p;");
  // For each definition, determine whether the process needs to be run at this time.
    for(ProcessDefinition *definition = processDefinitions.front(); definition < processDefinitions.back(); definition++){
//DEBUGprint("Considering process\n");
      if(definition->last_call + definition->period < new_time){
//DEBUGprint("Running process\n");
        definition->process->process();
        definition->last_call = new_time;
      }
    }

    return Status::Status__Good;
  }
};


