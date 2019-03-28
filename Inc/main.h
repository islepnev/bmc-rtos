/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal.h"
#include "stm32f7xx_ll_usart.h"
#include "stm32f7xx_ll_rcc.h"
#include "stm32f7xx_ll_bus.h"
#include "stm32f7xx_ll_cortex.h"
#include "stm32f7xx_ll_system.h"
#include "stm32f7xx_ll_utils.h"
#include "stm32f7xx_ll_pwr.h"
#include "stm32f7xx_ll_gpio.h"
#include "stm32f7xx_ll_dma.h"

#include "stm32f7xx_ll_exti.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define ADT_CS_B0_GPIO_Port GPIOE
#define ADT_CS_B0_Pin GPIO_PIN_11
#define ADT_CS_B1_GPIO_Port GPIOE
#define ADT_CS_B1_Pin GPIO_PIN_7
#define ADT_CS_B2_GPIO_Port GPIOE
#define ADT_CS_B2_Pin GPIO_PIN_10
#define ADT_CS_B3_GPIO_Port GPIOE
#define ADT_CS_B3_Pin GPIO_PIN_15
#define ADT_DIN_GPIO_Port GPIOE
#define ADT_DIN_Pin GPIO_PIN_14
#define ADT_DOUT_GPIO_Port GPIOE
#define ADT_DOUT_Pin GPIO_PIN_13
#define ADT_SCLK_GPIO_Port GPIOE
#define ADT_SCLK_Pin GPIO_PIN_12
#define CONFIG_IIC_SCL_GPIO_Port GPIOF
#define CONFIG_IIC_SCL_Pin GPIO_PIN_1
#define CONFIG_IIC_SDA_GPIO_Port GPIOF
#define CONFIG_IIC_SDA_Pin GPIO_PIN_0
#define FPGA_CORE_PGOOD_GPIO_Port GPIOB
#define FPGA_CORE_PGOOD_Pin GPIO_PIN_0
#define FPGA_MISO_GPIO_Port GPIOA
#define FPGA_MISO_Pin GPIO_PIN_6
#define FPGA_MOSI_GPIO_Port GPIOA
#define FPGA_MOSI_Pin GPIO_PIN_7
#define FPGA_NSS_GPIO_Port GPIOA
#define FPGA_NSS_Pin GPIO_PIN_4
#define FPGA_SCK_GPIO_Port GPIOA
#define FPGA_SCK_Pin GPIO_PIN_5
#define FPGA_UART_CTS_GPIO_Port GPIOA
#define FPGA_UART_CTS_Pin GPIO_PIN_0
#define FPGA_UART_RTS_GPIO_Port GPIOA
#define FPGA_UART_RTS_Pin GPIO_PIN_1
#define FPGA_UART_RX_GPIO_Port GPIOA
#define FPGA_UART_RX_Pin GPIO_PIN_3
#define FPGA_UART_TX_GPIO_Port GPIOA
#define FPGA_UART_TX_Pin GPIO_PIN_2
#define LED_GREEN_B_GPIO_Port GPIOC
#define LED_GREEN_B_Pin GPIO_PIN_7
#define LED_RED_B_GPIO_Port GPIOG
#define LED_RED_B_Pin GPIO_PIN_6
#define LED_YELLOW_B_GPIO_Port GPIOC
#define LED_YELLOW_B_Pin GPIO_PIN_6
#define LTM_PGOOD_GPIO_Port GPIOF
#define LTM_PGOOD_Pin GPIO_PIN_14
#define MON_SMB_ALERT_B_GPIO_Port GPIOH
#define MON_SMB_ALERT_B_Pin GPIO_PIN_10
#define MON_SMB_CLK_GPIO_Port GPIOH
#define MON_SMB_CLK_Pin GPIO_PIN_11
#define MON_SMB_DAT_GPIO_Port GPIOH
#define MON_SMB_DAT_Pin GPIO_PIN_12
#define MON_SMB_SW_RST_B_GPIO_Port GPIOH
#define MON_SMB_SW_RST_B_Pin GPIO_PIN_8
#define ON_1_0V_1_2V_GPIO_Port GPIOC
#define ON_1_0V_1_2V_Pin GPIO_PIN_5
#define ON_1_5V_GPIO_Port GPIOJ
#define ON_1_5V_Pin GPIO_PIN_0
#define ON_3_3V_GPIO_Port GPIOJ
#define ON_3_3V_Pin GPIO_PIN_1
#define ON_5V_GPIO_Port GPIOJ
#define ON_5V_Pin GPIO_PIN_2
#define PA_SCL_GPIO_Port GPIOA
#define PA_SCL_Pin GPIO_PIN_8
#define PA_SDA_GPIO_Port GPIOC
#define PA_SDA_Pin GPIO_PIN_9
#define PB_SCL_GPIO_Port GPIOB
#define PB_SCL_Pin GPIO_PIN_6
#define PB_SDA_GPIO_Port GPIOB
#define PB_SDA_Pin GPIO_PIN_9
#define PLL_M0_GPIO_Port GPIOF
#define PLL_M0_Pin GPIO_PIN_8
#define PLL_M3_GPIO_Port GPIOB
#define PLL_M3_Pin GPIO_PIN_1
#define PLL_M4_GPIO_Port GPIOC
#define PLL_M4_Pin GPIO_PIN_1
#define PLL_M5_GPIO_Port GPIOF
#define PLL_M5_Pin GPIO_PIN_9
#define PLL_M6_GPIO_Port GPIOF
#define PLL_M6_Pin GPIO_PIN_10
#define PLL_RESET_B_GPIO_Port GPIOC
#define PLL_RESET_B_Pin GPIO_PIN_0
#define SYSTEM_RDY_GPIO_Port GPIOF
#define SYSTEM_RDY_Pin GPIO_PIN_7
#define TTY_CTS_GPIO_Port GPIOD
#define TTY_CTS_Pin GPIO_PIN_11
#define TTY_RTS_GPIO_Port GPIOD
#define TTY_RTS_Pin GPIO_PIN_12
#define TTY_RX_GPIO_Port GPIOB
#define TTY_RX_Pin GPIO_PIN_11
#define TTY_TX_GPIO_Port GPIOB
#define TTY_TX_Pin GPIO_PIN_10
#define VME_DET_B_GPIO_Port GPIOF
#define VME_DET_B_Pin GPIO_PIN_3
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
