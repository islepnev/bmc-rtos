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
#define MAX_DHCP_TRIES  4
volatile uint8_t DHCP_state = DHCP_START;
#endif

#ifdef USE_DHCP

static int old_link_up = 0;
static u8_t old_dhcp_state = 0;
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

static uint8_t iptxt[20] = {0};

void log_dhcp_state_change(struct netif *netif)
{
    const struct dhcp *dhcp = (struct dhcp *)netif_get_client_data(netif, LWIP_NETIF_CLIENT_DATA_INDEX_DHCP);
    if (dhcp->state == old_dhcp_state)
        return;
    log_printf(LOG_INFO, "dhcp: %s", dhcp_state_str(dhcp->state));
    if (dhcp->state == DHCP_STATE_BOUND) {
        sprintf((char *)iptxt, "%s", ip4addr_ntoa((const ip4_addr_t *)&netif->ip_addr));
        log_printf(LOG_NOTICE, "dhcp: IP address assigned by a DHCP server: %s", iptxt);
        app_ipv4 = netif->ip_addr.addr;
    } else {
        app_ipv4 = 0;
    }
    old_dhcp_state = dhcp->state;
}

void DHCP_thread(void const * argument)
{
  struct netif *netif = (struct netif *) argument;
  ip_addr_t ipaddr;
  ip_addr_t netmask;
  ip_addr_t gw;
  struct dhcp *dhcp;

  for (;;)
  {
    const int link_up = netif_is_link_up(netif);
    if (link_up != old_link_up) {
        old_link_up = link_up;
        if (link_up) {
          dhcp_network_changed_link_up(netif);
        }
    }
    log_dhcp_state_change(netif);
    switch (DHCP_state)
    {
    case DHCP_START:
      {
        ip_addr_set_zero_ip4(&netif->ip_addr);
        ip_addr_set_zero_ip4(&netif->netmask);
        ip_addr_set_zero_ip4(&netif->gw);
        dhcp_start(netif);
        DHCP_state = DHCP_WAIT_ADDRESS;
        log_printf(LOG_INFO, "dhcp: Looking for DHCP server");
      }
      break;

    case DHCP_WAIT_ADDRESS:
      {
        if (dhcp_supplied_address(netif))
        {
          DHCP_state = DHCP_ADDRESS_ASSIGNED;

//          sprintf((char *)iptxt, "%s", ip4addr_ntoa((const ip4_addr_t *)&netif->ip_addr));
//          log_printf(LOG_NOTICE, "dhcp: IP address assigned by a DHCP server: %s", iptxt);
        }
        else
        {
          dhcp = (struct dhcp *)netif_get_client_data(netif, LWIP_NETIF_CLIENT_DATA_INDEX_DHCP);

          // DHCP timeout
          if (dhcp->tries > MAX_DHCP_TRIES)
          {
            DHCP_state = DHCP_TIMEOUT;

            // Stop DHCP
            dhcp_stop(netif);

            // Static address used
            IP_ADDR4(&ipaddr, IP_ADDR0 ,IP_ADDR1 , IP_ADDR2 , IP_ADDR3 );
            IP_ADDR4(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
            IP_ADDR4(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
            netif_set_addr(netif, ip_2_ip4(&ipaddr), ip_2_ip4(&netmask), ip_2_ip4(&gw));
            app_ipv4 = ipaddr.addr;

            sprintf((char *)iptxt, "%s", ip4addr_ntoa((const ip4_addr_t *)&netif->ip_addr));
            log_printf(LOG_WARNING, "dhcp: timeout");
            log_printf(LOG_WARNING, "Static IP address: %s", iptxt);
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
