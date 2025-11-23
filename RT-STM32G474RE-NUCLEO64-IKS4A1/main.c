/*
    NeaPolis Innovation Summer Campus 2024 Examples
    Copyright (C) 2020-2024 Giovanni Francesco Comune [giovanni.francesco11c@gmail.com]
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
 * [IKS4A1] IKS4A1 Shield Example
 * A simple example with the IKS4A1 Shield. This demo retrieves data from
 * the shield and prints it to the serial terminal.
 */

#include "ch.h"
#include "hal.h"

#include "chprintf.h"
#include "lps22hb.h"
#include "lsm6dsv16x.h"
#include "lsm303agr.h"

#include "sht40ad1b.h"

#define cls(chp)                            chprintf(chp, "\033[2J\033[1;1H")
#define MAX_AXIS_NUMBER                     3U

/* Array for data storage. */
static float cooked[MAX_AXIS_NUMBER];
/* Axis identifiers. */
static char axis_id[MAX_AXIS_NUMBER] = {'X', 'Y', 'Z'};

/* Generic I2C configuration for every MEMS. */
static const I2CConfig i2ccfg = {
  STM32_TIMINGR_PRESC(8U)  |            /* 72MHz/9 = 8MHz I2CCLK.           */
  STM32_TIMINGR_SCLDEL(3U) | STM32_TIMINGR_SDADEL(3U) |
  STM32_TIMINGR_SCLH(3U)   | STM32_TIMINGR_SCLL(9U),
  0,
  0
};

static uint32_t i;
static BaseSequentialStream* chp = (BaseSequentialStream*)&SD2;

/*===========================================================================*/
/* SHT40AD1B related.                                                           */
/*===========================================================================*/

/* SHT40AD1B Driver: This object represent an SHT40AD1B instance */
static  SHT40AD1BDriver SHT40AD1BD1;

static const SHT40AD1BConfig sht40ad1bcfg = {
  .i2cp               = &I2CD1,
  .i2ccfg             = &i2ccfg,
  .hygrosensitivity   = NULL,
  .hygrobias          = NULL,
  .thermosensitivity  = NULL,
  .thermobias         = NULL,
  .outputdatarate     = SHT40AD1B_ODR_ONE_SHOT
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
/* LSM6DSV16X related.                                                          */
/*===========================================================================*/

/* LSM6DSV16X Driver: This object represent an LSM6DSV16X instance */
static  LSM6DSV16XDriver LSM6DSV16XD1;

static const LSM6DSV16XConfig lsm6dsv16xcfg = {
  .i2cp               = &I2CD1,
  .i2ccfg             = &i2ccfg,
  .slaveaddress       = LSM6DSV16X_SAD_VCC,
  .accsensitivity     = NULL,
  .accbias            = NULL,
  .accfullscale       = LSM6DSV16X_2g,
  .accoutdatarate     = LSM6DSV16X_ACC_ODR_AT_7Hz5,
  .gyrosensitivity    = NULL,
  .gyrobias           = NULL,
  .gyrofullscale      = LSM6DSV16X_1000dps,
  .gyrooutdatarate    = LSM6DSV16X_GYRO_ODR_AT_240Hz,
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
static THD_WORKING_AREA(waThread1, 128);
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
  sht40ad1bObjectInit(&SHT40AD1BD1);
  lps22hbObjectInit(&LPS22HBD1);
  lsm6dsv16xObjectInit(&LSM6DSV16XD1);
  lsm303agrObjectInit(&LSM303AGRD1);

  /* Activates all the MEMS related drivers.*/
  sht40ad1bStart(&SHT40AD1BD1, &sht40ad1bcfg);
  lps22hbStart(&LPS22HBD1, &lps22hbcfg);
  lsm6dsv16xStart(&LSM6DSV16XD1, &lsm6dsv16xcfg);
  lsm303agrStart(&LSM303AGRD1, &lsm303agrcfg);

  /*
   * Normal main() thread activity, in this demo it does nothing except
   * sleeping in a loop and check the button state.
   */
  while (true) {
    sht40ad1bHygrometerReadCooked(&SHT40AD1BD1, cooked);
    chprintf(chp, "%4s: %9.1f\t", "hum",*cooked);

    sht40ad1bThermometerReadCooked(&SHT40AD1BD1, cooked);
    chprintf(chp, "%4s: %9.1f\t", "temp",*cooked);

    lps22hbBarometerReadCooked(&LPS22HBD1, cooked);
    chprintf(chp, "%4s: %9.1f\t", "pres",*cooked);

    lps22hbThermometerReadCooked(&LPS22HBD1, cooked);
    chprintf(chp, "%4s: %9.1f\t", "temp",*cooked);

    chprintf(chp, "\r\n");

    lsm6dsv16xAccelerometerReadCooked(&LSM6DSV16XD1, cooked);
    for(i = 0; i < LSM6DSV16X_ACC_NUMBER_OF_AXES; i++) {
      chprintf(chp, "%3s%c: %9.1f\t", "a",axis_id[i], cooked[i]);
    }

    chprintf(chp, "\r\n");

    lsm6dsv16xGyroscopeReadCooked(&LSM6DSV16XD1, cooked);
    for(i = 0; i < LSM6DSV16X_GYRO_NUMBER_OF_AXES; i++) {
      chprintf(chp, "%3s%c: %9.1f\t", "w",axis_id[i], cooked[i]);
    }

    chprintf(chp, "\r\n");

    lsm303agrCompassReadCooked(&LSM303AGRD1, cooked);
    for(i = 0; i < LSM303AGR_COMP_NUMBER_OF_AXES; i++) {
      chprintf(chp, "%3s%c: %9.4f\t","m", axis_id[i], cooked[i]);
    }

    chprintf(chp, "\r\n\r\n");

    chThdSleepMilliseconds(500);
  }
}
