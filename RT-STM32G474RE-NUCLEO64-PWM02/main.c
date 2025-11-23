/*
    NeaPolis Innovation Summer Campus Examples
    Copyright (C) 2020-2024 Salvatore Attanasio [salva0729@gmail.com]
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
 * [NISC2023-BASE] - Basic Project Structure.
 * DESCRIPTION: This project has been adapted from the ChibiOS Demos
 */

#include "ch.h"
#include "hal.h"

#define PWM_TIMER_FREQUENCY     100000 //100KHz -> T = 0.01 ms
#define PWM_PERIOD              100 // 100Ticks@100KHz -> T = 1.0ms

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

  palSetPadMode(GPIOB, 4, PAL_MODE_ALTERNATE(2));

  pwmStart(&PWMD3, &pwmcfg);
  pwmEnablePeriodicNotification(&PWMD3);
  pwmEnableChannelNotification(&PWMD3, 0);

  uint16_t percentage = 0;
  pwmEnableChannel(&PWMD3, 0, PWM_PERCENTAGE_TO_WIDTH(&PWMD3, 0));

  while (true) {
    if(palReadLine(LINE_BUTTON)){
      percentage += 1000; // increment light of 10%
      if(percentage > 10000){
        percentage = 0;
      }
      pwmEnableChannel(&PWMD3, 0, PWM_PERCENTAGE_TO_WIDTH(&PWMD3, percentage));
    }
    chThdSleepMilliseconds(500);
  }

  pwmDisableChannel(&PWMD3, 0);

  pwmStop(&PWMD3);
}
