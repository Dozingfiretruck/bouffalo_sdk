/**
 * @file nv3030b_spi.c
 * @brief
 *
 * Copyright (c) 2021 Bouffalolab team
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 */

#include "../lcd.h"

#if defined(LCD_SPI_NV3030B)

#if (LCD_SPI_INTERFACE_TYPE == 1)
#include "bl_spi_hard_4.h"

#define lcd_spi_init                          lcd_spi_hard_4_init
#define lcd_spi_isbusy                        lcd_spi_hard_4_is_busy

#define lcd_spi_transmit_cmd_para             lcd_spi_hard_4_transmit_cmd_para
#define lcd_spi_transmit_cmd_pixel_sync       lcd_spi_hard_4_transmit_cmd_pixel_sync
#define lcd_spi_transmit_cmd_pixel_fill_sync  lcd_spi_hard_4_transmit_cmd_pixel_fill_sync

#define lcd_spi_sync_callback_enable          lcd_spi_hard_4_async_callback_enable
#define lcd_spi_async_callback_register       lcd_spi_hard_4_async_callback_register
#define lcd_spi_transmit_cmd_pixel_async      lcd_spi_hard_4_transmit_cmd_pixel_async
#define lcd_spi_transmit_cmd_pixel_fill_async lcd_spi_hard_4_transmit_cmd_pixel_fill_async

static lcd_spi_hard_4_init_t spi_para = {
    .clock_freq = 80 * 1000 * 1000,
#if (NV3030B_SPI_PIXEL_FORMAT == 1)
    .pixel_format = LCD_SPI_LCD_PIXEL_FORMAT_RGB565,
#elif (NV3030B_SPI_PIXEL_FORMAT == 2)
    .pixel_format = LCD_SPI_LCD_PIXEL_FORMAT_NRGB8888,
#endif
};

#else

#error "Configuration error"

#endif
const nv3030b_spi_init_cmd_t nv3030b_spi_init_cmds[] = {
    { 0x01, NULL, 0 },
    { 0xFF, NULL, 10 },
    { 0x11, NULL, 0 }, /* Exit sleep */
    { 0xFF, NULL, 120 },

    { 0x3A, "\x05", 1 },

    { 0xFD, "\x06\x08", 2 },//private_access
    { 0x61, "\x07\x04", 2 },//add
    { 0x62, "\x00\x44\x45", 3 },//bias setting
    { 0x63, "\x41\x07\x12\x12", 4 },
    { 0x64, "\x37", 1 },
    { 0x65, "\x09\x10\x21", 3 },
    { 0x66, "\x09\x10\x21", 3 },
    { 0x67, "\x20\x40", 2 },
    { 0x68, "\x90\x4C\x7C\x66", 4 },
    { 0xB1, "\x0F\x02\x01", 3 },
    { 0xB4, "\x01", 1 },
    { 0xB5, "\x02\x02\x0A\x14", 4 },
    { 0xB6, "\x04\x01\x9F\x00\x02", 5 },
    { 0xDF, "\x11", 1 },
    { 0xE0, "\x01\x03\x0D\x0E\x0E\x0C\x15\x19", 8 },
    { 0xE1, "\x00\x57", 2 },
    { 0xE2, "\x13\x00\x00\x30\x33\x3F", 6 },
    { 0xE3, "\x1A\x16\x0C\x0F\x0E\x0D\x02\x01", 8 },
    { 0xE4, "\x58\x00", 2 },
    { 0xE5, "\x3F\x33\x30\x00\x00\x13", 6 },
    { 0xE6, "\x00\xFF", 2 },
    { 0xE7, "\x01\x04\x03\x03\x00\x12", 6 },
    { 0xE8, "\x00\x70\x00", 3 },
    { 0xEC, "\x52", 1 },
    { 0xF1, "\x01\x01\x02", 3 },
    { 0xF6, "\x09\x10\x00\x00", 4 },
    { 0xFD, "\xFA\xFC", 2 },
    
    { 0x21, NULL, 0 },
    { 0x29, NULL, 0 },
    { 0x36, "\x00", 1 },
};

/**
 * @brief nv3030b_spi_async_callback_enable
 *
 * @return
 */
void nv3030b_spi_async_callback_enable(bool enable)
{
    lcd_spi_sync_callback_enable(enable);
}

/**
 * @brief nv3030b_spi_async_callback_register
 *
 * @return
 */
void nv3030b_spi_async_callback_register(void (*callback)(void))
{
    lcd_spi_async_callback_register(callback);
}

/**
 * @brief nv3030b_spi_draw_is_busy, After the call nv3030b_spi_draw_picture_dma must check this,
 *         if nv3030b_spi_draw_is_busy() == 1, Don't allow other draw !!
 *         can run in the DMA interrupt callback function.
 *
 * @return int 0:draw end; 1:Being draw
 */
int nv3030b_spi_draw_is_busy(void)
{
    return lcd_spi_isbusy();
}

