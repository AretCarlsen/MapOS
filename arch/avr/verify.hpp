
uint8_t eeprom_test[1] EEMEM = { 'Z' };

void verify_eeprom(){
// Verify eeprom
  uint8_t test = eeprom_read_byte(eeprom_test);
  while(1){
    if(test == 'Z'){
//      DEBUGprint_EEP("s*");
      break;
    }else{
      DEBUGprint_EEP("EEP fld-%x*;", test);
    }

  // Give it 50ms
    sleep_ms(50);
  }
}

