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

#include "app_task_fpga_impl.h"

#include <stdint.h>

#include "app_shared_data.h"
#include "bsp.h"
#include "bsp_fpga.h"
#include "bsp_pin_defs.h"
#include "cmsis_os.h"
#include "dev_common_types.h"
#include "fpga/dev_fpga.h"
#include "fpga/dev_fpga_types.h"
#include "fpga_spi_hal.h"
#include "fpga_spi_iostat.h"
#include "gpio.h"
#include "log/log.h"
#include "powermon/dev_powermon.h"
#include "powermon/dev_powermon_types.h"

static const uint32_t LOAD_DELAY_TICKS = 5000;
static const uint32_t DETECT_DELAY_TICKS = 100;
static const uint32_t ERROR_DELAY_TICKS = 3000;
static const uint32_t POLL_DELAY_TICKS  = 500;


static uint32_t stateTicks(const Dev_fpga_priv *p)
{
    return osKernelSysTick() - p->fsm.stateStartTick;
}

static bool read_init_b(void)
{
    if (! fpga_done_pin_present())
        return 1;
#ifdef FPGA_INIT_B_Pin
    return read_gpio_pin(FPGA_INIT_B_GPIO_Port, FPGA_INIT_B_Pin);
#else
    return 1;
#endif
}
static bool read_done(void)
{
    if (! fpga_done_pin_present())
        return 1;
#ifdef FPGA_DONE_Pin
    return read_gpio_pin(FPGA_DONE_GPIO_Port, FPGA_DONE_Pin);
#else
    return 1;
#endif
}

static int old_fpga_done = -1;
static int fpga_detect_fail_count = 0;
static int fpga_error_count = 0;
static bool load_timeout = 0;

