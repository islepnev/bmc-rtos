//
//    Copyright 2019 Ilja Slepnev
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include "dev_vxsiic.h"

#include "stm32f7xx_hal_def.h"
#include "dev_vxsiic_types.h"
#include "vxsiic_hal.h"
#include "logbuffer.h"
#include "debug_helpers.h"

// valid slot range: 2..21
// slot 2 = array[0]
static const int map_slot_to_channel[VXSIIC_SLOTS] = {
    3, 2, 1, 1, 0, 5, 7, 6, 5, 2, 3, 4, 4, 7, 6, 0, 1, 0
};

static const int map_slot_to_subdevice[VXSIIC_SLOTS] = {
    1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 2, 2, 1
};

DeviceStatus dev_vxsiic_detect(Dev_vxsiic *d)
{
    DeviceStatus status = DEVICE_NORMAL;
    if (HAL_OK != vxsiic_detect_mux())
        status = DEVICE_FAIL;
    d->present = status;
    return d->present;
}

uint32_t make_test_data(uint32_t i)
{
    return ((i*4+3)<< 24) | ((i*4+2)<< 16) | ((i*4+1)<< 8) | (i*4+0);
}

DeviceStatus vxsiic_select_pp(Dev_vxsiic *d, uint8_t pp)
{
    HAL_StatusTypeDef ret = HAL_ERROR;
    if (pp >= VXSIIC_SLOTS)
        return ret;
    uint8_t channel = map_slot_to_channel[pp];
    uint8_t subdevice = map_slot_to_subdevice[pp];
    ret = vxsiic_mux_select(subdevice, channel);
    return ret;
}

HAL_StatusTypeDef dev_vxsiic_read_pp_eeprom(Dev_vxsiic *d, int pp)
{
    HAL_StatusTypeDef ret = HAL_OK;
    uint16_t addr = 0;
    uint8_t eeprom_data = 0;
    ret = vxsiic_read_pp_eeprom(pp, addr, &eeprom_data);
    if (HAL_OK != ret)
        return ret;
//    debug_printf("EEPROM at slot %2s [%04X] = %02X\n", map_slot_to_label[pp], addr, eeprom_data);
    if (eeprom_data != 0xFF) {
        debug_printf("EEPROM at slot %2s [%04X] = %02X\n", vxsiic_map_slot_to_label[pp], addr, eeprom_data);
        return HAL_ERROR;
    }
    return ret;
}

HAL_StatusTypeDef dev_vxsiic_test_pp_mcu_regs(Dev_vxsiic *d, int pp)
{
    HAL_StatusTypeDef ret = HAL_OK;
    uint32_t addr = 0;
    uint32_t data = 0;
    int count = 2;
    for (int i=0; i<count; i++) {
        addr = 1+i;
        data = make_test_data(i);
        ret = vxsiic_write_pp_mcu_4(pp, addr, data);
        if (HAL_OK != ret)
            goto err;
    }
    for (int i=0; i<count; i++) {
        addr = 1+i;
        ret = vxsiic_read_pp_mcu_4(pp, addr, &data);
        if (HAL_OK != ret)
            goto err;
        const uint32_t test_data = make_test_data(i);
        if (test_data != data) {
        }
    }
    return ret;
err:
    return ret;
}

HAL_StatusTypeDef dev_vxsiic_read_pp_mcu(Dev_vxsiic *d, int pp)
{
    HAL_StatusTypeDef ret = HAL_OK;
    vxsiic_slot_status_t *status = &d->status.slot[pp];
    uint32_t addr = 0;
    uint32_t data = 0;
    ret = vxsiic_read_pp_mcu_4(pp, addr, &status->magic);
    if (HAL_OK != ret)
        goto err;
    status->present = 1;
    for (int i=0; i<MCU_MAP_SIZE; i++) {
        addr = 1+i;
        ret = vxsiic_read_pp_mcu_4(pp, addr, &data);
        if (HAL_OK != ret)
            goto err;
        status->map[i] = data;
    }
    status->module_id = status->map[1];
    status->device_status = status->map[2];
    return ret;
err:
    {
        vxsiic_slot_status_t zz = {0};
        *status = zz;
    }
    return ret;
}

static int old_present[VXSIIC_SLOTS] = {0};

DeviceStatus dev_vxsiic_read(Dev_vxsiic *d)
{
    HAL_StatusTypeDef ret = HAL_OK;
    for (int pp=0; pp<VXSIIC_SLOTS; pp++) {
        vxsiic_reset_i2c_master();
        vxsiic_reset_mux();
        ret = vxsiic_select_pp(d, pp);
        if (HAL_OK != ret) {
            d->present = DEVICE_FAIL;
            return d->present;
        }
        vxsiic_slot_status_t *status = &d->status.slot[pp];
        if ((HAL_OK == vxsiic_get_pp_i2c_status(pp))
                && (HAL_OK == dev_vxsiic_read_pp_eeprom(d, pp))
                && (HAL_OK == dev_vxsiic_read_pp_mcu(d, pp))) {
            status->present = 1;
            if (!old_present[pp])
                log_printf(LOG_NOTICE, "VXS slot %s: board inserted", vxsiic_map_slot_to_label[pp]);
        } else {
            if (old_present[pp])
                log_printf(LOG_NOTICE, "VXS slot %s: board removed", vxsiic_map_slot_to_label[pp]);
            status->present = 0;
        }
        old_present[pp] = status->present;
    }
    return d->present;
}
