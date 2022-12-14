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

#ifndef DEV_FPGA_PRINT_H
#define DEV_FPGA_PRINT_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct Dev_fpga_priv;

extern char fpga_sdb_commit_id[16+1];
//extern char fpga_version_str[16+1];
extern char fpga_product_name[19+1];
extern char fpga_ow_serial_str[14+1];

void dev_fpga_print_box(void);
bool onewire_id_valid(uint64_t ow_id);
void dev_fpga_print_comm_state(const struct Dev_fpga_priv *priv);
void decode_fpga_info(const struct Dev_fpga_priv *priv);

#ifdef __cplusplus
}
#endif

#endif // DEV_FPGA_PRINT_H
