/*
**    Copyright 2022 Ilia Slepnev
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

#ifndef FPGA_SPI_IOSTAT_H
#define FPGA_SPI_IOSTAT_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct BusIoStat {
    int bus_errors;
    int bus_timeouts;
    int hal_errors;
    int no_response_errors;
    int rx_addr_errors;
    int rx_count;
    int rx_crc_errors;
    int rx_len_errors;
    int rx_opcode_errors;
    int rx_timeouts;
    int tx_count;
    int tx_crc_errors;
} BusIoStat;

extern BusIoStat iostat;

int bus_iostat_comm_errors(const BusIoStat *iostat);
int bus_iostat_dev_errors(const BusIoStat *iostat);
int bus_iostat_total_errors(const BusIoStat *iostat);

#ifdef __cplusplus
}
#endif

#endif // FPGA_SPI_IOSTAT_H
