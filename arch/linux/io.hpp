#include <ATcommon/DataTransfer/arch/linux/File.hpp>

/* INPUT BUFFER/THREAD */

uint8_t raw_uart_inputBuffer[INPUT_BUFFER_CAPACITY];
DataStore::RingBuffer<uint8_t,uint8_t> uart_inputBuffer(raw_uart_inputBuffer, INPUT_BUFFER_CAPACITY);

// Incoming data thread
class Process_Incoming : public Process { Status::Status_t process(){
  DEBUGprint_RARE("Process_Incoming: Waiting for data on stdin...\n");

  while(true){
    while(! feof(stdin)){
// Block, waiting for input on stdin.
  // Note that ringbuffer may overflow, causing silent data loss.
      uint8_t ch = getc(stdin);
//      DEBUGprint("Sd X%x", ch); 
      uart_inputBuffer.sinkData(ch);
    }

    usleep(30000);
  }

// Should never reach this point.
  return Status::Status__Good;
} } process_incoming;


/* OUTPUT BUFFER/THREAD */

uint8_t raw_uart_outputBuffer[OUTPUT_BUFFER_CAPACITY];
DataStore::RingBuffer<uint8_t,uint8_t> uart_outputBuffer(raw_uart_outputBuffer, OUTPUT_BUFFER_CAPACITY);

pthread_mutex_t outgoingTrigger_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  outgoingTrigger_condition  = PTHREAD_COND_INITIALIZER;

// Outgoing data thread
class Process_Outgoing : public Process { Status::Status_t process(){
  DEBUGprint_RARE("Process_Outgoing: Waiting for data outgoing on stdout...\n");
// Continuous loop
  while(1){
// Block, waiting for pending outgoing data
    pthread_mutex_lock( &outgoingTrigger_mutex );
    pthread_cond_wait( &outgoingTrigger_condition, &outgoingTrigger_mutex);
    pthread_mutex_unlock( &outgoingTrigger_mutex );

// Send all pending outgoing data
    uint8_t data;
    while(uart_outputBuffer.sourceData(&data) == Status::Status__Good){
// Block, waiting for output on stdout.
      putc(data, stdout);
    }
    fflush(stdout);
  }

// Should never reach this point.
  return Status::Status__Good;

} } process_outgoing;

// Trigger outgoing sends, if not already in progress.
void triggerOutgoing(){
// If the buffer is not currently empty, send a signal.
// Note that race conditions are not a problem here,
// as the output thread verifies that the buffer is not empty.
  if(! uart_outputBuffer.is_empty()){
    pthread_mutex_lock( &outgoingTrigger_mutex );
    pthread_cond_signal( &outgoingTrigger_condition );
    pthread_mutex_unlock( &outgoingTrigger_mutex );
  }
}

// Named to eliminate compiler warnings
class Process_TriggerOutgoing : public Process { Status::Status_t process(){
  triggerOutgoing(); return Status::Status__Good;
} } process_triggerOutgoing;

class FileComms{
  FILE* device;

public:
// Have to have a no-argument constructor for templated drivers.
  FileComms()
  { assert(false); }

  FileComms(FILE* new_device)
  : device(new_device)
  { }

  // Returns true if data was successfully sent.
  bool sinkData(uint8_t data){
    fputc(data, device);
    return true;
  }
  // Returns true if data was successfully received.
  bool sourceData(uint8_t &data){
    data = fgetc(device);
    return true;
  }
};

