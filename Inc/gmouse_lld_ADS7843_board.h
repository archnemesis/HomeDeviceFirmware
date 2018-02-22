/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#ifndef _GINPUT_LLD_MOUSE_BOARD_H
#define _GINPUT_LLD_MOUSE_BOARD_H

#include "gfx.h"
#include "spi.h"
#include "gpio.h"

// Resolution and Accuracy Settings
#define GMOUSE_ADS7843_PEN_CALIBRATE_ERROR      8
#define GMOUSE_ADS7843_PEN_CLICK_ERROR          6
#define GMOUSE_ADS7843_PEN_MOVE_ERROR           4
#define GMOUSE_ADS7843_FINGER_CALIBRATE_ERROR   14
#define GMOUSE_ADS7843_FINGER_CLICK_ERROR       18
#define GMOUSE_ADS7843_FINGER_MOVE_ERROR        14

#define CTRL_LO_DFR 0b0011
#define CTRL_LO_SER 0b0100
#define CTRL_HI_X 0b1001 << 4
#define CTRL_HI_Y 0b1101 << 4

// How much extra data to allocate at the end of the GMouse structure for the board's use
#define GMOUSE_ADS7843_BOARD_DATA_SIZE          0

static GFXINLINE void aquire_bus(GMouse* m);
static GFXINLINE void release_bus(GMouse* m);

static uint8_t GFXINLINE spi_transferbyte(uint8_t send)
{
    uint8_t b;
    HAL_SPI_TransmitReceive(&hspi2, &send, &b, 1, HAL_MAX_DELAY);
    return b;
}

static GFXINLINE uint16_t read_loop(uint8_t ctrl, uint8_t max_samples)
{
    uint16_t prev = 0xFFFF;
    uint16_t cur = 0xFFFF;
    uint8_t i = 0;

    do {
        prev = cur;
        cur = spi_transferbyte(0);
        cur = (cur << 4) | (spi_transferbyte(ctrl) >> 4);
    } while ((prev != cur) && (++i < max_samples));

    return cur;
}

static GFXINLINE void get_raw(GMouse* m, uint16_t* vi, uint16_t* vj)
{
    uint8_t ctrl = CTRL_LO_DFR;

    spi_transferbyte(CTRL_HI_X | ctrl);
    *vi = read_loop(CTRL_HI_X | ctrl, 0xFF);
    *vj = read_loop(CTRL_HI_Y | ctrl, 0xFF);

    spi_transferbyte(0);
    spi_transferbyte(CTRL_HI_Y | CTRL_LO_SER);
    spi_transferbyte(0);
}

static bool_t init_board(GMouse* m, unsigned driverinstance) {
    (void)m;
    (void)driverinstance;
    uint8_t cmd = 0x80;
    HAL_SPI_Transmit(&hspi2, &cmd, 1, HAL_MAX_DELAY);
    return TRUE;
}

static GFXINLINE bool_t getpin_pressed(GMouse* m) {
    (void)m;

    if (HAL_GPIO_ReadPin(XPT2046_PENIRQ_GPIO_Port, XPT2046_PENIRQ_Pin) == GPIO_PIN_RESET) {
        return TRUE;
    }
    return FALSE;
}

static GFXINLINE void aquire_bus(GMouse* m) {
    HAL_GPIO_WritePin(XPT2046_CS_GPIO_Port, XPT2046_CS_Pin, GPIO_PIN_RESET);
}

static GFXINLINE void release_bus(GMouse* m) {
    HAL_GPIO_WritePin(XPT2046_CS_GPIO_Port, XPT2046_CS_Pin, GPIO_PIN_SET);
}

#endif /* _GINPUT_LLD_MOUSE_BOARD_H */
