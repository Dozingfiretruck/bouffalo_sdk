/**
 * @file main.c
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
 *
 */
#include <FreeRTOS.h>
#include "semphr.h"
#include "board.h"
#include "bflb_gpio.h"
#include "bflb_l1c.h"
#include "bflb_mtimer.h"

#include "lv_conf.h"
#include "lvgl.h"

#include "lv_port_disp.h"
#include "lv_port_indev.h"

#include "demos/lv_demos.h"

#include "lcd.h"

static TaskHandle_t lvgl_handle;

static void lvgl_task(void *pvParameters)
{
    printf("lvgl case\r\n");

    /* lvgl init */
    lv_init();

    lv_tick_set_cb(xTaskGetTickCount);

    lv_delay_set_cb(vTaskDelay);

    lv_port_disp_init();
    // lv_port_indev_init();

    lv_demo_benchmark();
    // lv_demo_stress();

    lv_task_handler();

    printf("lvgl success\r\n");

    while (1) {
        uint32_t time_till_next = lv_timer_handler();
        vTaskDelay(time_till_next);
    }

    vTaskDelete(NULL);
}

int main(void)
{
    board_init();

    xTaskCreate(lvgl_task, (char *)"lvgl_task", 4096 * 2, NULL, configMAX_PRIORITIES, &lvgl_handle);

    vTaskStartScheduler();
    while (1) {
    }
}
