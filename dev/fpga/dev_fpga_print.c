/*
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

#include "dev_fpga_print.h"

#include <stdio.h>

#include "ansi_escape_codes.h"
#include "display.h"
#include "devicelist.h"

void dev_fpga_print_box(void)
{
    const DeviceBase *d = find_device_const(DEV_CLASS_FPGA);
    if (!d || !d->priv)
        return;
    const Dev_fpga_priv *priv = (const Dev_fpga_priv *)device_priv_const(d);

    printf("FPGA %s",
           priv->initb ? "" : ANSI_RED "INIT " ANSI_CLEAR);
    if (priv->initb && !priv->done)
        printf(ANSI_YELLOW "loading" ANSI_CLEAR);
    if (priv->done)
        printf("%04X", priv->id);
    printf("%s", sensor_status_ansi_str(get_fpga_sensor_status()));
    print_clear_eol();
}
