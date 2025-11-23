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

  sdWrite( &SD2, (uint8_t *)"Testing sdRead and sdWrite...\n\r", 31 );
  while (true) {
    uint8_t buf[5];
    sdReadTimeout( &SD2, buf, 5, TIME_S2I(5) );
    sdWrite( &SD2, buf, 5 );
    sdWrite( &SD2, (uint8_t *)"\n\r", 2 );
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
