/*
    NeaPolis Innovation Summer Campus Examples
    Copyright (C) 2025 Rocco Palermo [palermo.rocco561@gmail.com]
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
 * [RTC] - Using the Real Time Clock peripheral.
 * Set the date and time and print the current time every second.
 */

#include "ch.h"
#include "hal.h"
#include <string.h>
#include "chprintf.h"
#include "chscanf.h"
#include "stdio.h"
#include "stdlib.h"

#define TIME_BUFF_SIZE   10
#define SERIAL_BUFF_SIZE   64
char time_buff[TIME_BUFF_SIZE];
char serial_buff[SERIAL_BUFF_SIZE];

BaseSequentialStream * chp = (BaseSequentialStream *) &SD2;

/*
 * Function to convert the month to an integer
 */
int monthToNumber(const char *month) {
    if (strcmp(month, "Jan") == 0) return 1;
    if (strcmp(month, "Feb") == 0) return 2;
    if (strcmp(month, "Mar") == 0) return 3;
    if (strcmp(month, "Apr") == 0) return 4;
    if (strcmp(month, "May") == 0) return 5;
    if (strcmp(month, "Jun") == 0) return 6;
    if (strcmp(month, "Jul") == 0) return 7;
    if (strcmp(month, "Aug") == 0) return 8;
    if (strcmp(month, "Sep") == 0) return 9;
    if (strcmp(month, "Oct") == 0) return 10;
    if (strcmp(month, "Nov") == 0) return 11;
    if (strcmp(month, "Dec") == 0) return 12;
    return -1;
}

/*
 * Function to convert the milliseconds since 00:00 to a string with the HH:MM:SS format
 */
void millisToTimeString(unsigned long millis, char *buffer, size_t bufferSize) {
    unsigned long hours = millis / 3600000;
    millis %= 3600000;

    unsigned long minutes = millis / 60000;
    millis %= 60000;

    unsigned long seconds = millis / 1000;

    chsnprintf(buffer, bufferSize, "%02lu:%02lu:%02lu", hours, minutes, seconds);
}

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

  palSetPadMode( GPIOA, 2, PAL_MODE_ALTERNATE(7) );
  palSetPadMode( GPIOA, 3, PAL_MODE_ALTERNATE(7) );
  sdStart(&SD2, NULL );

  /*
   * The __TIME__ and __DATE__ macros expand to a string constant
   * that describe the time and date at which the preprocessor is being run.
   * They look like:
   * "Feb 12 2025" and "23:59:01"
   */
  char sysTime[] = __TIME__;
  char sysDate[] = __DATE__;
  char *token;

  RTCDateTime startTime;
  rtcObjectInit(&RTCD1);

  /*
   * Time parsing. To initialize the system's RTC, we need to tell it
   * how many milliseconds have passed since 00:00.
   */
  token = strtok(sysTime, ":");
  int time[3];
  for (int i=0; i<3; i++){
    time[i] = atoi(token);
    token = strtok(NULL, ":");
    }
  startTime.millisecond = (time[0] * 3600 + time[1] * 60 + time[2]) * 1000;

  /*
   * Date parsing. The first token will be a string representing the month
   * (ex. "Sep" for September), so we need to convert it into an integer before
   * sending it to the RTC for initialization.
   */
  token = strtok(sysDate, " ");
  int date[3];
  date[0] = monthToNumber(token); // month
  token = strtok(NULL, " ");
  date[1] = atoi(token); // day
  token = strtok(NULL, " ");
  date[2] = atoi(token); // year

  startTime.month = date[0];
  startTime.day = date[1];
  startTime.year = date[2] - 1980; //Year is stored as years since 1980
  rtcSetTime(&RTCD1, &startTime);
  RTCDateTime currTime;

  while (true) {
    rtcGetTime(&RTCD1, &currTime);
    millisToTimeString(currTime.millisecond, time_buff, TIME_BUFF_SIZE);
    chsnprintf(serial_buff, SERIAL_BUFF_SIZE, "%02lu/%02lu/%04lu - %s",
               currTime.day, currTime.month, currTime.year + 1980, time_buff);
    chprintf(chp, "%s\r\n", serial_buff);
    chThdSleepMilliseconds(1000);
  }
}
