/*
 NeaPolis Innovation Summer Campus Examples
 Copyright (C) 2020-2025
 Giovanni Francesco Comune  [giovanni.francesco11c@protonmail.com]
 Alfonso Letizia            [alfonso.letizia@st.com]
 Guglielmo Marignetti       [g.marignetti@studenti.unina.it]
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
 * [NISC2025-VL53L7CX-EX03] - VL53L7CX demo 4.
 * This demo shows the VL53L7CX motion indicator capabilities.
 * To use this example, user needs to be sure that macro
 * VL53L7CX_DISABLE_MOTION_INDICATOR is NOT enabled (see file platform.h).
 */

#include "ch.h"
#include "hal.h"
#include "vl53l7cx_api.h"
#include "vl53l7cx_plugin_motion_indicator.h"
#include "chprintf.h"

static BaseSequentialStream *chp = (BaseSequentialStream*)&SD2;

static const I2CConfig i2ccfg = {STM32_TIMINGR_PRESC(8U) | /* 72MHz/9 = 8MHz I2CCLK.           */
STM32_TIMINGR_SCLDEL(3U) | STM32_TIMINGR_SDADEL(3U) | STM32_TIMINGR_SCLH(3U)
| STM32_TIMINGR_SCLL(9U),
                                 0, 0};

static const VL53L7CX_Platform vl53l7cxcfg = {.i2cp = &I2CD1, .i2ccfg = &i2ccfg,
                                              .address = 0x29};

/*********************************/
/*   VL53L7CX ranging variables  */
/*********************************/

uint8_t status, loop, isAlive, isReady, i;
uint32_t integration_time_ms;
VL53L7CX_Configuration Dev; /* Sensor configuration */
VL53L7CX_ResultsData Results; /* Results data from VL53L7CX */
VL53L7CX_Motion_Configuration motion_config; /* Motion configuration*/

/*
 * Green LED blinker thread, times are in milliseconds.
 */
