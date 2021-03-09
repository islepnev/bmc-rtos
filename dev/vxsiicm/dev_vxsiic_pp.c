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

#include "cmsis_os.h"
#include "dev_vxsiicm_types.h"
#include "ipmi_sensor_types.h"
#include "ipmi_sensor_util.h"
#include "log/log.h"
#include "vxsiic_hal.h"
#include "fpga/dev_fpga_types.h"
#include "version.h"

bool dev_vxsiic_detect_pp(Dev_vxsiicm *d, int pp)
{
    bool eeprom_ok = vxsiic_detect_pp_eeprom(&d->dev.bus, pp);
    d->priv.status.slot[pp].pp_state.eeprom_found = eeprom_ok;
    bool ioexp_ok = vxsiic_detect_pp_ioexp(&d->dev.bus, pp);
    d->priv.status.slot[pp].pp_state.gpio_found = ioexp_ok;
    return eeprom_ok || ioexp_ok;
}

static bool dev_vxsiic_read_pp_eeprom(Dev_vxsiicm *d, int pp)
{
    uint16_t addr = 0;
    uint8_t eeprom_data = 0;
    bool ok = vxsiic_read_pp_eeprom(&d->dev.bus, pp, addr, &eeprom_data);
    if (ok) {
        // log_printf(LOG_DEBUG, "EEPROM at slot %2s [%04X] = %02X\n", vxsiic_map_slot_to_label[pp], addr, eeprom_data);
        if (eeprom_data != 0xFF) {
            log_printf(LOG_NOTICE, "EEPROM at slot %2s [%04X] = %02X\n", vxsiic_map_slot_to_label[pp], addr, eeprom_data);
        }
    }
    d->priv.status.slot[pp].pp_state.eeprom_found = ok;
    return ok;
}

static bool dev_vxsiic_read_pp_ioexp(Dev_vxsiicm *d, int pp)
{
    uint8_t addr = 0;
    uint8_t data = 0;
    vxsiic_slot_status_t *status = &d->priv.status.slot[pp];
    bool ret = vxsiic_read_pp_ioexp(&d->dev.bus, pp, addr, &data);
    status->pp_state.gpio_found = ret;
//    if (ret) {
//        log_printf(LOG_DEBUG, "IOEXP at slot %2s [%04X] = %02X\n", vxsiic_map_slot_to_label[pp], addr, data);
//    }
    status->ioexp = data;
    return ret;
}

static bool dev_vxsiic_read_pp_mcu_4(Dev_vxsiicm *d, int pp, uint16_t reg, uint32_t *data)
{
    return vxsiic_read_pp_mcu_4(&d->dev.bus, pp, reg, data);
}

static bool dev_vxsiic_write_pp_mcu_4(Dev_vxsiicm *d, int pp, uint16_t reg, uint32_t data)
{
    return vxsiic_write_pp_mcu_4(&d->dev.bus, pp, reg, data);
}

static bool dev_vxsiic_write_pp_mcu(Dev_vxsiicm *d, int pp)
{
    vxsiic_ttvxs_info ttvxs_info;
    ttvxs_info.bmc_ver = make_bmc_ver(VERSION_MAJOR_NUM, VERSION_MINOR_NUM, VERSION_PATCH_NUM);
    ttvxs_info.fpga_fw_version = make_fw_version(get_fpga_fw_ver(), get_fpga_fw_rev());
    ttvxs_info.module_id = get_fpga_id();
    ttvxs_info.module_serial = get_fpga_serial();
    uint32_t uptime = osKernelSysTick() / osKernelSysTickFrequency;
    return true
            && dev_vxsiic_write_pp_mcu_4(d, pp, VXSIIC_REG_TTVXS_BMC_VER, ttvxs_info.bmc_ver.raw)
            && dev_vxsiic_write_pp_mcu_4(d, pp, VXSIIC_REG_TTVXS_MODULE_ID, ttvxs_info.module_id)
            && dev_vxsiic_write_pp_mcu_4(d, pp, VXSIIC_REG_TTVXS_MODULE_SERIAL, ttvxs_info.module_serial)
            && dev_vxsiic_write_pp_mcu_4(d, pp, VXSIIC_REG_TTVXS_FPGA_FW_VER, ttvxs_info.fpga_fw_version.raw)
            && dev_vxsiic_write_pp_mcu_4(d, pp, VXSIIC_REG_TTVXS_UPTIME, uptime)
            ;
}

static bool dev_vxsiic_read_pp_mcu_info(Dev_vxsiicm *d, int pp)
{
    vxsiic_slot_status_t *status = &d->priv.status.slot[pp];
    uint32_t addr = VXSIIC_REG_MAGIC;
    uint32_t data = 0;
    bool ret = dev_vxsiic_read_pp_mcu_4(d, pp, addr, &status->mcu_info.magic);
    status->pp_state.mcu_found = ret;
    if (!ret)
        goto err;
    if (status->mcu_info.magic != BMC_MAGIC)
        goto err;
    status->present = 1;
    enum { MCU_READ_SIZE = VXSIIC_REG_FPGA_FW_VER-VXSIIC_REG_MAGIC+1 };
    static uint32_t map[MCU_READ_SIZE];
    //    map[0] = status->magic;
    for (int i=1; i<MCU_READ_SIZE; i++) {
        addr = i;
        if (! dev_vxsiic_read_pp_mcu_4(d, pp, addr, &data))
            goto err;
        map[i] = data;
    }
    status->mcu_info.bmc_ver.raw = map[VXSIIC_REG_BMC_VER];
    status->mcu_info.module_id = map[VXSIIC_REG_MODULE_ID];
    status->mcu_info.enc_status.w = map[VXSIIC_REG_ENC_STATUS];
    status->mcu_info.iic_stats.ops = map[VXSIIC_REG_IIC_OPS];
    status->mcu_info.iic_stats.errors = map[VXSIIC_REG_IIC_ERRORS];
    status->mcu_info.uptime = map[VXSIIC_REG_UPTIME];
    status->mcu_info.module_serial = map[VXSIIC_REG_MODULE_SERIAL];
    status->mcu_info.fpga_fw_ver.raw = map[VXSIIC_REG_FPGA_FW_VER];
    status->pp_state.mcu_info_ok = true;
    return true;
err:
    status->pp_state.mcu_info_ok = false;
    return false;
}

static bool dev_vxsiic_read_pp_mcu(Dev_vxsiicm *d, int pp)
{
    vxsiic_slot_status_t *status = &d->priv.status.slot[pp];
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
    status->pp_state.mcu_sensors_ok = true;
    return true;
err:
    {
    status->pp_state.mcu_sensors_ok = false;
//        vxsiic_slot_status_t zz = {0};
//        *status = zz;
    }
    return false;
}

bool dev_vxsiic_read_pp(Dev_vxsiicm *d, int pp)
{
    bool eeprom = dev_vxsiic_read_pp_eeprom(d, pp);
    bool ioexp =  dev_vxsiic_read_pp_ioexp(d, pp);
    bool found = (eeprom || ioexp);
    vxsiic_slot_status_t *status = &d->priv.status.slot[pp];
    bool read_mcu = found && dev_vxsiic_read_pp_mcu(d, pp);
    bool write_mcu = read_mcu && dev_vxsiic_write_pp_mcu(d, pp);
    bool ok = eeprom && ioexp && read_mcu && write_mcu;
    if (found) {
        if (ok)
            status->iic_master_stats.ops++;
        else
            status->iic_master_stats.errors++;
    }
    return found;
}
