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
#include "snmp_powermon_table.h"

#include "private_mib.h"

#if LWIP_SNMP

#include <string.h>
#include <stdio.h>

#include "lwip/apps/snmp_table.h"
#include "lwip/apps/snmp_scalar.h"
#include "app_shared_data.h"
#include "dev_common_types.h"
#include "devices_types.h"
#include "dev_pm_sensors_config.h"
#include "powermon/dev_pm_sensors_types.h"

#define SENSOR_COUNT POWERMON_SENSORS

#define SENSOR_MAX      SENSOR_COUNT

static snmp_err_t sensor_table_get_cell_instance(const u32_t* column, const u32_t* row_oid, u8_t row_oid_len, struct snmp_node_instance* cell_instance);
static snmp_err_t sensor_table_get_next_cell_instance(const u32_t* column, struct snmp_obj_id* row_oid, struct snmp_node_instance* cell_instance);
static s16_t      sensor_table_get_value(struct snmp_node_instance* instance, void* value);
static snmp_err_t sensor_table_set_value(struct snmp_node_instance* instance, u16_t len, void *value);

static const struct snmp_table_col_def sensor_table_columns[] = {
{ 1, SNMP_ASN1_TYPE_INTEGER,      SNMP_NODE_INSTANCE_READ_ONLY  },
{ 2, SNMP_ASN1_TYPE_INTEGER,      SNMP_NODE_INSTANCE_READ_ONLY  },
{ 3, SNMP_ASN1_TYPE_OCTET_STRING, SNMP_NODE_INSTANCE_READ_ONLY  },
{ 4, SNMP_ASN1_TYPE_INTEGER,      SNMP_NODE_INSTANCE_READ_ONLY  },
{ 5, SNMP_ASN1_TYPE_INTEGER,      SNMP_NODE_INSTANCE_READ_ONLY  },
{ 6, SNMP_ASN1_TYPE_INTEGER,      SNMP_NODE_INSTANCE_READ_ONLY  },
{ 7, SNMP_ASN1_TYPE_INTEGER,      SNMP_NODE_INSTANCE_READ_ONLY  },
{ 8, SNMP_ASN1_TYPE_INTEGER,      SNMP_NODE_INSTANCE_READ_ONLY  },
{ 9, SNMP_ASN1_TYPE_INTEGER,      SNMP_NODE_INSTANCE_READ_ONLY  },
{ 10, SNMP_ASN1_TYPE_INTEGER,      SNMP_NODE_INSTANCE_READ_ONLY  }
};

/* sensortable .1.3.6.1.4.1.26381.1.1 */
const struct snmp_table_node sensor_table = SNMP_TABLE_CREATE(
        1, sensor_table_columns,
        sensor_table_get_cell_instance, sensor_table_get_next_cell_instance,
        sensor_table_get_value, snmp_set_test_ok, sensor_table_set_value);

/* sensortable .1.3.6.1.4.1.26381.1.1 */
/* list of allowed value ranges for incoming OID */
static const struct snmp_oid_range sensor_table_oid_ranges[] = {
{ 1, SENSOR_MAX+1 }
};

static snmp_err_t
sensor_table_get_cell_instance(const u32_t* column, const u32_t* row_oid, u8_t row_oid_len, struct snmp_node_instance* cell_instance)
{
    //  size_t i;

    LWIP_UNUSED_ARG(column);

    /* check if incoming OID length and if values are in plausible range */
    if(!snmp_oid_in_range(row_oid, row_oid_len, sensor_table_oid_ranges, LWIP_ARRAYSIZE(sensor_table_oid_ranges))) {
        return SNMP_ERR_NOSUCHINSTANCE;
    }

    /* get sensor index from incoming OID */
    u32_t sensor_num = row_oid[0];
    if (sensor_num > 0 && sensor_num <= POWERMON_SENSORS) {
        cell_instance->reference.u32 = (u32_t)(sensor_num-1);
        return SNMP_ERR_NOERROR;
    }


    /* not found */
    return SNMP_ERR_NOSUCHINSTANCE;
}

