/* Lin 2020/07/04 */
#include "stdio.h"
#include "esp_log.h"

#include "LCD.h"
#include "GUI.h"

#define LOG_TAG "main"

void app_main(void){
    // gpio_set_direction(23, GPIO_MODE_OUTPUT);
    // gpio_set_level(23, 0);
    // gpio_set_level(23, 1);
    LCD_init();
    GUI_print(LCD_WHITE, "Mirai\n");
    GUI_print(LCD_C, "Mirai");
    GUI_print(LCD_M, "Mirai");
    GUI_print(LCD_Y, "Mirai");
    GUI_print(LCD_C, "Mirai");
    GUI_print(LCD_M, "Mirai");
    GUI_print(LCD_Y, "Mirai");
    GUI_print(LCD_C, "Mirai");
    GUI_print(LCD_M, "Mirai");
    GUI_print(LCD_Y, "Mirai");
    GUI_print(LCD_C, "Mirai");
    GUI_print(LCD_M, "Mirai");
    GUI_print(LCD_Y, "Mirai");
//    printf("\nFin");
    // while(1)
    // {
    //     vTaskDelay(1000 / portTICK_RATE_MS);
    //     gpio_set_level(13, 1);
    //     vTaskDelay(1000 / portTICK_RATE_MS);
    // }
    // LCD_backlight(0.5);
    // vTaskDelay(2000 / portTICK_PERIOD_MS);
    // LCD_backlight(0.1);
    // vTaskDelay(2000 / portTICK_PERIOD_MS);
    // LCD_backlight(1);
    // vTaskDelay(2000 / portTICK_PERIOD_MS);
    // LCD_backlight(0);
}