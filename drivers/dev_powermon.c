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
#include "display.h"
#include "logbuffer.h"
#include "dev_pm_sensors.h"
#include "dev_pm_sensors_types.h"
#include "dev_pot.h"
#include "bsp.h"
#include "bsp_pin_defs.h"
#include "cmsis_os.h"
#include "powermon_i2c_driver.h"
#include "app_shared_data.h"
#include "commands.h"
#include "error_handler.h"

static const uint32_t DETECT_TIMEOUT_TICKS = 1000;

int monIsOn(const pm_switches *sw, SensorIndex index)
{
    switch(index) {
    case SENSOR_1V5: return sw->switch_1v5;
    case SENSOR_5V: return sw->switch_5v;
    case SENSOR_5VPC: return 1;
    case SENSOR_VME_5V: return 1;
    case SENSOR_3V3: return sw->switch_3v3;
    case SENSOR_VME_3V3: return 1;
    case SENSOR_VMCU: return 1;
    case SENSOR_FPGA_CORE_1V0: return sw->switch_1v0;
    case SENSOR_FPGA_MGT_1V0: return sw->switch_1v0;
    case SENSOR_FPGA_MGT_1V2: return sw->switch_1v0;
    case SENSOR_FPGA_1V8: return sw->switch_3v3;
    case SENSOR_TDC_A: return sw->switch_3v3;
    case SENSOR_TDC_B: return sw->switch_3v3;
    case SENSOR_TDC_C: return sw->switch_3v3;
    case SENSOR_CLOCK_2V5: return sw->switch_3v3;
    }
    return 0;
}

