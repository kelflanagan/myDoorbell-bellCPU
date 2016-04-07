#include <avr/io.h>
#include <avr/eeprom.h>
#include "avr.h"
#include "util.h"
#include "spi.h"
#include "vs1011.h"
#include "mem.h"

//****************************
//  Public routines
//
//  MEM_init()
//  MEM_readSPI()
//  MEM_readPAR()
//  MEM_writePAR()

unsigned char mem_bus_granted;

// Initializes mem card
char MEM_init()
{
  char val;

  mem_bus_granted = 0;

  // acquire the SPI bus and other necesary signals
  val = MEM_acquireControl();
  if(val)
    return(val);
  
  // reset to known idle state
  // assert reset, wait and deassert reset
  ClearPin(PORTC, MEM_RESET);
  delay(1);
  SetPin(PORTC, MEM_RESET);
  
  // get memory status
  // select memory
  SPI_select(MEMCS);
  // read status register
  SPI_send(READ_STATUS_REGISTER);
  // get return value
  val = SPI_receive(0x00);
  SPI_deselect();

  if(((val >> 2) & 0x0f) == 0x0f) {
    if(!(val & 0x01)) {
      MEM.mem_size = 8650752;
      MEM.page_size = 1056;
    }
    else {
      blinkLED(10);
    }
    MEM.fbell_offset = MEM.page_size;
    MEM.rbell_offset = MEM.mem_size / 2;    
  }
  else
    blinkLED(10);
    
  if(val & 0x02)
    blinkLED(10);
  
  return(MEM_OK);
}


char MEM_readBlockSPI(unsigned long address, unsigned long length, unsigned char *buf)
{
  int i;
  unsigned long addr;
  
  // calculate starting address
  // for 1056 byte pages and 1024 byte pages
  if(MEM.page_size == 1056) {
    addr = (address / 1056) << 11;
    addr |= (address % 1056) & 0x7ff;
  } else {
    addr = address & 0x007fffff;
  }
  
  // wiat for memory to be ready
  while(!isMEMRdy());
  
  // fill buffer using slow continuous read command
  // select memory
  SPI_select(MEMCS);
  
  // send command
  SPI_send(CONTINUOUS_ARRAY_READ_SLOW);
  
  // send addr MSB first
  for(i=2;i>=0;i--)
    SPI_send((addr >> (8*i)) & 0xff);

  // receive data and fill buffer
  for(i=0;i<length;i++) {
    buf[i] = SPI_receive(0x00);
  }
  SPI_deselect();
  
  return(MEM_OK);
}


char isMEMRdy()
{
  return(ReadPin(PINC, MEM_RDY));
}


void actOnHeader()
{
  unsigned char buf[MEM_HEADER_SIZE];
  unsigned long v;
  int i;
  
  // read header 
  MEM_readBlockSPI(0, MEM_HEADER_SIZE, buf);

  // gather value portion
  v = 0;
  for(i=0;i<4;i++)
    v = (v << 8) | (buf[2+i] & 0xff);
  MEM_header.val = v;
  MEM_header.type = buf[0];
  MEM_header.bell = buf[1];
  
  switch(MEM_header.type) {
    case MEM_NOP:
      break;
    case MEM_FRONT_RINGTONE:
      // write length into EEPROM
      eeprom_write_dword((uint32_t *)FBELL_LENGTH_OFFSET, (uint32_t)MEM_header.val);
      break;
    case MEM_REAR_RINGTONE:
      // write length into EEPROM
      eeprom_write_dword((uint32_t *)RBELL_LENGTH_OFFSET, (uint32_t)MEM_header.val);
      break;
    case MEM_FRONT_VOLUME:
      eeprom_write_byte((uint8_t *)FBELL_VOLUME_OFFSET, (unsigned char)(MEM_header.val & 0xff));
      break;
    case MEM_REAR_VOLUME:
      eeprom_write_byte((uint8_t *)RBELL_VOLUME_OFFSET, (unsigned char)(MEM_header.val & 0xff));
      break;
    case MEM_FRONT_RING:
      VS_remotePlayMEM(FBELL, MEM_header.val);
      break;
    case MEM_REAR_RING:
      VS_remotePlayMEM(RBELL, MEM_header.val);
      break;
    case MEM_LOST_ASSOC:
      break;
  }
}


// This function results in the memory being controlled 
// by this processor via the SPI bus.  This is primarily
// for reading the memory for forwarding to the mp3 decoder
char MEM_acquireControl()
{
  long timeout;

  // deassert MEM_GRANT informing others that they
  // may not have access to the memory. Wait for requests
  // to vanish
  ClearPin(PORTB, MEM_GRANT);
  timeout = MEM_TIMEOUT;
  do {
    delay(1);
    timeout--;
  } while(ReadPin(PIND, MEM_REQ) && timeout);
  if(!timeout) {
    return(MEM_CANT_MASTER);
  }

  // drive memory CS line and deselect chip
  // writing MEM_SEL high sets the pull-up resistor
  // that removes the transition glitch.  It is reset
  // when pin set as output.
  SetPin(PORTB, MEM_SEL);
  DDRB = DDRB | (1<<MEM_SEL);
  SetPin(PORTB, MEM_SEL);
  
  // control SPI clock line
  DDRB = DDRB | (1<<BELL_SCK);
  
  // put in SPI mode
  SetPin(PORTC, MEM_SB);
  
  mem_bus_granted = 0;

  actOnHeader();
  return(MEM_OK);
}


void MEM_relinquishControl()
{
  // get everyone off the SPI bus so they don't respond
  SPI_deselect();
  
  // release memory CS line and SPI clock
  DDRB &= ~(1 << MEM_SEL);
  DDRB &= ~(1 << BELL_SCK);
  
  // put memory in parallel mode
  ClearPin(PORTC, MEM_SB);
  
  // assert MEM_GRANT informing others that they
  // may have control of the memory. Set variable so
  // we are aware as well
  mem_bus_granted = 1;
  SetPin(PORTB, MEM_GRANT);
}
