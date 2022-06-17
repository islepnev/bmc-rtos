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

#include "dev_tqdc_clkmux.h"

#include <string.h>
#include <stdint.h>

#include "dev_tqdc_clkmux_types.h"
#include "log/log.h"
#include "mcp23017/mcp23017_i2c_hal.h"
#include "app_shared_data.h"
#include "devicelist.h"
#include "display_common.h"

typedef union {
    struct {
        uint8_t clk_tdc_sel: 1;
        uint8_t reserved: 1;
        uint8_t clk_adc_sel: 1;
        uint8_t reserved2: 5;
    } bit;
    uint8_t all;
} clkmux_gpioa;

typedef union {
    struct {
        uint8_t reserved: 2;
        uint8_t crsw_sin: 2;
        uint8_t crsw_sout: 2;
        uint8_t crsw_load: 1;
        uint8_t crsw_conf: 1;
    } bit;
    uint8_t all;
} clkmux_gpiob;

typedef enum {
    CRSW1_IN_VXS = 0,
    CRSW1_IN_AD9516_DIV3 = 1,
    CRSW1_IN_REFIN = 2,
    CRSW1_IN_PLL_FB = 3,
} crsw_enum_t;

void dev_tqdc_clkmux_init(Dev_tqdc_clkmux *d)
{
    set_device_status(&d->dev, DEVICE_UNKNOWN);
    d->priv.clk_source = TQDC_CLK_SOURCE_LOCAL;
}

static bool is_fpga_device_ok()
{
    const DeviceBase *d = find_device_const(DEV_CLASS_FPGA);
    if (!d)
        return false;
    return d->device_status != DEVICE_UNKNOWN;
}

tqdc_clk_source_t get_clk_source()
{
    bool control_valid = is_fpga_device_ok();
    bool pll_bypass = control_valid ? clock_control.pll_bypass : false;
    clock_source_t selected_source = control_valid ? clock_control.source : CLOCK_SOURCE_LOCAL;
    switch (selected_source) {
    case CLOCK_SOURCE_VXS:
    case CLOCK_SOURCE_TTC:
    case CLOCK_SOURCE_LOCAL:
    case CLOCK_SOURCE_AUTO:
        break;
    default: selected_source = CLOCK_SOURCE_AUTO;
    }
    if (selected_source == CLOCK_SOURCE_AUTO) {
        if (clock_control.valid & CLOCK_SOURCE_VXS)
            selected_source = CLOCK_SOURCE_VXS;
        else
        if (clock_control.valid & CLOCK_SOURCE_TTC)
            selected_source = CLOCK_SOURCE_TTC;
        else
            selected_source = CLOCK_SOURCE_LOCAL;
    }
    switch (selected_source) {
    case CLOCK_SOURCE_VXS:
        return pll_bypass ? TQDC_CLK_SOURCE_VXS: TQDC_CLK_SOURCE_VXS_PLL;
    case CLOCK_SOURCE_TTC:
        return pll_bypass ? TQDC_CLK_SOURCE_REFIN: TQDC_CLK_SOURCE_REFIN_PLL;
    default:
        return pll_bypass ? TQDC_CLK_SOURCE_LOCAL: TQDC_CLK_SOURCE_LOCAL_PLL;
    }
}

bool dev_tqdc_clkmux_set(Dev_tqdc_clkmux *d)
{
    static tqdc_clk_source_t prev_source = -1;
    d->priv.clk_source = get_clk_source();
    if (d->priv.clk_source != prev_source) {
        prev_source = d->priv.clk_source;
        log_printf(LOG_INFO, "ClkMux: switch to %s", tqdc_clk_source_text(d->priv.clk_source));
    }
    clkmux_gpioa gpioa;
    gpioa.all = 0;
    gpioa.bit.clk_adc_sel =
        d->priv.clk_source == TQDC_CLK_SOURCE_LOCAL;
    gpioa.bit.clk_tdc_sel =
        d->priv.clk_source == TQDC_CLK_SOURCE_LOCAL_PLL ||
        d->priv.clk_source == TQDC_CLK_SOURCE_VXS_PLL ||
        d->priv.clk_source == TQDC_CLK_SOURCE_REFIN_PLL;

    if (!mcp23017_write(&d->dev, MCP23017_GPIOB, gpioa.all))
        goto err;

    clkmux_gpiob gpiob;
    gpiob.all = 0;
    if (!mcp23017_write(&d->dev, MCP23017_GPIOB, gpiob.all))
        goto err;
    crsw_enum_t crsw_in_main;
    switch (d->priv.clk_source) {
    case TQDC_CLK_SOURCE_VXS:
    case TQDC_CLK_SOURCE_VXS_PLL:
        crsw_in_main = CRSW1_IN_VXS; break;
    case TQDC_CLK_SOURCE_REFIN:
    case TQDC_CLK_SOURCE_REFIN_PLL:
        crsw_in_main = CRSW1_IN_REFIN; break;
    default: crsw_in_main = CRSW1_IN_AD9516_DIV3; break;
    }
    int crsw1_output_map[4] = {
        crsw_in_main,
        crsw_in_main,
        CRSW1_IN_PLL_FB,
        crsw_in_main
    };
    for (int i=0; i<4; i++) {
        gpiob.bit.crsw_sin = crsw1_output_map[i];
        gpiob.bit.crsw_sout = i;
        if (!mcp23017_write(&d->dev, MCP23017_GPIOB, gpiob.all))
            goto err;
        gpiob.bit.crsw_load = 1;
        if (!mcp23017_write(&d->dev, MCP23017_GPIOB, gpiob.all))
            goto err;
        gpiob.bit.crsw_load = 0;
        if (!mcp23017_write(&d->dev, MCP23017_GPIOB, gpiob.all))
            goto err;
        gpiob.bit.crsw_conf = 1;
        if (!mcp23017_write(&d->dev, MCP23017_GPIOB, gpiob.all))
            goto err;
        gpiob.bit.crsw_conf = 0;
        if (!mcp23017_write(&d->dev, MCP23017_GPIOB, gpiob.all))
            goto err;
    }
    return true;
err:
    return false;
}

bool dev_tqdc_clkmux_detect(Dev_tqdc_clkmux *d)
{
    if (! mcp23017_detect(&d->dev)) {
        goto err;
    }
//    uint8_t data = 0x55;
//    if (! mcp23017_read(&d->dev, MCP23017_IODIRB, &data))
//        goto err;
//    if (data != 0xFF) {
//        log_put(LOG_ERR, "clkmux: bad default value for register 1");
//        goto err;
//    }
    uint8_t data = 0x55;
    if (! mcp23017_read(&d->dev, MCP23017_IPOLA, &data))
        goto err;
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
