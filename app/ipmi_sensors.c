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
#include <string.h>
#include "dev_pm_sensors.h"
#include "dev_pm_sensors_config.h"
#include "dev_types.h"
#include "dev_powermon.h"
#include "app_shared_data.h"
#include "system_status.h"

IpmiSensors ipmi_sensors = {0};

void sync_ipmi_sensors(void)
{
    uint32_t index = 0;
    const Devices *dev = getDevices();
    // Power monitors
    const Dev_powermon *pm = &dev->pm;
    for (int i=0; i<POWERMON_SENSORS; i++) {
        const pm_sensor *s = &pm->sensors[i];
        GenericSensor *gs = &ipmi_sensors.sensors[index++];
        gs->hdr.b.type = IPMI_SENSOR_VOLTAGE;
        gs->hdr.b.state = s->sensorStatus;
        gs->hdr.b.optional = s->isOptional;
        gs->value = s->busVoltage;
        strncpy(gs->name, s->label, SENSOR_NAME_SIZE-1);
        if (s->hasShunt) {
            GenericSensor *gs = &ipmi_sensors.sensors[index++];
            gs->hdr.b.type = IPMI_SENSOR_CURRENT;
            gs->hdr.b.state = s->sensorStatus;
            gs->hdr.b.optional = s->isOptional;
            gs->value = s->current;
            strncpy(gs->name, s->label, SENSOR_NAME_SIZE-1);
        }
    }
    // Temperature sensors
    const Dev_thset *thset = &dev->thset;
    {
        GenericSensor *gs = &ipmi_sensors.sensors[index++];
        gs->hdr.b.type = IPMI_SENSOR_TEMPERATURE;
        gs->hdr.b.state = thset->th[0].valid ? SENSOR_NORMAL : SENSOR_UNKNOWN;
        gs->value = thset->th[0].rawTemp / 32.0;
        strncpy(gs->name, "Temp PLL", SENSOR_NAME_SIZE-1);
    }
    {
        GenericSensor *gs = &ipmi_sensors.sensors[index++];
        gs->hdr.b.type = IPMI_SENSOR_TEMPERATURE;
        gs->hdr.b.state = thset->th[1].valid ? SENSOR_NORMAL : SENSOR_UNKNOWN;
        gs->value = thset->th[1].rawTemp / 32.0;
        strncpy(gs->name, "Temp TDC-A", SENSOR_NAME_SIZE-1);
    }
    {
        GenericSensor *gs = &ipmi_sensors.sensors[index++];
        gs->hdr.b.type = IPMI_SENSOR_TEMPERATURE;
        gs->hdr.b.state = thset->th[2].valid ? SENSOR_NORMAL : SENSOR_UNKNOWN;
        gs->value = thset->th[2].rawTemp / 32.0;
        strncpy(gs->name, "Temp TDC-B", SENSOR_NAME_SIZE-1);
    }
    {
        GenericSensor *gs = &ipmi_sensors.sensors[index++];
        gs->hdr.b.type = IPMI_SENSOR_TEMPERATURE;
        gs->hdr.b.state = thset->th[3].valid ? SENSOR_NORMAL : SENSOR_UNKNOWN;
        gs->value = thset->th[3].rawTemp / 32.0;
        strncpy(gs->name, "Temp TDC-C", SENSOR_NAME_SIZE-1);
    }
    // PLL pseudo sensor
    const Dev_ad9545 *pll = &dev->pll;
    {
        GenericSensor *gs = &ipmi_sensors.sensors[index++];
        gs->hdr.b.type = IPMI_SENSOR_DISCRETE;
        gs->hdr.b.state = getPllStatus(pll);
        gs->value = getPllLockState(pll);
        strncpy(gs->name, "PLL", SENSOR_NAME_SIZE-1);
    }
    // FPGA pseudo sensor
    const Dev_fpga *fpga = &dev->fpga;
    {
        GenericSensor *gs = &ipmi_sensors.sensors[index++];
        gs->hdr.b.type = IPMI_SENSOR_DISCRETE;
        gs->hdr.b.state = getFpgaStatus(fpga);
        gs->value = (gs->hdr.b.state == SENSOR_NORMAL);
        strncpy(gs->name, "FPGA", SENSOR_NAME_SIZE-1);
    }
    // update sensor count
    ipmi_sensors.sensor_count = index;
}
