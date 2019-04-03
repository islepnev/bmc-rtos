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
#include "spi.h"
#include "bsp.h"
#include "bsp_pin_defs.h"
#include "logbuffer.h"

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

/**
 * @brief Read FPGA register
 * @param addr 15-bit address
 * @param data pointer to 16-bit destination
 * @return @arg HAL_OK on success
 */
HAL_StatusTypeDef fpga_spi_hal_read_reg(uint16_t addr, uint16_t *data)
{
    enum {Size = 2}; // number of 16-bit words
    uint16_t txBuf[2];
    uint16_t rxBuf[2];
    txBuf[0] = (0x8000 | (addr & 0x7FFF));
    txBuf[1] = 0;
    fpga_spi_hal_spi_nss_b(NSS_ASSERT);
    HAL_StatusTypeDef ret = HAL_SPI_TransmitReceive(fpga_spi, (uint8_t *)txBuf, (uint8_t *)rxBuf, Size, SPI_TIMEOUT_MS);
    fpga_spi_hal_spi_nss_b(NSS_DEASSERT);
    if (HAL_OK != ret) {
        return ret;
    }
    uint16_t result = rxBuf[1];
    if (data) {
        *data = result;
    }
    return ret;
}

/**
 * @brief Write FPGA register
 * @param addr 15-bit address
 * @param data 16-bit data to write
 * @return @arg HAL_OK on success
 */
HAL_StatusTypeDef fpga_spi_hal_write_reg(uint16_t addr, uint16_t data)
{
    enum {Size = 2};
    uint16_t txBuf[2];
    txBuf[0] = (0x0000 | (addr & 0x7FFF));
    txBuf[1] = data;
    fpga_spi_hal_spi_nss_b(NSS_ASSERT);
    HAL_StatusTypeDef ret = HAL_SPI_Transmit(fpga_spi, (uint8_t *)txBuf, Size, SPI_TIMEOUT_MS);
    fpga_spi_hal_spi_nss_b(NSS_DEASSERT);
    if (HAL_OK != ret) {
        log_printf(LOG_ERR, "fpga_spi_hal_write_reg: SPI error %d\n", ret);
        return ret;
    }
    return ret;
}
