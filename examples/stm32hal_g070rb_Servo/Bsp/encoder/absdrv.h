/**
 * @brief 绝对值编码器驱动公共接口
 */

#ifndef __ABS_DRV_H__
#define __ABS_DRV_H__

//#include "defs.h"
#include "gsdk.h"

/**
 * @brief 驱动开关
 */
//#define __ABS_ENC_MT6701_HWSPI_SW  (CONFIG_ABS_ENC_MT6701_SW & ABS_COMM_HWSPI)
//#define __ABS_ENC_MT6701_SWSPI_SW  (CONFIG_ABS_ENC_MT6701_SW & ABS_COMM_SWSPI)
//#define __ABS_ENC_MT6701_SW        (__ABS_ENC_MT6701_HWSPI_SW | __ABS_ENC_MT6701_SWSPI_SW)

//#define __ABS_ENC_AS5600_HWI2C_SW  (CONFIG_ABS_ENC_AS5600_SW & ABS_COMM_HWI2C)
//#define __ABS_ENC_AS5600_AIN_SW    (CONFIG_ABS_ENC_AS5600_SW & ABS_COMM_AIN)
//#define __ABS_ENC_AS5600_SW        (__ABS_ENC_AS5600_HWI2C_SW | __ABS_ENC_AS5600_AIN_SW)

//#define __ABS_ENC_AS5047_HWI2C_SW  (CONFIG_ABS_ENC_AS5047_SW & ABS_COMM_HWI2C)
//#define __ABS_ENC_AS5047_HWSPI_SW  (CONFIG_ABS_ENC_AS5047_SW & ABS_COMM_HWSPI)
//#define __ABS_ENC_AS5047_SW        (__ABS_ENC_AS5047_HWI2C_SW | __ABS_ENC_AS5047_HWSPI_SW)

//#define __ABS_ENC_MT6816_HWSPI_SW  (CONFIG_ABS_ENC_MT6816_SW & ABS_COMM_SWSPI)
//#define __ABS_ENC_MT6816_SW        (__ABS_ENC_MT6816_SWSPI_SW)

//#define __ABS_ENC_MT6835_HWSPI_SW  (CONFIG_ABS_ENC_MT6835_SW & ABS_COMM_SWSPI)
//#define __ABS_ENC_MT6835_SW        (__ABS_ENC_MT6835_SWSPI_SW)

#define __ABS_ENC_TFORMAT_RS485_SW 1// (CONFIG_ABS_ENC_TFORMAT_SW & ABS_COMM_RS485)
#define __ABS_ENC_TFORMAT_SW      1// (__ABS_ENC_TFORMAT_RS485_SW)

/**
 * @brief 通信状态
 */
typedef enum {
    ABS_COMM_WAIT,     ///< 等待响应
    ABS_COMM_DRDY,     ///< 数据就绪
    ABS_COMM_TIMEOUT,  ///< 通信超时
    ABS_COMM_CRC_ERR,  ///< 数据检验失败
} abs_comm_e;

/**
 * @brief 编码器驱动
 */
typedef struct {
    u8         u8Mode;      ///< 通信模式 (i2c/spi/uart...)
    abs_comm_e eCommSts;    ///< 通信状态
    u16        u16CommErr;  ///< 通信错误次数
    u32        u32Pos;      ///< 单圈位置
    void*      pArgs;       ///< 附加参数
} abs_drv_t;

#endif
