//
//    Copyright 2019 Ilja Slepnev
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include "dev_sfpiic.h"
#include "dev_sfpiic_ch.h"
#include "dev_sfpiic_driver.h"
#include "dev_sfpiic_types.h"
#include "logbuffer.h"

static HAL_StatusTypeDef dev_sfpiic_select_ch(uint8_t ch)
{
    if (ch >= SFPIIC_CH_CNT)
        return HAL_ERROR;

    sfpiic_master_reset();
    sfpiic_switch_reset();

    HAL_StatusTypeDef ret = sfpiic_switch_set_channel(ch);
    return ret;
}

void dev_sfpiic_init(struct Dev_sfpiic *d)
{
    sfpiic_init();
    sfpiic_stats_t zz = {0};
    for (int i=0; i<SFPIIC_CH_CNT; i++) {
        d->status.ch[i].iic_stats = zz;
    }
}
DeviceStatus dev_sfpiic_detect(Dev_sfpiic *d)
{
    DeviceStatus status = DEVICE_NORMAL;
    sfpiic_master_reset();
    sfpiic_switch_reset();
    if (HAL_OK != sfpiic_switch_detect())
        status = DEVICE_FAIL;
    d->present = status;
    return d->present;
}

static int sfp_old_present[SFPIIC_CH_CNT] = {0};

DeviceStatus dev_sfpiic_update(Dev_sfpiic *d)
{
    HAL_StatusTypeDef ret = HAL_OK;
//    for(uint8_t ch=0; ch<3; ++ch) {
//        // SFP channals
//    }

    for(uint8_t ch=3; ch<SFPIIC_CH_CNT; ++ch) {
        // QSFP channals
        ret = dev_sfpiic_select_ch(ch);
        if (HAL_OK != ret) {
            d->present = DEVICE_FAIL;
            return d->present;
        }
        sfpiic_ch_status_t *status = &d->status.ch[ch];
        if (HAL_OK == dev_sfpiic_ch_update(d, ch)) {
            status->present = 1;
            if (!sfp_old_present[ch])
                log_printf(LOG_NOTICE, "SFP ch #%d: inserted", ch);
        } else {
            if (sfp_old_present[ch])
                log_printf(LOG_NOTICE, "SFP ch #%d: removed", ch);
            status->present = 0;
        }
        sfp_old_present[ch] = status->present;
    }
    return d->present;
}

