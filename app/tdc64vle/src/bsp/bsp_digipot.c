/*
**    Digital Potentiometers
**
**    Copyright 2019-2021 Ilja Slepnev
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

#include "bsp_digipot.h"

#include <stdio.h>

#include "bsp_sensors_config.h"

const char *potLabel(PotIndex index)
{
    switch (index) {
    case POT_TDC_A: return "TDC-A";
    case POT_TDC_B: return "TDC-B";
    }
    return "";
}

int potBusNumber(PotIndex index)
{
    (void)index;
    return 2;
}

int potBusAddress(PotIndex index)
{
    switch (index) {
    case POT_TDC_A: return 0x20;
    case POT_TDC_B: return 0x23;
    }
    return 0;
}

int potSensorIndex(PotIndex index)
{
    switch (index) {
    case POT_TDC_A: return SENSOR_TDCA_2V5;
    case POT_TDC_B: return SENSOR_TDCB_2V5;
    }
    return 0;
}
