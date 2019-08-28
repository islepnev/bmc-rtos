/**
  ******************************************************************************
  * File Name          : gpio.c
  * Description        : This file provides code for the configuration
  *                      of all used GPIO pins.
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

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"
/* USER CODE BEGIN 0 */
#include "stm32f7xx_hal_gpio.h"
#include "stm32f7xx_hal_rcc.h"
#include "bsp_pin_defs.h"
/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOJ_CLK_ENABLE();

  HAL_GPIO_WritePin(ON_TDC_A_GPIO_Port, ON_TDC_A_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(ON_TDC_B_GPIO_Port, ON_TDC_B_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(ON_TDC_C_GPIO_Port, ON_TDC_C_Pin, GPIO_PIN_SET);

  HAL_GPIO_WritePin(ADT_CS_B0_GPIO_Port, ADT_CS_B0_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(ADT_CS_B1_GPIO_Port, ADT_CS_B1_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(ADT_CS_B2_GPIO_Port, ADT_CS_B2_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(ADT_CS_B3_GPIO_Port, ADT_CS_B3_Pin, GPIO_PIN_SET);

  GPIO_InitStruct.Pin = ON_TDC_C_Pin|ON_TDC_B_Pin|ON_TDC_A_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  // FPGA_NSS
  GPIO_InitStruct.Pin = FPGA_NSS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_WritePin(FPGA_NSS_GPIO_Port, FPGA_NSS_Pin, GPIO_PIN_RESET);
  HAL_GPIO_Init(FPGA_NSS_GPIO_Port, &GPIO_InitStruct);

  // MON_SMB_SW_RST_B
  GPIO_InitStruct.Pin = MON_SMB_SW_RST_B_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_WritePin(MON_SMB_SW_RST_B_GPIO_Port, MON_SMB_SW_RST_B_Pin, GPIO_PIN_RESET);
  HAL_GPIO_Init(MON_SMB_SW_RST_B_GPIO_Port, &GPIO_InitStruct);

  // LEDs, turn on by default
  HAL_GPIO_WritePin(LED_GREEN_B_GPIO_Port,     LED_GREEN_B_Pin,     GPIO_PIN_RESET);
  HAL_GPIO_WritePin(LED_YELLOW_B_GPIO_Port,    LED_YELLOW_B_Pin,    GPIO_PIN_RESET);
  HAL_GPIO_WritePin(LED_RED_B_GPIO_Port,       LED_RED_B_Pin,       GPIO_PIN_RESET);
  HAL_GPIO_WritePin(LED_ERROR_B_GPIO_Port,     LED_ERROR_B_Pin,     GPIO_PIN_RESET);
  HAL_GPIO_WritePin(LED_HEARTBEAT_B_GPIO_Port, LED_HEARTBEAT_B_Pin, GPIO_PIN_RESET);

  GPIO_InitStruct.Pin = LED_GREEN_B_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_GREEN_B_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LED_YELLOW_B_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_YELLOW_B_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LED_RED_B_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_RED_B_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LED_ERROR_B_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_ERROR_B_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LED_HEARTBEAT_B_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_HEARTBEAT_B_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PFPin PFPin */
  GPIO_InitStruct.Pin = VME_DET_B_Pin|LTM_PGOOD_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  // SYSTEM_RDY (to FPGA)
  GPIO_InitStruct.Pin = SYSTEM_RDY_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_WritePin(SYSTEM_RDY_GPIO_Port, SYSTEM_RDY_Pin, GPIO_PIN_SET);
  HAL_GPIO_Init(SYSTEM_RDY_GPIO_Port, &GPIO_InitStruct);

  // ON_5V (always on)
  // PCB R35.5 should be mounted 10kΩ
  // PCB R37.5 should NOT be mounted
  GPIO_InitStruct.Pin = ON_5V_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_WritePin(ON_5V_GPIO_Port, ON_5V_Pin, GPIO_PIN_SET);
  HAL_GPIO_Init(ON_5V_GPIO_Port, &GPIO_InitStruct);

  // ON_1_5V
  GPIO_InitStruct.Pin = ON_1_5V_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_WritePin(ON_1_5V_GPIO_Port, ON_1_5V_Pin, GPIO_PIN_SET);
  HAL_GPIO_Init(ON_1_5V_GPIO_Port, &GPIO_InitStruct);

  // ON_3_3V
  GPIO_InitStruct.Pin = ON_3_3V_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_WritePin(ON_3_3V_GPIO_Port, ON_3_3V_Pin, GPIO_PIN_SET);
  HAL_GPIO_Init(ON_3_3V_GPIO_Port, &GPIO_InitStruct);

  // ON_1_0V_1_2V
  GPIO_InitStruct.Pin = ON_1_0V_1_2V_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_WritePin(ON_1_0V_1_2V_GPIO_Port, ON_1_0V_1_2V_Pin, GPIO_PIN_SET);
  HAL_GPIO_Init(ON_1_0V_1_2V_GPIO_Port, &GPIO_InitStruct);

  // ADT_CS_B*
  // 4.7 kΩ pull-up on PCB
  GPIO_InitStruct.Pin = ADT_CS_B0_Pin|ADT_CS_B1_Pin|ADT_CS_B2_Pin|ADT_CS_B3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = MON_SMB_ALERT_B_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(MON_SMB_ALERT_B_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = FPGA_CORE_PGOOD_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(FPGA_CORE_PGOOD_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = FPGA_DONE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(FPGA_DONE_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = FPGA_INIT_B_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_WritePin(FPGA_INIT_B_GPIO_Port, FPGA_INIT_B_Pin, GPIO_PIN_SET);
  HAL_GPIO_Init(FPGA_INIT_B_GPIO_Port, &GPIO_InitStruct);
}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
