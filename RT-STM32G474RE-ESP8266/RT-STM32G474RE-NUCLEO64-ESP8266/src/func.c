#include "../lib/func.h"

/*
 * Prints sensor data struct values
 */
void print_data_tx(tx_data_t* Data){
  chprintf((BaseSequentialStream *) &SD2, "Printing from print_data_tx(): %d|%d|%d|%d . %c | %c | %c\n\r", Data->lightVal / 35, Data->waterVal, Data->tempVal, Data->humVal,
                                                                         (char)Data->pirOn, (char)Data->vibOn, (char)Data->butOn);
}

/*
 * Prints user gui input data struct values
 */
void print_data_rx(rx_data_t* Data){
  chprintf((BaseSequentialStream *) &SD2, "Printing from print_data_rx(): %d|%d|%d|%d\n\r", Data->ledOn, Data->windowOn, Data->alarmOn, Data->antiTheftOn);
}

/*
 * Init. and Starts the serial communications
 */
void serial_init(void){
  /*
   * SD1 - Used to make the nucleo and the ESP8266 WiFi Module communicate
   */
  palSetPadMode(GPIOC, 4, PAL_MODE_ALTERNATE(7)); //WIFI D0 D1 SD1
  palSetPadMode(GPIOC, 5, PAL_MODE_ALTERNATE(7));
  /*
   * SD2 - Used to print in the terminal
   */
  palSetPadMode(GPIOA, 2, PAL_MODE_ALTERNATE(7)); //TERMINAL SD2
  palSetPadMode(GPIOA, 3, PAL_MODE_ALTERNATE(7));
  sdStart( &SD1, NULL );
  sdStart( &SD2, NULL );
}

/*
 * Reads the user gui inputs from the web gui through serial connection
 * Waits for data (sdRead is blocking)
 * User input is encoded as a four bit string (like '0101')
 * Each bit represents a gui button. 0 and 1 values represent if the button
 * has been set off or on.
 */
void wifi_read(rx_data_t * Data){
  uint8_t buf[RX_MEX_DIM];
  sdRead( &SD1, buf, RX_MEX_DIM);
  Data->ledOn = buf[0];
  Data->windowOn = buf[1];
  Data->alarmOn = buf[2];
  Data->antiTheftOn = buf[3];
  //print_data_rx(Data);
}

/*
 * Sends the sensor data to the web gui through serial
 * The message sent is a string which carries three int values, separated by '|',
 * with '&' as a terminator.
 */
void wifi_write(tx_data_t * Data){
  chprintf((BaseSequentialStream *) &SD1, "%d|%d|%d&", Data->lightVal / 35, Data->tempVal, Data->humVal);
}

/*
 * Sets the emergency_has_started variable to 0
 * Shuts the buzzer, turns off the red led and the fan.
 */
void emergencyOff(uint8_t* emergency_on){
  *emergency_on = 0;
  play_note(0);
  Rled_write(PAL_LOW);
  fan_off();
}

/*
 * Checks the sensor data to verify if an emergency is happening
 * There is an emergency if:
 *  - water is perceived by the water level sensor (flood)
 *  - shaking is perceived by the vibration sensor (earthquake)
 *  - humidity is lower than a TH AND temperature is higher than a TH (fire)
 *  - anti theft mode is on AND movement is perceived by the PIR sensor (intruder movement)
 *  - anti theft mode is on AND the button is off (intruder opened the door)
 */
void check_emergency(tx_data_t * Data, rx_data_t * Check, uint8_t* emergency_on){
  if((Data->waterVal >= WATER_TH) ||
     (Data->vibOn == (uint8_t)'1') ||
     ((Data->humVal <= HUM_TH)&&(Data->tempVal >= TEMP_TH)) ||
     ((Data->butOn == (uint8_t)'0')&&(Check->antiTheftOn == (uint8_t)'1')) ||
     ((Data->pirOn == (uint8_t)'1')&&(Check->antiTheftOn == (uint8_t)'1')))
  {
    *emergency_on = 1;
  }
}

/*
 * Reads the fields of the web gui data struct and executes the user commands
 */
void exe_wifi_request(rx_data_t * Data, uint8_t* emergency_on){

  if(Data->ledOn == (uint8_t)'1'){
    Wled_write(PAL_HIGH);
  }
  else Wled_write(PAL_LOW);

  if(Data->windowOn == (uint8_t)'1'){
      open_window();
  }
  else close_window();

  if(Data->alarmOn == (uint8_t)'1'){
    *emergency_on = 1;
  }
  else {
    emergencyOff(emergency_on);
  }

}
