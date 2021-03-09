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

#include "vxsiic_types.h"

uint8_t vxsiic_map_slot_to_number[VXSIIC_SLOTS] = {
    2, 3, 4, 5, 6, 7, 8, 9, 10,
    13, 14, 15, 16, 17, 18, 19, 20, 21
};

const char * vxsiic_map_slot_to_label[VXSIIC_SLOTS] = {
    "2", "3", "4", "5", "6", "7", "8", "9", "10",
    "13", "14", "15", "16", "17", "18", "19", "20", "21"
};

const uint32_t BMC_MAGIC = 0x424D4320;

bmc_ver_t make_bmc_ver(uint8_t major, uint8_t minor, uint16_t patch)
{
    bmc_ver_t data = {
        .b.major = major,
        .b.minor = minor,
        .b.patch = patch
    };
    return data;
}

fw_version_t make_fw_version(uint16_t fw_ver, uint16_t fw_rev)
{
    fw_version_t data = {
        .b.major = (fw_ver >> 8) & 0xFF,
        .b.minor = (fw_ver) & 0xFF,
        .b.patch = fw_rev
    };
    return data;
}
