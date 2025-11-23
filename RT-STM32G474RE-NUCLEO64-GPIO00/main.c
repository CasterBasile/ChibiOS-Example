/*Onboard Green LED Toggling.
 * DESCRIPTION: Simplest project using ChibiOS-RT and HAL PAL driver on
 * STM32 Nucleo-64 Onboard Green LED.
 */

#include "ch.h"
#include "hal.h"

int main(void) {

  halInit();
  chSysInit();

  while( 1 ) {
      /*
       * Invert GPIOA LED_GREEN pin Output and wait
       */
      palTogglePad(GPIOA, GPIOA_LED_GREEN);
      chThdSleepMilliseconds(1000);
  }
}
