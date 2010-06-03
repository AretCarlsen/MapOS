
/* SYSTEM TIMER */
// Unbuffered system timer.
SystemTimer_t raw_systemTimer = 0;
class Process_SystemTimer : public Process { Status::Status_t process(){
  raw_systemTimer++;
  systemTimer.sinkData(raw_systemTimer);
  return Status::Status__Good;
} } process_systemTimer;

// Run a process in its own thread.
void *processThread(void* arg){
  while(1){
// Process
    ((Process*) arg)->process();
// Sleep for 1ms
    usleep(1000);
  }
}

// Initialize threads
pthread_t thread_incoming;
pthread_t thread_outgoing;
pthread_t thread_systemTimer;
void init(){
  pthread_create(&thread_incoming, NULL, processThread, (void*) &process_incoming);
  pthread_create(&thread_outgoing, NULL, processThread, (void*) &process_outgoing);
  pthread_create(&thread_systemTimer, NULL, processThread, (void*) &process_systemTimer);
}

