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
#include "mcp23017/mcp23017_i2c_hal.h"

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
    d->dev.device_status = DEVICE_UNKNOWN;
    d->priv.clk_source = TQDC_CLK_SOURCE_LOCAL_DIRECT;
}

static bool dev_clkmux_set_internal(Dev_tqdc_clkmux *d)
{
    bool ok = true;
    clkmux_gpioa gpioa;
    gpioa.all = 0;
    gpioa.bit.clk_adc_sel = (d->priv.clk_source == TQDC_CLK_SOURCE_LOCAL_DIRECT);
    gpioa.bit.clk_tdc_sel = !gpioa.bit.clk_adc_sel;
    ok &= mcp23017_write(&d->dev, MCP23017_GPIOB, gpioa.all);

    clkmux_gpiob gpiob;
    gpiob.all = 0;
    ok &= mcp23017_write(&d->dev, MCP23017_GPIOB, gpiob.all);
    crsw_enum_t crsw_in_main;
    switch (d->priv.clk_source) {
    case TQDC_CLK_SOURCE_VXS: crsw_in_main = CRSW1_IN_VXS; break;
    case TQDC_CLK_SOURCE_REFIN: crsw_in_main = CRSW1_IN_REFIN; break;
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
        ok &= mcp23017_write(&d->dev, MCP23017_GPIOB, gpiob.all);
        gpiob.bit.crsw_load = 1;
        ok &= mcp23017_write(&d->dev, MCP23017_GPIOB, gpiob.all);
        gpiob.bit.crsw_load = 0;
        ok &= mcp23017_write(&d->dev, MCP23017_GPIOB, gpiob.all);
        gpiob.bit.crsw_conf = 1;
        ok &= mcp23017_write(&d->dev, MCP23017_GPIOB, gpiob.all);
        gpiob.bit.crsw_conf = 0;
        ok &= mcp23017_write(&d->dev, MCP23017_GPIOB, gpiob.all);
    }
    return ok;
}

DeviceStatus dev_tqdc_clkmux_detect(Dev_tqdc_clkmux *d)
{
    if (! mcp23017_detect(&d->dev)) {
        goto unknown;
    }
//    uint8_t data = 0x55;
//    if (! mcp23017_read(&d->dev, MCP23017_IODIRB, &data))
//        goto err;
//    if (data != 0xFF) {
//        log_put(LOG_ERR, "clkmux: bad default value for register 1");
//        goto err;
//    }
//    if (! mcp23017_read(&d->dev, MCP23017_IPOLA, &data))
//        return DEVICE_FAIL;
//    if (data != 0x00) {
//        log_put(LOG_ERR, "clkmux: bad default value for register 2");
//        goto err;
//    }
    // set GPB1,GPB0
    if (! mcp23017_write(&d->dev, MCP23017_IODIRA, 0x00)) // 0 = output
        goto err;
    if (! mcp23017_write(&d->dev, MCP23017_IODIRB, 0x00)) // 0 = output
        goto err;

    d->dev.device_status = DEVICE_NORMAL;
    return DEVICE_NORMAL;
err:
    d->dev.device_status = DEVICE_FAIL;
    return DEVICE_FAIL;
unknown:
    d->dev.device_status = DEVICE_UNKNOWN;
    return DEVICE_UNKNOWN;
}

DeviceStatus dev_tqdc_clkmux_set(struct Dev_tqdc_clkmux *d)
{
    if (!dev_clkmux_set_internal(d)) {
        d->dev.device_status = DEVICE_FAIL;
    }
    return d->dev.device_status;
}
