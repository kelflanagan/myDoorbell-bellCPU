#include <string.h>
#include <avr/io.h>
#include <util/delay.h>
#include "avr.h"
#include "util.h"

/*
  delay(int x) busy waits for x ms before returning.
  The delay is only approximate.

  inptu: integer input parameter designates the number 
  of ms to delay.

  output: none
*/
void delay(int x)
{
  int i;

  // the library routine _delay_ms() has a maximum
  // delay of just over 32 ms at 8MHz
  if(x<33) {
    for(i=0;i<x;i++)
      _delay_ms(1);
  }
  else {
    for(i=0;i<(x/32);i++) {
      _delay_ms(32);
    }
    for(i=0;i<(x%32);i++)
    _delay_ms(1);
  }
}


/*
  On netCPU there is an LED connected to pin LED
  defined in netCPU.h.  This function enables the
  caller to blink this LED num times.

  input: integer input denotes the number of times
  to blink the LED

  output: none
*/
void blinkLED(int num)
{
  int i;

  // turn LED off
  ClearPin(PORTD, LED);

  for(i=0;i<num*2;i++) {
    delay(150);  /* max is 262.14 ms / F_CPU in MHz */
    PORTD ^= (1<<LED);    /* toggle the LED */
  }
  // turn LED off
  ClearPin(PORTD, LED);
  delay(500);  /* max is 262.14 ms / F_CPU in MHz */
}


void blinkLEDSlow(int num)
{
  int i;

  // turn LED off
  ClearPin(PORTD, LED);

  for(i=0;i<num*2;i++) {
    delay(250);  /* max is 262.14 ms / F_CPU in MHz */
    PORTD ^= (1<<LED);    /* toggle the LED */
  }
  // turn LED off
  ClearPin(PORTD, LED);
  delay(500);  /* max is 262.14 ms / F_CPU in MHz */
}


