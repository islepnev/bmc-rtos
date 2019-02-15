
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f7xx_hal.h"
#include "gpio.h"
#include "i2c.h"
#include "spi.h"
#include "usart.h"

/* USER CODE BEGIN Includes */
#include "fpga_spi_hal.h"
#include "adt7301_spi_hal.h"
#include "pca9548_i2c_hal.h"
#include "ad9545_i2c_hal.h"
#include "ina226_i2c_hal.h"
#include "dev_eeprom.h"
#include "dev_powermon.h"
#include "ftoa.h"
#include "ansi_escape_codes.h"
#include "display.h"
#include "devices.h"
#include "dev_types.h"
#include "dev_mcu.h"
#include "dev_leds.h"

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

uint32_t mainloopCount = 0;

uint32_t heartbeatCount = 0;
const uint32_t heartbeatInterval = 50;
uint32_t heartbeatUpdateTick = 999999; // run in first loop

uint32_t displayUpdateCount = 0;
const uint32_t displayUpdateInterval = 1000;
uint32_t displayUpdateTick = 999999;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

Devices dev;

void setStaticPins(void)
{
    pllSetStaticPins();
}

void task_oneshot(void)
{
    setStaticPins();
    // test leds
    for (int i=0; i<50000; i++) {
        dev_led_set(&dev.leds, LED_RED,    LED_ON);
        dev_led_set(&dev.leds, LED_YELLOW, LED_ON);
        dev_led_set(&dev.leds, LED_GREEN,  LED_ON);
    }
    for (int i=0; i<50000; i++) {
        dev_led_set(&dev.leds, LED_RED,    LED_OFF);
        dev_led_set(&dev.leds, LED_YELLOW, LED_OFF);
        dev_led_set(&dev.leds, LED_GREEN,  LED_OFF);
    }
    // display error state
    dev_led_set(&dev.leds, LED_RED,    LED_ON);
    dev_led_set(&dev.leds, LED_YELLOW, LED_ON);
    dev_led_set(&dev.leds, LED_GREEN,  LED_OFF);
}

void task_main(void)
{
    mainloopCount++;
    // Switch ON
    //      dev_switchPower(&dev, SWITCH_OFF);
    dev_switchPower(&dev, SWITCH_ON);
    //      HAL_Delay(1500);

    //      struct_Devices_init(&dev);
    DeviceStatus devStatus = devDetect(&dev);
    dev_led_set(&dev.leds, LED_YELLOW, devStatus != DEVICE_NORMAL);

    dev_read_thermometers(&dev);

    runMon(&dev.pm);
    int systemPowerState = getPowerMonState(dev.pm);
    dev_led_set(&dev.leds, LED_RED, !systemPowerState);
}

void task_heartbeat(void)
{
    dev_leds_toggle(&dev.leds, LED_GREEN);
}

void task_display(void)
{
    printf(ANSI_CLEARTERM ANSI_GOHOME ANSI_CLEAR);

    printf("CPU %lX rev %lX, HAL %lX, UID %08lX-%08lX-%08lX\n",
           HAL_GetDEVID(), HAL_GetREVID(),
           HAL_GetHalVersion(),
           HAL_GetUIDw0(), HAL_GetUIDw1(), HAL_GetUIDw2()
           );
    printf("Uptime: %-8ld Mainloop %-8ld Heartbeat %-6ld DisplayUpdate %-6ld\n",
           HAL_GetTick() / getTickFreqHz(), mainloopCount, heartbeatCount, displayUpdateCount);
    print_pm_switches(dev.pm.sw);
    int systemPowerState = getPowerMonState(dev.pm);
    printf("System power supplies: %s\n", systemPowerState ? STR_RESULT_NORMAL : STR_RESULT_FAIL);
    pm_pgood_print(dev.pm);
    dev_print_thermometers(dev);
    devPrintStatus(dev);
    monPrintValues(dev.pm);
    fflush(stdout);

}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void)
{
    /* USER CODE BEGIN 1 */

    /* USER CODE END 1 */

    /* MCU Configuration----------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* USER CODE BEGIN Init */
    struct_Devices_init(&dev);
    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_USART2_UART_Init();
    MX_USART3_UART_Init();
    MX_I2C1_Init();
    MX_I2C2_Init();
    //  MX_I2C3_SMBUS_Init();
    MX_I2C4_Init();
    MX_SPI1_Init();
    MX_SPI4_Init();
    /* USER CODE BEGIN 2 */

    task_oneshot();

    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1)
    {

        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
        task_main();

        if (HAL_GetTick() - heartbeatUpdateTick > heartbeatInterval) {
            heartbeatUpdateTick = HAL_GetTick();
            heartbeatCount++;
            task_heartbeat();
        };
        if (HAL_GetTick() - displayUpdateTick > displayUpdateInterval) {
            displayUpdateTick = HAL_GetTick();
            displayUpdateCount++;
            task_display();
        }
    }
    /* USER CODE END 3 */

}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{

    RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

    /**Configure the main internal regulator output voltage
    */
    __HAL_RCC_PWR_CLK_ENABLE();

    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /**Initializes the CPU, AHB and APB busses clocks
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 12;
    RCC_OscInitStruct.PLL.PLLN = 216;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 2;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        _Error_Handler(__FILE__, __LINE__);
    }

    /**Activate the Over-Drive mode
    */
    if (HAL_PWREx_EnableOverDrive() != HAL_OK)
    {
        _Error_Handler(__FILE__, __LINE__);
    }

    /**Initializes the CPU, AHB and APB busses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
            |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
    {
        _Error_Handler(__FILE__, __LINE__);
    }

    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART2|RCC_PERIPHCLK_USART3
            |RCC_PERIPHCLK_I2C1|RCC_PERIPHCLK_I2C2
            |RCC_PERIPHCLK_I2C3|RCC_PERIPHCLK_I2C4;
    PeriphClkInitStruct.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
    PeriphClkInitStruct.Usart3ClockSelection = RCC_USART3CLKSOURCE_PCLK1;
    PeriphClkInitStruct.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
    PeriphClkInitStruct.I2c2ClockSelection = RCC_I2C2CLKSOURCE_PCLK1;
    PeriphClkInitStruct.I2c3ClockSelection = RCC_I2C3CLKSOURCE_PCLK1;
    PeriphClkInitStruct.I2c4ClockSelection = RCC_I2C4CLKSOURCE_PCLK1;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
        _Error_Handler(__FILE__, __LINE__);
    }

    /**Configure the Systick interrupt time
    */
    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick
    */
    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

    /* SysTick_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    printf("HAL ERROR %s %d\n", file, line);
    while(1)
    {
        HAL_GPIO_WritePin(LED_GREEN_B_GPIO_Port, LED_GREEN_B_Pin, GPIO_PIN_RESET);
        int k = 100000;
        for (int i=0; i<k; i++) {
            HAL_GPIO_WritePin(LED_RED_B_GPIO_Port, LED_RED_B_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(LED_YELLOW_B_GPIO_Port, LED_YELLOW_B_Pin, GPIO_PIN_RESET);
        }
        for (int i=0; i<k; i++) {
            HAL_GPIO_WritePin(LED_YELLOW_B_GPIO_Port, LED_YELLOW_B_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(LED_RED_B_GPIO_Port, LED_RED_B_Pin, GPIO_PIN_RESET);
        }
    }
    /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
