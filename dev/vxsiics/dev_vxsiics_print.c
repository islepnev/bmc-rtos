/*
**    Copyright 2019-2021 Ilja Slepnev
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

#include <stdint.h>
#include <stdio.h>

#include "dev_vxsiics.h"
#include "dev_vxsiics_types.h"
#include "devicelist.h"
#include "display.h"

void dev_vxsiics_print(void)
{
    const DeviceBase *d = find_device_const(DEV_CLASS_VXSIICS);
    if (!d || !d->priv)
        return;
    const Dev_vxsiics_priv *vxsiics = (const Dev_vxsiics_priv *)device_priv_const(d);

    printf("VXS I2C:        %d boards %s\n", get_vxsiic_board_count(vxsiics),
           sensor_status_ansi_str(dev_vxsiics_sensor_status()));
}
