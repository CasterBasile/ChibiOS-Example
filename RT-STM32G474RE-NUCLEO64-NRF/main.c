/*
 ChibiOS - Copyright (C) 2025 Tammaro Cimmino Cinzia Barbato

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
#include "string.h"
#include "chprintf.h"
#include "rf.h"
#include "defines.h"

#define  TRANSMITTER    TRUE

BaseSequentialStream *chp = (BaseSequentialStream*)&SD2;

/*
 * Must be the same between transmitter and receiver,
 * unique for each communication channel.
 */
static char NRF_CHANNEL[] = "edit";

void send_data(void) {
  RFTxFrame txbuff;
  rf_msg_t rcvmsg;

  uint8_t data[4] = {1, 2, 3, 4};

  for (unsigned char i = 0; i < 5; i++) {
    txbuff.tx_address[i] = NRF_CHANNEL[i];
  }

  for (uint8_t i = 0; i < sizeof(data); i++) {
    txbuff.tx_payload[i] = data[i];
  }

  txbuff.tx_paylen = sizeof(data);

  do {
    rcvmsg = rfTransmit(&RFD1, 1, &txbuff, TIME_MS2I(75));
  }
  while(rcvmsg != RF_OK);
  chprintf(chp, "Sent array.\n\r");
}

void recv_data(void) {
  RFRxFrame rxbuff;
  rf_msg_t rcvmsg;
  uint8_t array_size = 4;

  for (unsigned char i = 0; i < 5; i++) {
    rxbuff.rx_address[i] = NRF_CHANNEL[i];
  }

  rxbuff.rx_paylen = array_size;

  rcvmsg = rfReceive(&RFD1, 1, &rxbuff, TIME_INFINITE);
  if (rcvmsg == RF_OK){
    chprintf(chp, "Received array: [");
    for(uint8_t i = 0; i < array_size; i++){
      chprintf(chp, "%d ", rxbuff.rx_payload[i]);
    }
    chprintf(chp, "]\n\r");
  }
}


void send_string(void) {
  rf_msg_t rcvmsg;
  char input_string[20] = "Hello!";
  do {
    rcvmsg = rfTransmitString(&RFD1, input_string, NRF_CHANNEL, TIME_MS2I(75));
  }
  while(rcvmsg != RF_OK);
  chprintf(chp, "Sent string: %s\n\r", input_string);

}

void recv_string(void) {
  rf_msg_t rcvmsg;
  char input_string[20];
  rcvmsg = rfReceiveString(&RFD1, input_string, NRF_CHANNEL, TIME_INFINITE);
  if (rcvmsg == RF_OK){
    chprintf(chp, "Received string: %s\n\r", input_string);
  }
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

  /*
   * Activates the serial driver 2 using A2 and A3 pins.
   */
  palSetPadMode(GPIOA, 2, PAL_MODE_ALTERNATE(7));
  palSetPadMode(GPIOA, 3, PAL_MODE_ALTERNATE(7));
  sdStart(&SD2, NULL);

  /*
   * Activates the RF driver.
   */
  palSetLineMode(NRF_SCK_LINE,
                 PAL_MODE_ALTERNATE(5) | PAL_STM32_OSPEED_HIGHEST);
  palSetLineMode(NRF_MISO_LINE,
                 PAL_MODE_ALTERNATE(5) | PAL_STM32_OSPEED_HIGHEST);
  palSetLineMode(NRF_MOSI_LINE,
                 PAL_MODE_ALTERNATE(5) | PAL_STM32_OSPEED_HIGHEST);
  palSetLineMode(NRF_SPI_CS,
                 PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);
  palSetLineMode(NRF_CE_LINE,
                 PAL_MODE_OUTPUT_PUSHPULL |PAL_STM32_OSPEED_HIGHEST);
  palSetLineMode(NRF_IRQ_LINE, PAL_MODE_INPUT | PAL_STM32_OSPEED_HIGHEST);

  rfInit();
  rfStart(&RFD1, &nrf24l01_cfg);

  while (true) {
#if TRANSMITTER == TRUE
    send_string();
    // send_data();
#else
    recv_data();
    // recv_data();
#endif
    chThdSleepMilliseconds(1000);
  }
}

