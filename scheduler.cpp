
#define PROCESS_DEFINITION_COUNT (3 + ADDL_PROCESS_DEFINITION_COUNT)
ProcessDefinition raw_processDefinitions[PROCESS_DEFINITION_COUNT];
TimedScheduler scheduler(raw_processDefinitions, PROCESS_DEFINITION_COUNT);

// Process, Delay (us)
// Process all pending incoming data (contained in the input ringbuffer).
  // MEP decode, then verify the MAP checksum.
scheduler.add_process(&process_incomingDecoder, 0);
// Push along any outgoing packets in the buffer.
scheduler.add_process(&process_outgoingPacketBuffer, 0);
// Process all pending outgoing packets/data.
scheduler.add_process(&process_outgoingEncoder, 0);

