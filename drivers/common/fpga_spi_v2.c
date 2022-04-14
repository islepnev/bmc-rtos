/*
**    Copyright 2022 Ilia Slepnev
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

#include "fpga_spi_v2.h"

#include "fpga_spi_hal.h"
#include "log/log.h"

/**
 * @brief Read FPGA register
 * @param addr 15-bit address
 * @param data pointer to 16-bit destination
 * @return @arg true on success
 */
bool fpga_spi_hal_read_reg(BusInterface *bus, uint16_t addr, uint16_t *data)
{
    uint16_t tx_data = (0x8000 | (addr & 0x7FFF));
    if (fpga_spi_duplex(bus)) {
        enum {Size = 2}; // number of 16-bit words
        uint16_t txBuf[2] = {tx_data, 0};
        uint16_t rxBuf[2] = {0};
        bool ret = fpga_spi_hal_tx_rx(bus, (uint8_t *)txBuf, (uint8_t *)rxBuf, Size);
        if (ret && data) {
            uint16_t result = rxBuf[1];
            *data = result;
        }
        return ret;
    } else {
        enum {Size = 1}; // number of 16-bit words
        uint16_t txBuf[1] = {tx_data};
        uint16_t rxBuf[1] = {0};
        bool ret = fpga_spi_hal_tx(bus, (uint8_t *)txBuf, Size);
        ret &= fpga_spi_hal_rx(bus, (uint8_t *)rxBuf, Size);
        if (ret && data) {
            uint16_t result = rxBuf[0];
            *data = result;
        }
        return ret;
    }
}

/**
 * @brief Write FPGA register
 * @param addr 15-bit address
 * @param data 16-bit data to write
 * @return @arg true on success
 */
bool fpga_spi_hal_write_reg(BusInterface *bus, uint16_t addr, uint16_t data)
{
    enum {Size = 2};
    uint16_t txBuf[2];
    txBuf[0] = (0x0000 | (addr & 0x7FFF));
    txBuf[1] = data;
    bool ret = fpga_spi_hal_tx(bus, (uint8_t *)txBuf, Size);
    if (! ret) {
        log_printf(LOG_ERR, "fpga_spi_hal_write_reg: SPI error");
        return ret;
    }
    return ret;
}
