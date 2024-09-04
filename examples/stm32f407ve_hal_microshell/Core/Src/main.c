/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32f4xx_hal_uart.h"
#include "gdefs.h"
#include "microshell.h"
#include "pinmap.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

// working buffers allocations (size could be customized)
#define BUF_IN_SIZE   128
#define BUF_OUT_SIZE  128
#define PATH_MAX_SIZE 128

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

static int ush_read(struct ush_object* self, char* ch);
static int ush_write(struct ush_object* self, char ch);

static void toggle_exec_callback(struct ush_object* self, struct ush_file_descriptor const* file, int argc, char* argv[]);
static void reboot_exec_callback(struct ush_object* self, struct ush_file_descriptor const* file, int argc, char* argv[]);
static void set_exec_callback(struct ush_object* self, struct ush_file_descriptor const* file, int argc, char* argv[]);
size_t      info_get_data_callback(struct ush_object* self, struct ush_file_descriptor const* file, uint8_t** data);
size_t      led_get_data_callback(struct ush_object* self, struct ush_file_descriptor const* file, uint8_t** data);
void        led_set_data_callback(struct ush_object* self, struct ush_file_descriptor const* file, uint8_t* data, size_t size);
size_t      time_get_data_callback(struct ush_object* self, struct ush_file_descriptor const* file, uint8_t** data);

static char ush_in_buf[BUF_IN_SIZE];
static char ush_out_buf[BUF_OUT_SIZE];

// I/O interface descriptor
static const struct ush_io_interface ush_iface = {
    .read  = ush_read,
    .write = ush_write,
};

// microshell instance handler
static struct ush_object ush;

// microshell descriptor
static const struct ush_descriptor ush_desc = {
    .io                 = &ush_iface,           // I/O interface pointer
    .input_buffer       = ush_in_buf,           // working input buffer
    .input_buffer_size  = sizeof(ush_in_buf),   // working input buffer size
    .output_buffer      = ush_out_buf,          // working output buffer
    .output_buffer_size = sizeof(ush_out_buf),  // working output buffer size
    .path_max_length    = PATH_MAX_SIZE,        // path maximum length (stack)
    .hostname           = "ushell",             // hostname (in prompt)
};

// clang-format off

// root directory files descriptor
static const struct ush_file_descriptor root_files[] = {
    {
        .name = "info.txt",                     // info.txt file name
        .description = NULL,
        .help = NULL,
        .exec = NULL,
        .get_data = info_get_data_callback,
    }
};

// bin directory files descriptor
static const struct ush_file_descriptor bin_files[] = {
    {
        .name = "toggle",                       // toggle file name
        .description = "toggle led",            // optional file description
        .help = "usage: toggle\r\n",            // optional help manual
        .exec = toggle_exec_callback,           // optional execute callback
    },
    {
        .name = "set",                          // set file name
        .description = "set led",
        .help = "usage: set {0,1}\r\n",
        .exec = set_exec_callback
    },
};

// dev directory files descriptor
static const struct ush_file_descriptor dev_files[] = {
    {
        .name = "led",
        .description = NULL,
        .help = NULL,
        .exec = NULL,
        .get_data = led_get_data_callback,      // optional data getter callback
        .set_data = led_set_data_callback,      // optional data setter callback
    },
    {
        .name = "time",
        .description = NULL,
        .help = NULL,
        .exec = NULL,
        .get_data = time_get_data_callback,
    },
};

// cmd files descriptor
static const struct ush_file_descriptor cmd_files[] = {
    {
        .name = "reboot",
        .description = "reboot device",
        .help = NULL,
        .exec = reboot_exec_callback,
    },
};

// clang-format on

// root directory handler
static struct ush_node_object root;
// dev directory handler
static struct ush_node_object dev;
// bin directory handler
static struct ush_node_object bin;

