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
#ifndef LLDP_NEIGHBOR_H
#define LLDP_NEIGHBOR_H

#include "lwip/arch.h"

#define NET_LLDP_CHASSIS_ID_VALUE_LEN (6)

struct net_lldp_chassis_tlv {
    u16_t type_length;	/* 7 bits for type, 9 bits for length */
    u8_t subtype;	/* ID subtype. */
    u8_t value[NET_LLDP_CHASSIS_ID_VALUE_LEN];
} __packed;

typedef struct net_lldp_chassis_tlv net_lldp_chassis_tlv;

typedef struct lldp_neighbor_t {
   net_lldp_chassis_tlv chassis;
} lldp_neighbor_t;

extern lldp_neighbor_t lldp_neighbor;

#endif // LLDP_NEIGHBOR_H
