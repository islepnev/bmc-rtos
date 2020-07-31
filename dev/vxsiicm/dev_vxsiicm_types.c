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

#include <stdint.h>

#include "dev_vxsiicm_types.h"

const uint32_t BMC_MAGIC = 0x424D4320;

uint8_t vxsiic_map_slot_to_number[VXSIIC_SLOTS] = {
    2, 3, 4, 5, 6, 7, 8, 9, 10,
    13, 14, 15, 16, 17, 18, 19, 20, 21
};
const char * vxsiic_map_slot_to_label[VXSIIC_SLOTS] = {
    "2", "3", "4", "5", "6", "7", "8", "9", "10",
    "13", "14", "15", "16", "17", "18", "19", "20", "21"
};

uint8_t get_vxsiic_board_count(const Dev_vxsiicm *d)
{
    uint8_t count = 0;
    for (uint32_t i=0; i<VXSIIC_SLOTS; i++) {
        const vxsiic_slot_status_t *status = &d->priv.status.slot[i];
        if (status->present)
            count++;
    }
    return count;
}

void struct_vxs_i2c_init(Dev_vxsiicm *d)
{
    d->dev.device_status = DEVICE_UNKNOWN;
    for (int i=0; i<VXSIIC_SLOTS; i++) {
        vxsiic_slot_status_t zz = {0};
        d->priv.status.slot[i] = zz;
    }
}
