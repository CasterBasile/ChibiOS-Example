/*
    Neapolis Innovation - Copyright (C) 2023

    Salvatore Bramante, Giovanni Francesco Comune, Salvatore Dello Iacono

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


#define PORTAB_DAC_TRIG             7
#define ADC_GRP_NUM_CHANNELS		1
#define ADC_BUFFER_SIZE				10

#define DAC_BUFFER_SIZE				360

#define SAMPLING_FREQ				1000000



static adcsample_t adc_buffer[ADC_GRP_NUM_CHANNELS * DAC_BUFFER_SIZE];
static dacsample_t dac_filtered_buffer[ADC_GRP_NUM_CHANNELS * DAC_BUFFER_SIZE];


/*
 * DAC error callback.
 */
static void error_cb1(DACDriver *dacp, dacerror_t err) {

  (void)dacp;
  (void)err;

  chSysHalt("DAC failure");
}

/*
 * DAC configuration.
 */
static const DACConfig dac1cfg1 = {
  .init         = 2047U,
  .datamode     = DAC_DHRM_12BIT_RIGHT,
  .cr           = 0
};

static const DACConversionGroup dacgrpcfg1 = {
  .num_channels = 1U,
  .end_cb       = NULL,
  .error_cb     = error_cb1,
  .trigger      = DAC_TRG(PORTAB_DAC_TRIG)
};


/*
 * GPT6 configuration.
 */
static GPTConfig gpt6cfg1 = {
  .frequency    = SAMPLING_FREQ,
  .callback     = NULL,
  .cr2          = TIM_CR2_MMS_1,    /* MMS = 010 = TRGO on Update Event.    */
  .dier         = 0U
};

#define FILTER_WINDOW_SIZE 10
static float filter_buff[FILTER_WINDOW_SIZE];
static int filter_idx = 0;

adcsample_t filter_function(adcsample_t sample) {
	float out = 0;
	int i = 0;

	filter_buff[filter_idx] = sample / FILTER_WINDOW_SIZE;
	filter_idx = (filter_idx + 1) % FILTER_WINDOW_SIZE;

	for( i = 0; i < FILTER_WINDOW_SIZE; i++ ) {
		out += filter_buff[i];
	}
	return (adcsample_t) out;
}

/*
 * ADC end callback.
 */
static void adc_end_cb(ADCDriver *adcp ) {

    adcsample_t * samples = adcp->samples;
    uint32_t depth = adcp->depth;

    uint32_t M = depth;

    for(size_t k=0; k < M ; k++){
    	dac_filtered_buffer[k] = filter_function(samples[k]);
    }

    dacStartConversion(&DACD1, &dacgrpcfg1, dac_filtered_buffer, DAC_BUFFER_SIZE);

}


/*
 * ADC error callback.
 */
static void adcerrorcallback(ADCDriver *adcp, adcerror_t err) {
  (void)adcp;
  (void)err;
  chSysHalt("ADC failure");
}


/*
 * DAC configuration.
 */
const ADCConversionGroup adcgrpcfg = {
  .circular     = true,
  .num_channels = ADC_GRP_NUM_CHANNELS,
  .end_cb       = adc_end_cb,
  .error_cb     = adcerrorcallback,
  .cfgr         = ADC_CFGR_EXTEN_RISING |
                  ADC_CFGR_EXTSEL_SRC(13),  /* TIM6_TRGO */
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

  /*
   * Start the GPT6 driver with a period of 10 cycles and a
   * frequency of 1000000 Hz
   */
  gptStart(&GPTD6, &gpt6cfg1);
  gptStartContinuous(&GPTD6, 10);

  /* Setting up the output pin as analog as suggested
       by the Reference Manual.*/
  palSetPadMode(GPIOA, 0U, PAL_MODE_INPUT_ANALOG);
  palSetPadMode(GPIOA, 4U, PAL_MODE_INPUT_ANALOG);
  palSetPadMode(GPIOA, 5u, PAL_MODE_INPUT_ANALOG);

  /*
   * Starting GPT6 driver, it is used for triggering the ADC and DAC.
   * Starting the ADC1 and DAC1 driver.
   */
  adcStart(&ADCD1, NULL);
  dacStart(&DACD1, &dac1cfg1);

  adcStartConversion(&ADCD1, &adcgrpcfg, adc_buffer, DAC_BUFFER_SIZE);


  while (true ) {
	  chThdSleepMilliseconds(200);
  }
}
