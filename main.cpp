// Copyright (C) 2010, Aret N Carlsen (aretcarlsen@autonomoustools.com).
// MapOS: Micro Addressing Protocol Operating System.
// Licensed under GPLv3 and later versions. See license.txt or <http://www.gnu.org/licenses/>.

// MapOS kernel routers.


/* ROUTERS */
//  MAP::MAPPacketSink* raw_packetSinks[PACKET_SINK_COUNT];
DataStore::DynamicArrayBuffer<MAP::MAPPacketSink*, uint8_t> packetSinks(&primary_memoryPool, INITIAL_PACKET_SINK_COUNT);

  // kernelSink is a Validator that sinks to kernelRouter.
AddressGraph kernelRouter(KernelRouter_AddressType, KernelRouter_AddressValue, &packetSinks, &dynRoute_memoryPool);
  // portRouter is just that.
//  AddressGraph portRouter(PortRouter_AddressType, PortRouter_AddressValue, &packetSinks);

// Static for access by StateControlServer
static EepromAddressGraph kernelRouter_eepromStore(&kernelRouter, kernelRouter_eeprom_edgeCount, raw_kernelRouter_eepromStore, KERNELROUTER_EEPROM_CAPACITY);

  // Everything that enters the kernelRouter must first be validated and stripped of checksums.
    // Static for auto-class access
static MAP::PacketValidator kernelSink(&kernelRouter);

// Packets incoming from unreliable transmission media must include checksums.
class : public MAP::MAPPacketSink { public: Status::Status_t sinkPacket(MAP::MAPPacket *packet, uint8_t headerOffset){
  // Header must exist and include a csum.
  MAP::Data_t *header = packet->get_header(headerOffset);
  // Note that kernel sink will actually verify checksum.
  if( header != NULL && MAP::get_checksumPresent(*header))
    kernelSink.sinkPacket(packet, headerOffset);

  return Status::Status__Good;
} } incoming_checksumValidator;

/* SCHEDULER */
TimedScheduler scheduler(&scheduler_memoryPool, INITIAL_PROCESS_DEFINITION_COUNT);

/* ROUTING GRAPH TARGETS */
// Kernel router
packetSinks.setExpand(0, &kernelSink);

// Port router
packetSinks.setExpand(1, &kernelSink);

