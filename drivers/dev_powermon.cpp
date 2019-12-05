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

#include "dev_powermon.h"

#include "stm32f7xx_hal_gpio.h"
#include "stm32f7xx_hal_dma.h"
#include "stm32f7xx_hal_i2c.h"
#include "ansi_escape_codes.h"
#include "app_shared_data.h"
#include "display.h"
#include "logbuffer.h"
#include "dev_pm_sensors.h"
#include "dev_pm_sensors_types.h"
#include "bsp.h"
#include "gpio.h"
#include "bsp_pin_defs.h"
#include "devices_types.h"
#include "cmsis_os.h"

static const uint32_t DETECT_TIMEOUT_TICKS = 1000;

void struct_powermon_sensors_init(Dev_powermon *d)
{
    for (int i=0; i<POWERMON_SENSORS; i++) {
        struct_pm_sensor_init(&d->sensors[i], (SensorIndex)i);
    }
}

void struct_powermon_init(Dev_powermon *d)
{
    d->monState = MON_STATE_INIT;
    d->stateStartTick = 0;
    d->monErrors = 0;
    d->monCycle = 0;
    struct_powermon_sensors_init(d);
//    d->present = DEVICE_UNKNOWN;
    d->vmePresent = 0;
    d->pgood_1v0_core = 0;
    d->pgood_1v0_mgt = 0;
    d->pgood_1v2_mgt = 0;
    d->pgood_2v5 = 0;
    d->pgood_3v3 = 0;
    d->pgood_3v3_fmc = 0;
    d->sw.switch_5v = 1;
    d->sw.switch_3v3 = 1;
    d->sw.switch_2v5 = 1;
    d->sw.switch_1v0_core = 1;
    d->sw.switch_1v0_mgt = 1;
    d->sw.switch_1v2_mgt = 1;
    d->sw.switch_5v_fmc = 1;
}

static int read_pgood_1v0_core(void)
{
    return read_gpio_pin(PGOOD_1V0_CORE_GPIO_Port, PGOOD_1V0_CORE_Pin);
}

static int read_pgood_1v0_mgt(void)
{
    return read_gpio_pin(PGOOD_1V0_MGT_GPIO_Port, PGOOD_1V0_MGT_Pin);
}

static int read_pgood_1v2_mgt(void)
{
    return read_gpio_pin(PGOOD_1V2_MGT_GPIO_Port, PGOOD_1V2_MGT_Pin);
}

static int read_pgood_2v5(void)
{
    return read_gpio_pin(PGOOD_2V5_GPIO_Port, PGOOD_2V5_Pin);
}

static int read_pgood_3v3(void)
{
    return read_gpio_pin(PGOOD_3V3_GPIO_Port, PGOOD_3V3_Pin);
}

static int read_pgood_3v3_fmc(void)
{
    return read_gpio_pin(PGOOD_FMC_3P3VAUX_GPIO_Port, PGOOD_FMC_3P3VAUX_Pin);
}

static int readLiveInsertPin(void)
{
    GPIO_PinState state;
    state = GPIO_PIN_SET; // TODO: HAL_GPIO_ReadPin(VME_DET_B_GPIO_Port, VME_DET_B_Pin);
    return (GPIO_PIN_RESET == state);
}

int pm_read_liveInsert(Dev_powermon *pm)
{
    pm->vmePresent = readLiveInsertPin();
    return pm->vmePresent;
}

void pm_read_pgood(Dev_powermon *pm)
{
    pm->pgood_1v0_core = read_pgood_1v0_core();
    pm->pgood_1v0_mgt  = read_pgood_1v0_mgt();
    pm->pgood_1v2_mgt  = read_pgood_1v2_mgt();
    pm->pgood_2v5      = read_pgood_2v5();
    pm->pgood_3v3      = read_pgood_3v3();
    pm->pgood_3v3_fmc  = read_pgood_3v3_fmc();
}

bool get_all_pgood(const Dev_powermon *pm)
{
    return pm->pgood_1v0_core
            && pm->pgood_1v0_mgt
            && pm->pgood_1v2_mgt
            && pm->pgood_2v5
            && pm->pgood_3v3
            && pm->pgood_3v3_fmc;
}

bool get_input_power_valid(const Dev_powermon *pm)
{
    return pm_sensor_isValid(&pm->sensors[SENSOR_VXS_5V]);
}

bool get_input_power_normal(const Dev_powermon *pm)
{
    return pm_sensor_isNormal(&pm->sensors[SENSOR_VXS_5V]);
}

