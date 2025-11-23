/*
    ChibiOS - Copyright (C) 2006..2025 Giovanni Di Sirio

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
 * [NISC2025-GFX01M2-DEMO03] - ChibiOS+uGFX - Demo Image.
 *
 */

#include "ch.h"
#include "hal.h"
#include "gfx.h"

static gdispImage  myImage;

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
  gfxInit();

  gdispClear(GFX_BLACK);

  gCoord swidth, sheight;
  swidth    = gdispGetWidth();
  sheight   = gdispGetHeight();

  gdispImageOpenFile(&myImage, "picto.png");
  gdispGImageDraw(GDISP, &myImage, 0, 0, swidth, sheight, 0, 0);
  gdispImageClose(&myImage);

  while (true) {
    chThdSleepMilliseconds(10);
  }
}

