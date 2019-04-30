#ifndef __ETHERNETIF_H__
#define __ETHERNETIF_H__

#include <stdint.h>
#include "lwip/err.h"
#include "lwip/netif.h"
#include "cmsis_os.h"

void get_mac_address(uint8_t buf[6]);
err_t ethernetif_init(struct netif *netif);
#endif
