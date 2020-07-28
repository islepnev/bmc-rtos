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

#include "devicelist.h"

#include <assert.h>
#include <stdio.h>

#include "ansi_escape_codes.h"

const char *device_class_str(DeviceClass class)
{
    switch (class) {
    case DEV_CLASS_0: return "none";
    case DEV_CLASS_VIRTUAL: return "Virtual";
    case DEV_CLASS_AUXPLL: return "AuxPLL";
    case DEV_CLASS_CLKMUX: return "clkmux";
    case DEV_CLASS_DIGIPOT: return "DigiPot";
    case DEV_CLASS_EEPROM_CONFIG: return "EEPROM-Config";
    case DEV_CLASS_FPGA: return "FPGA";
    case DEV_CLASS_PLL: return "PLL";
    case DEV_CLASS_POWERMON: return "PowerMon";
    case DEV_CLASS_SFP: return "SFP";
    case DEV_CLASS_THERM: return "Therm";
    case DEV_CLASS_VXSIICM: return "VXSIICM";
    case DEV_CLASS_VXSIICS: return "VXSIICS";
    default:
        assert(0);
        return "?";
    }
}

const char *bus_type_str(BusType type)
{
    switch (type) {
    case BUS_IIC: return "IIC";
    case BUS_SPI: return "SPI";
    default: assert(0); return "?";
    }
}

void devicelist_print(DeviceBase *d, int depth)
{
    if (!d) {
        return;
    }
    const BusInterface *bus = &d->bus;
    enum {BUFSZ = 16};
    char str[BUFSZ+1] = {0};
    int i=0;
    while (i<depth && i < BUFSZ) {
        str[i++] = ' ';
        str[i++] = ' ';
    }
    printf("%s %s %d.%02X    %s%s\n",
           str,
           bus_type_str(bus->type),
           bus->bus_number,
           bus->address,
           device_class_str(d->class),
           ANSI_CLEAR_EOL);

    DeviceBase *p = d->children;
    while (p) {
        devicelist_print(p, depth + 1);
        p = p->next;
    }
}