void struct_powermon_sensors_init(Dev_powermon *d)
{
    for (int i=0; i<POWERMON_SENSORS; i++) {
        struct_pm_sensor_init(&d->sensors[i], i);
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
    d->fpga_core_pgood = 0;
    d->ltm_pgood = 0;
    d->sw.switch_5v = 1;
    d->sw.switch_3v3 = 1;
    d->sw.switch_1v5 = 1;
    d->sw.switch_1v0 = 1;
    struct_pots_init(&d->pots);
}

static int readPowerGoodFpga(void)
{
    return (GPIO_PIN_SET == HAL_GPIO_ReadPin(FPGA_CORE_PGOOD_GPIO_Port, FPGA_CORE_PGOOD_Pin));
}

static int readPowerGood1v5(void)
{
    return (GPIO_PIN_SET == HAL_GPIO_ReadPin(LTM_PGOOD_GPIO_Port, LTM_PGOOD_Pin));
}

static int readLiveInsertPin(void)
{
    GPIO_PinState state;
    state = HAL_GPIO_ReadPin(VME_DET_B_GPIO_Port, VME_DET_B_Pin);
    return (GPIO_PIN_RESET == state);
}

int pm_read_liveInsert(Dev_powermon *pm)
{
    pm->vmePresent = readLiveInsertPin();
    return pm->vmePresent;
}

void pm_read_pgood(Dev_powermon *pm)
{
    pm->fpga_core_pgood = readPowerGoodFpga();
    pm->ltm_pgood = readPowerGood1v5();
}

bool get_all_pgood(const Dev_powermon *pm)
{
    return pm->fpga_core_pgood && pm->ltm_pgood;
}

void read_power_switches_state(Dev_powermon *pm)
{
    pm->sw_state.switch_1v0   = (GPIO_PIN_SET == HAL_GPIO_ReadPin(ON_1_0V_1_2V_GPIO_Port, ON_1_0V_1_2V_Pin));
    pm->sw_state.switch_1v5   = (GPIO_PIN_SET == HAL_GPIO_ReadPin(ON_1_5V_GPIO_Port, ON_1_5V_Pin));
    pm->sw_state.switch_3v3   = (GPIO_PIN_SET == HAL_GPIO_ReadPin(ON_3_3V_GPIO_Port, ON_3_3V_Pin));
    pm->sw_state.switch_5v    = (GPIO_PIN_SET == HAL_GPIO_ReadPin(ON_5V_GPIO_Port,   ON_5V_Pin));
    pm->sw_state.switch_tdc_a = (GPIO_PIN_SET == HAL_GPIO_ReadPin(ON_TDC_A_GPIO_Port, ON_TDC_A_Pin));
    pm->sw_state.switch_tdc_b = (GPIO_PIN_SET == HAL_GPIO_ReadPin(ON_TDC_B_GPIO_Port, ON_TDC_B_Pin));
    pm->sw_state.switch_tdc_c = (GPIO_PIN_SET == HAL_GPIO_ReadPin(ON_TDC_C_GPIO_Port, ON_TDC_C_Pin));
}

bool check_power_switches(const Dev_powermon *pm)
{
    bool ret = true;
    if (pm->sw_state.switch_5v != pm->sw.switch_5v) {
        log_printf(LOG_CRIT, "5V switch failure: stuck %s", pm->sw_state.switch_5v ? "high" : "low");
        ret = false;
    }
    if (pm->sw_state.switch_3v3 != pm->sw.switch_3v3) {
        log_printf(LOG_CRIT, "3.3V switch failure: stuck %s", pm->sw_state.switch_3v3 ? "high" : "low");
        ret = false;
    }
    if (pm->sw_state.switch_1v5 != pm->sw.switch_1v5) {
        log_printf(LOG_CRIT, "1.5V switch failure: stuck %s", pm->sw_state.switch_1v5 ? "high" : "low");
        ret = false;
    }
    if (pm->sw_state.switch_1v0 != pm->sw.switch_1v0) {
        log_printf(LOG_CRIT, "1.0V switch failure: stuck %s", pm->sw_state.switch_1v0 ? "high" : "low");
        ret = false;
    }
    if (pm->sw_state.switch_tdc_a != pm->sw.switch_tdc_a) {
        log_printf(LOG_CRIT, "TDC-A switch failure: stuck %s", pm->sw_state.switch_tdc_a ? "high" : "low");
        ret = false;
    }
    if (pm->sw_state.switch_tdc_b != pm->sw.switch_tdc_b) {
        log_printf(LOG_CRIT, "TDC-B switch failure: stuck %s", pm->sw_state.switch_tdc_b ? "high" : "low");
        ret = false;
    }
    if (pm->sw_state.switch_tdc_c != pm->sw.switch_tdc_c) {
        log_printf(LOG_CRIT, "TDC-C switch failure: stuck %s", pm->sw_state.switch_tdc_c ? "high" : "low");
        ret = false;
    }
    return ret;
}

void update_power_switches(Dev_powermon *pm)
{
    // turn off only when failed
    bool state_primary = (pm->pmState != PM_STATE_PWRFAIL) && (pm->pmState != PM_STATE_OFF) && (pm->pmState != PM_STATE_OVERHEAT);
    bool state = state_primary;

    // primary switches (required for monitors)
    pm->sw.switch_5v  = state_primary; // VME 5V and 3.3V
    pm->sw.switch_3v3 = state_primary;
    // secondary switches
    pm->sw.switch_1v5 = state;
    pm->sw.switch_1v0 = state;
    pm->sw.switch_tdc_a = state;
    pm->sw.switch_tdc_b = state;
    pm->sw.switch_tdc_c = state;

    HAL_GPIO_WritePin(ON_1_0V_1_2V_GPIO_Port, ON_1_0V_1_2V_Pin, pm->sw.switch_1v0 ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(ON_1_5V_GPIO_Port,      ON_1_5V_Pin,      pm->sw.switch_1v5 ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(ON_3_3V_GPIO_Port,      ON_3_3V_Pin,      pm->sw.switch_3v3 ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(ON_5V_GPIO_Port,        ON_5V_Pin,        pm->sw.switch_5v  ? GPIO_PIN_SET : GPIO_PIN_RESET);

    HAL_GPIO_WritePin(ON_TDC_A_GPIO_Port, ON_TDC_A_Pin, pm->sw.switch_tdc_a ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(ON_TDC_B_GPIO_Port, ON_TDC_B_Pin, pm->sw.switch_tdc_b ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(ON_TDC_C_GPIO_Port, ON_TDC_C_Pin, pm->sw.switch_tdc_c ? GPIO_PIN_SET : GPIO_PIN_RESET);

    read_power_switches_state(pm);
}

int pm_sensors_isAllValid(const Dev_powermon *d)
{
    for (int i=0; i < POWERMON_SENSORS; i++)
        if (!pm_sensor_isValid(&d->sensors[i]))
            return 0;
    return 1;
}

SensorStatus pm_sensors_getStatus(const Dev_powermon *d)
{
    SensorStatus maxStatus = SENSOR_NORMAL;
    for (int i=0; i < POWERMON_SENSORS; i++) {
        const pm_sensor *sensor = &d->sensors[i];
        if (sensor->isOptional)
            continue;
        DeviceStatus deviceStatus = sensor->deviceStatus;
        if (deviceStatus != DEVICE_NORMAL)
            maxStatus = SENSOR_CRITICAL;
        int isOn = monIsOn(&d->sw, i);
        if (isOn) {
            SensorStatus status = pm_sensor_status(sensor);
            if (status > maxStatus)
                maxStatus = status;
        }
    }
    return maxStatus;
}

void monClearMinMax(Dev_powermon *d)
{
    for (int i=0; i<POWERMON_SENSORS; i++)
        struct_pm_sensor_clear_minmax(&d->sensors[i]);
}

void monClearMeasurements(Dev_powermon *d)
{
    monClearMinMax(d);
    for (int i=0; i<POWERMON_SENSORS; i++) {
        struct_pm_sensor_clear_measurements(&d->sensors[i]);
    }
}

int monDetect(Dev_powermon *d)
{
//    if (HAL_I2C_STATE_READY != hi2c_sensors->State) {
//        log_printf(LOG_ERR, "%s: I2C not ready, state %d", __func__, hi2c_sensors->State);
//        return 0;
//    }
    int count = 0;
    for (int i=0; i<POWERMON_SENSORS; i++) {
        powermon_i2c_reset_master();
        DeviceStatus s = pm_sensor_detect(&d->sensors[i]);
        if (s == DEVICE_NORMAL) {
            count++;
        } else {
            powermon_i2c_reset_master();
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

void pot_command_process(Dev_pots *d, const CommandPots *cmd)
{
    if (cmd->arg >= DEV_POT_COUNT)
        return;
    Dev_ad5141 *p = &d->pot[cmd->arg];
    switch (cmd->command_id) {
    case COMMAND_POTS_RESET:
        dev_ad5141_reset(p);
        break;
    case COMMAND_POTS_INC:
        dev_ad5141_inc(p);
        break;
    case COMMAND_POTS_DEC:
        dev_ad5141_dec(p);
        break;
    case COMMAND_POTS_WRITE:
        dev_ad5141_write(p);
        break;
    default:
        break;
    }
}

static const int POT_MAX_MAIL_BATCH = 10;

void pot_check_mail(Dev_pots *d)
{
    if (!mq_cmd_pots_id)
        Error_Handler();
    for (int i=0; i<POT_MAX_MAIL_BATCH; i++) {
        osEvent event = osMailGet(mq_cmd_pots_id, 0);
        if (osEventMail != event.status) {
            return;
        }
        CommandPots *mail = (CommandPots *) event.value.p;
        if (!mail)
            Error_Handler();
        pot_command_process(d, mail);
        osMailFree(mq_cmd_pots_id, mail);
    }
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
        powermon_i2c_reset_master();
        int pots_detected = pot_detect(&pm->pots);
        powermon_i2c_reset_master();
        int num_detected = monDetect(pm);
        update_board_version(num_detected, pots_detected);
        if (board_version == PCB_4_2) {
            if ((num_detected == POWERMON_SENSORS_PCB_4_2) && (pots_detected == DEV_POT_COUNT)) {
                log_put(LOG_INFO, "PCB 4.2 sensors and digipots detected");
                pm->monState = MON_STATE_READ;
                break;
            }
        }
        if (board_version == PCB_4_1) {
            if ((num_detected == POWERMON_SENSORS_PCB_4_1) && (pots_detected == 0)) {
                log_put(LOG_INFO, "PCB 4.1 sensors detected");
                pm->monState = MON_STATE_READ;
                break;
            }
        }

        if (num_detected == 0) {
            pm->monState = MON_STATE_INIT;
            break;
        }

        if (getMonStateTicks(pm) > DETECT_TIMEOUT_TICKS) {
            log_printf(LOG_ERR, "Sensor detect timeout, %d of %d found", num_detected, POWERMON_SENSORS);
            pm->monState = MON_STATE_READ;
        }
        break;
    }
    case MON_STATE_READ:
        if (board_version >= PCB_4_2) {
            pot_read_rdac_all(&pm->pots);
            pot_check_mail(&pm->pots);
        }
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

int get_input_power_valid(const Dev_powermon *pm)
{
    return pm_sensor_isValid(&pm->sensors[SENSOR_VME_5V]);
}

int get_critical_power_valid(const Dev_powermon *pm)
{
    for (int i=0; i < POWERMON_SENSORS; i++) {
        const pm_sensor *sensor = &pm->sensors[i];
        if (!sensor->isOptional)
            if (!pm_sensor_isValid(sensor))
                return 0;
    }
    return 1;
}

int get_fpga_core_power_present(const Dev_powermon *pm)
{
    SensorStatus status_1v0 = pm_sensor_status(&pm->sensors[SENSOR_FPGA_CORE_1V0]);
    SensorStatus status_1v8 = pm_sensor_status(&pm->sensors[SENSOR_FPGA_1V8]);
    int present_1v0 = ((status_1v0 == SENSOR_NORMAL) || (status_1v0 == SENSOR_WARNING));
    int present_1v8 = ((status_1v8 == SENSOR_NORMAL) || (status_1v8 == SENSOR_WARNING));
    return present_1v0 && present_1v8;
}

static double get_sensor_power_w(const pm_sensor *d)
{
    SensorStatus sensor_status = pm_sensor_status(d);
    int sensor_present = ((sensor_status == SENSOR_NORMAL) || (sensor_status == SENSOR_WARNING));
    if (sensor_present)
        return d->busVoltage * d->current;
    else
        return 0;
}

double pm_get_power_w(const Dev_powermon *pm)
{
    double mw = 0;
    mw += get_sensor_power_w(&pm->sensors[SENSOR_5VPC]);
    mw += get_sensor_power_w(&pm->sensors[SENSOR_VME_5V]);
    mw += get_sensor_power_w(&pm->sensors[SENSOR_VME_3V3]);
    return mw;
}

double pm_get_power_max_w(const Dev_powermon *pm)
{
    double mw = 0;
    mw += pm->sensors[SENSOR_5VPC].powerMax;
    mw += pm->sensors[SENSOR_VME_5V].powerMax;
    mw += pm->sensors[SENSOR_VME_3V3].powerMax;
    return mw;
}
