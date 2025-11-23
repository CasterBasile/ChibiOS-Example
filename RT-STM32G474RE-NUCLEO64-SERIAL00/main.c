#include "ch.h"
#include "hal.h"

#define WA_SERIAL_SIZE 256
THD_WORKING_AREA( waSerial, WA_SERIAL_SIZE);
THD_FUNCTION( thdSerial, arg ) {
  (void) arg;
  chRegSetThreadName( "serial" );

  /*
   * Port configuration
   */
  palSetPadMode( GPIOA, 2, PAL_MODE_ALTERNATE(7) );
  palSetPadMode( GPIOA, 3, PAL_MODE_ALTERNATE(7) );

  /* Starting Serial Driver #2 */
  sdStart( &SD2, NULL );

  while (true) {
    char c = sdGet( &SD2 );
    sdPut( &SD2, c );
  }
}

/*
 * Application entry point.
 */
int main(void) {

  halInit();
  chSysInit();

  chThdCreateStatic( waSerial, sizeof(waSerial), NORMALPRIO-1, thdSerial, NULL);

  while (true) {
    palToggleLine( LINE_LED_GREEN );
    chThdSleepMilliseconds(500);
  }
}
