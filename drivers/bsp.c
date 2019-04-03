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

// TTVXS board specific functions

#include "bsp.h"
#include "bsp_pin_defs.h"
#include "stm32f7xx_hal_gpio.h"
#include "i2c.h"
#include "spi.h"

struct __I2C_HandleTypeDef * const hPll = &hi2c4;
const uint8_t pllDeviceAddr = 0x4A;

struct __I2C_HandleTypeDef * const hi2c_sensors = &hi2c2;

struct __SPI_HandleTypeDef * const fpga_spi = &hspi5;
struct __SPI_HandleTypeDef * const therm_spi = &hspi2;

void pllSetStaticPins(void)
{
    //    HAL_GPIO_WritePin(PLL_M0_GPIO_Port, PLL_M0_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(PLL_M3_GPIO_Port, PLL_M3_Pin, GPIO_PIN_RESET); // M3=0 - do not load eeprom
    HAL_GPIO_WritePin(PLL_M4_GPIO_Port, PLL_M4_Pin, GPIO_PIN_SET);   // M4=1 - I2C mode
    HAL_GPIO_WritePin(PLL_M5_GPIO_Port, PLL_M5_Pin, GPIO_PIN_RESET); // M5=0 - I2C address offset
    HAL_GPIO_WritePin(PLL_M6_GPIO_Port, PLL_M6_Pin, GPIO_PIN_SET);   // M6=1 - I2C address offset
}

void pllReset()
{
    // toggle reset_b pin
    HAL_GPIO_WritePin(PLL_RESET_B_GPIO_Port, PLL_RESET_B_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(PLL_RESET_B_GPIO_Port, PLL_RESET_B_Pin, GPIO_PIN_SET);
}

void pm_sensor_reset_i2c_master(void)
{
    __HAL_I2C_DISABLE(hi2c_sensors);
    __HAL_I2C_ENABLE(hi2c_sensors);
}
