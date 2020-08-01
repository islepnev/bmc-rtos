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

#include "lldp_handler.h"
#include <string.h>

#include "lwip/opt.h"
#include "lwip/pbuf.h"
#include "lwip/ip_addr.h"
#include "lwip/err.h"
#include "lwip/netif.h"
#include "lwip/prot/ethernet.h"

#include "log/log.h"
#include "lldp_neighbor.h"

enum {
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

static void decode_lldp_tlv(u8_t lldp_type, u16_t lldp_len, const u8_t *buf)
{
    switch (lldp_type) {
    case LLDP_TLV_END_LLDPDU:
        break;
    case LLDP_TLV_CHASSIS_ID:
        if (lldp_len != sizeof(lldp_chassis_tlv)) {
            log_printf(LOG_DEBUG, "LLDP TLV size mismatch, type %d, len %d", lldp_type, lldp_len);
            break;
        }
        memcpy(&lldp_neighbor.chassis, buf, sizeof(lldp_chassis_tlv));
        break;
    case LLDP_TLV_PORT_ID:
        break;
    case LLDP_TLV_TTL:
        break;
    case LLDP_TLV_PORT_DESC:
        break;
    case LLDP_TLV_SYSTEM_NAME:
        break;
    case LLDP_TLV_SYSTEM_DESC:
        break;
    case LLDP_TLV_SYSTEM_CAPABILITIES:
        break;
    case LLDP_TLV_MANAGEMENT_ADDR:
        break;
    case LLDP_TLV_ORG_SPECIFIC:
        break;
    default:
        log_printf(LOG_DEBUG, "LLDP unknown TLV, type %d, len %d", lldp_type, lldp_len);
        break;
    }
}

static void lldp_input(struct pbuf *p, struct netif *netif)
{
    LWIP_ERROR("netif != NULL", (netif != NULL), return;);
    u16_t i = 0;
    while (i < p->len - 1) {
        const u8_t *buf = (u8_t*)p->payload + i;
        u8_t lldp_type = buf[0] >> 1;
        u16_t lldp_len = ((buf[0] & 1)  << 8) + (buf[1]);
        i += 2;
        if (lldp_len == 0)
            continue;
        if (i + lldp_len > p->len) {
            log_printf(LOG_DEBUG, "LLDP PDU error at byte %d", i);
            break;
        }
        decode_lldp_tlv(lldp_type, lldp_len, &buf[2]);
        i += lldp_len;
    }
    pbuf_free(p);
}

int lwip_hook_unknown_eth_protocol(struct pbuf *p, struct netif *netif)
{

    u16_t next_hdr_offset = SIZEOF_ETH_HDR;
    struct eth_hdr *ethhdr = (struct eth_hdr *)p->payload;
    u16_t type = PP_HTONS(ethhdr->type);
    if (type != ETHTYPE_LLDP)
        goto free_and_return;

    /* skip Ethernet header (min. size checked above) */
    if (pbuf_remove_header(p, next_hdr_offset)) {
        goto free_and_return;
    }
    lldp_input(p, netif);
    return ERR_OK;

free_and_return:
    pbuf_free(p);
    return ERR_OK;
}
