/*
**    Copyright 2020 Ilja Slepnev
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

#include "ad9548.h"

#include "bsp_pin_defs.h"
#include "gpio.h"
#include "ad9548_setup_regs.h"
#include "ad9548_status_regs.h"
#include "bus/spi_driver.h"
#include "bus/impl/spi_driver_util.h" // FIXME: use index, not handle
#include "log/log.h"

enum {DEVICE_ID_AD9548 = 0x48};
enum {REVISION_ID_AD9548 = 0xC6};

bool ad9548_gpio_init(BusInterface *bus)
{
    // TODO: setup GPIO pins
    return true;
}

bool ad9548_gpio_test(BusInterface *bus)
{
#if defined (PLL_RESET_Pin) && defined (PLL_IRQ_B_Pin)
    bool pin_reset = read_gpio_pin(PLL_RESET_GPIO_Port, PLL_RESET_Pin);
    bool pin_irqb = read_gpio_pin(PLL_IRQ_B_GPIO_Port, PLL_IRQ_B_Pin);
    if (!pin_reset)
        return true;
    else {
        log_printf(LOG_NOTICE, "PLL GPIO: reset=%u, irq=%u", pin_reset, pin_irqb);
        return false;
    }
#endif
    return false;
}

static void set_nss(bool state)
{
//    write_gpio_pin(PLL_SPI_NSS_GPIO_Port, PLL_SPI_NSS_Pin, state);
}

bool ad9548_reset(BusInterface *bus)
{
    write_gpio_pin(PLL_RESET_GPIO_Port, PLL_RESET_Pin, 1);
    write_gpio_pin(PLL_RESET_GPIO_Port, PLL_RESET_Pin, 0);
    return true;
}

uint8_t ad9548_read_register(BusInterface *bus, uint16_t address)
{
    struct __SPI_HandleTypeDef *spi = hspi_handle(bus->bus_number);
    uint8_t data_out = 0x00;
    uint8_t data_in = 0x00;
    uint8_t address_L = address & 0xff;						// Extract 8 LSBs of the specified address
    uint8_t address_H = (address >> 8) & 0b11111;			// Extract 5 MSBs of the address

    data_out |= (1 << 7) | (0 << 5) | address_H;			// Set R/~W bit to R, byte number to 1 and inserting 5 MSBs of the address
    set_nss(0);
    bool ret = spi_driver_tx(spi, &data_out, 1, 1000);
    set_nss(1);

    data_out = 0x00;
    data_out |= address_L;									// Loading 8 LSBs of the address into the SPI output buffer
    set_nss(0);
    ret = spi_driver_tx(spi, &data_out, 1, 1000);
    set_nss(1);

    data_out = 0x00; // dummy
    set_nss(0);
    ret = spi_driver_tx_rx(spi, &data_out, &data_in, 1, 1000);
    set_nss(1);

    return data_in;
}

bool ad9548_write_register(BusInterface *bus, uint16_t address, uint8_t value)
{
    struct __SPI_HandleTypeDef *spi = hspi_handle(bus->bus_number);
    uint8_t data_out;
    uint8_t address_L = address & 0xff;						// Extract 8 LSBs of the specified address
    uint8_t address_H = (address >> 8) & 0b11111;			// Extract 5 MSBs of the address

    data_out = 0x0;
    data_out |= (0 << 7) | (0 << 5) | address_H;			// Set R/~W bit to ~W, byte number to 1 and inserting 5 MSBs of the address
    set_nss(0);
    bool ret = spi_driver_tx(spi, &data_out, 1, 1000);
    set_nss(1);
    if (!ret)
        return false;

    data_out = 0x0;
    data_out |= address_L;									// Loading 8 LSBs of the address into the SPI output buffer
    set_nss(0);
    ret = spi_driver_tx(spi, &data_out, 1, 1000);
    set_nss(1);
    if (!ret)
        return false;

    set_nss(0);
    ret = spi_driver_tx(spi, &value, 1, 1000);
    set_nss(1);
    if (!ret)
        return false;

    return true;
}

void ad9548_ioupdate(BusInterface *bus)
{
    ad9548_write_register(bus, 0x0005, 0x01);
}

bool ad9548_detect(BusInterface *bus)
{
    uint8_t device_id = ad9548_read_register(bus, 0x3);
    uint8_t revision_id = ad9548_read_register(bus, 0x2);
    log_printf(LOG_DEBUG, "AD9548 ID: %02X %02X", device_id, revision_id);
    if (device_id == DEVICE_ID_AD9548 && revision_id == REVISION_ID_AD9548) return true;
    return false;
}
