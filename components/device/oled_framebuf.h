#ifndef __OLED_FRAMEBUF_H__
#define __OLED_FRAMEBUF_H__

#include "gsdk.h"
#include "oled_font.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

typedef enum {
    BLACK,
    WHITE,
    XOR,
} color_e;

typedef struct {
    uint8_t* pu8Buffer;
    uint16_t u16Width;
    uint16_t u16Height;
    uint16_t u16CurrentX;
    uint16_t u16CurrentY;
} framebuf_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

void Framebuf_Fill(framebuf_t* pHandle, color_e eColor);

void Framebuf_DrawPixel(framebuf_t* pHandle, uint16_t x, uint16_t y, color_e eColor);
void Framebuf_DrawLine(framebuf_t* pHandle, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, color_e eColor);

void Framebuf_DrawRectangle(framebuf_t* pHandle, uint16_t x, uint16_t y, uint16_t w, uint16_t h, color_e eColor);
void Framebuf_DrawFilledRectangle(framebuf_t* pHandle, uint16_t x, uint16_t y, uint16_t w, uint16_t h, color_e eColor);

void Framebuf_DrawTriangle(framebuf_t* pHandle, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, color_e eColor);
void Framebuf_DrawFilledTriangle(framebuf_t* pHandle, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, color_e eColor);

void Framebuf_DrawCircle(framebuf_t* pHandle, int16_t x0, int16_t y0, int16_t r, color_e eColor);
void Framebuf_DrawFilledCircle(framebuf_t* pHandle, int16_t x0, int16_t y0, int16_t r, color_e eColor);

/**
 * @brief Sets cursor pointer to desired location for strings
 */
void Framebuf_GotoXY(framebuf_t* pHandle, uint16_t x, uint16_t y);

char Framebuf_Putc(framebuf_t* pHandle, char ch, oled_font_t* Font, color_e eColor);
char Framebuf_Puts(framebuf_t* pHandle, char* str, oled_font_t* Font, color_e eColor);

#ifdef __cplusplus
}
#endif

#endif
