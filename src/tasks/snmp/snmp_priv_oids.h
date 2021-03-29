/*
**    Copyright 2021 Ilia Slepnev
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

#ifndef SNMP_PRIV_OIDS_H
#define SNMP_PRIV_OIDS_H

// http://www.iana.org/assignments/enterprise-numbers

// .iso.org.dod.internet.private.enterprises.jinr.afi

#define SNMP_OID_JINR 53776
#define SNMP_OID_AFI 120

#define SNMP_OID_bmc 1
#define SNMP_OID_aevMIBObjects 2

// AFI Entity Vendortype OIDs
#define SNMP_OID_aevBoard 1

#define SNMP_OID_aevTtvxs 1
#define SNMP_OID_aevCru16 2
#define SNMP_OID_aevTqdc16vs 3

#endif // SNMP_PRIV_OIDS_H
