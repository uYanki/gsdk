#include "spi_mfrc522.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "rc522"
#define LOG_LOCAL_LEVEL LOG_LEVEL_INFO

/**
 * @brief command
 */
#define PCD_IDLE       0x00  // NO action / Cancel the current command
#define PCD_AUTHENT    0x0E  // Authentication Key
#define PCD_RECEIVE    0x08  // Receive Data
#define PCD_TRANSMIT   0x04  // Transmit data
#define PCD_TRANSCEIVE 0x0C  // Transmit and receive data,
#define PCD_RESETPHASE 0x0F  // Reset
#define PCD_CALCCRC    0x03  // CRC Calculate

/**
 * @brief M1 command (mifare one)
 */
#define PICC_REQIDL    0x26  // find the antenna area does not enter hibernation
#define PICC_REQALL    0x52  // find all the cards antenna area
#define PICC_ANTICOLL1 0x93  // anti-collision
#define PICC_ANTICOLL2 0x95  // anti-collision
#define PICC_SElECTTAG 0x93  // election card
#define PICC_AUTHENT1A 0x60  // authentication key A
#define PICC_AUTHENT1B 0x61  // authentication key B
#define PICC_READ      0x30  // Read Block
#define PICC_WRITE     0xA0  // write block
#define PICC_DECREMENT 0xC0  // debit
#define PICC_INCREMENT 0xC1  // recharge
#define PICC_RESTORE   0xC2  // transfer block data to the buffer
#define PICC_TRANSFER  0xB0  // save the data in the buffer
#define PICC_HALT      0x50  // Sleep

/**
 * @brief fifo size
 */
#define DEF_FIFO_LENGTH 64  // FIFO size = 64byte
#define MAXRLEN         18

/**
 * @brief register
 */

// Page 0: Command and Status
#define RFU00           0x00
#define REG_COMMAND     0x01
#define REG_IE          0x02
#define REG_DIV_IE      0x03
#define REG_IRQ         0x04
#define REG_DIV_IRQ     0x05
#define REG_ERROR       0x06
#define REG_STATUS1     0x07
#define REG_STATUS2     0x08
#define REG_FIFO_DATA   0x09
#define REG_FIFO_LEVEL  0x0A
#define REG_WATER_LEVEL 0x0B
#define REG_CONTROL     0x0C
#define REG_BIT_FRAMING 0x0D
#define REG_COLLISION   0x0E
#define RFU0F           0x0F

// Page 1: Command
#define RFU10            0x10  // RC522 mannual 36/95
#define REG_MODE         0x11
#define REG_TX_MODE      0x12
#define REG_RX_MODE      0x13  // RC522 mannual 9.3.2.4
#define REG_TX_CONTROL   0x14
#define REG_TX_ASK       0x15
#define REG_TX_SEL       0x16
#define REG_RX_SEL       0x17
#define REG_RX_THRESHOLD 0x18
#define REG_DEMOD        0x19
#define RFU1A            0x1A
#define RFU1B            0x1B
#define REG_MIFARE       0x1C
#define RFU1D            0x1D
#define RFU1E            0x1E
#define REG_SERIAL_SPEED 0x1F

// Page 2: CFG
#define RFU20               0x20
#define REG_CRC_RESULT_M    0x21
#define REG_CRC_RESULT_L    0x22
#define RFU23               0x23
#define REG_MOD_WIDTH       0x24
#define RFU25               0x25
#define REG_RF_CONFIG       0x26
#define REG_GSN             0x27
#define REG_CWF_CONFIG      0x28
#define REG_MODGS_CONFIG    0x29
#define REG_TMODE           0x2A
#define REG_TIMER_PRESCALER 0x2B
#define REG_TIMER_RELOAD_H  0x2C
#define REG_TIMER_RELOAD_L  0x2D
#define REG_TIMER_VALUE_H   0x2E
#define REG_TIMER_VALUE_L   0x2F

