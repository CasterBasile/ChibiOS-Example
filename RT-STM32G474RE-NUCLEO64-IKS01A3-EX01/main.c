/*
    NeaPolis Innovation Summer Campus 2025 Examples

    Copyright (C) 2020-2025 Giovanni Francesco Comune [giovanni.francesco11c@gmail.com]
                            Tammaro Cimmino [tammaro02cimmino@gmail.com]

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
 * [IKS01A3] IKS01A3 Shield Free Shield Example
 * This demo detects free fall using a IKS01A3 shield.
 */

#include "ch.h"
#include "hal.h"

#include "chprintf.h"
#include "hts221.h"
#include "lps22hb.h"
#include "lsm6dso.h"
#include "lsm303agr.h"

#define cls(chp)                            chprintf(chp, "\033[2J\033[1;1H")
#define MAX_AXIS_NUMBER                     3U

/* Generic I2C configuration for every MEMS. */
static const I2CConfig i2ccfg = {
  STM32_TIMINGR_PRESC(8U)  |            /* 72MHz/9 = 8MHz I2CCLK.           */
  STM32_TIMINGR_SCLDEL(3U) | STM32_TIMINGR_SDADEL(3U) |
  STM32_TIMINGR_SCLH(3U)   | STM32_TIMINGR_SCLL(9U),
  0,
  0
};

static BaseSequentialStream* chp = (BaseSequentialStream*)&SD2;

/*===========================================================================*/
/* HTS221 related.                                                           */
/*===========================================================================*/

/* HTS221 Driver: This object represent an HTS221 instance */
static  HTS221Driver HTS221D1;

static const HTS221Config hts221cfg = {
  .i2cp               = &I2CD1,
  .i2ccfg             = &i2ccfg,
  .hygrosensitivity   = NULL,
  .hygrobias          = NULL,
  .thermosensitivity  = NULL,
  .thermobias         = NULL,
  .outputdatarate     = HTS221_ODR_7HZ
};

/*===========================================================================*/
/* LPS22HB related.                                                           */
/*===========================================================================*/

/* LPS22HB Driver: This object represent an LPS22HB instance */
static  LPS22HBDriver LPS22HBD1;

static const LPS22HBConfig lps22hbcfg = {
  .i2cp               = &I2CD1,
  .i2ccfg             = &i2ccfg,
  .slaveaddress       = LPS22HB_SAD_VCC,
  .barosensitivity    = NULL,
  .barobias           = NULL,
  .thermosensitivity  = NULL,
  .thermobias         = NULL,
  .outputdatarate     = LPS22HB_ODR_10HZ
};

/*===========================================================================*/
/* LSM6DSO related.                                                          */
/*===========================================================================*/

/* LSM6DSO Driver: This object represent an LSM6DSO instance */
static  LSM6DSODriver LSM6DSOD1;

static const LSM6DSOConfig lsm6dsocfg = {
  .i2cp               = &I2CD1,
  .i2ccfg             = &i2ccfg,
  .slaveaddress       = LSM6DSO_SAD_VCC,
  .accsensitivity     = NULL,
  .accbias            = NULL,
  .accfullscale       = LSM6DSO_ACC_FS_2G,
  .accoutdatarate     = LSM6DSO_ACC_ODR_417Hz,
  .gyrosensitivity    = NULL,
  .gyrobias           = NULL,
  .gyrofullscale      = LSM6DSO_GYRO_FS_1000DPS,
  .gyrooutdatarate    = LSM6DSO_GYRO_ODR_417Hz,
  .freefallduration   = 0x04,
  .freefallthreshold  = LSM6DSO_FF_TSH_344mg,
  .lirvalue           = LSM6DSO_ALL_INT_LATCHED,
};

/*===========================================================================*/
/* LSM303AGR related.                                                        */
/*===========================================================================*/

/* LSM303AGR Driver: This object represent an LSM303AGR instance */
static LSM303AGRDriver LSM303AGRD1;

static const LSM303AGRConfig lsm303agrcfg = {
  .i2cp                 = &I2CD1,
  .i2ccfg               = &i2ccfg,
  .accsensitivity       = NULL,
  .accbias              = NULL,
  .accfullscale         = LSM303AGR_ACC_FS_4G,
  .accodr               = LSM303AGR_ACC_ODR_100Hz,
  .compsensitivity      = NULL,
  .compbias             = NULL,
  .compodr              = LSM303AGR_COMP_ODR_50HZ,
};

/*===========================================================================*/
/* Generic code.                                                             */
/*===========================================================================*/

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

  palSetPadMode(GPIOA, 2, PAL_MODE_ALTERNATE(7));
  palSetPadMode(GPIOA, 3, PAL_MODE_ALTERNATE(7));

  palSetLineMode(PAL_LINE(GPIOB, 9U), PAL_MODE_ALTERNATE(4) |
                    PAL_STM32_OTYPE_OPENDRAIN | PAL_STM32_OSPEED_HIGHEST |
                    PAL_STM32_PUPDR_PULLUP);
  palSetLineMode(PAL_LINE(GPIOB, 8U), PAL_MODE_ALTERNATE(4) |
                    PAL_STM32_OTYPE_OPENDRAIN | PAL_STM32_OSPEED_HIGHEST |
                    PAL_STM32_PUPDR_PULLUP);

  /*
   * Activates the serial driver 2 using the driver default configuration.
   */
  sdStart(&SD2, NULL);

  /*
   * Creates the blinker thread.
   */
  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO + 1, Thread1, NULL);

  /* MEMS Driver Objects Initialization.*/
  hts221ObjectInit(&HTS221D1);
  lps22hbObjectInit(&LPS22HBD1);
  lsm6dsoObjectInit(&LSM6DSOD1);
  lsm303agrObjectInit(&LSM303AGRD1);

  /* Activates all the MEMS related drivers.*/
  hts221Start(&HTS221D1, &hts221cfg);
  lps22hbStart(&LPS22HBD1, &lps22hbcfg);
  lsm6dsoStart(&LSM6DSOD1, &lsm6dsocfg);
  lsm303agrStart(&LSM303AGRD1, &lsm303agrcfg);

  /*
   * Normal main() thread activity, in this demo it checks for a free fall
   * signal from the iks01a3.
   */
  while (true) {
      lsm6dso_all_sources_t all_source;
      /* Check if Free Fall events */
      lsm6dsoAllSourcesGet(&LSM6DSOD1, &all_source);

      if (all_source.free_fall) {
        chprintf(chp, "free fall detected!\r\n");
      }
      chThdSleepMilliseconds(500);
    }
}
