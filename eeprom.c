#include <EEPROM.h>

// Write a long to the EEPROM at specified memory address
void EEPROM_writeLong(int start_addr, unsigned long val) {
  int i;
  
  for(i=0;i<4;i++) {
    eeprom_write_EEPROM.write(start_addr + i, (unsigned char)(val >> ((3-i)*8)) & 0xff);
  }
}


// Read long from EEPROM from specified address
unsigned long EEPROM_readLong(int start_addr) {
  int i;
  unsigned long val;
  
  val = 0;
  for(i=0;i<4;i++) {
    val |= ((unsigned long)EEPROM.read(start_addr + i)  << ((3-i)*8));
  }
  return(val);
}
