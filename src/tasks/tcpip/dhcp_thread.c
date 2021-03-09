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

#include "dhcp_thread.h"

#include "app_shared_data.h"
#include "ethernetif.h"
#include "ipaddress.h"
#include "log/log.h"
#include "lwip/dhcp.h"
#include "lwip/opt.h"
#include "lwip/prot/dhcp.h"
#include "lwipcfg.h"

#ifdef USE_DHCP
static const int MAX_DHCP_TRIES  = 4;
static uint8_t DHCP_state = DHCP_START;
#endif

#ifdef USE_DHCP

static int old_link_up = 0;
const char *dhcp_state_str(u8_t state)
{
    switch (state) {
    case DHCP_STATE_OFF: return "OFF";
    case DHCP_STATE_REQUESTING: return "REQUESTING";
    case DHCP_STATE_INIT: return "INIT";
    case DHCP_STATE_REBOOTING: return "REBOOTING";
    case DHCP_STATE_REBINDING: return "REBINDING";
    case DHCP_STATE_RENEWING: return "RENEWING";
    case DHCP_STATE_SELECTING: return "SELECTING";
    case DHCP_STATE_INFORMING: return "INFORMING";
    case DHCP_STATE_CHECKING: return "CHECKING";
    case DHCP_STATE_PERMANENT: return "PERMANENT";
    case DHCP_STATE_BOUND: return "BOUND";
    case DHCP_STATE_RELEASING: return "RELEASING";
    case DHCP_STATE_BACKING_OFF: return "BACKING_OFF";
    default:
        return "unknown";
    }
}

void app_notify_ip_address_change(const struct netif *netif)
{
    const int my_ipv4 = netif->ip_addr.addr;
    const struct dhcp *dhcp = (struct dhcp *)netif_get_client_data(netif, LWIP_NETIF_CLIENT_DATA_INDEX_DHCP);
    const int serv_ipv4 = dhcp ? dhcp->server_ip_addr.addr : 0;
    if (my_ipv4 != app_ipv4) {
        if (my_ipv4)
            log_printf(LOG_INFO, "%s: dhcp assigned IP address %d.%d.%d.%d by server %d.%d.%d.%d",
                       eth_port_descr,
                       my_ipv4 & 0xFF, (my_ipv4 >> 8) & 0xFF,
                       (my_ipv4 >> 16) & 0xFF, (my_ipv4 >> 24) & 0xFF,
                       serv_ipv4 & 0xFF, (serv_ipv4 >> 8) & 0xFF,
                       (serv_ipv4 >> 16) & 0xFF, (serv_ipv4 >> 24) & 0xFF);
        else
            log_printf(LOG_INFO, "%s: dhcp released IP address %d.%d.%d.%d",
                       eth_port_descr,
                       app_ipv4 & 0xFF, (app_ipv4 >> 8) & 0xFF,
                       (app_ipv4 >> 16) & 0xFF, (app_ipv4 >> 24) & 0xFF);

        app_ipv4 = my_ipv4;
    }
}

void check_dhcp_state_change(struct netif *netif)
{
    const struct dhcp *dhcp = (struct dhcp *)netif_get_client_data(netif, LWIP_NETIF_CLIENT_DATA_INDEX_DHCP);
    unsigned char dhcp_state = dhcp ? dhcp->state : DHCP_STATE_OFF;
    static u8_t old_dhcp_state = 0;
    if (dhcp_state != old_dhcp_state) {
        // log_printf(LOG_DEBUG, "%s: dhcp: state %s", ETH_PORT_NAME, dhcp_state_str(dhcp_state));
        app_notify_ip_address_change(netif);
        old_dhcp_state = dhcp_state;
    }
}

void DHCP_thread(void const * argument)
{
  struct netif *netif = (struct netif *) argument;
  ip_addr_t ipaddr;
  ip_addr_t netmask;
  ip_addr_t gw;

  for (;;)
  {
    const int link_up = netif_is_link_up(netif);
    if (link_up != old_link_up) {
        old_link_up = link_up;
        if (link_up) {
            dhcp_network_changed_link_up(netif);
        }
        app_notify_ip_address_change(netif);
    }
    check_dhcp_state_change(netif);
    switch (DHCP_state)
    {
    case DHCP_START:
      {
        ip_addr_set_zero_ip4(&netif->ip_addr);
        ip_addr_set_zero_ip4(&netif->netmask);
        ip_addr_set_zero_ip4(&netif->gw);
        dhcp_start(netif);
        DHCP_state = DHCP_WAIT_ADDRESS;
      }
      break;

    case DHCP_WAIT_ADDRESS:
      {
        if (dhcp_supplied_address(netif))
        {
          DHCP_state = DHCP_ADDRESS_ASSIGNED;
        }
        else
        {
            struct dhcp *dhcp = (struct dhcp *)netif_get_client_data(netif, LWIP_NETIF_CLIENT_DATA_INDEX_DHCP);

          // DHCP timeout
          if (dhcp && dhcp->tries > MAX_DHCP_TRIES)
          {
            DHCP_state = DHCP_TIMEOUT;

            if (false) { // do not use static IP
                // Stop DHCP
                dhcp_stop(netif);

                // Static address used
                IP_ADDR4(&ipaddr, IP_ADDR0 ,IP_ADDR1 , IP_ADDR2 , IP_ADDR3 );
                IP_ADDR4(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
                IP_ADDR4(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
                netif_set_addr(netif, ip_2_ip4(&ipaddr), ip_2_ip4(&netmask), ip_2_ip4(&gw));
                if (app_ipv4 != (int)ipaddr.addr) {
                    app_ipv4 = ipaddr.addr;
                    log_printf(LOG_NOTICE, "%s: dhcp timeout", eth_port_descr);
                    log_printf(LOG_INFO, "%s: using static IP address: %d.%d.%d.%d",
                               eth_port_descr,
                               app_ipv4 & 0xFF, (app_ipv4 >> 8) & 0xFF,
                               (app_ipv4 >> 16) & 0xFF, (app_ipv4 >> 24) & 0xFF);
                }
            }
          }
        }
      }
      break;
    default: break;
    }

    // wait 250 ms
    osDelay(250);
  }
}
#endif // USE_DHCP
