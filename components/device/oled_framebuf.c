#include "oled_framebuf.h"
#include "oled_font.h"
#include <string.h>  // memset

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "oledfb"
#define LOG_LOCAL_LEVEL LOG_LEVEL_INFO

/* Absolute value */
#define ABS(x) ((x) > 0 ? (x) : -(x))

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

void Framebuf_Fill(framebuf_t* pHandle, oled_color_e eColor)
{
    /* Set memory */

    uint16_t size = pHandle->u16Height * pHandle->u16Height / 8;

    switch (eColor)
    {
        case OLED_COLOR_WHITE:
        {
            memset(pHandle->pu8Buffer, 0x00, size);
            break;
        }

        case OLED_COLOR_BLACK:
        {
            memset(pHandle->pu8Buffer, 0xFF, size);
            break;
        }

        case OLED_COLOR_XOR:
        {
            for (uint8_t i = 0; i < size; i++)
            {
                pHandle->pu8Buffer[i] ^= 0xFF;
            }
            break;
        }

        default:
        {
            break;
        }
    }
}

void Framebuf_DrawPixel(framebuf_t* pHandle, uint16_t x, uint16_t y, oled_color_e eColor)
{
    if (x >= pHandle->u16Width || y >= pHandle->u16Height)
    {
        /* Error */
        return;
    }

    uint16_t byte   = x + (y / 8) * pHandle->u16Width;
    uint16_t bitmsk = 1 << (y % 8);

    /* Set eColor */
    switch (eColor)
    {
        case OLED_COLOR_WHITE:
            pHandle->pu8Buffer[byte] |= bitmsk;
            break;
        case OLED_COLOR_BLACK:
            pHandle->pu8Buffer[byte] &= ~bitmsk;
            break;
        case OLED_COLOR_XOR:
            pHandle->pu8Buffer[byte] ^= bitmsk;
            break;
        default:
            break;
    }
}

void Framebuf_GotoXY(framebuf_t* pHandle, uint16_t x, uint16_t y)
{
    /* Set write pointers */
    pHandle->u16CurrentX = x;
    pHandle->u16CurrentY = y;
}

char Framebuf_Putc(framebuf_t* pHandle, char ch, oled_font_t* Font, oled_color_e eColor)
{
    uint32_t i, b, j;

    /* Check available space in LCD */
    if (pHandle->u16Width <= (pHandle->u16CurrentX + Font->u16Width) ||
        pHandle->u16Height <= (pHandle->u16CurrentY + Font->u16Height))
    {
        /* Error */
        return 0;
    }

    /* Go through font */
    for (i = 0; i < Font->u16Height; i++)
    {
        // b = Font->data[(ch - 32) * Font->u16Height + i];
        for (j = 0; j < Font->u16Width; j++)
        {
            if ((b << j) & 0x8000)
            {
                Framebuf_DrawPixel(pHandle, pHandle->u16CurrentX + j, (pHandle->u16CurrentY + i), (oled_color_e)eColor);
            }
            else
            {
                Framebuf_DrawPixel(pHandle, pHandle->u16CurrentX + j, (pHandle->u16CurrentY + i), (oled_color_e)!eColor);
            }
        }
    }

    /* Increase pointer */
    pHandle->u16CurrentX += Font->u16Width;

    /* Return character written */
    return ch;
}

char Framebuf_Puts(framebuf_t* pHandle, char* str, oled_font_t* Font, oled_color_e eColor)
{
    /* Write characters */
    while (*str)
    {
        /* Write character by character */
        if (Framebuf_Putc(pHandle, *str, Font, eColor) != *str)
        {
            /* Return error */
            return *str;
        }

        /* Increase string pointer */
        str++;
    }

    /* Everything OK, zero should be returned */
    return *str;
}

