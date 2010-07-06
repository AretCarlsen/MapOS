
// UART MAP/MEP I/O

// Outgoing chain: checksum generator to MEP encoder.
MAP::OffsetMAPPacket raw_outgoing_packet_buffer[UART_OUTGOING_PACKET_BUFFER_CAPACITY];
MAP::MAPPacketBuffer process_outgoingPacketBuffer(&(uartComms.packetSink), raw_outgoing_packet_buffer, UART_OUTGOING_PACKET_BUFFER_CAPACITY);
// Static for access by inline classes
static MAP::PacketChecksumGenerator outgoingSink(&process_outgoingPacketBuffer);

// Push along any outgoing packets in the buffer.
scheduler.add_process(&process_outgoingPacketBuffer, 0);
// Outgoing sink
packetSinks.setExpand(UART_PACKET_SINK_INDEX, &outgoingSink);

// MEP-enc and -dec every 500us.
scheduler.add_process(&uartComms, 500);


