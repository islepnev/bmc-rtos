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

#include "dev_vxsiicm.h"

#include <stdio.h>
#include <stdint.h>

#include "dev_vxsiic_pp.h"
#include "dev_vxsiicm_types.h"
#include "devices.h"
#include "display.h"

void dev_vxsiicm_print(void)
{
    const DeviceBase *d = find_device_const(DEV_CLASS_VXSIICM);
    if (!d || !d->priv)
        return;
    const Dev_vxsiicm_priv *vxsiicm = (const Dev_vxsiicm_priv *)device_priv_const(d);

    printf("VXS I2C:        %d boards %s", get_vxsiic_board_count(vxsiicm), deviceStatusResultStr(vxsiicm->dev.device_status));
    print_clear_eol();
}
