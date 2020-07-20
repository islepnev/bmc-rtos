/*
**    CRU16 GPIO init
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
#include "stm32f7xx_hal_gpio.h"
#include "stm32f7xx_hal_rcc.h"
#include "bsp_pin_defs.h"

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
      {ON_1V5_GPIO_Port, ON_1V5_Pin},
      {ON_3V3_GPIO_Port, ON_3V3_Pin},
      {ON_2V5_CLK_GPIO_Port, ON_2V5_CLK_Pin},
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

  HAL_GPIO_WritePin(I2C_RESET3_B_GPIO_Port, I2C_RESET3_B_Pin, GPIO_PIN_RESET);

  // PMCU_PRSNT_PM2C_B
  HAL_GPIO_WritePin(GPIO2_GPIO_Port, GPIO2_Pin, GPIO_PIN_RESET);

  HAL_GPIO_WritePin(FPGA_NSS_GPIO_Port, FPGA_NSS_Pin, GPIO_PIN_RESET);

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
      GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
      GPIO_InitStruct.Pull = GPIO_NOPULL;
      GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
      HAL_GPIO_Init(led_pins[i].GPIOx, &GPIO_InitStruct);
  }

  // PGOOD, PEN
  static const pin_def_t pgood_pins[7] = {
      {PGOOD_1V0_MGT_GPIO_Port, PGOOD_1V0_MGT_Pin},
      {PGOOD_1V0_CORE_GPIO_Port, PGOOD_1V0_CORE_Pin},
      {PGOOD_1V2_MGT_GPIO_Port,PGOOD_1V2_MGT_Pin },
      {PGOOD_1V5_GPIO_Port, PGOOD_1V5_Pin},
      {ON_3V3_CLK_GPIO_Port, ON_3V3_CLK_Pin},
      {ON_1V8_FPGA_GPIO_Port, ON_1V8_FPGA_Pin},
      {PEN_B_GPIO_Port, PEN_B_Pin}
  };
  for (int i=0; i<7; i++) {
      GPIO_InitStruct.Pin = pgood_pins[i].pin;
      GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
      GPIO_InitStruct.Pull = GPIO_PULLUP;
      HAL_GPIO_Init(pgood_pins[i].GPIOx, &GPIO_InitStruct);
  }

  // PGOOD_PWR output
  HAL_GPIO_WritePin(PGOOD_PWR_GPIO_Port, PGOOD_PWR_Pin, GPIO_PIN_RESET);
  GPIO_InitStruct.Pin = PGOOD_PWR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(PGOOD_PWR_GPIO_Port, &GPIO_InitStruct);

  // MCU_GPIO1 (connected to FPGA PLL_IRQ)
  GPIO_InitStruct.Pin = GPIO1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIO1_GPIO_Port, &GPIO_InitStruct);

  // MCU_GPIO2 (PMCU_PRSNT_PM2C_B)
  GPIO_InitStruct.Pin = GPIO2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIO2_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = I2C_RESET2_B_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_WritePin(I2C_RESET2_B_GPIO_Port, I2C_RESET2_B_Pin, GPIO_PIN_SET);
  HAL_GPIO_Init(I2C_RESET2_B_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = I2C_RESET3_B_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_WritePin(I2C_RESET3_B_GPIO_Port, I2C_RESET3_B_Pin, GPIO_PIN_SET);
  HAL_GPIO_Init(I2C_RESET3_B_GPIO_Port, &GPIO_InitStruct);

  HAL_GPIO_WritePin(GPIOG, RJ45_LED_G_A_Pin|RJ45_LED_OR_A_Pin, GPIO_PIN_RESET);
  GPIO_InitStruct.Pin = RJ45_LED_G_A_Pin|RJ45_LED_OR_A_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = uSD_Detect_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(uSD_Detect_GPIO_Port, &GPIO_InitStruct);

  // PC15-OSC32_OUT
//  GPIO_InitStruct.Pin = GPIO_PIN_15;
//  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
//  GPIO_InitStruct.Pull = GPIO_NOPULL;
//  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = FPGA_NSS_Pin;
  // GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
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

  GPIO_InitStruct.Pin = PGOOD_VTT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(PGOOD_VTT_GPIO_Port, &GPIO_InitStruct);

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

  GPIO_InitStruct.Pin = SPI2_NSS_Pin;
  // GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD; // OD;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; // OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP; // 4.7k pullup on PCB
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_WritePin(SPI2_GPIO_Port, SPI2_NSS_Pin, GPIO_PIN_SET);
  HAL_GPIO_Init(SPI2_GPIO_Port, &GPIO_InitStruct);
}
