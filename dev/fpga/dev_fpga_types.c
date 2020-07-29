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

SensorStatus get_fpga_sensor_status(void)
{
    const DeviceBase *d = find_device_const(DEV_CLASS_FPGA);
    if (!d)
        return SENSOR_UNKNOWN;
    const Dev_fpga_priv *priv = (const Dev_fpga_priv *)device_priv_const(d);

   if (d->device_status != DEVICE_NORMAL)
      return SENSOR_WARNING;
   if (!priv->initb)
      return SENSOR_CRITICAL;
   if (!priv->done)
      return SENSOR_CRITICAL;
   if (priv->id == 0 || priv->id == 0xFFFFu)
      return SENSOR_WARNING;
   return SENSOR_NORMAL;
}

uint32_t get_fpga_id(void)
{
    const DeviceBase *d = find_device_const(DEV_CLASS_FPGA);
    if (!d)
        return 0;
    const Dev_fpga_priv *priv = (const Dev_fpga_priv *)device_priv_const(d);
    return priv->id;
}

uint32_t get_fpga_fw_ver(void)
{
    const DeviceBase *d = find_device_const(DEV_CLASS_FPGA);
    if (!d)
        return 0;
    const Dev_fpga_priv *priv = (const Dev_fpga_priv *)device_priv_const(d);
    return priv->fw_ver;
}

uint32_t get_fpga_fw_rev(void)
{
    const DeviceBase *d = find_device_const(DEV_CLASS_FPGA);
    if (!d)
        return 0;
    const Dev_fpga_priv *priv = (const Dev_fpga_priv *)device_priv_const(d);
    return priv->fw_rev;
}
