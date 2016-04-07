#include <avr/io.h>
#include "spi.h"
#include "util.h"
#include "avr.h"

// init SPI bus, spifreq is in KHz
void SPI_init(int spifreq)
{  
  // Enable SPI, Master, set clock rate
  SPI_adjustSpeed(spifreq);
}


void SPI_deselect()
{
  SetPin(PORTB, MEM_SEL);
  SetPin(PORTD, VS_XCS);
  SetPin(PORTD, VS_XDCS);
}


void SPI_select(int dev)
{
  SPI_deselect();
  switch(dev) {
    case MEMCS:
      ClearPin(PORTB, MEM_SEL);
      break;
    case VSXCS:
      ClearPin(PORTD, VS_XCS);
      break;
    case VSXDCS:
      ClearPin(PORTD, VS_XDCS);
      break;
  }
}


// this routine sets the SPI bus clock freq
// CPU_FREQ is defined in avr.h 
// spifreq is in KHz
int SPI_adjustSpeed(int spifreq)
{
  int clkdiv;
  
  clkdiv = (int)((long)CPU_FREQ / ((long)spifreq*1000));
  switch(clkdiv) {
    case 2:
      SPCR = (1<<SPE) | (1<<MSTR) | (0 << SPR1) | (0 << SPR0);
      SPSR = 1;
      break;
    case 4:
      SPCR = (1<<SPE) | (1<<MSTR) | (0 << SPR1) | (0 << SPR0);
      SPSR = 0;
      break;
    case 8:
      SPCR = (1<<SPE) | (1<<MSTR) | (0 << SPR1) | (1 << SPR0);
      SPSR = 1;
      break;
    case 16:
      SPCR = (1<<SPE) | (1<<MSTR) | (0 << SPR1) | (1 << SPR0);
      SPSR = 0;
      break;
    case 32:
      SPCR = (1<<SPE) | (1<<MSTR) | (1 << SPR1) | (0 << SPR0);
      SPSR = 1;
      break;
    case 64:
      SPCR = (1<<SPE) | (1<<MSTR) | (1 << SPR1) | (0 << SPR0);
      SPSR = 0;
      break;
    case 128:
      SPCR = (1<<SPE) | (1<<MSTR) | (1 << SPR1) | (1 << SPR0);
      SPSR = 0;
      break;
    default:
      blinkLED(10);
      SPCR = (1<<SPE) | (1<<MSTR) | (0 << SPR1) | (1 << SPR0);
      SPSR = 0;
      break;
  }  
  return(clkdiv);
}


// send a byte
unsigned char SPI_send(unsigned char data)
{  
  /* Start transmission */
  SPDR = data;
  /* Wait for transmission complete */ 
  while(!(SPSR & (1<<SPIF))) {
  }
  data = SPDR;
  return(data);
}


// receive a byte
unsigned char SPI_receive(unsigned char d)
{
  unsigned char data;

  SPDR = d;
  while(!(SPSR & (1<<SPIF)));
  data = SPDR;
  
  return(data);
}
