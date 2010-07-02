
#include <ATcommon/arch/linux/linux.hpp>
#include <Upacket/MEP/arch/linux/MEPEncoder.hpp>
#include <Upacket/MAP/arch/linux/MAP.hpp>
#include <ATcommon/DataStore/arch/linux/RingBuffer.hpp>
// ChecksumGenerator, Validator
#include <Upacket/PosixCRC32ChecksumEngine/arch/linux/PosixCRC32ChecksumEngine.hpp>

//#include <MotorControllerServer/addresses.hpp>

void die(const char* msg){
  fputs(msg, stderr);
  exit(1);
}

#define OUTPUT_BUFFER_CAPACITY 254
uint8_t raw_outputBuffer[OUTPUT_BUFFER_CAPACITY];
DataStore::RingBuffer<uint8_t,uint8_t> outputBuffer(raw_outputBuffer, OUTPUT_BUFFER_CAPACITY);

#define PACKET_MEMORYPOOL_CAPACITY 1000
MemoryPool packet_memoryPool(PACKET_MEMORYPOOL_CAPACITY);

#include <string.h>
int main(int argc, char **argv){
// If requested, redirect stderr to dev-null.
  if(argc > 1 && !strcmp(argv[1], "-s"))
    freopen("/dev/null", "a", stderr);

// Outgoing chain: checksum generator to MEP encoder.
  MEP::MEPEncoder process_outgoingEncoder(&outputBuffer);
// Static for access by inline classes
  static MAP::PacketChecksumGenerator outgoingSink(&process_outgoingEncoder);

// Allocate packet
  MAP::MAPPacket* packet;
  MAP::allocateNewPacket(&packet, 100, &packet_memoryPool);
  MAP::referencePacket(packet);

  int32_t int_value;
  float float_value;

  // Device dest addy
  scanf("%d", &int_value);
  DEBUGprint("Device addy: %d; ", int_value);
  if(int_value > 0){
    packet->sinkData(MAP::NextProtoPresent_Mask | MAP::DestAddressPresent_Mask | MAP::AddressType__LANLocal);
  // Next-proto
    packet->sinkData(MAP::Protocol__MAP);
    packet->sinkData(int_value);
  }

  // Inner headers
    // Address type
  scanf("%d", &int_value);
  DEBUGprint("Process addy type: %d; ", int_value);
  packet->sinkData(MAP::DestAddressPresent_Mask | int_value);
    // Process dest addy
  scanf("%d", &int_value);
  DEBUGprint("Process addy value: %d.\n", int_value);
  packet->sinkData(int_value);

  while(true){
//    char ch;
    uint8_t ch;

  // Skip spaces
    do { ch = getchar(); }while(ch == ' ');

  // End
    if(ch == 'e') break;

  // Send an integer
    if(ch == 'i'){
      do { ch = getchar(); }while(ch == ' ');
      scanf("%d", &int_value);
      DEBUGprint("Input value: %d.\n", int_value);
      if(ch == 's') packet->sinkC78Signed(int_value);
      else packet->sinkC78(int_value);

  // Send a fixedPoint
    }else if(ch == 'f'){
      do { ch = getchar(); }while(ch == ' ');
      uint32_t bitOffset = 0;
      scanf("%d", &bitOffset);
      scanf("%f", &float_value);
  // Sends in FP-14
      int_value = float_value * (1 << bitOffset);
      DEBUGprint("Input value: %d.\n", int_value);
      if(ch == 's') packet->sinkC78Signed(int_value);
      else packet->sinkC78(int_value);

#define MAX_STRING_SIZE 254
  // Send a string of raw bytes
    }else if(ch == 's'){
      uint8_t mode, delim;
    // Read mode. Discard whitespace
      do { mode = getchar(); }while(mode == ' ');
    // Read delimiter. Discard ws.
      do { delim = getchar(); }while(delim == ' ');
    // Read in string. First char is delimiter
      DEBUGprint("Sinking string, delimiter '%c'.\n", delim);
      uint8_t strBuf[MAX_STRING_SIZE];
      uint8_t strLen = 0;
      for(ch = getchar(); ch != delim; ch = getchar()){
        if(strLen >= MAX_STRING_SIZE) die("String too long!");
        strBuf[strLen] = ch;
        strLen++;
      }
    // Mode: C78 vs raw
      if(mode == 'c')
        packet->sinkC78(strLen);
      for(uint8_t *str = strBuf; strLen > 0; strLen--){
        packet->sinkData(*str);
        str++;
      }
    }
  }

  // Ref; sink; process; deref
  MAP::referencePacket(packet);
  outgoingSink.sinkPacket(packet);
  process_outgoingEncoder.process();  
  MAP::dereferencePacket(packet);

//DEBUGprint("outputBuffer.get_size() = %u\n", outputBuffer.get_size());
  // Write out
  uint8_t data;
  while(outputBuffer.sourceData(&data) == Status::Status__Good)
    putc(data, stdout);

  fflush(stdout);

  return 0;
}