// Page 3:TestRegister
#define RFU30              0x30
#define REG_TEST_SEL_1     0x31
#define REG_TEST_SEL_2     0x32
#define REG_TEST_PIN_EN    0x33
#define REG_TEST_PIN_VALUE 0x34
#define REG_TEST_BUS       0x35
#define REG_AUTO_TEST      0x36
#define REG_VERSION        0x37
#define REG_ANALOG_TEST    0x38
#define REG_TEST_ADC1      0x39
#define REG_TEST_ADC2      0x3A
#define REG_TEST_ADC       0x3B
#define RFU3C              0x3C
#define RFU3D              0x3D
#define RFU3E              0x3E
#define RFU3F              0x3F

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

static uint8_t _RC522_ReadRegister(spi_rc522_t* pHandle, uint8_t u8Addr);
static void    _RC522_WriteRegister(spi_rc522_t* pHandle, uint8_t u8Addr, uint8_t u8Data);
static void    _RC522_SetBitMask(spi_rc522_t* pHandle, uint8_t u8Addr, uint8_t u8Mask);
static void    _RC522_ClearBitMask(spi_rc522_t* pHandle, uint8_t u8Addr, uint8_t u8Mask);
static void    _RC522_CRC(spi_rc522_t* pHandle, __IN uint8_t* pu8Indata, uint8_t u8Length, __OUT uint8_t* pu8OutData);

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

static uint8_t _RC522_ReadRegister(spi_rc522_t* pHandle, uint8_t u8Addr)
{
    uint8_t u8Data;

    SPI_Master_Select(pHandle->hSPI);
    SPI_Master_TransmitByte(pHandle->hSPI, ((u8Addr << 1) & 0x7E) | 0x80);
    SPI_Master_ReceiveByte(pHandle->hSPI, &u8Data);
    SPI_Master_Deselect(pHandle->hSPI);

    return u8Data;
}

static void _RC522_WriteRegister(spi_rc522_t* pHandle, uint8_t u8Addr, uint8_t u8Data)
{
    SPI_Master_Select(pHandle->hSPI);
    SPI_Master_TransmitByte(pHandle->hSPI, (u8Addr << 1) & 0x7E);
    SPI_Master_TransmitByte(pHandle->hSPI, u8Data);
    SPI_Master_Deselect(pHandle->hSPI);
}

static void _RC522_SetBitMask(spi_rc522_t* pHandle, uint8_t u8Addr, uint8_t u8Mask)
{
    uint8_t u8Data = _RC522_ReadRegister(pHandle, u8Addr);

    if ((u8Data & u8Mask) != u8Mask)  // msk no set
    {
        _RC522_WriteRegister(pHandle, u8Addr, u8Data | u8Mask);
    }
}

static void _RC522_ClearBitMask(spi_rc522_t* pHandle, uint8_t u8Addr, uint8_t u8Mask)
{
    _RC522_WriteRegister(pHandle, u8Addr, _RC522_ReadRegister(pHandle, u8Addr) & (~u8Mask));  // clear bit u8Mask
}

err_t RC522_Init(spi_rc522_t* pHandle)
{
    PIN_SetMode(&pHandle->RST, PIN_MODE_OUTPUT_PUSH_PULL, PIN_PULL_UP);

    PIN_WriteLevel(&pHandle->RST, PIN_LEVEL_LOW);  // 复位
    DelayBlockMs(1);
    PIN_WriteLevel(&pHandle->RST, PIN_LEVEL_HIGH);
    DelayBlockMs(1);

    _RC522_WriteRegister(pHandle, REG_COMMAND, PCD_RESETPHASE);
    DelayBlockMs(1);

    // 配置模式
    _RC522_WriteRegister(pHandle, REG_MODE, 0x29);

    // 配置16位定时器的高低位
    _RC522_WriteRegister(pHandle, REG_TIMER_RELOAD_H, 0);
    _RC522_WriteRegister(pHandle, REG_TIMER_RELOAD_L, 30);

    _RC522_WriteRegister(pHandle, REG_TMODE, 0x8D);  // Tauto=1; f(Timer) = 6.78MHz/TPreScaler

    // 设置定时器分频系数
    _RC522_WriteRegister(pHandle, REG_TIMER_PRESCALER, 0x3E);

    // 调制发送信号为100%ASK
    _RC522_WriteRegister(pHandle, REG_TX_ASK, 0x40);

    return ERR_NONE;
}

