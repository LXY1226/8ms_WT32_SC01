
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "driver/ledc.h"

#include "common.h"
#include "LCD.h"

#define LOG_TAG "LCD"

#define LCD_RST (22) // GPIO
#define LCD_CS (15)
#define LCD_DCX (21)
#define LCD_SCL (14)
#define LCD_SDA (13)

#define LCD_BACKLIGHT_GPIO (23)
#define LCD_BACKLIGHT_FADE_TIME (500)
#define LCD_BACKLIGHT_MAX_DUTY (8191)
#define LCD_BACKLIGHT_SPEED_MODE LEDC_HIGH_SPEED_MODE
#define LCD_BACKLIGHT_CHANNEL LEDC_CHANNEL_0

#define LCD_buf (LCD_H*LCD_W * 2)
#define PARALLEL_BUFF_16 LCD_W * LCD_H / 16 // 320/16=20
#define PARALLEL_BUFF_8 PARALLEL_BUFF_16 * 2

uint16_t *frameBuf;

spi_device_handle_t LCD_handle;


void LCD_data(const uint8_t *data, int len) {
    if (len == 0) return;             //no need to send anything
//    printf("D: ");
//    for (int i = 0; i < len; ++i) {
//        printf("%02x", *(data + i));
//    }
//    printf("\n");
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));       //Zero out the transaction
    t.length = len * 8;                 //Len is in bytes, transaction length is in bits.
    t.tx_buffer = data;               //Data
    //t.user = (void *) 1;                //D/C needs to be set to 1
    gpio_set_level(LCD_DCX, 1);
    ESP_ERROR_CHECK(spi_device_transmit(LCD_handle, &t));  //Transmit!
}


void LCD_cmd(const uint8_t cmd, const void *data, int datalen) {
    gpio_set_level(LCD_DCX, 0);
    ESP_ERROR_CHECK(spi_device_polling_transmit(LCD_handle, &(spi_transaction_t) {
            .length = 8,
            .tx_buffer = &cmd,
    }));
    if (datalen > 0) {
        gpio_set_level(LCD_DCX, 1);
        ESP_ERROR_CHECK(spi_device_polling_transmit(LCD_handle, &(spi_transaction_t) {
                .length = datalen * 8,
                .tx_buffer = data,
        }));
    }
}

void LCD_setWindow(LCD_Point from, LCD_Point to) {
    to.x--;
    LCD_cmd(0x2a, (const uint8_t *) &(uint8_t[]) {from.x >> 8, from.x, to.x >> 8, to.x}, 4);
    to.y--;
    LCD_cmd(0x2b, (const uint8_t *) &(uint8_t[]) {from.y >> 8, from.y, to.y >> 8, to.y}, 4);
}

//#include "test_img.h"
//
//typedef struct imghead {
//    unsigned char scan;
//    unsigned char gray;
//    unsigned short w;
//    unsigned short h;
//    unsigned char is565;
//    unsigned char rgb;
//} IMG_HEAD;
//
//void test_img() {
//    IMG_HEAD *ihead = (IMG_HEAD *) &gImage_test_img;
//    int size = ihead->w * ihead->h * 2 - PARALLEL_BUFF_8;
//    //printf("%d %d %d\n", ihead->w, ihead->h, ihead->w*ihead->h);
//    LCD_setWindow((LCD_Point) {0, 0}, (LCD_Point) {ihead->w, ihead->h});
//    LCD_cmd(0x2c, 0, 0);
//    spi_transaction_t t;
//    memset(&t, 0, sizeof(t));
//    uint16_t *buff = heap_caps_malloc(PARALLEL_BUFF_8, MALLOC_CAP_DMA);
//    t.length = PARALLEL_BUFF_8 * 8;
//    gpio_set_level(LCD_DCX, 1);
//    int i = sizeof(IMG_HEAD);
//    for (; i < size; i += PARALLEL_BUFF_8) {
//        printf("%d %d %d %d\n", i, ihead->w, ihead->h, size);
//        t.tx_buffer = buff;
//        memcpy(buff, &gImage_test_img[i], PARALLEL_BUFF_8);
//        ESP_ERROR_CHECK(spi_device_polling_transmit(LCD_handle, &t));
//        //spi_device_polling_transmit()
//    }
//    //i -= PARALLEL_BUFF_8;
//    memcpy(buff, &gImage_test_img[i], ihead->w * ihead->h * 2 - i);
//    t.length = (ihead->w * ihead->h * 2 - i) * 8;
//    printf("%d %d %d %d\n", i, ihead->w, ihead->h, ihead->w * ihead->h * 2 - i);
//    ESP_ERROR_CHECK(spi_device_transmit(LCD_handle, &t));
//    heap_caps_free(buff);
//}


