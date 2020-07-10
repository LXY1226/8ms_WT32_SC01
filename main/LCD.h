#include <stdint.h>

#include "common.h"

#define LCD_H (320)
#define LCD_W (480)


#define LCD_ROW 0x80//(1<<8)
#define LCD_COL 0x40//(1<<7)
#define LCD_R2L 0x20//(1<<6)
#define LCD_BGR 0x8//(1<<4)
#define LCD_MAD_DEFAULT LCD_ROW|LCD_COL|LCD_R2L|LCD_BGR

#define LCD_WHITE (LCD_R|LCD_G|LCD_B)
#define LCD_BLACK 0x0000

#define LCD_R 0x00f8
#define LCD_G 0xe007
#define LCD_B 0x1f00

#define LCD_C LCD_G|LCD_B
#define LCD_M LCD_R|LCD_B
#define LCD_Y LCD_R|LCD_G
#define LCD_K LCD_BLACK

//#define LCD_drawVert(LCD_Point from, LCD_Point to, const void *bitmap, int len)
//LCD_cmd()


typedef struct {
    uint16_t x, y;
} LCD_Point;

typedef struct {
    LCD_Point from, to;
} LCD_Rect;

typedef struct CMD_DATA {
    uint8_t cmd;
    void *data;
    //uint8_t data[14];
    uint8_t databytes; //No of data in data; bit 7 = delay after set; 0xFF = end of cmds.
} LCD_cmd_data;

void LCD_init();

void LCD_setWindow(LCD_Point from, LCD_Point to);

void LCD_draw(LCD_Point from, LCD_Point to, const uint16_t *bitmap, int len);

void LCD_cmd(const uint8_t cmd, const void *data, int datalen);

void LCD_data(const uint8_t *data, int len);

void LCD_backlight(uint8_t brightness);

extern uint16_t *frameBuf;