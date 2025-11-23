#include "../lib/d_out.h"

/*
 * Init. Red LED, White LED and FAN pads
 */
void d_out_init(void){
  palSetPadMode(GPIOB, 6U, PAL_MODE_OUTPUT_PUSHPULL);   //R - D10
  palSetPadMode(GPIOA, 7U, PAL_MODE_OUTPUT_PUSHPULL);   //W - D11
  palSetPadMode(GPIOB, 10U, PAL_MODE_OUTPUT_PUSHPULL);  //FAN - D6
}

/*
 * Sets the White LED on or off
 */
void Wled_write(uint8_t W){
  palWritePad(GPIOA, 7U, W);
}

/*
 * Sets the Red LED on or off
 */
void Rled_write(uint8_t R){
  palWritePad(GPIOB, 6U, R);
}

/*
 * Turns on the FAN
 */
void fan_on(void){
  palSetPad(GPIOB, 10U);
}

/*
 * Turns off the FAN
 */
void fan_off(void){
  palClearPad(GPIOB, 10U);
}
