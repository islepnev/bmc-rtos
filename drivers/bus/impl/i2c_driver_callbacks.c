/*
**    Generic interrupt mode IIC driver
**
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

#include "i2c_driver_callbacks.h"

#include "cmsis_os.h"
#include "log/logbuffer.h"
#include "error_handler.h"
#include "i2c.h"
#include "i2c_driver_util.h"
#include "i2c_driver_impl.h"

void HAL_I2C_MasterTxCpltCallback(struct __I2C_HandleTypeDef *hi2c)
{
    release_it_sem(hi2c);
}

void HAL_I2C_MasterRxCpltCallback(struct __I2C_HandleTypeDef *hi2c)
{
    release_it_sem(hi2c);
}

void HAL_I2C_MemTxCpltCallback(struct __I2C_HandleTypeDef *hi2c)
{
    release_it_sem(hi2c);
}

void HAL_I2C_MemRxCpltCallback(struct __I2C_HandleTypeDef *hi2c)
{
    release_it_sem(hi2c);
}

void HAL_I2C_ErrorCallback(struct __I2C_HandleTypeDef *hi2c)
{
    log_printf(LOG_WARNING, "%s I2C%d error, code %d\n", __func__, hi2c_index(hi2c), hi2c->ErrorCode);
    // reinitialize I2C
    i2c_driver_reset_internal(hi2c);
    release_it_sem(hi2c);
}

void HAL_I2C_AbortCpltCallback(struct __I2C_HandleTypeDef *hi2c)
{
    log_printf(LOG_WARNING, "%s I2C%d\n", __func__, hi2c_index(hi2c));
    release_it_sem(hi2c);
}
