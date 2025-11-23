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
 * [NISC2025-GFX01M2-DEMO04] - ChibiOS+uGFX - Demo joystick.
 *
 */

#include "ch.h"
#include "hal.h"
#include "gfx.h"

// Definizione pin joystick
#define JOY_PORT_1      GPIOB
#define JOY_PORT_2      GPIOC

#define JOY_UP_PIN          0
#define JOY_DOWN_PIN        4
#define JOY_LEFT_PIN        6
#define JOY_RIGHT_PIN       0
#define JOY_CENTRE_PIN      7

/*
 * Inizializzazione pin joystick (ACTIVE-LOW)
 */
static void joystickInit(void) {
    palSetPadMode(JOY_PORT_2, JOY_UP_PIN, PAL_MODE_INPUT_PULLUP);            //UP
    palSetPadMode(JOY_PORT_1, JOY_DOWN_PIN, PAL_MODE_INPUT_PULLUP);          //DOWN
    palSetPadMode(JOY_PORT_1, JOY_LEFT_PIN, PAL_MODE_INPUT_PULLUP);          //LEFT
    palSetPadMode(JOY_PORT_1, JOY_RIGHT_PIN, PAL_MODE_INPUT_PULLUP);         //RIGHT
    palSetPadMode(JOY_PORT_2, JOY_CENTRE_PIN, PAL_MODE_INPUT_PULLUP);        //CENTRE
}

/*
 * Legge il joystick e restituisce la direzione premuta
 * NULL se nessun tasto premuto
 */
static const char* joystickRead(void) {
  if (!palReadPad(JOY_PORT_2, JOY_CENTRE_PIN)) {
             return "CENTRO";
         }
   if (!palReadPad(JOY_PORT_2, JOY_UP_PIN)) {
        return "SU";
    }
    if (!palReadPad(JOY_PORT_1, JOY_DOWN_PIN)) {
        return "GIU";
    }
     if (!palReadPad(JOY_PORT_1, JOY_LEFT_PIN)) {
        return "SINISTRA";
    }
    if (!palReadPad(JOY_PORT_1, JOY_RIGHT_PIN)) {
        return "DESTRA";
   }

    return NULL;
}

/*
 * Main
 */
int main(void) {
    halInit();
    chSysInit();
    gfxInit();

    gdispClear(GFX_BLACK);
    gFont font = gdispOpenFont("UI2");

    joystickInit();

    const char *lastMsg = NULL;

    while (true) {
        const char *msg = joystickRead();

        // Se cambia la direzione, aggiorna il display
        if (msg != lastMsg && msg != NULL) {
            gdispClear(GFX_BLACK);
            gdispDrawString(
                (gdispGetWidth() - gdispGetStringWidth(msg, font)) / 2,
                (gdispGetHeight() - gdispGetFontMetric(font, gFontHeight)) / 2,
                msg, font, GFX_WHITE
            );
            lastMsg = msg;
        }

        chThdSleepMilliseconds(50); // debounce e refresh
    }
}
