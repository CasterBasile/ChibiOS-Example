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
 * [COLOUR SENSOR] - ChibiOS/HAL GY-31 Colour Sensor Demo using the ICU Driver.
 * DESCRIPTION: Printing on the serial the RGB values (0-255) read by the sensor module
 * after the user presses the Blue Button.
 */

#include "ch.h"
#include "hal.h"
#include "chprintf.h"

#define     LINE_GY31               PAL_LINE(GPIOA, 7)      /* TIM3_CH1 */
#define     LINE_S0                 PAL_LINE(GPIOB, 11)
#define     LINE_S1                 PAL_LINE(GPIOB, 12)
#define     LINE_S2                 PAL_LINE(GPIOA, 11)
#define     LINE_S3                 PAL_LINE(GPIOA, 12)
#define     LINE_LEDS               PAL_LINE(GPIOB, 2)
#define     COLOUR_CHANNELS         3

#define     ICU_TIM_FREQ            2000000

#define     SAMPLE_MAX              800
#define     SAMPLE_MIN              130

static BaseSequentialStream *chp = (BaseSequentialStream*)&SD2;

static uint32_t normalize(uint32_t);
/*
 * ICU Driver Config and Callback setup.
 */
static icucnt_t period;
static volatile uint8_t period_rdy = 0;
static volatile uint8_t conversion_requested = 0;

static void icuPeriodCb(ICUDriver *icup) {

  period = icuGetPeriodX(icup);
  period_rdy = 1;
  icuDisableNotificationsI(icup);
}

static ICUConfig icucfg = {ICU_INPUT_ACTIVE_HIGH,
                           ICU_TIM_FREQ, /* 2MHz ICU clock frequency. */
                           NULL,
                           icuPeriodCb,
                           NULL,
                           ICU_CHANNEL_2,
                           0U,
                           0xFFFFFFFFU
};

#define     WA_COLOUR_SENS          512
static THD_WORKING_AREA(waColourSens, WA_COLOUR_SENS);
THD_FUNCTION( thdColourSens, arg ) {

  (void)arg;
  chRegSetThreadName("ColourSens");
  static icucnt_t rgb_val[COLOUR_CHANNELS];
  static icucnt_t rgb_raw_val[COLOUR_CHANNELS];

  /* Ports mode setup */
  palSetLineMode(LINE_GY31, PAL_MODE_ALTERNATE(2)); //ICU Input
  palSetLineMode(LINE_S0, PAL_MODE_OUTPUT_PUSHPULL);
  palSetLineMode(LINE_S1, PAL_MODE_OUTPUT_PUSHPULL);
  palSetLineMode(LINE_S2, PAL_MODE_OUTPUT_PUSHPULL);
  palSetLineMode(LINE_S3, PAL_MODE_OUTPUT_PUSHPULL);
  palSetLineMode(LINE_LEDS, PAL_MODE_OUTPUT_PUSHPULL);

  palWriteLine(LINE_LEDS, PAL_LOW);

  /*
   * Output Frequency Scaling, both S1 and S2 are set LOW
   * so that the device is turned OFF
   */
  palWriteLine(LINE_S0, PAL_LOW);
  palWriteLine(LINE_S1, PAL_LOW);

  /* Starting the ICU Peripheral */
  icuStart(&ICUD3, &icucfg);

  while (true) {

    /* Checks if a Colour Conversion has ben requested*/
    if (conversion_requested) {
      chprintf(chp, "Conversion Requested\n\r");
      conversion_requested = 0;

      /*
       * Output Frequency Scaling, both S1 and S2 are set HIGH
       * so that the device is turned ON and
       * output frequency scaling is at 100%
       */
      palWriteLine(LINE_S0, PAL_HIGH);
      palWriteLine(LINE_S1, PAL_HIGH);

      for (uint8_t i = 0; i < COLOUR_CHANNELS; i++) {

        /*
         * Cycles between the setups for the three colour readings,
         * Red, Green and then Blue in this order.
         */
        switch (i) {
        case 0:
          /* PIN Setup to read RED */
          palWriteLine(LINE_S2, PAL_LOW);
          palWriteLine(LINE_S3, PAL_LOW);
          break;
        case 1:
          /* PIN Setup to read GREEN */
          palWriteLine(LINE_S2, PAL_HIGH);
          palWriteLine(LINE_S3, PAL_HIGH);
          break;
        case 2:
          /* PIN Setup to read BLUE */
          palWriteLine(LINE_S2, PAL_LOW);
          palWriteLine(LINE_S3, PAL_HIGH);
          break;
        }

        chThdSleepMilliseconds(10);

        icuStartCapture(&ICUD3);
        palWriteLine(LINE_LEDS, PAL_HIGH);
        icuEnableNotifications(&ICUD3);
        while (!period_rdy) {
          chThdSleepMilliseconds(1);
        }
        icuStopCapture(&ICUD3);
        palWriteLine(LINE_LEDS, PAL_LOW);
        period_rdy = 0;
        rgb_val[i] = normalize((uint32_t)period);
        rgb_raw_val[i] = (uint32_t)period;
      }

      chprintf(chp, "R: %u\n\rG: %u\n\rB: %u\n\r", rgb_val[0],
               rgb_val[1], rgb_val[2]);
      chprintf(chp, "rR: %u\n\rrG: %u\n\rrB: %u\n\r", rgb_raw_val[0],
               rgb_raw_val[1], rgb_raw_val[2]);
    }
    chThdSleepMilliseconds(100);
  }
}

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

  /* Serial Setup*/
  palSetPadMode(GPIOA, 2, PAL_MODE_ALTERNATE(7));
  palSetPadMode(GPIOA, 3, PAL_MODE_ALTERNATE(7));
  sdStart(&SD2, NULL);

  chThdCreateStatic(waColourSens, sizeof(waColourSens), NORMALPRIO + 1,
                    thdColourSens, NULL);
  /*
   * Normal main() thread activity, in this demo it does nothing
   */
  while (true) {

    if (palReadLine(LINE_BUTTON)) {

      conversion_requested = 1;
    }
    chThdSleepMilliseconds(50);
  }
}

/*
 * Function to normalize the sensor's reading to a 0-255 scale.
 * Note: the maximum (800) and minimum (130) have been determined experimentally
 * and they mainly depend on:
 * - The timer's clock speed
 * - The distance between the sensor and the measured object
 * - The measured object's reflective properties
 *
 * For your application, I suggest you take some measurements and check the
 * output values against an RGB colour wheel, and adjust the MIN/MAX values
 * until you have an acceptable reading.
 */
static uint32_t normalize(uint32_t sample){

  uint32_t normalized;

  if(sample > SAMPLE_MAX){

    sample = 800;
  } else if (sample < SAMPLE_MIN){

    sample = 130;
  }

  normalized = ((800 - sample) * 255 ) / 670;

  return normalized;
}
