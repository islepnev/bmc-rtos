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

void write_gpio_pin(GPIO_TypeDef *gpio, uint16_t pin, bool state)
{
    const GPIO_PinState write = state ? GPIO_PIN_SET : GPIO_PIN_RESET;
    HAL_GPIO_WritePin(gpio, pin, write);
}

bool read_gpio_pin(GPIO_TypeDef *gpio, uint16_t pin)
{
    return HAL_GPIO_ReadPin(gpio, pin) == GPIO_PIN_SET;
}

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
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOJ_CLK_ENABLE();
  __HAL_RCC_GPIOI_CLK_ENABLE();
  __HAL_RCC_GPIOK_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, PLL_M6_Pin|ON_5V_VXS_Pin, GPIO_PIN_RESET);


  // ON_* (open drain, default = 1)
  HAL_GPIO_WritePin(ON_1V0_CORE_GPIO_Port, ON_1V0_CORE_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(ON_1V0_MGT_GPIO_Port, ON_1V0_MGT_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(ON_1V2_MGT_GPIO_Port, ON_1V2_MGT_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(ON_2V5_GPIO_Port, ON_2V5_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(ON_3V3_GPIO_Port, ON_3V3_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(ON_FMC_5V_GPIO_Port, ON_FMC_5V_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(ON_5V_VXS_GPIO_Port, ON_5V_VXS_Pin, GPIO_PIN_SET);

  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

  GPIO_InitStruct.Pin = ON_1V0_CORE_Pin;
  HAL_GPIO_Init(ON_1V0_CORE_GPIO_Port, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = ON_1V0_MGT_Pin;
  HAL_GPIO_Init(ON_1V0_MGT_GPIO_Port, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = ON_1V2_MGT_Pin;
  HAL_GPIO_Init(ON_1V2_MGT_GPIO_Port, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = ON_2V5_Pin;
  HAL_GPIO_Init(ON_2V5_GPIO_Port, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = ON_3V3_Pin;
  HAL_GPIO_Init(ON_3V3_GPIO_Port, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = ON_FMC_5V_Pin;
  HAL_GPIO_Init(ON_FMC_5V_GPIO_Port, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = ON_5V_VXS_Pin;
  HAL_GPIO_Init(ON_5V_VXS_GPIO_Port, &GPIO_InitStruct);

  HAL_GPIO_WritePin(PLL_M4_GPIO_Port, PLL_M4_Pin, GPIO_PIN_RESET);

  // RFU*
  HAL_GPIO_WritePin(RFU0_GPIO_Port, RFU0_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(RFU1_GPIO_Port, RFU1_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(RFU2_GPIO_Port, RFU2_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(RFU3_GPIO_Port, RFU3_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(RFU4_GPIO_Port, RFU4_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(RFU5_GPIO_Port, RFU5_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(RFU6_GPIO_Port, RFU6_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(RFU7_GPIO_Port, RFU7_Pin, GPIO_PIN_RESET);
  // HAL_GPIO_WritePin(RFU8_GPIO_Port, RFU8_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(RFU9_GPIO_Port, RFU9_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(RFU10_GPIO_Port, RFU10_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(RFU11_GPIO_Port, RFU11_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(RFU12_GPIO_Port, RFU12_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(RFU13_GPIO_Port, RFU13_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(RFU14_GPIO_Port, RFU14_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(RFU15_GPIO_Port, RFU15_Pin, GPIO_PIN_RESET);

  // RFU pins
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Pin = RFU0_Pin;
  HAL_GPIO_Init(RFU0_GPIO_Port, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = RFU1_Pin;
  HAL_GPIO_Init(RFU1_GPIO_Port, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = RFU2_Pin;
  HAL_GPIO_Init(RFU2_GPIO_Port, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = RFU3_Pin;
  HAL_GPIO_Init(RFU3_GPIO_Port, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = RFU4_Pin;
  HAL_GPIO_Init(RFU4_GPIO_Port, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = RFU5_Pin;
  HAL_GPIO_Init(RFU5_GPIO_Port, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = RFU6_Pin;
  HAL_GPIO_Init(RFU6_GPIO_Port, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = RFU7_Pin;
  HAL_GPIO_Init(RFU7_GPIO_Port, &GPIO_InitStruct);
  // GPIO_InitStruct.Pin = RFU8_Pin;
  // HAL_GPIO_Init(RFU8_GPIO_Port, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = RFU9_Pin;
  HAL_GPIO_Init(RFU9_GPIO_Port, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = RFU10_Pin;
  HAL_GPIO_Init(RFU10_GPIO_Port, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = RFU11_Pin;
  HAL_GPIO_Init(RFU11_GPIO_Port, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = RFU12_Pin;
  HAL_GPIO_Init(RFU12_GPIO_Port, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = RFU13_Pin;
  HAL_GPIO_Init(RFU13_GPIO_Port, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = RFU14_Pin;
  HAL_GPIO_Init(RFU14_GPIO_Port, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = RFU15_Pin;
  HAL_GPIO_Init(RFU15_GPIO_Port, &GPIO_InitStruct);

  HAL_GPIO_WritePin(PGOOD_PWR_GPIO_Port, PGOOD_PWR_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOI, I2C_RESET3_B_Pin
                          |PLL_RESET_B_Pin|GPIO1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(PLL_M5_GPIO_Port, PLL_M5_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOG, I2C_RESET2_B_Pin|RJ45_LED_G_A_Pin|RJ45_LED_OR_A_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOH, LED_GREEN_B_Pin|LED_RED_B_Pin|GPIO2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOF, FPGA_NSS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(FP_LED_B0_GPIO_Port, FP_LED_B0_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, FP_LED_B2_Pin|FP_LED_B1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : PEPin PEPin PEPin */
  GPIO_InitStruct.Pin = PLL_M6_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : PBPin PBPin PBPin */
  GPIO_InitStruct.Pin = PLL_M4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = PGOOD_1V0_MGT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(PGOOD_1V0_MGT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PJPin PJPin PJPin PJPin
                           PJPin PJPin PJPin */
  GPIO_InitStruct.Pin = PGOOD_PWR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOJ, &GPIO_InitStruct);

  /*Configure GPIO pins : PIPin PIPin PIPin PIPin
                           PIPin PIPin PIPin PIPin */
  GPIO_InitStruct.Pin = I2C_RESET3_B_Pin
                          |PLL_RESET_B_Pin|GPIO1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = PGOOD_1V0_CORE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(PGOOD_1V0_CORE_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PKPin PKPin PKPin */
  GPIO_InitStruct.Pin = PLL_M5_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOK, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = PLL_M0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(PLL_M0_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PGPin PGPin PGPin PGPin */
  GPIO_InitStruct.Pin = I2C_RESET2_B_Pin|RJ45_LED_G_A_Pin|RJ45_LED_OR_A_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = uSD_Detect_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(uSD_Detect_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = PLL_M3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(PLL_M3_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PHPin PHPin PHPin PHPin */
  GPIO_InitStruct.Pin = LED_GREEN_B_Pin|LED_RED_B_Pin|GPIO2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

  /*Configure GPIO pins : PIPin PIPin PIPin */
  GPIO_InitStruct.Pin = PGOOD_2V5_Pin|PGOOD_3V3_Pin|PEN_B_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);

  /*Configure GPIO pin : PC15 */
  GPIO_InitStruct.Pin = GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PFPin PFPin */
  GPIO_InitStruct.Pin = FPGA_NSS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = FP_LED_B0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(FP_LED_B0_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = FPGA_INIT_B_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(FPGA_INIT_B_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = RMII_RXER_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(RMII_RXER_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = RESET_BUTTON_B_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(RESET_BUTTON_B_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PHPin PHPin */
  GPIO_InitStruct.Pin = FMC_PRSNT_M2C_L_Pin|SMB_Alert_B_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

  /*Configure GPIO pins : PAPin PAPin PAPin */
  GPIO_InitStruct.Pin = FP_LED_B2_Pin|FP_LED_B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = FMC_PG_C2M_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(FMC_PG_C2M_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = FPGA_DONE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(FPGA_DONE_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = PGOOD_FMC_3P3VAUX_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(PGOOD_FMC_3P3VAUX_GPIO_Port, &GPIO_InitStruct);

#ifdef TTVXS_1_0
#else
  GPIO_InitStruct.Pin = AD9516_CS_B_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL; // 4.7k pullup on PCB
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(AD9516_CS_B_GPIO_Port, &GPIO_InitStruct);
#endif
}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
