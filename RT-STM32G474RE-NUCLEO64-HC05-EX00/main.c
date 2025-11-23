/*
 NeaPolis Innovation Summer Campus Examples
 Copyright (C) 2020-2025 Guglielmo Marignetti [g.marignetti@studenti.unina.it]
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
 * [NISC2025-HC05] - ChibiOS/HAL HC-05 Example.
 * DESCRIPTION: Configuration and simple echo-back example.
 */

#include "ch.h"
#include "hal.h"
#include "chprintf.h"
#include "chscanf.h"

BaseSequentialStream *chp = (BaseSequentialStream*)&SD2;
BaseSequentialStream *hc05 = (BaseSequentialStream*)&SD3;

void hc05CheckCommandOutput(BaseSequentialStream *hc05,
                            BaseSequentialStream *chp, const char *cmd) {
  uint8_t resp[64] = {0};
  chprintf(hc05, "%s\r\n", cmd);
  chprintf(chp, "[->]\r\n%s\r\n", cmd);
  sdReadTimeout(&SD3, (uint8_t* ) resp, 64, TIME_MS2I(500));
  chprintf(chp, "[<-]\r\n%s", resp);
}

#define WA_SERIAL_SIZE 256
THD_WORKING_AREA(waSerial, WA_SERIAL_SIZE);
THD_FUNCTION( thdSerial, arg ) {
  (void)arg;
  chRegSetThreadName("serial");

  /*
   * Port configuration
   */
  palSetPadMode(GPIOA, 2, PAL_MODE_ALTERNATE(7));
  palSetPadMode(GPIOA, 3, PAL_MODE_ALTERNATE(7));
  palSetPadMode(GPIOB, 8, PAL_MODE_ALTERNATE(7));
  palSetPadMode(GPIOB, 9, PAL_MODE_ALTERNATE(7));

  /* Starting Serial Driver #2 and #3*/
  sdStart(&SD2, NULL);
  sdStart(&SD3, NULL);

  hc05CheckCommandOutput(hc05, chp, "AT+VERSION?");
  hc05CheckCommandOutput(hc05, chp, "AT+NAME=HC-05");
  hc05CheckCommandOutput(hc05, chp, "AT+PSWD=1234");
  hc05CheckCommandOutput(hc05, chp, "AT+UART?");
  hc05CheckCommandOutput(hc05, chp, "AT+UART=38400,0,0");
  hc05CheckCommandOutput(hc05, chp, "AT+RESET");

  sdStop(&SD2);

  while (true) {
    char buf[16] = {0};
    chscanf((BaseBufferedStream *)hc05, "%15s", buf);
    chprintf(hc05, "Got: %s", buf);
  }
  sdStop(&SD3);
  chThdSleepMilliseconds(500);
}

/*
 * Application entry point.
 */
int main(void) {

  halInit();
  chSysInit();

  chThdCreateStatic(waSerial, sizeof(waSerial), NORMALPRIO - 1, thdSerial,
                    NULL);

  while (true) {
    palToggleLine(LINE_LED_GREEN);
    chThdSleepMilliseconds(500);
  }
}
