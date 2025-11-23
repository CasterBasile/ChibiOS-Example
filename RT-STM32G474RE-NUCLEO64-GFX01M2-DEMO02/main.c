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
 * [NISC2025-GFX01M2-DEMO02] - ChibiOS+uGFX - Demo analog+digital clock and banner.
 *
 */

#include "gfx.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

#define CLOCK_RADIUS   90
#define CENTER_X       (gdispGetWidth() / 2)
#define CENTER_Y       (gdispGetHeight() / 2)

#define DIGITAL_RECT_W 160
#define DIGITAL_RECT_H 50
#define DIGITAL_RECT_X ((gdispGetWidth() - DIGITAL_RECT_W) / 2)
#define DIGITAL_RECT_Y (gdispGetHeight() - DIGITAL_RECT_H - 10)

int slider_pos;

/* Funzione di utilità: converte RTCDateTime in ore/minuti/secondi */
static void rtcGetHMS(const RTCDateTime *timespec, int *hour, int *min, int *sec) {
    uint32_t sec_since_epoch = rtcConvertDateTimeToFAT(timespec);
    *sec  = sec_since_epoch % 60;
    *min  = (sec_since_epoch / 60) % 60;
    *hour = (sec_since_epoch / 3600) % 24;
}

static void draw_line(float angle_deg, int length, color_t color) {
    float angle = (angle_deg - 90.0f) * (M_PI / 180.0f);
    int x = CENTER_X + (int)(cosf(angle) * length);
    int y = CENTER_Y + (int)(sinf(angle) * length);

    // Lancetta doppia
    gdispDrawLine(CENTER_X, CENTER_Y, x, y, color);
    gdispDrawLine(CENTER_X + 1, CENTER_Y, x + 1, y, color);
}

/* Sovrascrive lancetta precedente con sfondo bianco */
static void clear_line(float angle_deg, int length) {
    float angle = (angle_deg - 90.0f) * (M_PI / 180.0f);
    int x = CENTER_X + (int)(cosf(angle) * length);
    int y = CENTER_Y + (int)(sinf(angle) * length);

    gdispDrawLine(CENTER_X, CENTER_Y, x, y, White);
    gdispDrawLine(CENTER_X + 1, CENTER_Y, x + 1, y, White);
}

static void draw_clock_face(void) {
    // Disegna solo il cerchio dell'orologio
    gdispFillCircle(CENTER_X, CENTER_Y, CLOCK_RADIUS, White);

    // Contorno
    gdispDrawCircle(CENTER_X, CENTER_Y, CLOCK_RADIUS, Black);
    gdispDrawCircle(CENTER_X, CENTER_Y, CLOCK_RADIUS - 1, Black);

    // Segni ore
    for (int i = 0; i < 12; i++) {
        float angle = i * 30.0f * (M_PI / 180.0f);
        int x1 = CENTER_X + (int)(cosf(angle) * (CLOCK_RADIUS - 8));
        int y1 = CENTER_Y + (int)(sinf(angle) * (CLOCK_RADIUS - 8));
        int x2 = CENTER_X + (int)(cosf(angle) * (CLOCK_RADIUS));
        int y2 = CENTER_Y + (int)(sinf(angle) * (CLOCK_RADIUS));
        gdispDrawLine(x1, y1, x2, y2, Black);
    }
}

static void drawNumbers(void) {
    static const char* numbers[12] = {"12","1","2","3","4","5","6","7","8","9","10","11"};
    font_t font = gdispOpenFont("DejaVuSans12");

    for (int i = 0; i < 12; i++) {
        float angle = i * 30.0f * (M_PI / 180.0f) - M_PI / 2;
        int x = CENTER_X + (int)(cosf(angle) * (CLOCK_RADIUS - 18));
        int y = CENTER_Y + (int)(sinf(angle) * (CLOCK_RADIUS - 18));

        gdispDrawString(x-6, y-6, numbers[i], font, Black);
    }
}

