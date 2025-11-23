#include "../lib/adc_in.h"

static adcsample_t samples[ADC_GRP_NUM_CHANNELS * ADC_GRP_BUF_DEPTH];

/*
 * Initializes LDR pad and Water Level Sensror pad
 * Starts ADC
 */
void adc_init(void){
  palSetPadMode(GPIOC, 0U, PAL_MODE_INPUT_ANALOG);  //LDR - A5
  palSetPadMode(GPIOC, 1U, PAL_MODE_INPUT_ANALOG);  //WATER - A4
  adcStart(&ADCD1, NULL);
  adcSTM32EnableVREF(&ADCD1);
}

/*
 * Reads ADC sensors' values
 * Does the convertions
 * Stores the values in the data struct
 */
void adc_read(tx_data_t * Data){

  adcConvert(&ADCD1,  &linearcfg, samples, ADC_GRP_BUF_DEPTH);
  Data->lightVal=samples[0];
  Data->waterVal=samples[1];

}