/**
  * @brief 寻卡
  * @param  req_code，寻卡方式 = 0x52，寻感应区内所有符合14443A标准的卡；
            寻卡方式= 0x26，寻未进入休眠状态的卡
  * @param  pTagType，卡片类型代码
             = 0x4400，Mifare_UltraLight
             = 0x0400，Mifare_One(S50)
             = 0x0200，Mifare_One(S70)
             = 0x0800，Mifare_Pro(X))
             = 0x4403，Mifare_DESFire
  * @retval 状态值 = ERR_NONE，成功
  */
uint8_t RC522_Request(spi_rc522_t* pHandle, uint8_t u8ReqCode, uint8_t* pu8TagType)
{
    uint16_t u16Length;
    uint8_t  au8Buffer[MAXRLEN];

    // 清理指示MIFARECyptol单元接通以及所有卡的数据通信被加密的情况
    _RC522_ClearBitMask(pHandle, REG_STATUS2, 0x08);
    // 发送的最后一个字节的 七位
    _RC522_WriteRegister(pHandle, REG_BIT_FRAMING, 0x07);
    // TX1,TX2管脚的输出信号传递经发送调制的13.56的能量载波信号
    _RC522_SetBitMask(pHandle, REG_TX_CONTROL, 0x03);

    au8Buffer[0] = u8ReqCode;  // 存入卡片命令字

    ERRCHK_RETURN(RC522_ComMF522(pHandle, PCD_TRANSCEIVE, au8Buffer, 1, au8Buffer, &u16Length));  // 寻卡

    if (u16Length != 0x10)
    {
        return ERR_NOT_EXIST;  // card doesn't exist
    }

    // 寻卡成功返回卡类型
    pu8TagType[0] = au8Buffer[0];
    pu8TagType[1] = au8Buffer[1];

    return ERR_NONE;
}

/**
 * @brief  防冲撞
 * @param  pSnr，卡片序列号，4字节
 * @retval 状态值 = ERR_NONE，成功
 */
uint8_t RC522_Anticoll(spi_rc522_t* pHandle, uint8_t* pu8Snr)
{
    uint8_t i;

    uint8_t  u8SnrChk = 0;
    uint16_t u16Length;
    uint8_t  au8Buffer[MAXRLEN];

    // 清MFCryptol On位 只有成功执行MFAuthent命令后，该位才能置位
    _RC522_ClearBitMask(pHandle, REG_STATUS2, 0x08);
    // 清理寄存器 停止收发
    _RC522_WriteRegister(pHandle, REG_BIT_FRAMING, 0x00);
    // 清ValuesAfterColl所有接收的位在冲突后被清除
    _RC522_ClearBitMask(pHandle, REG_COLLISION, 0x80);

    au8Buffer[0] = PICC_ANTICOLL1;  // 卡片防冲突命令
    au8Buffer[1] = 0x20;

    ERRCHK_RETURN(RC522_ComMF522(pHandle, PCD_TRANSCEIVE, au8Buffer, 2, au8Buffer, &u16Length));  // 与卡片通信

    // 通信成功
    for (i = 0; i < 4; i++)
    {
        pu8Snr[i] = au8Buffer[i];  // 读 UID
        u8SnrChk ^= au8Buffer[i];
    }

    if (u8SnrChk != au8Buffer[i])
    {
        return ERR_INVALID_CRC;  // fail to check snr
    }

    _RC522_SetBitMask(pHandle, REG_COLLISION, 0x80);

    return ERR_NONE;
}

/**
 * @brief  选定卡片
 * @param  pSnr，卡片序列号，4字节
 * @retval 状态值 = ERR_NONE，成功
 */
uint8_t RC522_Select(spi_rc522_t* pHandle, uint8_t* pu8Snr)
{
    uint8_t  i;
    uint16_t u16Length;
    uint8_t  au8Buffer[MAXRLEN];

    au8Buffer[0] = PICC_ANTICOLL1;
    au8Buffer[1] = 0x70;
    au8Buffer[6] = 0;

    for (i = 0; i < 4; i++)
    {
        au8Buffer[i + 2] = *(pu8Snr + i);
        au8Buffer[6] ^= *(pu8Snr + i);
    }

    _RC522_CRC(pHandle, au8Buffer, 7, &au8Buffer[7]);

    _RC522_ClearBitMask(pHandle, REG_STATUS2, 0x08);

    ERRCHK_RETURN(RC522_ComMF522(pHandle, PCD_TRANSCEIVE, au8Buffer, 9, au8Buffer, &u16Length));

    if (u16Length != 0x18)
    {
        return ERR_FAIL;
    }

    return ERR_NONE;
}

