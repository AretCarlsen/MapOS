// Copyright (C) 2010, Aret N Carlsen (aretcarlsen@autonomoustools.com).
// MapOS: Micro Addressing Protocol Operating System.
// Licensed under GPLv3 and later versions. See license.txt or <http://www.gnu.org/licenses/>.


// Common includes and variables for MapOS projects.


// DataSink/Source; Ringbuffer
#include <ATcommon/DataTransfer/DataTransfer.hpp>
#include <ATcommon/DataStore/RingBuffer.hpp>
// MEP Encoder / Decoder
#include <Upacket/MEP/MEPEncoder.hpp>
#include <Upacket/MEP/MEPDecoder.hpp>
// AddressGraph
#include <Upacket/Routing/AddressGraph.hpp>
#include <Upacket/Routing/EepromAddressGraph.hpp>
// SimpleServerProcess, EchoServer
#include <Upacket/Servers/SimpleServer.hpp>
#include <Upacket/Servers/EchoServer.hpp>
// Process (thread)
#include <MapOS/TimedScheduler/TimedScheduler.hpp>

// Addresses
#include "addresses.hpp"

#ifndef DEBUGprint_RARE
#define DEBUGprint_RARE(...)
#endif


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


/* ROUTERS */
//  MAP::MAPPacketSink* raw_packetSinks[PACKET_SINK_COUNT];
DataStore::DynamicArrayBuffer<MAP::MAPPacketSink*, uint8_t> packetSinks(&primary_memoryPool, INITIAL_PACKET_SINK_COUNT);

  // kernelSink is a Validator that sinks to kernelRouter.
AddressGraph kernelRouter(KernelRouter_AddressType, KernelRouter_AddressValue, &packetSinks, &dynRoute_memoryPool);
  // portRouter is just that.
//  AddressGraph portRouter(PortRouter_AddressType, PortRouter_AddressValue, &packetSinks);

// Static for access by StateControlServer
EepromAddressGraph kernelRouter_eepromStore(&kernelRouter, kernelRouter_eeprom_edgeCount, raw_kernelRouter_eepromStore, KERNELROUTER_EEPROM_CAPACITY);

  // Everything that enters the kernelRouter must first be validated and stripped of checksums.
    // Static for auto-class access
MAP::PacketValidator kernelSink(&kernelRouter);

// Packets incoming from unreliable transmission media must include checksums.
class IncomingChecksumValidator : public MAP::MAPPacketSink { public: Status::Status_t sinkPacket(MAP::MAPPacket *packet, uint8_t headerOffset){
  // Header must exist and include a csum.
  MAP::Data_t *header = packet->get_header(headerOffset);
  // Note that kernel sink will actually verify checksum.
  if( header != NULL && MAP::get_checksumPresent(*header))
    kernelSink.sinkPacket(packet, headerOffset);

  return Status::Status__Good;
} } incoming_checksumValidator;

/* SCHEDULER */
TimedScheduler scheduler(&scheduler_memoryPool, INITIAL_PROCESS_DEFINITION_COUNT);


