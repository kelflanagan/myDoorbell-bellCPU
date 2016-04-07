#include <avr/io.h>
#include "avr.h"
#include "util.h"
#include "spi.h"

void AVR_init_mcu()
{
  // initialize port directions
  // port B
  DDRB = (1<<MEM_SEL) | (1<<MEM_GRANT) | (1<<BELL_MOSI) | (1<<BELL_SCK) | (1<<SS);
  // port C
  DDRC = (1<<MEM_SB) | (1<<MUTE) | (1<<VS_RESET) | (1<<MEM_RESET);
  // port D
  DDRD = (1<<VS_XDCS) | (1<<VS_XCS) | (1<<LED) | (1<<FBELL_EVENT) | (1<<RBELL_EVENT);

  // events are low asserted, deassert them
  SetPin(PORTD, FBELL_EVENT);
  SetPin(PORTD, RBELL_EVENT);

  // initialize outputs
  ClearPin(PORTD, LED);
  ClearPin(PORTB, MEM_GRANT);
  SetPin(PORTB, MEM_SEL);

  // initialize SPI
  SPI_init(2000);
}

