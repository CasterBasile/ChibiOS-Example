/*
    NeaPolis Innovation Summer Campus Examples
    Copyright (C) 2024-2025 Rocco Palermo [rocco.palermo561@gmail.com]
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
 * [ICU00] - ChibiOS/HAL ICU Driver Example.
 * DESCRIPTION: Long press (>2 seconds) detection.
 */

#include "ch.h"
#include "hal.h"

#define     LINE_BUTTON1                PAL_LINE(GPIOA, 7)      //Button to TIM3_CH1

/*
 * ICU Driver Config and Callback setup.
 */

static icucnt_t duration;
static void icuWidthCb(ICUDriver *icup) {
  duration = icuGetWidthX(icup);
  if(duration > 2000){

    palToggleLine(LINE_LED_GREEN);
  }
}


static ICUConfig icucfg = {
                            ICU_INPUT_ACTIVE_HIGH,
                            1000,                                    /* 1kHz ICU clock frequency.   */
                            icuWidthCb,
                            NULL,
                            NULL,
                            ICU_CHANNEL_2,
                            0U,
                            0xFFFFFFFFU
};

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
   * Ports mode setup
   */
  palSetLineMode(LINE_BUTTON1, PAL_MODE_ALTERNATE(2));

  /*
   * Starting the ICU Peripheral
   */
  icuStart(&ICUD3, &icucfg);
  icuStartCapture(&ICUD3);
  icuEnableNotifications(&ICUD3);

  /*
   * Normal main() thread activity, in this demo it does nothing
   */
  while (true) {

    chThdSleepMilliseconds(500);
  }

  icuStopCapture(&ICUD3);
  icuStop(&ICUD3);
}
