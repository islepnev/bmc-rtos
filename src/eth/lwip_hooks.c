/*
**    Copyright 2019-2020 Ilja Slepnev
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

#include "lwip_hooks.h"

#include "lwip/opt.h"
#include "lwip/netif.h"
#include "lwip/def.h"
#include "lwip/dhcp.h"
#include "lwip/prot/dhcp.h"

static u16_t
dhcp_option(u16_t options_out_len, u8_t *options, u8_t option_type, u8_t option_len)
{
    LWIP_ASSERT("dhcp_option: options_out_len + 2 + option_len <= DHCP_OPTIONS_LEN", options_out_len + 2U + option_len <= DHCP_OPTIONS_LEN);
    options[options_out_len++] = option_type;
    options[options_out_len++] = option_len;
    return options_out_len;
}

static u16_t
dhcp_option_byte(u16_t options_out_len, u8_t *options, u8_t value)
{
    LWIP_ASSERT("dhcp_option_byte: options_out_len < DHCP_OPTIONS_LEN", options_out_len < DHCP_OPTIONS_LEN);
    options[options_out_len++] = value;
    return options_out_len;
}

void lwip_dhcp_hook_append_options(struct netif *netif, struct dhcp *dhcp, uint8_t state, struct dhcp_msg *msg,
                                          uint8_t msg_type, uint16_t *options_len_ptr)
{
    *options_len_ptr = dhcp_option(*options_len_ptr, msg->options, DHCP_OPTION_CLIENT_ID, 7);
    *options_len_ptr = dhcp_option_byte(*options_len_ptr, msg->options, 0x01);
    for (int _i = 0; _i < 6; _i++) {
        *options_len_ptr = dhcp_option_byte(*options_len_ptr, msg->options, netif->hwaddr[_i]);
    }

    *options_len_ptr = dhcp_option(*options_len_ptr, msg->options, DHCP_OPTION_US, 3);
    *options_len_ptr = dhcp_option_byte(*options_len_ptr, msg->options, 'A');
    *options_len_ptr = dhcp_option_byte(*options_len_ptr, msg->options, 'F');
    *options_len_ptr = dhcp_option_byte(*options_len_ptr, msg->options, 'I');
}