void drawDigitalClock(int hours, int minutes, int seconds) {
    // Rettangolo sfondo digitale
    gdispFillArea(DIGITAL_RECT_X, DIGITAL_RECT_Y, DIGITAL_RECT_W, DIGITAL_RECT_H, White);
    gdispDrawBox(DIGITAL_RECT_X, DIGITAL_RECT_Y, DIGITAL_RECT_W, DIGITAL_RECT_H, Black);
    gdispDrawBox(DIGITAL_RECT_X, DIGITAL_RECT_Y, DIGITAL_RECT_W - 1, DIGITAL_RECT_H - 1, Black);

    char time_str[9];
    time_str[0] = '0' + (hours / 10);
    time_str[1] = '0' + (hours % 10);
    time_str[2] = ':';
    time_str[3] = '0' + (minutes / 10);
    time_str[4] = '0' + (minutes % 10);
    time_str[5] = ':';
    time_str[6] = '0' + (seconds / 10);
    time_str[7] = '0' + (seconds % 10);
    time_str[8] = '\0';

    font_t font = gdispOpenFont("DejaVuSans24");
    gdispDrawString(DIGITAL_RECT_X + 20, DIGITAL_RECT_Y + 20, time_str, font, Black);
}

/* Thread clock */
static THD_WORKING_AREA(waClockThread, 512);
static THD_FUNCTION(ClockThread, arg) {
    (void)arg;
    chRegSetThreadName("Clock");

    RTCDateTime timespec;
    float prev_hour_angle = 0, prev_min_angle = 0, prev_sec_angle = 0;

    draw_clock_face();
    drawNumbers();

    while (true) {
        int h, m, s;
        rtcGetTime(&RTCD1, &timespec);
        rtcGetHMS(&timespec, &h, &m, &s);

        float hour_angle   = (h % 12) * 30.0f + (m * 0.5f);
        float minute_angle = m * 6.0f;
        float second_angle = s * 6.0f;

        clear_line(prev_hour_angle,   CLOCK_RADIUS * 0.5f);
        clear_line(prev_min_angle,    CLOCK_RADIUS * 0.75f);
        clear_line(prev_sec_angle,    CLOCK_RADIUS * 0.9f);

        draw_line(hour_angle,   CLOCK_RADIUS * 0.5f, Black);
        draw_line(minute_angle, CLOCK_RADIUS * 0.75f, Blue);
        draw_line(second_angle, CLOCK_RADIUS * 0.9f, Red);

        drawDigitalClock(h, m, s);

        prev_hour_angle = hour_angle;
        prev_min_angle = minute_angle;
        prev_sec_angle = second_angle;

        chThdSleepMilliseconds(1000);
    }
}

/* Thread slider */
static THD_WORKING_AREA(slider_wa, 1024);
static THD_FUNCTION(sliderThread, arg) {
    (void)arg;

    const char* text = "This is your NISC2025 clock! :)";
    font_t font = gdispOpenFont("DejaVuSans12");
    int y = 10;
    int speed = 1;
    int slider_x = gdispGetWidth();
    int text_width = gdispGetStringWidth(text, font);

    while (true) {
        gdispFillArea(0, y, gdispGetWidth(), gdispGetFontMetric(font, fontHeight), 0xE00F);
        gdispDrawString(slider_x, y, text, font, White);

        slider_x -= speed;
        if (slider_x + text_width < 0)
            slider_x = gdispGetWidth();

        chThdSleepMilliseconds(30);
    }
}

int main(void) {
    halInit();
    chSysInit();
    sdStart(&SD2, NULL);
    gfxInit();

    gdispFillArea(0, 0, gdispGetWidth(), gdispGetHeight(), Blue);

    RTCDateTime timespec;
    rtcConvertStructTmToDateTime(&(struct tm){
        .tm_year = (2025-1900),
        .tm_mon  = 9,
        .tm_mday = 2,
        .tm_hour = 8,
        .tm_min  = 25,
        .tm_sec  = 10,
        .tm_wday = 2
    }, 0, &timespec);
    rtcSetTime(&RTCD1, &timespec);

    chThdCreateStatic(waClockThread, sizeof(waClockThread), NORMALPRIO, ClockThread, NULL);
    chThdCreateStatic(slider_wa, sizeof(slider_wa), NORMALPRIO, sliderThread, NULL);

    while (true) {
        chThdSleepMilliseconds(1000);
    }
}

