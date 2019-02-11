/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H__
#define __MAIN_H__

/* Includes ------------------------------------------------------------------*/

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/

#define PB_SDA_Pin GPIO_PIN_9
#define PB_SDA_GPIO_Port GPIOB
#define PB_SCL_Pin GPIO_PIN_6
#define PB_SCL_GPIO_Port GPIOB
#define CONFIG_IIC_SDA_Pin GPIO_PIN_0
#define CONFIG_IIC_SDA_GPIO_Port GPIOF
#define CONFIG_IIC_SCL_Pin GPIO_PIN_1
#define CONFIG_IIC_SCL_GPIO_Port GPIOF
#define PA_SDA_Pin GPIO_PIN_9
#define PA_SDA_GPIO_Port GPIOC
#define PA_SCL_Pin GPIO_PIN_8
#define PA_SCL_GPIO_Port GPIOA
#define LED_GREEN_B_Pin GPIO_PIN_7
#define LED_GREEN_B_GPIO_Port GPIOC
#define VME_DET_B_Pin GPIO_PIN_3
#define VME_DET_B_GPIO_Port GPIOF
#define LED_YELLOW_B_Pin GPIO_PIN_6
#define LED_YELLOW_B_GPIO_Port GPIOC
#define LED_RED_B_Pin GPIO_PIN_6
#define LED_RED_B_GPIO_Port GPIOG
#define SYSTEM_RDY_Pin GPIO_PIN_7
#define SYSTEM_RDY_GPIO_Port GPIOF
#define PLL_M6_Pin GPIO_PIN_10
#define PLL_M6_GPIO_Port GPIOF
#define PLL_M5_Pin GPIO_PIN_9
#define PLL_M5_GPIO_Port GPIOF
#define PLL_M0_Pin GPIO_PIN_8
#define PLL_M0_GPIO_Port GPIOF
#define PLL_M0_EXTI_IRQn EXTI9_5_IRQn
#define PLL_RESET_B_Pin GPIO_PIN_0
#define PLL_RESET_B_GPIO_Port GPIOC
#define PLL_M4_Pin GPIO_PIN_1
#define PLL_M4_GPIO_Port GPIOC
#define TTY_RTS_Pin GPIO_PIN_12
#define TTY_RTS_GPIO_Port GPIOD
#define MON_SMB_DAT_Pin GPIO_PIN_12
#define MON_SMB_DAT_GPIO_Port GPIOH
#define FPGA_UART_RTS_Pin GPIO_PIN_1
#define FPGA_UART_RTS_GPIO_Port GPIOA
#define FPGA_UART_CTS_Pin GPIO_PIN_0
#define FPGA_UART_CTS_GPIO_Port GPIOA
#define FPGA_NSS_Pin GPIO_PIN_4
#define FPGA_NSS_GPIO_Port GPIOA
#define TTY_CTS_Pin GPIO_PIN_11
#define TTY_CTS_GPIO_Port GPIOD
#define MON_SMB_CLK_Pin GPIO_PIN_11
#define MON_SMB_CLK_GPIO_Port GPIOH
#define FPGA_UART_TX_Pin GPIO_PIN_2
#define FPGA_UART_TX_GPIO_Port GPIOA
#define FPGA_MISO_Pin GPIO_PIN_6
#define FPGA_MISO_GPIO_Port GPIOA
#define FPGA_SCK_Pin GPIO_PIN_5
#define FPGA_SCK_GPIO_Port GPIOA
#define ON_1_0V_1_2V_Pin GPIO_PIN_5
#define ON_1_0V_1_2V_GPIO_Port GPIOC
#define LTM_PGOOD_B_Pin GPIO_PIN_14
#define LTM_PGOOD_B_GPIO_Port GPIOF
#define ON_5V_Pin GPIO_PIN_2
#define ON_5V_GPIO_Port GPIOJ
#define ADT_CS_B0_Pin GPIO_PIN_11
#define ADT_CS_B0_GPIO_Port GPIOE
#define ADT_DIN_Pin GPIO_PIN_14
#define ADT_DIN_GPIO_Port GPIOE
#define TTY_TX_Pin GPIO_PIN_10
#define TTY_TX_GPIO_Port GPIOB
#define MON_SMB_SW_RST_B_Pin GPIO_PIN_8
#define MON_SMB_SW_RST_B_GPIO_Port GPIOH
#define MON_SMB_ALERT_B_Pin GPIO_PIN_10
#define MON_SMB_ALERT_B_GPIO_Port GPIOH
#define FPGA_UART_RX_Pin GPIO_PIN_3
#define FPGA_UART_RX_GPIO_Port GPIOA
#define FPGA_MOSI_Pin GPIO_PIN_7
#define FPGA_MOSI_GPIO_Port GPIOA
#define PLL_M3_Pin GPIO_PIN_1
#define PLL_M3_GPIO_Port GPIOB
#define FPGA_CORE_PGOOD_B_Pin GPIO_PIN_0
#define FPGA_CORE_PGOOD_B_GPIO_Port GPIOB
#define ON_1_5V_Pin GPIO_PIN_0
#define ON_1_5V_GPIO_Port GPIOJ
#define ON_3_3V_Pin GPIO_PIN_1
#define ON_3_3V_GPIO_Port GPIOJ
#define ADT_CS_B1_Pin GPIO_PIN_7
#define ADT_CS_B1_GPIO_Port GPIOE
#define ADT_CS_B2_Pin GPIO_PIN_10
#define ADT_CS_B2_GPIO_Port GPIOE
#define ADT_SCLK_Pin GPIO_PIN_12
#define ADT_SCLK_GPIO_Port GPIOE
#define ADT_CS_B3_Pin GPIO_PIN_15
#define ADT_CS_B3_GPIO_Port GPIOE
#define ADT_DOUT_Pin GPIO_PIN_13
#define ADT_DOUT_GPIO_Port GPIOE
#define TTY_RX_Pin GPIO_PIN_11
#define TTY_RX_GPIO_Port GPIOB

/* ########################## Assert Selection ############################## */
/**
  * @brief Uncomment the line below to expanse the "assert_param" macro in the
  *        HAL drivers code
  */
/* #define USE_FULL_ASSERT    1U */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
 extern "C" {
#endif
void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
