
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
#include "usart.h"
#include "spi.h"
#include "fpga_spi_hal.h"
#include "adt7301_spi_hal.h"
#include "ftoa.h"
#include <string.h>

/* USER CODE BEGIN Includes */

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

/*
// convert float to string one decimal digit at a time
// assumes float is < 65536 and ARRAYSIZE is big enough
// problem: it truncates numbers at size without rounding
// str is a char array to hold the result, float is the number to convert
// size is the number of decimal digits you want
void FloatToStringNew(char *str, float f, char size)
{
       char pos;  // position in string
       char len;  // length of decimal part of result
       char* curr;  // temp holder for next digit
       int value;  // decimal digit(s) to convert
       pos = 0;  // initialize pos, just to be sure

       value = (int)f;  // truncate the floating point number
       itoa(value,str);  // this is kinda dangerous depending on the length of str
       // now str array has the digits before the decimal

       if (f < 0 )  // handle negative numbers
       {
               f *= -1;
               value *= -1;
       }

    len = strlen(str);  // find out how big the integer part was
       pos = len;  // position the pointer to the end of the integer part
       str[pos++] = '.';  // add decimal point to string

       while(pos < (size + len + 1) )  // process remaining digits
       {
               f = f - (float)value;  // hack off the whole part of the number
               f *= 10;  // move next digit over
               value = (int)f;  // get next digit
               itoa(value, curr); // convert digit to string
               str[pos++] = *curr; // add digit to result string and increment pointer
       }
}
*/
void print_therm(uint16_t rawTemp)
{
//   if (rawTemp != TEMP_RAW_ERROR) {
      int16_t temp32 = adt7301_convert_temp_adt7301_scale32(rawTemp);
      int16_t tempInt = temp32 / 32;
      int16_t tempDec = (temp32 - tempInt * 32) * 10 / 32;
      printf("%d.%01d", tempInt, tempDec);
//   }
}

void print_therm_n(int16_t temp[], int n, int fractdigits)
{
    printf("temp=");
    for (int i=0; i<n; i++) {
        int16_t temp32 = adt7301_convert_temp_adt7301_scale32(temp[i]);
        float tempf = (float)temp32 / 32;
        char str[10];
        ftoa(tempf, str, fractdigits);
        printf("%s ", str);
//        print_therm(temp[i]); printf(" ");
    }
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

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  MX_SPI1_Init();
  MX_SPI4_Init();
  /* USER CODE BEGIN 2 */

  printf("\n%lu, %d, %lu\n", HAL_RCC_GetHCLKFreq(), HAL_GetTickFreq(), HAL_GetTick());

  // Power ON
  HAL_GPIO_WritePin(GPIOC, ON_1_0V_1_2V_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOJ, ON_1_5V_Pin,      GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOJ, ON_3_3V_Pin,      GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOJ, ON_5V_Pin,        GPIO_PIN_SET);
/*
  // Power OFF
  HAL_GPIO_WritePin(GPIOJ, ON_5V_Pin,        GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOJ, ON_3_3V_Pin,      GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOJ, ON_1_5V_Pin,      GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOC, ON_1_0V_1_2V_Pin, GPIO_PIN_RESET);
*/
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */

      for (int i=0; i<100000; i++) {
//          HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_13, GPIO_PIN_SET);
          HAL_GPIO_WritePin(LED_RED_B_GPIO_Port,    LED_RED_B_Pin,    GPIO_PIN_SET);
          HAL_GPIO_WritePin(LED_YELLOW_B_GPIO_Port, LED_YELLOW_B_Pin, GPIO_PIN_SET);
          HAL_GPIO_WritePin(LED_GREEN_B_GPIO_Port,  LED_GREEN_B_Pin,  GPIO_PIN_SET);
      }
      for (int i=0; i<100000; i++) {
//          HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_13, GPIO_PIN_RESET);
          HAL_GPIO_WritePin(LED_RED_B_GPIO_Port,    LED_RED_B_Pin,    GPIO_PIN_RESET);
          HAL_GPIO_WritePin(LED_YELLOW_B_GPIO_Port, LED_YELLOW_B_Pin, GPIO_PIN_RESET);
          HAL_GPIO_WritePin(LED_GREEN_B_GPIO_Port,  LED_GREEN_B_Pin,  GPIO_PIN_RESET);
      }
      printf("\r %8ld ", HAL_GetTick());
      int fpga_core_pgood = (GPIO_PIN_SET == HAL_GPIO_ReadPin(FPGA_CORE_PGOOD_B_GPIO_Port, FPGA_CORE_PGOOD_B_Pin));
      int ltm_pgood = (GPIO_PIN_SET == HAL_GPIO_ReadPin(LTM_PGOOD_B_GPIO_Port, LTM_PGOOD_B_Pin));
      printf("pgood=%d,%d ", fpga_core_pgood, ltm_pgood);

      int16_t temp[4] = {0,0,0,0};
      for(int i=0; i<4; i++) temp[i] = adt7301_convert_temp_adt7301_scale32(adt7301_read_temp(i));
      print_therm_n(temp, 4, 1);
      uint16_t data[8] = {0,0,0,0,0,0,0,0};
      for (int i=0; i<8; i++) fpga_spi_hal_read_reg(i, &data[i]);
      printf("fpga=%4X %4X %4X %4X %4X %4X %4X %4X", data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);
      fflush(stdout);
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
  while(1)
  {
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