bool get_input_power_failed(const Dev_powermon *pm)
{
    return SENSOR_CRITICAL == pm_sensor_status(&pm->sensors[SENSOR_VXS_5V]);
}

bool get_critical_power_valid(const Dev_powermon *pm)
{
    for (int i=0; i < POWERMON_SENSORS; i++) {
        const pm_sensor *sensor = &pm->sensors[i];
        if (!sensor->isOptional)
            if (!pm_sensor_isValid(sensor))
                return false;
    }
    return true;
}

bool get_critical_power_failure(const Dev_powermon *pm)
{
    for (int i=0; i < POWERMON_SENSORS; i++) {
        const pm_sensor *sensor = &pm->sensors[i];
        if (!sensor->isOptional)
            if (pm_sensor_isCritical(sensor))
                return true;
    }
    return false;
}

void update_system_powergood_pin(const Dev_powermon *pm)
{
    int state = get_critical_power_valid(pm);
    write_gpio_pin(PGOOD_PWR_GPIO_Port,   PGOOD_PWR_Pin, state);
}

bool pm_switches_isEqual(const pm_switches &l, const pm_switches &r)
{
    return l.switch_5v == r.switch_5v
           && l.switch_5v_fmc == r.switch_5v_fmc
           && l.switch_3v3 == r.switch_3v3
           && l.switch_2v5 == r.switch_2v5
           && l.switch_1v0_core == r.switch_1v0_core
           && l.switch_1v0_mgt == r.switch_1v0_mgt
           && l.switch_1v2_mgt == r.switch_1v2_mgt;
}

bool update_power_switches(Dev_powermon *pm, bool state)
{
    // int pcb_ver = get_mcb_pcb_ver();
    if (state)
        log_put(LOG_NOTICE, "Switching ON");
    else
        log_put(LOG_NOTICE, "Switching OFF");
    pm->sw.switch_1v0_core = state;
    pm->sw.switch_1v0_mgt = state;
    pm->sw.switch_1v2_mgt = state;
    pm->sw.switch_2v5 = state;
    pm->sw.switch_3v3 = state;
    pm->sw.switch_5v_fmc = state;
    pm->sw.switch_5v = 1; // (pcb_ver == PCB_VER_A_MCB_1_0) ? 1 : state; // TTVXS version
    write_gpio_pin(ON_1V0_CORE_GPIO_Port, ON_1V0_CORE_Pin, pm->sw.switch_1v0_core);
    write_gpio_pin(ON_1V0_MGT_GPIO_Port,  ON_1V0_MGT_Pin,  pm->sw.switch_1v0_mgt);
    write_gpio_pin(ON_1V2_MGT_GPIO_Port,  ON_1V2_MGT_Pin,  pm->sw.switch_1v2_mgt);
    write_gpio_pin(ON_2V5_GPIO_Port,      ON_2V5_Pin,      pm->sw.switch_2v5);
    write_gpio_pin(ON_3V3_GPIO_Port,      ON_3V3_Pin,      pm->sw.switch_3v3);
    write_gpio_pin(ON_FMC_5V_GPIO_Port,   ON_FMC_5V_Pin,   pm->sw.switch_5v_fmc);
    write_gpio_pin(ON_5V_VXS_GPIO_Port,   ON_5V_VXS_Pin,   pm->sw.switch_5v);
    if (state)
        osDelay(1); // allow 20 us for charge with pullups
//    pm->sw_state
    pm->sw_state.switch_1v0_core = read_gpio_pin(ON_1V0_CORE_GPIO_Port, ON_1V0_CORE_Pin);
    pm->sw_state.switch_1v0_mgt = read_gpio_pin(ON_1V0_MGT_GPIO_Port,  ON_1V0_MGT_Pin);
    pm->sw_state.switch_1v2_mgt = read_gpio_pin(ON_1V2_MGT_GPIO_Port,  ON_1V2_MGT_Pin);
    pm->sw_state.switch_2v5 = read_gpio_pin(ON_2V5_GPIO_Port,      ON_2V5_Pin);
    pm->sw_state.switch_3v3 = read_gpio_pin(ON_3V3_GPIO_Port,      ON_3V3_Pin);
    pm->sw_state.switch_5v_fmc = read_gpio_pin(ON_FMC_5V_GPIO_Port,   ON_FMC_5V_Pin);
    pm->sw_state.switch_5v = pm->sw.switch_5v; // read_gpio_pin(ON_5V_VXS_GPIO_Port,   ON_5V_VXS_Pin);
    bool ok = pm_switches_isEqual(pm->sw_state, pm->sw);
    if (!ok) {
        const char *label_on = "ON";
        const char *label_off = "OFF";
        log_printf(LOG_ERR, "GPIO failure (power %s): VXS 5V %s   3.3V %s   2.5V %s   1.0V core %s   1.0V mgt %s   1.2V mgt %s   5V FMC %s",
                   state ? "enable" : "disable",
                   pm->sw_state.switch_5v ? label_on : label_off,
                   pm->sw_state.switch_3v3 ? label_on : label_off,
                   pm->sw_state.switch_2v5 ? label_on : label_off,
                   pm->sw_state.switch_1v0_core ? label_on : label_off,
                   pm->sw_state.switch_1v0_mgt ? label_on : label_off,
                   pm->sw_state.switch_1v2_mgt ? label_on : label_off,
                   pm->sw_state.switch_5v_fmc ? label_on : label_off
                   );
    }
    return ok;
}

