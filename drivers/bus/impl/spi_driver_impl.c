/*
**    Generic interrupt mode SPI driver
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

#include "spi_driver_impl.h"

#include <assert.h>

#include "bus/bus_types.h"
#include "cmsis_os.h"
#include "error_handler.h"
#include "log/log.h"
#include "spi.h"
#include "spi_driver_util.h"
#include "stm32_hal.h"
#include "stm32_ll.h"

#define SPI_DRIVER_INTERRUPT_MODE 1

/*
#define HAL_SPI_ERROR_NONE              (0x00000000U)   //! No error
#define HAL_SPI_ERROR_MODF              (0x00000001U)   //! MODF error
#define HAL_SPI_ERROR_CRC               (0x00000002U)   //! CRC error
#define HAL_SPI_ERROR_OVR               (0x00000004U)   //! OVR error
#define HAL_SPI_ERROR_FRE               (0x00000008U)   //! FRE error
#define HAL_SPI_ERROR_DMA               (0x00000010U)   //! DMA transfer error
#define HAL_SPI_ERROR_FLAG              (0x00000020U)   //! Error on RXNE/TXE/BSY/FTLVL/FRLVL Flag
#define HAL_SPI_ERROR_ABORT             (0x00000040U)   //! Error during SPI Abort procedure
*/

void spi_driver_log_error(const char *title, struct __SPI_HandleTypeDef *hspi)
{
    log_printf(LOG_WARNING, "%s: SPI %d %s%s%s%s%s%s%s (error code 0x%04X)\n",
               title, hspi_index(hspi),
               (hspi->ErrorCode & HAL_SPI_ERROR_MODF)          ? " MODF" : "",
               (hspi->ErrorCode & HAL_SPI_ERROR_CRC)           ? " CRC" : "",
               (hspi->ErrorCode & HAL_SPI_ERROR_OVR)           ? " OVR" : "",
               (hspi->ErrorCode & HAL_SPI_ERROR_FRE)           ? " FRE" : "",
               (hspi->ErrorCode & HAL_SPI_ERROR_DMA)           ? " DMA_transfer" : "",
               (hspi->ErrorCode & HAL_SPI_ERROR_FLAG)          ? " flag_error" : "",
               (hspi->ErrorCode & HAL_SPI_ERROR_ABORT)         ? " Abort_error" : "",
               hspi->ErrorCode);
}

void spi_driver_reset_internal(struct __SPI_HandleTypeDef *handle)
{
    __HAL_SPI_DISABLE(handle);
    handle->ErrorCode = HAL_SPI_ERROR_NONE;
    handle->State = HAL_SPI_STATE_READY;
    __HAL_SPI_ENABLE(handle);
}

#if SPI_DRIVER_INTERRUPT_MODE

static bool spi_driver_wait_complete(const char *title, struct __SPI_HandleTypeDef *hspi, uint32_t millisec)
{
    int32_t status = spi_driver_wait_it_sem(hspi, millisec);
    if (status != osOK) {
        log_printf(LOG_CRIT, "%s: SPI %d timeout\n", title, hspi_index(hspi));
        int ret = HAL_SPI_Abort_IT(hspi);
        if (HAL_OK != ret) {
            log_printf(LOG_CRIT, "%s: SPI %d abort failed: %d\n", title, hspi_index(hspi), ret);
        }
        spi_driver_reset_internal(hspi);
        return false;
    }
    if (! spi_driver_is_transfer_ok(hspi)) {
        log_printf(LOG_CRIT, "%s: SPI %d transfer failed\n", title, hspi_index(hspi));
        return false;
    }
    return true;
}

static bool spi_driver_check_hal_ret(const char *title, struct __SPI_HandleTypeDef *hspi, int ret)
{
    if (HAL_OK == ret)
        return true;
    assert(ret != HAL_BUSY);
    spi_driver_log_error(title, hspi);
    return false;
}

