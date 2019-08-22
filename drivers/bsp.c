/*
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

// TDC72VHL v4 board specific functions

#include "bsp.h"
#include "bsp_pin_defs.h"
#include "stm32f7xx_hal_gpio.h"
#include "i2c.h"
#include "spi.h"
#include "dev_pm_sensors_config.h"
#include "dev_pot.h"
#include "logbuffer.h"

board_version_t board_version = PCB_4_1;
void update_board_version(int powermon_count, int pots_count)
{
    if ((pots_count == 0) && (powermon_count <= POWERMON_SENSORS_PCB_4_1))
        board_version = PCB_4_1;
    if ((pots_count > 0) && (powermon_count > POWERMON_SENSORS_PCB_4_1))
        board_version = PCB_4_2;
    if (pots_count == DEV_POT_COUNT)
        board_version = PCB_4_2;
    if (powermon_count == POWERMON_SENSORS_PCB_4_2)
        board_version = PCB_4_2;
}

struct __I2C_HandleTypeDef * const hPll = &hi2c2;
const uint8_t pllDeviceAddr = 0x4A;

struct __I2C_HandleTypeDef * const hi2c_sensors = &hi2c4;

struct __SPI_HandleTypeDef * const fpga_spi = &hspi1;
struct __SPI_HandleTypeDef * const therm_spi = &hspi4;

// AD9545
// M0, M1, M2 do not have internal resistors
static void pll_gpio_init(pll_enable_t enable)
{
    uint32_t tristate = (PLL_ENABLE == enable)? GPIO_MODE_OUTPUT_PP : GPIO_MODE_INPUT;
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    // 4.7 kΩ pull-up on PCB, internal 100 kΩ pull-up resistor
    GPIO_InitStruct.Pin = PLL_RESET_B_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_WritePin(PLL_RESET_B_GPIO_Port, PLL_RESET_B_Pin, GPIO_PIN_SET);
    HAL_GPIO_Init(PLL_RESET_B_GPIO_Port, &GPIO_InitStruct);

    // input
    GPIO_InitStruct.Pin = PLL_M0_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(PLL_M0_GPIO_Port, &GPIO_InitStruct);

    // M3=0 - do not load eeprom.
    // internal 100 kΩ pull-down, disable pin
    GPIO_InitStruct.Pin = PLL_M3_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    HAL_GPIO_WritePin(PLL_M3_GPIO_Port, PLL_M3_Pin, GPIO_PIN_RESET);
    HAL_GPIO_Init(PLL_M3_GPIO_Port, &GPIO_InitStruct);

    // M4=1 - I2C mode
    // 4.7 kΩ pull-up on PCB, internal 100 kΩ pull-down
    GPIO_InitStruct.Pin = PLL_M4_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT; // tristate;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_WritePin(PLL_M4_GPIO_Port, PLL_M4_Pin, GPIO_PIN_SET);
    HAL_GPIO_Init(PLL_M4_GPIO_Port, &GPIO_InitStruct);

    // M5=0 - I2C address offset
    // 4.7 kΩ pull-down on PCB
    GPIO_InitStruct.Pin = PLL_M5_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_WritePin(PLL_M5_GPIO_Port, PLL_M5_Pin, GPIO_PIN_RESET);
    HAL_GPIO_Init(PLL_M5_GPIO_Port, &GPIO_InitStruct);

    // M6=1 - I2C address offset, internal 10 kΩ pull-up resistor
    // 4.7 kΩ pull-up on PCB
    GPIO_InitStruct.Pin = PLL_M6_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_WritePin(PLL_M6_GPIO_Port, PLL_M6_Pin, GPIO_PIN_SET);
    HAL_GPIO_Init(PLL_M6_GPIO_Port, &GPIO_InitStruct);

}

void pllSetStaticPins(pll_enable_t enable)
{
    pll_gpio_init(enable);
}

void pllReset()
{
    // toggle reset_b pin
    HAL_GPIO_WritePin(PLL_RESET_B_GPIO_Port, PLL_RESET_B_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(PLL_RESET_B_GPIO_Port, PLL_RESET_B_Pin, GPIO_PIN_SET);
}

pll_gpio_test_t pll_gpio_test(void)
{
    GPIO_PinState pin_resetb = HAL_GPIO_ReadPin(PLL_RESET_B_GPIO_Port, PLL_RESET_B_Pin);
    GPIO_PinState pin_m3 = HAL_GPIO_ReadPin(PLL_M3_GPIO_Port, PLL_M3_Pin);
    GPIO_PinState pin_m4 = HAL_GPIO_ReadPin(PLL_M4_GPIO_Port, PLL_M4_Pin);
    GPIO_PinState pin_m5 = HAL_GPIO_ReadPin(PLL_M5_GPIO_Port, PLL_M5_Pin);
    GPIO_PinState pin_m6 = HAL_GPIO_ReadPin(PLL_M6_GPIO_Port, PLL_M6_Pin);
    if ( 1
         && (GPIO_PIN_SET   == pin_resetb)
         && (GPIO_PIN_RESET == pin_m3)
         && (GPIO_PIN_SET   == pin_m4)
         && (GPIO_PIN_RESET == pin_m5)
         && (GPIO_PIN_SET   == pin_m6)
         )
        return PLL_GPIO_TEST_OK;
    else {
        log_printf(LOG_NOTICE, "PLL GPIO: resetb=%u, m3=%u, m4=%u, m5=%u, m6=%u", pin_resetb, pin_m3, pin_m4, pin_m5, pin_m6);
        return PLL_GPIO_TEST_FAIL;
    }
}

void pm_sensor_reset_i2c_master(void)
{
    __HAL_I2C_DISABLE(hi2c_sensors);
    __HAL_I2C_ENABLE(hi2c_sensors);
}
