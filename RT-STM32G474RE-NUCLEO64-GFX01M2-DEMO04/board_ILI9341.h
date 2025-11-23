/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.io/license.html
 */

#ifndef _GDISP_LLD_BOARD_H
#define _GDISP_LLD_BOARD_H

#include "hal.h"

#define LCD_DC_PORT		GPIOB
#define LCD_DC_PIN  	10U

#define LCD_RST_PORT 	GPIOA
#define LCD_RST_PIN 	1U

#define LCD_CS_PORT		GPIOA
#define LCD_CS_PIN		9U

#define LCD_TE_PORT		GPIOA
#define LCD_TE_PIN		0U

#define LCD_LINE_SCK	PAL_LINE(GPIOA, 5U)
#define LCD_LINE_MISO	PAL_LINE(GPIOA, 6U)
#define LCD_LINE_MOSI	PAL_LINE(GPIOA, 7U)

#define LCD_DC_CMD	palClearPad(LCD_DC_PORT, LCD_DC_PIN)
#define LCD_DC_DATA	palSetPad(LCD_DC_PORT, LCD_DC_PIN)
#define LCD_SCK_SET	palSetPad(GPIOA, 5U)
#define LCD_SCK_RES	palClearPad(GPIOA, 5U)
#define LCD_CS_RES	palSetPad(LCD_CS_PORT, LCD_CS_PIN)
#define LCD_CS_SET	palClearPad(LCD_CS_PORT, LCD_CS_PIN)

/**
 * SPI configuration structure.
 * Speed 12 MHz, CPHA=0, CPOL=0, 8bits frames, MSb transmitted first.
 * Soft slave select.
 */
const SPIConfig spi1cfg = {
  .circular         = false,
  .slave            = false,
  .data_cb          = NULL,
  .error_cb         = NULL,
  .ssport           = LCD_CS_PORT,
  .sspad            = LCD_CS_PIN,
  .cr1              = SPI_CR1_BR_1 | SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI,
  .cr2              = 0U
};


static GFXINLINE void init_board(GDisplay *g) {
	(void) g;
	palSetPadMode(LCD_TE_PORT, LCD_TE_PIN, PAL_MODE_OUTPUT_PUSHPULL); // Check this
	palSetPadMode(LCD_CS_PORT, LCD_CS_PIN, PAL_MODE_OUTPUT_PUSHPULL);
	palSetPadMode(LCD_DC_PORT, LCD_DC_PIN, PAL_MODE_OUTPUT_PUSHPULL);
	palSetPadMode(LCD_RST_PORT, LCD_RST_PIN, PAL_MODE_OUTPUT_PUSHPULL);
  	palSetLineMode(LCD_LINE_MOSI, PAL_MODE_ALTERNATE(5));
  	palSetLineMode(LCD_LINE_MISO, PAL_MODE_ALTERNATE(5));
  	palSetLineMode(LCD_LINE_SCK,  PAL_MODE_ALTERNATE(5));

    spiStart(&SPID1, &spi1cfg);
	// spiSelect(&SPID1);
}

static GFXINLINE void post_init_board(GDisplay *g) {
	(void) g;
}

static GFXINLINE void setpin_reset(GDisplay *g, gBool state) {
	(void) g;
	if (state)
        palClearPad(LCD_RST_PORT, LCD_RST_PIN);
    else
        palSetPad(LCD_RST_PORT, LCD_RST_PIN);
}

static GFXINLINE void set_backlight(GDisplay *g, gU8 percent) {
	(void) g;
	(void) percent;
}

static GFXINLINE void acquire_bus(GDisplay *g) {
	(void) g;
	spiSelect(&SPID1);
}

static GFXINLINE void release_bus(GDisplay *g) {
	(void) g;
	spiUnselect(&SPID1);
}

static GFXINLINE void write_index(GDisplay *g, gU16 index) {
	(void) g;
	palClearPad(LCD_DC_PORT, LCD_DC_PIN);  // Command mode
    spiSend(&SPID1, 1, &index);
}

static GFXINLINE void write_data(GDisplay *g, gU16 data) {
	(void) g;
	
	palSetPad(LCD_DC_PORT, LCD_DC_PIN);    // Data mode
    spiSend(&SPID1, 1, &data);
}

static GFXINLINE void setreadmode(GDisplay *g) {
	(void) g;
}

static GFXINLINE void setwritemode(GDisplay *g) {
	(void) g;
}

static GFXINLINE gU16 read_data(GDisplay *g) {
	(void) g;
	gU16 data;
    spiReceive(&SPID1, 1, &data);
    return data;
}

#endif /* _GDISP_LLD_BOARD_H */
