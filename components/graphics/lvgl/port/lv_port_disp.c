/**
 * @file lv_port_disp_templ.c
 *
 */

/*Copy this file as "lv_port_disp.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lcd.h"

/*********************
 *      DEFINES
 *********************/
#ifndef MY_DISP_HOR_RES
    #warning Please define or replace the macro MY_DISP_HOR_RES with the actual screen width, default value 320 is used for now.
    #define MY_DISP_HOR_RES    LCD_H
#endif

#ifndef MY_DISP_VER_RES
    #warning Please define or replace the macro MY_DISP_VER_RES with the actual screen height, default value 240 is used for now.
    #define MY_DISP_VER_RES    LCD_W
#endif

#define BYTE_PER_PIXEL (LV_COLOR_FORMAT_GET_SIZE(LV_COLOR_FORMAT_RGB565)) /*will be 2 for RGB565 */

#ifdef CONFIG_PSRAM 
#define DRAW_BUFF_ATTR __attribute__((section(".psram_noinit"), aligned(64)))
#else
#define DRAW_BUFF_ATTR __attribute__((aligned(64)))
#endif

#if (LCD_INTERFACE_TYPE == LCD_INTERFACE_DPI) || (LCD_INTERFACE_TYPE == LCD_INTERFACE_DSI_VIDIO)
/* Triple buffer mode, An additional video memory is required, for better performance */
#define RGB_TRIPLE_BUFF_MODE 1
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void disp_init(void);

static void disp_flush(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map);

// /**********************
//  *  STATIC VARIABLES
//  **********************/

#if (LCD_INTERFACE_TYPE == LCD_INTERFACE_DBI) || (LCD_INTERFACE_TYPE == LCD_INTERFACE_SPI)
/* MCU LCD Common interface */

static uint8_t DRAW_BUFF_ATTR draw_buf_1[MY_DISP_HOR_RES * MY_DISP_VER_RES / 10 * BYTE_PER_PIXEL]; /* A buffer */
static uint8_t DRAW_BUFF_ATTR draw_buf_2[MY_DISP_HOR_RES * MY_DISP_VER_RES / 10 * BYTE_PER_PIXEL]; /* An other buffer */
static volatile lv_display_t *p_disp_drv_cb = NULL;

#elif (LCD_INTERFACE_TYPE == LCD_INTERFACE_DPI) || (LCD_INTERFACE_TYPE == LCD_INTERFACE_DSI_VIDIO)
/* RGB LCD Common interface */

#if 0
static lv_color_t draw_buf_1[LCD_W * LCD_H] ATTR_EALIGN(64); /*A screen sized buffer*/
static lv_color_t draw_buf_2[LCD_W * LCD_H] ATTR_EALIGN(64); /*An other screen sized buffer*/

#if RGB_TRIPLE_BUFF_MODE
static lv_color_t draw_buf_3[LCD_W * LCD_H] ATTR_EALIGN(64); /*An other screen sized buffer*/
#endif

#else
#if defined(CONFIG_PSRAM)
static lv_color_t DRAW_BUFF_ATTR draw_buf_1[LCD_W * LCD_H];
static lv_color_t DRAW_BUFF_ATTR draw_buf_2[LCD_W * LCD_H];
#else
#error "No config psram!"
#endif

// #define LVGL_DRAW_BUF1_BASE (0xA8100000)
// #define LVGL_DRAW_BUF2_BASE (0xA8200000)

// static lv_color_t *draw_buf_1 = (void *)(uintptr_t)LVGL_DRAW_BUF1_BASE;
// static lv_color_t *draw_buf_2 = (void *)(uintptr_t)LVGL_DRAW_BUF2_BASE;

#if RGB_TRIPLE_BUFF_MODE
// #define LVGL_DRAW_BUF3_BASE (0xA8300000)
// static lv_color_t *draw_buf_3 = (void *)(uintptr_t)LVGL_DRAW_BUF3_BASE;
#if defined(CONFIG_PSRAM)
static lv_color_t DRAW_BUFF_ATTR draw_buf_3[LCD_W * LCD_H];
static volatile lv_color_t *last_disp_buff_p = (void *)(uintptr_t)draw_buf_3;
static volatile lv_color_t *last_lvgl_flush_p = NULL;
#else
#error "No config psram!"
#endif

#else
static volatile uint8_t swap_flag = 0;
#endif

#endif

#endif

// /* Descriptor of a display buffer */
// static volatile lv_disp_draw_buf_t draw_buf_dsc;