/**
 * @brief  验证卡片密码
 * @param  auth_mode，密码验证模式= 0x60，验证A密钥，
 *                    密码验证模式= 0x61，验证B密钥
 * @param  addr，块地址
 * @param  pKey，密码
 * @param  pSnr，卡片序列号，4字节
 * @retval 状态值 = ERR_NONE，成功
 */
uint8_t RC522_AuthState(spi_rc522_t* pHandle, uint8_t auth_mode, uint8_t addr, uint8_t* pKey, uint8_t* pu8Snr)
{
    uint16_t u16Length;
    uint8_t  i, au8Buffer[MAXRLEN];

    au8Buffer[0] = auth_mode;
    au8Buffer[1] = addr;
    memcpy(&au8Buffer[2], &pKey[0], 6);
    memcpy(&au8Buffer[8], &pu8Snr[0], 4);

    ERRCHK_RETURN(RC522_ComMF522(pHandle, PCD_AUTHENT, au8Buffer, 12, au8Buffer, &u16Length));

    if ((!(_RC522_ReadRegister(pHandle, REG_STATUS2) & 0x08)))
    {
        return ERR_FAIL;
    }

    return ERR_NONE;
}

/**
 * @brief  读取M1卡一块数据
 * @param  addr，块地址（0-63）。M1卡总共有16个扇区(每个扇区有：3个数据块+1个控制块),共64个块
 * @param  pData，读出的数据，16字节
 * @retval 状态值 = ERR_NONE，成功
 */
uint8_t RC522_Read(spi_rc522_t* pHandle, uint8_t addr, uint8_t* pData)
{
    uint16_t u16Length;
    uint8_t  i, au8Buffer[MAXRLEN];

    au8Buffer[0] = PICC_READ;
    au8Buffer[1] = addr;
    _RC522_CRC(pHandle, au8Buffer, 2, &au8Buffer[2]);

    ERRCHK_RETURN(RC522_ComMF522(pHandle, PCD_TRANSCEIVE, au8Buffer, 4, au8Buffer, &u16Length));

    if (u16Length == 0x90)
    {
        for (i = 0; i < 16; i++)
        {
            pData[i] = au8Buffer[i];
        }

        return ERR_NONE;
    }
    else
    {
        return ERR_FAIL;
    }
}

/**
 * @brief  写数据到M1卡一块
 * @param  addr，块地址（0-63）。M1卡总共有16个扇区(每个扇区有：3个数据块+1个控制块),共64个块
 * @param  pData，写入的数据，16字节
 * @retval 状态值 = ERR_NONE，成功
 */
uint8_t RC522_Write(spi_rc522_t* pHandle, uint8_t addr, uint8_t* pData)
{
    uint16_t u16Length;
    uint8_t  i, au8Buffer[MAXRLEN];

    au8Buffer[0] = PICC_WRITE;
    au8Buffer[1] = addr;
    _RC522_CRC(pHandle, au8Buffer, 2, &au8Buffer[2]);

    ERRCHK_RETURN(RC522_ComMF522(pHandle, PCD_TRANSCEIVE, au8Buffer, 4, au8Buffer, &u16Length));

    if ((u16Length != 4) || ((au8Buffer[0] & 0x0F) != 0x0A))
    {
        return ERR_FAIL;
    }

    for (i = 0; i < 16; ++i)
    {
        au8Buffer[i] = *(pData + i);
    }
    _RC522_CRC(pHandle, au8Buffer, 16, &au8Buffer[16]);

    ERRCHK_RETURN(RC522_ComMF522(pHandle, PCD_TRANSCEIVE, au8Buffer, 18, au8Buffer, &u16Length));

    if ((u16Length != 4) || ((au8Buffer[0] & 0x0F) != 0x0A))
    {
        return ERR_FAIL;
    }

    return ERR_NONE;
}

