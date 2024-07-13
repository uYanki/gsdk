#ifndef __OLED_I2C_H__
#define __OLED_I2C_H__

#include "main.h"
#include "defs.h"

#define OLED_DEV_ADDR 0x78  // address = 0x78 / 0x7A

#define OLED_SCREEN_W 128
#define OLED_SCREEN_H 4

typedef enum {
    crNone  = 0,  // transparent
    crBLACK = 1,
    crWHITE = 2,
    crXOR   = 3,
} oled_color_t;

void oled_init(void);
void oled_display_on(void);
void oled_display_off(void);
void oled_update(void);
void oled_clear_line(uint8_t row, uint8_t xstart, uint8_t xend, oled_color_t cr);
void oled_clear(void);
void oled_draw_point(uint8_t x, uint8_t y, oled_color_t cr);
void oled_draw_line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, oled_color_t cr);
void oled_draw_circle(uint8_t x, uint8_t y, uint8_t r, oled_color_t cr);
void oled_draw_char(uint8_t x, uint8_t y, char ch, oled_color_t cr_fore, oled_color_t cr_back);
void oled_draw_char_zh(uint8_t x, uint8_t y, uint16_t ch, oled_color_t cr_fore, oled_color_t cr_back);
void oled_draw_str(uint8_t x, uint8_t y, char* ch, oled_color_t cr_fore, oled_color_t cr_back);
void oled_draw_img(uint8_t x, uint8_t y, uint8_t imgw, uint8_t imgh, uint8_t* bmp);
void oled_reset(void);

#endif
