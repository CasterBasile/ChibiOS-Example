/*
    ChibiOS - Copyright (C) 2006..2018 Giovanni Di Sirio

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

#include "ch.h"
#include "hal.h"
#include "gfx.h"

void mandelbrot(float x1, float y1, float x2, float y2) {
    unsigned int i,j, width, height;
    gU16 iter;
    gColor color;
    float fwidth, fheight;

    float sy = y2 - y1;
    float sx = x2 - x1;
    const int MAX = 512;

    width = (unsigned int)gdispGetWidth();
    height = (unsigned int)gdispGetHeight();
    fwidth = width;
    fheight = height;

    for(i = 0; i < width; i++) {
        for(j = 0; j < height; j++) {
            float cy = j * sy / fheight + y1;
            float cx = i * sx / fwidth + x1;
            float x=0.0f, y=0.0f, xx=0.0f, yy=0.0f;
            for(iter=0; iter <= MAX && xx+yy<4.0f; iter++) {
                xx = x*x;
                yy = y*y;
                y = 2.0f*x*y + cy;
                x = xx - yy + cx;
            }
            //color = ((iter << 8) | (iter&0xFF));
            color = RGB2COLOR(iter<<7, iter<<4, iter);
            gdispDrawPixel(i, j, color);
        }
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
  gfxInit();

  gdispClear(GFX_BLACK);

  float cx, cy;
  float zoom = 1.0f;

  /* where to zoom in */
  cx = -0.086f;
  cy = 0.85f;

  while (true) {
    mandelbrot(-2.0f*zoom+cx, -1.5f*zoom+cy, 2.0f*zoom+cx, 1.5f*zoom+cy);
    zoom *= 0.7f;
    if(zoom <= 0.00001f)
        zoom = 1.0f;
  }
}

