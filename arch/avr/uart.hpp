
#pragma once

#include <ATcommon/DataStore/RingBuffer.hpp>

// set the uart baud rate
inline void uartSetBaudRate(u32 baudrate) {
  // calculate division factor for requested baud rate, and set it
  u16 bauddiv = (F_CPU+(baudrate*8L))/(baudrate*16L)-1;

  outb(UBRR0L, bauddiv);
  #ifdef UBRR0H
  outb(UBRR0H, bauddiv>>8);
  #endif
}

// enable and initialize the uart
inline void init_UART(void) {
  // enable RxD/TxD and interrupts
  UCSR0B = BV(RXCIE0)|BV(TXCIE0)|BV(RXEN0)|BV(TXEN0);
  // set default baud rate
  uartSetBaudRate(UART_INITIAL_BAUD_RATE);
  // enable interrupts
  sei();
// DEBUG: First byte
//  outb(UDR0, 'a');
}

/* INPUT BUFFER/INTERRUPT */

uint8_t raw_uart_inputBuffer[INPUT_BUFFER_CAPACITY];
 // Data, Capacity
DataStore::RingBuffer<uint8_t,uint8_t> uart_inputBuffer(raw_uart_inputBuffer, INPUT_BUFFER_CAPACITY);

ISR(USART_RX_vect){
  uart_inputBuffer.sinkData(UDR0);
}

/* OUTPUT BUFFER/INTERRUPT */

uint8_t raw_uart_outputBuffer[OUTPUT_BUFFER_CAPACITY];
 // Data, Capacity
#ifdef OUTPUT_BUFFER_CAPACITY_T
#error (todo)
#endif
DataStore::RingBuffer<uint8_t,uint8_t> uart_outputBuffer(raw_uart_outputBuffer, OUTPUT_BUFFER_CAPACITY);

// Volatile because can be changed by an interrupt.
volatile bool uartReadyTx = true;
ISR(USART_TX_vect){
// DEBUG: Ongoing bytes
//  outb(UDR0, 'z'); return;

  uint8_t data;
  if(uart_outputBuffer.sourceData(&data) == Status::Status__Good)
    outb(UDR0, data);
  else
    uartReadyTx = true;
}
void triggerOutgoing(){
  uint8_t data;
  if(uartReadyTx && (uart_outputBuffer.sourceData(&data) == Status::Status__Good)){
    outb(UDR0, data);
    uartReadyTx = false;
  }
}

void uartSendByte(const uint8_t byte){
  uart_outputBuffer.sinkData(byte);
}

// Named to eliminate compiler warnings
class Process_TriggerOutgoing : public Process { Status::Status_t process(){
  triggerOutgoing(); return Status::Status__Good;
} } process_triggerOutgoing;


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


class UartComms {
  bool sinkData(uint8_t data){
    if(uart_outputBuffer.is_full()) return false;
    uart_outputBuffer.sinkData(data);
    return true;
  }
  bool sourceData(uint8_t &data){
    if(uart_inputBuffer.is_empty()) return false;
    uart_inputBuffer.sourceData(&data);
    return true;
  }
};


