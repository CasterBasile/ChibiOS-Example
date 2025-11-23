/*
    NeaPolis Innovation Summer Campus Examples
    Copyright (C) 2020-2025 Salvatore Dello Iacono [delloiaconos@gmail.com]
    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at
        http://www.apache.org/licenses/LICENSE-2.0
    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

/*
 * [NISC2025-GPIO08] - Basic Parametric Threads.
 * DESCRIPTION: Creating multiple threads using the same thread function
 */

#include "ch.h"
#include "hal.h"

/* External Leds definitions */
#define LED_LINE1 PAL_LINE( GPIOA, 8U )
#define LED_LINE2 PAL_LINE( GPIOA, 9U )

/* Structure for thread configuration */
typedef struct {
  ioline_t line;
  uint32_t ton;
  uint32_t toff;
} blinker_t;

static THD_WORKING_AREA(waBlinker1, 256);
static THD_WORKING_AREA(waBlinker2, 256);
static THD_FUNCTION(thdBlinker, arg) {

  blinker_t * blinker = (blinker_t *) arg;

  chRegSetThreadName("Blinker");

  palSetLineMode( blinker->line, PAL_MODE_OUTPUT_PUSHPULL );

  while (true) {
    palSetLine(blinker->line);
    chThdSleep( TIME_MS2I(blinker->ton) );
    palClearLine(blinker->line);
    chThdSleep( TIME_MS2I(blinker->toff) );
  }
}

/*
 *  Definition of the threads configuration structures.
 */
static blinker_t blinker1 = {LED_LINE1, 500, 600};
static blinker_t blinker2 = {LED_LINE2, 1500, 300};

/*
 * Application entry point.
 */
int main(void) {

  /*
   * System initializations.
   * - HAL initialization, this also initializes the configured device drivers
   *   and performs the board-specific initializations.
   * - Kernel initialization, the main() function becomes a thread and the
   *   RTOS is active.
   */
  halInit();
  chSysInit();

  /*
   * Creating the blinker threads.
   */
  chThdCreateStatic( waBlinker1, sizeof(waBlinker1), NORMALPRIO, thdBlinker, (void *) &blinker1 );
  chThdCreateStatic( waBlinker2, sizeof(waBlinker2), NORMALPRIO, thdBlinker, (void *) &blinker2 );

  /*
   * Normal main() thread activity, in this demo it does nothing.
   */
  while (true) {
    chThdSleepMilliseconds( 500 );
  }
}
