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

#include "ipmi_sensors.h"

#include <assert.h>
#include <string.h>

#include "bsp_sensors_config.h"
#include "powermon/dev_powermon.h"
#include "system_status.h"
#include "system_status_common.h"
#include "thset/dev_thset_types.h"

IpmiSensors ipmi_sensors = {0};

void sync_ipmi_sensors(void)
{
    uint32_t index = 0;
    // Power monitors
    const Dev_powermon_priv *pm = get_powermon_priv_const();
    const pm_sensors_arr *sensors = &pm->sensors;
    for (int i=0; pm && (i<sensors->count); i++) {
        const pm_sensor *p = &sensors->arr[i];
        const DeviceBase *dev = &p->dev;
        const pm_sensor_priv *s = &p->priv;
        GenericSensor *gs = &ipmi_sensors.sensors[index++];
        gs->hdr.b.type = IPMI_SENSOR_VOLTAGE;
        gs->hdr.b.state = dev->sensor;
        gs->hdr.b.optional = s->isOptional;
        gs->value = s->busVoltage;
        strncpy(gs->name, s->label, SENSOR_NAME_SIZE-1);
        if (s->hasShunt) {
            GenericSensor *gs = &ipmi_sensors.sensors[index++];
            gs->hdr.b.type = IPMI_SENSOR_CURRENT;
            gs->hdr.b.state = dev->sensor;
            gs->hdr.b.optional = s->isOptional;
            gs->value = s->current;
            strncpy(gs->name, s->label, SENSOR_NAME_SIZE-1);
        }
    }

    const Dev_thset_priv *thset = get_thset_priv_const();
    if (thset) {
        assert(index + thset->count < MAX_SENSOR_COUNT);
        // Temperature sensors
        for (int i=0; i<thset->count; i++) {
            GenericSensor *gs = &ipmi_sensors.sensors[index++];
            *gs = thset->sensors[i];
        }
    }

    assert(index + 2 < MAX_SENSOR_COUNT);

    // PLL pseudo sensor
    {
        GenericSensor *gs = &ipmi_sensors.sensors[index++];
        gs->hdr.b.type = IPMI_SENSOR_DISCRETE;
        gs->hdr.b.state = getPllStatus();
        gs->value = getPllLockState();
        strncpy(gs->name, "PLL", SENSOR_NAME_SIZE-1);
    }
    // FPGA pseudo sensor
    {
        GenericSensor *gs = &ipmi_sensors.sensors[index++];
        gs->hdr.b.type = IPMI_SENSOR_DISCRETE;
        gs->hdr.b.state = getFpgaStatus();
        gs->value = (gs->hdr.b.state == SENSOR_NORMAL);
        strncpy(gs->name, "FPGA", SENSOR_NAME_SIZE-1);
    }
    // update sensor count
    ipmi_sensors.sensor_count = index;
}
