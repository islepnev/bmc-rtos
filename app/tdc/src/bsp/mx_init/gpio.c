/*
**    TDC72 GPIO init
**
**    Copyright 2019 Ilja Slepnev
**
**    This program is free software: you can redistribute it and/or modify
**    it under the terms of the GNU General Public License as published by
**    the Free Software Foundation, either version 3 of the License, or
**    (at your option) any later version.
**
**    This program is distributed in the hope that it will be useful,
**    but WITHOUT ANY WARRANTY; without even the implied warranty of
**    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**    GNU General Public License for more details.
**
**    You should have received a copy of the GNU General Public License
**    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "gpio.h"

#include "bsp_pin_defs.h"
#include "stm32f7xx_hal_gpio.h"
#include "stm32f7xx_hal_rcc.h"

typedef struct {
    GPIO_TypeDef* GPIOx;
    uint16_t pin;
} pin_def_t;

void write_gpio_pin(GPIO_TypeDef *gpio, uint16_t pin, bool state)
{
    const GPIO_PinState write = state ? GPIO_PIN_SET : GPIO_PIN_RESET;
    HAL_GPIO_WritePin(gpio, pin, write);
}

bool read_gpio_pin(GPIO_TypeDef *gpio, uint16_t pin)
{
    return HAL_GPIO_ReadPin(gpio, pin) == GPIO_PIN_SET;
}

void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

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

  // ON_5V (always on)
  // PCB R35.5 should be mounted 10kΩ
  // PCB R37.5 should NOT be mounted

  // ON_* (open drain, default = 1)
  static const pin_def_t on_pins[8] = {
      {ON_5V_GPIO_Port, ON_5V_Pin},
      {ON_1_5V_GPIO_Port, ON_1_5V_Pin},
      {ON_3_3V_GPIO_Port, ON_3_3V_Pin},
      {ON_1_0V_1_2V_GPIO_Port, ON_1_0V_1_2V_Pin},
      {ON_TDC_A_GPIO_Port, ON_TDC_A_Pin},
      {ON_TDC_B_GPIO_Port, ON_TDC_B_Pin},
      {ON_TDC_C_GPIO_Port, ON_TDC_C_Pin},
      {ON_TDC_D_GPIO_Port, ON_TDC_D_Pin} // not connected on TDC72
  };

  for (int i=0; i<8; i++) {
      HAL_GPIO_WritePin(on_pins[i].GPIOx, on_pins[i].pin, GPIO_PIN_SET);
      GPIO_InitStruct.Pin = on_pins[i].pin;
      GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; // PCB 4.1 has no pullups?
      GPIO_InitStruct.Pull = GPIO_NOPULL;
      GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
      HAL_GPIO_Init(on_pins[i].GPIOx, &GPIO_InitStruct);
  }

  // MON_SMB_SW_RST_B
  GPIO_InitStruct.Pin = MON_SMB_SW_RST_B_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_WritePin(MON_SMB_SW_RST_B_GPIO_Port, MON_SMB_SW_RST_B_Pin, GPIO_PIN_SET);
  HAL_GPIO_Init(MON_SMB_SW_RST_B_GPIO_Port, &GPIO_InitStruct);

  // LEDs
  static const pin_def_t led_pins[5] = {
      {LED_GREEN_B_GPIO_Port,     LED_GREEN_B_Pin},
      {LED_YELLOW_B_GPIO_Port,    LED_YELLOW_B_Pin},
      {LED_RED_B_GPIO_Port,       LED_RED_B_Pin},
      {LED_ERROR_B_GPIO_Port,     LED_ERROR_B_Pin},
      {LED_HEARTBEAT_B_GPIO_Port, LED_HEARTBEAT_B_Pin}
  };
  for (int i=0; i<5; i++) {
      // LEDs, turn on by default
      HAL_GPIO_WritePin(led_pins[i].GPIOx, led_pins[i].pin, GPIO_PIN_RESET);
      GPIO_InitStruct.Pin = led_pins[i].pin;
      GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
      GPIO_InitStruct.Pull = GPIO_NOPULL;
      GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
      HAL_GPIO_Init(led_pins[i].GPIOx, &GPIO_InitStruct);
  }

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

#ifdef BOARD_TDC72
  // ADT_CS_B*
  HAL_GPIO_WritePin(ADT_CS_B0_GPIO_Port, ADT_CS_B0_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(ADT_CS_B1_GPIO_Port, ADT_CS_B1_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(ADT_CS_B2_GPIO_Port, ADT_CS_B2_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(ADT_CS_B3_GPIO_Port, ADT_CS_B3_Pin, GPIO_PIN_SET);
  // 4.7 kΩ pull-up on PCB
  GPIO_InitStruct.Pin = ADT_CS_B0_Pin|ADT_CS_B1_Pin|ADT_CS_B2_Pin|ADT_CS_B3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
  #endif

  // PCB_VER_A* (not implemented in TDC72 <= 4.2)
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Pin = PCB_VER_A0_Pin;
  HAL_GPIO_Init(PCB_VER_A0_GPIO_Port, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = PCB_VER_A1_Pin;
  HAL_GPIO_Init(PCB_VER_A1_GPIO_Port, &GPIO_InitStruct);

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

#ifdef BOARD_TDC64
  HAL_GPIO_WritePin(AD9516_CS_GPIO_Port, AD9516_CS_Pin, GPIO_PIN_SET);
  GPIO_InitStruct.Pin = AD9516_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP; // 4.7k pullup on PCB
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(AD9516_CS_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Pin = AD9516_LD_Pin;
  HAL_GPIO_Init(AD9516_LD_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = AD9516_ST_Pin;
  HAL_GPIO_Init(AD9516_ST_GPIO_Port, &GPIO_InitStruct);
#endif
}
