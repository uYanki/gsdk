#include "pindefs.h"
#include "platform.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "drv_pin"
#define LOG_LOCAL_LEVEL LOG_LEVEL_INFO

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

static err_t       PIN_SetMode(const pin_t* pHandle, pin_mode_e eMode, pin_pull_e ePull);
static void        PIN_WriteLevel(const pin_t* pHandle, pin_level_e eLevel);
static pin_level_e PIN_ReadLevel(const pin_t* pHandle);
static void        PIN_ToggleLevel(const pin_t* pHandle);

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

const pin_ops_t g_hwPinOps = {
    .SetMode     = PIN_SetMode,
    .ReadLevel   = PIN_ReadLevel,
    .WriteLevel  = PIN_WriteLevel,
    .ToggleLevel = PIN_ToggleLevel,
};

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

static err_t PIN_SetMode(const pin_t* pHandle, pin_mode_e eMode, pin_pull_e ePull)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    switch (eMode)
    {
        case PIN_MODE_INPUT_FLOATING: GPIO_InitStruct.Mode = GPIO_MODE_INPUT; break;
        case PIN_MODE_OUTPUT_OPEN_DRAIN: GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD; break;
        case PIN_MODE_OUTPUT_PUSH_PULL: GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; break;
        case PIN_MODE_HIGH_RESISTANCE: HAL_GPIO_DeInit(pHandle->Port, pHandle->Pin); return ERR_NONE;
        default: return MakeError(ERR_INVALID_VALUE, "invaild in/out mode");
    }

    switch (ePull)
    {
        case PIN_PULL_NONE: GPIO_InitStruct.Pull = GPIO_NOPULL; break;
        case PIN_PULL_DOWN: GPIO_InitStruct.Pull = GPIO_PULLDOWN; break;
        case PIN_PULL_UP: GPIO_InitStruct.Pull = GPIO_PULLUP; break;
        default: return MakeError(ERR_INVALID_VALUE, "invaild pull mode");
    }

    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Pin   = pHandle->Pin;
    HAL_GPIO_Init(pHandle->Port, &GPIO_InitStruct);

    return ERR_NONE;
}

static void PIN_WriteLevel(const pin_t* pHandle, pin_level_e eLevel)
{
    HAL_GPIO_WritePin(pHandle->Port, pHandle->Pin, (eLevel == PIN_LEVEL_LOW) ? GPIO_PIN_RESET : GPIO_PIN_SET);
}

static pin_level_e PIN_ReadLevel(const pin_t* pHandle)
{
    return (HAL_GPIO_ReadPin(pHandle->Port, pHandle->Pin) == GPIO_PIN_RESET) ? PIN_LEVEL_LOW : PIN_LEVEL_HIGH;
}

static void PIN_ToggleLevel(const pin_t* pHandle)
{
    HAL_GPIO_TogglePin(pHandle->Port, pHandle->Pin);
}
