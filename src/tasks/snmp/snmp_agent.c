/*
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Dirk Ziegelmeier <dziegel@gmx.de>
 *
 */

#include "snmp_agent.h"

#include "lwip/netif.h"
#include "lwip/apps/snmp.h"
#include "lwip/apps/snmp_mib2.h"
#include "lwip/apps/snmpv3.h"
#include "lwip/apps/snmp_snmpv2_framework.h"
#include "lwip/apps/snmp_snmpv2_usm.h"
//#include "examples/snmp/snmp_v3/snmpv3_dummy.h"
#include "snmp_private_mib/private_mib.h"
#include "lldp_mib.h"
#include "lldpv2_mib.h"
#include "app_name.h"
#include "snmp_priv_oids.h"
#include "version.h"
#include "tcpip/app_task_tcpip_impl.h" // gnetif

#if LWIP_SNMP
static const struct snmp_mib *mibs[] = {
  &mib2,
  &mib_lldp,
  &mib_lldpv2,
  &mib_private
#if LWIP_SNMP_V3
  , &snmpframeworkmib
  , &snmpusmmib
#endif
};
#endif /* LWIP_SNMP */

#if defined(BOARD_TTVXS)
#define BOARD_OID SNMP_OID_aevTtvxs
#elif defined(BOARD_CRU16)
#define BOARD_OID SNMP_OID_aevCru16
#elif defined(BOARD_TDC64VLE)
#define BOARD_OID SNMP_OID_aevTdc64vle
#elif defined(BOARD_TQDC)
#define BOARD_OID SNMP_OID_aevTqdc16vs
#else
#error Unknown BOARD
#endif

#define MY_DEVICE_ENTERPRISE_OID {1, 3, 6, 1, 4, 1, \
    SNMP_OID_JINR, SNMP_OID_AFI, SNMP_OID_aevMIBObjects, SNMP_OID_aevBoard, BOARD_OID}
#define MY_DEVICE_ENTERPRISE_OID_LEN 11

static const struct snmp_obj_id  device_enterprise_oid = {
    MY_DEVICE_ENTERPRISE_OID_LEN, MY_DEVICE_ENTERPRISE_OID
};

void
snmp_agent_init(void)
{
#if LWIP_SNMP
  s32_t req_nr;
  lwip_privmib_init();
#if SNMP_LWIP_MIB2
#if SNMP_USE_NETCONN
  snmp_threadsync_init(&snmp_mib2_lwip_locks, snmp_mib2_lwip_synchronizer);
#endif /* SNMP_USE_NETCONN */
  snmp_mib2_set_syscontact_readonly((const u8_t*)"root", NULL);
  snmp_mib2_set_syslocation_readonly((const u8_t*)"unknown", NULL);
  snmp_mib2_set_sysname_readonly((const u8_t*)gnetif.hostname, NULL);
  snmp_mib2_set_sysdescr((const u8_t*)(APP_NAME_STR_BMC " v" VERSION_STR), NULL);
#endif /* SNMP_LWIP_MIB2 */

#if LWIP_SNMP_V3
  snmpv3_dummy_init();
#endif

  snmp_set_device_enterprise_oid(&device_enterprise_oid);
  snmp_set_mibs(mibs, LWIP_ARRAYSIZE(mibs));
  snmp_init();

  snmp_trap_dst_ip_set(0, &netif_default->gw);
  snmp_trap_dst_enable(0, 1);

  snmp_send_inform_generic(SNMP_GENTRAP_COLDSTART, NULL, &req_nr);
  snmp_send_trap_generic(SNMP_GENTRAP_COLDSTART);

#endif /* LWIP_SNMP */
}
