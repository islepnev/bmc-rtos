/*
**    Interrupt mode SPI driver
**
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

#include "spi_driver.h"

#include "cmsis_os.h"
#include "error_handler.h"
#include "impl/spi_driver_impl.h"
#include "impl/spi_driver_util.h"
#include "log/log.h"
#include "spi.h"
#include "stm32_hal.h"

void spi_driver_init(void)
{
    if (! spi_driver_util_init())
        Error_Handler();
}

bool spi_driver_get_master_ready(struct __SPI_HandleTypeDef *hspi)
{
    return  HAL_SPI_STATE_READY == HAL_SPI_GetState(hspi);
}

// Device-locked functions

#if SPI_DRIVER_INTERRUPT_MODE
bool spi_driver_tx_rx(struct __SPI_HandleTypeDef *hspi, uint8_t *txBuf, uint8_t *rxBuf, uint16_t Size, uint32_t millisec)
{
    int dev_index = hspi_index(hspi);
    if (osOK != spi_driver_wait_dev_mutex(dev_index, osWaitForever))
        return false;
    bool ret = spi_driver_tx_rx_internal(hspi, txBuf, rxBuf, Size, millisec);
    spi_driver_release_dev_mutex(dev_index);
    return ret;
}

bool spi_driver_rx(struct __SPI_HandleTypeDef *hspi, uint8_t *rxBuf, uint16_t Size, uint32_t millisec)
{
    int dev_index = hspi_index(hspi);
    if (osOK != spi_driver_wait_dev_mutex(dev_index, osWaitForever))
        return false;
    bool ret = spi_driver_rx_internal(hspi, rxBuf, Size, millisec);
    spi_driver_release_dev_mutex(dev_index);
    return ret;
}

bool spi_driver_tx(struct __SPI_HandleTypeDef *hspi, uint8_t *txBuf, uint16_t Size, uint32_t millisec)
{
    int dev_index = hspi_index(hspi);
    if (osOK != spi_driver_wait_dev_mutex(dev_index, osWaitForever))
        return false;
    bool ret = spi_driver_tx_internal(hspi, txBuf, Size, millisec);
    spi_driver_release_dev_mutex(dev_index);
    return ret;
}

#else
bool spi_driver_tx_rx(struct __SPI_HandleTypeDef *hspi, uint8_t *txBuf, uint8_t *rxBuf, uint16_t Size, uint32_t millisec)
{
    if (!hspi)
        Error_Handler();
    int dev_index = hspi_index(hspi);
    if (osOK != spi_driver_wait_dev_mutex(dev_index, osWaitForever))
        return false;
    HAL_StatusTypeDef ret = HAL_OK;
    ret = HAL_SPI_TransmitReceive(hspi, txBuf, rxBuf, Size, millisec);
    spi_driver_release_dev_mutex(dev_index);
    spi_driver_check_hal_ret(__func__, hspi, ret);
    return HAL_OK == ret;
}

bool spi_driver_rx(struct __SPI_HandleTypeDef *hspi, uint8_t *rxBuf, uint16_t Size, uint32_t millisec)
{
    if (!hspi)
        Error_Handler();
    int dev_index = hspi_index(hspi);
    if (osOK != spi_driver_wait_dev_mutex(dev_index, osWaitForever))
        return false;
    HAL_StatusTypeDef ret = HAL_OK;
    ret = HAL_SPI_Receive(hspi, rxBuf, Size, millisec);
    spi_driver_release_dev_mutex(dev_index);
    spi_driver_check_hal_ret(__func__, hspi, ret);
    return HAL_OK == ret;
}

bool spi_driver_tx(struct __SPI_HandleTypeDef *hspi, uint8_t *txBuf, uint16_t Size, uint32_t millisec)
{
    if (!hspi)
        Error_Handler();
    int dev_index = hspi_index(hspi);
    if (osOK != spi_driver_wait_dev_mutex(dev_index, osWaitForever))
        return false;
    HAL_StatusTypeDef ret = HAL_OK;
    ret = HAL_SPI_Transmit(hspi, txBuf, Size, millisec);
    spi_driver_release_dev_mutex(dev_index);
    spi_driver_check_hal_ret(__func__, hspi, ret);
    return HAL_OK == ret;
}
#endif

void spi_enable_interface(struct __SPI_HandleTypeDef *hspi, bool enable)
{
    int dev_index = hspi_index(hspi);
    if (osOK != spi_driver_wait_dev_mutex(dev_index, osWaitForever))
        return;
    if (enable) {
        log_printf(LOG_INFO, "Enabling SPI%d", dev_index);
        HAL_SPI_Init(hspi);
    } else {
        log_printf(LOG_INFO, "Disabling SPI%d", dev_index);
        // HAL_SPI_Abort(hspi);
        HAL_SPI_DeInit(hspi);
    }
    spi_driver_release_dev_mutex(dev_index);
}
