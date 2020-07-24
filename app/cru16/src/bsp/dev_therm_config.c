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

// CRU16 board specific definitions

#include "dev_therm_config.h"

BusType thermBusType(ThermIndex index)
{
    switch(index) {
    case THERM_FPGA_DIE: return BUS_IIC;
    case THERM_VCXO: return BUS_IIC;
    default: return 0;
    }
}

int thermBusIndex(ThermIndex index)
{
    switch(index) {
    case THERM_FPGA_DIE: return 2;
    case THERM_VCXO: return 2;
    default: return 0;
    }
}

int thermBusAddress(ThermIndex index)
{
    switch(index) {
    case THERM_FPGA_DIE: return 0x1C;
    case THERM_VCXO: return 0x56;
    default: return 0;
    }
}

ThermType thermType(ThermIndex index)
{
    switch(index) {
    case THERM_FPGA_DIE: return THERM_TMP421;
    case THERM_VCXO: return THERM_MAX31725;
    default: return 0;
    }
}

const char *thermLabel(ThermIndex index)
{
    switch(index) {
    case THERM_FPGA_DIE: return "FPGA";
    case THERM_VCXO: return "VCXO";
    default: return 0;
    }
}