void Framebuf_DrawLine(framebuf_t* pHandle, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, oled_color_e eColor)
{
    int16_t dx, dy, sx, sy, err, e2, i, tmp;

    /* Check for overflow */
    if (x0 >= pHandle->u16Width)
    {
        x0 = pHandle->u16Width - 1;
    }
    if (x1 >= pHandle->u16Width)
    {
        x1 = pHandle->u16Width - 1;
    }
    if (y0 >= pHandle->u16Height)
    {
        y0 = pHandle->u16Height - 1;
    }
    if (y1 >= pHandle->u16Height)
    {
        y1 = pHandle->u16Height - 1;
    }

    dx  = (pHandle, x0 < x1) ? (pHandle, x1 - x0) : (pHandle, x0 - x1);
    dy  = (y0 < y1) ? (y1 - y0) : (y0 - y1);
    sx  = (pHandle, x0 < x1) ? 1 : -1;
    sy  = (y0 < y1) ? 1 : -1;
    err = ((dx > dy) ? dx : -dy) / 2;

    if (dx == 0)
    {
        if (y1 < y0)
        {
            tmp = y1;
            y1  = y0;
            y0  = tmp;
        }

        if (pHandle, x1 < x0)
        {
            tmp = x1;
            x1  = x0;
            x0  = tmp;
        }

        /* Vertical line */
        for (i = y0; i <= y1; i++)
        {
            Framebuf_DrawPixel(pHandle, x0, i, eColor);
        }

        /* Return from function */
        return;
    }

    if (dy == 0)
    {
        if (y1 < y0)
        {
            tmp = y1;
            y1  = y0;
            y0  = tmp;
        }

        if (pHandle, x1 < x0)
        {
            tmp = x1;
            x1  = x0;
            x0  = tmp;
        }

        /* Horizontal line */
        for (i = x0; i <= x1; i++)
        {
            Framebuf_DrawPixel(pHandle, i, y0, eColor);
        }

        /* Return from function */
        return;
    }

    while (1)
    {
        Framebuf_DrawPixel(pHandle, x0, y0, eColor);
        if (pHandle, x0 == x1 && y0 == y1)
        {
            break;
        }
        e2 = err;
        if (e2 > -dx)
        {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dy)
        {
            err += dx;
            y0 += sy;
        }
    }
}

void Framebuf_DrawRectangle(framebuf_t* pHandle, uint16_t x, uint16_t y, uint16_t w, uint16_t h, oled_color_e eColor)
{
    /* Check input parameters */
    if (x >= pHandle->u16Width || y >= pHandle->u16Height)
    {
        /* Return error */
        return;
    }

    /* Check u16Width and height */
    if ((pHandle, x + w) >= pHandle->u16Width)
    {
        w = pHandle->u16Width - x;
    }
    if ((y + h) >= pHandle->u16Height)
    {
        h = pHandle->u16Height - y;
    }

    /* Draw 4 lines */
    Framebuf_DrawLine(pHandle, x, y, x + w, y, eColor);         /* Top line */
    Framebuf_DrawLine(pHandle, x, y + h, x + w, y + h, eColor); /* Bottom line */
    Framebuf_DrawLine(pHandle, x, y, x, y + h, eColor);         /* Left line */
    Framebuf_DrawLine(pHandle, x + w, y, x + w, y + h, eColor); /* Right line */
}

void Framebuf_FillRectangle(framebuf_t* pHandle, uint16_t x, uint16_t y, uint16_t w, uint16_t h, oled_color_e eColor)
{
    uint8_t i;

    /* Check input parameters */
    if (x >= pHandle->u16Width || y >= pHandle->u16Height)
    {
        /* Return error */
        return;
    }

    /* Check u16Width and height */
    if ((x + w) >= pHandle->u16Width)
    {
        w = pHandle->u16Width - x;
    }
    if ((y + h) >= pHandle->u16Height)
    {
        h = pHandle->u16Height - y;
    }

    /* Draw lines */
    for (i = 0; i <= h; i++)
    {
        /* Draw lines */
        Framebuf_DrawLine(pHandle, x, y + i, x + w, y + i, eColor);
    }
}

void Framebuf_DrawTriangle(framebuf_t* pHandle, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, oled_color_e eColor)
{
    /* Draw lines */
    Framebuf_DrawLine(pHandle, x1, y1, x2, y2, eColor);
    Framebuf_DrawLine(pHandle, x2, y2, x3, y3, eColor);
    Framebuf_DrawLine(pHandle, x3, y3, x1, y1, eColor);
}

