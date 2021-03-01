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

#include "snmp_lldprem_table.h"

#include <string.h>
#include <stdio.h>

#include "lwip/apps/snmp_table.h"
#include "lwip/apps/snmp_scalar.h"
#include "tcpip/lldp_neighbor.h"

enum { SFP_COUNT = 1 };

enum { SFP_MAX = SFP_COUNT };

static snmp_err_t table_get_cell_instance(const u32_t* column, const u32_t* row_oid, u8_t row_oid_len, struct snmp_node_instance* cell_instance);
static snmp_err_t table_get_next_cell_instance(const u32_t* column, struct snmp_obj_id* row_oid, struct snmp_node_instance* cell_instance);
static s16_t      table_get_value(struct snmp_node_instance* instance, void* value);
static snmp_err_t table_set_value(struct snmp_node_instance* instance, u16_t len, void *value);

static const struct snmp_table_col_def table_columns[] = {
{ 2, SNMP_ASN1_TYPE_INTEGER, SNMP_NODE_INSTANCE_READ_ONLY  },
{ 3, SNMP_ASN1_TYPE_INTEGER, SNMP_NODE_INSTANCE_READ_ONLY  },
{ 4, SNMP_ASN1_TYPE_INTEGER, SNMP_NODE_INSTANCE_READ_ONLY  },
{ 5, SNMP_ASN1_TYPE_OCTET_STRING, SNMP_NODE_INSTANCE_READ_ONLY  },
{ 6, SNMP_ASN1_TYPE_INTEGER, SNMP_NODE_INSTANCE_READ_ONLY  },
{ 7, SNMP_ASN1_TYPE_OCTET_STRING, SNMP_NODE_INSTANCE_READ_ONLY  },
{ 8, SNMP_ASN1_TYPE_OCTET_STRING, SNMP_NODE_INSTANCE_READ_ONLY  },
{ 9, SNMP_ASN1_TYPE_OCTET_STRING, SNMP_NODE_INSTANCE_READ_ONLY  },
{ 10, SNMP_ASN1_TYPE_OCTET_STRING, SNMP_NODE_INSTANCE_READ_ONLY  },
};

const struct snmp_table_node lldpRemTable = SNMP_TABLE_CREATE(
        1, table_columns,
        table_get_cell_instance, table_get_next_cell_instance,
        table_get_value, snmp_set_test_ok, table_set_value);

/* sfptable .1.3.6.1.4.1.26381.1.1 */
/* list of allowed value ranges for incoming OID */
static const struct snmp_oid_range table_oid_ranges[] = {
{ 1, SFP_MAX+1 }
};

static snmp_err_t
table_get_cell_instance(const u32_t* column, const u32_t* row_oid, u8_t row_oid_len, struct snmp_node_instance* cell_instance)
{
    //  size_t i;

    LWIP_UNUSED_ARG(column);

    /* check if incoming OID length and if values are in plausible range */
    if(!snmp_oid_in_range(row_oid, row_oid_len, table_oid_ranges, LWIP_ARRAYSIZE(table_oid_ranges))) {
        return SNMP_ERR_NOSUCHINSTANCE;
    }

    /* get index from incoming OID */
    u32_t num = row_oid[0];
    if (num > 0 && num <= SFP_COUNT) {
        cell_instance->reference.u32 = (u32_t)(num-1);
        return SNMP_ERR_NOERROR;
    }


    /* not found */
    return SNMP_ERR_NOSUCHINSTANCE;
}

static snmp_err_t
table_get_next_cell_instance(const u32_t* column, struct snmp_obj_id* row_oid, struct snmp_node_instance* cell_instance)
{
    size_t i;
    struct snmp_next_oid_state state;
    u32_t result_temp[LWIP_ARRAYSIZE(table_oid_ranges)];

    LWIP_UNUSED_ARG(column);

    /* init struct to search next oid */
    snmp_next_oid_init(&state, row_oid->id, row_oid->len, result_temp, LWIP_ARRAYSIZE(table_oid_ranges));

    /* iterate over all possible OIDs to find the next one */
    for (i=0; i<SFP_COUNT; i++) {
        u32_t test_oid[LWIP_ARRAYSIZE(table_oid_ranges)];

        test_oid[0] = i+1; // sfps[i].num;

        /* check generated OID: is it a candidate for the next one? */
        snmp_next_oid_check(&state, test_oid, LWIP_ARRAYSIZE(table_oid_ranges), (void*)i);
    }

    /* did we find a next one? */
    if(state.status == SNMP_NEXT_OID_STATUS_SUCCESS) {
        snmp_oid_assign(row_oid, state.next_oid, state.next_oid_len);
        /* store index for subsequent operations (get/test/set) */
        cell_instance->reference.u32 = LWIP_CONST_CAST(u32_t, state.reference);
        return SNMP_ERR_NOERROR;
    }

    /* not found */
    return SNMP_ERR_NOSUCHINSTANCE;
}

static s16_t
table_get_value(struct snmp_node_instance* instance, void* value)
{
    u32_t i = instance->reference.u32;
    if (i >= SFP_COUNT)
        return 0;
    switch (SNMP_TABLE_GET_COLUMN_FROM_OID(instance->instance_oid.id))
    {
    case 2: { // lldpRemLocalIfIndex InterfaceIndex
        *(s32_t *)value = 1;
        return sizeof(s32_t);
    }
    case 3: { // lldpRemIndex Unsigned32
        *(s32_t *)value = 1;
        return sizeof(s32_t);
    }
    case 4: { // lldpRemChassisIdSubtype
        *(s32_t *)value = lldp_neighbor.chassis.subtype;
        return sizeof(s32_t);
    }
    case 5: { // lldpRemChassisId
        size_t len = lldp_neighbor.chassis.size;
        MEMCPY(value, lldp_neighbor.chassis.value, len);
        return (s16_t)len;
    }
    case 6: { // lldpRemPortIdSubtype
        *(s32_t *)value = lldp_neighbor.port.subtype;
        return sizeof(s32_t);
    }
    case 7: { // lldpRemPortId
        size_t len = lldp_neighbor.port.size;
        MEMCPY(value, lldp_neighbor.port.value, len);
        return (s16_t)len;
    }
    case 8: { // lldpRemPortDesc
        size_t len = strlen(lldp_neighbor.portdescr);
        MEMCPY(value, lldp_neighbor.portdescr, len);
        return (s16_t)len;
    }
    case 9: { // lldpRemSysName
        size_t len = strlen(lldp_neighbor.sysname);
        MEMCPY(value, lldp_neighbor.sysname, len);
        return (s16_t)len;
    }
    case 10: { // lldpRemSysDesc
        size_t len = strlen(lldp_neighbor.sysdescr);
        MEMCPY(value, lldp_neighbor.sysdescr, len);
        return (s16_t)len;
    }
    default:
        return 0;
    }
}

static snmp_err_t
table_set_value(struct snmp_node_instance* instance, u16_t len, void *value)
{
    u32_t i = instance->reference.u32;

    LWIP_UNUSED_ARG(len);
    return SNMP_ERR_NOTWRITABLE;
    return SNMP_ERR_NOERROR;
}