static THD_WORKING_AREA(waThread1, 256);
static THD_FUNCTION(Thread1, arg) {

  (void)arg;
  chRegSetThreadName("blinker");
  while (true) {
    palClearPad(GPIOA, GPIOA_LED_GREEN);
    chThdSleepMilliseconds(500);
    palSetPad(GPIOA, GPIOA_LED_GREEN);
    chThdSleepMilliseconds(500);
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

  sdStart(&SD2, NULL);

  palSetPadMode(GPIOA, 2U, PAL_MODE_ALTERNATE(7));
  palSetPadMode(GPIOA, 3U, PAL_MODE_ALTERNATE(7));

  palSetLineMode(
      PAL_LINE(GPIOB, 8U),
      PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN | PAL_STM32_OSPEED_HIGHEST | PAL_STM32_PUPDR_PULLUP);
  palSetLineMode(
      PAL_LINE(GPIOB, 9U),
      PAL_MODE_ALTERNATE(4) | PAL_STM32_OTYPE_OPENDRAIN | PAL_STM32_OSPEED_HIGHEST | PAL_STM32_PUPDR_PULLUP);

  /*
   palSetPadMode(GPIOB, 4, PAL_MODE_OUTPUT_PUSHPULL);
   palSetPad(GPIOB, 4);

   palSetPadMode(GPIOA,4U, PAL_MODE_INPUT);
   */

  palSetPadMode(GPIOC, 0U, PAL_MODE_OUTPUT_PUSHPULL);
  palSetPadMode(GPIOA, 0U, PAL_MODE_OUTPUT_PUSHPULL);
  palSetPadMode(GPIOA, 5U, PAL_MODE_OUTPUT_PUSHPULL);
  palSetPadMode(GPIOA, 8U, PAL_MODE_OUTPUT_PUSHPULL);
  palSetPadMode(GPIOA, 9U, PAL_MODE_OUTPUT_PUSHPULL);
  palSetPadMode(GPIOB, 0U, PAL_MODE_OUTPUT_PUSHPULL);
  palSetPadMode(GPIOB, 10U, PAL_MODE_OUTPUT_PUSHPULL);
  palSetPadMode(GPIOB, 4U, PAL_MODE_OUTPUT_PUSHPULL);
  palSetPadMode(GPIOB, 5U, PAL_MODE_OUTPUT_PUSHPULL);
  palSetPadMode(GPIOB, 3U, PAL_MODE_OUTPUT_PUSHPULL);

  palSetLine(PAL_LINE(GPIOC, 0U));
  palSetLine(PAL_LINE(GPIOA, 0U));
  palClearLine(PAL_LINE(GPIOA, 5U));
  palClearLine(PAL_LINE(GPIOA, 8U));
  palClearLine(PAL_LINE(GPIOA, 9U));
  palSetLine(PAL_LINE(GPIOB, 0U));
  palSetLine(PAL_LINE(GPIOB, 10U));
  palSetLine(PAL_LINE(GPIOB, 4U));
  palSetLine(PAL_LINE(GPIOB, 5U));
  palSetLine(PAL_LINE(GPIOB, 3U));
  palClearLine(PAL_LINE(GPIOB, 3U));

  /*
   * Creates the blinker thread.
   */
  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO + 1, Thread1,
  NULL);

  Dev.platform = vl53l7cxcfg;

  /* (Optional) Check if there is a VL53L7CX sensor connected */
  VL53L7CX_WaitMs(&(Dev.platform), 100);
  status = vl53l7cx_is_alive(&Dev, &isAlive);
  if (!isAlive || status) {
    chprintf(chp, "VL53L7CX not detected at requested address\r\n");
    return status;
  }

  /* (Mandatory) Init VL53L7CX sensor */
  status = vl53l7cx_init(&Dev);
  if (status) {
    chprintf(chp, "VL53L7CX ULD Loading failed\r\n");
    return status;
  }

  chprintf(chp, "VL53L7CX ULD ready ! (Version : %s)\r\n",
  VL53L7CX_API_REVISION);

  /*********************************/
  /*   Program motion indicator    */
  /*********************************/

  /* Create motion indicator with resolution 4x4 */
  status = vl53l7cx_motion_indicator_init(&Dev, &motion_config,
  VL53L7CX_RESOLUTION_4X4);
  if (status) {
    chprintf(chp, "Motion indicator init failed with status : %u\r\n", status);
    return status;
  }

  /* (Optional) Change the min and max distance used to detect motions. The
   * difference between min and max must never be >1500mm, and minimum never be <400mm,
   * otherwise the function below returns error 127 */
  status = vl53l7cx_motion_indicator_set_distance_motion(&Dev, &motion_config,
                                                         1000, 2000);
  if (status) {
    chprintf(chp,
             "Motion indicator set distance motion failed with status : %u\r\n",
             status);
    return status;
  }

  /* If user want to change the resolution, he also needs to update the motion indicator resolution */
  //status = vl53l7cx_set_resolution(&Dev, VL53L7CX_RESOLUTION_4X4);
  //status = vl53l7cx_motion_indicator_set_resolution(&Dev, &motion_config, VL53L7CX_RESOLUTION_4X4);
  /* Increase ranging frequency for the example */
  status = vl53l7cx_set_ranging_frequency_hz(&Dev, 2);

  /*********************************/
  /*         Ranging loop          */
  /*********************************/

  status = vl53l7cx_start_ranging(&Dev);

  while (loop < 10) {
    /* Use polling function to know when a new measurement is ready.
     * Another way can be to wait for HW interrupt raised on PIN A3
     * (GPIO 1) when a new measurement is ready */

    status = vl53l7cx_check_data_ready(&Dev, &isReady);

    if (isReady) {
      vl53l7cx_get_ranging_data(&Dev, &Results);

      /* As the sensor is set in 4x4 mode by default, we have a total
       * of 16 zones to print. For this example, only the data of first zone are
       * print */
      chprintf(chp, "Print data no : %3u\r\n", Dev.streamcount);
      for (i = 0; i < 16; i++) {
        chprintf(chp, "Zone : %3d, Motion power : %3lu\r\n", i,
                 Results.motion_indicator.motion[motion_config.map_id[i]]);
      }
      chprintf(chp, "\r\n");
    }

    /* Wait a few ms to avoid too high polling (function in platform
     * file, not in API) */
    VL53L7CX_WaitMs(&(Dev.platform), 5);
  }
}
