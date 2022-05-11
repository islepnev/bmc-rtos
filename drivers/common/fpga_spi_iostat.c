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

#include "fpga_spi_iostat.h"

BusIoStat iostat = {0};

int bus_iostat_comm_errors(const BusIoStat *iostat)
{
    return
        iostat->hal_errors +
        iostat->no_response_errors +
        iostat->rx_addr_errors +
        iostat->rx_crc_errors +
        iostat->rx_len_errors +
        iostat->rx_opcode_errors +
        iostat->tx_crc_errors;
}

int bus_iostat_dev_errors(const BusIoStat *iostat)
{
    return
        iostat->bus_errors +
        iostat->bus_timeouts;
}

int bus_iostat_total_errors(const BusIoStat *iostat)
{
    return
        bus_iostat_comm_errors(iostat) +
        bus_iostat_dev_errors(iostat);
}
