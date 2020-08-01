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

#include "dev_fpga.h"

#include "../ad9545/dev_ad9545.h"
#include "bsp_fpga.h"
#include "dev_fpga_types.h"
#include "thset/dev_thset_types.h"
#include "devices_types.h"
#include "fpga_spi_hal.h"
#include "logbuffer.h"
#include "powermon/dev_powermon_types.h"
#include "system_status.h"
#include "system_status_common.h"
#include "version.h"

static uint16_t live_magic = 0x55AA;

static bool fpga_test_reg(DeviceBase *dev, uint16_t addr, uint16_t wdata, uint16_t *rdata)
{
    BusInterface *bus = &dev->bus;
    return fpga_spi_hal_write_reg(bus, addr, wdata) &&
           fpga_spi_hal_read_reg(bus, addr, rdata) &&
           rdata &&
           (wdata == *rdata);
}

static void fpga_write_live_magic(DeviceBase *dev)
{
    BusInterface *bus = &dev->bus;
    uint16_t addr1 = 0x000E;
    uint16_t addr2 = 0x000F;
    live_magic++;
    uint16_t wdata1 = live_magic;
    uint16_t wdata2 = ~live_magic;
    fpga_spi_hal_write_reg(bus, addr1, wdata1);
    fpga_spi_hal_write_reg(bus, addr2, wdata2);
}

bool fpga_check_live_magic(DeviceBase *dev)
{
    BusInterface *bus = &dev->bus;
    uint16_t addr1 = 0x000E;
    uint16_t addr2 = 0x000F;
    uint16_t rdata1 = 0, rdata2 = 0;
    fpga_spi_hal_read_reg(bus, addr1, &rdata1);
    fpga_spi_hal_read_reg(bus, addr2, &rdata2);
    uint16_t test1 = live_magic;
    uint16_t test2 = ~live_magic;
    if ((rdata1 != test1) || (rdata2 != test2)) {
        log_put(LOG_ERR, "FPGA register contents unexpectedly changed");
        return false;
    }
    fpga_write_live_magic(dev);
    return true;
}

bool fpga_test(DeviceBase *dev)
{
    BusInterface *bus = &dev->bus;
    uint16_t addr1 = 0x000E;
    uint16_t addr2 = 0x000F;
    uint16_t wdata1 = 0x3210;
    uint16_t wdata2 = 0xDCBA;
    uint16_t rdata1 = 0, rdata2 = 0;
    if (! fpga_spi_hal_write_reg(bus, addr1, wdata1))
        goto err;
    if (! fpga_spi_hal_write_reg(bus, addr2, wdata2))
        goto err;
    if (! fpga_spi_hal_read_reg(bus, addr1, &rdata1))
        goto err;
    if (! fpga_spi_hal_read_reg(bus, addr2, &rdata2))
        goto err;
    if (rdata1 == wdata1 && rdata2 == wdata2) {
        log_printf(LOG_INFO, "FPGA register test Ok: addr1 %04X, wdata1 %04X, rdata1 %04X", addr1, wdata1, rdata1);
        log_printf(LOG_INFO, "FPGA register test Ok: addr2 %04X, wdata2 %04X, rdata2 %04X", addr2, wdata2, rdata2);
        fpga_write_live_magic(dev);
        return true;
    }
    log_printf(LOG_ERR, "FPGA register test failed: addr1 %04X, wdata1 %04X, rdata1 %04X", addr1, wdata1, rdata1);
    log_printf(LOG_ERR, "FPGA register test failed: addr2 %04X, wdata2 %04X, rdata2 %04X", addr2, wdata2, rdata2);
    return false;
err:
    log_printf(LOG_ERR, "FPGA register test failed: SPI error");
    return false;
}

