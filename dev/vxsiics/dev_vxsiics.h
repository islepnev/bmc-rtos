/*
**    VXS IIC Slave
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

#ifndef DEV_VXSIICS_H
#define DEV_VXSIICS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct Dev_vxsiics;
void iic_write_callback(uint16_t addr, uint32_t data);
void iic_read_callback(uint16_t addr, uint32_t *data);
void dev_vxsiics_poll_status(struct Dev_vxsiics *d);

#ifdef __cplusplus
}
#endif

#endif // DEV_VXSIICS_H
