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

#include "dev_vxsiic.h"
#include <string.h>
#include <stdint.h>
#include "stm32f7xx_hal_def.h"
#include "dev_vxsiic_types.h"
#include "dev_vxsiic_pp.h"
#include "vxsiic_iic_driver.h"
#include "vxsiic_hal.h"
#include "logbuffer.h"
#include "debug_helpers.h"
#include "ipmi_sensor_types.h"
#include "cmsis_os.h"

// valid slot range: 2..21
// slot 2 = array[0]
static const int map_slot_to_channel[VXSIIC_SLOTS] = {
    3, 2, 1, 1, 0, 5, 7, 6, 5, 2, 3, 4, 4, 7, 6, 0, 1, 0
};

static const int map_slot_to_subdevice[VXSIIC_SLOTS] = {
    1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 2, 2, 1
};

static HAL_StatusTypeDef vxsiic_select_pp(Dev_vxsiic *d, uint8_t pp)
{
    HAL_StatusTypeDef ret = HAL_ERROR;
    if (pp >= VXSIIC_SLOTS)
        return ret;

    vxsiic_reset_i2c_master();
    vxsiic_reset_mux();

    uint8_t channel = map_slot_to_channel[pp];
    uint8_t subdevice = map_slot_to_subdevice[pp];
    ret = vxsiic_mux_select(subdevice, channel);
    return ret;
}

void dev_vxsiic_init(Dev_vxsiic *d)
{
    vxsiic_init();
    iic_stats_t zz = {0};
    for (int i=0; i<VXSIIC_SLOTS; i++) {
        d->status.slot[i].iic_stats = zz;
    }
}

DeviceStatus dev_vxsiic_detect(Dev_vxsiic *d)
{
    DeviceStatus status = DEVICE_NORMAL;
    if (HAL_OK != vxsiic_detect_mux())
        status = DEVICE_FAIL;
    d->present = status;
    return d->present;
}

#ifdef MCU_TEST
uint32_t make_test_data(uint32_t i)
{
    return ((i*4+3)<< 24) | ((i*4+2)<< 16) | ((i*4+1)<< 8) | (i*4+0);
}

HAL_StatusTypeDef dev_vxsiic_test_pp_mcu_regs(Dev_vxsiic *d, int pp)
{
    HAL_StatusTypeDef ret = HAL_OK;
    uint32_t addr = 0;
    uint32_t data = 0;
    int count = 2;
    for (int i=0; i<count; i++) {
        addr = 1+i;
        data = make_test_data(i);
        ret = dev_vxsiic_write_pp_mcu_4(d, pp, addr, data);
        if (HAL_OK != ret)
            goto err;
    }
    for (int i=0; i<count; i++) {
        addr = 1+i;
        ret = dev_vxsiic_read_pp_mcu_4(d, pp, addr, &data);
        if (HAL_OK != ret)
            goto err;
        const uint32_t test_data = make_test_data(i);
        if (test_data != data) {
        }
    }
    return ret;
err:
    return ret;
}
#endif

static int old_present[VXSIIC_SLOTS] = {0};

DeviceStatus dev_vxsiic_read(Dev_vxsiic *d)
{
//    uint32_t tick_begin = osKernelSysTick();
    HAL_StatusTypeDef ret = HAL_OK;
    for (int pp=0; pp<VXSIIC_SLOTS; pp++) {
        ret = vxsiic_select_pp(d, pp);
        if (HAL_OK != ret) {
            d->present = DEVICE_FAIL;
            return d->present;
        }
        vxsiic_slot_status_t *status = &d->status.slot[pp];
        if (HAL_OK == dev_vxsiic_read_pp(d, pp)) {
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
//    uint32_t tick_end = osKernelSysTick();
//    uint32_t ticks = tick_end - tick_begin;
//    debug_printf("%s: %ld ticks\n", __func__, ticks);
    return d->present;
}
