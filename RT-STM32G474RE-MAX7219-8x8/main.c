/*
    NeaPolis Innovation Summer Campus Play Embedded Porting
    Copyright (C) 2025 Cinzia Barbato [cinziaabarbato@outlook.it]
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
 * [NISC2024-MAX7219-8x8] - Driver for 8x8 led matrix
 * DESCRIPTION: Random symbols on matrix.
 */
#include "ch.h"
#include "hal.h"

#include "max7219.h"

/*===========================================================================*/
/* LED Matrix related code.                                                  */
/*===========================================================================*/

#define MAX_PRESET_NUMB                         17
#define MAX_DIGITS                              8

#define LINE_ARD_D13 PAL_LINE(GPIOA,5U)  // SCK
#define LINE_ARD_D12 PAL_LINE(GPIOA,6U)  // MISO (non usato dal MAX7219)
#define LINE_ARD_D11 PAL_LINE(GPIOA,7U)  // MOSI (DIN)
#define LINE_ARD_D10 PAL_LINE(GPIOB,6U)  // CS (Chip Select)

static uint8_t presets[MAX_PRESET_NUMB][MAX_DIGITS] = {
   {0x18, 0x18, 0x18, 0x99, 0xDB, 0x7E, 0x3C, 0x18}, /**< Arrow dx.         */
   {0x18, 0x3C, 0x7E, 0xDB, 0x99, 0x18, 0x18, 0x18}, /**< Arrow sx.         */
   {0x00, 0x20, 0x44, 0x40, 0x40, 0x44, 0x20, 0x00}, /**< Happy face.       */
   {0x00, 0x20, 0x14, 0x10, 0x10, 0x14, 0x20, 0x00}, /**< Sad face.         */
   {0x00, 0x00, 0x00, 0xDF, 0xDF, 0x00, 0x00, 0x00}, /**< Exclamation point.*/
   {0x24, 0x66, 0xE7, 0xE7, 0xE7, 0xE7, 0x66, 0x24}, /**< Negate.           */
   {0x00, 0x84, 0x86, 0xFF, 0xFF, 0x80, 0x80, 0x00}, /**< 1.                */
   {0x00, 0x86, 0xC7, 0xE1, 0xB1, 0x9F, 0x8E, 0x00}, /**< 2.                */
   {0x00, 0x00, 0x42, 0xC3, 0xDB, 0xFF, 0x66, 0x00}, /**< 3.                */
   {0x00, 0x3F, 0x3F, 0x20, 0x20, 0xF8, 0xF8, 0x00}, /**< 4.                */
   {0x00, 0xDF, 0xDB, 0xDB, 0xDB, 0xFB, 0x71, 0x00}, /**< 5.                */
   {0x00, 0x7E, 0xFF, 0x89, 0x89, 0x89, 0x70, 0x00}, /**< 6.                */
   {0x00, 0x03, 0x03, 0xE3, 0xFB, 0x1F, 0x07, 0x00}, /**< 7.                */
   {0x00, 0x76, 0xFF, 0x99, 0x99, 0xFF, 0x76, 0x00}, /**< 8.                */
   {0x00, 0x8E, 0x91, 0x91, 0x91, 0xFF, 0x7E, 0x00}, /**< 9.                */
   {0x00, 0x7E, 0xFF, 0x81, 0x81, 0xFF, 0x7E, 0x00}, /**< 0.                */
   {0x00, 0x06, 0x76, 0x30, 0x30, 0x76, 0x06, 0x00}  /**< Creeper face.     */
};

/*===========================================================================*/
/* SPI related code.                                                         */
/*===========================================================================*/



static const SPIConfig spicfg = {
  .circular         = FALSE,
  .slave            = FALSE,
  .data_cb          = NULL,
  .error_cb         = NULL,
  .ssport          = GPIOB ,
  .sspad           = 6U,
  .cr1              = SPI_CR1_BR_2 | SPI_CR1_BR_1 | SPI_CR1_BR_0,  //Baud rate f_PCLK/8
  .cr2              = SPI_CR2_DS_3 | SPI_CR2_DS_2 | SPI_CR2_DS_1 | SPI_CR2_DS_0 // Frame di 16 bit (DS = 1111)
};

/*===========================================================================*/
/* Generic code.                                                             */
/*===========================================================================*/
 static unsigned ii, jj;

/*
 * Application entry point.
 */
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
   * SPID1 I/O pins setup.(It bypasses board.h configurations)
   */
  palSetLineMode(LINE_ARD_D13, PAL_MODE_ALTERNATE(5) |
                                   PAL_STM32_OSPEED_HIGHEST);
  palSetLineMode(LINE_ARD_D12, PAL_MODE_ALTERNATE(5) |
                                    PAL_STM32_OSPEED_HIGHEST);
  palSetLineMode(LINE_ARD_D11, PAL_MODE_ALTERNATE(5) |
                                    PAL_STM32_OSPEED_HIGHEST);
  palSetLineMode(LINE_ARD_D10, PAL_MODE_OUTPUT_PUSHPULL |
                               PAL_STM32_OSPEED_HIGHEST);

  spiStart(&SPID1, &spicfg);
  max7219WriteRegister(&SPID1, MAX7219_AD_DISPLAY_TEST, FALSE);
  max7219WriteRegister(&SPID1, MAX7219_AD_SHUTDOWN, MAX7219_OM_Normal);
  max7219WriteRegister(&SPID1, MAX7219_AD_SCAN_LIMIT, MAX7219_SL_7);
  max7219WriteRegister(&SPID1, MAX7219_AD_DECODE_MODE,
                       MAX7219_DM_No_decode);
  max7219WriteRegister(&SPID1, MAX7219_AD_INTENSITY,
                       MAX7219_IM_31_32);
  while (TRUE) {

    for(ii = 0; ii < MAX_DIGITS; ii++) {

      max7219WriteRegister(&SPID1, MAX7219_AD_DIGIT_0 + (ii << 8),
                           presets[jj][ii]);
    }
    jj = (jj + 1) % MAX_PRESET_NUMB;
    chThdSleepMilliseconds(500);
  }
  spiStop(&SPID1);
}

