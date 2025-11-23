/*
    PLAY Embedded demos - Copyright (C) 2014-2016 Rocco Marco Guglielmi
    This file is part of PLAY Embedded demos.
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
    Tested under ChibiOS 16.1.4, Project version 1.1
    * 1.1 Change-log
    * - Fixed bug on Checksum as suggested by Dave Edwards.
 */
#include "ch.h"
#include "hal.h"
#include "chprintf.h"
/*
 * Enable if your terminal supports ANSI ESCAPE CODE
 */
#define ANSI_ESCAPE_CODE_ALLOWED                  TRUE
static BaseSequentialStream * chp = (BaseSequentialStream*) &SD2;
/*===========================================================================*/
/* DHT11 related defines                                                     */
/*===========================================================================*/
/*
 * Width are in useconds
 */
#define    MCU_REQUEST_WIDTH                     18000
#define    DHT_ERROR_WIDTH                         200
#define    DHT_START_BIT_WIDTH                      80
#define    DHT_LOW_BIT_WIDTH                        28
#define    DHT_HIGH_BIT_WIDTH                       70
/*===========================================================================*/
/* ICU related code                                                          */
/*===========================================================================*/
#define    ICU_TIM_FREQ                        1000000
static uint8_t TEMP, HR, CHECK_SUM, tmp, bit_counter = 0;;
static icucnt_t widths [40];
static void icuwidthcb(ICUDriver *icup) {
  icucnt_t width = icuGetWidthX(icup);
  if(width >= DHT_START_BIT_WIDTH){
    /* starting bit resetting the bit counter */
    bit_counter = 0;
  }
  else{
    /* Recording current width. Just for fun  */
    widths[bit_counter] = width;
    if(width > DHT_LOW_BIT_WIDTH){
      tmp |= (1 << (7 - (bit_counter % 8)));
    }
    else{
      tmp &= ~(1 << (7 - (bit_counter % 8)));
    }
    /* When bit_counter is 7, tmp contains the bit from 0 to 7 corresponding to
       The Humidity Rate integer part (Decimal part is 0 on DHT 11) */
    if(bit_counter == 7)
      HR = tmp;
    /* When bit_counter is 23, tmp contains the bit from 16 to 23 corresponding to
       The Temperature integer part (Decimal part is 0 on DHT 11) */
    if(bit_counter == 23)
      TEMP = tmp;
    /* When bit_counter is 39, tmp contains the bit from 32 to 39 corresponding to
       The Check sum value */
    if(bit_counter == 39)
      CHECK_SUM = tmp;
    bit_counter++;
  }
}

static ICUConfig icucfg = {
  ICU_INPUT_ACTIVE_HIGH,
  ICU_TIM_FREQ,                                /* 1MHz ICU clock frequency.   */
  icuwidthcb,
  NULL,
  NULL,
  ICU_CHANNEL_1,
  0U,
  0xFFFFFFFFU
};
/*===========================================================================*/
/* Generic function                                                          */
/*===========================================================================*/
/*
 * Red LED blinker thread, times are in milliseconds.
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

  /*
   * Activates the serial driver 2 using the driver default configuration.
   */
  sdStart(&SD2, NULL);
  /*
   * Creates the blinker thread.
   */
  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);
  chThdSleepMilliseconds(1000);
  /*
   * Normal main() thread activity, in this demo it does nothing except
   * sleeping in a loop and check the button state.
   */
  while (true) {
    /*
     * Making a request
     */
    palSetPadMode(GPIOA, GPIOA_PIN8, PAL_MODE_OUTPUT_PUSHPULL);
    palWritePad(GPIOA, GPIOA_PIN8, PAL_LOW);
    chThdSleepMicroseconds(MCU_REQUEST_WIDTH);
    palWritePad(GPIOA, GPIOA_PIN8, PAL_HIGH);
    /*
     * Initializes the ICU driver 1.
     * GPIOA8 is the ICU input.
     */
    palSetPadMode(GPIOA, GPIOA_PIN8, PAL_MODE_ALTERNATE(6));
    icuStart(&ICUD1, &icucfg);
    icuStartCapture(&ICUD1);
    icuEnableNotifications(&ICUD1);
    chThdSleepMilliseconds(700);
#if ANSI_ESCAPE_CODE_ALLOWED
    chprintf(chp, "\033[2J\033[1;1H");
#endif
    icuStopCapture(&ICUD1);
    icuStop(&ICUD1);
    chprintf(chp, "Temperature: %d C, Humidity Rate: %d %% \n\r", TEMP, HR);
    if(CHECK_SUM == (TEMP + HR)){
      chprintf(chp, "Checksum OK!\n\r");
    }
    else{
      chprintf(chp, "Checksum FAILED!\n\r");
    }
  }
}
