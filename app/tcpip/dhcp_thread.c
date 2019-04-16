
#include "lwip/opt.h"
#include "main.h"
#include "lwip/dhcp.h"
#include "dhcp_thread.h"
#include "ethernetif.h"
#include "ipaddress.h"
#include "lwipcfg.h"
#include "logbuffer.h"

#ifdef USE_DHCP
#define MAX_DHCP_TRIES  4
__IO uint8_t DHCP_state = DHCP_OFF;
#endif

void User_notification(struct netif *netif)
{
  if (netif_is_up(netif))
  {
#ifdef USE_DHCP
    // Update DHCP state machine
    DHCP_state = DHCP_START;
#else
    uint8_t iptxt[20];
    sprintf((char *)iptxt, "%s", ip4addr_ntoa((const ip4_addr_t *)&netif->ip_addr));
    log_printf(LOG_INFO, "Static IP address: %s", iptxt);
#endif // USE_DHCP
  }
  else
  {
#ifdef USE_DHCP
    // Update DHCP state machine
    DHCP_state = DHCP_LINK_DOWN;
#endif  // USE_DHCP
    log_printf(LOG_WARNING, "The network cable is not connected");
  }
}

#ifdef USE_DHCP

void DHCP_thread(void const * argument)
{
  struct netif *netif = (struct netif *) argument;
  ip_addr_t ipaddr;
  ip_addr_t netmask;
  ip_addr_t gw;
  struct dhcp *dhcp;
  uint8_t iptxt[20];

  for (;;)
  {
    switch (DHCP_state)
    {
    case DHCP_START:
      {
        ip_addr_set_zero_ip4(&netif->ip_addr);
        ip_addr_set_zero_ip4(&netif->netmask);
        ip_addr_set_zero_ip4(&netif->gw);
        dhcp_start(netif);
        DHCP_state = DHCP_WAIT_ADDRESS;
        log_printf(LOG_INFO, "  State: Looking for DHCP server");
      }
      break;

    case DHCP_WAIT_ADDRESS:
      {
        if (dhcp_supplied_address(netif))
        {
          DHCP_state = DHCP_ADDRESS_ASSIGNED;

          sprintf((char *)iptxt, "%s", ip4addr_ntoa((const ip4_addr_t *)&netif->ip_addr));
          log_printf(LOG_NOTICE, "IP address assigned by a DHCP server: %s", iptxt);
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

            sprintf((char *)iptxt, "%s", ip4addr_ntoa((const ip4_addr_t *)&netif->ip_addr));
            log_printf(LOG_WARNING, "DHCP timeout");
            log_printf(LOG_WARNING, "Static IP address: %s", iptxt);
          }
        }
      }
      break;
  case DHCP_LINK_DOWN:
    {
      // Stop DHCP
      dhcp_stop(netif);
      DHCP_state = DHCP_OFF;
    }
    break;
    default: break;
    }

    // wait 250 ms
    osDelay(250);
  }
}
#endif // USE_DHCP
