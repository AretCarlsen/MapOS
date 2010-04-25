
// KernelRouter EEPROM edge storage area
#define KERNELROUTER_EEPROM_CAPACITY 15
uint8_t kernelRouter_eeprom_edgeCount[1] EEMEM = { 0 };
//AddressFilter raw_kernelRouter_eepromStore[KERNELROUTER_EEPROM_CAPACITY] EEMEM;  //  = { 0 };
uint8_t subraw_kernelRouter_eepromStore[KERNELROUTER_EEPROM_CAPACITY * sizeof(AddressFilter)] EEMEM = { 0 };
AddressFilter *raw_kernelRouter_eepromStore = (AddressFilter*) subraw_kernelRouter_eepromStore;
//AddressFilter test;

