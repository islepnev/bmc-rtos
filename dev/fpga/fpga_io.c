/*
**    Copyright 2020 Ilia Slepnev
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

#include "fpga_io.h"

#include "fpga_spi_hal.h"
#include "dev_fpga_types.h"

bool fpga_r16(struct Dev_fpga *d, uint32_t addr, uint16_t *data)
{
    if (d->priv.proto_version == 3) {
        uint64_t tmp = 0;
        bool ok = fpga_spi_v3_hal_read_reg(&d->dev.bus, addr, &tmp);
        if (ok && data)
            *data = tmp;
        return ok;
    }
    return fpga_spi_hal_read_reg(&d->dev.bus, addr, data);
}

bool fpga_w16(struct Dev_fpga *d, uint32_t addr, uint16_t data)
{
    if (d->priv.proto_version == 3) {
        return fpga_spi_v3_hal_write_reg(&d->dev.bus, addr, data);
    }
    return fpga_spi_hal_write_reg(&d->dev.bus, addr, data);
}

bool fpga_r32(struct Dev_fpga *dev, uint32_t addr, uint32_t *data)
{
    addr &= ~1;
    uint16_t buf[2] = {0};
    bool ok =
            fpga_r16(dev, addr + 0, &buf[0]) &&
            fpga_r16(dev, addr + 1, &buf[1])
            ;
    if (ok && data)
        *data =
            (((uint32_t)buf[0] & 0xFFFF)) |
            (((uint32_t)buf[1] & 0xFFFF) << 16);
    return ok;
}

bool fpga_w32(struct Dev_fpga *dev, uint32_t addr, uint32_t data)
{
    addr &= ~1;
    return
            fpga_w16(dev, addr + 0, data & 0xFFFF) &&
            fpga_w16(dev, addr + 1, (data >> 16) & 0xFFFF);
}

bool fpga_r64(struct Dev_fpga *dev, uint32_t addr, uint64_t *data)
{
    addr &= ~3;
    uint16_t buf[4] = {0};
    bool ok =
            fpga_r16(dev, addr + 0, &buf[0]) &&
            fpga_r16(dev, addr + 1, &buf[1]) &&
            fpga_r16(dev, addr + 2, &buf[2]) &&
            fpga_r16(dev, addr + 3, &buf[3])
            ;
    if (ok && data)
        *data =
            (((uint64_t)(buf[0]) & 0xFFFF)) |
            (((uint64_t)(buf[1]) & 0xFFFF) << 16) |
            (((uint64_t)(buf[2]) & 0xFFFF) << 32) |
            (((uint64_t)(buf[3]) & 0xFFFF) << 48);
    return ok;
}

bool fpga_w64(struct Dev_fpga *dev, uint32_t addr, uint64_t data)
{
    addr &= ~3;
    return
            fpga_w16(dev, addr + 0, data & 0xFFFF) &&
            fpga_w16(dev, addr + 1, (data >> 16) & 0xFFFF) &&
            fpga_w16(dev, addr + 2, (data >> 32) & 0xFFFF) &&
            fpga_w16(dev, addr + 3, (data >> 48) & 0xFFFF);
}

bool fpga_read(struct Dev_fpga *dev, uint32_t addr, void *buf, size_t size)
{
    if (size % 1)
        return false;
    for (size_t i=0; i<size / REGIO_WORD_SIZE; i++) {
        uint16_t rdata;
        if (!fpga_r16(dev, addr + i, &rdata))
            return false;
        uint16_t *data = (uint16_t *)buf;
        data[i] = (rdata);
    }
    return true;
}

bool fpga_write(struct Dev_fpga *dev, uint32_t addr, const void *buf, size_t size)
{
    if (size % 1)
        return false;
    for (size_t i=0; i<size / REGIO_WORD_SIZE; i++) {
        uint16_t *data = (uint16_t *)buf;
        uint16_t wdata = (data[i]);
        if (!fpga_w16(dev, addr + i, wdata))
            return false;
    }
    return true;
}
