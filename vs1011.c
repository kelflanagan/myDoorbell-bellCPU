#include <avr/io.h>
#include <avr/eeprom.h>
#include "avr.h"
#include "mem.h"
#include "util.h"
#include "vs1011.h"
#include "spi.h"

char VS_init()
{
  unsigned int modeReg, statusReg, ret=0;
    
  // perform a hardware reset
  ClearPin(PORTC, VS_RESET);
  delay(1);
  SetPin(PORTC, VS_RESET);
  // wait for DREQ to go HIGH
  if(!VS_isReady())
    return(0);
      
  // deselect all SPI slaves to get them off the SPI bus
  SPI_deselect();
  
  // adjust SPI bus down to 2MHz to enable slow speed communication
  // until VS1011e setup for 4MHz communication
  SPI_adjustSpeed(2000);

  if((statusReg = VS_readRegister(STATUS_REG)) == 0xffff) {
    return(0);
  }

  if((modeReg = VS_readRegister(MODE_REG)) == 0xffff) {
    return(0);
  }

  switch((statusReg >> 4) & 0xf) {
  case 0:  // found vs001
    ret = 1;
    break;
  case 1:  // found vs1011
    ret = 4;
    break;
  case 2:  
    if(modeReg & (1 << SM_SDINEW))
      ret = 2;  // found vs1001
    else
      ret = 5;  // found vs1011e
    break;
  case 3:  // found vs1003
    ret = 3;
    break;
  }
  
  // set desired mode values
  modeReg = (1 << SM_TESTS) | (1 << SM_SDINEW);
  if(!VS_writeRegister(MODE_REG, modeReg)) {
    return(0);
  }
  
  // enable 2x clock
  if(!VS_writeRegister(CLOCKF_REG, 0x9800)) {
    return(0);
  }

  // set sample rate so clock doubling will immediately be enabled
  // nothing magic about the sample rate, just write anything
  if(!VS_writeRegister(AUDATA_REG, 0x1f40)) {
    return(0);
  }

  // wait more than 11000 clock cycles
  delay(10);
  
  // Set SPI bus to 4 MHz or 4000 KHz
  SPI_adjustSpeed(2000);
  
  // set volume
  if(!VS_writeRegister(VOL_REG, 0x0000)) {
    return(0);
  }
    
  return(ret);
}


void VS_remotePlayMEM(int bell, unsigned long vol)
{
  unsigned char cur_vol;
  
  if(vol > 99)
    vol = 99;
    
  if(bell == FBELL) {
    cur_vol = eeprom_read_byte((const uint8_t *)FBELL_VOLUME_OFFSET);
    eeprom_write_byte((uint8_t *)FBELL_VOLUME_OFFSET, (unsigned char)vol);
    VS_playFileMEM(bell);
    eeprom_write_byte((uint8_t *)FBELL_VOLUME_OFFSET, (unsigned char)cur_vol);
  }
  else {
    cur_vol = eeprom_read_byte((const uint8_t *)RBELL_VOLUME_OFFSET);
    eeprom_write_byte((uint8_t *)RBELL_VOLUME_OFFSET, (unsigned char)vol);
    VS_playFileMEM(bell);
    eeprom_write_byte((uint8_t *)RBELL_VOLUME_OFFSET, (unsigned char)cur_vol);
  }  
}


