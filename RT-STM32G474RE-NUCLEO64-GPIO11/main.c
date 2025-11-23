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
 * [NISC2025-GPIO11] - Mutex Example 
 * DESCRIPTION: Thread syncronizaion with mutex.
 */

#include "ch.h"
#include "hal.h"

#define LED_LINE PAL_LINE( GPIOA, 5U )
#define BTN_LINE PAL_LINE( GPIOC, 13U )

static uint32_t flag = 0;
static mutex_t mtxFlag;


static THD_WORKING_AREA(waBlinker, 256);
static THD_FUNCTION(thdBlinker, arg) {

  ioline_t line = *( (ioline_t *) arg );

  chRegSetThreadName("Blinker");

  while (true) {

    chMtxLock( &mtxFlag );

    if( flag ) {
      flag = 0;
      palToggleLine(line);
    }

    chMtxUnlock( &mtxFlag );

    chThdSleepMilliseconds(500);
  }
}

static ioline_t myline;

int main(void) {

  halInit();
  chSysInit();

  myline = LED_LINE;
  chMtxObjectInit( &mtxFlag );

  chThdCreateStatic( waBlinker, sizeof(waBlinker), NORMALPRIO, thdBlinker, (void *) &myline );

  palSetLineMode( BTN_LINE, PAL_MODE_INPUT );

  while (true) {

    if( palReadLine(BTN_LINE) == PAL_HIGH  ) {

      chMtxLock( &mtxFlag );
      flag = 1;
      chMtxUnlock( &mtxFlag );

      while(palReadLine(BTN_LINE) == PAL_HIGH){
        chThdSleepMilliseconds( 10 );
      }

    }

    chThdSleepMilliseconds( 20 );
  }
}
