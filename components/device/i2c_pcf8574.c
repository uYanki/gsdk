#include "i2c_pcf8574.h"
#include "bitops.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "pcf8574"
#define LOG_LOCAL_LEVEL LOG_LEVEL_DEBUG

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

err_t PCF8574_Init(i2c_pcf8574_t* pHandle)
{
    if (I2C_Master_IsDeviceReady(pHandle->hI2C, pHandle->u8SlvAddr, I2C_FLAG_7BIT_SLVADDR) == false)
    {
        return ThrowError(ERR_NOT_EXIST, "device doesn't exist");
    }

    return ERR_NONE;
}

err_t PCF8574_ReadPort(i2c_pcf8574_t* pHandle, uint8_t* pu8InputState)
{
    return I2C_Master_ReceiveByte(pHandle->hI2C, pHandle->u8SlvAddr, pu8InputState, I2C_FLAG_7BIT_SLVADDR);
}

err_t PCF8574_WritePort(i2c_pcf8574_t* pHandle, uint8_t u8OutputState)
{
    ERROR_CHECK_RETURN(I2C_Master_TransmitByte(pHandle->hI2C, pHandle->u8SlvAddr, u8OutputState, I2C_FLAG_7BIT_SLVADDR));
    pHandle->_u8OutputState = u8OutputState;
    return ERR_NONE;
}

err_t PCF8574_TogglePort(i2c_pcf8574_t* pHandle)
{
    return PCF8574_WritePort(pHandle, ~pHandle->_u8OutputState);
}

err_t PCF8574_ReadPins(i2c_pcf8574_t* pHandle, uint8_t u8PinMask, pin_level_e* peLevel)
{
    uint8_t u8State;
    ERROR_CHECK_RETURN(PCF8574_ReadPort(pHandle, &u8State));
    *peLevel = (u8State & u8PinMask) ? PIN_LEVEL_HIGH : PIN_LEVEL_LOW;
    return ERR_NONE;
}

err_t PCF8574_WritePins(i2c_pcf8574_t* pHandle, uint8_t u8PinMask, pin_level_e eLevel)
{
    if (eLevel == PIN_LEVEL_LOW)
    {
        return PCF8574_WritePort(pHandle, pHandle->_u8OutputState & ~u8PinMask);
    }
    else  // PIN_LEVEL_HIGH
    {
        return PCF8574_WritePort(pHandle, pHandle->_u8OutputState | u8PinMask);
    }
}

err_t PCF8574_TogglePins(i2c_pcf8574_t* pHandle, uint8_t u8PinMask)
{
    return PCF8574_WritePort(pHandle, pHandle->_u8OutputState ^ u8PinMask);
}

err_t PCF8574_ShiftLeft(i2c_pcf8574_t* pHandle, uint8_t u8Shift)
{
    return PCF8574_WritePort(pHandle, pHandle->_u8OutputState << u8Shift);
}

err_t PCF8574_ShiftRight(i2c_pcf8574_t* pHandle, uint8_t u8Shift)
{
    return PCF8574_WritePort(pHandle, pHandle->_u8OutputState >> u8Shift);
}

err_t PCF8574_RotateLeft(i2c_pcf8574_t* pHandle, uint8_t u8Shift)
{
    u8Shift &= 7;
    return PCF8574_WritePort(pHandle, (pHandle->_u8OutputState << u8Shift) | (pHandle->_u8OutputState >> (8 - u8Shift)));
}

err_t PCF8574_RotateRight(i2c_pcf8574_t* pHandle, uint8_t u8Shift)
{
    u8Shift &= 7;
    return PCF8574_WritePort(pHandle, (pHandle->_u8OutputState >> u8Shift) | (pHandle->_u8OutputState << (8 - u8Shift)));
}

err_t PCF8574_Reverse(i2c_pcf8574_t* pHandle)
{
    return PCF8574_WritePort(pHandle, revbit8(pHandle->_u8OutputState));
}

/**
 * @note 按键一端接地, 另一端接PCF8574
 */
err_t PCF8574_ReadKey(i2c_pcf8574_t* pHandle, pcf8574_pin_e ePin, bool* pbIsPressed)
{
    uint8_t     u8OutputState = pHandle->_u8OutputState;
    uint8_t     u8PinMask     = (uint8_t)ePin;
    pin_level_e ePinLevel;

    ERROR_CHECK_RETURN(PCF8574_WritePins(pHandle, u8PinMask, PIN_LEVEL_HIGH));
    ERROR_CHECK_RETURN(PCF8574_ReadPins(pHandle, u8PinMask, &ePinLevel));
    ERROR_CHECK_RETURN(PCF8574_WritePort(pHandle, u8OutputState));

    *pbIsPressed = (ePinLevel == PIN_LEVEL_LOW);

    return ERR_NONE;
}

