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

#include <string.h>

#include "cmsis_os.h"
#include "bus/dev_vxsiics_stats.h"
#include "fpga/dev_fpga_types.h"
#include "ipmi_sensor_util.h"
#include "ipmi_sensors.h"
#include "system_status.h"
#include "version.h"

static const uint32_t BMC_MAGIC = 0x424D4320;
enum {MEM_START_ADDR = 8};
enum {MEM_SIZE = 4};
static uint32_t mem[MEM_SIZE] = {0};

void iic_write_callback(uint16_t addr, uint32_t data)
{
    // writable scratch pad
    if (addr >= MEM_START_ADDR && addr < MEM_START_ADDR + MEM_SIZE) {
        mem[addr - MEM_START_ADDR] = data;
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
    } else if (addr >= MEM_START_ADDR && addr < MEM_START_ADDR + MEM_SIZE) {
        *data = mem[addr - MEM_START_ADDR];
    } else {
        switch (addr) {
        case 0:
            *data = BMC_MAGIC;
            break;
        case 1:
            *data = ((uint32_t)(VERSION_MAJOR_NUM) << 16) | (uint16_t)(VERSION_MINOR_NUM);
            break;
        case 2:
            *data = get_fpga_id();
            break;
        case 3: {
            *data = encode_system_status().w;
            break;
        }
        case 4:
            *data = vxsiic_i2c_stats.ops;
            break;
        case 5:
            *data = vxsiic_i2c_stats.errors;
            break;
        case 6:
            *data = osKernelSysTick() / osKernelSysTickFrequency;
            break;
        default:
            *data = 0;
            break;
        }
    }
}