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
    gpio_init_type gpio_init_struct;

    switch (eMode)
    {
        case PIN_MODE_INPUT_FLOATING: gpio_init_struct.gpio_mode = GPIO_MODE_INPUT; break;
        case PIN_MODE_OUTPUT_OPEN_DRAIN: gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT, gpio_init_struct.gpio_out_type = GPIO_OUTPUT_OPEN_DRAIN; break;
        case PIN_MODE_OUTPUT_PUSH_PULL: gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT, gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL; break;
        case PIN_MODE_HIGH_RESISTANCE: return MakeError(ERR_NOT_SUPPORTED, "unsupported mode");
        default: return MakeError(ERR_INVALID_VALUE, "invaild in/out mode");
    }

    switch (ePull)
    {
        case PIN_PULL_NONE: gpio_init_struct.gpio_pull = GPIO_PULL_NONE; break;
        case PIN_PULL_DOWN: gpio_init_struct.gpio_pull = GPIO_PULL_DOWN; break;
        case PIN_PULL_UP: gpio_init_struct.gpio_pull = GPIO_PULL_UP; break;
        default: return MakeError(ERR_INVALID_VALUE, "invaild pull mode");
    }

    gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
    gpio_init_struct.gpio_pins           = pHandle->Pin;

    gpio_init(pHandle->Port, &gpio_init_struct);

    return ERR_NONE;
}

static void PIN_WriteLevel(const pin_t* pHandle, pin_level_e eLevel)
{
    gpio_type* gpio = (gpio_type*)(pHandle->Port);
    uint16_t   pin  = pHandle->Pin;

    (eLevel == PIN_LEVEL_LOW) ? (gpio->clr = pin) : (gpio->scr = pin);
}

static pin_level_e PIN_ReadLevel(const pin_t* pHandle)
{
    gpio_type* gpio = (gpio_type*)(pHandle->Port);
    uint16_t   pin  = pHandle->Pin;

    return (pin != (pin & gpio->idt)) ? PIN_LEVEL_LOW : PIN_LEVEL_HIGH;
}

static void PIN_ToggleLevel(const pin_t* pHandle)
{
    gpio_type* gpio = (gpio_type*)(pHandle->Port);
    uint16_t   pin  = pHandle->Pin;

    gpio->odt ^= pin;
}
