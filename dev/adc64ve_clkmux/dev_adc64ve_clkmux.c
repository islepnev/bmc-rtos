/*
**    Copyright 2021 Ilja Slepnev
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

#include "dev_adc64ve_clkmux.h"

#include <string.h>
#include <stdint.h>

#include "bsp_pin_defs.h"
#include "cmsis_os.h"
#include "dev_adc64ve_clkmux_types.h"
#include "gpio_util.h"
#include "log/log.h"
#include "mcp23017/mcp23017_i2c_hal.h"

typedef union {
    struct {
        uint8_t unused: 2;
        uint8_t crsw_sin: 2;
        uint8_t crsw_sout: 2;
        uint8_t crsw_load: 1;
        uint8_t crsw_conf: 1;
    } bit;
    uint8_t all;
} clkmux_gpioa;

typedef union {
    struct {
        uint8_t unused: 2;
        uint8_t crsw_sin: 2;
        uint8_t crsw_sout: 2;
        uint8_t crsw_load: 1;
        uint8_t crsw_conf: 1;
    } bit;
    uint8_t all;
} clkmux_gpiob;

enum {
    CRSW1_IN_VXS = 0,
    CRSW1_IN_FB9545 = 1,
    CRSW1_IN_UNCONNECTED = 2,
    CRSW1_IN_AD9516 = 3,
};

enum {
    CRSW2_IN_PLL0A = 0,
    CRSW2_IN_PLL0B = 1,
    CRSW2_IN_PLL0C = 2,
    CRSW2_IN_CRSW1_Q3 = 3,
};

static bool dev_clkmux_set_crsw1(Dev_adc64ve_clkmux *d)
{
    clkmux_gpiob data;
    data.all = 0;
    if (!mcp23017_write(&d->dev, MCP23017_GPIOB, data.all))
        goto err;
    int clock_source = (d->priv.pll_source == ADC64VE_PLL_SOURCE_VXS) ? CRSW1_IN_VXS : CRSW1_IN_AD9516;
    const int crsw1_output_map[4] = {
        clock_source,
        CRSW1_IN_FB9545,
        clock_source,
        clock_source
    };
    for (int i=0; i<4; i++) {
        data.bit.crsw_sin = crsw1_output_map[i];
        data.bit.crsw_sout = i;
        if (!mcp23017_write(&d->dev, MCP23017_GPIOB, data.all))
            goto err;
        data.bit.crsw_load = 1;
        if (!mcp23017_write(&d->dev, MCP23017_GPIOB, data.all))
            goto err;
        data.bit.crsw_load = 0;
        if (!mcp23017_write(&d->dev, MCP23017_GPIOB, data.all))
            goto err;
        data.bit.crsw_conf = 1;
        if (!mcp23017_write(&d->dev, MCP23017_GPIOB, data.all))
            goto err;
        data.bit.crsw_conf = 0;
        if (!mcp23017_write(&d->dev, MCP23017_GPIOB, data.all))
            goto err;
    }
    return true;
err:
    return false;
}

static bool dev_clkmux_set_crsw2(Dev_adc64ve_clkmux *d)
{
    clkmux_gpioa data;
    data.all = 0;
    if (!mcp23017_write(&d->dev, MCP23017_GPIOA, data.all))
        return false;
    const int crsw2_output_map[4] = {
        CRSW2_IN_PLL0A,
        CRSW2_IN_PLL0B,
        CRSW2_IN_PLL0B,
        CRSW2_IN_PLL0B
    };
    for (int i=0; i<4; i++) {
        data.bit.crsw_sin = crsw2_output_map[i];
        data.bit.crsw_sout = i;
        if (!mcp23017_write(&d->dev, MCP23017_GPIOA, data.all))
            goto err;
        data.bit.crsw_load = 1;
        if (!mcp23017_write(&d->dev, MCP23017_GPIOA, data.all))
            goto err;
        data.bit.crsw_load = 0;
        if (!mcp23017_write(&d->dev, MCP23017_GPIOA, data.all))
            goto err;
        data.bit.crsw_conf = 1;
        if (!mcp23017_write(&d->dev, MCP23017_GPIOA, data.all))
            goto err;
        data.bit.crsw_conf = 0;
        if (!mcp23017_write(&d->dev, MCP23017_GPIOA, data.all))
            goto err;
    }
    return true;
err:
    return false;
}

bool dev_adc64ve_clkmux_detect(Dev_adc64ve_clkmux *d)
{
    write_gpio_pin(CLK_IO_EXP_RESET_B_GPIO_Port, CLK_IO_EXP_RESET_B_Pin, 0);
    write_gpio_pin(CLK_IO_EXP_RESET_B_GPIO_Port, CLK_IO_EXP_RESET_B_Pin, 1);
    osDelay(1);
    bool reset = read_gpio_pin(CLK_IO_EXP_RESET_B_GPIO_Port, CLK_IO_EXP_RESET_B_Pin);
    if (0 == reset) {
        log_printf(LOG_ERR, "MCP23017 reset pin stuck low");
        goto err;
    }
    if (!mcp23017_detect(&d->dev)) {
        log_printf(LOG_ERR, "MCP23017 detect failed");
        goto err;
    }
//    uint8_t data = 0x55;
//    if (! mcp23017_read(MCP23017_IODIRB, &data))
//        goto err;
//    if (data != 0xFF) {
//        log_put(LOG_ERR, "clkmux: bad default value for register 1");
//        goto err;
//    }
    uint8_t data = 0x55;
    if (! mcp23017_read(&d->dev, MCP23017_IPOLA, &data))
        return DEVICE_FAIL;
    if (data != 0x00) {
        log_printf(LOG_ERR, "clkmux: bad default value for register %d", MCP23017_IPOLA);
        goto err;
    }
    // set GPB1,GPB0
    if (! mcp23017_write(&d->dev, MCP23017_IODIRA, 0x00)) // 0 = output
        goto err;
    if (! mcp23017_write(&d->dev, MCP23017_IODIRB, 0x00)) // 0 = output
        goto err;

    return true;
err:
    return false;
}

bool dev_adc64ve_clkmux_set(struct Dev_adc64ve_clkmux *d)
{
    bool ok = true;
    ok &= dev_clkmux_set_crsw1(d);
    ok &= dev_clkmux_set_crsw2(d);
    return ok;
}
