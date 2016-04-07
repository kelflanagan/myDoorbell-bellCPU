/* Name: main.c
 * Author: J. Kelly Flanagan
 * Copyright: Copyright (c) 2013
 * License: No unauthorized use is permitted
 */

#include <avr/io.h>
#include <avr/eeprom.h>
#include "avr.h"
#include "util.h"
#include "vs1011.h"
#include "mem.h"

// If the doorbell button is not pressed then a
// square wave of 60 Hz is seen at the input pin.
// if the button is pressed then the input is a constant
// 1 or 3.3V. Sample each ms for a complete cycle and if a
// low input is seen during sampling the button is not
// pressed, otherwise it is.
char isFrontDoorButtonPressed()
{
  int i;
  
  for(i=0;i<17;i++) {
    if(ReadPin(PIND, FBELL_BUTTON) == 0)
      return(RELEASED);
    delay(1);
  }
  return(PRESSED);
}


char isRearDoorButtonPressed()
{
  int i;
  
  for(i=0;i<17;i++) {
    if(ReadPin(PIND, RBELL_BUTTON) == 0)
      return(RELEASED);
    delay(1);
  }
  return(PRESSED);
}


int main(void)
{
  char ret;

  AVR_init_mcu();

  ret = VS_init();
  if(ret == 0)
    blinkLED(10);
  else
    blinkLED((int)ret);

  if(MEM_init())
    blinkLED(10);

  while(true) {
    if(isFrontDoorButtonPressed()) {
      blinkLED(1);
      // create pulse for interrupt on WiFiCPU
      ClearPin(PORTD, FBELL_EVENT);
      SetPin(PORTD, FBELL_EVENT);
      // ring bell
      if(eeprom_read_byte((const uint8_t *)FBELL_VOLUME_OFFSET))
	VS_playFileMEM(FBELL);
    }

    if(isRearDoorButtonPressed()) {
      blinkLED(2);
      // create pulse for interrupt on WiFiCPU
      ClearPin(PORTD, RBELL_EVENT);
      SetPin(PORTD, RBELL_EVENT);
      // ring bell
      if(eeprom_read_byte((const uint8_t *)RBELL_VOLUME_OFFSET))
	VS_playFileMEM(RBELL);
    }

    // if the bus is requested and it hasn't been given, give it
    if(ReadPin(PIND, MEM_REQ) && !mem_bus_granted) {
      MEM_relinquishControl();
    }

    // if the request has been withdrawn, but currently given, get back
    if(!ReadPin(PIND, MEM_REQ) && mem_bus_granted) {
      MEM_acquireControl();
    }
  }
  
  return 0;   /* never reached */
}


