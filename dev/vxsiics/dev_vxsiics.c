/*
**    VXS IIC Slave
**
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

#include "dev_vxsiics.h"

#include <stdlib.h>
#include <string.h>

#include "bus/dev_vxsiics_stats.h"
#include "cmsis_os.h"
#include "dev_vxsiics_types.h"
#include "fpga/dev_fpga_types.h"
#include "ipmi_sensor_util.h"
#include "ipmi_sensors.h"
#include "log/log.h"
#include "system_status.h"
#include "version.h"
#include "vxsiic_types.h"

void iic_write_callback(uint16_t addr, uint32_t data)
{
    // writable scratch pad
    if (addr >= VXSIIC_SCRATCH_MEM_START_ADDR && addr < VXSIIC_SCRATCH_MEM_START_ADDR + VXSIIC_SCRATCH_MEM_SIZE) {
        vxsiics_scratch_mem[addr - VXSIIC_SCRATCH_MEM_START_ADDR] = data;
    }
}

void iic_read_callback(uint16_t addr, uint32_t *data)
{
    if (!data)
        return;
    if (addr >= IIC_SENSORS_MAP_START && addr < IIC_SENSORS_MAP_START + IIC_SENSORS_MAP_SIZE_BYTES / 4) {
        uint32_t offset = addr - IIC_SENSORS_MAP_START;
        uint32_t *ptr = (uint32_t *)&ipmi_sensors + offset;
        memcpy(data, ptr, sizeof(uint32_t));
    } else if (addr >= VXSIIC_SCRATCH_MEM_START_ADDR && addr < VXSIIC_SCRATCH_MEM_START_ADDR + VXSIIC_SCRATCH_MEM_SIZE) {
        *data = vxsiics_scratch_mem[addr - VXSIIC_SCRATCH_MEM_START_ADDR];
    } else {
        switch (addr) {
        case VXSIIC_REG_MAGIC:
            *data = BMC_MAGIC;
            break;
        case VXSIIC_REG_BMC_VER:
            *data = make_bmc_ver(VERSION_MAJOR_NUM, VERSION_MINOR_NUM, VERSION_PATCH_NUM).raw;
            break;
        case VXSIIC_REG_MODULE_ID:
            *data = get_fpga_id();
            break;
        case VXSIIC_REG_ENC_STATUS: {
            *data = encode_system_status().w;
            break;
        }
        case VXSIIC_REG_IIC_OPS:
            *data = vxsiic_i2c_stats.ops;
            break;
        case VXSIIC_REG_IIC_ERRORS:
            *data = vxsiic_i2c_stats.errors;
            break;
        case VXSIIC_REG_UPTIME:
            *data = osKernelSysTick() / osKernelSysTickFrequency;
            break;
        case VXSIIC_REG_MODULE_SERIAL:
            *data = get_fpga_serial();
            break;
        case VXSIIC_REG_FPGA_FW_VER:
            *data = make_fw_version(get_fpga_fw_ver(), get_fpga_fw_rev()).raw;
            break;
        default:
            *data = 0;
            break;
        }
    }
}
static uint32_t current_timestamp(void)
{
    return osKernelSysTick() / osKernelSysTickFrequency;
}

void dev_vxsiics_poll_status(Dev_vxsiics *d)
{
    static vxsiic_ttvxs_info save_ttvxs_info = {0};
    static uint32_t save_ttvxs_uptime = {0};
    d->priv.ttvxs_info.bmc_ver.raw = vxsiics_scratch_mem[VXSIIC_REG_TTVXS_BMC_VER - VXSIIC_SCRATCH_MEM_START_ADDR];
    d->priv.ttvxs_info.module_id = vxsiics_scratch_mem[VXSIIC_REG_TTVXS_MODULE_ID - VXSIIC_SCRATCH_MEM_START_ADDR];
    d->priv.ttvxs_info.module_serial = vxsiics_scratch_mem[VXSIIC_REG_TTVXS_MODULE_SERIAL - VXSIIC_SCRATCH_MEM_START_ADDR];
    d->priv.ttvxs_info.fpga_fw_version.raw = vxsiics_scratch_mem[VXSIIC_REG_TTVXS_FPGA_FW_VER - VXSIIC_SCRATCH_MEM_START_ADDR];
    d->priv.ttvxs_uptime = vxsiics_scratch_mem[VXSIIC_REG_TTVXS_UPTIME - VXSIIC_SCRATCH_MEM_START_ADDR];
    bool modified = memcmp(&save_ttvxs_info, &d->priv.ttvxs_info, sizeof(save_ttvxs_info));
    bool uptime_modified = (save_ttvxs_uptime != d->priv.ttvxs_uptime);
    const uint32_t now = current_timestamp();
    bool ttvxs_bmc_restart_flag = 0;
    int32_t delta_uptime = d->priv.ttvxs_uptime - save_ttvxs_uptime;
    if (uptime_modified) {
        d->priv.ttvxs_uptime_timestamp = now;
        if (d->priv.ttvxs_uptime < 2 && abs(delta_uptime) > 2) {
            ttvxs_bmc_restart_flag = 1;
        }
        //log_printf(LOG_INFO, "TTVXS: uptime %08X", d->priv.ttvxs_uptime);
        save_ttvxs_uptime = d->priv.ttvxs_uptime;
    }
    static bool ttvxs_update_state = 0;
    if (now - d->priv.ttvxs_uptime_timestamp > 2) {
        if (ttvxs_update_state)
            log_printf(LOG_INFO, "TTVXS: update stopped at uptime %d", d->priv.ttvxs_uptime);
        ttvxs_update_state = 0;
    } else {
        if (!ttvxs_update_state) {
            if (d->priv.ttvxs_uptime < 2)
                ttvxs_bmc_restart_flag = 1;
            else
                log_printf(LOG_INFO, "TTVXS: update resumed, uptime %d", d->priv.ttvxs_uptime);
        }
        ttvxs_update_state = 1;
    }
    if (ttvxs_bmc_restart_flag)
        log_printf(LOG_INFO, "TTVXS: BMC restart");

    if (modified) {
        d->priv.ttvxs_info_timestamp = now;
        log_printf(LOG_INFO, "TTVXS: BMC %d.%d.%d, FPGA %02X %04X-%04X v%d.%d.%d",
                   d->priv.ttvxs_info.bmc_ver.b.major,
                   d->priv.ttvxs_info.bmc_ver.b.minor,
                   d->priv.ttvxs_info.bmc_ver.b.patch,
                   d->priv.ttvxs_info.module_id,
                   d->priv.ttvxs_info.module_serial >> 16,
                   d->priv.ttvxs_info.module_serial & 0xFFFF,
                   d->priv.ttvxs_info.fpga_fw_version.b.major,
                   d->priv.ttvxs_info.fpga_fw_version.b.minor,
                   d->priv.ttvxs_info.fpga_fw_version.b.patch
                   );
        save_ttvxs_info = d->priv.ttvxs_info;
    }

//    static int32_t save_delta_uptime = 0;
//    uint32_t local_uptime = current_timestamp();
//    int32_t delta_uptime = d->priv.ttvxs_uptime - local_uptime;
//    if (delta_uptime - save_delta_uptime <= 2) {
//        log_printf(LOG_INFO, "TTVXS: time drifted by %d", d->priv.ttvxs_uptime);
//        save_delta_uptime = delta_uptime;
//    }
//    if (delta_uptime - save_delta_uptime >= 2) {
//        log_printf(LOG_INFO, "TTVXS: time drifted by %d", d->priv.ttvxs_uptime);
//        save_delta_uptime = delta_uptime;
//    }
}
