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

#include "ad9516_spi_hal.h"

#include "bsp_pin_defs.h"
#include "bus/impl/spi_driver_util.h" // FIXME: use index, not handle
#include "bus/spi_driver.h"
#include "cmsis_os.h"
#include "gpio.h"
#include "log/log.h"
#include "spi.h"

static const int SPI_TIMEOUT_MS = 500;

static bool set_csb(BusInterface *bus, int state)
{
    return true; // SPI_NSS_HARD_OUTPUT
//    if (ad9516_spi.Init.NSS != SPI_NSS_SOFT)
//        return true;
    bool write = state;
    write_gpio_pin(AD9516_CS_GPIO_Port, AD9516_CS_Pin, write);
    bool read = read_gpio_pin(AD9516_CS_GPIO_Port, AD9516_CS_Pin);
    if (write != read) {
        log_printf(LOG_CRIT, "AD9516_CS_B stuck %s", read ? "high": "low");
    }
    return write == read;
}

static bool ad9516_read1_duplex(BusInterface *bus, uint16_t reg, uint8_t *data)
{
    enum {Size = 3};
    uint8_t tx[Size];
    uint8_t rx[Size];
    reg &= 0x1FFF;
    tx[0] = 0x80 | (reg >> 8);  // MSB first, bit 15 = read
    tx[1] = reg & 0xFF;
    tx[2] = 0;
    set_csb(bus, 0);
    bool ret = spi_driver_tx_rx(hspi_handle(bus->bus_number), tx, rx, Size, SPI_TIMEOUT_MS);
    set_csb(bus, 1);
    if (ret) {
        if (data) {
            *data = rx[2];
        }
    }
    return ret;
}

static bool ad9516_write1_internal(BusInterface *bus, uint16_t reg, uint8_t data)
{
    enum {Size = 3};
    uint8_t tx[Size];
    reg &= 0x1FFF;
    tx[0] = (reg >> 8) & 0x1F;  // MSB first
    tx[1] = reg & 0xFF;
    tx[2] = data;
    set_csb(bus, 0);
    bool ret = spi_driver_tx(hspi_handle(bus->bus_number), tx, Size, SPI_TIMEOUT_MS);
    set_csb(bus, 1);
    return ret;
}

void ad9516_test_loop(BusInterface *bus)
{
    uint16_t reg = 0x011; // r-counter lsb
    uint8_t data = 0x55;
    while (1) {
        //sdata = ~data;
        ad9516_write1_internal(bus, reg, data);
        uint8_t read = 0;
        ad9516_read1_duplex(bus, reg, &read);
        osDelay(1);
    }
}

bool ad9516_read1(BusInterface *bus, uint16_t reg, uint8_t *data)
{
    return ad9516_read1_duplex(bus, reg, data);
}

bool ad9516_write1(BusInterface *bus, uint16_t reg, uint8_t data)
{
    if (! ad9516_write1_internal(bus, reg, data))
        return false;
    if (! ad9516_write1_internal(bus, 0x232, 1))
        return false; // io update
    uint8_t readback = 0;
    if (! ad9516_read1(bus, reg, &readback))
        return false;
    if ((reg != 0x232) && (readback != data)) {
        log_printf(LOG_ERR, "Error writing %04X: wrote %02X, read %02X", reg, data, readback);
        return false;
    }
    return true;
}

bool ad9516_write_config(BusInterface *bus, uint8_t data)
{
    return ad9516_write1_internal(bus, 0, data);
}

void ad9516_enable_interface(BusInterface *bus)
{
    struct __SPI_HandleTypeDef *hspi = hspi_handle(bus->bus_number);
    spi_enable_interface(hspi, true);
}

void ad9516_disable_interface(BusInterface *bus)
{
    struct __SPI_HandleTypeDef *hspi = hspi_handle(bus->bus_number);
    spi_enable_interface(hspi, false);
}
