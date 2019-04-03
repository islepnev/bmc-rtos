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

#include "dev_fpga.h"

#include "stm32f7xx_hal.h"
#include "spi.h"
#include "fpga_spi_hal.h"
#include "version.h"
#include "app_shared_data.h"
#include "logbuffer.h"
#include "dev_fpga_types.h"
#include "dev_pll_types.h"
#include "dev_thset_types.h"

static HAL_StatusTypeDef fpga_test_reg(uint16_t addr, uint16_t wdata, uint16_t *rdata)
{
    HAL_StatusTypeDef ret = HAL_OK;
    ret = fpga_spi_hal_write_reg(addr, wdata);
    if (ret != HAL_OK)
        return ret;
    ret = fpga_spi_hal_read_reg(addr, rdata);
    if (ret != HAL_OK)
        return ret;
    return ret;
}

DeviceStatus fpgaDetect(Dev_fpga *d)
{
    uint16_t data[2] = {0,0};
    int err = 0;
    for (int i=0; i<2; i++) {
        if (HAL_OK != fpga_spi_hal_read_reg(i, &data[i])) {
            err++;
            break;
        }
    }
    uint16_t id = data[0];
    if (id == 0x0000 || id == 0xFFFF)
        err++;
    if (err == 0)
        d->present = DEVICE_NORMAL;
    else
        d->present = DEVICE_FAIL;

    d->id = id;
    if (0) {
        uint16_t addr = 0x0010;
        uint16_t wdata = 0x5aa5;
        uint16_t rdata = 0;
        fpga_test_reg(addr, wdata, &rdata);
        if (rdata != wdata)
            log_printf(LOG_ERR, "FPGA register test failed: addr %04X, wdata %04X, rdata %04X", addr, wdata, rdata);
    }
    return d->present;
}

HAL_StatusTypeDef fpgaWriteBmcVersion(void)
{
    HAL_StatusTypeDef ret = HAL_OK;
    ret = fpga_spi_hal_write_reg(FPGA_SPI_ADDR_0, VERSION_MAJOR_NUM);
    if (ret != HAL_OK)
        return ret;
    ret = fpga_spi_hal_write_reg(FPGA_SPI_ADDR_7, VERSION_MINOR_NUM);
    if (ret != HAL_OK)
        return ret;
    return ret;
}

HAL_StatusTypeDef fpgaWriteBmcTemperature(const struct Dev_thset *thset)
{
    HAL_StatusTypeDef ret = HAL_OK;
    for (int i=0; i<4; i++) {
        ret = fpga_spi_hal_write_reg(FPGA_SPI_ADDR_3 + i, thset->th[i].rawTemp);
        if (ret != HAL_OK)
            return ret;
    }
    return ret;
}

HAL_StatusTypeDef fpgaWritePllStatus(const struct Dev_pll *pll)
{
    HAL_StatusTypeDef ret = HAL_OK;
    uint16_t data = 0;
    if ((DEVICE_NORMAL != pll->present) || (pll->fsm_state != PLL_STATE_RUN) || (!pll->status.sysclk.b.locked))
        data |= 0x8;
    else {
        if (pll->status.sysclk.b.pll0_locked)
            data |= 0x1;
    }
    ret = fpga_spi_hal_write_reg(FPGA_SPI_ADDR_1, data);
    return ret;
}
