#ifndef __ENDDRV_TFORMAT_H__
#define __ENDDRV_TFORMAT_H__

// 多摩川 TAMAGAWA 绝对值编码器
// STM32G030F86P6 + MT6818 + SP485
// ST 读编码器位置，通过485发送数据，数据格式是 TFormat 的

#include "gsdk.h"

#include "absdrv.h"

typedef enum {
    TFORMAT_CMD_READ_POS,      // 读位置
    TFORMAT_CMD_SET_ZERO_POS,  // 置零点
    TFORMAT_CMD_CLR_TURNS,     // 清多圈
} abs_cmd_e;

typedef struct {
    u8 au8RxBuf[6];  ///< 接收数据缓冲
} abs_tformat_t;

void TFormatRdPos(abs_drv_t* pEnc);
void TFormatTimeout(abs_drv_t* pEnc);
void TFormatRdData(abs_drv_t* pEnc);
void TFormatWrCmd(abs_drv_t* pEnc, abs_cmd_e eCmd);

#endif
