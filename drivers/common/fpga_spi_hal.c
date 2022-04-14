/*
**    Copyright 2019-2020 Ilja Slepnev
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

#include "fpga_spi_hal.h"

#include "bsp_pin_defs.h"
#include "bus/impl/spi_driver_util.h" // FIXME: use index, not handle
#include "bus/spi_driver.h"
#include "gpio.h"
#include "stm32_hal.h"

static const int SPI_TIMEOUT_MS = 100; // osWaitForever;

typedef enum {
    NSS_ASSERT = 0,
    NSS_DEASSERT = 1,
} NssState;

/**
 * @brief toggle NSS pin by software
 * @param state state of NSS pin (active low)
 */
static void fpga_spi_hal_spi_nss_b(NssState state)
{
    write_gpio_pin(FPGA_NSS_GPIO_Port, FPGA_NSS_Pin, (state == NSS_DEASSERT) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

bool fpga_spi_duplex(BusInterface *bus)
{
    struct __SPI_HandleTypeDef *hspi = hspi_handle(bus->bus_number);
    return hspi->Init.Direction == SPI_DIRECTION_2LINES;
}

bool fpga_spi_hal_tx_rx(BusInterface *bus, uint8_t *txBuf, uint8_t *rxBuf, uint16_t Size)
{
    fpga_spi_hal_spi_nss_b(NSS_ASSERT);
    bool ret = spi_driver_tx_rx(hspi_handle(bus->bus_number), (uint8_t *)txBuf, (uint8_t *)rxBuf, Size, SPI_TIMEOUT_MS);
    fpga_spi_hal_spi_nss_b(NSS_DEASSERT);
    return ret;
}

bool fpga_spi_hal_tx(BusInterface *bus, uint8_t *txBuf, uint16_t Size)
{
    fpga_spi_hal_spi_nss_b(NSS_ASSERT);
    bool ret = spi_driver_tx(hspi_handle(bus->bus_number), (uint8_t *)txBuf, Size, SPI_TIMEOUT_MS);
    fpga_spi_hal_spi_nss_b(NSS_DEASSERT);
    return ret;
}

bool fpga_spi_hal_rx(BusInterface *bus, uint8_t *rxBuf, uint16_t Size)
{
    fpga_spi_hal_spi_nss_b(NSS_ASSERT);
    bool ret = spi_driver_rx(hspi_handle(bus->bus_number), (uint8_t *)rxBuf, Size, SPI_TIMEOUT_MS);
    fpga_spi_hal_spi_nss_b(NSS_DEASSERT);
    return ret;
}

void fpga_enable_interface(BusInterface *bus)
{
    struct __SPI_HandleTypeDef *hspi = hspi_handle(bus->bus_number);
    spi_enable_interface(hspi, true);
}

void fpga_disable_interface(BusInterface *bus)
{
    struct __SPI_HandleTypeDef *hspi = hspi_handle(bus->bus_number);
    spi_enable_interface(hspi, false);
}
