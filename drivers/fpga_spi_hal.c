//
//    Copyright 2019 Ilja Slepnev
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include "fpga_spi_hal.h"
#include <stdint.h>
#include "stm32f7xx.h"
#include "stm32f7xx_hal.h"
#include "main.h"
#include "spi.h"

static const int SPI_TIMEOUT_MS = HAL_MAX_DELAY;

typedef enum {
    NSS_ASSERT = 0,
    NSS_DEASSERT = 1,
} NssState;

/**
 * @brief toggle NSS pin by software
 * @param state state of NSS pin (active low)
 */
void fpga_spi_hal_spi_nss_b(NssState state)
{
    HAL_GPIO_WritePin(FPGA_NSS_GPIO_Port, FPGA_NSS_Pin, (state == NSS_DEASSERT) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

/*
 * read 13-bit data from FPGA MCU register
 */
HAL_StatusTypeDef fpga_spi_hal_read_reg(uint8_t addr, uint16_t *data)
{
    uint8_t buf1[4] = {
        (0x80 | ((addr >> 8) & 0x7F)),
        (addr & 0xFF),
        0,
        0
    };
    uint8_t buf2[4] = {0,0,0,0};
    fpga_spi_hal_spi_nss_b(NSS_ASSERT);
    HAL_StatusTypeDef ret = HAL_SPI_TransmitReceive(fpga_spi, buf1, buf2, 4, SPI_TIMEOUT_MS);
    fpga_spi_hal_spi_nss_b(NSS_DEASSERT);
    if (HAL_OK != ret) {
        printf("%s: SPI error\n", __func__);
        return ret;
    }
    if (data) {
        *data = ((uint16_t)buf2[3] << 8) | buf2[2];
    }
    return ret;
}

/**
 * @brief Write FPGA register
 * @param addr 15-bit address
 * @param data 16-bit data
 * @return @arg FPGA_SPI_OK on success
 */
HAL_StatusTypeDef fpga_spi_hal_write_reg(uint8_t addr, uint16_t data)
{
    uint8_t buf1[4] = {
        (0x00 | ((addr >> 8) & 0x7F)),
        (addr & 0xFF),
        (data >> 8),
        (data & 0xFF)
    };
    fpga_spi_hal_spi_nss_b(NSS_ASSERT);
    HAL_StatusTypeDef ret = HAL_SPI_Transmit(fpga_spi, buf1, 4, SPI_TIMEOUT_MS);
    fpga_spi_hal_spi_nss_b(NSS_DEASSERT);
    if (HAL_OK != ret) {
        printf("%s: SPI error\n", __func__);
        return ret;
    }
    return ret;
}
