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

#include "dev_sfpiic.h"

#include "dev_sfpiic_ch.h"
#include "dev_sfpiic_driver.h"
#include "dev_sfpiic_types.h"
#include "devicelist.h"
#include "log/log.h"

static bool dev_sfpiic_select_ch(uint8_t ch)
{
    if (ch >= SFPIIC_CH_CNT)
        return false;

    sfpiic_switch_reset();

    return sfpiic_switch_set_channel(ch);
}

void dev_sfpiic_init(struct Dev_sfpiic *d)
{
    sfpiic_stats_t zz = {0};
    for (int i=0; i<SFPIIC_CH_CNT; i++) {
        d->priv.status.sfp[i].iic_stats = zz;
    }
}
DeviceStatus dev_sfpiic_detect(Dev_sfpiic *d)
{
    DeviceStatus status = DEVICE_NORMAL;
    sfpiic_switch_reset();
    if (! sfpiic_device_detect(PCA9548_BASE_I2C_ADDRESS))
        status = DEVICE_FAIL;
    d->dev.device_status = status;
    return d->dev.device_status;
}

static int sfp_old_present[SFPIIC_CH_CNT] = {0};

DeviceStatus dev_sfpiic_update(Dev_sfpiic *d)
{
    for(int ch=0; ch<SFPIIC_CH_CNT; ++ch) {
        if (! dev_sfpiic_select_ch(ch)) {
            d->dev.device_status = DEVICE_FAIL;
            return d->dev.device_status;
        }
        sfpiic_ch_status_t *status = &d->priv.status.sfp[ch];
        if (dev_sfpiic_ch_update(d, ch)) {
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
    return d->dev.device_status;
}

const Dev_sfpiic_priv *get_sfpiic_priv_const(void)
{
    const DeviceBase *d = find_device_const(DEV_CLASS_SFPIIC);
    if (!d || !d->priv)
        return 0;
    return (const Dev_sfpiic_priv *)device_priv_const(d);
}

SensorStatus get_sfpiic_sensor_status(void)
{
    const Dev_sfpiic_priv *priv = get_sfpiic_priv_const();
    if (!priv)
        return SENSOR_UNKNOWN;
    SensorStatus status = SENSOR_NORMAL;
    for (int i=0; i<SFPIIC_CH_CNT; i++) {
        const SensorStatus sfp_status = priv->status.sfp[i].system_status;
        if (sfp_status > status)
            status = sfp_status;
    }
    return status;
}

