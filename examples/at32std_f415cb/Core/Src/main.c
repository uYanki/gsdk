#include "gsdk.h"
#include "hwif.h"
#include "i2c_eeprom.h"
#include "platform.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "main"
#define LOG_LOCAL_LEVEL LOG_LEVEL_INFO

#define I2C_TIMEOUT                      0xFFFFFFFF

#define I2Cx_SPEED                       100000
#define I2Cx_ADDRESS                     0xA0

#define I2Cx_PORT                        I2C1
#define I2Cx_CLK                         CRM_I2C1_PERIPH_CLOCK

#define I2Cx_SCL_PIN                     GPIO_PINS_6
#define I2Cx_SCL_GPIO_PORT               GPIOB
#define I2Cx_SCL_GPIO_CLK                CRM_GPIOB_PERIPH_CLOCK

#define I2Cx_SDA_PIN                     GPIO_PINS_7
#define I2Cx_SDA_GPIO_PORT               GPIOB
#define I2Cx_SDA_GPIO_CLK                CRM_GPIOB_PERIPH_CLOCK

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

#include "i2c_application.h"

i2c_handle_type hi2cx;

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
	
	  hi2cx.i2cx = I2Cx_PORT;
    i2c_config(&hi2cx);

#if 1  // eeprom demo

    i2c_mst_t i2c = {
        .SDA = {EEPROM_SDA_PIN},
        .SCL = {EEPROM_SCL_PIN},
#if 0
        .I2Cx = nullptr,
#else
        .I2Cx = &hi2cx,
#endif
    };

    I2C_Master_Init(&i2c, 1e5, I2C_DUTYCYCLE_50_50);
    I2C_Master_ScanAddress(&i2c);

    EEPROM_Test(&i2c);

#endif

    while (1)
    {
        DelayBlockMs(1000);
        PIN_ToggleLevel(&led1);
        DelayBlockMs(1000);
        PIN_ToggleLevel(&led2);
        printf("hello\r\n");
    }
}


/**
  * @brief  initializes peripherals used by the i2c.
  * @param  none
  * @retval none
  */
void i2c_lowlevel_init(i2c_handle_type* hi2c)
{
  gpio_init_type gpio_initstructure;

  if(hi2c->i2cx == I2Cx_PORT)
  {
    /* i2c periph clock enable */
    crm_periph_clock_enable(I2Cx_CLK, TRUE);
    crm_periph_clock_enable(I2Cx_SCL_GPIO_CLK, TRUE);
    crm_periph_clock_enable(I2Cx_SDA_GPIO_CLK, TRUE);

    /* gpio configuration */
    gpio_initstructure.gpio_out_type       = GPIO_OUTPUT_OPEN_DRAIN;
    gpio_initstructure.gpio_pull           = GPIO_PULL_UP;
    gpio_initstructure.gpio_mode           = GPIO_MODE_MUX;
    gpio_initstructure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;

    /* configure i2c pins: scl */
    gpio_initstructure.gpio_pins = I2Cx_SCL_PIN;
    gpio_init(I2Cx_SCL_GPIO_PORT, &gpio_initstructure);

    /* configure i2c pins: sda */
    gpio_initstructure.gpio_pins = I2Cx_SDA_PIN;
    gpio_init(I2Cx_SDA_GPIO_PORT, &gpio_initstructure);

    i2c_init(hi2c->i2cx, I2C_FSMODE_DUTY_2_1, I2Cx_SPEED);

    i2c_own_address1_set(hi2c->i2cx, I2C_ADDRESS_MODE_7BIT, I2Cx_ADDRESS);
  }
}