void fpga_task_run(Dev_fpga *d)
{
    const fpga_state_t old_state = d->priv.fsm.state;
    Dev_fpga_gpio *gpio = &d->priv.gpio;
    gpio->initb = read_init_b();
    gpio->done = read_done();
    int fpga_core_power_present = true;
#ifdef ENABLE_POWERMON
    const Dev_powermon_priv *priv = get_powermon_priv_const();
    if (priv) {
        const pm_sensors_arr *sensors = &priv->sensors;
        fpga_core_power_present = get_fpga_core_power_present(sensors);
    }
#endif
    int fpga_power_present = enable_power && fpga_core_power_present;
    int fpga_enable = main_clock_ready && fpga_power_present && gpio->initb;
//    int fpga_loading = fpga_power_present && gpio->initb && !gpio->done;
    int fpga_done = fpga_power_present && gpio->done;
    bool fpga_loaded = !old_fpga_done && fpga_done;
    if (old_fpga_done != fpga_done) {
        old_fpga_done = fpga_done;
        if (fpga_done)
            fpga_enable_interface(&d->dev.bus);
        else
            fpga_disable_interface(&d->dev.bus);
    }
    switch (d->priv.fsm.state) {
    case FPGA_STATE_STANDBY:
        d->dev.device_status = DEVICE_UNKNOWN;
        clear_fpga_runtime_info();
        fpga_detect_fail_count = 0;
        fpga_error_count = 0;
        load_timeout = 0;
        if (fpga_enable) {
            d->priv.fsm.state = FPGA_STATE_LOAD;
            d->priv.fsm.fpga_load_start_tick = osKernelSysTick();
        }
        break;
    case FPGA_STATE_LOAD:
        if (!fpga_enable) {
            d->priv.fsm.state = FPGA_STATE_STANDBY;
            break;
        }
        if (fpga_done) {
            if (fpga_done_pin_present()) {
                const uint32_t tick_freq_hz = osKernelSysTickFrequency;
                const uint32_t ticks = osKernelSysTick() - d->priv.fsm.fpga_load_start_tick;
                if (fpga_loaded)
                    log_printf(LOG_INFO, "FPGA loaded in %u ms", ticks * 1000 / tick_freq_hz);
            }
            d->priv.fsm.state = FPGA_STATE_RESET;
            break;
        }
        if (stateTicks(&d->priv) > LOAD_DELAY_TICKS) {
            if (!load_timeout) {
                load_timeout = true;
                log_put(LOG_ERR, "FPGA load timeout");
            }
            // d->priv.fsm.state = FPGA_STATE_ERROR;
        }
        break;
    case FPGA_STATE_RESET:
        if (!fpga_power_present) {
            d->priv.fsm.state = FPGA_STATE_STANDBY;
            break;
        }
        if (fpga_done_pin_present() && !fpga_done) {
            load_timeout = 0;
            d->priv.fsm.state = FPGA_STATE_LOAD;
            break;
        }
        d->dev.device_status = DEVICE_UNKNOWN;
        d->priv.fpga.csr_read = 0;
        if (!fpgaDetect(d)) {
            d->priv.fsm.state = FPGA_STATE_ERROR;
            break;
        }
        if (d->dev.device_status == DEVICE_NORMAL) {
            d->priv.fsm.state = FPGA_STATE_RUN;
            break;
        }
        // log_printf(LOG_INFO, "FPGA device Ok");
        // if (! fpga_done_pin_present()) {
        // const uint32_t tick_freq_hz = osKernelSysTickFrequency;
        // const uint32_t ticks = osKernelSysTick() - d->priv.fsm.fpga_load_start_tick;
        // if (fpga_loaded)
        //     log_printf(LOG_INFO, "FPGA loaded in %u ms", ticks * 1000 / tick_freq_hz);
        // }
        {
            uint32_t detect_timeout = DETECT_DELAY_TICKS;
            if (! fpga_done_pin_present())
                detect_timeout += LOAD_DELAY_TICKS;
            if (stateTicks(&d->priv) > detect_timeout) {
                // if (! fpga_detect_fail_count)
                //      log_put(LOG_ERR, "FPGA detect timeout");
                fpga_detect_fail_count++;
                d->priv.fsm.state = FPGA_STATE_ERROR;
            }
        }
        break;
    case FPGA_STATE_RUN:
        if (!fpga_power_present) {
            d->priv.fsm.state = FPGA_STATE_STANDBY;
            break;
        }
        if (!fpga_done) {
            d->priv.fsm.state = FPGA_STATE_STANDBY;
            break;
        }
        uint32_t dev_errors_before = bus_iostat_total_errors(&iostat);
        if (fpga_periodic_task(d)) {
            d->priv.fsm.state = FPGA_STATE_PAUSE;
            uint32_t dev_errors_after = bus_iostat_total_errors(&iostat);
            uint32_t nerr = dev_errors_after - dev_errors_before;
            if (nerr && d->dev.sensor == SENSOR_NORMAL)
                d->dev.sensor = SENSOR_WARNING;
            if (!nerr && d->dev.sensor == SENSOR_WARNING)
                d->dev.sensor = SENSOR_NORMAL;
            break;
        } else {
            // log_printf(LOG_ERR, "FPGA communication error");
            fpga_error_count++;
            d->priv.fsm.state = FPGA_STATE_ERROR;
            break;
        }
    case FPGA_STATE_PAUSE:
        if (!fpga_done)
            d->priv.fsm.state = FPGA_STATE_STANDBY;
        if (stateTicks(&d->priv) > POLL_DELAY_TICKS) {
            d->priv.fsm.state = FPGA_STATE_RUN;
        }
        break;
    case FPGA_STATE_ERROR:
        // if (!fpga_error_count)
        //     log_printf(LOG_ERR, "FPGA error");
        fpga_error_count++;
        d->dev.device_status = DEVICE_FAIL;
        if (stateTicks(&d->priv) > ERROR_DELAY_TICKS) {
            d->priv.fsm.state = FPGA_STATE_RESET;
        }
        break;
    default:
        break;
    }
    if (old_state != d->priv.fsm.state) {
        d->priv.fsm.stateStartTick = osKernelSysTick();
    }
}
