/*
**    Copyright 2019-2020 Ilja Slepnev
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

#include "bsp_fpga.h"

#include "fpga_spi_hal.h"
#include "dev_pm_sensors_config.h"
#include "powermon/dev_powermon_types.h"

bool fpgaWriteSensors(void)
{
    const Dev_powermon_priv *p = get_powermon_priv_const();
    if (!p)
        return false;
    const pm_sensors_arr *sensors = &p->sensors;
    uint16_t address = FPGA_SPI_ADDR_0 + 0x10;
    if (! fpga_spi_hal_write_reg(address++, (int16_t)(sensors->arr[SENSOR_VME_5V].priv.busVoltage * 1000)))
        return false;
    if (! fpga_spi_hal_write_reg(address++, (int16_t)(sensors->arr[SENSOR_VME_5V].priv.current * 1000)))
        return false;
    if (! fpga_spi_hal_write_reg(address++, (int16_t)(sensors->arr[SENSOR_3V3].priv.busVoltage * 1000)))
        return false;
    if (! fpga_spi_hal_write_reg(address++, (int16_t)(sensors->arr[SENSOR_3V3].priv.current * 1000)))
        return false;
    return true;
}
