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
#ifndef LLDP_HANDLER_H
#define LLDP_HANDLER_H

struct pbuf;
struct netif;

//extern err_t lwip_hook_unknown_eth_protocol(struct pbuf *pbuf, struct netif *netif);

#define LWIP_HOOK_UNKNOWN_ETH_PROTOCOL(pbuf, netif) lwip_hook_unknown_eth_protocol((pbuf), (netif))

#endif // LLDP_HANDLER_H