uint8_t RC522_Value(spi_rc522_t* pHandle, uint8_t dd_mode, uint8_t addr, uint8_t* pValue)
{
    uint16_t u16Length;
    uint8_t  i, au8Buffer[MAXRLEN];

    au8Buffer[0] = dd_mode;
    au8Buffer[1] = addr;
    _RC522_CRC(pHandle, au8Buffer, 2, &au8Buffer[2]);

    ERRCHK_RETURN(RC522_ComMF522(pHandle, PCD_TRANSCEIVE, au8Buffer, 4, au8Buffer, &u16Length));

    if ((u16Length != 4) || ((au8Buffer[0] & 0x0F) != 0x0A))
    {
        return ERR_FAIL;
    }

    memcpy(&au8Buffer[0], &pValue[0], 16);

    _RC522_CRC(pHandle, au8Buffer, 4, &au8Buffer[4]);
    u16Length = 0;
    ERRCHK_RETURN(RC522_ComMF522(pHandle, PCD_TRANSCEIVE, au8Buffer, 6, au8Buffer, &u16Length));

    au8Buffer[0] = PICC_TRANSFER;
    au8Buffer[1] = addr;
    _RC522_CRC(pHandle, au8Buffer, 2, &au8Buffer[2]);

    ERRCHK_RETURN(RC522_ComMF522(pHandle, PCD_TRANSCEIVE, au8Buffer, 4, au8Buffer, &u16Length));

    if ((u16Length != 4) || ((au8Buffer[0] & 0x0F) != 0x0A))
    {
        return ERR_FAIL;
    }

    return ERR_NONE;
}

uint8_t RC522_BakValue(spi_rc522_t* pHandle, uint8_t u8SrcAddr, uint8_t u8DstAddr)
{
    uint16_t u16Length;
    uint8_t  au8Buffer[MAXRLEN];

    au8Buffer[0] = PICC_RESTORE;
    au8Buffer[1] = u8SrcAddr;
    _RC522_CRC(pHandle, au8Buffer, 2, &au8Buffer[2]);

    ERRCHK_RETURN(RC522_ComMF522(pHandle, PCD_TRANSCEIVE, au8Buffer, 4, au8Buffer, &u16Length));

    if ((u16Length != 4) || ((au8Buffer[0] & 0x0F) != 0x0A))
    {
        return ERR_FAIL;
    }

    au8Buffer[0] = 0;
    au8Buffer[1] = 0;
    au8Buffer[2] = 0;
    au8Buffer[3] = 0;
    _RC522_CRC(pHandle, au8Buffer, 4, &au8Buffer[4]);

    ERRCHK_RETURN(RC522_ComMF522(pHandle, PCD_TRANSCEIVE, au8Buffer, 6, au8Buffer, &u16Length));

    au8Buffer[0] = PICC_TRANSFER;
    au8Buffer[1] = u8DstAddr;
    _RC522_CRC(pHandle, au8Buffer, 2, &au8Buffer[2]);

    ERRCHK_RETURN(RC522_ComMF522(pHandle, PCD_TRANSCEIVE, au8Buffer, 4, au8Buffer, &u16Length));

    if ((u16Length != 4) || ((au8Buffer[0] & 0x0F) != 0x0A))
    {
        return ERR_FAIL;
    }

    return ERR_NONE;
}

/**
 * @brief  命令卡片进入休眠状态
 * @retval 状态值 = ERR_NONE，成功
 */
err_t RC522_Halt(spi_rc522_t* pHandle)
{
    uint16_t u16Length;
    uint8_t  au8Buffer[MAXRLEN];

    au8Buffer[0] = PICC_HALT;
    au8Buffer[1] = 0;
    _RC522_CRC(pHandle, au8Buffer, 2, &au8Buffer[2]);

    // iIgnore the returned status
    RC522_ComMF522(pHandle, PCD_TRANSCEIVE, au8Buffer, 4, au8Buffer, &u16Length);

    return ERR_NONE;
}

