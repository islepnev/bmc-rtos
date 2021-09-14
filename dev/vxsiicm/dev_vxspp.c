/*
**    VXS Payload Port (IIC Slave)
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

#include "dev_vxspp.h"
#include "dev_vxspp_impl.h"
#include "vxsiic_types.h"
#include "log/log.h"
#include "vxsiic_iic_driver.h"

static int old_present = 0;

DeviceStatus dev_vxspp_run(struct Dev_vxspp *d)
{
    Dev_vxspp_priv *p = &d->priv;
    bool detect_ok = dev_vxspp_detect(d);
    p->bus_ready = vxsiic_bus_ready(&d->dev.bus);
    if (detect_ok && dev_vxspp_read(d)) {
        p->present = 1;
        if (!old_present)
            log_printf(LOG_NOTICE, "%s: board inserted", d->dev.name);
    } else {
        if (old_present)
            log_printf(LOG_NOTICE, "%s: board removed", d->dev.name);
        p->present = 0;
    }
    old_present = p->present;
    return d->dev.device_status;
}
