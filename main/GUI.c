//
// Created by Lin on 2020/7/8.
//
#include <stdio.h>

#include "LCD.h"
#include "Fonts.c"
#include "common.h"


LCD_Point currPoint = {
        10, 10
};

LCD_Rect initRect = {
        {10, 10},
        {LCD_H - 10, LCD_W - 10},
};


void GUI_setPos(const LCD_Rect rect) {
    currPoint.x = rect.from.y;
    currPoint.y = rect.from.x;
    initRect.from.x = rect.from.y;
    initRect.from.y = rect.from.x;
    initRect.to.x = rect.to.y;
    initRect.to.y = rect.to.x;
}

void GUI_init_bar() {

}

void GUI_print(uint16_t fColor, const char *string) {
    uint16_t buff[16 * 32];
    int c = 0;
    LCD_cmd(0x36, &(uint8_t) {LCD_ROW | LCD_COL | LCD_BGR}, 1);
    LCD_setWindow(currPoint, (LCD_Point) {currPoint.x + 32, LCD_W});
    LCD_cmd(0x2c, 0, 0);
    while (string[c] != 0) {
        if (string[c] == '\n') {
            currPoint.x += 32;
            currPoint.y = initRect.from.y;
            goto next;
        } else if (string[c] < '!' || string[c] > '~') {
            currPoint.y += 16;
            goto next;
        }
        if (currPoint.y > initRect.to.y) {
            currPoint.x += 32;
            currPoint.y = initRect.from.y;
            LCD_setWindow(currPoint, (LCD_Point) {currPoint.x + 32, LCD_W});
            LCD_cmd(0x2c, 0, 0);
        }
        const uint8_t *k = charArray[string[c] - 33]; // start from 33
        int pb = 0;
        for (int a = 0; k[a] != 0; a++) {
            const uint8_t alpha = (k[a] & 0xF0) >> 4;
            const uint16_t cColor =
                    ((fColor & LCD_R) == 0 ? 0 : (R_alpha[alpha])) |
                    ((fColor & LCD_G) == 0 ? 0 : (G_alpha[alpha])) |
                    ((fColor & LCD_B) == 0 ? 0 : (B_alpha[alpha]));
            for (int i = 0; i < (k[a] & 0xF); ++i)
                buff[pb++] = cColor;
        }
        LCD_data((const uint8_t *) buff, 16 * 32 * 2);
        currPoint.y += 16;
        c++;
        continue;
        next:
        LCD_setWindow(currPoint, (LCD_Point) {currPoint.x + 32, LCD_W});
        LCD_cmd(0x2c, 0, 0);
        c++;
    }
    LCD_cmd(0x36, &(uint8_t) {LCD_MAD_DEFAULT}, 1);
}