/*
    NeaPolis Innovation Summer Campus 2025 Examples
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
 * [NISC2025-GPIO10] - Multithreading Example 
 * DESCRIPTION: Execution of a single thread function multiple times; 
 * main controls the threads execution.
 */

#include "ch.h"
#include "hal.h"

#define LED_LINE1 PAL_LINE( GPIOA, 5U )
#define LED_LINE2 PAL_LINE( GPIOA, 10U )

typedef struct {
  ioline_t line;
  uint32_t ton;
  uint32_t toff;
  uint32_t start;
} blinker_t;

static THD_WORKING_AREA(waBlinker1, 256);
static THD_WORKING_AREA(waBlinker2, 256);

static THD_FUNCTION(thdBlinker, arg) {

  blinker_t * blinker = (blinker_t *) arg;

  chRegSetThreadName("Blinker");

  palSetLineMode( blinker->line, PAL_MODE_OUTPUT_PUSHPULL );

  while (true) {
    palWriteLine(blinker->line, blinker->start ? PAL_HIGH : PAL_LOW );
    chThdSleep( TIME_MS2I(blinker->ton) );
    palWriteLine(blinker->line, blinker->start ? PAL_LOW : PAL_HIGH );
    chThdSleep( TIME_MS2I(blinker->toff) );
  }
}

static blinker_t blinker1 = {LED_LINE1, 500, 600, PAL_LOW };
static blinker_t blinker2 = {LED_LINE2, 500, 600, PAL_HIGH };

int main(void) {

  halInit();
  chSysInit();

  chThdCreateStatic( waBlinker1, sizeof(waBlinker1), NORMALPRIO, thdBlinker, (void *) &blinker1 );
  chThdCreateStatic( waBlinker2, sizeof(waBlinker2), NORMALPRIO+1, thdBlinker, (void *) &blinker2 );

  blinker_t * blinker = &blinker1;

  while (true) {

    chThdSleepMilliseconds( 500 );

    if( blinker->ton > 100 ) {
      blinker->ton -= 100;
    } else {
      blinker->ton = 2000;
    }

    chThdSleepMilliseconds( 500 );

    if( blinker->toff <= 2000 ) {
      blinker->toff+= 100;
    } else {
      blinker->toff = 100;
    }
  }
}