bool fpgaDetect(Dev_fpga *d)
{
    BusInterface *bus = &d->dev.bus;
    int err = 0;
    for (int i=0; i<FPGA_REG_COUNT; i++) {
        if (! fpga_spi_hal_read_reg(bus, i, &d->priv.regs[i])) {
            err++;
            break;
        }
    }
    uint16_t id = d->priv.regs[0];
    if (id == 0x0000 || id == 0xFFFF)
        err++;
    if (err == 0)
        d->dev.device_status = DEVICE_NORMAL;
//    else
//        d->present = DEVICE_FAIL;

    d->priv.id = id;

//    if (0 && (DEVICE_NORMAL == d->present)) {

        //        fpga_test_reg(addr, wdata, &rdata);
//        if (rdata != wdata)
//            log_printf(LOG_ERR, "FPGA register test failed: addr %04X, wdata %04X, rdata %04X", addr, wdata, rdata);
//    }
    return DEVICE_NORMAL == d->dev.device_status;
}

bool fpgaWriteBmcVersion(DeviceBase *dev)
{
    BusInterface *bus = &dev->bus;
    if (! fpga_spi_hal_write_reg(bus, FPGA_SPI_ADDR_8, VERSION_MAJOR_NUM))
        return false;
    if (! fpga_spi_hal_write_reg(bus, FPGA_SPI_ADDR_9, VERSION_MINOR_NUM))
        return false;
    return true;
}

bool fpgaWriteBmcTemperature(DeviceBase *dev)
{
    BusInterface *bus = &dev->bus;
    const Dev_thset_priv *p = get_thset_priv_const();
    if (!p)
        return false;

    for (int i=0; i<4; i++) {
        int16_t v = (i < p->count && p->sensors[i].hdr.b.state == DEVICE_NORMAL)
                        ? (p->sensors[i].value * 32)
                        : 0x8000;
        if (! fpga_spi_hal_write_reg(bus, FPGA_SPI_ADDR_3 + i, v))
            return false;
    }
    return true;
}

bool fpgaWritePllStatus(DeviceBase *dev)
{
    BusInterface *bus = &dev->bus;
    const DeviceBase *d = find_device_const(DEV_CLASS_AD9545);
    if (!d || !d->priv)
        return false;
    const Dev_ad9545_priv *priv = (Dev_ad9545_priv *)device_priv_const(d);

    uint16_t data = 0;
    if (SENSOR_NORMAL == d->sensor) {
        data |= 0x8;
    } else {
        if (priv->status.sysclk.b.pll0_locked)
            data |= 0x1;
    }
    if (! fpga_spi_hal_write_reg(bus, FPGA_SPI_ADDR_1, data))
        return false;
    return true;
}

bool fpgaWriteSystemStatus(DeviceBase *dev)
{
    BusInterface *bus = &dev->bus;
    uint16_t data = 0;
    data = getSystemStatus();
    if (! fpga_spi_hal_write_reg(bus, FPGA_SPI_ADDR_A, data))
        return false;
    data = getPowermonStatus();
    if (! fpga_spi_hal_write_reg(bus, FPGA_SPI_ADDR_B, data))
        return false;
    data = getPllStatus();
    if (! fpga_spi_hal_write_reg(bus, FPGA_SPI_ADDR_C, data))
        return false;
    return true;
}

static bool fpgaWriteSensorsByIndex(DeviceBase *dev, int *indices, int count)
{
    const Dev_powermon_priv *p = get_powermon_priv_const();
    if (!p)
        return false;
    const pm_sensors_arr *sensors = &p->sensors;
    BusInterface *bus = &dev->bus;
    uint16_t address = FPGA_SPI_ADDR_0 + 0x10;

    for (int i=0; i<count; i++) {
        SensorIndex index = (SensorIndex)indices[i];
        const pm_sensor_priv *p = &sensors->arr[index].priv;
        if (!fpga_spi_hal_write_reg(bus, address++,
                                    (int16_t)(p->busVoltage * 1000)) &&
            fpga_spi_hal_write_reg(bus, address++,
                                   (int16_t)(p->current * 1000)))
            return false;
    }
    return true;
}

bool fpgaWriteSensors(DeviceBase *dev)
{
    return fpgaWriteSensorsByIndex(
        dev, fpga_sensor_map.indices, fpga_sensor_map.count);
}
