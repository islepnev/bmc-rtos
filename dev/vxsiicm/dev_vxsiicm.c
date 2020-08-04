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

#include "dev_vxsiicm.h"

#include <string.h>
#include <stdint.h>

#include "cmsis_os.h"
#include "dev_vxsiic_pp.h"
#include "dev_vxsiicm_types.h"
#include "devicelist.h"
#include "ipmi_sensor_types.h"
#include "log/log.h"
#include "vxsiic_hal.h"
#include "vxsiic_iic_driver.h"

// valid slot range: 2..21
// slot 2 = array[0]
static const int map_slot_to_channel[VXSIIC_SLOTS] = {
    3, 2, 1, 1, 0, 5, 7, 6, 5, 2, 3, 4, 4, 7, 6, 0, 1, 0
};

static const int map_slot_to_subdevice[VXSIIC_SLOTS] = {
    1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 2, 2, 1
};

static bool vxsiic_select_pp(Dev_vxsiicm *d, uint8_t pp)
{
    if (pp >= VXSIIC_SLOTS)
        return false;

    vxsiic_reset_mux(&d->dev.bus);

    uint8_t channel = map_slot_to_channel[pp];
    uint8_t subdevice = map_slot_to_subdevice[pp];
    return vxsiic_mux_select(&d->dev.bus, subdevice, channel, true);
    //    bool ret = true;
    //    for (uint8_t i = 0; i < 3; i++) {
    //        ret &= vxsiic_mux_select(&d->dev.bus, i, channel, i == subdevice);
    //    }
    //    return ret;
}

static bool vxsiic_unselect_pp(Dev_vxsiicm *d)
{
    vxsiic_reset_mux(&d->dev.bus);
//    bool ret = true;
//    for (uint8_t subdevice = 0; subdevice < 3; subdevice++) {
//        ret &= vxsiic_mux_select(&d->dev.bus, subdevice, 0, false);
//    }
    return true;
}

DeviceStatus dev_vxsiicm_detect(Dev_vxsiicm *d)
{
    DeviceStatus status = DEVICE_NORMAL;
    if (! vxsiic_detect_mux(&d->dev.bus))
        status = DEVICE_FAIL;
    d->dev.device_status = status;
    return d->dev.device_status;
}

#ifdef MCU_TEST
uint32_t make_test_data(uint32_t i)
{
    return ((i*4+3)<< 24) | ((i*4+2)<< 16) | ((i*4+1)<< 8) | (i*4+0);
}

HAL_StatusTypeDef dev_vxsiic_test_pp_mcu_regs(Dev_vxsiicm *d, int pp)
{
    uint32_t addr = 0;
    uint32_t data = 0;
    int count = 2;
    for (int i=0; i<count; i++) {
        addr = 1+i;
        data = make_test_data(i);
        if (! dev_vxsiic_write_pp_mcu_4(d, pp, addr, data))
            return false;
    }
    for (int i=0; i<count; i++) {
        addr = 1+i;
        if (! dev_vxsiic_read_pp_mcu_4(d, pp, addr, &data))
            return false;
        const uint32_t test_data = make_test_data(i);
        if (test_data != data) {
        }
    }
    return true;
}
#endif

static int old_present[VXSIIC_SLOTS] = {0};

DeviceStatus dev_vxsiicm_read(Dev_vxsiicm *d)
{
//    uint32_t tick_begin = osKernelSysTick();
    for (int pp=0; pp<VXSIIC_SLOTS; pp++) {
        if (! vxsiic_select_pp(d, pp)) {
            d->dev.device_status = DEVICE_FAIL;
            return d->dev.device_status;
        }
        vxsiic_slot_status_t *status = &d->priv.status.slot[pp];
        if (dev_vxsiic_read_pp(d, pp)) {
            status->present = 1;
            if (!old_present[pp])
                log_printf(LOG_NOTICE, "VXS slot %s: board inserted", vxsiic_map_slot_to_label[pp]);
        } else {
            if (old_present[pp])
                log_printf(LOG_NOTICE, "VXS slot %s: board removed", vxsiic_map_slot_to_label[pp]);
            status->present = 0;
        }
        old_present[pp] = status->present;
    }
    vxsiic_unselect_pp(d);
//    uint32_t tick_end = osKernelSysTick();
//    uint32_t ticks = tick_end - tick_begin;
//    debug_printf("%s: %ld ticks\n", __func__, ticks);
    return d->dev.device_status;
}

SensorStatus dev_vxsiicm_sensor_status(void)
{
    const DeviceBase *d = find_device_const(DEV_CLASS_VXSIICM);
    if (!d)
        return SENSOR_UNKNOWN;
    // const Dev_vxsiicm_priv *priv = (const Dev_vxsiicm_priv *)device_priv_const(d);

    if (d->device_status != DEVICE_NORMAL)
        return SENSOR_WARNING;
    return SENSOR_NORMAL;
}