void LCD_draw(LCD_Point from, LCD_Point to, const uint16_t *bitmap, int len) {
    LCD_setWindow(from, to);
    LCD_cmd(0x2c, 0, 0);
    gpio_set_level(LCD_DCX, 1);
    ESP_ERROR_CHECK(spi_device_polling_transmit(LCD_handle, &(spi_transaction_t) {
            .length = len * 8,
            .tx_buffer = bitmap,
    }));
}

void LCD_fill(uint16_t color) {
//    ESP_LOGI(LOG_TAG, "Fresh");
    LCD_setWindow((LCD_Point) {0, 0}, (LCD_Point) {LCD_W, LCD_H});
    LCD_cmd(0x2c, 0, 0);
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    uint16_t *buff = heap_caps_malloc(PARALLEL_BUFF_8, MALLOC_CAP_DMA);
    //uint16_t buff[PARALLEL_BUFF_16];
    if ((color & 0xff00) == (color & 0x00ff)) // Simple color
        memset(buff, color, PARALLEL_BUFF_8);
    else
        for (int i = 0; i < PARALLEL_BUFF_16; i++)
            buff[i] = color;
    t.length = PARALLEL_BUFF_8 * 8;
    t.tx_buffer = (void *) buff;
    gpio_set_level(LCD_DCX, 1);
    for (int i = 0; i < 16; i++)
        spi_device_transmit(LCD_handle, &t);
    heap_caps_free(buff);
}

//void LCD_fresh() {
//    gpio_set_level(LCD_DCX, 1);
//    spi_transaction_t t;
//    memset(&t, 0, sizeof(t));
//    uint8_t *buff1, *buff2;
//    buff1 = heap_caps_malloc(LCD_W, MALLOC_CAP_DMA);
//    buff2 = heap_caps_malloc(LCD_W, MALLOC_CAP_DMA);
//    t.tx_buffer = buff1;
//    t.length = LCD_W;
//    memcpy(frameBuf, buff1, LCD_W);
//    ESP_ERROR_CHECK(spi_device_polling_start(LCD_handle, &t, portMAX_DELAY));
//    for (int i = LCD_W; i < LCD_buf; i += LCD_W * 2) {
//        memcpy(frameBuf + i, buff2, LCD_W);
//        ESP_ERROR_CHECK(spi_device_polling_end(LCD_handle, portMAX_DELAY));
//        t.tx_buffer = buff2;
//        ESP_ERROR_CHECK(spi_device_polling_start(LCD_handle, &t, portMAX_DELAY));
//        memcpy(frameBuf, buff1, LCD_W);
//        ESP_ERROR_CHECK(spi_device_polling_end(LCD_handle, portMAX_DELAY));
//        t.tx_buffer = buff1;
//        ESP_ERROR_CHECK(spi_device_polling_start(LCD_handle, &t, portMAX_DELAY));
//    }
//    ESP_ERROR_CHECK(spi_device_polling_end(LCD_handle, portMAX_DELAY));
//    free(buff1);
//    free(buff2);
//}


