#ifdef __ABS_ENC1_H__
#define __ABS_ENC1_H__

#include "gsdk.h"
#include "paratbl.h"

// #include "axis.h"

// #include "./as5600.h"
// #include "./as5047.h"
// #include "./mt6701.h"
// #include "./mt6816.h"
// #include "./mt6835.h"
#include "./abs_tformat.h"

//typedef enum {
//    ABS_ENC_AS5600,
//    ABS_ENC_AS5047,
//    ABS_ENC_MT6701,
//    ABS_ENC_MT6816,
//    ABS_ENC_MT6835,
//    ABS_ENC_TFORMAT,
//} enc_type_e;

///**
// * @brief 编码器工作模式
// */
//typedef enum {
//    ABS_ENC_MODE_ABS,  ///< 以绝对位置模式工作
//    ABS_ENC_MODE_INC,  ///< 以增量位置模式工作
//} abs_enc_mode_e;

///**
// * @brief 编码器运行阶段
// */
//typedef enum {
//    ABS_ENC_STATE_INIT,       ///< 初始化
//    ABS_ENC_STATE_NORMAL,     ///< 通信正常
//    ABS_ENC_STATE_COMM_FAIL,  ///< 通信失败(初始化时)
//    ABS_ENC_STATE_COMM_LOST,  ///< 通信丢失(通信过程中)
//} abs_enc_sts_e;

///**
// * @brief 编码器指令
// */
//typedef enum {
//    ABS_ENC_CMD_TURN_CLR,   // 多圈值清除
//    ABS_ENC_CMD_ERR_CLR,    // 错误清除
//    ABS_ENC_CMD_STATE_RST,  // 恢复初始状态
//} abs_enc_cmd_t;

//typedef struct {
//    __IN u32        u32EncRes;  ///< 编码器分辨率
//    __IN u8         u8EncBit;   ///< 编码器位数
//    __IN abs_drv_t* pEncDrv;    ///< 编码器驱动指针

//    __OUT abs_enc_sts_e eState;  ///< 编码器状态

//    u16 u16CmdPre;
//    s32 s32PosPre;
//    s32 s32PosDelt;

//    __OUT s64 s64PosInit;  ///< 编码器初始位置
//    __OUT s32 s32Pos;      ///< 编码器单圈位置
//    __OUT s32 s32Turns;    ///< 编码器圈数
//    __OUT s64 s64MultPos;  ///< 编码器多圈位置
//    __OUT s32 s32SpdFb;    ///< 编码器速度反馈

//    u16 u16Tick;  ///< 异步读取计时
//} abs_enc_t;

//extern void AbsEncCreat(abs_enc_t* pEnc, axis_e eAxisNo);
//extern void AbsEncInit(abs_enc_t* pEnc, axis_e eAxisNo);
//extern void AbsEncCycle(abs_enc_t* pEnc, axis_e eAxisNo);
//extern void AbsEncIsr(abs_enc_t* pEnc, axis_e eAxisNo);

#endif
