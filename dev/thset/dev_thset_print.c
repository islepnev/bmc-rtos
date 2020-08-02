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

#include "dev_thset_types.h"

#include <stdio.h>

#include "devicelist.h"
#include "display.h"
#include "thset/dev_thset.h"
#include "thset/dev_thset_types.h"

void print_thset_box(void)
{
    const Dev_thset_priv *p = get_thset_priv_const();
    if (!p)
        return;
    printf("Temp: ");
    for (int i=0; i<p->count; i++) {
        const GenericSensor *sensor = &p->sensors[i];
        SensorStatus status = sensor->hdr.b.state;
        const char *prefix = "";
        const char *suffix = "";
        switch (status) {
        case SENSOR_UNKNOWN:
            prefix = ANSI_BGR_PUR ANSI_GRAY;
            suffix = ANSI_CLEAR;
            break;
        case SENSOR_NORMAL:
            prefix = ANSI_CLEAR;
            suffix = "";
            break;
        case SENSOR_WARNING:
            prefix = ANSI_BGR_YELLOW ANSI_GRAY;
            suffix = ANSI_CLEAR;
            break;
        case SENSOR_CRITICAL:
            prefix = ANSI_BGR_RED ANSI_GRAY;
            suffix = ANSI_CLEAR;
            break;
        }
        if (status != SENSOR_UNKNOWN)
            printf("%s%s%s %.1f%s", i ? ", " : "", prefix, sensor->name, sensor->value, suffix);
        else
            printf("%s%s%s ---%s", i ? ", " : "", prefix, sensor->name, suffix);
    }
    const SensorStatus status = dev_thset_thermStatus();
    printf("%s", sensor_status_ansi_str(status));
    print_clear_eol();
}
