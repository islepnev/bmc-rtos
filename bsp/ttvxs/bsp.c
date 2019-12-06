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

#include <stdbool.h>

#include "bsp_pin_defs.h"
#include "stm32f7xx_hal_gpio.h"
#include "stm32f7xx_ll_gpio.h"
#include "i2c.h"
#include "bus/i2c_driver.h"
#include "spi.h"
#include "usart.h"
#include "error_handler.h"

struct __I2C_HandleTypeDef * const hi2c_ad9545 = &hi2c4;
const uint8_t ad9545_deviceAddr = 0x4A;
struct __I2C_HandleTypeDef * const vxsiic_hi2c = &hi2c1;
struct __I2C_HandleTypeDef * const hi2c_sensors = &hi2c2;
struct __I2C_HandleTypeDef * const hi2c_eeprom_cfg = &hi2c3;
const uint8_t eeprom_cfg_deviceAddr = 0x50;

struct __SPI_HandleTypeDef * const fpga_spi = &hspi5;
#ifdef TTVXS_1_0
struct __SPI_HandleTypeDef * const therm_spi = &hspi2;
#else
struct __SPI_HandleTypeDef * const ad9516_spi = &hspi2;
#endif

uint32_t detect_pcb_version(void)
{
    bool a0 = (GPIO_PIN_SET == HAL_GPIO_ReadPin(PCB_VER_A0_GPIO_Port, PCB_VER_A0_Pin));
    bool a1 = (GPIO_PIN_SET == HAL_GPIO_ReadPin(PCB_VER_A1_GPIO_Port, PCB_VER_A1_Pin));
    return a1 * 2 + a0;
}

void pm_sensor_reset_i2c_master(void)
{
    i2c_driver_reset(hi2c_sensors);
}

void fpga_enable_interface(bool enable)
{
    GPIO_PinState low = GPIO_PIN_RESET;
    GPIO_PinState high = enable ? GPIO_PIN_SET : GPIO_PIN_RESET;

    HAL_GPIO_WritePin(FPGA_RX_GPIO_Port, FPGA_RX_Pin, high);
    HAL_GPIO_WritePin(FPGA_TX_GPIO_Port, FPGA_TX_Pin, high);
    HAL_GPIO_WritePin(I2C_RESET2_B_GPIO_Port, I2C_RESET2_B_Pin, high);
    HAL_GPIO_WritePin(I2C_RESET3_B_GPIO_Port, I2C_RESET3_B_Pin, high);
    HAL_GPIO_WritePin(FPGA_INIT_B_GPIO_Port, FPGA_INIT_B_Pin, high);
    HAL_GPIO_WritePin(FPGA_DONE_GPIO_Port,   FPGA_DONE_Pin,   high);
    HAL_GPIO_WritePin(FPGA_NSS_GPIO_Port,    FPGA_NSS_Pin,    high);
//    HAL_GPIO_WritePin(FPGA_SCLK_GPIO_Port,   FPGA_NSS_Pin,    low);
//    HAL_GPIO_WritePin(FPGA_MOSI_GPIO_Port,   FPGA_MOSI_Pin,   low);
//    HAL_GPIO_WritePin(FPGA_MISO_GPIO_Port,   FPGA_MISO_Pin,   low);

    if (enable) {
//        LL_GPIO_SetPinMode(FPGA_SCLK_GPIO_Port,   FPGA_SCLK_Pin,   GPIO_MODE_AF_PP);
//        LL_GPIO_SetPinMode(FPGA_MOSI_GPIO_Port,   FPGA_MOSI_Pin,   GPIO_MODE_AF_PP);
//        LL_GPIO_SetPinMode(FPGA_MISO_GPIO_Port,   FPGA_MISO_Pin,   GPIO_MODE_AF_PP);
        __HAL_UART_ENABLE(&huart6);
//        MX_SPI5_Init();
//        __HAL_SPI_ENABLE(fpga_spi);
    } else {
//        __HAL_SPI_DISABLE(fpga_spi);
        __HAL_UART_DISABLE(&huart6);
//        HAL_SPI_MspDeInit(fpga_spi);
//        if (HAL_SPI_DeInit(fpga_spi) != HAL_OK)
//        {
//          Error_Handler();
//        }

//        LL_GPIO_SetPinMode(FPGA_SCLK_GPIO_Port,   FPGA_SCLK_Pin,   GPIO_MODE_ANALOG);
//        LL_GPIO_SetPinMode(FPGA_MOSI_GPIO_Port,   FPGA_MOSI_Pin,   GPIO_MODE_ANALOG);
//        LL_GPIO_SetPinMode(FPGA_MISO_GPIO_Port,   FPGA_MISO_Pin,   GPIO_MODE_ANALOG);

//        FPGA_RX_GPIO_Port->ODR &= ~FPGA_RX_Pin;
//        FPGA_TX_GPIO_Port->ODR &= ~FPGA_TX_Pin;
//        FPGA_NSS_GPIO_Port->ODR &= ~FPGA_NSS_Pin;
//        FPGA_MOSI_GPIO_Port->ODR &= ~FPGA_MOSI_Pin;
//        FPGA_MISO_GPIO_Port->ODR &= ~FPGA_MISO_Pin;
//        FPGA_SCLK_GPIO_Port->ODR &= ~FPGA_SCLK_Pin;
//        FPGA_INIT_B_GPIO_Port->ODR &= ~FPGA_INIT_B_Pin;
//        FPGA_DONE_GPIO_Port->ODR &= ~FPGA_DONE_Pin;
    }
}
