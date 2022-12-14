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

#ifndef DEV_AD9545_H
#define DEV_AD9545_H

#include <stdint.h>

#include "bus/bus_types.h"
#include "dev_ad9545_fsm.h"
#include "devicebase.h"
#include "ad9545/ad9545_setup_regs.h"
#include "ad9545/ad9545_status_regs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    AD9545_STATE_INIT,
    AD9545_STATE_RESET,
    AD9545_STATE_SETUP_SYSCLK,
    AD9545_STATE_SYSCLK_WAITLOCK,
    AD9545_STATE_SETUP,
    AD9545_STATE_RUN,
    AD9545_STATE_ERROR,
    AD9545_STATE_FATAL
} ad9545_state_t;

typedef struct Dev_ad9545_priv {
    ad9545_setup_t setup;
    AD9545_Status status;
    ad9545_state_t fsm_state;
    uint32_t stateStartTick;
    uint32_t recoveryCount;
} Dev_ad9545_priv;

typedef struct Dev_ad9545 {
    DeviceBase dev;
    Dev_ad9545_priv priv;
} Dev_ad9545;

void ad9545_update_pll_sensor_status(Dev_ad9545 *pll);
void pll_ad9545_clear_status(AD9545_Status *status);
bool ad9545_running(Dev_ad9545 *d);

#ifdef __cplusplus
}
#endif

#endif // DEV_AD9545_H
