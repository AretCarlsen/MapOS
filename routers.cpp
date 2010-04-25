
// Try to get this reasonably accurate, to avoid unnecessary realloc()s.
#define INITIAL_PACKET_SINK_COUNT 10

/* ROUTERS */
//  MAP::MAPPacketSink* raw_packetSinks[PACKET_SINK_COUNT];
  DataStore::DynamicArrayBuffer<MAP::MAPPacketSink*, uint8_t> packetSinks(primary_memoryPool, INITIAL_PACKET_SINK_COUNT);

  // kernelSink is a Validator that sinks to kernelRouter.
  AddressGraph kernelRouter(KernelRouter_AddressType, KernelRouter_AddressValue, &packetSinks, &dynRoute_memoryPool);
  // portRouter is just that.
//  AddressGraph portRouter(PortRouter_AddressType, PortRouter_AddressValue, &packetSinks);

// Static for access by StateControlServer
  static EepromAddressGraph kernelRouter_eepromStore(&kernelRouter, kernelRouter_eeprom_edgeCount, raw_kernelRouter_eepromStore, KERNELROUTER_EEPROM_CAPACITY);

  // Everything that enters the kernelRouter must first be validated and stripped of checksums.
    // Static for auto-class access
  static MAP::PacketValidator kernelSink(&kernelRouter);

  // Outgoing chain: checksum generator to MEP encoder.
  MEP::MEPEncoder process_outgoingEncoder(&uart_outputBuffer);
  MAP::OffsetMAPPacket raw_outgoing_packet_buffer[OUTGOING_PACKET_BUFFER_CAPACITY];
  MAP::MAPPacketBuffer process_outgoingPacketBuffer(&process_outgoingEncoder, raw_outgoing_packet_buffer, OUTGOING_PACKET_BUFFER_CAPACITY);
  // Static for access by inline classes
  static MAP::PacketChecksumGenerator outgoingSink(&process_outgoingPacketBuffer);


// Packets incoming from unreliable transmission media must include checksums.
  class : public MAP::MAPPacketSink { public: Status::Status_t sinkPacket(MAP::MAPPacket *packet, uint8_t headerOffset){
    // Header must exist and include a csum.
    MAP::Data_t *header = packet->get_header(headerOffset);
    // Note that kernel sink will actually verify checksum.
    if( header != NULL && MAP::get_checksumPresent(*header))
      kernelSink.sinkPacket(packet, headerOffset);

    return Status::Status__Good;
  } } incoming_checksumValidator;

// Incoming chain: MEP decoder to checksum validator.
// Static for access by inline classes
  static MEP::MEPDecoder incomingDecoder(&incoming_checksumValidator, &packet_memoryPool);
  class : public Process { public: Status::Status_t process(){
      uint8_t byte;
      while(uart_inputBuffer.sourceData(&byte) == Status::Status__Good){
      // "Busy" indicates a reallocation failure. We could retain the data and try to sink
      // it again, but this may be a large packet that simply cannot be accommodated at this
      // time. Instead, we drop the packet.
        if(incomingDecoder.sinkData(byte) != Status::Status__Good){
          DEBUGprint("incDec: Discard packet\n");
          incomingDecoder.discardPacket();
          break;
        }
      }
      return Status::Status__Good;
  } } process_incomingDecoder;

