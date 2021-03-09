/*
**    Copyright 2020 Ilia Slepnev
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

#ifndef LLDP_MESSAGE_H
#define LLDP_MESSAGE_H

#include <stdint.h>

typedef struct lldp_buf_t {
    uint8_t *buf;
    uint16_t size;
    uint16_t len;
} lldp_buf_t;

typedef struct lldp_info_t {
    uint8_t mac[6];
    const char *portdescr;
    const char *sysname;
    const char *sysdescr;
    uint16_t ttl;
    uint32_t ipv4;
    uint16_t syscaps;
    // org specific
    const char *fw_rev;
    const char *hw_rev;
    const char *sw_rev;
    const char *serial;
    const char *manuf;
    const char *model;
} lldp_info_t;

uint16_t lldp_create_pdu(void *payload, uint16_t size, const lldp_info_t *info);

#endif // LLDP_MESSAGE_H
