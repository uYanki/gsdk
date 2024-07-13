#ifndef __OLED_FONT_H__
#define __OLED_FONT_H__

#include "gsdk.h"

#ifdef __cplusplus
extern "C" {
#endif

// ascii only
#define CONFIG_FONTSRC_CONSLONS_8X16  FONT_SRC_FLASH
#define CONFIG_FONTSRC_CONSLONS_16X24 FONT_SRC_NONE
#define CONFIG_FONTSRC_CONSLONS_24X32 FONT_SRC_NONE

// ascii and chinese
#define CONFIG_FONTSRC_GB2312 FONT_SRC_NONE  // (未测试)

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

/**
 * @brief 字体加载源, CONFIG_FONTSRC_xxx
 */
#define FONT_SRC_NONE     (0u)  // 不启用
#define FONT_SRC_FLASH    (1u)  // 片上 Flash
#define FONT_SRC_EXTFLASH (0u)  // 片外 Flash (未测试)
#define FONT_SRC_SDCARD   (0u)  // 内存卡 (未测试)

typedef struct {
    uint8_t  source;   // 字体来源
    void*    address;  // 储存地址
    uint16_t width;    // 字体宽度
    uint16_t height;   // 字体高度
} oled_font_t;

#if (CONFIG_FONTSRC_CONSLONS_8X16 != FONT_SRC_NONE)
extern oled_font_t g_Font_Conslons_8x16;
#endif
#if (CONFIG_FONTSRC_CONSLONS_16X24 != FONT_SRC_NONE)
extern oled_font_t g_Font_Conslons_16x24;
#endif
#if (CONFIG_FONTSRC_CONSLONS_24X32 != FONT_SRC_NONE)
extern oled_font_t g_Font_Conslons_24x32;
#endif
#if (CONFIG_FONTSRC_GB2312 != FONT_SRC_NONE)
extern oled_font_t g_Font_GB2312_H1616;
#endif

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

/**
 * @brief 获取字符编码
 * @param[out] ucBuffer 字模缓冲数组
 * @param[in]  usChar   字符码(国标码)
 * @param[in]  Fonts    字体结构体
 */
uint8_t GetConslonsCode(uint8_t* pBuffer, char chr, oled_font_t* Fonts);
uint8_t GetGBKCode(uint8_t* pBuffer, char chr, oled_font_t* Fonts);

#ifdef __cplusplus
}
#endif

#endif