void Framebuf_FillTriangle(framebuf_t* pHandle, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, oled_color_e eColor)
{
    int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0,
            yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0,
            curpixel = 0;

    deltax = ABS(x2 - x1);
    deltay = ABS(y2 - y1);
    x      = x1;
    y      = y1;

    if (pHandle, x2 >= x1)
    {
        xinc1 = 1;
        xinc2 = 1;
    }
    else
    {
        xinc1 = -1;
        xinc2 = -1;
    }

    if (y2 >= y1)
    {
        yinc1 = 1;
        yinc2 = 1;
    }
    else
    {
        yinc1 = -1;
        yinc2 = -1;
    }

    if (deltax >= deltay)
    {
        xinc1     = 0;
        yinc2     = 0;
        den       = deltax;
        num       = deltax / 2;
        numadd    = deltay;
        numpixels = deltax;
    }
    else
    {
        xinc2     = 0;
        yinc1     = 0;
        den       = deltay;
        num       = deltay / 2;
        numadd    = deltax;
        numpixels = deltay;
    }

    for (curpixel = 0; curpixel <= numpixels; curpixel++)
    {
        Framebuf_DrawLine(pHandle, x, y, x3, y3, eColor);

        num += numadd;
        if (num >= den)
        {
            num -= den;
            x += xinc1;
            y += yinc1;
        }
        x += xinc2;
        y += yinc2;
    }
}

void Framebuf_DrawCircle(framebuf_t* pHandle, int16_t x0, int16_t y0, int16_t r, oled_color_e eColor)
{
    int16_t f     = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x     = 0;
    int16_t y     = r;

    Framebuf_DrawPixel(pHandle, x0, y0 + r, eColor);
    Framebuf_DrawPixel(pHandle, x0, y0 - r, eColor);
    Framebuf_DrawPixel(pHandle, x0 + r, y0, eColor);
    Framebuf_DrawPixel(pHandle, x0 - r, y0, eColor);

    while (pHandle, x < y)
    {
        if (f >= 0)
        {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        Framebuf_DrawPixel(pHandle, x0 + x, y0 + y, eColor);
        Framebuf_DrawPixel(pHandle, x0 - x, y0 + y, eColor);
        Framebuf_DrawPixel(pHandle, x0 + x, y0 - y, eColor);
        Framebuf_DrawPixel(pHandle, x0 - x, y0 - y, eColor);

        Framebuf_DrawPixel(pHandle, x0 + y, y0 + x, eColor);
        Framebuf_DrawPixel(pHandle, x0 - y, y0 + x, eColor);
        Framebuf_DrawPixel(pHandle, x0 + y, y0 - x, eColor);
        Framebuf_DrawPixel(pHandle, x0 - y, y0 - x, eColor);
    }
}

void Framebuf_FillCircle(framebuf_t* pHandle, int16_t x0, int16_t y0, int16_t r, oled_color_e eColor)
{
    int16_t f     = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x     = 0;
    int16_t y     = r;

    Framebuf_DrawPixel(pHandle, x0, y0 + r, eColor);
    Framebuf_DrawPixel(pHandle, x0, y0 - r, eColor);
    Framebuf_DrawPixel(pHandle, x0 + r, y0, eColor);
    Framebuf_DrawPixel(pHandle, x0 - r, y0, eColor);
    Framebuf_DrawLine(pHandle, x0 - r, y0, x0 + r, y0, eColor);

    while (pHandle, x < y)
    {
        if (f >= 0)
        {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        Framebuf_DrawLine(pHandle, x0 - x, y0 + y, x0 + x, y0 + y, eColor);
        Framebuf_DrawLine(pHandle, x0 + x, y0 - y, x0 - x, y0 - y, eColor);

        Framebuf_DrawLine(pHandle, x0 + y, y0 + x, x0 - y, y0 + x, eColor);
        Framebuf_DrawLine(pHandle, x0 + y, y0 - x, x0 - y, y0 - x, eColor);
    }
}
