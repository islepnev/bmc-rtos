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

typedef struct {
    GPIO_TypeDef* GPIOx;
    uint16_t pin;
} pin_def_t;

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
  __HAL_RCC_GPIOI_CLK_ENABLE();
  __HAL_RCC_GPIOJ_CLK_ENABLE();
  __HAL_RCC_GPIOK_CLK_ENABLE();

  // PCB VER
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

  GPIO_InitStruct.Pin = PCB_VER_A0_Pin;
  HAL_GPIO_Init(PCB_VER_A0_GPIO_Port, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = PCB_VER_A1_Pin;
  HAL_GPIO_Init(PCB_VER_A1_GPIO_Port, &GPIO_InitStruct);

  // ON_* (open drain, default = 1)
  static const pin_def_t on_pins[7] = {
      {ON_1V0_CORE_GPIO_Port, ON_1V0_CORE_Pin},
      {ON_1V0_MGT_GPIO_Port, ON_1V0_MGT_Pin},
      {ON_1V2_MGT_GPIO_Port, ON_1V2_MGT_Pin},
      {ON_2V5_GPIO_Port, ON_2V5_Pin},
      {ON_3V3_GPIO_Port, ON_3V3_Pin},
      {ON_FMC_5V_GPIO_Port, ON_FMC_5V_Pin},
      {ON_5V_VXS_GPIO_Port, ON_5V_VXS_Pin}
  };
  for (int i=0; i<7; i++) {
      HAL_GPIO_WritePin(on_pins[i].GPIOx, on_pins[i].pin, GPIO_PIN_SET);
      GPIO_InitStruct.Pin = on_pins[i].pin;
      GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
      GPIO_InitStruct.Pull = GPIO_NOPULL;
      GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
      HAL_GPIO_Init(on_pins[i].GPIOx, &GPIO_InitStruct);
  }

  HAL_GPIO_WritePin(PLL_M4_GPIO_Port, PLL_M4_Pin, GPIO_PIN_RESET);

  // RFU*
  static const pin_def_t rfu_pins[15] = {
      {RFU0_GPIO_Port, RFU0_Pin},
      {RFU1_GPIO_Port, RFU1_Pin},
      {RFU2_GPIO_Port, RFU2_Pin},
      {RFU3_GPIO_Port, RFU3_Pin},
      {RFU4_GPIO_Port, RFU4_Pin},
      {RFU5_GPIO_Port, RFU5_Pin},
      {RFU6_GPIO_Port, RFU6_Pin},
      {RFU7_GPIO_Port, RFU7_Pin},
      // {RFU8_GPIO_Port, RFU8_Pin},
      {RFU9_GPIO_Port, RFU9_Pin},
      {RFU10_GPIO_Port, RFU10_Pin},
      {RFU11_GPIO_Port, RFU11_Pin},
      {RFU12_GPIO_Port, RFU12_Pin},
      {RFU13_GPIO_Port, RFU13_Pin},
      {RFU14_GPIO_Port, RFU14_Pin},
      {RFU15_GPIO_Port, RFU15_Pin}
  };
  for (int i=0; i<15; i++) {
      HAL_GPIO_WritePin(rfu_pins[i].GPIOx, rfu_pins[i].pin, GPIO_PIN_RESET);
      GPIO_InitStruct.Pin = rfu_pins[i].pin;
      GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
      GPIO_InitStruct.Pull = GPIO_NOPULL;
      GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
      HAL_GPIO_Init(rfu_pins[i].GPIOx, &GPIO_InitStruct);
  }

  HAL_GPIO_WritePin(GPIOI, I2C_RESET3_B_Pin
                          |PLL_RESET_B_Pin|GPIO1_Pin, GPIO_PIN_RESET);

  HAL_GPIO_WritePin(GPIOG, I2C_RESET2_B_Pin|RJ45_LED_G_A_Pin|RJ45_LED_OR_A_Pin, GPIO_PIN_RESET);

  HAL_GPIO_WritePin(GPIOF, FPGA_NSS_Pin, GPIO_PIN_RESET);

  // LEDs
  static const pin_def_t led_pins[5] = {
      {LED_RED_B_GPIO_Port, LED_RED_B_Pin},
      {LED_YELLOW_B_GPIO_Port, LED_YELLOW_B_Pin},
      {LED_GREEN_B_GPIO_Port, LED_GREEN_B_Pin},
      {LED_ERROR_B_GPIO_Port, LED_ERROR_B_Pin},
      {LED_HEARTBEAT_B_GPIO_Port, LED_HEARTBEAT_B_Pin}
  };
  for (int i=0; i<5; i++) {
      HAL_GPIO_WritePin(led_pins[i].GPIOx, led_pins[i].pin, GPIO_PIN_RESET);
      GPIO_InitStruct.Pin = led_pins[i].pin;
      GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
      GPIO_InitStruct.Pull = GPIO_NOPULL;
      GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
      HAL_GPIO_Init(led_pins[i].GPIOx, &GPIO_InitStruct);
  }

  // PLL AD9545
  GPIO_InitStruct.Pin = PLL_M0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(PLL_M0_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = PLL_M3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(PLL_M3_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = PLL_M4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(PLL_M4_GPIO_Port, &GPIO_InitStruct);

  HAL_GPIO_WritePin(PLL_M5_GPIO_Port, PLL_M5_Pin, GPIO_PIN_RESET);
  GPIO_InitStruct.Pin = PLL_M5_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(PLL_M5_GPIO_Port, &GPIO_InitStruct);

  HAL_GPIO_WritePin(PLL_M6_GPIO_Port, PLL_M6_Pin, GPIO_PIN_RESET);
  GPIO_InitStruct.Pin = PLL_M6_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(PLL_M6_GPIO_Port, &GPIO_InitStruct);

  // PGOOD, PEN
  static const pin_def_t pgood_pins[6] = {
      {PGOOD_1V0_MGT_GPIO_Port, PGOOD_1V0_MGT_Pin},
      {PGOOD_1V0_CORE_GPIO_Port, PGOOD_1V0_CORE_Pin},
      {PGOOD_2V5_GPIO_Port, PGOOD_2V5_Pin},
      {PGOOD_3V3_GPIO_Port, PGOOD_3V3_Pin},
      {PGOOD_FMC_3P3VAUX_GPIO_Port, PGOOD_FMC_3P3VAUX_Pin},
      {PEN_B_GPIO_Port, PEN_B_Pin}
  };
  for (int i=0; i<6; i++) {
      GPIO_InitStruct.Pin = pgood_pins[i].pin;
      GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
      GPIO_InitStruct.Pull = GPIO_PULLUP;
      HAL_GPIO_Init(pgood_pins[i].GPIOx, &GPIO_InitStruct);
  }

  // PGOOD_PWR output
  HAL_GPIO_WritePin(PGOOD_PWR_GPIO_Port, PGOOD_PWR_Pin, GPIO_PIN_RESET);
  GPIO_InitStruct.Pin = PGOOD_PWR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(PGOOD_PWR_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = I2C_RESET3_B_Pin
                          |PLL_RESET_B_Pin|GPIO1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = I2C_RESET2_B_Pin|RJ45_LED_G_A_Pin|RJ45_LED_OR_A_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = uSD_Detect_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(uSD_Detect_GPIO_Port, &GPIO_InitStruct);

  // ???
  HAL_GPIO_WritePin(GPIOH, GPIO2_Pin, GPIO_PIN_RESET);
  GPIO_InitStruct.Pin = GPIO2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

  // ???
  GPIO_InitStruct.Pin = GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = FPGA_NSS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(FPGA_NSS_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = FPGA_INIT_B_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(FPGA_INIT_B_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = RMII_RXER_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(RMII_RXER_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = RESET_BUTTON_B_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(RESET_BUTTON_B_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = FMC_PRSNT_M2C_L_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(FMC_PRSNT_M2C_L_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = SMB_Alert_B_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(SMB_Alert_B_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = FMC_PG_C2M_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(FMC_PG_C2M_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = FPGA_DONE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(FPGA_DONE_GPIO_Port, &GPIO_InitStruct);

#ifdef TTVXS_1_0
#else
  GPIO_InitStruct.Pin = AD9516_CS_B_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; // OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL; // 4.7k pullup on PCB
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(AD9516_CS_B_GPIO_Port, &GPIO_InitStruct);
#endif
}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
