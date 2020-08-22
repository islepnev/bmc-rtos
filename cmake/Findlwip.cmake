### lwIP

# can not include Filelists.cmake due to include_guard
#include(${LWIP_DIR}/src/Filelists.cmake)

# The minimum set of files needed for lwIP.
set(lwipcore_SRCS
    ${LWIP_DIR}/src/core/init.c
    ${LWIP_DIR}/src/core/def.c
    ${LWIP_DIR}/src/core/dns.c
    ${LWIP_DIR}/src/core/inet_chksum.c
    ${LWIP_DIR}/src/core/ip.c
    ${LWIP_DIR}/src/core/mem.c
    ${LWIP_DIR}/src/core/memp.c
    ${LWIP_DIR}/src/core/netif.c
    ${LWIP_DIR}/src/core/pbuf.c
    ${LWIP_DIR}/src/core/raw.c
    ${LWIP_DIR}/src/core/stats.c
    ${LWIP_DIR}/src/core/sys.c
    ${LWIP_DIR}/src/core/altcp.c
    ${LWIP_DIR}/src/core/altcp_alloc.c
    ${LWIP_DIR}/src/core/altcp_tcp.c
    ${LWIP_DIR}/src/core/tcp.c
    ${LWIP_DIR}/src/core/tcp_in.c
    ${LWIP_DIR}/src/core/tcp_out.c
    ${LWIP_DIR}/src/core/timeouts.c
    ${LWIP_DIR}/src/core/udp.c
)
set(lwipcore4_SRCS
    ${LWIP_DIR}/src/core/ipv4/acd.c
    ${LWIP_DIR}/src/core/ipv4/autoip.c
    ${LWIP_DIR}/src/core/ipv4/dhcp.c
    ${LWIP_DIR}/src/core/ipv4/etharp.c
    ${LWIP_DIR}/src/core/ipv4/icmp.c
    ${LWIP_DIR}/src/core/ipv4/igmp.c
    ${LWIP_DIR}/src/core/ipv4/ip4_frag.c
    ${LWIP_DIR}/src/core/ipv4/ip4.c
    ${LWIP_DIR}/src/core/ipv4/ip4_addr.c
)

# APIFILES: The files which implement the sequential and socket APIs.
set(lwipapi_SRCS
    ${LWIP_DIR}/src/api/api_lib.c
    ${LWIP_DIR}/src/api/api_msg.c
    ${LWIP_DIR}/src/api/err.c
    ${LWIP_DIR}/src/api/if_api.c
    ${LWIP_DIR}/src/api/netbuf.c
    ${LWIP_DIR}/src/api/netdb.c
    ${LWIP_DIR}/src/api/netifapi.c
    ${LWIP_DIR}/src/api/sockets.c
    ${LWIP_DIR}/src/api/tcpip.c
)

# Files implementing various generic network interface functions
set(lwipnetif_SRCS
    ${LWIP_DIR}/src/netif/ethernet.c
    ${LWIP_DIR}/src/netif/bridgeif.c
    ${LWIP_DIR}/src/netif/bridgeif_fdb.c
    ${LWIP_DIR}/src/netif/slipif.c
)

# SNMPv3 agent
set(lwipsnmp_SRCS
    ${LWIP_DIR}/src/apps/snmp/snmp_asn1.c
    ${LWIP_DIR}/src/apps/snmp/snmp_core.c
    ${LWIP_DIR}/src/apps/snmp/snmp_mib2.c
    ${LWIP_DIR}/src/apps/snmp/snmp_mib2_icmp.c
    ${LWIP_DIR}/src/apps/snmp/snmp_mib2_interfaces.c
    ${LWIP_DIR}/src/apps/snmp/snmp_mib2_ip.c
    ${LWIP_DIR}/src/apps/snmp/snmp_mib2_snmp.c
    ${LWIP_DIR}/src/apps/snmp/snmp_mib2_system.c
    ${LWIP_DIR}/src/apps/snmp/snmp_mib2_tcp.c
    ${LWIP_DIR}/src/apps/snmp/snmp_mib2_udp.c
    ${LWIP_DIR}/src/apps/snmp/snmp_snmpv2_framework.c
    ${LWIP_DIR}/src/apps/snmp/snmp_snmpv2_usm.c
    ${LWIP_DIR}/src/apps/snmp/snmp_msg.c
    ${LWIP_DIR}/src/apps/snmp/snmpv3.c
    ${LWIP_DIR}/src/apps/snmp/snmp_netconn.c
    ${LWIP_DIR}/src/apps/snmp/snmp_pbuf_stream.c
    ${LWIP_DIR}/src/apps/snmp/snmp_raw.c
    ${LWIP_DIR}/src/apps/snmp/snmp_scalar.c
    ${LWIP_DIR}/src/apps/snmp/snmp_table.c
    ${LWIP_DIR}/src/apps/snmp/snmp_threadsync.c
    ${LWIP_DIR}/src/apps/snmp/snmp_traps.c
)

# SNTP client
set(lwipsntp_SRCS
    ${LWIP_DIR}/src/apps/sntp/sntp.c
)

SET(lwip_SOURCES
   "${lwipcore_SRCS}"
   "${lwipcore4_SRCS}"
   "${lwipapi_SRCS}"
   "${lwipnetif_SRCS}"
   "${lwipsntp_SRCS}"
   "${lwipsnmp_SRCS}"
)

LIST(APPEND lwip_SOURCES
    ${MY_SRC_DIR}/system/OS/sys_arch.c
)

FIND_PATH(LWIP_INCLUDE_DIRS lwip/opt.h
    PATH_SUFFIXES include
    HINTS ${LWIP_DIR}/src/include
    NO_DEFAULT_PATH
    NO_CMAKE_FIND_ROOT_PATH
)

INCLUDE(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(lwip DEFAULT_MSG LWIP_INCLUDE_DIRS lwip_SOURCES)
