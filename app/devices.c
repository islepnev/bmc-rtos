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

#include "adt7301_spi_hal.h"
#include "dev_sfpiic.h"
//#include "dev_vxsiic.h"
//#include "dev_pll.h"
#include "devices_types.h"
#include "fpga_spi_hal.h"
#include "dev_eeprom.h"
#include "dev_powermon.h"
#include "dev_thset.h"
#include "i2c.h"
#include "display.h"
#include "dev_leds_types.h"

DeviceStatus getDeviceStatus(const Devices *d)
{
    DeviceStatus status = DEVICE_FAIL;
    if ((d->sfpiic.present == DEVICE_NORMAL)
            && (d->vxsiic.present == DEVICE_NORMAL)
//            && (d->eeprom_config.present == DEVICE_NORMAL)
//            && (d->pll.present == DEVICE_NORMAL)
            && (d->fpga.present == DEVICE_NORMAL)
            )
        status = DEVICE_NORMAL;
    return status;
}

PgoodState dev_readPgood(Dev_powermon *pm)
{
    pm_read_pgood(pm);
    return get_all_pgood(pm);
}

void dev_thset_read(Dev_thset *d)
{
    for(int i=0; i<DEV_THERM_COUNT; i++) {
        int16_t rawTemp;
        HAL_StatusTypeDef ret = adt7301_read_temp(i, &rawTemp);
        d->th[i].valid = (ret == HAL_OK);
        d->th[i].rawTemp = rawTemp;
    }
}