static snmp_err_t
sensor_table_get_next_cell_instance(const u32_t* column, struct snmp_obj_id* row_oid, struct snmp_node_instance* cell_instance)
{
    size_t i;
    struct snmp_next_oid_state state;
    u32_t result_temp[LWIP_ARRAYSIZE(sensor_table_oid_ranges)];

    LWIP_UNUSED_ARG(column);

    /* init struct to search next oid */
    snmp_next_oid_init(&state, row_oid->id, row_oid->len, result_temp, LWIP_ARRAYSIZE(sensor_table_oid_ranges));

    /* iterate over all possible OIDs to find the next one */
    for (i=0; i<POWERMON_SENSORS; i++) {
        u32_t test_oid[LWIP_ARRAYSIZE(sensor_table_oid_ranges)];

        test_oid[0] = i+1; // sensors[i].num;

        /* check generated OID: is it a candidate for the next one? */
        snmp_next_oid_check(&state, test_oid, LWIP_ARRAYSIZE(sensor_table_oid_ranges), (void*)i);
    }

    /* did we find a next one? */
    if(state.status == SNMP_NEXT_OID_STATUS_SUCCESS) {
        snmp_oid_assign(row_oid, state.next_oid, state.next_oid_len);
        /* store sensor index for subsequent operations (get/test/set) */
        cell_instance->reference.u32 = LWIP_CONST_CAST(u32_t, state.reference);
        return SNMP_ERR_NOERROR;
    }

    /* not found */
    return SNMP_ERR_NOSUCHINSTANCE;
}

static s16_t
sensor_table_get_value(struct snmp_node_instance* instance, void* value)
{
    u32_t i = instance->reference.u32;
    if (i >= POWERMON_SENSORS)
        return 0;
    const Devices *dev = getDevicesConst();
    const pm_sensor *s = &dev->pm.sensors[i];
    switch (SNMP_TABLE_GET_COLUMN_FROM_OID(instance->instance_oid.id))
    {
    case 1: {/* sensor index */
        *(s32_t *)value = i+1;
        return sizeof(s32_t);
    }
    case 2: {/* sensor I2C bus address */
        *(s32_t *)value = s->bus.address;
        return sizeof(s32_t);
    }
    case 3: {/* sensor name */
        const char *name = monLabel((SensorIndex)i);
        size_t len = strlen(name);
        MEMCPY(value, name, len);
        return (s16_t)len;
    }
    case 4: {/* sensor optional */
//        *(s32_t *)value = s->isOptional ? 1 : 2;
//        return sizeof(s32_t);
        return snmp_encode_truthvalue((s32_t *)value, s->isOptional);

    }
    case 5: {/* sensor state */
        *(s32_t *)value = pm_sensor_status(s);
        return sizeof(s32_t);
    }
    case 6: {/* nominal voltage */
        *(s32_t *)value = s->busNomVoltage * 1000;
        return sizeof(s32_t);
    }
    case 7: {/* voltage sensor value */
        *(s32_t *)value = s->busVoltage * 1000;
        return sizeof(s32_t);
    }
    case 8: {/* current sensor value */
        return snmp_encode_truthvalue((s32_t *)value, s->hasShunt);
    }
    case 9: {/* current sensor value */
        *(s32_t *)value = s->current * 1000;
        return sizeof(s32_t);
    }
    case 10: {/* peak current sensor value */
        *(s32_t *)value = s->currentMax * 1000;
        return sizeof(s32_t);
    }
    default:
        return 0;
    }
}

static snmp_err_t
sensor_table_set_value(struct snmp_node_instance* instance, u16_t len, void *value)
{
    u32_t i = instance->reference.u32;

    LWIP_UNUSED_ARG(len);
    return SNMP_ERR_NOTWRITABLE;
    return SNMP_ERR_NOERROR;
}

#endif /* LWIP_SNMP */
