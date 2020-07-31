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

#include "dev_eeprom_config_fsm.h"
#include "cmsis_os.h"
#include "eeprom_config/dev_eeprom_config.h"
#include "app_shared_data.h"
#include "logbuffer.h"

static const uint32_t ERROR_DELAY_TICKS = 3000;
static const uint32_t POLL_DELAY_TICKS  = 1000;

static uint32_t stateTicks(Dev_eeprom_config *d)
{
    return osKernelSysTick() - d->priv.state_start_tick;
}

void dev_eeprom_config_run(Dev_eeprom_config *d)
{
    const eeprom_config_state_t old_state = d->priv.fsm_state;
    switch (d->priv.fsm_state) {
    case EEPROM_CONFIG_STATE_RESET: {
        DeviceStatus status = dev_eeprom_config_detect(d);
        if (status == DEVICE_NORMAL) {
            d->priv.fsm_state = EEPROM_CONFIG_STATE_RUN;
            log_printf(LOG_INFO, "%s EEPROM Ok", d->dev.name);
            break;
        }
        if (stateTicks(d) > 2000) {
            log_printf(LOG_ERR, "%s EEPROM not found", d->dev.name);
            d->priv.fsm_state = EEPROM_CONFIG_STATE_ERROR;
            break;
        }
        break;
    }
    case EEPROM_CONFIG_STATE_RUN:
        if (DEVICE_NORMAL != dev_eeprom_config_read(d)) {
            d->priv.fsm_state = EEPROM_CONFIG_STATE_ERROR;
            break;
        }
        d->priv.fsm_state = EEPROM_CONFIG_STATE_PAUSE;
        break;
    case EEPROM_CONFIG_STATE_PAUSE:
        if (stateTicks(d) > POLL_DELAY_TICKS) {
            d->priv.fsm_state = EEPROM_CONFIG_STATE_RUN;
        }
        break;
    case EEPROM_CONFIG_STATE_ERROR:
        if (old_state != d->priv.fsm_state) {
            log_printf(LOG_ERR, "%s: EEPROM error", d->dev.name);
        }
        if (stateTicks(d) > ERROR_DELAY_TICKS) {
            d->priv.fsm_state = EEPROM_CONFIG_STATE_RESET;
        }
        break;
    }
    if (old_state != d->priv.fsm_state) {
        d->priv.state_start_tick = osKernelSysTick();
    }
}
