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
        uint8_t reserved: 2;
        uint8_t crsw_sin: 2;
        uint8_t crsw_sout: 2;
        uint8_t crsw_load: 1;
        uint8_t crsw_conf: 1;
    } bit;
    uint8_t all;
} clkmux_gpioa;

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
    return mcp23017_write(MCP23017_GPIOB, data.all);
}

enum {
    CRSW1_IN_VXS = 0,
    // CRSW1_IN_UNCONNECTED = 1,
    CRSW1_IN_AD9516 = 2,
    CRSW1_IN_PLL0B_CRSWQ2 = 3,
};
/*
enum {
    CRSW2_IN_PLL0C = 0,
    CRSW2_IN_PLL1B = 1,
    CRSW2_IN_FMC_GBT = 2,
    CRSW2_IN_AD9516 = 3,
};
*/
static bool dev_clkmux_set_crsw1(Dev_cru16_clkmux *d)
{
    bool ok = true;
    clkmux_gpiob data;
    data.all = 0;
    data.bit.pll_bypass = 0;
    ok &= mcp23017_write(MCP23017_GPIOB, data.all);
    int crsw1_output_map[4] = {
        CRSW1_IN_AD9516,
        CRSW1_IN_PLL0B_CRSWQ2,
        CRSW1_IN_AD9516,
        CRSW1_IN_AD9516 // Q3 unused
    };
    for (int i=0; i<4; i++) {
        data.bit.crsw_sin = crsw1_output_map[i];
        data.bit.crsw_sout = i;
        ok &= mcp23017_write(MCP23017_GPIOB, data.all);
        data.bit.crsw_load = 1;
        ok &= mcp23017_write(MCP23017_GPIOB, data.all);
        data.bit.crsw_load = 0;
        ok &= mcp23017_write(MCP23017_GPIOB, data.all);
        data.bit.crsw_conf = 1;
        ok &= mcp23017_write(MCP23017_GPIOB, data.all);
        data.bit.crsw_conf = 0;
        ok &= mcp23017_write(MCP23017_GPIOB, data.all);
    }
    return ok;
}

/*
static void dev_clkmux_set_crsw2(Dev_cru16_clkmux *d)
{
    clkmux_gpioa data;
    data.all = 0;
    int crsw2_output_map[4] = {
        CRSW2_IN_AD9516,
        CRSW2_IN_AD9516,
        CRSW2_IN_AD9516,
        CRSW2_IN_AD9516
    };
    for (int i=0; i<4; i++) {
        data.bit.crsw_sin = crsw2_output_map[i];
        data.bit.crsw_sout = i;
        mcp23017_write(MCP23017_GPIOA, data.all);
        data.bit.crsw_load = 1;
        mcp23017_write(MCP23017_GPIOA, data.all);
        data.bit.crsw_load = 0;
        mcp23017_write(MCP23017_GPIOA, data.all);
        data.bit.crsw_conf = 1;
        mcp23017_write(MCP23017_GPIOA, data.all);
        data.bit.crsw_conf = 0;
        mcp23017_write(MCP23017_GPIOA, data.all);
    }
}
*/

DeviceStatus dev_cru16_clkmux_detect(Dev_cru16_clkmux *d)
{
    if (!mcp23017_detect()) {
        goto unknown;
    }
//    uint8_t data = 0x55;
//    if (! mcp23017_read(MCP23017_IODIRB, &data))
//        goto err;
//    if (data != 0xFF) {
//        log_put(LOG_ERR, "clkmux: bad default value for register 1");
//        goto err;
//    }
//    if (! mcp23017_read(MCP23017_IPOLA, &data))
//        return DEVICE_FAIL;
//    if (data != 0x00) {
//        log_put(LOG_ERR, "clkmux: bad default value for register 2");
//        goto err;
//    }
    // set GPB1,GPB0
    if (! mcp23017_write(MCP23017_IODIRA, 0x00)) // 0 = output
        goto err;
    if (! mcp23017_write(MCP23017_IODIRB, 0x00)) // 0 = output
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

DeviceStatus dev_cru16_clkmux_set(struct Dev_cru16_clkmux *d)
{
    bool ok = true;
    ok &= dev_clkmux_set_pll_source(d);
    ok &= dev_clkmux_set_crsw1(d);
//    dev_clkmux_set_crsw2(d);
    return ok ? DEVICE_NORMAL : DEVICE_FAIL;
}
