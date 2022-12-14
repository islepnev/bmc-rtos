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

#include "spi_driver_util.h"

#include <assert.h>

#include "cmsis_os.h"
#include "error_handler.h"
#include "log/log.h"
#include "spi.h"
#include "stm32_hal.h"

extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi2;
extern SPI_HandleTypeDef hspi3;
#ifndef STM32F3
extern SPI_HandleTypeDef hspi4;
extern SPI_HandleTypeDef hspi5;
#endif
enum {SPI_BUS_COUNT = 5};

// interrupt wait semaphores
osSemaphoreId spi1_it_sem;
osSemaphoreId spi2_it_sem;
osSemaphoreId spi3_it_sem;
osSemaphoreId spi4_it_sem;
osSemaphoreId spi5_it_sem;

// device mutexes
osMutexId spi1_dev_mutex;
osMutexId spi2_dev_mutex;
osMutexId spi3_dev_mutex;
osMutexId spi4_dev_mutex;
osMutexId spi5_dev_mutex;

osSemaphoreDef(spi1_it_sem);
osSemaphoreDef(spi2_it_sem);
osSemaphoreDef(spi3_it_sem);
osSemaphoreDef(spi4_it_sem);
osSemaphoreDef(spi5_it_sem);

osMutexDef(spi1_dev_mutex);
osMutexDef(spi2_dev_mutex);
osMutexDef(spi3_dev_mutex);
osMutexDef(spi4_dev_mutex);
osMutexDef(spi5_dev_mutex);

// transfer error flags
static bool spi_driver_transfer_error[SPI_BUS_COUNT] = {0};


void spi_driver_clear_transfer_error(struct __SPI_HandleTypeDef *hspi)
{
    int index = hspi_index(hspi);
    if (index == 0 || index > SPI_BUS_COUNT) return;
    spi_driver_transfer_error[index-1] = 0;
}

void spi_driver_raise_transfer_error(struct __SPI_HandleTypeDef *hspi)
{
    int index = hspi_index(hspi);
    if (index == 0 || index > SPI_BUS_COUNT) return;
    spi_driver_transfer_error[index-1] = true;
}

bool spi_driver_is_transfer_ok(struct __SPI_HandleTypeDef *hspi)
{
    int index = hspi_index(hspi);
    if (index == 0 || index > SPI_BUS_COUNT) return false;
    return 0 == spi_driver_transfer_error[index-1];
}

bool spi_driver_util_init(void)
{
    // Create and take the interrupt wait semaphore
    spi1_it_sem = osSemaphoreCreate(osSemaphore(spi1_it_sem), 1);
    spi2_it_sem = osSemaphoreCreate(osSemaphore(spi2_it_sem), 1);
    spi3_it_sem = osSemaphoreCreate(osSemaphore(spi3_it_sem), 1);
    spi4_it_sem = osSemaphoreCreate(osSemaphore(spi4_it_sem), 1);
    spi5_it_sem = osSemaphoreCreate(osSemaphore(spi5_it_sem), 1);
    if (NULL == spi1_it_sem
        || NULL == spi2_it_sem
        || NULL == spi3_it_sem
        || NULL == spi4_it_sem
        || NULL == spi5_it_sem
        ) {
        assert(false);
        return false;
    }
    if ((xSemaphoreTake(spi1_it_sem, 0) != pdTRUE) ||
        (xSemaphoreTake(spi2_it_sem, 0) != pdTRUE) ||
        (xSemaphoreTake(spi3_it_sem, 0) != pdTRUE) ||
        (xSemaphoreTake(spi4_it_sem, 0) != pdTRUE) ||
        (xSemaphoreTake(spi5_it_sem, 0) != pdTRUE)
        ) {
        assert(false);
        return false;
    }

    // create device mutexes
    spi1_dev_mutex = osMutexCreate(osMutex(spi1_dev_mutex));
    spi2_dev_mutex = osMutexCreate(osMutex(spi2_dev_mutex));
    spi3_dev_mutex = osMutexCreate(osMutex(spi3_dev_mutex));
    spi4_dev_mutex = osMutexCreate(osMutex(spi4_dev_mutex));
    spi5_dev_mutex = osMutexCreate(osMutex(spi5_dev_mutex));
    if (NULL == spi1_dev_mutex
        || NULL == spi2_dev_mutex
        || NULL == spi3_dev_mutex
        || NULL == spi4_dev_mutex
        || NULL == spi5_dev_mutex
        ) {
        assert(false);
        return false;
    }
    return true;
}

SemaphoreHandle_t spi_driver_it_sem_by_hspi(struct __SPI_HandleTypeDef *hspi)
{
    if (hspi == &hspi1)
        return spi1_it_sem;
    if (hspi == &hspi2)
        return spi2_it_sem;
    if (hspi == &hspi3)
        return spi3_it_sem;
#ifndef STM32F3
    if (hspi == &hspi4)
        return spi4_it_sem;
    if (hspi == &hspi5)
        return spi5_it_sem;
#endif
    assert(false);
    return NULL;
}

osMutexId spi_driver_dev_mutex_by_index(int index)
{
    assert(index >=1 && index <= SPI_BUS_COUNT);
    switch (index) {
    case 1: return spi1_dev_mutex;
    case 2: return spi2_dev_mutex;
    case 3: return spi3_dev_mutex;
    case 4: return spi4_dev_mutex;
    case 5: return spi5_dev_mutex;
    default: return NULL;
    }
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
#ifndef STM32F3
    if (hspi == &hspi4)
        return 4;
    if (hspi == &hspi5)
        return 5;
#endif
    assert(false);
    return 0;
}

struct __SPI_HandleTypeDef * hspi_handle(BusIndex index)
{
    if (1 == index)
        return &hspi1;
    if (2 == index)
        return &hspi2;
    if (3 == index)
        return &hspi3;
#ifndef STM32F3
    if (4 == index)
        return &hspi4;
    if (5 == index)
        return &hspi5;
#endif
    assert(false);
    return NULL;
}

inline void *spi_instance(int index)
{
#ifdef SPI1
    if (index == 1)
        return SPI1;
#endif
#ifdef SPI2
    if (index == 2)
        return SPI2;
#endif
#ifdef SPI3
    if (index == 3)
        return SPI3;
#endif
#ifdef SPI4
    if (index == 4)
        return SPI4;
#endif
#ifdef SPI5
    if (index == 5)
        return SPI5;
#endif
    assert(false);
    return NULL;
}

int32_t spi_driver_wait_it_sem(struct __SPI_HandleTypeDef *hspi, uint32_t millisec)
{
    SemaphoreHandle_t sem = spi_driver_it_sem_by_hspi(hspi);
    if (sem)
        return osSemaphoreWait(sem, millisec);
    else
        return osErrorValue;
}

void spi_driver_release_it_sem(struct __SPI_HandleTypeDef *hspi)
{
    SemaphoreHandle_t sem = spi_driver_it_sem_by_hspi(hspi);
    if (sem)
        osSemaphoreRelease(sem);
}

int32_t spi_driver_wait_dev_mutex(int index, uint32_t millisec)
{
    osMutexId m = spi_driver_dev_mutex_by_index(index);
    assert(m);
    if (!m)
        return osErrorValue;
    int32_t ret =  osMutexWait(m, millisec);
    if (ret)
        log_printf(LOG_WARNING, "spi%d device lock error\n", index);
    return  ret;
}

void spi_driver_release_dev_mutex(int index)
{
    osMutexId m = spi_driver_dev_mutex_by_index(index);
    assert(m);
    if (m)
        osMutexRelease(m);
}

