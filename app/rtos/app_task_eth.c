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

#include "app_task_eth.h"

#ifdef WITH_APP_TASK_ETH

#include <stdint.h>
#include "cmsis_os.h"
#include "lwip/tcpip.h"
#include "dev_leds.h"
#include "app_tasks.h"

#include "ethernetif.h"
#include "lwip/netif.h"
#include "lwip/tcpip.h"
#include "ipaddress.h"

struct netif gnetif; /* network interface structure */
/* Semaphore to signal Ethernet Link state update */
osSemaphoreId Netif_LinkSemaphore = NULL;
/* Ethernet link thread Argument */
struct link_str link_arg;

enum { ethThreadStackSize = threadStackSize };
/*
static void task_eth_rtos(void)
{
    led_toggle(LED_GREEN);
}

void tcpecho_init(void)
{
    sys_thread_new("tcpecho_thread", tcpecho_thread, NULL, DEFAULT_THREAD_STACKSIZE, TCPECHO_THREAD_PRIO);
}

static void tcpecho_thread(void *arg)
{ // Create a new connection identifier.
  conn = netconn_new(NETCONN_TCP);
  if (conn != NULL) { // Bind connection to well known port number 7.
    err = netconn_bind(conn, NULL, 7);
    if (err == ERR_OK) { // Tell connection to go into listening mode.
      netconn_listen(conn);
      while (1) { // Grab new connection.
        accept_err =
            netconn_accept(conn, &newconn); // Process the new connection.
        if (accept_err == ERR_OK) {
          while ((recv_err = netconn_recv(newconn, &buf)) == ERR_OK) {
            do {
              netbuf_data(buf, &data, &len);
              netconn_write(newconn, data, len, NETCONN_COPY);
            } while (netbuf_next(buf) >= 0);
            netbuf_delete(buf);
          } // Close connection and discard connection identifier.
          netconn_close(newconn);
          netconn_delete(newconn);
        }
      }
    } else {
      netconn_delete(newconn);
    }
  }
}

static void ethThreadTask(void const *arg) {
  (void)arg;

  tcpip_init(NULL, NULL);
  netif_config();
  tcpecho_init();
  for (;;) {
    osDelay(100);
  }
}
*/

static void Netif_Config(void)
{
  ip_addr_t ipaddr;
  ip_addr_t netmask;
  ip_addr_t gw;

#ifdef USE_DHCP
  ip_addr_set_zero_ip4(&ipaddr);
  ip_addr_set_zero_ip4(&netmask);
  ip_addr_set_zero_ip4(&gw);
#else
  IP_ADDR4(&ipaddr,IP_ADDR0,IP_ADDR1,IP_ADDR2,IP_ADDR3);
  IP_ADDR4(&netmask,NETMASK_ADDR0,NETMASK_ADDR1,NETMASK_ADDR2,NETMASK_ADDR3);
  IP_ADDR4(&gw,GW_ADDR0,GW_ADDR1,GW_ADDR2,GW_ADDR3);
#endif /* USE_DHCP */

  /* add the network interface */
  netif_add(&gnetif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &tcpip_input);

  /*  Registers the default network interface */
  netif_set_default(&gnetif);

  if (netif_is_link_up(&gnetif))
  {
    /* When the netif is fully configured this function must be called */
    netif_set_up(&gnetif);
  }
  else
  {
    /* When the netif link is down this function must be called */
    netif_set_down(&gnetif);
  }

  /* Set the link callback function, this function is called on change of link status*/
  netif_set_link_callback(&gnetif, ethernetif_update_config);
  /* create a binary semaphore used for informing ethernetif of frame reception */

  osSemaphoreDef(Netif_SEM);
  Netif_LinkSemaphore = osSemaphoreCreate(osSemaphore(Netif_SEM) , 1 );

  link_arg.netif = &gnetif;
  link_arg.semaphore = Netif_LinkSemaphore;
  /* Create the Ethernet link handler thread */
  osThreadDef(LinkThr, ethernetif_set_link, osPriorityNormal, 0, configMINIMAL_STACK_SIZE * 2);
  osThreadCreate (osThread(LinkThr), &link_arg);
}


/**
  * @brief  Start Thread
  * @param  argument not used
  * @retval None
  */
static void StartThread(void const * argument)
{
  /* Create tcp_ip stack thread */
  tcpip_init(NULL, NULL);

  /* Initialize the LwIP stack */
  Netif_Config();

  /* Initialize webserver demo */
//  http_server_netconn_init();

  /* Notify user about the network interface config */
//  User_notification(&gnetif);

//#ifdef USE_DHCP
//  /* Start DHCPClient */
//  osThreadDef(DHCP, DHCP_thread, osPriorityBelowNormal, 0, configMINIMAL_STACK_SIZE * 2);
//  osThreadCreate (osThread(DHCP), &gnetif);
//#endif

  for( ;; )
  {
    /* Delete the Init Thread */
    osThreadTerminate(NULL);
  }
}

void create_task_eth(void)
{
    osThreadDef(Start, StartThread, osPriorityNormal, 0, configMINIMAL_STACK_SIZE * 2);

    osThreadCreate (osThread(Start), NULL);
}
#else // not WITH_APP_TASK_ETH
void create_task_eth(void) {
    start_app_httpd();
}
#endif // WITH_APP_TASK_ETH
