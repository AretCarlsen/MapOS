
#include <ATcommon/DataStore/Buffer.hpp>
#include <ATcommon/DataStore/AtomicBuffer.hpp>

/* ADC BUFFER */

typedef uint16_t ADC_t;
DataStore::AtomicBuffer<ADC_t> raw_adc_inputBuffers[ADC_CHANNEL_COUNT];
DataStore::ArrayBuffer<DataStore::AtomicBuffer<ADC_t>, uint8_t> adc_inputBuffers(raw_adc_inputBuffers, ADC_CHANNEL_COUNT);


/* ADC INTERRUPT */

// ADC interrupt
ISR(ADC_vect){
  increment_raw_systemTime();

// Readings cycle between different channels (mux sources), round-robin.
  // On the first interrupt, it is assumed that the current reading is
  // for chan 0, and that the ADC hardware has been preset to begin reading
  // chan 1 immediately.
// Don't need to init; statics always init to 0.
  static uint8_t current_adc_channel;
// MUST read ADCL first.

// Don't need to init; statics always init to 0.
  static uint8_t adc_readings;
  static uint32_t adc_accumulators[ADC_CHANNEL_COUNT];

// Gather reading into accumulator.
  // MUST read ADCL first. (For 10-bit measurements.)
  adc_accumulators[current_adc_channel] += ADCW;  //ADCL | (ADCH<<8);

// Increment adc_readings on first channel.
  if(current_adc_channel == 0)
    adc_readings++;

// If averaging count has been reached, deliver average.
  if(adc_readings == ADC_AVERAGED_READINGS){
  // Calculate average and sink.
    raw_adc_inputBuffers[current_adc_channel].sinkData(adc_accumulators[current_adc_channel] / ADC_AVERAGED_READINGS);

  // Reset sum accumulator.
    adc_accumulators[current_adc_channel] = 0;
  // On last index, reset reading counter.
    if(current_adc_channel == (ADC_CHANNEL_COUNT - 1))
      adc_readings = 0;
  }

// Increment ADC channel to the channel which is currently being read.
  current_adc_channel = (current_adc_channel + 1) % ADC_CHANNEL_COUNT;
// Set the *next* channel to be read.
  // The ADC is initially reading chan 0, and set to read chan 1 next.
  // On the first interrupt, it will read into buffer 0, then increment
  // the current_adc_channel to 1 (which is being read by the ADC hardware
  // in the meantime), then set the ADMUX value such that the *next* reading
  // will occur from channel 2 (or 0, if only two channels are being read).
  ADMUX = (current_adc_channel + 1) % ADC_CHANNEL_COUNT;
}

void init_ADC(){
// Enable ADC
  sbi(ADCSRA, ADEN);
// Free-running
  sbi(ADCSRA, ADATE);
  // Trigger source: Overflow
  ADCSRB = 0;
// Interrupts
  sbi(ADCSRA, ADIE);
// Prescale by 128 (max)
  sbi(ADCSRA, ADPS2);
  sbi(ADCSRA, ADPS1);
  sbi(ADCSRA, ADPS0);
// Right-adjusted values.
  cbi(ADMUX, ADLAR);

// PC0 needs to be an input for chan 1; PC1 for chan 2.
  cbi(DDRC, PC0);
  cbi(DDRC, PC1);

// First read is from channel 0, Aref reference.
  ADMUX = 0;
// Activate interrupts.
  sei();
// Start first conversion.
  sbi(ADCSRA, ADSC);
// Wait for one ADC clock cycle before updating ADMUX
  // Need to wait 128 CPU cycles. This loop performs 64 sleeps, plus
  // at least 64 add'l instructions in the i-decrement.
  // (Probably several times more. Hopefully less than 13*128.)
  for(uint8_t i = 0; i > 64; i--)
    nop();
// Second read is from channel 0, Aref reference.
  ADMUX = 1;
}


