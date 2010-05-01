
// DataSink/Source; Ringbuffer
#include "../DataTransfer/DataTransfer.hpp"
#include "../DataStore/RingBuffer.hpp"
// MEP Encoder / Decoder
#include "../Packet/MEP/MEPEncoder.hpp"
#include "../Packet/MEP/MEPDecoder.hpp"
// AddressGraph
#include "../Packet/Routing/AddressGraph.hpp"
#include "../Packet/Routing/EepromAddressGraph.hpp"
// SimpleServerProcess, EchoServer
#include "../Packet/Servers/SimpleServer.hpp"
#include "../Packet/Servers/EchoServer.hpp"
// Process (thread)
#include "../TimedScheduler/TimedScheduler.hpp"

// Addresses
#include "addresses.hpp"


/* MEMORY POOLS */
MemoryPool primary_memoryPool(PRIMARY_MEMORY_POOL_LIMIT);
MemoryPool& packet_memoryPool(primary_memoryPool);
//MemoryPool& dynRoute_memoryPool = packet_memoryPool;
MemoryPool& dynRoute_memoryPool(packet_memoryPool);
MemoryPool& packetSink_memoryPool(packet_memoryPool);
MemoryPool& scheduler_memoryPool(packet_memoryPool);

void verify_memPool(){
// Verify memory pool
  uint16_t memSize;
  DEBUGprint_RARE("PackPool: st\n");
  for(memSize = 1; memSize < 3000; memSize++){
    void* test = packet_memoryPool.malloc(memSize);
    if(test == NULL){
      memSize--;
      break;
    }
    packet_memoryPool.free(test, memSize);
  }
  DEBUGprint_RARE("PackPool: %d\n", memSize);
}


/* ROUTERS */

// KernelRouter EEPROM edge storage area
#define KERNELROUTER_EEPROM_CAPACITY 15
uint8_t kernelRouter_eeprom_edgeCount[1] EEMEM = { 0 };
//AddressFilter raw_kernelRouter_eepromStore[KERNELROUTER_EEPROM_CAPACITY] EEMEM;  //  = { 0 };
uint8_t subraw_kernelRouter_eepromStore[KERNELROUTER_EEPROM_CAPACITY * sizeof(AddressFilter)] EEMEM = { 0 };
AddressFilter *raw_kernelRouter_eepromStore = (AddressFilter*) subraw_kernelRouter_eepromStore;
//AddressFilter test;

/* TIME: system-specific */
#include "time.hpp"