static bool spi_driver_before_hal_call(const char *title, struct __SPI_HandleTypeDef *hspi)
{
    if (hspi->State == HAL_SPI_STATE_RESET) {
        log_printf(LOG_CRIT, "%s: SPI %d not initialized\n",
                   title, hspi_index(hspi));
        return false;
    }
    if (hspi->State != HAL_SPI_STATE_READY) {
        log_printf(LOG_CRIT, "%s: SPI %d not ready: %d\n",
                   title, hspi_index(hspi), hspi->State);
        return false;
    }
    assert(hspi->State == HAL_SPI_STATE_READY);
    spi_driver_clear_transfer_error(hspi);
    if (LL_SPI_IsActiveFlag_BSY(hspi->Instance)) {
        log_printf(LOG_CRIT, "%s: SPI %d bus busy\n",
                   title, hspi_index(hspi));
        spi_driver_reset_internal(hspi);
        return false;
    }
    return true;
}

static bool spi_driver_after_hal_call(const char *title, struct __SPI_HandleTypeDef *hspi, int ret, uint32_t millisec)
{
    return spi_driver_check_hal_ret(title, hspi, ret) &&
           spi_driver_wait_complete(title, hspi, millisec);
}


bool spi_driver_tx_rx_internal(struct __SPI_HandleTypeDef *hspi, uint8_t *txBuf, uint8_t *rxBuf, uint16_t Size, uint32_t millisec)
{
    if (!spi_driver_before_hal_call(__func__, hspi))
        return false;
    HAL_StatusTypeDef ret = HAL_SPI_TransmitReceive_IT(hspi, txBuf, rxBuf, Size);
    return spi_driver_after_hal_call(__func__, hspi, ret, millisec);
}

bool spi_driver_rx_internal(struct __SPI_HandleTypeDef *hspi, uint8_t *rxBuf, uint16_t Size, uint32_t millisec)
{
    if (!spi_driver_before_hal_call(__func__, hspi))
        return false;
    HAL_StatusTypeDef ret = HAL_SPI_Receive_IT(hspi, rxBuf, Size);
    return spi_driver_after_hal_call(__func__, hspi, ret, millisec);
}

bool spi_driver_tx_internal(struct __SPI_HandleTypeDef *hspi, uint8_t *txBuf, uint16_t Size, uint32_t millisec)
{
    if (!spi_driver_before_hal_call(__func__, hspi))
        return false;
    HAL_StatusTypeDef ret = HAL_SPI_Transmit_IT(hspi, txBuf, Size);
    return spi_driver_after_hal_call(__func__, hspi, ret, millisec);
}

#else

bool spi_driver_tx_rx_internal(struct __SPI_HandleTypeDef *hspi, uint8_t *txBuf, uint8_t *rxBuf, uint16_t Size, uint32_t millisec)
{
    if (!hspi)
        Error_Handler();
    HAL_StatusTypeDef ret = HAL_OK;
    ret = HAL_SPI_TransmitReceive(hspi, txBuf, rxBuf, Size, millisec);
    if (ret != HAL_OK) {
        log_printf(LOG_WARNING, "%s: SPI%d %s (code %d), %d\n", __func__, hspi_index(hspi),
                   (ret == HAL_BUSY) ? "busy" : "error", ret, hspi->ErrorCode);
        return false;
    }
    return true;
}

bool spi_driver_rx_internal(struct __SPI_HandleTypeDef *hspi, uint8_t *rxBuf, uint16_t Size, uint32_t millisec)
{
    if (!hspi)
        Error_Handler();
    HAL_StatusTypeDef ret = HAL_OK;
    ret = HAL_SPI_Receive(hspi, rxBuf, Size, millisec);
    if (ret != HAL_OK) {
        log_printf(LOG_WARNING, "%s: SPI%d %s (code %d), %d\n", __func__, hspi_index(hspi),
                   (ret == HAL_BUSY) ? "busy" : "error", ret, hspi->ErrorCode);
        return false;
    }
    return true;
}

bool spi_driver_tx_internal(struct __SPI_HandleTypeDef *hspi, uint8_t *txBuf, uint16_t Size, uint32_t millisec)
{
    if (!hspi)
        Error_Handler();
    HAL_StatusTypeDef ret = HAL_OK;
    ret = HAL_SPI_Transmit(hspi, txBuf, Size, millisec);
    if (ret != HAL_OK) {
        log_printf(LOG_WARNING, "%s: SPI%d %s (code %d), %d\n", __func__, hspi_index(hspi),
                   (ret == HAL_BUSY) ? "busy" : "error", ret, hspi->ErrorCode);
        return false;
    }
    return true;
}

#endif
