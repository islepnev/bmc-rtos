
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

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

Devices dev;

int switch_5v = 1;
int switch_3v3 = 0;
int switch_1v5 = 1;
int switch_1v0 = 0;

int monIsOn(int deviceAddr)
{
    switch(deviceAddr) {
    case 0x40: return switch_1v5;
    case 0x42: return switch_5v;
    case 0x43: return 1;
    case 0x44: return switch_3v3;
    case 0x45: return 1;
    case 0x46: return switch_1v0;
    case 0x47: return switch_1v0;
    case 0x48: return switch_1v0;
    case 0x4A: return switch_3v3;
    case 0x4B: return switch_3v3;
    case 0x4C: return switch_3v3;
    case 0x4D: return switch_3v3;
    case 0x4E: return switch_3v3;
    default: return 0;
    }

}

enum {
    MON_STATE_INIT = 0,
    MON_STATE_DETECT = 1,
    MON_STATE_READ = 2,
    MON_STATE_ERROR = 3
};

int monState = MON_STATE_INIT;
int monErrors = 0;
int monCycle = 0;

const char *monStateStr(int monState)
{
    switch(monState) {
    case MON_STATE_INIT: return "INIT";
    case MON_STATE_DETECT: return "DETECT";
    case MON_STATE_READ: return "READ";
    case MON_STATE_ERROR: return "ERROR";
    default: return "?";
    }
}

void monPrintValues(const Dev_powermon d)
{
    printf("Mon state: %s (cycle %d, errors %d) %s\n", monStateStr(monState), monCycle, monErrors, monErrors ? STR_FAIL : STR_NORMAL);
    if (monState == MON_STATE_READ) {
        for (int i=0; i<POWERMON_SENSORS; i++) {
//            uint16_t deviceAddr = monAddr[i];
//            printMonValue(deviceAddr, monValuesBus[i], monValuesShunt[i], monShuntVal(deviceAddr));
            pm_sensor_print(d.sensors[i]);
        }
    }
}

void runMon()
{
    monCycle++;
//    if (!switch_5v) {
//        monState = MON_STATE_INIT;
//        return;
//    }
    switch(monState) {
    case MON_STATE_INIT:
        struct_powermon_init(&dev.pm);
        monState = MON_STATE_DETECT;
        break;
    case MON_STATE_DETECT:
        if (monDetect(&dev.pm) == 0)
            monState = MON_STATE_READ;
        else
            monState = MON_STATE_ERROR;
        break;
    case MON_STATE_READ:
        if (monReadValues(&dev.pm) == 0)
            monState = MON_STATE_READ;
        else
            monState = MON_STATE_ERROR;
        break;
    case MON_STATE_ERROR:
        monErrors++;
        monState = MON_STATE_INIT;
        break;
    default:
        break;
    }
}

int fpga_core_pgood = 0;
int ltm_pgood = 0;

void update_power_switches()
{
    switch_5v  = 1; // monBusValid(0x43); // && monBusValid(0x45); // VME 5V and 3.3V
    switch_1v5 = 1; // monBusValid(0x42); // 5V
    switch_1v0 = ltm_pgood; // && monBusValid(0x40); // 1.5V
    switch_3v3 = fpga_core_pgood && switch_1v0 && switch_1v5; // && monBusValid(0x45);
    if (!switch_5v) {
        switch_3v3 = 0;
        switch_1v5 = 0;
        switch_1v0 = 0;
    }
    if (!switch_1v5) {
        switch_1v0 = 0;
        switch_3v3 = 0;
    }
    if (!switch_1v0) {
        switch_3v3 = 0;
    }
    HAL_GPIO_WritePin(GPIOC, ON_1_0V_1_2V_Pin, switch_1v0 ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOJ, ON_1_5V_Pin,      switch_1v5 ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOJ, ON_3_3V_Pin,      switch_3v3 ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOJ, ON_5V_Pin,        switch_5v  ? GPIO_PIN_SET : GPIO_PIN_RESET);
}
//  // Power OFF
//  HAL_GPIO_WritePin(GPIOJ, ON_5V_Pin,        GPIO_PIN_RESET);
//  HAL_GPIO_WritePin(GPIOJ, ON_3_3V_Pin,      GPIO_PIN_RESET);
//  HAL_GPIO_WritePin(GPIOJ, ON_1_5V_Pin,      GPIO_PIN_RESET);
//  HAL_GPIO_WritePin(GPIOC, ON_1_0V_1_2V_Pin, GPIO_PIN_RESET);

int readPowerGoodFpga()
{
    return (GPIO_PIN_SET == HAL_GPIO_ReadPin(FPGA_CORE_PGOOD_GPIO_Port, FPGA_CORE_PGOOD_Pin));
}
int readPowerGood1v5()
{
    return (GPIO_PIN_SET == HAL_GPIO_ReadPin(LTM_PGOOD_GPIO_Port, LTM_PGOOD_Pin));
}

