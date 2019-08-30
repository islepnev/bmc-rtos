/*
**    VXS payload port IIC master functions
**
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
#include "dev_vxsiic_pp.h"

#include <string.h>
#include <stdint.h>
#include "stm32f7xx_hal_def.h"
#include "dev_vxsiic_types.h"
#include "vxsiic_hal.h"
#include "logbuffer.h"
#include "debug_helpers.h"
#include "ipmi_sensor_types.h"
#include "cmsis_os.h"

static void dev_vxsiic_update_pp_state(Dev_vxsiic *d, int pp, HAL_StatusTypeDef ret)
{
    vxsiic_slot_status_t *status = &d->status.slot[pp];
    if (HAL_OK == ret) {
        status->iic_master_stats.ops++;
        status->pp_state = VXSIIC_PP_STATE_READY;
    } else {
        if (status->pp_state == VXSIIC_PP_STATE_READY)
            status->iic_master_stats.errors++;
        status->pp_state = VXSIIC_PP_STATE_ERROR;
    }
}

static HAL_StatusTypeDef dev_vxsiic_read_pp_eeprom(Dev_vxsiic *d, int pp)
{
    HAL_StatusTypeDef ret = HAL_OK;
    uint16_t addr = 0;
    uint8_t eeprom_data = 0;
    ret = vxsiic_read_pp_eeprom(pp, addr, &eeprom_data);
    dev_vxsiic_update_pp_state(d, pp, ret);
    if (HAL_OK == ret) {
        //    debug_printf("EEPROM at slot %2s [%04X] = %02X\n", map_slot_to_label[pp], addr, eeprom_data);
        if (eeprom_data != 0xFF) {
            debug_printf("EEPROM at slot %2s [%04X] = %02X\n", vxsiic_map_slot_to_label[pp], addr, eeprom_data);
            return HAL_ERROR;
        }
    }
    return ret;
}

static HAL_StatusTypeDef dev_vxsiic_read_pp_ioexp(Dev_vxsiic *d, int pp)
{
    HAL_StatusTypeDef ret = HAL_OK;
    uint8_t addr = 0;
    uint8_t data = 0;
    ret = vxsiic_read_pp_ioexp(pp, addr, &data);
    dev_vxsiic_update_pp_state(d, pp, ret);
//    if (HAL_OK == ret) {
//        debug_printf("IOEXP at slot %2s [%04X] = %02X\n", vxsiic_map_slot_to_label[pp], addr, data);
//    }
    vxsiic_slot_status_t *status = &d->status.slot[pp];
    status->ioexp = data;
    return ret;
}

static HAL_StatusTypeDef dev_vxsiic_read_pp_mcu_4(Dev_vxsiic *d, int pp, uint16_t reg, uint32_t *data)
{
    HAL_StatusTypeDef ret = HAL_OK;
    ret = vxsiic_read_pp_mcu_4(pp, reg, data);
    dev_vxsiic_update_pp_state(d, pp, ret);
    return ret;
}

static HAL_StatusTypeDef dev_vxsiic_write_pp_mcu_4(Dev_vxsiic *d, int pp, uint16_t reg, uint32_t data)
{
    HAL_StatusTypeDef ret = HAL_OK;
    ret = vxsiic_write_pp_mcu_4(pp, reg, data);
    dev_vxsiic_update_pp_state(d, pp, ret);
    return ret;
}

static HAL_StatusTypeDef dev_vxsiic_read_pp_mcu(Dev_vxsiic *d, int pp)
{
    HAL_StatusTypeDef ret = HAL_OK;
    vxsiic_slot_status_t *status = &d->status.slot[pp];
    uint32_t addr = 0;
    uint32_t data = 0;
    ret = dev_vxsiic_read_pp_mcu_4(d, pp, addr, &status->magic);
    if (HAL_OK != ret)
        goto err;
    if (status->magic != BMC_MAGIC)
        goto err;
    status->present = 1;
    enum { MCU_READ_SIZE = 7 };
    static uint32_t map[MCU_READ_SIZE];
    //    map[0] = status->magic;
    for (int i=1; i<MCU_READ_SIZE; i++) {
        addr = i;
        ret = dev_vxsiic_read_pp_mcu_4(d, pp, addr, &data);
        if (HAL_OK != ret)
            goto err;
        map[i] = data;
    }
    status->bmc_ver = map[1];
    status->module_id = map[2];
    status->enc_status.w = map[3];
    status->device_status = status->enc_status.b.system;
    status->iic_stats.ops = map[4];
    status->iic_stats.errors = map[5];
    status->uptime = map[6];
    // read ipmi sensors
    ret = dev_vxsiic_read_pp_mcu_4(d, pp, IIC_SENSORS_MAP_START, &data);
    if (HAL_OK != ret)
        goto err;
    status->sensor_count = (data < MAX_SENSOR_COUNT) ? data : MAX_SENSOR_COUNT;
    for (uint32_t i=0; i<status->sensor_count; i++) {
        uint32_t wordcount = sizeof(GenericSensor) / 4;
        static GenericSensor buf;
        for (uint32_t j=0; j<wordcount; j++) {
            uint32_t *ptr = (uint32_t *)&buf + j;
//            uint32_t *ptr = (uint32_t *)(&status->sensors[i]) + j;
            uint16_t addr = 1+IIC_SENSORS_MAP_START + wordcount*i + j;
            ret = dev_vxsiic_read_pp_mcu_4(d, pp, addr, ptr);
            if (HAL_OK != ret)
                goto err;
        }
        buf.name[SENSOR_NAME_SIZE-1] = '\0';
        memcpy(&status->sensors[i], &buf, sizeof(buf));
    }
    return ret;
err:
    {
        vxsiic_slot_status_t zz = {0};
        *status = zz;
    }
    return ret;
}

HAL_StatusTypeDef dev_vxsiic_read_pp(Dev_vxsiic *d, int pp)
{
    HAL_StatusTypeDef ret = HAL_OK;
    ret = vxsiic_get_pp_i2c_status(pp);
    if (HAL_OK != ret)
        return ret;
    ret = dev_vxsiic_read_pp_eeprom(d, pp);
    if (HAL_OK != ret)
        return ret;
    ret = dev_vxsiic_read_pp_ioexp(d, pp);
    if (HAL_OK != ret)
        return ret;
    ret = dev_vxsiic_read_pp_mcu(d, pp);
    return ret;
}
