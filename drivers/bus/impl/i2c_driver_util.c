/*
**    Generic interrupt mode I2C driver
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

#include "i2c_driver_util.h"

#include <assert.h>

#include "cmsis_os.h"
#include "error_handler.h"
#include "i2c.h"
#include "i2c_hal_handles.h"
#include "log/log.h"

enum {I2C_BUS_COUNT = 4};

// interrupt wait semaphores
osSemaphoreId i2c1_it_sem;
osSemaphoreId i2c2_it_sem;
osSemaphoreId i2c3_it_sem;
osSemaphoreId i2c4_it_sem;

// device mutexes
osMutexId i2c1_dev_mutex;
osMutexId i2c2_dev_mutex;
osMutexId i2c3_dev_mutex;
osMutexId i2c4_dev_mutex;

osSemaphoreDef(i2c1_it_sem);
osSemaphoreDef(i2c2_it_sem);
osSemaphoreDef(i2c3_it_sem);
osSemaphoreDef(i2c4_it_sem);

osMutexDef(i2c1_dev_mutex);
osMutexDef(i2c2_dev_mutex);
osMutexDef(i2c3_dev_mutex);
osMutexDef(i2c4_dev_mutex);

// transfer error flags
static bool i2c_driver_transfer_error[I2C_BUS_COUNT] = {0};

void i2c_driver_clear_transfer_error(struct __I2C_HandleTypeDef *hi2c)
{
    int index = hi2c_index(hi2c);
    if (index == 0 || index > I2C_BUS_COUNT) return;
    i2c_driver_transfer_error[index-1] = 0;
}

void i2c_driver_raise_transfer_error(struct __I2C_HandleTypeDef *hi2c)
{
    int index = hi2c_index(hi2c);
    if (index == 0 || index > I2C_BUS_COUNT) return;
    i2c_driver_transfer_error[index-1] = true;
}

bool i2c_driver_is_transfer_ok(struct __I2C_HandleTypeDef *hi2c)
{
    int index = hi2c_index(hi2c);
    if (index == 0 || index > I2C_BUS_COUNT) return false;
    return 0 == i2c_driver_transfer_error[index-1];
}

bool i2c_driver_util_init(void)
{
    // Create and take the interrupt wait semaphore
    i2c1_it_sem = osSemaphoreCreate(osSemaphore(i2c1_it_sem), 1);
    i2c2_it_sem = osSemaphoreCreate(osSemaphore(i2c2_it_sem), 1);
    i2c3_it_sem = osSemaphoreCreate(osSemaphore(i2c3_it_sem), 1);
    i2c4_it_sem = osSemaphoreCreate(osSemaphore(i2c4_it_sem), 1);
    if (NULL == i2c1_it_sem
            || NULL == i2c2_it_sem
            || NULL == i2c3_it_sem
            || NULL == i2c4_it_sem
            ) {
        assert(false);
        return false;
    }
    if ((xSemaphoreTake(i2c1_it_sem, 0) != pdTRUE) ||
        (xSemaphoreTake(i2c2_it_sem, 0) != pdTRUE) ||
        (xSemaphoreTake(i2c3_it_sem, 0) != pdTRUE) ||
        (xSemaphoreTake(i2c4_it_sem, 0) != pdTRUE)
        ) {
        assert(false);
        return false;
    }

    // create device mutexes
    i2c1_dev_mutex = osMutexCreate(osMutex(i2c1_dev_mutex));
    i2c2_dev_mutex = osMutexCreate(osMutex(i2c2_dev_mutex));
    i2c3_dev_mutex = osMutexCreate(osMutex(i2c3_dev_mutex));
    i2c4_dev_mutex = osMutexCreate(osMutex(i2c4_dev_mutex));
    if (NULL == i2c1_dev_mutex
        || NULL == i2c2_dev_mutex
        || NULL == i2c3_dev_mutex
        || NULL == i2c4_dev_mutex
        ) {
        assert(false);
        return false;
    }
    return true;
}

SemaphoreHandle_t i2c_driver_it_sem_by_hi2c(struct __I2C_HandleTypeDef *hi2c)
{
    if (hi2c == &hi2c1)
        return i2c1_it_sem;
    if (hi2c == &hi2c2)
        return i2c2_it_sem;
    if (hi2c == &hi2c3)
        return i2c3_it_sem;
    if (hi2c == &hi2c4)
        return i2c4_it_sem;
    assert(false);
    return NULL;
}

osMutexId i2c_driver_dev_mutex_by_index(int index)
{
    assert(index >=1 && index <= I2C_BUS_COUNT);
    switch (index) {
    case 1: return i2c1_dev_mutex;
    case 2: return i2c2_dev_mutex;
    case 3: return i2c3_dev_mutex;
    case 4: return i2c4_dev_mutex;
    default: return NULL;
    }
    return NULL;
}

int hi2c_index(struct __I2C_HandleTypeDef *hi2c)
{
    if (hi2c == &hi2c1)
        return 1;
    if (hi2c == &hi2c2)
        return 2;
    if (hi2c == &hi2c3)
        return 3;
    if (hi2c == &hi2c4)
        return 4;
    assert(false);
    return 0;
}

struct __I2C_HandleTypeDef * hi2c_handle(BusIndex index)
{
    if (1 == index)
        return &hi2c1;
    if (2 == index)
        return &hi2c2;
    if (3 == index)
        return &hi2c3;
    if (4 == index)
        return &hi2c4;
    assert(false);
    return NULL;
}

int32_t i2c_driver_wait_it_sem(struct __I2C_HandleTypeDef *hi2c, uint32_t millisec)
{
    SemaphoreHandle_t sem = i2c_driver_it_sem_by_hi2c(hi2c);
    if (sem)
        return osSemaphoreWait(sem, millisec);
    else
        return osErrorValue;
}

void i2c_driver_release_it_sem(struct __I2C_HandleTypeDef *hi2c)
{
    SemaphoreHandle_t sem = i2c_driver_it_sem_by_hi2c(hi2c);
    if (sem)
        osSemaphoreRelease(sem);
}

int32_t i2c_driver_wait_dev_mutex(int index, uint32_t millisec)
{
    osMutexId m = i2c_driver_dev_mutex_by_index(index);
    assert(m);
    if (!m)
        return osErrorValue;
    int32_t ret =  osMutexWait(m, millisec);
    if (ret)
        log_printf(LOG_WARNING, "I2C%d device lock error\n", index);
    return  ret;
}

void i2c_driver_release_dev_mutex(int index)
{
    osMutexId m = i2c_driver_dev_mutex_by_index(index);
    assert(m);
    if (m)
        osMutexRelease(m);
}
