/*
**    Copyright 2020 Ilja Slepnev
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

#ifndef BUS_TYPES_H
#define BUS_TYPES_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct BusIoStat {
    uint32_t bus_errors;
    uint32_t bus_timeouts;
    uint32_t hal_errors;
    uint32_t no_response_errors;
    uint32_t rx_count;
    uint32_t rx_crc_errors;
    uint32_t tx_count;
    uint32_t tx_crc_errors;
} BusIoStat;

typedef enum BusType {
    BUS_NONE,
    BUS_SPI,
    BUS_IIC
} BusType;

typedef int BusIndex;

typedef struct BusInterface {
    BusType type;
    int bus_number;
    BusIndex address;
    struct DeviceBase *dev;
    BusIoStat iostat;
} BusInterface;

extern const BusInterface null_bus_info;

uint32_t bus_iostat_comm_errors(const BusIoStat *iostat);
uint32_t bus_iostat_dev_errors(const BusIoStat *iostat);
uint32_t bus_iostat_total_errors(const BusIoStat *iostat);

#ifdef __cplusplus
}
#endif

#endif // BUS_TYPES_H
