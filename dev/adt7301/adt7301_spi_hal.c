/*
**    Copyright 2017-2020 Ilja Slepnev
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

#include "adt7301_spi_hal.h"

#include "bsp.h"
#include "bsp_pin_defs.h"
#include "gpio.h"
#include "spi.h"
#include "bus/spi_driver.h"
#include "bus/impl/spi_driver_util.h"

#ifdef BOARD_TDC72

static const int SPI_TIMEOUT_MS = 10;

bool adt7301_read(BusInterface *bus, int16_t *data)
{
    uint16_t SPI_transmit_buffer = {0};
    uint16_t SPI_receive_buffer = {0};

    GPIO_TypeDef * port;
    uint16_t cs_pin;

    switch (bus->address)
    {
    case 0:
        port = ADT_CS_B0_GPIO_Port;
        cs_pin = ADT_CS_B0_Pin;
        break;
    case 1:
        port = ADT_CS_B1_GPIO_Port;
        cs_pin = ADT_CS_B1_Pin;
        break;
    case 2:
        port = ADT_CS_B2_GPIO_Port;
        cs_pin = ADT_CS_B2_Pin;
        break;
    case 3:
        port = ADT_CS_B3_GPIO_Port;
        cs_pin = ADT_CS_B3_Pin;
        break;
    default:
        port = ADT_CS_B0_GPIO_Port;
        cs_pin = ADT_CS_B0_Pin;
        break;
    }

    write_gpio_pin(port, cs_pin, 0);
    bool ret = spi_driver_tx_rx(hspi_handle(bus->bus_number),
                                (uint8_t *)&SPI_transmit_buffer,
                                (uint8_t *)&SPI_receive_buffer,
                                1,
                                SPI_TIMEOUT_MS);
    write_gpio_pin(port, cs_pin, 1);
    if (ret && data) {
        int16_t result = SPI_receive_buffer;
        *data = (result << 2) >> 2;
    }
    return ret;
}

#endif

#ifdef TTVXS_1_0

static const int SPI_TIMEOUT_MS = 10;

bool adt7301_read_temp(BusInterface *bus, int16_t *data)
{

    uint16_t SPI_transmit_buffer = {0};
    uint16_t SPI_receive_buffer = {0};

    GPIO_TypeDef * port;
    uint16_t cs_pin;

    switch (bus->address)
    {
    default:
        port = ADT_CS_B_GPIO_Port;
        cs_pin = ADT_CS_B_Pin;
        break;
    }

    write_gpio_pin(port, cs_pin, 0);
    bool ret = spi_driver_tx_rx(hspi_handle(bus->bus_number),
                                (uint8_t *)&SPI_transmit_buffer,
                                (uint8_t *)&SPI_receive_buffer,
                                1,
                                SPI_TIMEOUT_MS);
    write_gpio_pin(port, cs_pin, 1);
    if (ret && data) {
        int16_t result = SPI_receive_buffer;
        *data = (result << 2) >> 2;
    }
    return ret;
}

#endif
