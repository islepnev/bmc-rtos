/*
**    Copyright 2020-2021 Ilia Slepnev
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
**
**
*/

/*
**  See IEEE 802.1AB (Station and Media Access Control Connectivity Discovery)
**  See ANSI/TIA-1057 LLDP-MED (Media Endpoint Discovery)
*/

#include "lldp_message.h"
#include "lldp_tlv.h"

#ifdef NO_LWIP
#include <arpa/inet.h>
#else
#include "lwip/def.h"
#endif
#include <assert.h>
#include <string.h>

lldp_buf_t lldp_buf_set(void *payload, uint16_t size)
{
    lldp_buf_t buf;
    buf.buf = payload;
    buf.size = size;
    buf.len = 0;
    return buf;
}

static void lldp_message_add_buf(lldp_buf_t *pdu, const uint8_t *src, size_t n)
{
    assert(src);
    assert(pdu->len + n <= pdu->size);
    if (pdu->len + n > pdu->size)
        return;
    memcpy((char *)&pdu->buf[pdu->len], (const char *)src, n);
    pdu->len += n;
}

static void lldp_message_add_byte(lldp_buf_t *pdu, uint8_t data)
{
    lldp_message_add_buf(pdu, &data, sizeof(data));
}

static void lldp_message_add_word(lldp_buf_t *pdu, uint16_t data)
{
    uint16_t ndata = htons(data);
    lldp_message_add_buf(pdu, (const uint8_t *)&ndata, sizeof(ndata));
}

static void lldp_message_add_long(lldp_buf_t *pdu, uint32_t data)
{
    uint32_t ndata = htonl(data);
    lldp_message_add_buf(pdu, (const uint8_t *)&ndata, sizeof(ndata));
}

static void lldp_message_add_str(lldp_buf_t *pdu, const char *str)
{
    if (str)
        lldp_message_add_buf(pdu, (const uint8_t *)str, strlen(str));
}

static void lldp_message_add_tlv_header(lldp_buf_t *pdu, int tlv_type, int tlv_len)
{
    lldp_message_add_byte(pdu, tlv_type << 1);
    lldp_message_add_byte(pdu, tlv_len);
}

enum { ETH_ALEN = 6 };
enum { CHASSIS_ID_TLV_LEN = 1 + ETH_ALEN };
enum { CHASSIS_ID_TYPE_MAC = 4 }; // 802.1AB-2005, table 9-2
enum { PORT_ID_TLV_LEN = 1 + ETH_ALEN };
enum { PORT_ID_SUBTYPE_MAC = 3 }; // 802.1AB-2005, table 9-3
enum { TTL_TLV_LEN = 2 };
enum { SYSCAP_TLV_LEN = 4 };

static void lldp_message_add_tlv(lldp_buf_t *pdu, const lldp_info_t *info, enum LLDP_TLV_TYPE type)
{
    size_t len = 0;
    switch (type) {
    case LLDP_TLV_CHASSIS_ID:
        lldp_message_add_tlv_header(pdu, type, CHASSIS_ID_TLV_LEN);
        lldp_message_add_byte(pdu, CHASSIS_ID_TYPE_MAC);
        lldp_message_add_buf(pdu, info->mac, ETH_ALEN);
        break;
    case LLDP_TLV_PORT_ID:
        lldp_message_add_tlv_header(pdu, type, PORT_ID_TLV_LEN);
        lldp_message_add_byte(pdu, PORT_ID_SUBTYPE_MAC);
        lldp_message_add_buf(pdu, info->mac, ETH_ALEN);
        break;
    case LLDP_TLV_TTL:
        lldp_message_add_tlv_header(pdu, type, TTL_TLV_LEN);
        lldp_message_add_word(pdu, info->ttl);
        break;
    case LLDP_TLV_PORT_DESC:
        len = strlen(info->portdescr);
        lldp_message_add_tlv_header(pdu, type, len);
        lldp_message_add_str(pdu, info->portdescr);
        break;

    case LLDP_TLV_SYSTEM_NAME:
        len = strlen(info->sysname);
        lldp_message_add_tlv_header(pdu, type, len);
        lldp_message_add_str(pdu, info->sysname);
        break;
    case LLDP_TLV_SYSTEM_DESC:
        len = strlen(info->sysdescr);
        lldp_message_add_tlv_header(pdu, type, len);
        lldp_message_add_str(pdu, info->sysdescr);
        break;
    case LLDP_TLV_SYSTEM_CAPABILITIES:
        lldp_message_add_tlv_header(pdu, type, SYSCAP_TLV_LEN);
        lldp_message_add_word(pdu, info->syscaps);
        lldp_message_add_word(pdu, info->syscaps);
        break;
    case LLDP_TLV_MANAGEMENT_ADDR:
        len = 12;
        lldp_message_add_tlv_header(pdu, type, len);
        lldp_message_add_byte(pdu, 5); // address length
        lldp_message_add_byte(pdu, 1); // address subtype 1: IPv4
        lldp_message_add_long(pdu, htonl(info->ipv4));
        lldp_message_add_byte(pdu, 2); // interface number subtype 2: system port number
        lldp_message_add_long(pdu, 1); // port number
        lldp_message_add_byte(pdu, 0); // OID length
        break;
    case LLDP_TLV_END_LLDPDU:
        lldp_message_add_tlv_header(pdu, type, 0);
        break;
    default:
        return;
    }
}

