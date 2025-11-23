#include "../lib/icu_in.h"

static uint8_t TEMP, HR, CHECK_SUM, tmp, bit_counter = 0;;
static icucnt_t widths [40];

static void icuwidthcb(ICUDriver *icup) {
  icucnt_t width = icuGetWidthX(icup);
  if(width >= DHT_START_BIT_WIDTH){
    /* starting bit resetting the bit counter */
    bit_counter = 0;
  }
  else{
    /* Recording current width */
    widths[bit_counter] = width;
    if(width > DHT_LOW_BIT_WIDTH){
      tmp |= (1 << (7 - (bit_counter % 8)));
    }
    else{
      tmp &= ~(1 << (7 - (bit_counter % 8)));
    }
    /* When bit_counter is 7, tmp contains the bit from 0 to 7 corresponding to
       The Humidity Rate integer part (Decimal part is 0 on DHT 11) */
    if(bit_counter == 7)
      HR = tmp;
    /* When bit_counter is 23, tmp contains the bit from 16 to 23 corresponding to
       The Temperature integer part (Decimal part is 0 on DHT 11) */
    if(bit_counter == 23)
      TEMP = tmp;
    /* When bit_counter is 39, tmp contains the bit from 32 to 39 corresponding to
       The Check sum value */
    if(bit_counter == 39)
      CHECK_SUM = tmp;
    bit_counter++;
  }
}

static ICUConfig icucfg = {
  ICU_INPUT_ACTIVE_HIGH,
  ICU_FREQUENCY,
  NULL,
  icuwidthcb,
  NULL,
  ICU_CHANNEL_1,
  0U,
  0xFFFFFFFFU
};

void icu_read(tx_data_t * Data){
  /*
   * Making a request
   */
  palSetPadMode(GPIOA, 0, PAL_MODE_OUTPUT_PUSHPULL); //DHT - A0
  palWritePad(GPIOA, 0, PAL_LOW);
  chThdSleepMicroseconds(MCU_REQUEST_WIDTH);
  palWritePad(GPIOA, 0, PAL_HIGH);
  /*
   * Initializes the ICU driver 1.
   * GPIOA8 is the ICU input.
   */
  palSetPadMode(GPIOA, 0, PAL_MODE_ALTERNATE(2));
  icuStart(&ICUD5, &icucfg);
  icuStartCapture(&ICUD5);
  icuEnableNotifications(&ICUD5);
  chThdSleepMilliseconds(700);
  #if ANSI_ESCAPE_CODE_ALLOWED
    //chprintf((BaseSequentialStream *) &SD2, "\033[2J\033[1;1H");
  #endif
  icuStopCapture(&ICUD5);
  icuStop(&ICUD5);

  /*
  chprintf(chp, "Temperature: %d C, Humidity Rate: %d %% \n\r", TEMP, HR);
  if(CHECK_SUM == (TEMP + HR)){
    chprintf(chp, "Checksum OK!\n\r");
  }
  else{
    chprintf(chp, "Checksum FAILED!\n\r");
  }
  */

  Data->humVal = HR;
  Data->tempVal = TEMP;
}
