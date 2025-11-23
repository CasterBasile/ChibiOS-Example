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
 * [NISC2024-ICU01] - ChibiOS/HAL ICU Driver Example 01.
 * DESCRIPTION: Using a rotary encoder, determining whether the encoder is rotating
 * Clockwise (CW) or Counter-Clockwise (CCW).
 */

#include "ch.h"
#include "hal.h"

#define     LINE_ENCODER_1              PAL_LINE(GPIOA, 7)      //Encoder CLK to TIM3_CH2
#define     LINE_ENCODER_2              PAL_LINE(GPIOA, 6)      //Encoder DT

/*
 * ICU Driver Config and Callback setup.
 */
static uint8_t pin01_triggered = FALSE;
static uint8_t pin02_state = FALSE;
static systime_t last_interrupt_time;

static void icuWidthCb(ICUDriver *icup) {
  (void)icup;

  if(chVTGetSystemTimeX() > last_interrupt_time + TIME_MS2I(5)){

    pin01_triggered = TRUE;
    pin02_state = palReadLine(LINE_ENCODER_2);
    last_interrupt_time = chVTGetSystemTimeX();
  }
}

static ICUConfig icucfg = {ICU_INPUT_ACTIVE_HIGH,
                           1000000, /* 1MHz ICU clock frequency.   */
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
  palSetPadMode(GPIOA, 2, PAL_MODE_ALTERNATE(7));
  palSetPadMode(GPIOA, 3, PAL_MODE_ALTERNATE(7));
  palSetLineMode(LINE_ENCODER_1, PAL_MODE_ALTERNATE(2));
  palSetLineMode(LINE_ENCODER_2, PAL_MODE_INPUT);

  /*
   * Starting the ICU Peripherals
   */
  icuStart(&ICUD3, &icucfg); // Baud rate = 38400
  icuStartCapture(&ICUD3);
  icuEnableNotifications(&ICUD3);

  /* Starting Serial Driver #2 */
  sdStart(&SD2, NULL);

  /*
   * Normal main() thread activity, in this demo it determines wether
   * the Rotary Encoder has turned clockwise or counterclockwise.
   */
  while (true) {

    if (pin01_triggered == TRUE) {

      /* We reset the trigger to make a new acquisition possible. */
      pin01_triggered = FALSE;

      if (pin02_state) {

        sdWrite(&SD2, (uint8_t*)"CW turn\n\r", 9);
      }
      else {

        sdWrite(&SD2, (uint8_t*)"CCW turn\n\r", 10);
      }
    }

    chThdSleepMilliseconds(10);
  }

  icuStopCapture(&ICUD3);
  icuStop(&ICUD3);
}