void LCD_init() {
    ESP_LOGI(LOG_TAG, "Initalizing ST7796... at 40Mhz SPI");
    gpio_set_direction(LCD_RST, GPIO_MODE_OUTPUT);
    gpio_set_direction(LCD_DCX, GPIO_MODE_OUTPUT);
    //gpio_set_direction(LCD_CS, GPIO_MODE_OUTPUT);
    esp_err_t ret;
    spi_bus_config_t buscfg = {
            .mosi_io_num = LCD_SDA,
            .sclk_io_num = LCD_SCL,
            .max_transfer_sz = PARALLEL_BUFF_8/* + 8*/};

    spi_device_interface_config_t devcfg = {
            .clock_speed_hz = 27000000,
            .spics_io_num = LCD_CS,  //CS pin
            .queue_size = 16,         //We want to be able to queue 7 transactions at a time
            //.pre_cb = LCD_pre_dcset, //Specify pre-transfer callback to handle D/C line
            .flags=SPI_DEVICE_HALFDUPLEX//|SPI_DEVICE_TXBIT_LSBFIRST
    };
    //ESP_LOGI(LOG_TAG, "0x%x, 0x%x, 0x%x", SPI_MOSI_DLEN_REG(2), SPI_W0_REG(2), SPI_CMD_REG(2));
    ret = spi_bus_initialize(HSPI_HOST, &buscfg, 1);
    ESP_ERROR_CHECK(ret);
    ret = spi_bus_add_device(HSPI_HOST, &devcfg, &LCD_handle);
    ESP_ERROR_CHECK(ret);
    gpio_set_level(LCD_RST, 0);
//    delay(20);
    gpio_set_level(LCD_RST, 1);
    delay(100);
//    LCD_cmd(0x11, 0, 0); // Sleep out
//    delay(120);
//    LCD_cmd(0xf0, &(uint8_t[]) {0xc3}, 1);
//    LCD_cmd(0xf0, &(uint8_t[]) {0x96}, 1);
    LCD_cmd(0x36, &(uint8_t[]) {LCD_MAD_DEFAULT}, 1);
    /* Interface Pixel Format, 16bits/pixel for RGB/MCU interface */
    LCD_cmd(0x3a, &(uint8_t[]) {0x55}, 1);
    /* Porch Setting */
//    LCD_cmd(0xb2, &(uint8_t[]) {0x0c, 0x0c, 0x00, 0x33, 0x33}, 5);
    /* Gate Control, Vgh=13.65V, Vgl=-10.43V */
//    LCD_cmd(0xb7, &(uint8_t[]) {0x45}, 1);
    /* VCOM Setting, VCOM=1.175V */
//    LCD_cmd(0xBB, &(uint8_t[]) {0x2B}, 1);
    /* LCM Control, XOR: BGR, MX, MH */
//    LCD_cmd(0xC0, &(uint8_t[]) {0x2C}, 1);
    /* VDV and VRH Command Enable, enable=1 */
//    LCD_cmd(0xBB, &(uint8_t[]) {0x2B}, 1);
    /* LCM Control, XOR: BGR, MX, MH */
//    LCD_cmd(0xC0, &(uint8_t[]) {0x2C}, 1);
    /* VDV and VRH Command Enable, enable=1 */
//    LCD_cmd(0xC2, &(uint8_t[]) {0x01, 0xff}, 2);
    /* VRH Set, Vap=4.4+... */
//    LCD_cmd(0xC3, &(uint8_t[]) {0x11}, 1);
    /* VDV Set, VDV=0 */
//    LCD_cmd(0xC4, &(uint8_t[]) {0x20}, 1);
    /* Frame Rate Control, 60Hz, inversion=0 */
//    LCD_cmd(0xB1, &(uint8_t[]) {0xA0, 0x00}, 2);
    /* Power Control 1, AVDD=6.8V, AVCL=-4.8V, VDDS=2.3V */
//    LCD_cmd(0xD0, &(uint8_t[]) {0xA4, 0xA1}, 2);
    /* Positive Voltage Gamma Control */
//    LCD_cmd(0xE0, &(uint8_t[]) {0xD0, 0x00, 0x05, 0x0E, 0x15, 0x0D, 0x37, 0x43, 0x47, 0x09, 0x15, 0x12, 0x16, 0x19}, 14);
    /* Negative Voltage Gamma Control */
//    LCD_cmd(0xE0, &(uint8_t[]) {0xD0, 0x00, 0x05, 0x0D, 0x0C, 0x06, 0x2D, 0x44, 0x40, 0x0E, 0x1C, 0x18, 0x16, 0x19}, 14);
//    LCD_cmd(0xf0, &(uint8_t[]) {0x3C}, 1);
//    LCD_cmd(0xf0, &(uint8_t[]) {0x69}, 1);
//    delay(120);
//    delay(120);
    /* Display On */
//    LCD_cmd(0xf0, &(uint8_t[]) {0xc3}, 1);
//    LCD_cmd(0xf0, &(uint8_t[]) {0x96}, 1);
//    LCD_cmd(0x3a, &(uint8_t[]) {0x55}, 1);
    /* Column inversion: 1-dot*/
    LCD_cmd(0xb4, &(uint8_t[]) {0x01}, 1);
//    LCD_cmd(0xb6, &(uint8_t[]) {0x80, 0x02, 0x3b}, 3);
//    LCD_cmd(0xe8, &(uint8_t[]) {0x40, 0x8a, 0x00, 0x00, 0x29, 0x19, 0xa5, 0x33}, 8);
//    LCD_cmd(0xc1, &(uint8_t[]) {0x06}, 1);
//    LCD_cmd(0xc2, &(uint8_t[]) {0xa7}, 1);
//    LCD_cmd(0xc5, &(uint8_t[]) {0x18}, 1);
//    delay(120);
//    LCD_cmd(0xe0, &(uint8_t[]) {0xF0, 0x09, 0x0b, 0x06, 0x04, 0x15, 0x2F, 0x54, 0x42, 0x3C, 0x17, 0x14, 0x18, 0x1B},
//            14);
//    LCD_cmd(0xe1, &(uint8_t[]) {0xE0, 0x09, 0x0B, 0x06, 0x04, 0x03, 0x2B, 0x43, 0x42, 0x3B, 0x16, 0x14, 0x17, 0x1B},
//            14);
//    LCD_cmd(0xf0, &(uint8_t[]) {0x3C}, 1);
//    LCD_cmd(0xf0, &(uint8_t[]) {0x69}, 1);
//    delay(120);
    /* Sleep Out */
    LCD_cmd(0x11, 0,0);
    LCD_fill(LCD_K);
    LCD_cmd(0x29, 0, 0); //Display on
    delay(70); // wait to fill
    ledc_timer_config_t ledc_timer = {
            .duty_resolution = LEDC_TIMER_13_BIT,
            .freq_hz = 5000,
            .speed_mode = LCD_BACKLIGHT_SPEED_MODE,
            .timer_num = LEDC_TIMER_0,
            .clk_cfg = LEDC_AUTO_CLK,
    };
    ledc_timer_config(&ledc_timer);
    ledc_channel_config_t ledc_channel = {
            .channel    = LCD_BACKLIGHT_CHANNEL,
            .duty       = 16 << 5,
            .gpio_num   = LCD_BACKLIGHT_GPIO,
            .speed_mode = LCD_BACKLIGHT_SPEED_MODE,
            .hpoint     = 0,
            .timer_sel  = LEDC_TIMER_0
    };
    ledc_channel_config(&ledc_channel);
    ledc_fade_func_install(0);
    //test_img();
//    delay(20000);
//    while (true) {
//        LCD_fill(LCD_R);
//        delay(3000);
//        LCD_fill(LCD_G);
//        delay(3000);
//        LCD_fill(LCD_B);
//        delay(3000);
//        LCD_fill(LCD_C);
//        delay(3000);
//        LCD_fill(LCD_M);
//        delay(3000);
//        LCD_fill(LCD_Y);
//        delay(3000);
//        LCD_fill(LCD_K);
//        delay(3000);
//    }
//delay(1000);
//LCD_fill(0xffff);
//    while (1) {
//        LCD_fill(LCD_R); //R
//        delay(1000);
//        LCD_fill(LCD_G); //G
//        delay(1000);
//        LCD_fill(LCD_B); //B
//        delay(1000);
//    }
}

void LCD_backlight(uint8_t brightness) {
    ledc_set_fade_with_time(LCD_BACKLIGHT_SPEED_MODE,
                            LCD_BACKLIGHT_CHANNEL, (unsigned) brightness << 5, LCD_BACKLIGHT_FADE_TIME);
    // ESP_LOGI(LOG_TAG, "duty: %f, calc: %d", percent, (unsigned)(5000 * percent));
    ledc_fade_start(LCD_BACKLIGHT_SPEED_MODE,
                    LCD_BACKLIGHT_CHANNEL, LEDC_FADE_NO_WAIT);
}