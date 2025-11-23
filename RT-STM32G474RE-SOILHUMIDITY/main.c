/*
    NeaPolis Innovation Summer Campus 2025 Examples
    Copyright (C) 2020-2025

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
 * [SSD1306] Ground Humidity Sensor Example
 * A simple example with the Ground Humidity Sensor
 */

#include "ch.h"
#include "hal.h"
#include "chprintf.h"
#include "stdio.h"
#include "../lib/map.h"

#define BUFF_SIZE   2
char buff1[BUFF_SIZE];
float igrometer;
int humidityLevelPercentage=0;


BaseSequentialStream * chp = (BaseSequentialStream *) &SD2;

#define VOLTAGE_RES            ((float)3.3/4096)

#define MSG_ADC_OK               0x1337
#define MSG_ADC_KO               0x7331
static thread_reference_t trp = NULL;

static void adccallback(ADCDriver *adcp) {
  if (adcIsBufferComplete(adcp)) {
    chSysLockFromISR();
    chThdResumeI(&trp, (msg_t) MSG_ADC_OK );
    chSysUnlockFromISR();
  }
}


static void adcerrorcallback(ADCDriver *adcp, adcerror_t err) {
  (void)adcp;
  (void)err;
  chSysLockFromISR();
  chThdResumeI(&trp, (msg_t) MSG_ADC_KO );
  chSysUnlockFromISR();
}

#define ADC_GRP_NUM_CHANNELS        1
#define ADC_GRP_BUF_DEPTH           16
static adcsample_t samples[ADC_GRP_NUM_CHANNELS * ADC_GRP_BUF_DEPTH];

static const ADCConversionGroup adcgrpcfg = {
          .circular     = false,
          .num_channels = ADC_GRP_NUM_CHANNELS,
          .end_cb       = adccallback,
          .error_cb     = adcerrorcallback,
          .cfgr         = ADC_CFGR_CONT,
          .cfgr2        = 0U,
          .tr1          = ADC_TR_DISABLED,
          .tr2          = ADC_TR_DISABLED,
          .tr3          = ADC_TR_DISABLED,
          .awd2cr       = 0U,
          .awd3cr       = 0U,
          .smpr         = {
            ADC_SMPR1_SMP_AN1(ADC_SMPR_SMP_247P5),
            0U
          },
          .sqr          = {
            ADC_SQR1_SQ1_N(ADC_CHANNEL_IN1),
            0U,
            0U,
            0U
          }
        };

static float converted[ADC_GRP_NUM_CHANNELS];

static THD_WORKING_AREA( waThdHumidityLevelSensor, 1024);
static THD_FUNCTION( thdHumidityLevelSensor, arg ) {
  (void) arg;

   palSetPadMode(GPIOA, 1U, PAL_MODE_INPUT_ANALOG);

  // chprintf( chp, "Test %%\n\r");

   adcStart(&ADCD1, NULL);

   while ( true ){
    msg_t msg;
    int i;

    chSysLock();
    adcStartConversionI(&ADCD1, &adcgrpcfg, samples, ADC_GRP_BUF_DEPTH);

    msg = chThdSuspendS(&trp);

    chSysUnlock();

    if( msg == MSG_ADC_KO ) {
      continue;
    }

    for( i = 0; i < ADC_GRP_NUM_CHANNELS; i++ ) {
      converted[i] = 0.0f;
    }

    for( i = 0; i < ADC_GRP_NUM_CHANNELS * ADC_GRP_BUF_DEPTH; i++ ) {
      converted[ i % ADC_GRP_NUM_CHANNELS] += (float) samples[i] * VOLTAGE_RES;
    }

    for( i = 0; i < ADC_GRP_NUM_CHANNELS; i++ ) {
      converted[i] /= ADC_GRP_BUF_DEPTH;
    }

    igrometer = converted[0];
    humidityLevelPercentage = map(igrometer, 0, 2.05, 0, 100);
    chprintf(chp, "Voltage: %.3f V | Humidity: %d %%\n\r", igrometer, humidityLevelPercentage);
    chThdSleepMilliseconds(500);
  }
  adcStop(&ADCD1);
}



int main(void) {

  halInit();
  chSysInit();

  palSetPadMode(GPIOA, 2, PAL_MODE_ALTERNATE(7));
  palSetPadMode(GPIOA, 3, PAL_MODE_ALTERNATE(7));
  sdStart(&SD2, NULL);

  chThdCreateStatic( waThdHumidityLevelSensor, sizeof( waThdHumidityLevelSensor), NORMALPRIO + 5, thdHumidityLevelSensor, (void*) NULL );


  while (true) {
    chThdSleepMilliseconds(500);
  }
}
