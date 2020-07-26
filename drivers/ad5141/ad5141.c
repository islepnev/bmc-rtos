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

#include "ad5141.h"
#include "ad5141_i2c_hal.h"

bool ad5141_nop(BusInterface *bus)
{
    uint8_t ctrl_addr = 0x00;
    return ad5141_write(bus, ctrl_addr, 0);
}

bool ad5141_reset(BusInterface *bus)
{
    uint8_t ctrl_addr = 0xB0;
    return ad5141_write(bus, ctrl_addr, 0);
}

bool ad5141_copy_rdac_to_eeprom(BusInterface *bus)
{
    uint8_t ctrl_addr = 0x70;
    uint8_t data = 0x01;
    if (! ad5141_write(bus, ctrl_addr, data))
        return false;
    // wait 18 ms
    for (int i=0; i<360; i++)
        if (! ad5141_nop(bus))
            return false;
    return true;
}

bool ad5141_copy_eeprom_to_rdac(BusInterface *bus)
{
    uint8_t ctrl_addr = 0x70;
    uint8_t data = 0x00;
    return ad5141_write(bus, ctrl_addr, data);
}

bool ad5141_write_rdac(BusInterface *bus, uint8_t data)
{
    uint8_t ctrl_addr = 0x10;
    return ad5141_write(bus, ctrl_addr, data);
}

bool ad5141_read_rdac(BusInterface *bus, uint8_t *data)
{
    uint16_t command = 0x3003;
    return ad5141_read(bus, command, data);
}

bool ad5141_inc_rdac(BusInterface *bus)
{
    uint8_t ctrl_addr = 0x40;
    return ad5141_write(bus, ctrl_addr, 1);
}

bool ad5141_dec_rdac(BusInterface *bus)
{
    uint8_t ctrl_addr = 0x40;
    return ad5141_write(bus, ctrl_addr, 0);
}
