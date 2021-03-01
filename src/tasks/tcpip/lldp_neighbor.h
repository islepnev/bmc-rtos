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
#include "lwip/apps/snmp_opts.h"

#define LLDP_MAX_ID_LENGTH (SNMP_MAX_OCTET_STRING_LEN-1)
#define LLDP_MAX_STR_LENGTH (SNMP_MAX_OCTET_STRING_LEN-1)

struct lldp_id_t {
    u8_t subtype;
    char value[LLDP_MAX_ID_LENGTH]; // NOT null-terminated
    u8_t size;
};
typedef struct lldp_id_t lldp_id_t;

typedef char lldp_string_t[LLDP_MAX_STR_LENGTH+1]; // null-terminated

typedef struct lldp_neighbor_t {
    u16_t ttl; // 0 = delete lldp info
    lldp_id_t chassis;
    lldp_id_t port;
    lldp_string_t portdescr;
    lldp_string_t sysname;
    lldp_string_t sysdescr;
} lldp_neighbor_t;

extern u32_t lldp_neighbor_timestamp;
extern lldp_neighbor_t lldp_neighbor;

#endif // LLDP_NEIGHBOR_H
