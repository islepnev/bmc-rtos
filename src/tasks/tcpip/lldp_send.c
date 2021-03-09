/*
**    Copyright 2019-2021 Ilja Slepnev
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

#include "lldp_send.h"

#include <string.h>

#include "lwip/opt.h"
#include "lwip/pbuf.h"
#include "lwip/err.h"
#include "lwip/netif.h"

#include "log/log.h"
#include "lldp_message.h"
#include "netif/ethernet.h"
#include "lwip/timeouts.h"
#include "app_name.h"
#include "app_shared_data.h"
#include "version.h"

#define MAX_LLDP_LEN 250
#define LLDP_SEND_PERIOD 5

const struct eth_addr ethaddr = {{0x01, 0x80, 0xc2, 0x00, 0x00, 0x00}};

static void lldp_send_callback(void *arg) {
    struct netif *netif = (struct netif *) arg;
    LWIP_ERROR("netif != NULL", (netif != NULL), return;);

    struct pbuf *p;
    p = pbuf_alloc(PBUF_LINK, MAX_LLDP_LEN, PBUF_RAM);
    if (p == NULL) {
        LWIP_DEBUGF(LWIP_DBG_TRACE | LWIP_DBG_LEVEL_SERIOUS,
                    ("lldp_send: could not allocate pbuf\n"));
        return;
    }
    lldp_info_t info = {
        .portdescr = eth_port_descr,
        .sysname = netif->hostname,
        .sysdescr = APP_DESCR_STR,
        .ttl = LLDP_SEND_PERIOD * 4,
        .ipv4 = app_ipv4,
        .syscaps = 0x0080,
        // org specific
        .fw_rev = "",
        .hw_rev = "",
        .sw_rev = VERSION_STR,
        .serial = "",
        .manuf = VENDOR_STR,
        .model = APP_NAME_STR_BMC,
    };
    memcpy(&info.mac, netif->hwaddr, 6);
    lldp_create_pdu(p->payload, p->len, &info);

//    u8_t *raw = (u8_t *)p->payload;
//    for (int i=0; i<p->len; i++)
//        log_printf(LOG_INFO, "%2d %02X ", i, raw[i]);

    ethernet_output(netif, p, (struct eth_addr *)(netif->hwaddr), &ethaddr, ETHTYPE_LLDP);

    pbuf_free(p);
    p = NULL;
    sys_timeout((u32_t)LLDP_SEND_PERIOD * 1000, lldp_send_callback, netif);
}

void lldp_send(struct netif *netif)
{
    lldp_send_callback(netif);
}
