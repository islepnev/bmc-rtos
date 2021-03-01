
#include "lldp_mib.h"
#include "lwip/apps/snmp_scalar.h"
#include "lwip/apps/snmp_core.h"
#include "lwip/apps/snmp_table.h"
#include "snmp_lldpv2rem_table.h"

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

static const struct snmp_node* const lldpV2RemoteSystemsData_nodes[] = {
    &lldpV2RemTable.node.node
};

static const struct snmp_tree_node lldpV2RemoteSystemsData = SNMP_CREATE_TREE_NODE(4, lldpV2RemoteSystemsData_nodes);

static const struct snmp_node* const lldpV2Objects_nodes[] = {
    &lldpV2RemoteSystemsData.node
};

static const struct snmp_tree_node lldpV2Objects = SNMP_CREATE_TREE_NODE(1, lldpV2Objects_nodes);

static const struct snmp_node* const lldpV2MIB_nodes[] = {
    &lldpV2Objects.node
};

static const struct snmp_tree_node lldpV2MIB_node = SNMP_CREATE_TREE_NODE(1, lldpV2MIB_nodes);

static const u32_t lldpV2MIB_oid[] = { 1,3,111,2,802,1,1,13 };

const struct snmp_mib mib_lldpv2 = SNMP_MIB_CREATE(lldpV2MIB_oid, &lldpV2MIB_node.node);
