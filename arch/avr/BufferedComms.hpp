
#pragma once

#include <ATcommon/DataStore/RingBuffer.hpp>

// Bidirectional buffered comms.
template <int InputBufferCapacity, int OutputBufferCapacity>
class BufferedComms : public Process {
protected:
  // Incoming chain
  uint8_t raw_inputBuffer[InputBufferCapacity];
  // Data, Capacity
  DataStore::RingBuffer<uint8_t,uint8_t> inputBuffer;
  MEP::MEPDecoder incomingDecoder;

  uint8_t raw_outputBuffer[OutputBufferCapacity];
  // Data, Capacity
  DataStore::RingBuffer<uint8_t,uint8_t> outputBuffer;

public:
  // MEPEncoder is public for packetSink calls
  MEP::MEPEncoder packetSink;

  BufferedComms(MAP::MAPPacketSink *new_packetSink, MemoryPool *new_memoryPool)
  : inputBuffer(raw_inputBuffer, InputBufferCapacity),
    incomingDecoder(new_packetSink, new_memoryPool),
    outputBuffer(raw_outputBuffer, OutputBufferCapacity),
    packetSink(&outputBuffer)
  { }

  // Sink data into the input buffer.
  inline Status::Status_t sinkData(uint8_t data){
    return inputBuffer.sinkData(data);
  }
  // Source data from the output buffer.
  inline Status::Status_t sourceData(uint8_t &data){
    return outputBuffer.sourceData(data);
  }

  inline Status::Status_t process_MEP(){
  // Source data from the input buffer into the decoder.
    uint8_t byte;
    while(inputBuffer.sourceData(&byte) == Status::Status__Good){
    // "Busy" indicates a reallocation failure. We could retain the data and try to sink
    // it again, but this may be a large packet that simply cannot be accommodated at this
    // time. Instead, we drop the packet.
      if(incomingDecoder.sinkData(byte) != Status::Status__Good){
        DEBUGprint("incDec: Discard packet\n");
        incomingDecoder.discardPacket();
        break;
      }
    }
    // The preceding data xfer fully actuates the decoder.
    // incomingDecoder.process(); 

  // Run encoder on available packet data.
    packetSink.process(); 

    return Status::Status__Good;
  }
};

