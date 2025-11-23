#include "../lib/d_in.h"

/*
 * Init. PIR Motion sensor, Vibration Sensor and Button pads
 */
void d_in_init(void){
  palSetPadMode(GPIOA, 8U, PAL_MODE_INPUT);     //PIR - D7
  palSetPadMode(GPIOA, 9U, PAL_MODE_INPUT);     //VIB - D8
  palSetPadMode(GPIOA, 10U, PAL_MODE_INPUT);    //BUT - D2
}

/*
 * Reads digital inputs
 * Stores the values in the data struct
 */
void d_in_read(tx_data_t * Data){

  if(palReadPad(GPIOA, 8U) == PAL_LOW ){
    Data->pirOn = (uint8_t)'1';
  }
  else {
    Data->pirOn = (uint8_t)'0';
  }

  if(palReadPad(GPIOA, 9U) == PAL_LOW ){
      Data->vibOn = (uint8_t)'1';
  }
  else {
    Data->vibOn = (uint8_t)'0';
  }

  if(palReadPad(GPIOA, 10U) == PAL_LOW ){
      Data->butOn = (uint8_t)'1';
  }
  else {
    Data->butOn = (uint8_t)'0';
  }

}