// cmd commands handler
static struct ush_node_object cmd;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
    /* USER CODE BEGIN 1 */

    /* USER CODE END 1 */

    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_DMA_Init();
    MX_USART1_UART_Init();
    /* USER CODE BEGIN 2 */

    DelayInit();

    /**
     * @example
     *
     *  > help
     *  > echo helloword
     *
     *  > cat info.txt
     *
     *  > cd dev
     *  > ls
     *  > cat time
     *  > echo 0 > led
     *  > echo 1 > led
     *
     *  > cd ..
     *  > cd bin
     *
     */

    // initialize microshell instance
    ush_init(&ush, &ush_desc);

    // add custom commands
    ush_commands_add(&ush, &cmd, cmd_files, sizeof(cmd_files) / sizeof(cmd_files[0]));

    // mount root directory (root must be first)
    ush_node_mount(&ush, "/", &root, root_files, sizeof(root_files) / sizeof(root_files[0]));
    // mount dev directory
    ush_node_mount(&ush, "/dev", &dev, dev_files, sizeof(dev_files) / sizeof(dev_files[0]));
    // mount bin directory
    ush_node_mount(&ush, "/bin", &bin, bin_files, sizeof(bin_files) / sizeof(bin_files[0]));

    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */

    while (1)
    {
        // non-blocking microshell service
        ush_service(&ush);

        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
    }
    /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Configure the main internal regulator output voltage
     */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState            = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource       = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM            = 8;
    RCC_OscInitStruct.PLL.PLLN            = 168;
    RCC_OscInitStruct.PLL.PLLP            = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ            = 7;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType      = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
    {
        Error_Handler();
    }
}

/* USER CODE BEGIN 4 */

// non-blocking read interface
static int ush_read(struct ush_object* self, char* ch)
{
    if (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_RXNE))
    {
        return HAL_UART_Receive(&huart1, (uint8_t*)ch, 1, 1) == HAL_OK;
    }

    return 0;
}

// non-blocking write interface
static int ush_write(struct ush_object* self, char ch)
{
    return HAL_UART_Transmit(&huart1, (uint8_t*)&ch, 1, 1) == HAL_OK;
}

// toggle file execute callback
static void toggle_exec_callback(struct ush_object* self, struct ush_file_descriptor const* file, int argc, char* argv[])
{
    // simple toggle led, without any arguments validation
    HAL_GPIO_TogglePin(LED1_PIN);
}

// reboot cmd file execute callback
static void reboot_exec_callback(struct ush_object* self, struct ush_file_descriptor const* file, int argc, char* argv[])
{
    HAL_NVIC_SystemReset();
}

// set file execute callback
static void set_exec_callback(struct ush_object* self, struct ush_file_descriptor const* file, int argc, char* argv[])
{
    // arguments count validation
    if (argc != 2)
    {
        // return predefined error message
        ush_print_status(self, USH_STATUS_ERROR_COMMAND_WRONG_ARGUMENTS);
        return;
    }

    // arguments validation
    if (strcmp(argv[1], "1") == 0)
    {
        // turn led on
        HAL_GPIO_WritePin(LED1_PIN, GPIO_PIN_RESET);
    }
    else if (strcmp(argv[1], "0") == 0)
    {
        // turn led off
        HAL_GPIO_WritePin(LED1_PIN, GPIO_PIN_SET);
    }
    else
    {
        // return predefined error message
        ush_print_status(self, USH_STATUS_ERROR_COMMAND_WRONG_ARGUMENTS);
        return;
    }
}

// info file get data callback
size_t info_get_data_callback(struct ush_object* self, struct ush_file_descriptor const* file, uint8_t** data)
{
    static const char* info = "Use MicroShell and make fun!\r\n";

    // return pointer to data
    *data = (uint8_t*)info;
    // return data size
    return strlen(info);
}

// led file get data callback
size_t led_get_data_callback(struct ush_object* self, struct ush_file_descriptor const* file, uint8_t** data)
{
    // read current led state
    bool state = HAL_GPIO_ReadPin(LED1_PIN) == GPIO_PIN_RESET;
    //  return pointer to data
    *data      = (uint8_t*)((state) ? "1\r\n" : "0\r\n");
    // return data size
    return strlen((char*)(*data));
}

// led file set data callback
void led_set_data_callback(struct ush_object* self, struct ush_file_descriptor const* file, uint8_t* data, size_t size)
{
    // data size validation
    if (size < 1)
    {
        return;
    }

    // arguments validation
    if (data[0] == '1')
    {
        // turn led on
        HAL_GPIO_WritePin(LED1_PIN, GPIO_PIN_RESET);
    }
    else if (data[0] == '0')
    {
        // turn led off
        HAL_GPIO_WritePin(LED1_PIN, GPIO_PIN_SET);
    }
}

// time file get data callback
size_t time_get_data_callback(struct ush_object* self, struct ush_file_descriptor const* file, uint8_t** data)
{
    static char time_buf[16];
    // read current time
    long        current_time = GetTickUs() / 1000;
    // convert
    snprintf(time_buf, sizeof(time_buf), "%ld\r\n", current_time);
    time_buf[sizeof(time_buf) - 1] = 0;
    // return pointer to data
    *data                          = (uint8_t*)time_buf;
    // return data size
    return strlen((char*)(*data));
}

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1)
    {
    }
    /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       ex: PRINTF("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