/**
 * @brief nv3030b_spi_init
 *
 * @return int
 */
int nv3030b_spi_init()
{
    lcd_spi_init(&spi_para);

    for (uint16_t i = 0; i < (sizeof(nv3030b_spi_init_cmds) / sizeof(nv3030b_spi_init_cmds[0])); i++) {
        if (nv3030b_spi_init_cmds[i].cmd == 0xFF && nv3030b_spi_init_cmds[i].data == NULL && nv3030b_spi_init_cmds[i].databytes) {
            bflb_mtimer_delay_ms(nv3030b_spi_init_cmds[i].databytes);
        } else {
            lcd_spi_transmit_cmd_para(nv3030b_spi_init_cmds[i].cmd, (void *)(nv3030b_spi_init_cmds[i].data), nv3030b_spi_init_cmds[i].databytes);
        }
    }

    return 0;
}

/**
 * @brief
 *
 * @param dir
 * @param mir_flag
 */
int nv3030b_spi_set_dir(uint8_t dir, uint8_t mir_flag)
{
    uint8_t param;

    switch (dir) {
        case 0:
            if (!mir_flag)
                param = 0x08;
            else
                param = 0x40;
            break;
        case 1:
            if (!mir_flag)
                param = 0x78;
            else
                param = 0xA0;
            break;
        case 2:
            if (!mir_flag)
                param = 0xC8;
            else
                param = 0xC0;
            break;
        case 3:
            if (!mir_flag)
                param = 0xA8;
            else
                param = 0x60;

            break;
        default:
            return -1;
            break;
    }

    lcd_spi_transmit_cmd_para(0x36, (void *)&param, 1);

    return dir;
}

/**
 * @brief nv3030b_spi_set_draw_window
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 */
void nv3030b_spi_set_draw_window(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
#if NV3030B_SPI_OFFSET_X
    x1 += NV3030B_SPI_OFFSET_X;
    x2 += NV3030B_SPI_OFFSET_X;
#endif
#if NV3030B_SPI_OFFSET_Y
    y1 += NV3030B_SPI_OFFSET_Y;
    y2 += NV3030B_SPI_OFFSET_Y;
#endif

    int8_t param[4];

    param[0] = (x1 >> 8) & 0xFF;
    param[1] = x1 & 0xFF;
    param[2] = (x2 >> 8) & 0xFF;
    param[3] = x2 & 0xFF;

    lcd_spi_transmit_cmd_para(0x2A, (void *)param, 4);

    param[0] = (y1 >> 8) & 0xFF;
    param[1] = y1 & 0xFF;
    param[2] = (y2 >> 8) & 0xFF;
    param[3] = y2 & 0xFF;

    lcd_spi_transmit_cmd_para(0x2B, (void *)param, 4);
}

/**
 * @brief nv3030b_spi_draw_point
 *
 * @param x
 * @param y
 * @param color
 */
void nv3030b_spi_draw_point(uint16_t x, uint16_t y, nv3030b_spi_color_t color)
{
    /* set window */
    nv3030b_spi_set_draw_window(x, y, x, y);

    lcd_spi_transmit_cmd_pixel_sync(0x2C, (void *)&color, 1);
}

/**
 * @brief nv3030b_draw_area
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param color
 */
void nv3030b_spi_draw_area(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, nv3030b_spi_color_t color)
{
    uint32_t pixel_cnt = (x2 - x1 + 1) * (y2 - y1 + 1);

    /* set window */
    nv3030b_spi_set_draw_window(x1, y1, x2, y2);

    lcd_spi_transmit_cmd_pixel_fill_sync(0x2C, (uint32_t)color, pixel_cnt);
}

/**
 * @brief nv3030b_draw_picture_dma, Non-blocking! Using DMA acceleration, Not waiting for the draw end
 *  After the call, No other operations are allowed until (nv3030b_draw_is_busy()==0)
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param picture
 */
void nv3030b_spi_draw_picture_nonblocking(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, nv3030b_spi_color_t *picture)
{
    size_t pixel_cnt = (x2 - x1 + 1) * (y2 - y1 + 1);

    /* set window */
    nv3030b_spi_set_draw_window(x1, y1, x2, y2);

    lcd_spi_transmit_cmd_pixel_async(0x2C, (void *)picture, pixel_cnt);
}

/**
 * @brief nv3030b_draw_picture,Blocking,Using DMA acceleration,Waiting for the draw end
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param picture
 */
void nv3030b_spi_draw_picture_blocking(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, nv3030b_spi_color_t *picture)
{
    size_t pixel_cnt = (x2 - x1 + 1) * (y2 - y1 + 1);

    /* set window */
    nv3030b_spi_set_draw_window(x1, y1, x2, y2);

    lcd_spi_transmit_cmd_pixel_sync(0x2C, (void *)picture, pixel_cnt);
}

#endif
