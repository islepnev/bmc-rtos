
#include "lldp_mib.h"
#include "lwip/apps/snmp_scalar.h"
#include "lwip/apps/snmp_core.h"
#include "lwip/apps/snmp_table.h"
#include "snmp_lldprem_table.h"

// test only

static s16_t dummy_get_value(struct snmp_node_instance* instance, void* value)
{
    LWIP_UNUSED_ARG(instance);
    u32_t *uint_ptr = (u32_t*)value;
    *uint_ptr = (u32_t)12345;
    return sizeof(*uint_ptr);
}

//static const struct snmp_scalar_node lldpV2RemoteSystemsData = SNMP_SCALAR_CREATE_NODE_READONLY(
//        4, SNMP_ASN1_TYPE_INTEGER, dummy_get_value);

static const struct snmp_node* const lldpRemoteSystemsData_nodes[] = {
    &lldpRemTable.node.node
};

static const struct snmp_tree_node lldpRemoteSystemsData = SNMP_CREATE_TREE_NODE(4, lldpRemoteSystemsData_nodes);

static const struct snmp_node* const lldpObjects_nodes[] = {
    &lldpRemoteSystemsData.node
};

static const struct snmp_tree_node lldpObjects = SNMP_CREATE_TREE_NODE(1, lldpObjects_nodes);

static const struct snmp_node* const lldpMIB_nodes[] = {
    &lldpObjects.node
};

static const struct snmp_tree_node lldpMIB_node = SNMP_CREATE_TREE_NODE(1, lldpMIB_nodes);

static const u32_t lldpMIB_oid[] = { 1, 0, 8802, 1, 1, 2 };

const struct snmp_mib mib_lldp = SNMP_MIB_CREATE(lldpMIB_oid, &lldpMIB_node.node);
