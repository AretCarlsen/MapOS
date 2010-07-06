
#pragma once

#include "BufferedComms.hpp"

// UART input is handled via an interrupt.
// Output is also via interrupt, but only if a transmission is already in progress.
// Therefore, call triggerOutgoing() to make sure that buffered bytes are being transmitted.
template <int InputBufferCapacity, int OutputBufferCapacity>
class Process_UartComms : public BufferedComms<InputBufferCapacity,OutputBufferCapacity> {
public:
  // Volatile because can be changed by an interrupt.
  volatile bool readyTx;

  Process_UartComms(MAP::MAPPacketSink *new_packetSink, MemoryPool *new_memoryPool)
  : BufferedComms<InputBufferCapacity,OutputBufferCapacity>(new_packetSink, new_memoryPool),
    readyTx(true)
  { }

  void init(){
    // enable RxD/TxD and interrupts
    UCSR0B = BV(RXCIE0)|BV(TXCIE0)|BV(RXEN0)|BV(TXEN0);
    // set default baud rate
    setBaudRate(UART_INITIAL_BAUD_RATE);
    // enable interrupts
    sei();
    // DEBUG: First byte
    //  outb(UDR0, 'a');
  }
  void setBaudRate(uint32_t baudrate){
    // calculate division factor for requested baud rate, and set it
    u16 bauddiv = (F_CPU+(baudrate*8L))/(baudrate*16L)-1;

    outb(UBRR0L, bauddiv);
    #ifdef UBRR0H
    outb(UBRR0H, bauddiv>>8);
    #endif
  }

  inline void triggerOutgoing(){
    uint8_t data;
    if(readyTx && (BufferedComms<InputBufferCapacity,OutputBufferCapacity>::outputBuffer.sourceData(data) == Status::Status__Good)){
      outb(UDR0, data);
      readyTx = false;
    }
  }

  inline Status::Status_t process(){
  // MEP encoding/decoding
    BufferedComms<InputBufferCapacity,OutputBufferCapacity>::process_MEP();
  // Trigger outgoing UART bus, if necessary.
    triggerOutgoing(); 
    return Status::Status__Good;
  }
};

template class BufferedComms<UART_INPUT_BUFFER_CAPACITY, UART_OUTPUT_BUFFER_CAPACITY>;
template class Process_UartComms<UART_INPUT_BUFFER_CAPACITY, UART_OUTPUT_BUFFER_CAPACITY>;
Process_UartComms<UART_INPUT_BUFFER_CAPACITY, UART_OUTPUT_BUFFER_CAPACITY> uartComms(&incoming_checksumValidator, &packet_memoryPool);

ISR(USART_TX_vect){
// DEBUG: Ongoing bytes
//  outb(UDR0, 'z'); return;

  uint8_t data;
  if(uartComms.sourceData(data) == Status::Status__Good)
    outb(UDR0, data);
  else
    uartComms.readyTx = true;
}

void uartSendByte(const uint8_t byte){
  uartComms.sinkData(byte);
}

ISR(USART_RX_vect){
  uartComms.sinkData(UDR0);
}

/*
void uartSendString(const char* str){
  for(; *str != 0; str++)
    uart_outputBuffer.sinkData(*str);
  triggerOutgoing();
}

void uartSendHexNibble(uint8_t value){
// Detect bad nibble
  if(value > 0x0F)
    uart_outputBuffer.sinkData('Z');
  if(value < 10)
    uart_outputBuffer.sinkData('0' + value);
  else
    uart_outputBuffer.sinkData('A' + value - 10);

  triggerOutgoing();
}

void uartSendHexByte(uint8_t value){
// Send individual nibbles
  uartSendHexNibble((value & 0xF0) >> 4);
  uartSendHexNibble(value & 0x0F);
}

void uartSendHexUint16(uint16_t value){
  uartSendHexByte(value & 0xFF);
  uartSendHexByte((value >> 8) & 0xFF);
}
void uartSendHexUint32(uint32_t value){
  for(uint8_t i = 4; i > 0; i --){
    uartSendHexByte(value & 0xFF);
    value = value >> 8;
  }
}
*/

