/*
    NeaPolis Innovation Summer Campus Examples
    Copyright (C) 2020-2025 Salvatore Attanasio [salva0729@gmail.com]
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
 * [NISC2025-PWM00] - PWM Example 00
 * DESCRIPTION: Using the PWM peripheral to blink an external LED
 * connected to pin GPIOB 4
 */

#include "ch.h"
#include "hal.h"

#define PWM_TIMER_FREQUENCY     10000 //10KHz
#define PWM_PERIOD              10000 //1s

/*
 * Configures PWM Driver.
 */
static PWMConfig pwmcfg = {
  PWM_TIMER_FREQUENCY,
  PWM_PERIOD,
  NULL,
  {
   {PWM_OUTPUT_ACTIVE_HIGH, NULL},
   {PWM_OUTPUT_DISABLED, NULL},
   {PWM_OUTPUT_DISABLED, NULL},
   {PWM_OUTPUT_DISABLED, NULL}
  },
  0,
  0,
  0
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

  /* PWM Output */
  palSetPadMode(GPIOB, 4, PAL_MODE_ALTERNATE(2));

  /*
   * Starting PWM3 driver.
   */
  pwmStart(&PWMD3, &pwmcfg);
  pwmEnableChannel(&PWMD3, 0, PWM_PERCENTAGE_TO_WIDTH(&PWMD3, 2500));

  /*
   * Normal main() thread activity, in this demo it does nothing.
   */
  while (true) {
    chThdSleepMilliseconds(500);
  }

  pwmDisableChannel(&PWMD3, 0);
  pwmStop(&PWMD3);
}