void _RC522_CRC(spi_rc522_t* pHandle, uint8_t* pu8Indata, uint8_t u8Length, uint8_t* pu8OutData)
{
    uint8_t i, n;
    _RC522_ClearBitMask(pHandle, REG_DIV_IRQ, 0x04);
    _RC522_WriteRegister(pHandle, REG_COMMAND, PCD_IDLE);
    _RC522_SetBitMask(pHandle, REG_FIFO_LEVEL, 0x80);
    for (i = 0; i < u8Length; i++) { _RC522_WriteRegister(pHandle, REG_FIFO_DATA, *(pu8Indata + i)); }
    _RC522_WriteRegister(pHandle, REG_COMMAND, PCD_CALCCRC);
    i = 0xFF;
    do {
        n = _RC522_ReadRegister(pHandle, REG_DIV_IRQ);
        i--;
    } while ((i != 0) && !(n & 0x04));
    pu8OutData[0] = _RC522_ReadRegister(pHandle, REG_CRC_RESULT_L);
    pu8OutData[1] = _RC522_ReadRegister(pHandle, REG_CRC_RESULT_M);
}

/**
 * @brief  通过RC522和ISO14443卡通讯
 * @param  Command，RC522命令字
 * @param  pInData，通过RC522发送到卡片的数据
 * @param  ucInLenByte，发送数据的字节长度
 * @param  pOutData，接收到的卡片返回数据
 * @param  pOutLenBit，返回数据的位长度
 * @retval 状态值 = ERR_NONE，成功
 */
uint8_t RC522_ComMF522(spi_rc522_t* pHandle,
                       uint8_t      Command,
                       uint8_t*     pInData,
                       uint8_t      InLenByte,
                       uint8_t*     pu8OutData,
                       uint16_t*    pOutLenBit)
{
    uint8_t  irqEn   = 0x00;
    uint8_t  waitFor = 0x00;
    uint8_t  lastBits;
    uint8_t  n;
    uint16_t i;

    switch (Command)
    {
        case PCD_AUTHENT:  // Mifare认证
        {
            irqEn   = 0x12;  // 允许错误中断请求 ErrIEn, 允许空闲中断 IdleIEn
            waitFor = 0x10;  // 认证寻卡等待的时候 查询空闲中断标志位
            break;
        }
        case PCD_TRANSCEIVE:  // 接收发送 发送接收
        {
            irqEn   = 0x77;  // 允许 TxIEn RxIEn IdleIEn LoAlertIEn ErrIEn TimerIEn
            waitFor = 0x30;  // 寻卡等待的时候 查询接收中断标志位与 空闲中断标志位
            break;
        }
        default:
        {
            break;
        }
    }

    // IRqInv置位管脚IRQ与Status1Reg的IRq位的值相反
    _RC522_WriteRegister(pHandle, REG_IE, irqEn | 0x80);
    // Set1该位清零时，CommIRqReg的屏蔽位清零
    _RC522_ClearBitMask(pHandle, REG_IRQ, 0x80);
    // 写空闲命令
    _RC522_WriteRegister(pHandle, REG_COMMAND, PCD_IDLE);
    // 置位FlushBuffer清除内部FIFO的读和写指针以及ErrReg的BufferOvfl标志位被清除
    _RC522_SetBitMask(pHandle, REG_FIFO_LEVEL, 0x80);

    // 写数据进 FIFOdata
    for (i = 0; i < InLenByte; i++)
    {
        _RC522_WriteRegister(pHandle, REG_FIFO_DATA, pInData[i]);
    }

    // 写命令
    _RC522_WriteRegister(pHandle, REG_COMMAND, Command);
    if (Command == PCD_TRANSCEIVE)
    {
        // StartSend 置位启动数据发送 该位与收发命令使用时才有效
        _RC522_SetBitMask(pHandle, REG_BIT_FRAMING, 0x80);
    }

    // 根据时钟频率调整，操作M1卡最大等待时间25ms
    i = 600;
    // 认证 与寻卡等待时间
    do {
        n = _RC522_ReadRegister(pHandle, REG_IRQ);  // 查询事件中断
        i--;
    } while ((i != 0) && !(n & 0x01) && !(n & waitFor));
    _RC522_ClearBitMask(pHandle, REG_BIT_FRAMING, 0x80);  // 清理允许StartSend位

    if (i != 0)
    {
        // 读错误标志寄存器BufferOfI CollErr ParityErr ProtocolErr
        if (!(_RC522_ReadRegister(pHandle, REG_ERROR) & 0x1B))
        {
            // 是否发生定时器中断
            if (n & irqEn & 0x01)
            {
                return ERR_NOT_EXIST;  // no tag
            }

            if (Command == PCD_TRANSCEIVE)
            {
                // 读FIFO中保存的字节数
                n        = _RC522_ReadRegister(pHandle, REG_FIFO_LEVEL);
                // 最后接收到得字节的有效位数
                lastBits = _RC522_ReadRegister(pHandle, REG_CONTROL) & 0x07;
                if (lastBits)
                {
                    // N个字节数减去1（最后一个字节）+最后一位的位数 读取到的数据总位数
                    *pOutLenBit = (n - 1) * 8 + lastBits;
                }
                else
                {
                    // 最后接收到的字节整个字节有效
                    *pOutLenBit = n * 8;
                }
                if (n == 0)
                {
                    n = 1;
                }
                if (n > MAXRLEN)
                {
                    n = MAXRLEN;
                }
                for (i = 0; i < n; i++)
                {
                    pu8OutData[i] = _RC522_ReadRegister(pHandle, REG_FIFO_DATA);
                }
            }
        }
        else
        {
            return ERR_FAIL;
        }
    }
    _RC522_SetBitMask(pHandle, REG_CONTROL, 0x80);  // stop timer now
    _RC522_WriteRegister(pHandle, REG_COMMAND, PCD_IDLE);
    return ERR_NONE;
}