const int FPGA_DEVICE_ID = 0x68; // FIXME: 0xD0


void setStaticPinsPll()
{
    //    HAL_GPIO_WritePin(PLL_M0_GPIO_Port, PLL_M0_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(PLL_M3_GPIO_Port, PLL_M3_Pin, GPIO_PIN_RESET); // M3=0 - do not load eeprom
    HAL_GPIO_WritePin(PLL_M4_GPIO_Port, PLL_M4_Pin, GPIO_PIN_SET);   // M4=1 - I2C mode
    HAL_GPIO_WritePin(PLL_M5_GPIO_Port, PLL_M5_Pin, GPIO_PIN_RESET); // M5=0 - I2C address offset
    HAL_GPIO_WritePin(PLL_M6_GPIO_Port, PLL_M6_Pin, GPIO_PIN_SET);   // M6=1 - I2C address offset
}

void setStaticPins()
{
    setStaticPinsPll();
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

  setStaticPins();
//  printf("\n%lu, %d, %lu\n", HAL_RCC_GetHCLKFreq(), HAL_GetTickFreq(), HAL_GetTick());

  switch_5v = 1;
  switch_1v5 = 1;
  switch_1v0 = 1;
  switch_3v3 = 0;
//    update_power_switches();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
      printf(ANSI_CLEARTERM ANSI_GOHOME ANSI_CLEAR);
/*
      for (int i=0; i<100000; i++) {
//          HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_13, GPIO_PIN_SET);
          HAL_GPIO_WritePin(LED_RED_B_GPIO_Port,    LED_RED_B_Pin,    GPIO_PIN_SET);
          HAL_GPIO_WritePin(LED_YELLOW_B_GPIO_Port, LED_YELLOW_B_Pin, GPIO_PIN_SET);
          HAL_GPIO_WritePin(LED_GREEN_B_GPIO_Port,  LED_GREEN_B_Pin,  GPIO_PIN_SET);
      }
      for (int i=0; i<100000; i++) {
//          HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_13, GPIO_PIN_RESET);
//          HAL_GPIO_WritePin(LED_RED_B_GPIO_Port,    LED_RED_B_Pin,    GPIO_PIN_RESET);
//          HAL_GPIO_WritePin(LED_YELLOW_B_GPIO_Port, LED_YELLOW_B_Pin, GPIO_PIN_RESET);
          HAL_GPIO_WritePin(LED_GREEN_B_GPIO_Port,  LED_GREEN_B_Pin,  GPIO_PIN_RESET);
      }
      */
      printf("Tick: %8ld \n", HAL_GetTick());
      printf("Switch 5V   %s\n", switch_5v ? STR_ON : STR_OFF);
      printf("Switch 3.3V %s\n", switch_3v3 ? STR_ON : STR_OFF);
      printf("Switch 1.5V %s\n", switch_1v5 ? STR_ON : STR_OFF);
      printf("Switch 1.0V %s\n", switch_1v0 ? STR_ON : STR_OFF);

      for (int i=0; i<10; i++) {
          update_power_switches();
          fpga_core_pgood = readPowerGoodFpga();
          ltm_pgood = readPowerGood1v5();
      }

      printf("Intermediate 1.5V: %s\n", ltm_pgood ? STR_NORMAL : switch_1v5 ? STR_FAIL : STR_OFF);
      printf("FPGA Core 1.0V:    %s\n", fpga_core_pgood ? STR_NORMAL : switch_1v0 ? STR_FAIL : STR_OFF);

//      struct_Devices_init(&dev);
      devDetect(&dev);

      if (pm_sensor_isValid(dev.pm.sensors[2])) { // 5V
          for (int i=0; i<DEV_THERM_COUNT; i++)
              dev_thset_read(&dev.thset);
          dev_thset_print(dev.thset);
      } else {
          printf("Temp: no power\n");
      }

      runMon();
      int systemPowerState = getPowerMonState(dev.pm);
      printf("System power supplies: %s\n", systemPowerState ? STR_NORMAL : STR_FAIL);
      HAL_GPIO_WritePin(LED_RED_B_GPIO_Port,    LED_RED_B_Pin,    systemPowerState ? GPIO_PIN_SET : GPIO_PIN_RESET);

//      if (systemPowerState) {
//          devDetect(&dev);
//      } else {
//      }
      devPrintStatus(dev);

      monPrintValues(dev.pm);
      fflush(stdout);
      for (int i=0; i<200000; i++) {
          HAL_GPIO_WritePin(LED_GREEN_B_GPIO_Port,  LED_GREEN_B_Pin,  GPIO_PIN_RESET);
      }
      for (int i=0; i<200000; i++) {
          HAL_GPIO_WritePin(LED_GREEN_B_GPIO_Port,  LED_GREEN_B_Pin,  GPIO_PIN_SET);
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
