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
#include "dev_vxsiicm_types.h"
#include "vxsiic_hal.h"
#include "logbuffer.h"
#include "debug_helpers.h"
#include "ipmi_sensor_types.h"
#include "cmsis_os.h"

static bool dev_vxsiic_read_pp_eeprom(Dev_vxsiicm *d, int pp)
{
    uint16_t addr = 0;
    uint8_t eeprom_data = 0;
    bool ok = vxsiic_read_pp_eeprom(pp, addr, &eeprom_data);
    if (ok) {
        // log_printf(LOG_DEBUG, "EEPROM at slot %2s [%04X] = %02X\n", vxsiic_map_slot_to_label[pp], addr, eeprom_data);
        if (eeprom_data != 0xFF) {
            log_printf(LOG_NOTICE, "EEPROM at slot %2s [%04X] = %02X\n", vxsiic_map_slot_to_label[pp], addr, eeprom_data);
        }
    }
    d->status.slot[pp].pp_state.eeprom_found = ok;
    return ok;
}

static bool dev_vxsiic_read_pp_ioexp(Dev_vxsiicm *d, int pp)
{
    uint8_t addr = 0;
    uint8_t data = 0;
    bool ret = vxsiic_read_pp_ioexp(pp, addr, &data);
    d->status.slot[pp].pp_state.gpio_found = ret;
//    if (ret) {
//        log_printf(LOG_DEBUG, "IOEXP at slot %2s [%04X] = %02X\n", vxsiic_map_slot_to_label[pp], addr, data);
//    }
    vxsiic_slot_status_t *status = &d->status.slot[pp];
    status->ioexp = data;
    return ret;
}

static bool dev_vxsiic_read_pp_mcu_4(Dev_vxsiicm *d, int pp, uint16_t reg, uint32_t *data)
{
    return vxsiic_read_pp_mcu_4(pp, reg, data);
}

static bool dev_vxsiic_write_pp_mcu_4(Dev_vxsiicm *d, int pp, uint16_t reg, uint32_t data)
{
    return vxsiic_write_pp_mcu_4(pp, reg, data);
}

static bool dev_vxsiic_read_pp_mcu_info(Dev_vxsiicm *d, int pp)
{
    vxsiic_slot_status_t *status = &d->status.slot[pp];
    uint32_t addr = 0;
    uint32_t data = 0;
    bool ret = dev_vxsiic_read_pp_mcu_4(d, pp, addr, &status->mcu_info.magic);
    d->status.slot[pp].pp_state.mcu_found = ret;
    if (!ret)
        goto err;
    if (status->mcu_info.magic != BMC_MAGIC)
        goto err;
    status->present = 1;
    enum { MCU_READ_SIZE = 7 };
    static uint32_t map[MCU_READ_SIZE];
    //    map[0] = status->magic;
    for (int i=1; i<MCU_READ_SIZE; i++) {
        addr = i;
        if (! dev_vxsiic_read_pp_mcu_4(d, pp, addr, &data))
            goto err;
        map[i] = data;
    }
    status->mcu_info.bmc_ver = map[1];
    status->mcu_info.module_id = map[2];
    status->mcu_info.enc_status.w = map[3];
    status->mcu_info.iic_stats.ops = map[4];
    status->mcu_info.iic_stats.errors = map[5];
    status->mcu_info.uptime = map[6];
    d->status.slot[pp].pp_state.mcu_info_ok = true;
    return true;
err:
    d->status.slot[pp].pp_state.mcu_info_ok = false;
    return false;
}

static bool dev_vxsiic_read_pp_mcu(Dev_vxsiicm *d, int pp)
{
    vxsiic_slot_status_t *status = &d->status.slot[pp];
    uint32_t data = 0;

    if (! dev_vxsiic_read_pp_mcu_info(d, pp))
        goto err;
    status->system_status = status->mcu_info.enc_status.b.system;

    // read ipmi sensors
    if (! dev_vxsiic_read_pp_mcu_4(d, pp, IIC_SENSORS_MAP_START, &data))
        goto err;
    status->mcu_sensors.count = (data < MAX_SENSOR_COUNT) ? data : MAX_SENSOR_COUNT;
    for (uint32_t i=0; i<status->mcu_sensors.count; i++) {
        uint32_t wordcount = sizeof(GenericSensor) / 4;
        static GenericSensor buf;
        for (uint32_t j=0; j<wordcount; j++) {
            uint32_t *ptr = (uint32_t *)&buf + j;
//            uint32_t *ptr = (uint32_t *)(&status->sensors[i]) + j;
            uint16_t addr = 1+IIC_SENSORS_MAP_START + wordcount*i + j;
            if (! dev_vxsiic_read_pp_mcu_4(d, pp, addr, ptr))
                goto err;
        }
        buf.name[SENSOR_NAME_SIZE-1] = '\0';
        memcpy(&status->mcu_sensors.sensors[i], &buf, sizeof(buf));
    }
    d->status.slot[pp].pp_state.mcu_sensors_ok = true;
    return true;
err:
    {
    d->status.slot[pp].pp_state.mcu_sensors_ok = false;
//        vxsiic_slot_status_t zz = {0};
//        *status = zz;
    }
    return false;
}

bool dev_vxsiic_read_pp(Dev_vxsiicm *d, int pp)
{
    if (! vxsiic_get_pp_i2c_status(pp))
        return false;
    bool eeprom = dev_vxsiic_read_pp_eeprom(d, pp);
    bool ioexp =  dev_vxsiic_read_pp_ioexp(d, pp);
    bool found = (eeprom || ioexp);
    vxsiic_slot_status_t *status = &d->status.slot[pp];
    bool mcu = found && dev_vxsiic_read_pp_mcu(d, pp);
    bool ok = eeprom && ioexp && mcu;
    if (found) {
        if (ok)
            status->iic_master_stats.ops++;
        else
            status->iic_master_stats.errors++;
    }
    return found;
}
