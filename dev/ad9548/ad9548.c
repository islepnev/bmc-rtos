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

#define AD9548_HARDWARE_NSS 1

void ad9548_gpio_init(BusInterface *bus)
{
    (void) bus;
    // TODO: setup GPIO pins
}

bool ad9548_gpio_test(BusInterface *bus)
{
    (void) bus;
#if defined (AD9548_RESET_Pin) && defined (AD9548_IRQ_B_Pin)
    bool pin_reset = read_gpio_pin(AD9548_RESET_GPIO_Port, AD9548_RESET_Pin);
    bool pin_irqb = read_gpio_pin(AD9548_IRQ_B_GPIO_Port, AD9548_IRQ_B_Pin);
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
#if AD9548_HARDWARE_NSS
    (void) state;
#else
    write_gpio_pin(AD9548_SPI_NSS_GPIO_Port, AD9548_SPI_NSS_Pin, state);
#endif
}

void ad9548_write_reset_pin(BusInterface *bus, bool reset)
{
    (void) bus;

    write_gpio_pin(AD9548_RESET_GPIO_Port, AD9548_RESET_Pin, reset);
}

bool ad9548_read_register(BusInterface *bus, uint16_t address, uint8_t *data)
{
    struct __SPI_HandleTypeDef *spi = hspi_handle(bus->bus_number);
    uint8_t data_out = 0x00;
    uint8_t data_in = 0x00;
    uint8_t address_L = address & 0xff;						// Extract 8 LSBs of the specified address
    uint8_t address_H = (address >> 8) & 0b11111;			// Extract 5 MSBs of the address

    set_nss(0);

    data_out |= (1 << 7) | (0 << 5) | address_H;			// Set R/~W bit to R, byte number to 1 and inserting 5 MSBs of the address
    bool ret = spi_driver_tx(spi, &data_out, 1, 1000);
    if (!ret)
        goto err;

    data_out = 0x00;
    data_out |= address_L;									// Loading 8 LSBs of the address into the SPI output buffer
    ret = spi_driver_tx(spi, &data_out, 1, 1000);
    if (!ret)
        goto err;

    data_out = 0x00; // dummy
    ret = spi_driver_rx(spi, &data_in, 1, 1000);
    if (!ret)
        goto err;

    set_nss(1);

    if (data)
        *data = data_in;
    return true;
err:
    set_nss(1);
    return false;
}

bool ad9548_write_register(BusInterface *bus, uint16_t address, uint8_t value)
{
    struct __SPI_HandleTypeDef *spi = hspi_handle(bus->bus_number);
    uint8_t data_out;
    uint8_t address_L = address & 0xff;						// Extract 8 LSBs of the specified address
    uint8_t address_H = (address >> 8) & 0b11111;			// Extract 5 MSBs of the address

    set_nss(0);

    data_out = 0x0;
    data_out |= (0 << 7) | (0 << 5) | address_H;			// Set R/~W bit to ~W, byte number to 1 and inserting 5 MSBs of the address
    bool ret = spi_driver_tx(spi, &data_out, 1, 1000);
    if (!ret)
        goto err;

    data_out = 0x0;
    data_out |= address_L;									// Loading 8 LSBs of the address into the SPI output buffer
    ret = spi_driver_tx(spi, &data_out, 1, 1000);
    if (!ret)
        goto err;

    ret = spi_driver_tx(spi, &value, 1, 1000);
    if (!ret)
        goto err;

    set_nss(1);

    return true;
err:
    set_nss(1);
    return false;
}

bool ad9548_ioupdate(BusInterface *bus)
{
    return ad9548_write_register(bus, 0x0005, 0x01);
}

bool ad9548_check_id(BusInterface *bus, bool *ok)
{
    uint8_t device_id;
    uint8_t revision_id;
    if (!ad9548_read_register(bus, AD9545_REG1_DEVICE_ID, &device_id) ||
        !ad9548_read_register(bus, AD9545_REG1_REVISION_ID, &revision_id))
        return false;
    if (ok)
        *ok = (device_id == DEVICE_ID_AD9548 && revision_id == REVISION_ID_AD9548);
    return true;
}

bool ad9548_detect(BusInterface *bus)
{
    const int cycles = 100;
    uint8_t device_id;
    uint8_t revision_id;
    for (int i=0; i<cycles; i++) {
        if (!ad9548_read_register(bus, AD9545_REG1_DEVICE_ID, &device_id))
            return false;
        if (device_id == DEVICE_ID_AD9548) {
            if (i>0)
                log_printf(LOG_DEBUG, "AD9548: ID found on step %d", i);
            break;
        }
    }
    for (int i=0; i<cycles; i++) {
        if (!ad9548_read_register(bus, AD9545_REG1_DEVICE_ID, &device_id))
            return false;
        if (device_id != DEVICE_ID_AD9548) {
            // log_printf(LOG_ERR, "AD9548: bad ID %02X on step %d", device_id, i);
            return false;
        }
    }
    if (device_id != DEVICE_ID_AD9548) return false;
    if (!ad9548_read_register(bus, AD9545_REG1_REVISION_ID, &revision_id))
        return false;
    if (device_id == DEVICE_ID_AD9548 && revision_id == REVISION_ID_AD9548) return true;
    log_printf(LOG_WARNING, "Unknown AD9548 ID: %02X %02X", device_id, revision_id);
    return false;
}

bool ad9548_repeat_read_register(BusInterface *bus, uint16_t address)
{
    const int cycles = 100;
    uint8_t first_data;
    if (!ad9548_read_register(bus, address, &first_data))
        goto ioerr;
    uint8_t data;
    for (int i=0; i<cycles; i++) {
        if (!ad9548_read_register(bus, address, &data))
            goto ioerr;
        if (data != first_data) {
            log_printf(LOG_ERR, "AD9548: read %02X, %02X [%d]", first_data, data, i);
            goto dataerr;
        }
    }
    return true;
ioerr:
    log_put(LOG_ERR, "AD9548: SPI error");
    return false;
dataerr:
    log_put(LOG_ERR, "AD9548: data error");
    return false;
}
