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

#include "board_config_ad9548.h"

#include "bsp.h"

const uint64_t ad9548_dds_period_fs = 8000000; // design constant

#if defined(BOARD_TDC72VHLV3)
const char *ad9548_ref_label(int refIndex)
{
    switch (refIndex) {
    case 0:
    case 1:
        return "VCXO   125  MHz";
    case 2:
    case 3:
        return "TTC    41.7 MHz";
    case 4:
    case 5:
        return "VXS    41.7 MHz";
    case 6:
    case 7:
        return "VCXO/3 41.7 MHz";
    default:
        return "?";
    }
}
#elif defined BOARD_TQDC16VSV1
const char *ad9548_ref_label(int refIndex)
{
    switch (refIndex) {
    case 0:
    case 1:
        return "FPGA";
    case 2:
    case 3:
        return "TTC ";
    case 4:
    case 5:
        return "    ";
    case 6:
    case 7:
        return "VCXO";
    default:
        return "?   ";
    }
}
#elif defined BOARD_ADC64VE
const char *ad9548_ref_label(int refIndex)
{
    return "?";
}
#else
const char *ad9548_ref_label(int refIndex)
{
    return "?";
}
#endif