int pm_sensors_isAllValid(const Dev_powermon *d)
{
    for (int i=0; i < POWERMON_SENSORS; i++)
        if (!pm_sensor_isValid(&d->sensors[i]))
            return 0;
    return 1;
}

void monClearMeasurements(Dev_powermon *d)
{
    for (int i=0; i<POWERMON_SENSORS; i++) {
        struct_pm_sensor_clear_minmax(&d->sensors[i]);
        struct_pm_sensor_clear_measurements(&d->sensors[i]);
    }
}

int monDetect(Dev_powermon *d)
{
    if (HAL_I2C_STATE_READY != hi2c_sensors->State) {
        log_printf(LOG_ERR, "%s: I2C not ready, state %d", __func__, hi2c_sensors->State);
        return 0;
    }
    int count = 0;
    for (int i=0; i<POWERMON_SENSORS; i++) {
        pm_sensor_reset_i2c_master();
        DeviceStatus s = pm_sensor_detect(&d->sensors[i]);
        if (s == DEVICE_NORMAL) {
            count++;
        } else {
            pm_sensor_reset_i2c_master();
        }
    }
    return count;
}

int monReadValues(Dev_powermon *d)
{
    int err = 0;
    for (int i=0; i<POWERMON_SENSORS; i++) {
        pm_sensor *sensor = &d->sensors[i];
        if (sensor->deviceStatus == DEVICE_NORMAL) {
            DeviceStatus s = pm_sensor_read(sensor);
            if (s != DEVICE_NORMAL)
                err++;
        }
    }
    return err;
}

void pm_setStateStartTick(Dev_powermon *pm)
{
    pm->stateStartTick = osKernelSysTick();
}

uint32_t getMonStateTicks(const Dev_powermon *pm)
{
    return osKernelSysTick() - pm->stateStartTick;
}

MonState runMon(Dev_powermon *pm)
{
    pm->monCycle++;
    const MonState oldState = pm->monState;
    switch(pm->monState) {
    case MON_STATE_INIT:
        monClearMeasurements(pm);
        pm->monState = MON_STATE_DETECT;
        break;
    case MON_STATE_DETECT: {
        pm_sensor_reset_i2c_master();
        int num_detected = monDetect(pm);
        if (num_detected == 0) {
            pm->monState = MON_STATE_INIT;
            break;
        }
        if (num_detected == POWERMON_SENSORS) {
            log_printf(LOG_INFO, "All %d sensors present", num_detected);
            pm->monState = MON_STATE_READ;
            break;
        }
        if (getMonStateTicks(pm) > DETECT_TIMEOUT_TICKS) {
            log_printf(LOG_ERR, "Sensor detect timeout, %d of %d found", num_detected, POWERMON_SENSORS);
            pm->monState = MON_STATE_READ;
        }
        break;
    }
    case MON_STATE_READ:
        if (monReadValues(pm) == 0)
            pm->monState = MON_STATE_READ;
        else
            pm->monState = MON_STATE_ERROR;
        break;
    case MON_STATE_ERROR:
        log_put(LOG_ERR, "Sensor read error");
        pm->monErrors++;
        pm->monState = MON_STATE_INIT;
        break;
    default:
        break;
    }
    if (oldState != pm->monState) {
        pm_setStateStartTick(pm);
    }
    return pm->monState;
}
