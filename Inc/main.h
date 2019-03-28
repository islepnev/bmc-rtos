/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2019 STMicroelectronics International N.V.
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice,
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other
  *    contributors to this software may be used to endorse or promote products
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under
  *    this license is void and will automatically terminate your rights under
  *    this license.
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H__
#define __MAIN_H__

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

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/

#define ADT_CS_B_GPIO_Port GPIOI
#define ADT_CS_B_Pin GPIO_PIN_0
#define ADT_DIN_GPIO_Port GPIOI
#define ADT_DIN_Pin GPIO_PIN_3
#define ADT_DOUT_GPIO_Port GPIOI
#define ADT_DOUT_Pin GPIO_PIN_2
#define ADT_SCLK_GPIO_Port GPIOI
#define ADT_SCLK_Pin GPIO_PIN_1
#define CAN_RXD_GPIO_Port GPIOB
#define CAN_RXD_Pin GPIO_PIN_12
#define CAN_TXD_GPIO_Port GPIOB
#define CAN_TXD_Pin GPIO_PIN_13
#define CONFIG_IIC_SCL_GPIO_Port GPIOH
#define CONFIG_IIC_SCL_Pin GPIO_PIN_7
#define CONFIG_IIC_SDA_GPIO_Port GPIOH
#define CONFIG_IIC_SDA_Pin GPIO_PIN_8
#define FLASH_CLK_GPIO_Port GPIOB
#define FLASH_CLK_Pin GPIO_PIN_2
#define FLASH_D0_GPIO_Port GPIOD
#define FLASH_D0_Pin GPIO_PIN_11
#define FLASH_D1_GPIO_Port GPIOD
#define FLASH_D1_Pin GPIO_PIN_12
#define FLASH_D2_GPIO_Port GPIOE
#define FLASH_D2_Pin GPIO_PIN_2
#define FLASH_D3_GPIO_Port GPIOD
#define FLASH_D3_Pin GPIO_PIN_13
#define FLASH_FCS_B_GPIO_Port GPIOB
#define FLASH_FCS_B_Pin GPIO_PIN_6
#define FMC_A0_GPIO_Port GPIOF
#define FMC_A0_Pin GPIO_PIN_0
#define FMC_A1_GPIO_Port GPIOF
#define FMC_A1_Pin GPIO_PIN_1
#define FMC_A2_GPIO_Port GPIOF
#define FMC_A2_Pin GPIO_PIN_2
#define FMC_A3_GPIO_Port GPIOF
#define FMC_A3_Pin GPIO_PIN_3
#define FMC_A4_GPIO_Port GPIOF
#define FMC_A4_Pin GPIO_PIN_4
#define FMC_A5_GPIO_Port GPIOF
#define FMC_A5_Pin GPIO_PIN_5
#define FMC_A6_GPIO_Port GPIOF
#define FMC_A6_Pin GPIO_PIN_12
#define FMC_A7_GPIO_Port GPIOF
#define FMC_A7_Pin GPIO_PIN_13
#define FMC_A8_GPIO_Port GPIOF
#define FMC_A8_Pin GPIO_PIN_14
#define FMC_A9_GPIO_Port GPIOF
#define FMC_A9_Pin GPIO_PIN_15
#define FMC_A10_GPIO_Port GPIOG
#define FMC_A10_Pin GPIO_PIN_0
#define FMC_A11_GPIO_Port GPIOG
#define FMC_A11_Pin GPIO_PIN_1
#define FMC_BA0_GPIO_Port GPIOG
#define FMC_BA0_Pin GPIO_PIN_4
#define FMC_BA1_GPIO_Port GPIOG
#define FMC_BA1_Pin GPIO_PIN_5
#define FMC_D0_GPIO_Port GPIOD
#define FMC_D0_Pin GPIO_PIN_14
#define FMC_D1_GPIO_Port GPIOD
#define FMC_D1_Pin GPIO_PIN_15
#define FMC_D2_GPIO_Port GPIOD
#define FMC_D2_Pin GPIO_PIN_0
#define FMC_D3_GPIO_Port GPIOD
#define FMC_D3_Pin GPIO_PIN_1
#define FMC_D4_GPIO_Port GPIOE
#define FMC_D4_Pin GPIO_PIN_7
#define FMC_D5_GPIO_Port GPIOE
#define FMC_D5_Pin GPIO_PIN_8
#define FMC_D6_GPIO_Port GPIOE
#define FMC_D6_Pin GPIO_PIN_9
#define FMC_D7_GPIO_Port GPIOE
#define FMC_D7_Pin GPIO_PIN_10
#define FMC_D8_GPIO_Port GPIOE
#define FMC_D8_Pin GPIO_PIN_11
#define FMC_D9_GPIO_Port GPIOE
#define FMC_D9_Pin GPIO_PIN_12
#define FMC_D10_GPIO_Port GPIOE
#define FMC_D10_Pin GPIO_PIN_13
#define FMC_D11_GPIO_Port GPIOE
#define FMC_D11_Pin GPIO_PIN_14
#define FMC_D12_GPIO_Port GPIOE
#define FMC_D12_Pin GPIO_PIN_15
#define FMC_D13_GPIO_Port GPIOD
#define FMC_D13_Pin GPIO_PIN_8
#define FMC_D14_GPIO_Port GPIOD
#define FMC_D14_Pin GPIO_PIN_9
#define FMC_D15_GPIO_Port GPIOD
#define FMC_D15_Pin GPIO_PIN_10
#define FMC_NBL0_GPIO_Port GPIOE
#define FMC_NBL0_Pin GPIO_PIN_0
#define FMC_NBL1_GPIO_Port GPIOE
#define FMC_NBL1_Pin GPIO_PIN_1
#define FMC_PG_C2M_GPIO_Port GPIOH
#define FMC_PG_C2M_Pin GPIO_PIN_10
#define FMC_PRSNT_M2C_L_GPIO_Port GPIOH
#define FMC_PRSNT_M2C_L_Pin GPIO_PIN_9
#define FMC_SDCKE0_GPIO_Port GPIOC
#define FMC_SDCKE0_Pin GPIO_PIN_3
#define FMC_SDCLK_GPIO_Port GPIOG
#define FMC_SDCLK_Pin GPIO_PIN_8
#define FMC_SDNCAS_GPIO_Port GPIOG
#define FMC_SDNCAS_Pin GPIO_PIN_15
#define FMC_SDNE0_GPIO_Port GPIOH
#define FMC_SDNE0_Pin GPIO_PIN_3
#define FMC_SDNRAS_GPIO_Port GPIOF
#define FMC_SDNRAS_Pin GPIO_PIN_11
#define FMC_SDNWE_GPIO_Port GPIOH
#define FMC_SDNWE_Pin GPIO_PIN_5
#define FPGA_DONE_GPIO_Port GPIOB
#define FPGA_DONE_Pin GPIO_PIN_1
#define FPGA_INIT_B_GPIO_Port GPIOC
#define FPGA_INIT_B_Pin GPIO_PIN_2
#define FPGA_MISO_GPIO_Port GPIOF
#define FPGA_MISO_Pin GPIO_PIN_8
#define FPGA_MOSI_GPIO_Port GPIOF
#define FPGA_MOSI_Pin GPIO_PIN_9
#define FPGA_NSS_GPIO_Port GPIOF
#define FPGA_NSS_Pin GPIO_PIN_6
#define FPGA_RX_GPIO_Port GPIOC
#define FPGA_RX_Pin GPIO_PIN_7
#define FPGA_SCLK_GPIO_Port GPIOF
#define FPGA_SCLK_Pin GPIO_PIN_7
#define FPGA_TX_GPIO_Port GPIOC
#define FPGA_TX_Pin GPIO_PIN_6
#define FP_LED_B0_GPIO_Port GPIOC
#define FP_LED_B0_Pin GPIO_PIN_0
#define FP_LED_B1_GPIO_Port GPIOA
#define FP_LED_B1_Pin GPIO_PIN_3
#define FP_LED_B2_GPIO_Port GPIOA
#define FP_LED_B2_Pin GPIO_PIN_6
#define GPIO1_GPIO_Port GPIOI
#define GPIO1_Pin GPIO_PIN_13
#define GPIO2_GPIO_Port GPIOH
#define GPIO2_Pin GPIO_PIN_4
#define I2C_RESET2_B_GPIO_Port GPIOG
#define I2C_RESET2_B_Pin GPIO_PIN_12
#define I2C_RESET3_B_GPIO_Port GPIOI
#define I2C_RESET3_B_Pin GPIO_PIN_4
#define LED_GREEN_B_GPIO_Port GPIOH
#define LED_GREEN_B_Pin GPIO_PIN_15
#define LED_RED_B_GPIO_Port GPIOH
#define LED_RED_B_Pin GPIO_PIN_14
#define MCU_RS232_CTS_GPIO_Port GPIOA
#define MCU_RS232_CTS_Pin GPIO_PIN_11
#define MCU_RS232_RTS_GPIO_Port GPIOA
#define MCU_RS232_RTS_Pin GPIO_PIN_12
#define MCU_RS232_TXA9_GPIO_Port GPIOA
#define MCU_RS232_TXA9_Pin GPIO_PIN_9
#define MCU_RS232_TX_GPIO_Port GPIOA
#define MCU_RS232_TX_Pin GPIO_PIN_10
#define ON_1V0_CORE_GPIO_Port GPIOI
#define ON_1V0_CORE_Pin GPIO_PIN_8
#define ON_1V0_MGT_GPIO_Port GPIOE
#define ON_1V0_MGT_Pin GPIO_PIN_4
#define ON_2V5_GPIO_Port GPIOI
#define ON_2V5_Pin GPIO_PIN_7
#define ON_3V3_GPIO_Port GPIOI
#define ON_3V3_Pin GPIO_PIN_12
#define ON_5V_VXS_GPIO_Port GPIOE
#define ON_5V_VXS_Pin GPIO_PIN_6
#define ON_FMC_5V_GPIO_Port GPIOI
#define ON_FMC_5V_Pin GPIO_PIN_5
#define OSC_25M_GPIO_Port GPIOH
#define OSC_25M_Pin GPIO_PIN_0
#define PEN_B_GPIO_Port GPIOI
#define PEN_B_Pin GPIO_PIN_15
#define PGOOD_1V0_CORE_GPIO_Port GPIOK
#define PGOOD_1V0_CORE_Pin GPIO_PIN_7
#define PGOOD_1V0_MGT_GPIO_Port GPIOE
#define PGOOD_1V0_MGT_Pin GPIO_PIN_5
#define PGOOD_2V5_GPIO_Port GPIOI
#define PGOOD_2V5_Pin GPIO_PIN_9
#define PGOOD_3V3_GPIO_Port GPIOI
#define PGOOD_3V3_Pin GPIO_PIN_11
#define PGOOD_FMC_3P3VAUX_GPIO_Port GPIOB
#define PGOOD_FMC_3P3VAUX_Pin GPIO_PIN_15
#define PGOOD_PWR_GPIO_Port GPIOJ
#define PGOOD_PWR_Pin GPIO_PIN_5
#define PLL_M0_GPIO_Port GPIOK
#define PLL_M0_Pin GPIO_PIN_5
#define PLL_M3_GPIO_Port GPIOI
#define PLL_M3_Pin GPIO_PIN_6
#define PLL_M4_GPIO_Port GPIOB
#define PLL_M4_Pin GPIO_PIN_7
#define PLL_M5_GPIO_Port GPIOK
#define PLL_M5_Pin GPIO_PIN_6
#define PLL_M6_GPIO_Port GPIOE
#define PLL_M6_Pin GPIO_PIN_3
#define PLL_RESET_B_GPIO_Port GPIOI
#define PLL_RESET_B_Pin GPIO_PIN_10
#define PLL_SCL_GPIO_Port GPIOH
#define PLL_SCL_Pin GPIO_PIN_11
#define PLL_SDA_GPIO_Port GPIOH
#define PLL_SDA_Pin GPIO_PIN_12
#define RESET_BUTTON_B_GPIO_Port GPIOA
#define RESET_BUTTON_B_Pin GPIO_PIN_4
#define RFU0_GPIO_Port GPIOK
#define RFU0_Pin GPIO_PIN_4
#define RFU1_GPIO_Port GPIOK
#define RFU1_Pin GPIO_PIN_3
#define RFU2_GPIO_Port GPIOG
#define RFU2_Pin GPIO_PIN_10
#define RFU3_GPIO_Port GPIOB
#define RFU3_Pin GPIO_PIN_5
#define RFU4_GPIO_Port GPIOJ
#define RFU4_Pin GPIO_PIN_13
#define RFU5_GPIO_Port GPIOJ
#define RFU5_Pin GPIO_PIN_14
#define RFU6_GPIO_Port GPIOJ
#define RFU6_Pin GPIO_PIN_12
#define RFU7_GPIO_Port GPIOD
#define RFU7_Pin GPIO_PIN_7
#define RFU8_GPIO_Port GPIOI
#define RFU8_Pin GPIO_PIN_14
#define RFU9_GPIO_Port GPIOH
#define RFU9_Pin GPIO_PIN_2
#define RFU10_GPIO_Port GPIOF
#define RFU10_Pin GPIO_PIN_10
#define RFU11_GPIO_Port GPIOA
#define RFU11_Pin GPIO_PIN_5
#define RFU12_GPIO_Port GPIOB
#define RFU12_Pin GPIO_PIN_0
#define RFU13_GPIO_Port GPIOJ
#define RFU13_Pin GPIO_PIN_0
#define RFU14_GPIO_Port GPIOJ
#define RFU14_Pin GPIO_PIN_2
#define RFU15_GPIO_Port GPIOJ
#define RFU15_Pin GPIO_PIN_1
#define RJ45_LED_G_A_GPIO_Port GPIOG
#define RJ45_LED_G_A_Pin GPIO_PIN_7
#define RJ45_LED_OR_A_GPIO_Port GPIOG
#define RJ45_LED_OR_A_Pin GPIO_PIN_6
#define RMII_CRS_DV_GPIO_Port GPIOA
#define RMII_CRS_DV_Pin GPIO_PIN_7
#define RMII_MDC_GPIO_Port GPIOC
#define RMII_MDC_Pin GPIO_PIN_1
#define RMII_MDIO_GPIO_Port GPIOA
#define RMII_MDIO_Pin GPIO_PIN_2
#define RMII_REF_CLK_GPIO_Port GPIOA
#define RMII_REF_CLK_Pin GPIO_PIN_1
#define RMII_RXD0_GPIO_Port GPIOC
#define RMII_RXD0_Pin GPIO_PIN_4
#define RMII_RXD1_GPIO_Port GPIOC
#define RMII_RXD1_Pin GPIO_PIN_5
#define RMII_RXER_GPIO_Port GPIOG
#define RMII_RXER_Pin GPIO_PIN_2
#define RMII_TXD0_GPIO_Port GPIOG
#define RMII_TXD0_Pin GPIO_PIN_13
#define RMII_TXD1_GPIO_Port GPIOG
#define RMII_TXD1_Pin GPIO_PIN_14
#define RMII_TX_EN_GPIO_Port GPIOG
#define RMII_TX_EN_Pin GPIO_PIN_11
#define SMB_Alert_B_GPIO_Port GPIOH
#define SMB_Alert_B_Pin GPIO_PIN_6
#define SMB_SCL_GPIO_Port GPIOB
#define SMB_SCL_Pin GPIO_PIN_10
#define SMB_SDA_GPIO_Port GPIOB
#define SMB_SDA_Pin GPIO_PIN_11
#define TTY_CTS_GPIO_Port GPIOD
#define TTY_CTS_Pin GPIO_PIN_3
#define TTY_RTS_GPIO_Port GPIOD
#define TTY_RTS_Pin GPIO_PIN_4
#define TTY_RX_GPIO_Port GPIOD
#define TTY_RX_Pin GPIO_PIN_6
#define TTY_TX_GPIO_Port GPIOD
#define TTY_TX_Pin GPIO_PIN_5
#define VXS_SCL_GPIO_Port GPIOB
#define VXS_SCL_Pin GPIO_PIN_8
#define VXS_SDA_GPIO_Port GPIOB
#define VXS_SDA_Pin GPIO_PIN_9
#define uSD_CLK_GPIO_Port GPIOC
#define uSD_CLK_Pin GPIO_PIN_12
#define uSD_CMD_GPIO_Port GPIOD
#define uSD_CMD_Pin GPIO_PIN_2
#define uSD_D0_GPIO_Port GPIOC
#define uSD_D0_Pin GPIO_PIN_8
#define uSD_D1_GPIO_Port GPIOC
#define uSD_D1_Pin GPIO_PIN_9
#define uSD_D2_GPIO_Port GPIOC
#define uSD_D2_Pin GPIO_PIN_10
#define uSD_D3_GPIO_Port GPIOC
#define uSD_D3_Pin GPIO_PIN_11
#define uSD_Detect_GPIO_Port GPIOC
#define uSD_Detect_Pin GPIO_PIN_13

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
