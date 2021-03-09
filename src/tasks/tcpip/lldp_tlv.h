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

#ifndef LLDP_TLV_H
#define LLDP_TLV_H

enum LLDP_TLV_TYPE {
    LLDP_TLV_END_LLDPDU = 0,
    LLDP_TLV_CHASSIS_ID,
    LLDP_TLV_PORT_ID,
    LLDP_TLV_TTL,
    LLDP_TLV_PORT_DESC,
    LLDP_TLV_SYSTEM_NAME,
    LLDP_TLV_SYSTEM_DESC,
    LLDP_TLV_SYSTEM_CAPABILITIES,
    LLDP_TLV_MANAGEMENT_ADDR,
    /* Types 9 - 126 are reserved. */
    LLDP_TLV_ORG_SPECIFIC = 127
};

#endif // LLDP_TLV_H
