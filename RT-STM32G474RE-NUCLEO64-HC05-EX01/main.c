/*
 NeaPolis Innovation Summer Campus Examples
 Copyright (C) 2025  Guglielmo Marignetti [g.marignetti@studenti.unina.it]
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
 * DESCRIPTION: Master-Slave configuration and simple communication example.
 */

#include "ch.h"
#include "hal.h"
#include "chprintf.h"
#include "hc05/hc05.h"

#define HC05_PIN            "1234"
#define HC05_NAME_MASTER    "HC05-Master"
#define HC05_NAME_SLAVE     "HC05-Slave"
#define HC05_BITRATE        "38400"

static char HC05SlaveAddr[15] = "0000,00,000000";

static const ATCommand HC05ConfigSlave[] = {
  {"NAME", HC05_NAME_SLAVE},
  {"PSWD", HC05_PIN},
  {"UART", HC05_BITRATE ",0,0"},
  {"ROLE", "0"},
  {"RESET", NULL},
  {NULL, NULL}
};

static ATCommand HC05ConfigMaster[] = {
  {"NAME", HC05_NAME_MASTER},
  {"PSWD", HC05_PIN},
  {"UART", HC05_BITRATE ",0,0"},
  {"ROLE", "1"},
  {"CMODE", "0"},
  {"BIND", HC05SlaveAddr},
  {"RESET", NULL},
  {NULL, NULL}
};

BaseSequentialStream *HC05Master = (BaseSequentialStream*)&SD1;
BaseSequentialStream *chp = (BaseSequentialStream*)&SD2;
BaseSequentialStream *HC05Slave = (BaseSequentialStream*)&SD3;


#define WA_SERIAL_SIZE 256
THD_WORKING_AREA(waSerial, WA_SERIAL_SIZE);
THD_FUNCTION( thdSerial, arg ) {
  (void)arg;
  chRegSetThreadName("serial");

  /* Port configuration. */
  palSetPadMode(GPIOC, 4, PAL_MODE_ALTERNATE(7));
  palSetPadMode(GPIOC, 5, PAL_MODE_ALTERNATE(7));
  palSetPadMode(GPIOA, 2, PAL_MODE_ALTERNATE(7));
  palSetPadMode(GPIOA, 3, PAL_MODE_ALTERNATE(7));
  palSetPadMode(GPIOB, 8, PAL_MODE_ALTERNATE(7));
  palSetPadMode(GPIOB, 9, PAL_MODE_ALTERNATE(7));

  /* Starting Serial Drivers #1, #2 and #3*/
  sdStart(&SD1, NULL);
  sdStart(&SD2, NULL);
  sdStart(&SD3, NULL);

  while (palReadLine(LINE_BUTTON) == PAL_LOW) {
    chThdSleepMilliseconds(50);
  }

  if (!HC05IsAlive(HC05Slave)) {
    chprintf(chp, "Cannot communicate with slave module.\r\n");
    chThdExit(1);
  }
  chprintf(chp, "Configuring slave HC05 module...\r\n");
  chThdSleepMilliseconds(1000);

  HC05GetAddress(HC05Slave, HC05SlaveAddr);
  chprintf(chp, "Got module address: %s\r\n", HC05SlaveAddr);
  chThdSleepMilliseconds(1000);

  const char *errCmd = HC05ApplyConfig(HC05Slave, HC05ConfigSlave);
  if (errCmd != NULL) {
    chprintf(chp, "Error: parameter %s not set correctly.\r\n", errCmd);
    chThdExit(1);
  }

  if (!HC05IsAlive(HC05Master)) {
    chprintf(chp, "Cannot communicate with master module.\r\n");
    chThdExit(1);
  }
  chprintf(chp, "Configuring Master HC05 module...\r\n");
  chThdSleepMilliseconds(1000);

  errCmd = HC05ApplyConfig(HC05Master, HC05ConfigMaster);
    if (errCmd != NULL) {
      chprintf(chp, "Error: parameter %s not set correctly.\r\n", errCmd);
      chThdExit(1);
    }

  chprintf(
      chp,
      "Both modules reset. Waiting 10 seconds for the modules to pair...\r\n");
  chThdSleepMilliseconds(10000);
  chprintf(chp, "Beginning module data communication...\r\n");

  while (true) {
    char buf[16] = {0};
    chprintf(HC05Master, "Test OK\r\n");
    sdReadTimeout((SerialDriver* )HC05Slave, (uint8_t* ) buf, 15,
                  TIME_MS2I(500));
    chprintf(chp, "Got: %s", buf);
  }
  sdStop(&SD1);
  sdStop(&SD2);
  sdStop(&SD3);
  chThdSleepMilliseconds(500);
}

/* Application entry point. */
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
