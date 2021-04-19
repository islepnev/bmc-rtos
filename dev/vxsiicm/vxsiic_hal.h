/*
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
#ifndef VXSIIC_HAL_H
#define VXSIIC_HAL_H

#include <stdbool.h>
#include <stdint.h>

#include "bus/bus_types.h"

#ifdef __cplusplus
extern "C" {
#endif

void vxsiic_reset_mux(BusInterface *bus);
bool vxsiic_detect_mux(BusInterface *bus);
bool vxsiic_mux_select(BusInterface *bus, uint8_t subdevice, uint8_t channel, bool enable);
//bool vxsiic_get_pp_i2c_status(BusInterface *bus, uint8_t pp);
//bool vxsiic_detect_pp_eeprom(BusInterface *bus, uint8_t pp);
bool vxsiic_detect_pp_eeprom(BusInterface *bus, uint8_t pp);
bool vxsiic_detect_pp_ioexp(BusInterface *bus, uint8_t pp);
bool vxsiic_read_pp_eeprom(BusInterface *bus, uint8_t pp, uint16_t reg, uint8_t *data);
bool vxsiic_read_pp_ioexp(BusInterface *bus, uint8_t pp, uint8_t reg, uint8_t *data);
bool vxsiic_write_pp_ioexp(BusInterface *bus, uint8_t pp, uint8_t reg, uint8_t data);
bool vxsiic_read_pp_mcu_4(BusInterface *bus, uint8_t pp, uint16_t reg, uint32_t *data);
bool vxsiic_write_pp_mcu_4(BusInterface *bus, uint8_t pp, uint16_t reg, uint32_t data);

#ifdef __cplusplus
}
#endif

#endif // VXSIIC_HAL_H
