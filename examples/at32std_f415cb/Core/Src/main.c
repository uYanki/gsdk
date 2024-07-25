#include "gsdk.h"
#include "hwif.h"
#include "i2c_eeprom.h"
#include "i2c_application.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "main"
#define LOG_LOCAL_LEVEL LOG_LEVEL_INFO

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

extern void DbgUartInit(uint32_t u32Baudrate);

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

static const pin_t led1 = {LED1_PIN};
static const pin_t led2 = {LED2_PIN};
static const pin_t btn  = {BUTTON_PIN};

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

int main(void)
{
    system_clock_config();

    cm_backtrace_init("demo", "at32", "V0.01");
    SEGGER_RTT_Init();

    crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
    crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);
    crm_periph_clock_enable(CRM_GPIOC_PERIPH_CLOCK, TRUE);
    crm_periph_clock_enable(CRM_GPIOD_PERIPH_CLOCK, TRUE);
    crm_periph_clock_enable(CRM_GPIOF_PERIPH_CLOCK, TRUE);

    PIN_SetMode(&led1, PIN_MODE_OUTPUT_PUSH_PULL, PIN_PULL_NONE);
    PIN_SetMode(&led2, PIN_MODE_OUTPUT_PUSH_PULL, PIN_PULL_NONE);
    PIN_SetMode(&btn, PIN_MODE_INPUT_FLOATING, PIN_PULL_NONE);

    DbgUartInit(115200);
    DelayInit();

#if 0  // eeprom demo

    i2c_mst_t i2c = {
        .SDA = {EEPROM_SDA_PIN},
        .SCL = {EEPROM_SCL_PIN},
#if 0
        .I2Cx = nullptr,
#else
        .I2Cx = I2C1,
#endif
    };

    I2C_Master_Init(&i2c, 1e6, I2C_DUTYCYCLE_67_33);
    I2C_Master_ScanAddress(&i2c);

    EEPROM_Test(&i2c);

#endif

#if 1  // lcd demo
		ST7735_Test();
#endif

    while (1)
    {
        DelayBlockMs(1000);
        PIN_ToggleLevel(&led1);
        DelayBlockMs(1000);
        PIN_ToggleLevel(&led2);
        // printf("hello\r\n");
    }
}
