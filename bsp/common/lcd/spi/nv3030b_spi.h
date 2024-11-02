/**
 * @file nv3030b_spi.h
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

#ifndef _NV3030B_SPI_H_
#define _NV3030B_SPI_H_

#include "../lcd_conf.h"

#if defined LCD_SPI_NV3030B

/* Do not modify the following */

#define NV3030B_SPI_COLOR_DEPTH 16

typedef struct {
    uint8_t cmd; /* 0xFF : delay(databytes)ms */
    const char *data;
    uint8_t databytes; /* Num of data in data; or delay time */
} nv3030b_spi_init_cmd_t;

typedef uint16_t nv3030b_spi_color_t;

int nv3030b_spi_init();
void nv3030b_spi_async_callback_enable(bool enable);
void nv3030b_spi_async_callback_register(void (*callback)(void));
int nv3030b_spi_set_dir(uint8_t dir, uint8_t mir_flag);
void nv3030b_spi_set_draw_window(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void nv3030b_spi_draw_point(uint16_t x, uint16_t y, nv3030b_spi_color_t color);
void nv3030b_spi_draw_area(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, nv3030b_spi_color_t color);
void nv3030b_spi_draw_picture_nonblocking(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, nv3030b_spi_color_t *picture);
void nv3030b_spi_draw_picture_blocking(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, nv3030b_spi_color_t *picture);
int nv3030b_spi_draw_is_busy(void);

#endif
#endif