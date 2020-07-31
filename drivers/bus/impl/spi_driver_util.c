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

#include "spi.h"
#include "debug_helpers.h"
#include "cmsis_os.h"
#include "error_handler.h"
#include "log/logbuffer.h"

enum {SPI_BUS_COUNT = 5};

// interrupt wait semaphores
osSemaphoreId spi1_it_sem;
osSemaphoreId spi2_it_sem;
osSemaphoreId spi3_it_sem;
osSemaphoreId spi4_it_sem;
osSemaphoreId spi5_it_sem;

// device semaphores
osSemaphoreId spi1_dev_sem;
osSemaphoreId spi2_dev_sem;
osSemaphoreId spi3_dev_sem;
osSemaphoreId spi4_dev_sem;
osSemaphoreId spi5_dev_sem;

osSemaphoreDef(spi1_it_sem);
osSemaphoreDef(spi2_it_sem);
osSemaphoreDef(spi3_it_sem);
osSemaphoreDef(spi4_it_sem);
osSemaphoreDef(spi5_it_sem);

osSemaphoreDef(spi1_dev_sem);
osSemaphoreDef(spi2_dev_sem);
osSemaphoreDef(spi3_dev_sem);
osSemaphoreDef(spi4_dev_sem);
osSemaphoreDef(spi5_dev_sem);

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
    osSemaphoreWait(spi1_it_sem, osWaitForever);
    osSemaphoreWait(spi2_it_sem, osWaitForever);
    osSemaphoreWait(spi3_it_sem, osWaitForever);
    osSemaphoreWait(spi4_it_sem, osWaitForever);
    osSemaphoreWait(spi5_it_sem, osWaitForever);

    // create device semaphores
    spi1_dev_sem = osSemaphoreCreate(osSemaphore(spi1_dev_sem), 1);
    spi2_dev_sem = osSemaphoreCreate(osSemaphore(spi2_dev_sem), 1);
    spi3_dev_sem = osSemaphoreCreate(osSemaphore(spi3_dev_sem), 1);
    spi4_dev_sem = osSemaphoreCreate(osSemaphore(spi4_dev_sem), 1);
    spi5_dev_sem = osSemaphoreCreate(osSemaphore(spi5_dev_sem), 1);
    if (NULL == spi1_dev_sem
        || NULL == spi2_dev_sem
        || NULL == spi3_dev_sem
        || NULL == spi4_dev_sem
        || NULL == spi5_dev_sem
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
    if (hspi == &hspi4)
        return spi4_it_sem;
    if (hspi == &hspi5)
        return spi5_it_sem;
    assert(false);
    return NULL;
}

SemaphoreHandle_t spi_driver_dev_sem_by_index(int index)
{
    assert(index >=1 && index <= SPI_BUS_COUNT);
    switch (index) {
    case 1: return spi1_dev_sem;
    case 2: return spi2_dev_sem;
    case 3: return spi3_dev_sem;
    case 4: return spi4_dev_sem;
    case 5: return spi5_dev_sem;
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
    if (hspi == &hspi4)
        return 4;
    if (hspi == &hspi5)
        return 5;
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
    if (4 == index)
        return &hspi4;
    if (5 == index)
        return &hspi5;
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

int32_t spi_driver_wait_dev_sem(int index, uint32_t millisec)
{
    SemaphoreHandle_t sem = spi_driver_dev_sem_by_index(index);
    assert(sem);
    if (!sem)
        return osErrorValue;
    int32_t ret =  osSemaphoreWait(sem, millisec);
    if (ret)
        log_printf(LOG_WARNING, "spi%d device lock error\n", index);
    return  ret;
}

void spi_driver_release_dev_sem(int index)
{
    SemaphoreHandle_t sem = spi_driver_dev_sem_by_index(index);
    assert(sem);
    if (sem)
        osSemaphoreRelease(sem);
}