/* Descriptor of a display driver */
lv_display_t* disp_driver = NULL;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_disp_init(void)
{
    /*-------------------------
     * Initialize your display
     * -----------------------*/
    disp_init();

    /*------------------------------------
     * Create a display and set a flush_cb
     * -----------------------------------*/
    disp_driver = lv_display_create(MY_DISP_HOR_RES, MY_DISP_VER_RES);
    lv_display_set_flush_cb(disp_driver, disp_flush);

    // lv_display_set_rotation(disp_driver, LV_DISPLAY_ROTATION_90);

    // /* Example 1
    //  * One buffer for partial rendering*/
    // LV_ATTRIBUTE_MEM_ALIGN
    // static uint8_t buf_1_1[MY_DISP_HOR_RES * 10 * BYTE_PER_PIXEL];            /*A buffer for 10 rows*/
    // lv_display_set_buffers(disp, buf_1_1, NULL, sizeof(buf_1_1), LV_DISPLAY_RENDER_MODE_PARTIAL);

    // /* Example 2
    //  * Two buffers for partial rendering
    //  * In flush_cb DMA or similar hardware should be used to update the display in the background.*/
    // LV_ATTRIBUTE_MEM_ALIGN
    // static uint8_t buf_2_1[MY_DISP_HOR_RES * 10 * BYTE_PER_PIXEL];

    // LV_ATTRIBUTE_MEM_ALIGN
    // static uint8_t buf_2_2[MY_DISP_HOR_RES * 10 * BYTE_PER_PIXEL];
    // lv_display_set_buffers(disp, buf_2_1, buf_2_2, sizeof(buf_2_1), LV_DISPLAY_RENDER_MODE_PARTIAL);

    // /* Example 3
    //  * Two buffers screen sized buffer for double buffering.
    //  * Both LV_DISPLAY_RENDER_MODE_DIRECT and LV_DISPLAY_RENDER_MODE_FULL works, see their comments*/
    // LV_ATTRIBUTE_MEM_ALIGN
    // static uint8_t buf_3_1[MY_DISP_HOR_RES * MY_DISP_VER_RES * BYTE_PER_PIXEL];

    // LV_ATTRIBUTE_MEM_ALIGN
    // static uint8_t buf_3_2[MY_DISP_HOR_RES * MY_DISP_VER_RES * BYTE_PER_PIXEL];
    // lv_display_set_buffers(disp, buf_3_1, buf_3_2, sizeof(buf_3_1), LV_DISPLAY_RENDER_MODE_DIRECT);


/* MCU LCD Common interface */
#if (LCD_INTERFACE_TYPE == LCD_INTERFACE_DBI) || (LCD_INTERFACE_TYPE == LCD_INTERFACE_SPI)

    lv_display_set_buffers(disp_driver, draw_buf_1, draw_buf_2, sizeof(draw_buf_2), LV_DISPLAY_RENDER_MODE_PARTIAL);
    // lv_disp_draw_buf_init((lv_disp_draw_buf_t *)&draw_buf_dsc, draw_buf_1, draw_buf_2, LCD_W * LCD_H / 8); /*Initialize the display buffer*/

/* RGB LCD Common interface,  */
#elif (LCD_INTERFACE_TYPE == LCD_INTERFACE_DPI) || (LCD_INTERFACE_TYPE == LCD_INTERFACE_DSI_VIDIO)

    lv_disp_draw_buf_init((lv_disp_draw_buf_t *)&draw_buf_dsc, draw_buf_1, draw_buf_2, LCD_W * LCD_H); /*Initialize the display buffer*/

#endif
    /*-----------------------------------
     * Register the display in LVGL
     *----------------------------------*/

//     lv_disp_drv_init(&disp_drv_dsc); /*Basic initialization*/

//     /*Set up the functions to access to your display*/

//     /*Set the resolution of the display*/
//     disp_drv_dsc.hor_res = LCD_W;
//     disp_drv_dsc.ver_res = LCD_H;

//     /* hardware rotation */
// /* MCU LCD Common interface */
// #if (LCD_INTERFACE_TYPE == LCD_INTERFACE_DBI) || (LCD_INTERFACE_TYPE == LCD_INTERFACE_SPI)
//     disp_drv_dsc.sw_rotate = 0;

// /* RGB LCD Common interface,  */
// #elif (LCD_INTERFACE_TYPE == LCD_INTERFACE_DPI) || (LCD_INTERFACE_TYPE == LCD_INTERFACE_DSI_VIDIO)
//     disp_drv_dsc.sw_rotate = 1;
//     disp_drv_dsc.direct_mode = 1;
//     disp_drv_dsc.full_refresh = 1;
// #endif

//     /*  rotation */
//     disp_drv_dsc.rotated = LV_DISP_ROT_NONE;

//     /*Used to copy the buffer's content to the display*/
//     disp_drv_dsc.flush_cb = disp_flush;

//     /*Set a display buffer*/
//     disp_drv_dsc.draw_buf = (lv_disp_draw_buf_t *)&draw_buf_dsc;

//     /*Finally register the driver*/
//     lv_disp_drv_register(&disp_drv_dsc);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
/* MCU LCD Common interface */
#if (LCD_INTERFACE_TYPE == LCD_INTERFACE_DBI) || (LCD_INTERFACE_TYPE == LCD_INTERFACE_SPI)
void flush_async_callback(void)
{
    if (p_disp_drv_cb != NULL) {
        /*IMPORTANT!!!
        *Inform the graphics library that you are ready with the flushing*/
        lv_display_flush_ready((lv_display_t *)p_disp_drv_cb);
        p_disp_drv_cb = NULL;
    }
}
/* RGB LCD Common interface,  */
#elif (LCD_INTERFACE_TYPE == LCD_INTERFACE_DPI) || (LCD_INTERFACE_TYPE == LCD_INTERFACE_DSI_VIDIO)

void rgb_screen_frame_callback(void)
{
/* Triple buffer mode */
#if RGB_TRIPLE_BUFF_MODE

    lcd_color_t *screen_using = lcd_get_screen_using();
    if (screen_using != (lcd_color_t *)last_disp_buff_p) {
        if (last_lvgl_flush_p == draw_buf_dsc.buf1) {
            draw_buf_dsc.buf1 = (void *)last_disp_buff_p;
            last_disp_buff_p = (lv_color_t *)screen_using;
        } else if (last_lvgl_flush_p == draw_buf_dsc.buf2) {
            draw_buf_dsc.buf2 = (void *)last_disp_buff_p;
            last_disp_buff_p = (lv_color_t *)screen_using;
        }
    }

#else
    swap_flag = 0;
#endif
}

#endif

/* Initialize your display and the required peripherals. */
/* MCU LCD Common interface */
#if (LCD_INTERFACE_TYPE == LCD_INTERFACE_DBI) || (LCD_INTERFACE_TYPE == LCD_INTERFACE_SPI)

void disp_init(void)
{
    lcd_init();
    lcd_async_callback_register(flush_async_callback);
    lcd_set_dir(1, 0);
    lcd_clear(LCD_COLOR_RGB(0x00, 0X00, 0X00));
}

/* RGB LCD Common interface,  */
#elif (LCD_INTERFACE_TYPE == LCD_INTERFACE_DPI) || (LCD_INTERFACE_TYPE == LCD_INTERFACE_DSI_VIDIO)

void disp_init(void)
{
    /* init MIPI-DPI */
#if RGB_TRIPLE_BUFF_MODE
    lcd_clear((lcd_color_t *)draw_buf_3, LCD_COLOR_RGB(0x00, 0x00, 0x00));
    lcd_init((lcd_color_t *)draw_buf_3);
    lcd_frame_callback_register(FRAME_INT_TYPE_SWAP, rgb_screen_frame_callback);
#else
    lcd_clear((lcd_color_t *)draw_buf_1, LCD_COLOR_RGB(0x00, 0x00, 0x00));
    lcd_init((lcd_color_t *)draw_buf_1);
    lcd_frame_callback_register(FRAME_INT_TYPE_CYCLE, rgb_screen_frame_callback);
#endif
}

#endif


volatile bool disp_flush_enabled = true;

/* Enable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_enable_update(void)
{
    disp_flush_enabled = true;
}

/* Disable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_disable_update(void)
{
    disp_flush_enabled = false;
}


/* Flush the content of the internal buffer the specific area on the display
 * You can use DMA or any hardware acceleration to do this operation in the background but
 * 'lv_disp_flush_ready()' has to be called when finished. */
/* MCU LCD Common interface */
#if (LCD_INTERFACE_TYPE == LCD_INTERFACE_DBI) || (LCD_INTERFACE_TYPE == LCD_INTERFACE_SPI)

static void disp_flush(lv_display_t * disp_drv, const lv_area_t * area, uint8_t * px_map)
{
    if(disp_flush_enabled) {
        static uint8_t rotated_dir = 0;
        lv_display_rotation_t rotation = lv_display_get_rotation(disp_drv);
        if (rotated_dir != rotation) {
            rotated_dir = rotation;
            lcd_set_dir(rotated_dir, 0);
        }
        p_disp_drv_cb = disp_drv;
        lcd_draw_picture_nonblocking(area->x1, area->y1, area->x2, area->y2, (lcd_color_t *)px_map);
    }
}

/* RGB LCD Common interface,  */
#elif (LCD_INTERFACE_TYPE == LCD_INTERFACE_DPI) || (LCD_INTERFACE_TYPE == LCD_INTERFACE_DSI_VIDIO)

static void disp_flush(lv_display_t * disp_drv, const lv_area_t * area, uint8_t * px_map)
{
    /* Triple buffer mode */
#if RGB_TRIPLE_BUFF_MODE
    lcd_screen_switch((lcd_color_t *)color_p);
    last_lvgl_flush_p = color_p;

#else
    lcd_screen_switch((lcd_color_t *)color_p);
    swap_flag = 1;
    while (swap_flag) {
        __NOP();
        __NOP();
        __NOP();
        __NOP();
    }

#endif

    lv_disp_flush_ready(disp_drv);
}

#endif

#else /* Enable this file at the top */

/* This dummy typedef exists purely to silence -Wpedantic. */
typedef int keep_pedantic_happy;
#endif