enum LLDP_TLV_ORG_TYPE {
    LLDP_TLV_ORG_CAPS = 1,
    LLDP_TLV_ORG_HW_REV = 5,
    LLDP_TLV_ORG_FW_REV = 6,
    LLDP_TLV_ORG_SW_REV = 7,
    LLDP_TLV_ORG_SERIALNUM = 8,
    LLDP_TLV_ORG_MANUF = 9,
    LLDP_TLV_ORG_MODEL = 10
};

static const uint8_t oui_tia[3] = {0x00, 0x12, 0xbb};

static void lldp_message_add_org_tlv(lldp_buf_t *pdu, const lldp_info_t *info, enum LLDP_TLV_ORG_TYPE subtype)
{
    size_t len = 0;
    switch (subtype) {
    case LLDP_TLV_ORG_CAPS:
        len = 7;
        lldp_message_add_tlv_header(pdu, LLDP_TLV_ORG_SPECIFIC, len);
        lldp_message_add_buf(pdu, oui_tia, sizeof(oui_tia));
        lldp_message_add_byte(pdu, subtype);
        lldp_message_add_byte(pdu, 0);
        lldp_message_add_byte(pdu, 0x21); // Inventory
        lldp_message_add_byte(pdu, 1); // Class 1
        break;
    case LLDP_TLV_ORG_HW_REV:
        len = 4 + strlen(info->hw_rev);
        lldp_message_add_tlv_header(pdu, LLDP_TLV_ORG_SPECIFIC, len);
        lldp_message_add_buf(pdu, oui_tia, sizeof(oui_tia));
        lldp_message_add_byte(pdu, subtype);
        lldp_message_add_str(pdu, info->hw_rev);
        break;
    case LLDP_TLV_ORG_FW_REV:
        len = 4 + strlen(info->fw_rev);
        lldp_message_add_tlv_header(pdu, LLDP_TLV_ORG_SPECIFIC, len);
        lldp_message_add_buf(pdu, oui_tia, sizeof(oui_tia));
        lldp_message_add_byte(pdu, subtype);
        lldp_message_add_str(pdu, info->fw_rev);
        break;
    case LLDP_TLV_ORG_SW_REV:
        len = 4 + strlen(info->sw_rev);
        lldp_message_add_tlv_header(pdu, LLDP_TLV_ORG_SPECIFIC, len);
        lldp_message_add_buf(pdu, oui_tia, sizeof(oui_tia));
        lldp_message_add_byte(pdu, subtype);
        lldp_message_add_str(pdu, info->sw_rev);
        break;
    case LLDP_TLV_ORG_SERIALNUM:
        len = 4 + strlen(info->serial);
        lldp_message_add_tlv_header(pdu, LLDP_TLV_ORG_SPECIFIC, len);
        lldp_message_add_buf(pdu, oui_tia, sizeof(oui_tia));
        lldp_message_add_byte(pdu, subtype);
        lldp_message_add_str(pdu, info->serial);
        break;
    case LLDP_TLV_ORG_MANUF:
        len = 4 + strlen(info->manuf);
        lldp_message_add_tlv_header(pdu, LLDP_TLV_ORG_SPECIFIC, len);
        lldp_message_add_buf(pdu, oui_tia, sizeof(oui_tia));
        lldp_message_add_byte(pdu, subtype);
        lldp_message_add_str(pdu, info->manuf);
        break;
    case LLDP_TLV_ORG_MODEL:
        len = 4 + strlen(info->model);
        lldp_message_add_tlv_header(pdu, LLDP_TLV_ORG_SPECIFIC, len);
        lldp_message_add_buf(pdu, oui_tia, sizeof(oui_tia));
        lldp_message_add_byte(pdu, subtype);
        lldp_message_add_str(pdu, info->model);
        break;
    default:
        return;
    }
}

void lldp_message_update(lldp_buf_t *pdu, const lldp_info_t *info)
{
    pdu->len = 0;
    lldp_message_add_tlv(pdu, info, LLDP_TLV_CHASSIS_ID);
    lldp_message_add_tlv(pdu, info, LLDP_TLV_PORT_ID);
    lldp_message_add_tlv(pdu, info, LLDP_TLV_TTL);
    lldp_message_add_tlv(pdu, info, LLDP_TLV_PORT_DESC);
    lldp_message_add_tlv(pdu, info, LLDP_TLV_SYSTEM_NAME);
    lldp_message_add_tlv(pdu, info, LLDP_TLV_SYSTEM_DESC);
    lldp_message_add_tlv(pdu, info, LLDP_TLV_SYSTEM_CAPABILITIES);
    lldp_message_add_tlv(pdu, info, LLDP_TLV_MANAGEMENT_ADDR);
    lldp_message_add_org_tlv(pdu, info, LLDP_TLV_ORG_CAPS);
    lldp_message_add_org_tlv(pdu, info, LLDP_TLV_ORG_FW_REV);
    lldp_message_add_org_tlv(pdu, info, LLDP_TLV_ORG_HW_REV);
    lldp_message_add_org_tlv(pdu, info, LLDP_TLV_ORG_SW_REV);
    lldp_message_add_org_tlv(pdu, info, LLDP_TLV_ORG_SERIALNUM);
    lldp_message_add_org_tlv(pdu, info, LLDP_TLV_ORG_MANUF);
    lldp_message_add_org_tlv(pdu, info, LLDP_TLV_ORG_MODEL);
    lldp_message_add_tlv(pdu, info, LLDP_TLV_END_LLDPDU);
}

uint16_t lldp_create_pdu(void *payload, uint16_t size, const lldp_info_t *info)
{
    lldp_buf_t lldp_buf = lldp_buf_set(payload, size);
    lldp_message_update(&lldp_buf, info);
    return lldp_buf.len;
}
