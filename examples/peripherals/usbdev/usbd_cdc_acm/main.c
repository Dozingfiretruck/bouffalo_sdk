#include <FreeRTOS.h>
#include "semphr.h"
#include "usbd_core.h"
#include "board.h"

#define DBG_TAG "MAIN"
#include "log.h"

extern void cdc_acm_init(uint8_t busid, uintptr_t reg_base);
extern void cdc_acm_data_send_with_dtr_test(void);

static TaskHandle_t usb_handle;

static void usb_task(void *pvParameters)
{

    LOG_I("Starting usb host task...\r\n");

    cdc_acm_init(0, 0x20072000);
    while (1) {
        cdc_acm_data_send_with_dtr_test();
        bflb_mtimer_delay_ms(500);
    }

    vTaskDelete(NULL);
}

int main(void)
{
    board_init();

    xTaskCreate(usb_task, (char *)"usb_task", 512, NULL, configMAX_PRIORITIES - 3, &usb_handle);

    vTaskStartScheduler();
    while (1) {
    }
}