void VS_playFileMEM(int bell)
{
  int j;
  unsigned long i;
  unsigned char buf[256];
  unsigned long len=0, offset=0;
  unsigned int vol=0;

  // set volume for playback
  if(bell == FBELL) {
    vol = eeprom_read_byte((const uint8_t *)FBELL_VOLUME_OFFSET); //vol=0 to 99, where 99 is loudest
    len = eeprom_read_dword((const uint32_t *)FBELL_LENGTH_OFFSET);
    offset = MEM.fbell_offset;
  }
  if(bell == RBELL) {
    vol = eeprom_read_byte((const uint8_t *)RBELL_VOLUME_OFFSET); //vol=0 to 99, where 99 is loudest
    len = eeprom_read_dword((const uint32_t *)RBELL_LENGTH_OFFSET);
    offset = MEM.rbell_offset;
  }
  vol = (99 - vol) * 1.01;              // invert and adjust to range 0 to 0xfe
  vol = (vol << 8) | vol;
  // set volume
  if(!VS_writeRegister(VOL_REG, vol))
    blinkLED(10);
  
  SetPin(PORTC, MUTE);
  // read all complete sectors and send them 32 bytes at a time
  for(i=0;i<len/256;i++) {
    if(MEM_readBlockSPI((i*256)+offset, 256, buf))
      blinkLED(10);
    for(j=0;j<8;j++) {
      VS_writeData((unsigned char *)&buf[j*32]);
    }
  }
  
  // deal with the last incomplete, less than 256 byte buffer
  if(MEM_readBlockSPI((i*256)+offset, 256, buf)) {
    blinkLED(10);
  }
  for(j=0;j<((len % 256) / 32);j++)
    VS_writeData((unsigned char *)&buf[j*32]);
  // pad the last buffer with zeros
  for(j=((len % 256) % 32);j<32;j++)
    buf[7*32+j] = 0;
  // send the last bit
  VS_writeData((unsigned char *)&buf[7*32]);
  
  // silence amplifier
  ClearPin(PORTC, MUTE);
}


char VS_writeData(unsigned char *buf)
{
  int i;
  // wait for device to be ready
  if(!VS_isReady()) {
    return(0);
  }
  
  // select data unit
  SPI_select(VSXDCS);
  
  for(i=0;i<32;i++) {
    SPI_send(buf[i]);
  }
    
  if(!VS_isReady()) {
    blinkLED(10);
  }
  
  SPI_deselect();
  return(1);
}


unsigned int VS_readRegister(unsigned char reg)
{
  unsigned int ret;
  
  // wait for device to be ready
  if(!VS_isReady()) {
    blinkLED(10);
    return(0xffff);
  }
  
  // select command unit
  SPI_select(VSXCS);
  
  // issue read command
  SPI_send(SCI_READ);
  // of register, reg
  SPI_send(reg);
  
  // acquire register content
  ret = (SPI_receive(0xff) << 8) | (SPI_receive(0xff));
  
  SPI_deselect();
  return(ret);
}


char VS_writeRegister(unsigned char reg, unsigned int data)
{
  // wait for device to be ready
  if(!VS_isReady()) {
    blinkLED(10);
    return(0);
  }
  
  // select command unit
  SPI_select(VSXCS);
  
  // issue read command
  SPI_send(SCI_WRITE);
  // of register, reg
  SPI_send(reg);
  // send data
  SPI_send((data >> 8) & 0xff);
  SPI_send(data & 0xff);
  
  SPI_deselect();
  return(1);
}


void VS_sineTest(unsigned char index, int duration)
{
  unsigned int modeReg, vol;
  
  ClearPin(PORTC, VS_RESET);
  delay(500);
  SetPin(PORTC, VS_RESET);
  delay(500);
  // set desired mode values
  modeReg = (1 << SM_TESTS) | (1 << SM_SDINEW);
  VS_writeRegister(MODE_REG, modeReg);
  // set volume
  vol = 75;
  vol = (99 - vol) * 2.57;              // invert and adjust to range 0 to 0xfe
  vol = (vol << 8) | vol;
  // set volume
  VS_writeRegister(VOL_REG, vol);
  
  SPI_deselect();
  VS_isReady();
  index &= 0x1f;
  index |= 0x60;

  blinkLED(1);

  SPI_select(VSXDCS);
  
  SPI_send(0x53);
  SPI_send(0xef);
  SPI_send(0x6e);
  SPI_send(index & 0x1f);
  SPI_send(0);
  SPI_send(0);
  SPI_send(0);
  SPI_send(0);
  
  SetPin(PORTC, MUTE);
  delay(1000 * duration);
  ClearPin(PORTC, MUTE);
  
  SPI_send(0x45);
  SPI_send(0x78);
  SPI_send(0x69);
  SPI_send(0x74);
  SPI_send(0);
  SPI_send(0);
  SPI_send(0);
  SPI_send(0);
  
  SPI_deselect();
}


char VS_isReady()
{
  unsigned long attempts;

  attempts = 30000;
  while((ReadPin(PINC, VS_DREQ) == 0) && (attempts)) {
    attempts--;
  }

  if(!attempts)
    return(false);
  else
    return(true); 
}

