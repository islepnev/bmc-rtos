#ifndef DHCP_THREAD_H
#define DHCP_THREAD_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "lwip/netif.h"
#include "lwipcfg.h"

#define DHCP_OFF                   (uint8_t) 0
#define DHCP_START                 (uint8_t) 1
#define DHCP_WAIT_ADDRESS          (uint8_t) 2
#define DHCP_ADDRESS_ASSIGNED      (uint8_t) 3
#define DHCP_TIMEOUT               (uint8_t) 4

#ifdef USE_DHCP
void DHCP_thread(void const * argument);
#endif

#ifdef __cplusplus
}
#endif

#endif /* DHCP_THREAD_H */