void RC522_AntennaOn(spi_rc522_t* pHandle)
{
    _RC522_SetBitMask(pHandle, REG_TX_CONTROL, 0x03);
}

void RC522_AntennaOff(spi_rc522_t* pHandle)
{
    _RC522_ClearBitMask(pHandle, REG_TX_CONTROL, 0x03);
}

/**
 * @brief 判断地址是否数据块
 * @param[in] u8Addr，块绝对地址（0-63）
 * @retval 0:非数据块
 * @retval 1:是数据块
 */
bool RC522_IsDataBlock(spi_rc522_t* pHandle, uint8_t u8Addr)
{
    if (u8Addr == 0)  // 块 0 不可更改
    {
        return 0;
    }

    if (u8Addr < 64)
    {
        if ((u8Addr + 1) % 4 != 0)
        {
            return true;
        }
    }

    return false;
}

//---------------------------------------------------------------------------
// Example
//---------------------------------------------------------------------------

#if CONFIG_DEMOS_SW

#include "hexdump.h"

uint8_t g_ucTempbuf[20];
uint8_t flag_loop       = 0;
uint8_t defaultKeyA[16] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

static uint8_t CompareID(uint8_t* au8CardID, uint8_t* au8CompareID)
{
    uint8_t i;

    for (i = 0; i < 5; ++i)
    {
        if (au8CardID[i] != au8CompareID[i])
        {
            return ERR_MISMACTH;  // id mismatch
        }
    }

    return ERR_NONE;
}

void RC522_ReadWriteTest(spi_rc522_t* pHandle)
{
    uint8_t u8Addr = 5;
    uint8_t au8SerNum[5];
    uint8_t au8Data[16];

    ERRCHK_EXIT(RC522_Request(pHandle, PICC_REQALL, g_ucTempbuf));
    ERRCHK_EXIT(RC522_Anticoll(pHandle, au8SerNum));
    ERRCHK_EXIT(RC522_Select(pHandle, au8SerNum));
    ERRCHK_EXIT(RC522_AuthState(pHandle, PICC_AUTHENT1A, u8Addr, defaultKeyA, au8SerNum));

    ERRCHK_EXIT(RC522_Read(pHandle, u8Addr, au8Data));  // 读取
    hexdump(au8Data, ARRAY_SIZE(au8Data), 16, 1, false, "RD: ", 0);
    for (uint8_t i = 0; i < 16; ++i) { ++au8Data[i]; }
    ERRCHK_EXIT(RC522_Write(pHandle, u8Addr, au8Data));  // 写入
    ERRCHK_EXIT(RC522_Read(pHandle, u8Addr, au8Data));   // 读取
    hexdump(au8Data, ARRAY_SIZE(au8Data), 16, 1, false, "WR: ", 0);

    RC522_Halt(pHandle);
}

