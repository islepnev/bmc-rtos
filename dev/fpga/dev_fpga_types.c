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

#include "dev_fpga_types.h"
#include "devicelist.h"
#include <stddef.h>
#include <string.h>

void clear_fpga_runtime_info(void)
{
    DeviceBase *d = find_device(DEV_CLASS_FPGA);
    if (!d)
        return;
    Dev_fpga_priv *priv = (Dev_fpga_priv *)device_priv(d);
    Dev_fpga_runtime *fpga = &priv->fpga;
    memset(fpga, 0, sizeof(Dev_fpga_runtime));
}

const Dev_fpga_runtime *get_fpga_runtime_const()
{
    const DeviceBase *d = find_device_const(DEV_CLASS_FPGA);
    if (!d)
        return NULL;
    const Dev_fpga_priv *priv = (const Dev_fpga_priv *)device_priv_const(d);
    return &priv->fpga;
}

SensorStatus get_fpga_sensor_status(void)
{
    const DeviceBase *d = find_device_const(DEV_CLASS_FPGA);
    if (!d)
        return SENSOR_UNKNOWN;
    const Dev_fpga_priv *priv = (const Dev_fpga_priv *)device_priv_const(d);
    if (!priv)
        return SENSOR_UNKNOWN;
    const Dev_fpga_runtime *fpga = &priv->fpga;
    const Dev_fpga_gpio *gpio = &priv->gpio;

    if (d->device_status != DEVICE_NORMAL)
      return SENSOR_WARNING;
   if (!gpio->initb)
      return SENSOR_CRITICAL;
   if (!gpio->done)
      return SENSOR_CRITICAL;
   if (fpga->csr.id == 0 || fpga->csr.id == 0xFFFFu)
      return SENSOR_WARNING;
   return SENSOR_NORMAL;
}

uint32_t get_fpga_id(void)
{
    const Dev_fpga_runtime *fpga = get_fpga_runtime_const();
    if (!fpga)
        return 0;
    return fpga->csr.id;
}

uint64_t get_fpga_ow_id(void)
{
    const Dev_fpga_runtime *fpga = get_fpga_runtime_const();
    if (!fpga)
        return 0;
    return fpga->csr.ow_id;
}

uint32_t get_fpga_serial(void)
{
    return get_fpga_ow_id() >> 8;
}

uint32_t get_fpga_fw_ver(void)
{
    const Dev_fpga_runtime *fpga = get_fpga_runtime_const();
    if (!fpga)
        return 0;
    return fpga->csr.fw_ver;
}

uint32_t get_fpga_fw_rev(void)
{
    const Dev_fpga_runtime *fpga = get_fpga_runtime_const();
    if (!fpga)
        return 0;
    return fpga->csr.fw_rev;
}
