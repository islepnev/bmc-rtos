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

#include "spi.h"
#include "debug_helpers.h"
#include "cmsis_os.h"
#include "error_handler.h"

#define USE_INTERRUPT_MODE_SPI

osSemaphoreId spi1_sem;
osSemaphoreId spi2_sem;
osSemaphoreId spi3_sem;
osSemaphoreId spi4_sem;
osSemaphoreId spi5_sem;
osSemaphoreDef(spi1_sem);
osSemaphoreDef(spi2_sem);
osSemaphoreDef(spi3_sem);
osSemaphoreDef(spi4_sem);
osSemaphoreDef(spi5_sem);

void spi_driver_init(void)
{
    // Create and take the semaphore
    spi1_sem = osSemaphoreCreate(osSemaphore(spi1_sem), 1);
    spi2_sem = osSemaphoreCreate(osSemaphore(spi2_sem), 1);
    spi3_sem = osSemaphoreCreate(osSemaphore(spi3_sem), 1);
    spi4_sem = osSemaphoreCreate(osSemaphore(spi4_sem), 1);
    spi5_sem = osSemaphoreCreate(osSemaphore(spi5_sem), 1);
    if (NULL == spi1_sem
            || NULL == spi2_sem
            || NULL == spi3_sem
            || NULL == spi4_sem
            || NULL == spi5_sem
            ) {
        Error_Handler();
    }

    osSemaphoreWait(spi1_sem, osWaitForever);
    osSemaphoreWait(spi2_sem, osWaitForever);
    osSemaphoreWait(spi3_sem, osWaitForever);
    osSemaphoreWait(spi4_sem, osWaitForever);
    osSemaphoreWait(spi5_sem, osWaitForever);
}

void spi_driver_reset(struct __SPI_HandleTypeDef *handle)
{
    __HAL_SPI_DISABLE(handle);
    handle->ErrorCode = HAL_SPI_ERROR_NONE;
    handle->State = HAL_SPI_STATE_READY;
    __HAL_SPI_ENABLE(handle);
}

static SemaphoreHandle_t sem_by_hspi(struct __SPI_HandleTypeDef *hspi)
{
    if (hspi == &hspi1)
        return spi1_sem;
    if (hspi == &hspi2)
        return spi2_sem;
    if (hspi == &hspi3)
        return spi3_sem;
    if (hspi == &hspi4)
        return spi4_sem;
    if (hspi == &hspi5)
        return spi5_sem;
    return NULL;
}

int hspi_index(struct __SPI_HandleTypeDef *hspi)
{
    if (hspi == &hspi1)
        return 1;
    if (hspi == &hspi2)
        return 2;
    if (hspi == &hspi3)
        return 3;
    if (hspi == &hspi4)
        return 4;
    if (hspi == &hspi5)
        return 5;
    return 0;
}

static int32_t spi_driver_wait_sem(struct __SPI_HandleTypeDef *hspi, uint32_t millisec)
{
    SemaphoreHandle_t sem = sem_by_hspi(hspi);
    if (sem) {
        osStatus result = osSemaphoreWait(sem, millisec);
        if (result != osOK) {
            debug_printf("%s: SPI%d error %d\n", __func__, hspi_index(hspi), result);
        }
        return result;
    }
    else {
        Error_Handler();
        return -1;
    }
}

void spi_driver_release_sem(struct __SPI_HandleTypeDef *hspi)
{
    SemaphoreHandle_t sem = sem_by_hspi(hspi);
    if (sem) {
        osStatus result = osSemaphoreRelease(sem);
        if (result != osOK) {
            debug_printf("%s: SPI%d error %d\n", __func__, hspi_index(hspi), result);
        }
    } else {
        Error_Handler();
    }
}

void HAL_SPI_TxCpltCallback(struct __SPI_HandleTypeDef *hspi)
{
    spi_driver_release_sem(hspi);
}

void HAL_SPI_RxCpltCallback(struct __SPI_HandleTypeDef *hspi)
{
    spi_driver_release_sem(hspi);
}

void HAL_SPI_TxRxCpltCallback(struct __SPI_HandleTypeDef *hspi)
{
    spi_driver_release_sem(hspi);
}

void HAL_SPI_ErrorCallback(struct __SPI_HandleTypeDef *hspi)
{
    if (!hspi)
        Error_Handler();
    debug_printf("%s SPI%d error, code %d\n", __func__, hspi_index(hspi), hspi->ErrorCode);
    // reinitialize SPI
    spi_driver_reset(hspi);
    spi_driver_release_sem(hspi);
}
#ifdef USE_INTERRUPT_MODE_SPI
HAL_StatusTypeDef spi_driver_tx_rx(struct __SPI_HandleTypeDef *hspi, uint8_t *txBuf, uint8_t *rxBuf, uint16_t Size, uint32_t millisec)
{
    if (!hspi)
        Error_Handler();
    HAL_StatusTypeDef ret = HAL_OK;
    ret = HAL_SPI_TransmitReceive_IT(hspi, txBuf, rxBuf, Size);
    if (ret != HAL_OK) {
        debug_printf("%s: SPI%d %s (code %d), %d\n", __func__, hspi_index(hspi),
                     (ret == HAL_BUSY) ? "busy" : "error", ret, hspi->ErrorCode);
        return ret;
    }
    osStatus status = spi_driver_wait_sem(hspi, millisec);
    if (status != osOK) {
        debug_printf("%s: SPI%d timeout\n", __func__, hspi_index(hspi));
        return HAL_TIMEOUT;
    }
    return ret;
}

HAL_StatusTypeDef spi_driver_tx(struct __SPI_HandleTypeDef *hspi, uint8_t *txBuf, uint16_t Size, uint32_t millisec)
{
    if (!hspi)
        Error_Handler();
    HAL_StatusTypeDef ret = HAL_OK;
    ret = HAL_SPI_Transmit_IT(hspi, txBuf, Size);
    if (ret != HAL_OK) {
        debug_printf("%s: SPI%d %s (code %d), %d\n", __func__, hspi_index(hspi),
                     (ret == HAL_BUSY) ? "busy" : "error", ret, hspi->ErrorCode);
        return ret;
    }
    osStatus status = spi_driver_wait_sem(hspi, millisec);
    if (status != osOK) {
        debug_printf("%s: SPI%d timeout\n", __func__, hspi_index(hspi));
        return HAL_TIMEOUT;
    }
    return ret;
}
#else
HAL_StatusTypeDef spi_driver_tx_rx(struct __SPI_HandleTypeDef *hspi, uint8_t *txBuf, uint8_t *rxBuf, uint16_t Size, uint32_t millisec)
{
    if (!hspi)
        Error_Handler();
    HAL_StatusTypeDef ret = HAL_OK;
    ret = HAL_SPI_TransmitReceive(hspi, txBuf, rxBuf, Size, millisec);
    if (ret != HAL_OK) {
        debug_printf("%s: SPI%d error %d, %d\n", __func__, hspi_index(hspi), ret, hspi->ErrorCode);
    }
    return ret;
}

HAL_StatusTypeDef spi_driver_tx(struct __SPI_HandleTypeDef *hspi, uint8_t *txBuf, uint16_t Size, uint32_t millisec)
{
    if (!hspi)
        Error_Handler();
    HAL_StatusTypeDef ret = HAL_OK;
    ret = HAL_SPI_Transmit(hspi, txBuf, Size, millisec);
    if (ret != HAL_OK) {
        debug_printf("%s: SPI%d error %d, %d\n", __func__, hspi_index(hspi), ret, hspi->ErrorCode);
    }
    return ret;
}
#endif
