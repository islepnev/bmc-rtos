/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
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
#include "stm32f3xx_hal.h"
#include "stm32f3xx_ll_usart.h"
#include "stm32f3xx_ll_rcc.h"
#include "stm32f3xx_ll_bus.h"
#include "stm32f3xx_ll_cortex.h"
#include "stm32f3xx_ll_system.h"
#include "stm32f3xx_ll_utils.h"
#include "stm32f3xx_ll_pwr.h"
#include "stm32f3xx_ll_gpio.h"
#include "stm32f3xx_ll_dma.h"

#include "stm32f3xx_ll_exti.h"

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
#define TTY_TX_Pin GPIO_PIN_2
#define TTY_TX_GPIO_Port GPIOA
#define TTY_RX_Pin GPIO_PIN_3
#define TTY_RX_GPIO_Port GPIOA
#define PLL_CS_Pin GPIO_PIN_4
#define PLL_CS_GPIO_Port GPIOA
#define PLL_SCLK_Pin GPIO_PIN_5
#define PLL_SCLK_GPIO_Port GPIOA
#define PLL_MOSI_Pin GPIO_PIN_7
#define PLL_MOSI_GPIO_Port GPIOA
#define PLL_LED_EN_B_Pin GPIO_PIN_0
#define PLL_LED_EN_B_GPIO_Port GPIOB
#define LED_RED_B_Pin GPIO_PIN_2
#define LED_RED_B_GPIO_Port GPIOB
#define LED_YELLOW_B_Pin GPIO_PIN_10
#define LED_YELLOW_B_GPIO_Port GPIOB
#define LED_GREEN_B_Pin GPIO_PIN_11
#define LED_GREEN_B_GPIO_Port GPIOB
#define FPGA_NSS_Pin GPIO_PIN_12
#define FPGA_NSS_GPIO_Port GPIOB
#define FPGA_SCLK_Pin GPIO_PIN_13
#define FPGA_SCLK_GPIO_Port GPIOB
#define FPGA_MISO_Pin GPIO_PIN_14
#define FPGA_MISO_GPIO_Port GPIOB
#define FPGA_MOSI_Pin GPIO_PIN_15
#define FPGA_MOSI_GPIO_Port GPIOB
#define ADT_CS_B1_Pin GPIO_PIN_9
#define ADT_CS_B1_GPIO_Port GPIOA
#define ADT_CS_B3_Pin GPIO_PIN_10
#define ADT_CS_B3_GPIO_Port GPIOA
#define ADT_CS_B2_Pin GPIO_PIN_11
#define ADT_CS_B2_GPIO_Port GPIOA
#define ADT_CS_B0_Pin GPIO_PIN_15
#define ADT_CS_B0_GPIO_Port GPIOA
#define ADT_SCLK_Pin GPIO_PIN_3
#define ADT_SCLK_GPIO_Port GPIOB
#define ADT_DOUT_Pin GPIO_PIN_4
#define ADT_DOUT_GPIO_Port GPIOB
#define ADT_DIN_Pin GPIO_PIN_5
#define ADT_DIN_GPIO_Port GPIOB
#define PLL_IRQ_B_Pin GPIO_PIN_6
#define PLL_IRQ_B_GPIO_Port GPIOB
#define PLL_IRQ_B_EXTI_IRQn EXTI9_5_IRQn
#define PLL_RESET_Pin GPIO_PIN_7
#define PLL_RESET_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
