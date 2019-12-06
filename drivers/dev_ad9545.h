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
#ifndef DEV_AD9545_TYPES_H
#define DEV_AD9545_TYPES_H

#include <stdint.h>
#include "dev_common_types.h"
#include "ad9545/ad9545_setup_regs.h"
#include "ad9545/ad9545_status_regs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    PLL_STATE_INIT,
    PLL_STATE_RESET,
    PLL_STATE_SETUP_SYSCLK,
    PLL_STATE_SYSCLK_WAITLOCK,
    PLL_STATE_SETUP,
    PLL_STATE_RUN,
    PLL_STATE_ERROR,
    PLL_STATE_FATAL
} ad9545_state_t;

typedef struct Dev_ad9545 {
    DeviceStatus present;
    ad9545_setup_t setup;
    AD9545_Status status;
    ad9545_state_t fsm_state;
    uint32_t recoveryCount;
} Dev_ad9545;

SensorStatus get_pll_sensor_status(const Dev_ad9545 *pll);
const char *dev_ad9545_state_str(ad9545_state_t state);
void dev_ad9545_verbose_status(const Dev_ad9545 *d);

#ifdef __cplusplus
}
#endif

#endif // DEV_AD9545_TYPES_H
