/*
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

#include "display_log.h"

#include <stdio.h>

#include "ansi_escape_codes.h"
#include "app_shared_data.h"
#include "bsp.h"
#include "devicelist.h"
#include "display.h"
#include "log/log.h"
#include "fpga/dev_fpga_types.h"
#include "fpga/dev_fpga_print.h"
#include "fpga_spi_iostat.h"

static void display_fpga_spi_stats()
{
    const DeviceBase *d = find_device_const(DEV_CLASS_FPGA);
    if (!d || !d->priv)
        return;
    // printf("SPI stats\n");
    int mcu_rx_errors = iostat.rx_crc_errors + iostat.rx_timeouts +
                        iostat.no_response_errors + iostat.rx_addr_errors +
                        iostat.rx_opcode_errors + iostat.rx_other_errors;
    int fpga_errors = iostat.tx_crc_errors + iostat.bus_errors + iostat.bus_timeouts;
    int errors = bus_iostat_total_errors(&iostat);
    printf("MCU packets   %-10d FPGA packets %-10d ", iostat.tx_count, iostat.rx_count);
    if (errors == 0) {
        printf("(no errors)\n");
    } else {
        printf("Errors %-10d\n", errors);
        if (mcu_rx_errors) {
            printf("MCU Rx status\n");
            if (iostat.hal_errors)
                printf("   HAL error   %-10d\n", iostat.hal_errors);
            printf("   Bad CRC     %-10d Op timeout  %-10d No reply    %-10d\n",
                   iostat.rx_crc_errors, iostat.rx_timeouts, iostat.no_response_errors);
            printf("   Bad address %-10d Bad opcode  %-10d Other error %-10d\n",
                   iostat.rx_addr_errors, iostat.rx_opcode_errors, iostat.rx_other_errors);
        }
        if (fpga_errors) {
            printf("FPGA status\n");
            printf("   Bad CRC     %-10d Bus error   %-10d Bus timeout %-10d\n",
                   iostat.tx_crc_errors, iostat.bus_errors, iostat.bus_timeouts);
        }
    }
}

static void display_fpga_info()
{
    const DeviceBase *d = find_device_const(DEV_CLASS_FPGA);
    if (!d || !d->priv)
        return;
}

void display_fpga_page(int y, bool repaint)
{
    print_goto(y, 1);
    const DeviceBase *d = find_device_const(DEV_CLASS_FPGA);
    if (!d || !d->priv) {
        printf("No FPGA device\n");
        return;
    }
    const Dev_fpga_priv *priv = (const Dev_fpga_priv *)device_priv_const(d);
//    const Dev_fpga_gpio *gpio = &priv->gpio;
//    const Dev_fpga_runtime *fpga = &priv->fpga;
    dev_fpga_print_comm_state(priv);
    printf("\n");
    printf("\n");
    decode_fpga_info(priv);
    // int cur_y = y + 2;
    display_fpga_spi_stats();
    printf("\n");
    display_fpga_info();
}
