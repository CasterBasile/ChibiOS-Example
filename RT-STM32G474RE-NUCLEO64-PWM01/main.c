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
 * [NISC20245-PWM01] - PWM Example 01
 * DESCRIPTION: Using the PWM peripheral to blink the internal LED using callbacks.
 */

#include "ch.h"
#include "hal.h"

#define PWM_TIMER_FREQUENCY     10000 //10KHz
#define PWM_PERIOD              10000 //1s

void pwmPeriodCb(PWMDriver *pwmp){
  (void)pwmp;
  palToggleLine(LINE_LED_GREEN);
}

void pwmChannelCb(PWMDriver *pwmp){
  (void)pwmp;
}

/*
 * Configures PWM Driver.
 */
static PWMConfig pwmcfg = {
  PWM_TIMER_FREQUENCY,
  PWM_PERIOD,
  pwmPeriodCb,
  {
   {PWM_OUTPUT_ACTIVE_HIGH, pwmChannelCb},
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

  /*
   * Starting PWM3 driver.
   */
  pwmStart(&PWMD3, &pwmcfg);
  pwmEnableChannel(&PWMD3, 0, PWM_PERCENTAGE_TO_WIDTH(&PWMD3, 2500));

  /*
   * Activating callbacks.
   */
  pwmEnablePeriodicNotification(&PWMD3);
  pwmEnableChannelNotification(&PWMD3, 0);

  /*
   * Normal main() thread activity, in this demo it does nothing.
   */
  while (true) {
    chThdSleepMilliseconds(500);
  }

  pwmDisableChannel(&PWMD3, 0);
  pwmStop(&PWMD3);
}
