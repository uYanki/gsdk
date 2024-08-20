#ifndef __AXIS_DEFS_H__
#define __AXIS_DEFS_H__

#include "gdefs.h"

typedef enum {
    AXIS_0,
    AXIS_1,
    AXIS_2,
    // ...
} axis_e;

typedef enum {
    AXIS_STATE_INITIAL,  ///< axis initial
    AXIS_STATE_STANDBY,  ///< axis status is ok, waiting for command
    AXIS_STATE_ENABLE,   ///< axis is enabled
    AXIS_STATE_FAULT,    ///< axis fault
} axis_state_e;

typedef enum {
    AXIS_APP_GENERIC,   // 通用控制
    AXIS_APP_OPENLOOP,  // 开环测试
    AXIS_APP_ENCIDENT,  // 编码器辨识
    AXIS_APP_FUNCTEST,  // 功能测试
    AXIS_APP_INVAILD,
} axis_app_e;

/**
 * @brief 控制模式
 */
typedef enum {
    CTRL_MODE_POS,    ///< position mode
    CTRL_MODE_SPD,    ///< speed mode
    CTRL_MODE_TRQ,    ///< torque mode
    CTRL_MODE_DS402,  ///< ds402 over CANopen/EtherCAT mode
} ctrl_mode_e;

/**
 * @brief 控制指令来源
 */
typedef enum {
    CTRL_CMD_SRC_COMM,  ///< communication parameter
    CTRL_CMD_SRC_DI,    ///< digital input
} ctrl_cmd_src_e;

/**
 * @brief 控制字
 */
typedef union {
    struct {
        uint32_t Enable : 1;
        uint32_t _Resv  : 31;
    } u32Bit;
    uint32_t u32All;
} ctrlword_u;

/**
 * @brief 状态字
 */
typedef struct {
    uint32_t ServoOn : 1;
    uint32_t _Resv   : 31;
} statusword_t;

/**
 * @brief 速度指令源
 */
typedef enum {
    SPD_REF_SRC_DIGITAL,
    SPD_REF_SRC_MULTI_DIGITAL,
    SPD_REF_SRC_AI_1,
    SPD_REF_SRC_AI_2,
} spd_ref_src_e;

/**
 * @brief 速度限制源
 */
typedef enum {
    SPD_LIM_NONE,
    SPD_LIM_S,     ///< 正反向均为速度限制0
    SPD_LIM_M,     ///< 正向为速度限制0，反向为速度限制1
    SPD_LIM_AI_S,  ///< 正反向速度限制均由AI1决定
    SPD_LIM_AI_M,  ///< 正反速度限制均由AI1决定，反向为速度限制均由AI2决定
} spd_lim_src_e;

/**
 * @brief  速度规划模式
 */
typedef enum {
    SPD_PLAN_MODE_SLOPE,     ///< 斜坡规划
    SPD_PLAN_MODE_CURVE_S1,  ///< S曲线1(三次多项式)规划
    SPD_PLAN_MODE_CURVE_S2,  ///< S曲线2(二次多项式)规划
    SPD_PLAN_MODE_STEP,      ///< 无规划，直接阶跃
} spd_plan_mode_e;

typedef enum {
    TRQ_LIM_NONE,  ///< 不启用转矩限制
    TRQ_LIM_S,     ///< 正反向均为转矩限制0
    TRQ_LIM_M,     ///< 正向为转矩限制0，反向为转矩限制1
    TRQ_LIM_AI_S,  ///< 正反向转矩限制均由AI1决定
    TRQ_LIM_AI_M,  ///< 正反转矩限制均由AI1决定，反向为转矩限制均由AI2决定
} trq_lim_src_e;

/**
 * @brief 转矩指令来源
 */
typedef enum {
    TRQ_REF_SRC_DIGITAL,
    TRQ_REF_SRC_MULTI_DIGITAL,
    TRQ_REF_SRC_AI_1,
    TRQ_REF_SRC_AI_2,
} trq_ref_src_e;

/**
 * @brief 停机过程方式
 */
typedef enum {
    STOP_MODE_NO,        ///< 不停机
    STOP_MODE_FREE,      ///< 自由停机(关闭PWM输出)
    STOP_MODE_DEC,       ///< 减速停机(按减速时间DEC停机)
    STOP_MODE_ZERO_SPD,  ///< 零速停机(给0速度阶跃指令)
    STOP_MODE_DB_TRQ,    ///< 动态转矩制动停机(给定反向转矩指令)
    // STOP_MODE_DB_FLUX,   ///< 动态磁通制动停机(给定磁通指令)
    STOP_MODE_DB_RES,  ///< 动态能耗制动停机(永磁电机有效,需硬件支持,电机相线并电阻短接)
} stop_mode_e;

/**
 * @brief 停机完成后动作
 */
typedef enum {
    STOP_OVER_MODE_INVALID,    ///< 无效
    STOP_OVER_MODE_FREE,       ///< 自由运行(关闭PWM输出)
    STOP_OVER_MODE_ZERO_LOCK,  ///< 零位锁定
    STOP_OVER_MODE_DB_RES,     ///< 动态能耗制动(永磁电机有效,需硬件支持,电机相线并电阻短接)
} stop_over_mode_e;

/**
 * @brief 位置指令源
 */
typedef enum {
    POS_REF_SRC_DIGITAL,        ///< 数字位置指令
    POS_REF_SRC_MULTI_DIGITAL,  ///< 多段数字位置指令
    POS_REF_SRC_PULSE,          ///< 脉冲位置指令
} pos_ref_src_e;

/**
 * @brief 相对位置指令生效模式
 */
typedef enum {
    POS_REF_SET_IMMEDIATELY,  ///< 位置指令立即生效
    POS_REF_SET_TRIG,         ///< 位置指令触发生效
} pos_ref_set_e;

/**
 * @brief 位置指令类型
 */
typedef enum {
    POS_REF_TYPE_ABSOLUTE,
    POS_REF_TYPE_RELATIVE,
} pos_ref_type_e;

/**
 * @brief 行程限位源
 */
typedef enum {
    POS_LIM_SRC_NONE,            ///< 无位置限定
    POS_LIM_SRC_DIGITAL_REGION,  ///< 数字边界位置限制
    POS_LIM_SRC_SWITCH_SIGNAL,   ///< 开关信号位置限制(DI/通信命令)
} pos_lim_src_e;

/**
 * @brief 行程限位状态
 */
typedef enum {
    POS_LIM_STATUS_NO,   ///< 位置限制未触发
    POS_LIM_STATUS_FWD,  ///< 正向位置限制触发
    POS_LIM_STATUS_REV,  ///< 反向位置限制触发
} pos_lim_sts_e;

/**
 * @brief 位置规划模式
 */
typedef enum {
    POS_PLAN_MODE_CURVE_X2,   ///< 二次曲线规划(速度一次曲线(斜坡)，位置二次曲线)
    POS_PLAN_MODE_CURVE_SIN,  ///< SIN曲线规划(速度COS曲线，位置SIN曲线)
    POS_PLAN_MODE_STEP,       ///< 无规划，直接阶跃
    POS_PLAN_MODE_SLOPE,      ///< 一次斜坡规划(直线插补)
    __POS_PLAN_MODE_MIN = POS_PLAN_MODE_CURVE_X2,
    __POS_PLAN_MODE_MAX = POS_PLAN_MODE_STEP,
} pos_plan_mode_e;

#endif  // ! __AXIS_DEFS_H__