err_t PCF8574_ScanMatrixkey4x4(i2c_pcf8574_t* pHandle, uint8_t* pu8KeyIndex)
{
    uint8_t u8InputState, u8OutputState;
    uint8_t u8PressedRow, u8PressedCol;

    GSDK_ASSERT(pu8KeyIndex != nullptr, "pointer is nullptr");

    for (uint8_t i = 0; i < 4; ++i)
    {
        u8OutputState = 0xF0 | ~BV(i);

        ERROR_CHECK_RETURN(PCF8574_WritePort(pHandle, u8OutputState));
        ERROR_CHECK_RETURN(PCF8574_ReadPort(pHandle, &u8InputState));

        u8InputState = ~u8InputState & 0xF0;

        if (u8InputState == 0)
        {
            continue;
        }

        u8PressedRow = i;

        switch (u8InputState >> 4)
        {
            case PCF8574_PIN_0: u8PressedCol = 0; break;
            case PCF8574_PIN_1: u8PressedCol = 1; break;
            case PCF8574_PIN_2: u8PressedCol = 2; break;
            case PCF8574_PIN_3: u8PressedCol = 3; break;
        }

        *pu8KeyIndex = u8PressedRow * 4 + u8PressedCol;

        LOGD("key pressed %02d (at row %d, col %d)", *pu8KeyIndex, u8PressedRow, u8PressedCol);

        break;
    }

    return ERR_NONE;
}

//---------------------------------------------------------------------------
// Example
//---------------------------------------------------------------------------

#if CONFIG_DEMOS_SW

void PCF8574_Test(i2c_mst_t* hI2C)
{
    i2c_pcf8574_t pcf8574 = {
        .hI2C      = hI2C,
        .u8SlvAddr = PCF8574_ADDRESS_A101,
    };

    PCF8574_Init(&pcf8574);

#if 0  // led

    LOGI("write port");
    PCF8574_WritePort(&pcf8574, PCF8574_PIN_NONE);  // all off
    DelayBlockMs(500);
    PCF8574_WritePort(&pcf8574, PCF8574_PIN_ALL);  // all on
    DelayBlockMs(500);
    PCF8574_WritePort(&pcf8574, PCF8574_PIN_NONE);  // all off
    DelayBlockMs(500);
    PCF8574_TogglePort(&pcf8574);  // all on
    DelayBlockMs(500);
    PCF8574_WritePort(&pcf8574, PCF8574_PIN_NONE);  // all off
    DelayBlockMs(500);

    LOGI("write pin");
    PCF8574_WritePins(&pcf8574, PCF8574_PIN_0 | PCF8574_PIN_2 | PCF8574_PIN_4 | PCF8574_PIN_6, PIN_LEVEL_HIGH);  // set
    DelayBlockMs(500);
    PCF8574_WritePins(&pcf8574, PCF8574_PIN_1 | PCF8574_PIN_3 | PCF8574_PIN_5 | PCF8574_PIN_7, PIN_LEVEL_HIGH);  // set
    DelayBlockMs(500);
    PCF8574_WritePins(&pcf8574, PCF8574_PIN_0 | PCF8574_PIN_2 | PCF8574_PIN_4 | PCF8574_PIN_6, PIN_LEVEL_LOW);  // reset
    DelayBlockMs(500);
    PCF8574_WritePins(&pcf8574, PCF8574_PIN_1 | PCF8574_PIN_3 | PCF8574_PIN_5 | PCF8574_PIN_7, PIN_LEVEL_LOW);  // reset
    DelayBlockMs(500);

    LOGI("reverse");
    PCF8574_WritePins(&pcf8574, PCF8574_PIN_0 | PCF8574_PIN_1 | PCF8574_PIN_3, PIN_LEVEL_HIGH);  // reset
    DelayBlockMs(500);
    PCF8574_Reverse(&pcf8574);
    DelayBlockMs(500);
    PCF8574_Reverse(&pcf8574);
    DelayBlockMs(500);

    LOGI("shift left");

    PCF8574_WritePort(&pcf8574, PCF8574_PIN_ALL);  // set

    for (uint8_t i = 0; i < 10; i++)
    {
        PCF8574_ShiftLeft(&pcf8574, 1);
        DelayBlockMs(300);
    }

    LOGI("shift right");

    PCF8574_WritePort(&pcf8574, PCF8574_PIN_ALL);  // set

    for (uint8_t i = 0; i < 10; i++)
    {
        PCF8574_ShiftRight(&pcf8574, 1);
        DelayBlockMs(300);
    }

    LOGI("rotate");

    PCF8574_WritePort(&pcf8574, PCF8574_PIN_0 | PCF8574_PIN_2);  // set

    for (uint8_t i = 0; i < 16; i++)
    {
        PCF8574_RotateLeft(&pcf8574, 1);
        DelayBlockMs(300);
    }

    for (uint8_t i = 0; i < 16; i++)
    {
        PCF8574_RotateRight(&pcf8574, 1);
        DelayBlockMs(300);
    }

    LOGI("toggle port");

    PCF8574_WritePort(&pcf8574, PCF8574_PIN_0 | PCF8574_PIN_2);  // set

    for (uint8_t i = 0; i < 8; i++)
    {
        PCF8574_TogglePort(&pcf8574);
        DelayBlockMs(300);
    }

    LOGI("toggle pin");

    for (uint8_t i = 0; i < 8; i++)
    {
        PCF8574_TogglePins(&pcf8574, PCF8574_PIN_0 | PCF8574_PIN_2);
        DelayBlockMs(300);
    }

    while (1)
    {
    }

#else  // key

    LOGI("scan key");

    while (1)
    {
        static uint8_t u8KeyIdx;
        PCF8574_ScanMatrixkey4x4(&pcf8574, &u8KeyIdx);
    }

#endif
}

#endif