void Read64Block(spi_rc522_t* pHandle)
{
    uint8_t k = 0, p = 0;
    uint8_t readdata[16];
    uint8_t serNum[5];
    for (k = 0; k < 64; ++k)
    {
        if (RC522_Request(pHandle, PICC_REQALL, g_ucTempbuf) != ERR_NONE)
        {
            continue;
        }
        if (RC522_Anticoll(pHandle, serNum) != ERR_NONE)
        {
            continue;
        }
        if (RC522_Select(pHandle, serNum) != ERR_NONE)
        {
            continue;
        }
        if (RC522_AuthState(pHandle, PICC_AUTHENT1A, k, defaultKeyA, serNum) != ERR_NONE)
        {
            continue;
        }
        PRINTF("[%03d] ", k);
        if (RC522_Read(pHandle, k, readdata) == ERR_NONE)
        {
            for (p = 0; p < 16; ++p)
            {
                PRINTF("%02x ", readdata[p]);
            }
            PRINTLN("");
            RC522_Halt(pHandle);
        }
    }
}

void RC522_Test(void)
{
#if defined(BOARD_STM32F407VET6_XWS)

    spi_mst_t spi = {
        .MISO = {GPIOA, GPIO_PIN_4}, // MISO
        .MOSI = {GPIOA, GPIO_PIN_5}, // MOSI
        .SCLK = {GPIOA, GPIO_PIN_6}, // SCK
        .CS   = {GPIOA, GPIO_PIN_3}, // SDA
    };
    spi_rc522_t rc522 = {
        .hSPI = &spi,
        .RST  = {GPIOA, GPIO_PIN_1},
    };
#elif defined(BOARD_AT32F415CB_DEV)

    spi_mst_t spi = {
        .MOSI = {GPIOB, GPIO_PINS_15}, /*MOSI*/
        .MISO = {GPIOB, GPIO_PINS_14}, /*MISO*/
        .SCLK = {GPIOB, GPIO_PINS_13}, /*SCLK*/
        .CS   = {GPIOB, GPIO_PINS_12}, /*CS*/
                                       //  .SPIx = SPI2,
    };
    spi_rc522_t rc522 = {
        .hSPI = &spi,
        .RST  = {GPIOA, GPIO_PINS_1},
    };
#endif

    SPI_Master_Init(&spi, 1000000, SPI_DUTYCYCLE_33_67, RC522_SPI_TIMING | SPI_FLAG_SOFT_CS);  // clk spd

    uint8_t MyID[5] = {0x2c, 0xff, 0xd3, 0x21};
    RC522_Init(&rc522);
    LOGI("RFID Reading\r\n");
    while (1)
    {
        if (RC522_Request(&rc522, PICC_REQALL, g_ucTempbuf) != ERR_NONE)
        {
            flag_loop = 0;
            continue;
        }
        if (RC522_Anticoll(&rc522, g_ucTempbuf) != ERR_NONE)
        {
            flag_loop = 0;
            continue;
        }
        if (flag_loop == 1)
        {
            RC522_Halt(&rc522);
            continue;
        }
        flag_loop = 1;
        LOGI("\r\n[UID] %x:%x:%x:%x", g_ucTempbuf[0], g_ucTempbuf[1], g_ucTempbuf[2], g_ucTempbuf[3]);
        RC522_Halt(&rc522);
        if (CompareID(g_ucTempbuf, MyID) == ERR_NONE)
        {
            LOGI(" [CHECK] True");
        }
        else
        {
            LOGI(" [CHECK] False");
        }
#if 0
        // PRINTF("Read 64 sectors\r\n");
        // Read64Block();
#else
        LOGI("Read Write Block");
        RC522_ReadWriteTest(&rc522);
#endif
    }
}

#endif /* CONFIG_DEMOS_SW */
