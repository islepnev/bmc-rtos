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

#include "dev_clkmux.h"
#include <string.h>
#include <stdint.h>
#include "stm32f7xx_hal_def.h"

#include "dev_clkmux_types.h"
#include "mcp23017_i2c_hal.h"
#include "logbuffer.h"

/*
#include "dev_vxsiic_pp.h"
#include "vxsiic_iic_driver.h"
#include "vxsiic_hal.h"
#include "logbuffer.h"
#include "debug_helpers.h"
#include "ipmi_sensor_types.h"
#include "cmsis_os.h"
*/

void dev_clkmux_init(Dev_clkmux *d)
{
    Dev_clkmux zz = {0};
    *d = zz;
}

typedef enum {
    MCP23017_IODIRA = 0,
    MCP23017_IODIRB = 1,
    MCP23017_IPOLA = 2,
    MCP23017_IPOLB = 3,
    MCP23017_GPINTENA = 4,
    MCP23017_GPINTENB = 5,
    MCP23017_GPPUA = 0xC,
    MCP23017_GPPUB = 0xD,
    MCP23017_GPIOA = 0x12,
    MCP23017_GPIOB = 0x13,
    MCP23017_OLATA = 0x14,
    MCP23017_OLATB = 0x15,
} mcp23017_regs_bank_0;


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
        uint8_t pll_source_sel: 2;
        uint8_t crsw_sin: 2;
        uint8_t crsw_sout: 2;
        uint8_t crsw_load: 1;
        uint8_t crsw_conf: 1;
    } bit;
    uint8_t all;
} clkmux_gpiob;

void dev_clkmux_set_pll_source(Dev_clkmux *d)
{
    clkmux_gpiob data;
    data.all = 0;
    data.bit.pll_source_sel = d->pll_source & 0x3;
    mcp23017_write(MCP23017_GPIOB, data.all);
}

enum {
    CRSW1_IN_PLL0A = 0,
    CRSW1_IN_PLL0B = 1,
    CRSW1_IN_FMC = 2,
    CRSW1_IN_AD9516_DIV3 = 3,
};

enum {
    CRSW2_IN_PLL0C = 0,
    CRSW2_IN_PLL1B = 1,
    CRSW2_IN_FMC_GBT = 2,
    CRSW2_IN_AD9516 = 3,
};

void dev_clkmux_set_crsw1(Dev_clkmux *d)
{
    clkmux_gpiob data;
    data.all = 0;
    data.bit.pll_source_sel = 0;//d->pll_source & 0x3;
    mcp23017_write(MCP23017_GPIOB, data.all);
    int crsw1_output_map[4] = {
        2,
        3,
        2,
        2
    };
    //while (1)
    for (int i=0; i<4; i++) {
        data.bit.crsw_sin = crsw1_output_map[i]; // CRSW1_IN_AD9516_DIV3;
        data.bit.crsw_sout = i;
        mcp23017_write(MCP23017_GPIOB, data.all);
        data.bit.crsw_load = 1;
        mcp23017_write(MCP23017_GPIOB, data.all);
        data.bit.crsw_load = 0;
        mcp23017_write(MCP23017_GPIOB, data.all);
        data.bit.crsw_conf = 1;
        mcp23017_write(MCP23017_GPIOB, data.all);
        data.bit.crsw_conf = 0;
        mcp23017_write(MCP23017_GPIOB, data.all);
    }
}

void dev_clkmux_set_crsw2(Dev_clkmux *d)
{
    clkmux_gpioa data;
    data.all = 0;
    int crsw2_output_map[4] = {
        2,
        2,
        2,
        2
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

DeviceStatus dev_clkmux_detect(Dev_clkmux *d)
{
    if (HAL_OK != mcp23017_detect()) {
        goto unknown;
    }
//    uint8_t data = 0x55;
//    if (HAL_OK != mcp23017_read(MCP23017_IODIRB, &data))
//        goto err;
//    if (data != 0xFF) {
//        log_put(LOG_ERR, "clkmux: bad default value for register 1");
//        goto err;
//    }
//    if (HAL_OK != mcp23017_read(MCP23017_IPOLA, &data))
//        return DEVICE_FAIL;
//    if (data != 0x00) {
//        log_put(LOG_ERR, "clkmux: bad default value for register 2");
//        goto err;
//    }
    // set GPB1,GPB0
    if (HAL_OK != mcp23017_write(MCP23017_IODIRA, 0x00)) // 0 = output
        goto err;
    if (HAL_OK != mcp23017_write(MCP23017_IODIRB, 0x00)) // 0 = output
        goto err;

    d->present = DEVICE_NORMAL;
    return DEVICE_NORMAL;
err:
    d->present = DEVICE_FAIL;
    return DEVICE_FAIL;
unknown:
    d->present = DEVICE_UNKNOWN;
    return DEVICE_UNKNOWN;
}

DeviceStatus dev_clkmux_set(struct Dev_clkmux *d)
{
    dev_clkmux_set_pll_source(d);
    dev_clkmux_set_crsw1(d);
    dev_clkmux_set_crsw2(d);
    return DEVICE_NORMAL;
}
