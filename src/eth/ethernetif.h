#ifndef __ETHERNETIF_H__
#define __ETHERNETIF_H__

#include <stdint.h>
#include "lwip/err.h"
#include "lwip/netif.h"
#include "cmsis_os.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Structure that include link thread parameters */

struct link_str {
    struct netif *netif;
    osSemaphoreId semaphore;
};

err_t ethernetif_init(struct netif *netif);
void ethernetif_set_link(void const *argument);
void ethernetif_update_config(struct netif *netif);
void ethernetif_notify_conn_changed(struct netif *netif);

#ifdef __cplusplus
}
#endif

#endif
