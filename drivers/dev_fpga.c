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

HAL_StatusTypeDef fpgaWriteBmcTemperature(const Dev_thset *thset)
{
    HAL_StatusTypeDef ret = HAL_OK;
    for (int i=0; i<4; i++) {
        ret = fpga_spi_hal_write_reg(FPGA_SPI_ADDR_3 + i, thset->th[i].rawTemp);
        if (ret != HAL_OK)
            return ret;
    }
    return ret;
}