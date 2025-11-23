/*
 NeaPolis Innovation Summer Campus Examples
 Copyright (C) 2025 Rocco Palermo [rocco.palermo561@gmail.com]
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
 * [NISC2025-ICU02] - ChibiOS/HAL ICU Driver Example 02.
 * DESCRIPTION: Reading the PWM signal from an external source, calculating its duty cycle and frequency.
 */

#include "ch.h"
#include "hal.h"
#include "chprintf.h"

#define     LINE_ICU              PAL_LINE(GPIOA, 7)      //ICU input to TIM3_CH2
BaseSequentialStream * chp = (BaseSequentialStream *) &SD2;

/*
 * ICU Driver Config and Callback setup.
 */
static icucnt_t t_on;
static void icuWidthCb(ICUDriver *icup) {

  t_on = icuGetWidthX(icup);
}

static icucnt_t t_period;
static void icuPeriodCb(ICUDriver *icup) {

  t_period = icuGetPeriodX(icup);
}
static ICUConfig icucfg = {ICU_INPUT_ACTIVE_HIGH,
                           10000,                 /* 10kHz ICU clock frequency. */
                           icuWidthCb,
                           icuPeriodCb,
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
  palSetPadMode(GPIOA, 2, PAL_MODE_ALTERNATE(7));
  palSetPadMode(GPIOA, 3, PAL_MODE_ALTERNATE(7));
  palSetLineMode(LINE_ICU, PAL_MODE_ALTERNATE(2));

  /*
   * Starting the ICU Peripherals
   */
  icuStart(&ICUD3, &icucfg);
  icuStartCapture(&ICUD3);
  icuEnableNotifications(&ICUD3);

  /* Starting Serial Driver #2 */
  sdStart(&SD2, NULL);

  /*
   * Normal main() thread activity, in this demo it calculates the PWM's duty cycle and frequency,
   * printing them alongside the raw period and t_on values.
   */
  float duty, frequency;
  while (true) {

    frequency = (1/((float)t_period / 10)) * 1000;
    duty = ((float)t_on/(float)t_period) * 100;

    chprintf(chp, "\033[HWidth: %d ticks\r\n", t_on);
    chprintf(chp, "Period: %d ticks\r\n", t_period);
    chprintf(chp, "Frequency: %.2f kHz\r\n", frequency);
    chprintf(chp, "Duty Cycle: %.2f %%\r\n", duty);

    chThdSleepMilliseconds(500);
  }

  icuStopCapture(&ICUD3);
  icuStop(&ICUD3);
}

