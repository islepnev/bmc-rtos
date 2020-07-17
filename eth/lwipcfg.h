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

#ifndef LWIPCFG_H
#define LWIPCFG_H

/** Define this to the index of the windows network adapter to use */
#define PACKET_LIB_ADAPTER_NR         1
/** Define this to the GUID of the windows network adapter to use
 * or NOT define this if you want PACKET_LIB_ADAPTER_NR to be used */
/*#define PACKET_LIB_ADAPTER_GUID       "00000000-0000-0000-0000-000000000000"*/
/*#define PACKET_LIB_GET_ADAPTER_NETADDRESS(addr) IP4_ADDR((addr), 192,168,1,0)*/
/*#define PACKET_LIB_QUIET*/

/* #define USE_PCAPIF 1 */
#define LWIP_PORT_INIT_IPADDR(addr)   IP4_ADDR((addr), 192,168,1,200)
#define LWIP_PORT_INIT_GW(addr)       IP4_ADDR((addr), 192,168,1,1)
#define LWIP_PORT_INIT_NETMASK(addr)  IP4_ADDR((addr), 255,255,255,0)

/* remember to change this MAC address to suit your needs!
   the last octet will be increased by netif->num for each netif */
#define LWIP_MAC_ADDR_BASE            {0x00,0x01,0x02,0x03,0x04,0x05}

/* configuration for applications */

#define LWIP_CHARGEN_APP              1
#define LWIP_DNS_APP                  1
#define LWIP_HTTPD_APP                LWIP_TCP
/* Set this to 1 to use the netconn http server,
 * otherwise the raw api server will be used. */
/*#define LWIP_HTTPD_APP_NETCONN     */
#define LWIP_NETBIOS_APP              LWIP_IPV4 && LWIP_UDP
#define LWIP_NETIO_APP                1
#define LWIP_MDNS_APP                 LWIP_UDP
#define LWIP_MQTT_APP                 LWIP_TCP
#define LWIP_PING_APP                 1
#define LWIP_RTP_APP                  1
#define LWIP_SHELL_APP                LWIP_TCP
#define LWIP_SNMP_APP                 LWIP_UDP
#define LWIP_SNTP_APP                 LWIP_UDP
#define LWIP_SOCKET_EXAMPLES_APP      1
#define LWIP_TCPECHO_APP              LWIP_TCP
/* Set this to 1 to use the netconn tcpecho server,
 * otherwise the raw api server will be used. */
/*#define LWIP_TCPECHO_APP_NETCONN   */
#define LWIP_TFTP_APP                 LWIP_UDP
#define LWIP_TFTP_CLIENT_APP          LWIP_UDP
#define LWIP_UDPECHO_APP              LWIP_UDP
#define LWIP_LWIPERF_APP              LWIP_TCP

#define USE_DHCP                      LWIP_DHCP
#define USE_AUTOIP                    LWIP_AUTOIP

/* define this to your custom application-init function */
/* #define LWIP_APP_INIT my_app_init() */

#endif // LWIPCFG_H
