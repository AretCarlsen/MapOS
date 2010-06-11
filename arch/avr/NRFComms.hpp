
#include "BufferedComms.hpp"

template <typename MIRF_t, int InputBufferCapacity, int OutputBufferCapacity>
class Process_NRFComms : public BufferedComms<InputBufferCapacity, OutputBufferCapacity> {
private:
  MIRF_t mirf;

public:
  Process_NRFComms(MAP::MAPPacketSink *new_packetSink, MemoryPool *new_memoryPool, MIRF_t &new_mirf)
  : BufferedComms<InputBufferCapacity,OutputBufferCapacity>(new_packetSink, new_memoryPool),
    mirf(new_mirf)
  { }

  inline void process(){
  // MIRF polling goes here.

  // MEP encoding/decoding
    process_MEP();
  } 
};

Nordic_nRF24L01P::Nordic_nRF24L01P<mirf1_CSN_t, mirf1_CE_t, mirf1_SPI_t> mirf1;
// Usage: NRFComms nrfComms(&mirf1);

