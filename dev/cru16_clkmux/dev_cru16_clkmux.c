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

#include "dev_cru16_clkmux.h"

#include <string.h>
#include <stdint.h>

#include "dev_cru16_clkmux_types.h"
#include "mcp23017/mcp23017_i2c_hal.h"

typedef union {
    struct {
        uint8_t pll_bypass: 1;
        uint8_t unused1: 1;
        uint8_t crsw_sin: 2;
        uint8_t crsw_sout: 2;
        uint8_t crsw_load: 1;
        uint8_t crsw_conf: 1;
    } bit;
    uint8_t all;
} clkmux_gpiob;

static bool dev_clkmux_set_pll_source(Dev_cru16_clkmux *d)
{
    clkmux_gpiob data;
    data.all = 0;
    data.bit.pll_bypass = 0;
    return mcp23017_write(&d->dev, MCP23017_GPIOB, data.all);
}

enum {
    CRSW1_IN_VXS = 0,
    // CRSW1_IN_UNCONNECTED = 1,
    CRSW1_IN_AD9516 = 2,
    CRSW1_IN_PLL0B_CRSWQ2 = 3,
};

static bool dev_clkmux_set_crsw1(Dev_cru16_clkmux *d)
{
    clkmux_gpiob data;
    data.all = 0;
    data.bit.pll_bypass = 0;
    if (!mcp23017_write(&d->dev, MCP23017_GPIOB, data.all))
        goto err;
    int clock_source = (d->priv.pll_source == CRU16_PLL_SOURCE_VXS) ? CRSW1_IN_VXS : CRSW1_IN_AD9516;
    int crsw1_output_map[4] = {
        clock_source,
        CRSW1_IN_PLL0B_CRSWQ2,
        clock_source,
        clock_source // Q3 unused
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

bool dev_cru16_clkmux_detect(Dev_cru16_clkmux *d)
{
    if (!mcp23017_detect(&d->dev)) {
        goto err;
    }
//    uint8_t data = 0x55;
//    if (! mcp23017_read(MCP23017_IODIRB, &data))
//        goto err;
//    if (data != 0xFF) {
//        log_put(LOG_ERR, "clkmux: bad default value for register 1");
//        goto err;
//    }
//    if (! mcp23017_read(MCP23017_IPOLA, &data))
//        goto err;
//    if (data != 0x00) {
//        log_put(LOG_ERR, "clkmux: bad default value for register 2");
//        goto err;
//    }
    // set GPB1,GPB0
    if (! mcp23017_write(&d->dev, MCP23017_IODIRA, 0x00)) // 0 = output
        goto err;
    if (! mcp23017_write(&d->dev, MCP23017_IODIRB, 0x00)) // 0 = output
        goto err;

    return true;
err:
    return false;
}

bool dev_cru16_clkmux_set(struct Dev_cru16_clkmux *d)
{
    if (!dev_clkmux_set_pll_source(d))
        goto err;
    if (!dev_clkmux_set_crsw1(d))
        goto err;

    return true;
err:
    return false;
}
